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
#include <cassert>
GUTIL_USING_CORE_NAMESPACE(DataObjects);

bst_node::bst_node()
    :Parent(0),
      LChild(0),
      RChild(0),
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
        sum += LChild->Height;
    if(RChild)
        sum -= RChild->Height;
    return sum;
}

bool bst_node::Balanced() const
{
    return gAbs(HeightDifference()) <= 1;
}

void bst_node::rebalance(bst_node *n)
{
    int height_difference = n->HeightDifference();
    if(height_difference > 1)
    {
        // The node is left-heavy

        // Check if it's LR imbalance so we can resolve that first.
        if(n->LChild->HeightDifference() < 0)
            rotate_left(n->LChild);

        // Now that the LR imbalance is fixed, do the LL rebalance.
        rotate_right(n);
    }
    else if(height_difference < -1)
    {
        // The node is right-heavy

        // Check if it's RL imbalance so we can resolve that first.
        if(n->LChild->HeightDifference() > 0)
            rotate_right(n->RChild);

        // Now that the RL imbalance is fixed, do the RR rebalance.
        rotate_left(n);
    }
    else
        // Node already balanced
        return;

    // Update the node's height
    n->Height = gMax(n->LChild->Height, n->RChild->Height) + 1;
}

void bst_node::rotate_right(bst_node *n)
{
    bst_node *parent(n->Parent);
    if(parent)
    {
        if(parent->LChild == n)
            parent->LChild = n->LChild;
        else
            parent->RChild = n->LChild;
    }
    n->LChild->Parent = parent;

    bst_node *tmp(n->LChild->RChild);
    n->Parent = n->LChild;
    n->LChild->RChild = n;
    n->LChild = tmp;
    tmp->Parent = n;
}

void bst_node::rotate_left(bst_node *parent)
{
    bst_node *parent_parent(parent->Parent);
    if(parent_parent)
    {
        if(parent_parent->LChild == parent)
            parent_parent->LChild = parent->RChild;
        else
            parent_parent->RChild = parent->RChild;
    }
    parent->RChild->Parent = parent_parent;

    bst_node *tmp(parent->RChild->LChild);
    parent->Parent = parent->RChild;
    parent->RChild->LChild = parent;
    parent->RChild = tmp;
    tmp->Parent = parent;
}

void bst_node::Insert(bst_node *parent, bst_node *new_node, bst_node::SideEnum side)
{
    bst_node **node_ref(0);
    switch(side)
    {
    case LeftSide:
        node_ref = &parent->LChild;
        break;
    case RightSide:
        node_ref = &parent->RChild;
        break;
    default:
        return;
    }

    // We are only allowed to insert on the leafs of nodes, so if we were told to
    //  insert on a non-leaf that is an error.
    assert(*node_ref == 0);

    *node_ref = new_node;
    new_node->Parent = parent;

    // Now we need to ascend the tree to the root and update the heights
    update_height(parent);
}

void bst_node::Delete(bst_node *node, bst_node *replacement)
{
    // This variable determines where to start adjusting the node height after deletion.
    bst_node *start_height_adjustment(0);

    if(replacement)
    {
        // If the replacement has a child (at most 1) then we move it into the replacement's place
        bst_node *replacement_child(0);
        if(replacement->RChild)
        {
            replacement_child = replacement->RChild;
            replacement->RChild = 0;
        }
        else if(replacement->LChild)
        {
            replacement_child = replacement->LChild;
            replacement->LChild = 0;
        }

        if(replacement_child)
        {
            switch(replacement->SideOfParent())
            {
            case RightSide:
                replacement->Parent->RChild = replacement_child;
            case LeftSide:
                replacement->Parent->LChild = replacement_child;
                break;
            default:
                assert(false);
                break;
            }
            replacement_child->Parent = replacement->Parent;
            start_height_adjustment = replacement->Parent;
        }

        replacement->Parent = node->Parent;
        if(replacement != node->RChild)
            replacement->RChild = node->RChild;
        if(replacement != node->LChild)
            replacement->LChild = node->LChild;
    }
    else
    {
        start_height_adjustment = node->Parent;
    }


    if(node->RChild)
        node->RChild->Parent = replacement;
    if(node->LChild)
        node->LChild->Parent = replacement;
    if(node->Parent)
    {
        switch(node->SideOfParent())
        {
        case RightSide:
            node->Parent->RChild = replacement;
            break;
        case LeftSide:
            node->Parent->LChild = replacement;
            break;
        default:
            assert(false);
            break;
        }
    }

    // Walk up the tree and update the height variables.
    update_height(start_height_adjustment);

    // Delete the node
    node->RChild = 0;
    node->LChild = 0;
    delete node;
}

bst_node::SideEnum bst_node::SideOfParent() const
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

void bst_node::update_height(bst_node *n)
{
    if(n)
    {
        n->Height = gMax(n->LChild->Height, n->RChild->Height) + 1;

        // Rebalance the node if it's unbalanced
        if(!n->Balanced())
            rebalance(n);

        if(n->Parent)
            update_height(n->Parent);
    }
}
