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

#ifndef GUTIL_DICE_H
#define GUTIL_DICE_H

#include "gutil_vector.h"

NAMESPACE_GUTIL1(Games);


/** Represents a bunch of dice.

    You set the number of dice in the bunch through the constructor, or with SetNumberDice().

    By default, the dice are 6-sided, but you can configure this with SetNumberSides().  The maximum
    number of sides you may have on a die is 256.

    You roll them with the Roll() function, and inspect them with Inspect().
*/
class Dice
{
public:

    /** A typedef for a Die object, which can have up to 256 sides. */
    typedef GUINT8 Die;

    /** Constructs a set of the specified number of dice, with the given number
        of sides, and all dice initialized to 0 (invalid).

        \sa Roll(), Inspect()
    */
    explicit Dice(GUINT32 number_of_dice, GUINT8 number_of_sides = 6);

    /** Sets the number of dice in the bunch.

        After calling this you must call Roll() again before the dice are valid.
    */
    void SetNumberDice(GUINT32 number_of_dice){ m_dice.Resize(number_of_dice, 0); }

    /** Returns the number of dice in the bunch. */
    GUINT32 GetNumberDice() const{ return m_dice.Length(); }

    /** Sets the number of sides for the dice.

        After calling this you must call Roll() again before the dice are valid.
    */
    void SetNumberSides(GUINT8 number_of_sides){ m_sides = number_of_sides; }

    /** Returns the number of sides on the dice being rolled. */
    GUINT8 GetNumberSides() const{ return m_sides; }

    /** Rolls the dice, which randomly generates a value for each die in the collection.

        You must call this before calling Inspect(), otherwise all dice will be 0 (invalid).

        \note This is virtual so that you can program special dice functionality, such
        as removing the max and min die values, or raising an event when doubles are rolled,
        or whatever you need.

        \sa Inspect()
    */
    virtual void Roll();

    /** Returns the list of dice for inspection.

        If you inspect them before rolling, then all dice will be 0 (invalid).

        \sa Roll()
    */
    const GUtil::Vector<Die> &Inspect() const{
        return m_dice;
    }

    /** Causes all dice to be set to 0 (invalid).

        \note If you want to clear the memory, then call SetNumberDice() with 0.
    */
    void Clear(){
        GUtil::ForEach(m_dice, [](Die &d){ d = 0; });
    }


private:

    GUtil::Vector<Die> m_dice;
    GUINT8 m_sides;

};


END_NAMESPACE_GUTIL1;

#endif // GUTIL_DICE_H
