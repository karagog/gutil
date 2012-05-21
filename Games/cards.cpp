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

#include "cards.h"
#include "gutil_circularint.h"
#include "gutil_rng.h"
USING_NAMESPACE_GUTIL1(Utils);
USING_NAMESPACE_GUTIL1(DataObjects);


/** A lookup table for card value strings. */
static char const * __card_value_LUT [] =
{
    "INVALID",
    "Ace",
    "Two",
    "Three",
    "Four",
    "Five",
    "Six",
    "Seven",
    "Eight",
    "Nine",
    "Ten",
    "Jack",
    "Queen",
    "King",
    "Joker"
};


/** A lookup table for card suit strings. */
static char const * __card_suit_LUT [] =
{
    "INVALID",
    "Clubs",
    "Diamonds",
    "Hearts",
    "Spades",
    "Wild"
};


NAMESPACE_GUTIL1(Games);


Deck::Deck(GUINT32 number_of_hands)
    :m_hands(number_of_hands)
{
    // Make sure the RNG is initialized
    RNG::Initialize();

    SetNumberHands(number_of_hands);
}

Deck::~Deck()
{
    Clear();
}

void Deck::Clear()
{
    Collect();

    // Dispose of the card memory
    G_FOREACH(Card *c, m_cards)
        delete c;

    m_cards.Empty();
}

void Deck::Initialize(Deck::InitializationEnum e)
{
    if(IsInitialized())
        Clear();

    switch(e)
    {
    case Init_Standard52Card:
        _append_standard_52_cards();
        break;
    case Init_Standard52CardWithJokers:
        _append_standard_52_cards();
        _append_2_jokers();
        break;
    case Init_Euchre:
        _append_euchre();
        break;
    default:
        THROW_NEW_GUTIL_EXCEPTION(NotImplementedException);
    }
}

void Deck::_append_standard_52_cards()
{
    m_cards.Reserve(m_cards.Count() + 52);

    for(Card::SuitEnum suit = Card::Clubs; suit <= Card::Spades; suit = (Card::SuitEnum)(suit + 1))
        for(Card::ValueEnum value = Card::Ace; value <= Card::King; value = (Card::ValueEnum)(value + 1))
            m_cards.PushBack(new Card(value, suit));
}

void Deck::_append_2_jokers()
{
    m_cards.Reserve(m_cards.Count() + 2);

    for(int i = 0; i < 2; ++i)
        m_cards.PushBack(new Card(Card::Joker, Card::Wild));
}

void Deck::_append_euchre()
{
    m_cards.Reserve(m_cards.Count() + 24);

    for(Card::SuitEnum suit = Card::Clubs; suit <= Card::Spades; suit = (Card::SuitEnum)(suit + 1))
    {
        for(Card::ValueEnum value = Card::Nine; value <= Card::King; value = (Card::ValueEnum)(value + 1)){
            m_cards.PushBack(new Card(value, suit));
        }
        m_cards.PushBack(new Card(Card::Ace, suit));
    }
}

void Deck::Shuffle()
{
    if(0 < m_cards.Count())
    {
        Vector<Card *> tmp( m_cards.Count() );

        // Deal each card randomly into the tmp vector
        while(1 < m_cards.Count())
        {
            GINT32 index( RNG::U_Discrete(0, m_cards.Count() - 1) );

            tmp.PushBack( m_cards[index] );
            m_cards.RemoveAt(index);
        }
        tmp.PushBack( m_cards[0] );

        // Now all the cards exist in the tmp vector, so we call swap to put them
        //  back in the deck in constant time.
        Vector<Card *>::Swap(m_cards, tmp);
    }
}

void Deck::Deal(GUINT32 starting_hand, GUINT32 num_cards)
{
    if(starting_hand >= m_hands.Count())
        THROW_NEW_GUTIL_EXCEPTION(IndexOutOfRangeException);

    CircularInt tmp(0, m_hands.Count() - 1, starting_hand);
    while(num_cards-- != 0 && 0 < m_cards.Count())
        for(GUINT32 i(0); 0 < m_cards.Count() && i < m_hands.Count(); ++i, ++tmp)
            m_hands[tmp].m_cards.PushBack( pick_one() );
}

Card *Deck::pick_one()
{
    Card *ret( m_cards.Back() );
    m_cards.PopBack();
    return ret;
}

void Deck::SetNumberHands(GUINT32 N)
{
    if(!IsCollected())
        THROW_NEW_GUTIL_EXCEPTION2(Exception, "The cards must be collected first");
    m_hands.Resize(N);
}

bool Deck::IsCollected() const
{
    G_FOREACH_CONST(const Hand &h, m_hands){
        if(0 < h.CardCount())
            return false;
    }
    return true;
}

void Deck::Collect()
{
    G_FOREACH(Hand &h, m_hands)
    {
        m_cards.PushBack(h.m_cards);
        m_cards.PushBack(h.m_cardsPlayed);

        clear_hand(h);
    }
}



int Hand::default_compare(Card * const &lhs, Card * const &rhs)
{
    // Compare the suits
    if(lhs->Suit() < rhs->Suit())
        return -1;
    else if(rhs->Suit() < lhs->Suit())
        return 1;

    // The suits are equal, so compare the values
    if(lhs->Value() < rhs->Value())
        return -1;
    else if(rhs->Value() < lhs->Value())
        return 1;

    // Both the suits and the values are equal, the cards must be equal
    return 0;
}



char const *Card::ValueString(Card::ValueEnum v)
{
    if(v < Card::InvalidValue || v > Card::Joker)
        THROW_NEW_GUTIL_EXCEPTION2(Exception, "Invalid card value");
    return __card_value_LUT[ static_cast<int>(v) ];
}

char const *Card::SuitString(Card::SuitEnum s)
{
    if(s < Card::InvalidSuit || s > Card::Wild)
        THROW_NEW_GUTIL_EXCEPTION2(Exception, "Invalid suit value");
    return __card_suit_LUT[ static_cast<int>(s) ];
}









Card *InfiniteDeck::pick_one()
{
    // Pick a card at random and don't remove it from the deck
    return m_cards[ RNG::U_Discrete(0, m_cards.Count() - 1) ];
}

void InfiniteDeck::Collect()
{
    G_FOREACH(Hand &h, m_hands)
        clear_hand(h);
}

InfiniteDeck::~InfiniteDeck()
{
    Collect();
}


END_NAMESPACE_GUTIL1;