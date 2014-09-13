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

#include "dice.h"
#include "gutil_rng.h"
USING_NAMESPACE_GUTIL;

NAMESPACE_GUTIL1(Games);


Dice::Dice(GUINT32 n, GUINT8 s)
    :m_dice(n), m_sides(s)
{
    // Initialize all dice to 0, which means they have not yet been rolled
    m_dice.Resize(n, 0);
}

void Dice::Roll()
{
    ForEach(m_dice, [this](Die &d){
        d = GlobalRNG()->U_Discrete(1, m_sides);
    });
}


END_NAMESPACE_GUTIL1;
