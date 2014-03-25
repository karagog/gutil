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

/** \file

    Declares several classes useful for representing the data and
    logic of a card game.

    You find the Card class, which has a value and a suit.  A Deck
    is a list of cards, which is very flexible and can be modified
    easily to facilitate any game.

    The Deck contains logical "Hands" to which cards are dealt.  You
    can distribute references to these hands to the card players,
    and theirs will receive the cards when you call Deal() on the Deck.
*/


#ifndef GUTIL_CARDS_H
#define GUTIL_CARDS_H

#include "gutil_flexibletypecomparer.h"
#include "gutil_vector.h"

NAMESPACE_GUTIL1(Games);


/** Represents a traditional playing card.

    It supports the values Ace-Two-Three-...-King.

    It supports 5 suits: Clubs, Diamonds, Hearts, Spades and "Wild"

    The Wild suit is optional, and can be used to represent a Joker or other wild card.

    The Card object is implemented as a shared data object, in order
    to support the Dealing/Hand implementation.
*/
class Card
{
public:

    /** To indicate a card's suit.

        The suits are ordered in "Bridge style" just by convention.  If you
        are sorting cards then you can implement your own sort order, but
        this is a common ordering of the suits.
    */
    enum SuitEnum
    {
        InvalidSuit = 0,
        Clubs = 1,
        Diamonds = 2,
        Hearts = 3,
        Spades = 4,

        /** A special suit to denote a wild card, like a Joker. */
        Wild = 5
    };


    /** Enumerates the possible values a card may have.

        \note An ace is low in the enumeration, but it is up to the implementation
        to add logic for the ace being High.
    */
    enum ValueEnum
    {
        InvalidValue = 0,
        Ace = 1,
        Two = 2,
        Three = 3,
        Four = 4,
        Five = 5,
        Six = 6,
        Seven = 7,
        Eight = 8,
        Nine = 9,
        Ten = 10,
        Jack = 11,
        Queen = 12,
        King = 13,
        Joker = 14
    };

    /** Creates a card with the given value and suit.

        \note It is impossible to change the card's value or suit after
        creation.  This is with cards in real life.  The deck is
        created once, and the cards never change over the entire life of
        the deck.
    */
    Card(ValueEnum v, SuitEnum s) :m_value(v), m_suit(s) {}

    /** Copies the other card. */
    Card(const Card &other) :m_value(other.m_value), m_suit(other.m_suit) {}

    /** Copies the other card. */
    Card &operator = (const Card &other){ new(this) Card(other); return *this; }


    /** Returns the value of the card. */
    ValueEnum Value() const{ return m_value; }

    /** Returns the suit of the card. */
    SuitEnum Suit() const{ return m_suit; }

    /** Returns a human-readable string for this card's value.
        (King, Queen, Seven, etc...)
    */
    char const *ValueString() const{
        return ValueString(m_value);
    }

    /** Returns a human-readable string for this card's suit.
        (Clubs, Diamonds, Spades, etc...)
    */
    char const *SuitString() const{
        return SuitString(m_suit);
    }

    /** Returns a human-readable string for the given card value.
        (King, Queen, Seven, etc...)
    */
    static char const *ValueString(ValueEnum);

    /** Returns a human-readable string for the given card suit.
        (Clubs, Diamonds, Spades, etc...)
    */
    static char const *SuitString(SuitEnum);


    /** Compares the cards' values and suits for equality. */
    bool operator == (const Card &c) const{
        return m_value == c.m_value && m_suit == c.m_suit;
    }

    /** Compares the cards' values and suits for inequality. */
    bool operator != (const Card &c) const{
        return m_value != c.m_value || m_suit != c.m_suit;
    }


private:

    ValueEnum m_value;
    SuitEnum  m_suit;

};



/** Implements a logical hand of cards, to which the deck deals.

    Hands exist as a member of the Deck class, so that ownership of the card
    memory never leaves the Deck.  Cards are simply transferred from the
    Deck to a hand and then back again.

    Just like in a real game of cards, you deal the deck into the hands,
    and then distribute the hands to the players.  In this case, once you
    have distributed the hands, you do not need to redistribute them.  Logically
    the player retains the same "hand" between deals; the cards change of
    course from deal to deal, but the player's "hand" object remains with him
    the whole time.
*/
class Hand
{
    friend class Deck;

    GUtil::Vector<Card *> m_cards;
    GUtil::Vector<Card *> m_cardsPlayed;
    GUtil::FlexibleTypeComparer<Card *> m_cardComparer;

public:

    /** Constructs an empty hand. */
    Hand() { SetCompareFunction(&default_compare); }

    /** Returns the number of cards left in the hand which have not yet been played. */
    GUINT32 CardCount() const{ return m_cards.Length() + m_cardsPlayed.Length(); }

    /** Returns the list of cards in the hand. */
    const GUtil::Vector<Card *> &Cards() const{ return m_cards; }

    /** Sorts the remaining cards in the hand, typically for display purposes. */
    void Sort(){ m_cards.Sort(true, m_cardComparer); }

    /** Sets the compare function for sorting the hand.

        The new sort order takes effect when you call Sort() again.

        \note Each hand may have its own sort order, to allow different playes
        to arrange their hands differently.
    */
    void SetCompareFunction(int (*cmp)(Card * const &, Card * const &)){
        m_cardComparer = cmp;
    }


private:

    /** Provides a default ordering for your cards.

        This default assumes Aces low, and ties with the same value are
        broken by comparing the suits, the order for which is the same as
        in the game Bridge (Clubs, Diamonds, Hearts, Spades).

        \note This ordering is only used for the Hand's Sort() method.  The way
        cards are played and the tricks assessed are dependent on the underlying
        game logic.
    */
    static int default_compare(Card * const &lhs, Card * const &rhs);

};



/** A class to represent a logical deck of cards.

    The implementation is powerful enough to support any kind of card deck,
    with several common decks already implemented in the library.  It is easily
    extensible so you can implement your own special decks on a case-by-case basis.

    After construction, the deck is ready for use.  All the cards are
    in sorted order (like a new pack of cards) until you call Shuffle().  After shuffling, you can call
    Deal() with the proper parameters to create a list of Hands.  By default
    dealing happens one card at a time, sequentially from Hand #1 to Hand #4
    (assuming a 4 hand game).  You can modify this behavior to deal in
    alternating 2-3-2 patterns, as would be the case for Euchre, by overriding
    this virtual function.

    After the hands have been dealt and played, you call Collect() to reassemble
    the cards into the Deck, and then call Shuffle() again to randomize them.
    For your convenience, you could also call CollectAndShuffle().
*/
class Deck
{
public:

    /** Declares several options for initializing the deck. */
    enum DeckTypeEnum
    {
        /** Makes an empty deck with no cards.  This is useful if you're making a
         *  custom deck type.
         *  \sa Deck_UserDefined
        */
        Deck_Empty = 0,

        /** Makes a standard 52-card deck. */
        Deck_Standard52Card,

        /** Makes a standard 52-card deck plus 2 jokers. */
        Deck_Standard52CardWithJokers,

        /** Makes a traditional Euchre deck. */
        Deck_Euchre,

        /** Any enum values higher than or equal to this are user-defined. */
        Deck_UserDefined = 100
    };


    /** Creates a deck object.

        \param number_of_hands The number of hands to which cards will be dealt.
        \param deck_type The type of deck to generate.  The default will create a standard 52-card deck.
    */
    Deck(GUINT32 number_of_hands, DeckTypeEnum deck_type = Deck_Standard52Card);

    /** Frees all memory associated with the deck, including all the cards and hands. */
    virtual ~Deck();


    /** Returns the number of cards remaining in the deck.  That means it
        is the number of cards at initialization, minus the cards which have
        already been dealt to a hand.

        If the deck is not yet initialized, the number of cards is 0.
    */
    GUINT32 CardLength() const{ return m_cards.Length(); }


    /** Collects all the hands which have been dealt and returns them to
        the deck without shuffling.

        \note Specialized decks may have specialized collection functions,
        hence this function is virtual
    */
    virtual void Collect();


    /** Shuffles the deck for the next deal.  By putting this into it's
        own function, it makes the Deal() function happen faster because it doesn't
        have to generate a random order on the fly.

        \note The function is virtual to allow you to make your own shuffle
        function.

        \sa Deal()
    */
    virtual void Shuffle();


    /** Sets the number of hands to which to deal.

        After you set the numbe of hands, you can call Deal().

        \note All the cards must be "collected" and in the deck before you
        can change the number of hands.  Otherwise an exception is thrown.
    */
    void SetNumberHands(GUINT32);

    /** Returns the number of hands. */
    GUINT32 NumberHands() const{ return m_hands.Length(); }

    /** Returns our list of hands for use by the card players.

        \sa SetNumberHands()
    */
    GUtil::Vector<Hand> &Hands(){ return m_hands; }

    /** Returns a const-reference to our list of hands

        \sa SetNumberHands()
    */
    const GUtil::Vector<Hand> &Hands() const{ return m_hands; }


    /** Deals the given number of cards evenly into the hands.

        The implementation calls pick_one() to retrieve a card from the deck,
        and then that card is distributed to the next hand in the deal order.
        You can override pick_one() to change how cards are selected.

        \param starting_hand Indicates on which hand to start dealing.  For
        simplicity, you can simply maintain a deal counter, starting at 0, that increments
        on every deal. The Deal() function takes care to mod this parameter by the
        number of hands, so the current dealer automatically rotates.  The only
        thing you need to check in this implementation is that the deal
        counter does not approach the maximum integer value, in which case
        if it rolls over back to 0 a player may experience the deal skipping over them.
        This is only a problem if the deal counter's range is not divisible evenly
        by the number of players, and it is easily fixed if you roll the counter
        over yourself at a sensible time to retain a continuous deal rotation.

        \param num_rounds The number of cards to deal to each hand.  If you
        do not specify this number, then all the cards will be dealt evenly
        until they are gone.

        \note This is a virtual function to allow you extra flexibility
        to customize the deal.
    */
    virtual void Deal(GUINT32 starting_hand = 0, GUINT32 num_rounds = GUINT32_MAX);

    /** Deals a single card to the given hand.

        Just like the normal Deal() function, this also calls pick_one() to
        select a card, and then deals it to the proper hand.
    */
    void DealOne(GUINT32 hand_index){
        m_hands.At(hand_index).m_cards.PushBack( pick_one() );
    }

    /** Collects all the hands and shuffles the cards so they are ready
        for the next deal.
    */
    void CollectAndShuffle(){
        Collect();
        Shuffle();
    }

    /** Returns true if all the cards are in the deck; i.e. None of the
        cards still exist in any of the hands.

        This is true after calling Collect().

        \sa Collect(), CollectAndShuffle()
    */
    bool IsCollected() const;


    const GUtil::Vector<Card *> &Cards() const{ return m_cards; }


protected:

    /** Our list of cards, accessible to any subclass. */
    GUtil::Vector<Card *> m_cards;

    /** Our list of hands, accessible to any subclass. */
    GUtil::Vector<Hand> m_hands;

    /** Appends a standard 52 card deck to our list of cards.
        Used for deck initialization.
    */
    void append_standard_52_cards();

    /** Appends 2 jokers to our list of cards.
        Used for deck initialization.
    */
    void append_2_jokers();

    /** Appends a euchre deck to our list of cards.
        Used for deck initialization.
    */
    void append_euchre();

    /** Selects a card from the deck.  The default implementation simply
        grabs a card at the bottom of the deck and removes it.

        This is called to select each card when you call Deal(), so you can
        customize the behavior by overriding this.
    */
    virtual Card *pick_one();

    /** This function clears all the cards out of a hand.  It is protected
        so subclasses can use it if they need to.
    */
    static void clear_hand(Hand &h){
        h.m_cards.Empty();
        h.m_cardsPlayed.Empty();
    }

};





/** A special deck that has an infinite number of cards.

    This is useful for games like blackjack, which would use such
    a deck in real life, but since it's not possible they can only
    approximate it by using 4+ decks.  This implementation, however,
    is truly infinite.  No card-counter can foil this.

    \note There is no need to call Shuffle() on this deck.  When you deal
    it randomly picks a card out of the deck, rather than a deal that
    takes cards off the back of the deck.  Dealing does not remove cards
    from the deck, so there is always equal probability of any card coming up.
*/
class InfiniteDeck :
        public Deck
{
public:

    /** \sa Deck() */
    InfiniteDeck(GUINT32 number_of_hands) :Deck(number_of_hands) {}

    virtual ~InfiniteDeck();

    /** Overridden from the Deck implementation, so it just throws away
        the cards in the hands, rather than returning them to the deck.
    */
    virtual void Collect();


protected:

    /** Overridden from Deck::pick_one(), so it picks a card randomly and
        doesn't remove it from the deck.
    */
    virtual Card *pick_one();

};


END_NAMESPACE_GUTIL1;

#endif // GUTIL_CARDS_H
