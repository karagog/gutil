/*Copyright 2012 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include <QtCore/QString>
#include <QtTest/QtTest>
#include "gutil_console.h"
#include "gutil_cards.h"
#include "gutil_strings.h"
#include "gutil_set.h"
USING_NAMESPACE_GUTIL1(Games);
USING_NAMESPACE_GUTIL1(DataAccess);
USING_NAMESPACE_GUTIL1(DataObjects);

class CardsTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    /** Tests that the cards fundamentally behave like we expect. */
    void test_card_basics();

    /** Tests that the deck fundamentally behaves like we expect. */
    void test_deck_basics();

    /** Tests that the hands fundamentally behave like we expect. */
    void test_hand_basics();


    /** Tests that the deck creates a valid 52-card deck,
        by comparing it to one we create in this sample program.
    */
    void test_deck_Standard52Card();

    /** Tests that the deck creates a valid 52-card deck, plus 2 Jokers,
        by comparing it to one we create in this sample program.
    */
    void test_deck_Standard52CardWithJokers();

    /** Tests that the deck creates a valid Euchre deck,
        by comparing it to one we create in this sample program.
    */
    void test_deck_Euchre();

    /** Tests the infinite deck implementation. */
    void test_deck_Inifinite();


private:

    static void print_hand(const Hand &);

    static bool decks_equal(const Deck &, const Deck &);
    static bool hands_equal(const Hand &, const Hand &);

    static Set<Card> generate_standard_52_cards();
    static Set<Card> generate_standard_52_cards_with_jokers();
    static Set<Card> generate_euchre();

    static bool compare_cards(const Deck &d, const Set<Card> &);

    static int arbitrary_card_compare(const Card &lhs, const Card &rhs);

};


void CardsTest::test_card_basics()
{
    // Initialize some cards for testing
    Card c1( Card::Queen, Card::Spades );
    Card c2( Card::Five, Card::Diamonds );

    // Check the comparison operators
    QVERIFY( c1 == Card(Card::Queen, Card::Spades) );
    QVERIFY( c1 != Card(Card::King, Card::Spades) );
    QVERIFY( c1 != Card(Card::King, Card::Hearts) );
    QVERIFY( c1 != c2 );


    // Check the accessor functions
    QVERIFY( c1.Value() == Card::Queen );
    QVERIFY( c1.Suit() == Card::Spades );
    QVERIFY( c2.Value() == Card::Five );
    QVERIFY( c2.Suit() == Card::Diamonds );


    // Check the "ToString()" function for every suit and every value
    QVERIFY(0 == strcmp("Clubs", Card(Card::InvalidValue, Card::Clubs).SuitString()));
    QVERIFY(0 == strcmp("Spades", Card(Card::InvalidValue, Card::Spades).SuitString()));
    QVERIFY(0 == strcmp("Hearts", Card(Card::InvalidValue, Card::Hearts).SuitString()));
    QVERIFY(0 == strcmp("Diamonds", Card(Card::InvalidValue, Card::Diamonds).SuitString()));
    QVERIFY(0 == strcmp("Wild", Card(Card::InvalidValue, Card::Wild).SuitString()));

    QVERIFY(0 == strcmp("Ace", Card(Card::Ace, Card::Clubs).ValueString()));
    QVERIFY(0 == strcmp("Two", Card(Card::Two, Card::Clubs).ValueString()));
    QVERIFY(0 == strcmp("Three", Card(Card::Three, Card::Clubs).ValueString()));
    QVERIFY(0 == strcmp("Four", Card(Card::Four, Card::Clubs).ValueString()));
    QVERIFY(0 == strcmp("Five", Card(Card::Five, Card::Clubs).ValueString()));
    QVERIFY(0 == strcmp("Six", Card(Card::Six, Card::Clubs).ValueString()));
    QVERIFY(0 == strcmp("Seven", Card(Card::Seven, Card::Clubs).ValueString()));
    QVERIFY(0 == strcmp("Eight", Card(Card::Eight, Card::Clubs).ValueString()));
    QVERIFY(0 == strcmp("Nine", Card(Card::Nine, Card::Clubs).ValueString()));
    QVERIFY(0 == strcmp("Ten", Card(Card::Ten, Card::Clubs).ValueString()));
    QVERIFY(0 == strcmp("Jack", Card(Card::Jack, Card::Clubs).ValueString()));
    QVERIFY(0 == strcmp("Queen", Card(Card::Queen, Card::Clubs).ValueString()));
    QVERIFY(0 == strcmp("King", Card(Card::King, Card::Clubs).ValueString()));
    QVERIFY(0 == strcmp("Joker", Card(Card::Joker, Card::Clubs).ValueString()));
}

void CardsTest::test_deck_basics()
{
    Deck d1(0), d2(0);
    d2.SetNumberHands(4);
    d1.Initialize(Deck::Init_Standard52Card);
    d2.Initialize(Deck::Init_Standard52Card);

    QVERIFY( d1.CardCount() == 52 );
    QVERIFY( decks_equal(d1, d2) );

    // Test Shuffling
    d2.Shuffle();

    // Shuffling has no effect on the number of cards
    QVERIFY( d2.CardCount() == 52 );

    // After shuffling there is practically 0% chance that the decks are equal
    QVERIFY( !decks_equal(d1, d2) );


    // Now test dealing and collecting
    d2.Deal();

    // Dealing the whole deck results in a card count of 0
    QVERIFY( 0 == d2.CardCount() );

    // Each hand has 13 cards
    QVERIFY( d2.Hands()[0].CardCount() == 13 );
    QVERIFY( d2.Hands()[1].CardCount() == 13 );
    QVERIFY( d2.Hands()[2].CardCount() == 13 );
    QVERIFY( d2.Hands()[3].CardCount() == 13 );


    d2.Collect();

    // Collecting the hands returns all the cards to the deck
    QVERIFY( 52 == d2.CardCount() );

    // After collecting, none of the hands have any cards
    QVERIFY( d2.Hands()[0].CardCount() == 0 );
    QVERIFY( d2.Hands()[1].CardCount() == 0 );
    QVERIFY( d2.Hands()[2].CardCount() == 0 );
    QVERIFY( d2.Hands()[3].CardCount() == 0 );


    // And deal again to make sure we can deal multiple times
    d2.Deal();
    QVERIFY( 0 == d2.CardCount() );
    QVERIFY( d2.Hands()[0].CardCount() == 13 );
    QVERIFY( d2.Hands()[1].CardCount() == 13 );
    QVERIFY( d2.Hands()[2].CardCount() == 13 );
    QVERIFY( d2.Hands()[3].CardCount() == 13 );

    // In case you're curious, you can display the hands
//    G_FOREACH_CONST(Hand &h, d2.Hands()){
//        h.Sort();
//        print_hand(h);
//    }
}

void CardsTest::test_hand_basics()
{
    Deck d(4);
    d.Initialize();

    d.Deal();

    Hand &h( d.Hands()[0] );
    Hand h_cpy( d.Hands()[0] );
    QVERIFY( hands_equal(h, h_cpy) );

    // Sorting the hand in the deck does not affect the copy
    h.Sort();
    QVERIFY( !hands_equal(h, h_cpy) );
    QVERIFY( h.CardCount() == h_cpy.CardCount() );


    // Collecting the hands renders the reference empty, but leaves the copy full of cards
    d.Collect();
    QVERIFY( 0 == h.CardCount() );
    QVERIFY( 13 == h_cpy.CardCount() );


    // When you deal again, the hand reference sees the new cards
    Hand h_cpy_cpy( h_cpy );
    d.Shuffle();
    d.Deal();
    QVERIFY( 13 == h.CardCount() );
    QVERIFY( 13 == h_cpy.CardCount() );
    QVERIFY( hands_equal(h_cpy, h_cpy_cpy) );
    QVERIFY( !hands_equal(h, h_cpy) );
}


void CardsTest::test_deck_Standard52Card()
{
    Deck d(4);
    d.Initialize(Deck::Init_Standard52Card);

    Set<Card> all_cards( generate_standard_52_cards() );
    QVERIFY(compare_cards(d, all_cards));
    
    // Make sure the deck is still correct after shuffling
    d.Shuffle();
    QVERIFY(compare_cards(d, all_cards));
}

void CardsTest::test_deck_Standard52CardWithJokers()
{
    Deck d(4);
    d.Initialize(Deck::Init_Standard52CardWithJokers);

    Set<Card> all_cards( generate_standard_52_cards_with_jokers() );
    QVERIFY(compare_cards(d, all_cards));
    
    // Make sure the deck is still correct after shuffling
    d.Shuffle();
    QVERIFY(compare_cards(d, all_cards));
}

void CardsTest::test_deck_Euchre()
{
    Deck d(4);
    d.Initialize(Deck::Init_Euchre);

    Set<Card> all_cards( generate_euchre() );
    QVERIFY(compare_cards(d, all_cards));
    
    // Make sure the deck is still correct after shuffling
    d.Shuffle();
    QVERIFY(compare_cards(d, all_cards));
}

void CardsTest::test_deck_Inifinite()
{
    InfiniteDeck d(4);
    d.Initialize(Deck::Init_Standard52Card);

    // Even though the deck is infinite, it still has a finite number of
    //  cards.  It's just that the cards aren't removed from the deck when
    //  they're dealt, just pointers to the cards are distributed.
    QVERIFY(52 == d.CardCount());


    // Deal a bunch of cards to the 4 hands
    const int num_cards(52);
    d.Deal(0, num_cards);

    // Display the hands if you want to
//    d.Hands()[0].Sort();
//    print_hand(d.Hands()[0]);

    // Dealing should not affect the card count of the infinite deck
    QVERIFY(52 == d.CardCount());

    // Collecting the hands has no effect on the card count of the infinite deck
    d.Collect();
    QVERIFY(52 == d.CardCount());

    // Indirectly testing the destructor:  How does it handle the case when
    //  there are cards remaining in the hands when destructing?  This test
    //  fails if you get a segmentation fault or leaked memory during execution.
    d.Deal(0, 20);
}



void CardsTest::print_hand(const Hand &h)
{
    G_FOREACH_CONST(Card *c, h.Cards())
        Console::WriteLine( String::Format("%s of %s", c->ValueString(), c->SuitString()) );

    Console::WriteLine();
}

bool CardsTest::decks_equal(const Deck &d1, const Deck &d2)
{
    bool ret( d1.CardCount() == d2.CardCount() );

    if(ret)
        for(int i = 0; ret && i < d1.Cards().Count(); ++i)
            ret = *d1.Cards()[i] == *d2.Cards()[i];

    return ret;
}

bool CardsTest::hands_equal(const Hand &h1, const Hand &h2)
{
    bool ret( h1.CardCount() == h2.CardCount() );

    if(ret)
        for(int i = 0; ret && i < h1.CardCount(); ++i)
            ret = *h1.Cards()[i] == *h2.Cards()[i];

    return ret;
}

Set<Card> CardsTest::generate_standard_52_cards()
{
    Set<Card> ret(&arbitrary_card_compare);

    // Generate the deck manually, so it is a different generation
    //  algorithm than is used by the deck itself
    ret.Insert( Card(Card::Ace, Card::Clubs) );
    ret.Insert( Card(Card::Two, Card::Clubs) );
    ret.Insert( Card(Card::Three, Card::Clubs) );
    ret.Insert( Card(Card::Four, Card::Clubs) );
    ret.Insert( Card(Card::Five, Card::Clubs) );
    ret.Insert( Card(Card::Six, Card::Clubs) );
    ret.Insert( Card(Card::Seven, Card::Clubs) );
    ret.Insert( Card(Card::Eight, Card::Clubs) );
    ret.Insert( Card(Card::Nine, Card::Clubs) );
    ret.Insert( Card(Card::Ten, Card::Clubs) );
    ret.Insert( Card(Card::Jack, Card::Clubs) );
    ret.Insert( Card(Card::Queen, Card::Clubs) );
    ret.Insert( Card(Card::King, Card::Clubs) );

    ret.Insert( Card(Card::Ace, Card::Diamonds) );
    ret.Insert( Card(Card::Two, Card::Diamonds) );
    ret.Insert( Card(Card::Three, Card::Diamonds) );
    ret.Insert( Card(Card::Four, Card::Diamonds) );
    ret.Insert( Card(Card::Five, Card::Diamonds) );
    ret.Insert( Card(Card::Six, Card::Diamonds) );
    ret.Insert( Card(Card::Seven, Card::Diamonds) );
    ret.Insert( Card(Card::Eight, Card::Diamonds) );
    ret.Insert( Card(Card::Nine, Card::Diamonds) );
    ret.Insert( Card(Card::Ten, Card::Diamonds) );
    ret.Insert( Card(Card::Jack, Card::Diamonds) );
    ret.Insert( Card(Card::Queen, Card::Diamonds) );
    ret.Insert( Card(Card::King, Card::Diamonds) );

    ret.Insert( Card(Card::Ace, Card::Hearts) );
    ret.Insert( Card(Card::Two, Card::Hearts) );
    ret.Insert( Card(Card::Three, Card::Hearts) );
    ret.Insert( Card(Card::Four, Card::Hearts) );
    ret.Insert( Card(Card::Five, Card::Hearts) );
    ret.Insert( Card(Card::Six, Card::Hearts) );
    ret.Insert( Card(Card::Seven, Card::Hearts) );
    ret.Insert( Card(Card::Eight, Card::Hearts) );
    ret.Insert( Card(Card::Nine, Card::Hearts) );
    ret.Insert( Card(Card::Ten, Card::Hearts) );
    ret.Insert( Card(Card::Jack, Card::Hearts) );
    ret.Insert( Card(Card::Queen, Card::Hearts) );
    ret.Insert( Card(Card::King, Card::Hearts) );

    ret.Insert( Card(Card::Ace, Card::Spades) );
    ret.Insert( Card(Card::Two, Card::Spades) );
    ret.Insert( Card(Card::Three, Card::Spades) );
    ret.Insert( Card(Card::Four, Card::Spades) );
    ret.Insert( Card(Card::Five, Card::Spades) );
    ret.Insert( Card(Card::Six, Card::Spades) );
    ret.Insert( Card(Card::Seven, Card::Spades) );
    ret.Insert( Card(Card::Eight, Card::Spades) );
    ret.Insert( Card(Card::Nine, Card::Spades) );
    ret.Insert( Card(Card::Ten, Card::Spades) );
    ret.Insert( Card(Card::Jack, Card::Spades) );
    ret.Insert( Card(Card::Queen, Card::Spades) );
    ret.Insert( Card(Card::King, Card::Spades) );

    return ret;
}

Set<Card> CardsTest::generate_standard_52_cards_with_jokers()
{
    Set<Card> ret( generate_standard_52_cards() );
    ret.InsertMulti( Card(Card::Joker, Card::Wild) );
    ret.InsertMulti( Card(Card::Joker, Card::Wild) );
    return ret;
}

Set<Card> CardsTest::generate_euchre()
{
    Set<Card> ret(&arbitrary_card_compare);

    // Generate the deck manually, so it is a different generation
    //  algorithm than is used by the deck itself
    ret.Insert( Card(Card::Nine, Card::Clubs) );
    ret.Insert( Card(Card::Ten, Card::Clubs) );
    ret.Insert( Card(Card::Jack, Card::Clubs) );
    ret.Insert( Card(Card::Queen, Card::Clubs) );
    ret.Insert( Card(Card::King, Card::Clubs) );
    ret.Insert( Card(Card::Ace, Card::Clubs) );

    ret.Insert( Card(Card::Nine, Card::Diamonds) );
    ret.Insert( Card(Card::Ten, Card::Diamonds) );
    ret.Insert( Card(Card::Jack, Card::Diamonds) );
    ret.Insert( Card(Card::Queen, Card::Diamonds) );
    ret.Insert( Card(Card::King, Card::Diamonds) );
    ret.Insert( Card(Card::Ace, Card::Diamonds) );

    ret.Insert( Card(Card::Nine, Card::Hearts) );
    ret.Insert( Card(Card::Ten, Card::Hearts) );
    ret.Insert( Card(Card::Jack, Card::Hearts) );
    ret.Insert( Card(Card::Queen, Card::Hearts) );
    ret.Insert( Card(Card::King, Card::Hearts) );
    ret.Insert( Card(Card::Ace, Card::Hearts) );

    ret.Insert( Card(Card::Nine, Card::Spades) );
    ret.Insert( Card(Card::Ten, Card::Spades) );
    ret.Insert( Card(Card::Jack, Card::Spades) );
    ret.Insert( Card(Card::Queen, Card::Spades) );
    ret.Insert( Card(Card::King, Card::Spades) );
    ret.Insert( Card(Card::Ace, Card::Spades) );

    return ret;
}



bool CardsTest::compare_cards(const Deck &d, const Set<Card> &s)
{
    Set<Card> s_cpy( s );

    G_FOREACH_CONST(const Card * const &c, d.Cards())
    {
        if(s_cpy.Contains(*c))
        {
            s_cpy.RemoveOne(*c);
        }
        else
        {
            qDebug("Found an extra card that was not expected:  %s of %s", c->ValueString(), c->SuitString());
            return false;
        }
    }

    if(0 != s_cpy.Size())
    {
        G_FOREACH_CONST(const Card &c, s_cpy)
            qDebug("Did not find the expected card:  %s of %s", c.ValueString(), c.SuitString());
        return false;
    }
    else
    {
        return true;
    }
}

int CardsTest::arbitrary_card_compare(const Card &lhs, const Card &rhs)
{
    if((int)lhs.Value() < (int)rhs.Value())
        return -1;
    else if((int)lhs.Value() > (int)rhs.Value())
        return 1;

    if((int)lhs.Suit() < (int)rhs.Suit())
        return -1;
    else if((int)lhs.Suit() > (int)rhs.Suit())
        return 1;

    return 0;
}


QTEST_APPLESS_MAIN(CardsTest)

#include "tst_cardstest.moc"
