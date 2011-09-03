/*Copyright 2011 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include "bst_node.h"
#include "gutil_globals.h"
#include "Core/exception.h"
GUTIL_USING_CORE_NAMESPACE(DataObjects);

bst_node::bst_node()
    :Parent(0),
      LChild(0),
      RChild(0),
      LeftmostChild(this),
      RightmostChild(this),
      Height(0),
      Data(0)
{

}

bst_node::~bst_node()
{
    if(LChild)
        delete LChild;
    if(RChild)
        delete RChild;
}

int bst_node::HeightDifference() const
{
    int sum(0);
    if(LChild)
        sum += (1 + LChild->Height);
    if(RChild)
        sum -= (1 + RChild->Height);
    return sum;
}

bool bst_node::Balanced() const
{
    return gAbs(HeightDifference()) <= 1;
}

SideEnum bst_node::SideOfParent() const
{
    SideEnum ret(NoSide);
    if(Parent)
    {
        if(Parent->LChild == this)
            ret = LeftSide;
        else if(Parent->RChild == this)
            ret = RightSide;
    }
    return ret;
}
