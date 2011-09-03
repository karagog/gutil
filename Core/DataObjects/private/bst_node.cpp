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
        if(n->RChild->HeightDifference() > 0)
            rotate_right(n->RChild);

        // Now that the RL imbalance is fixed, do the RR rebalance.
        rotate_left(n);
    }
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
    if(tmp)
        tmp->Parent = n;

    // Have to refresh the node we just rotated, the other one will be refreshed
    //  automatically when we walk up the tree
    refresh_node_state(n);    
}

void bst_node::rotate_left(bst_node *n)
{
    bst_node *parent(n->Parent);
    if(parent)
    {
        if(parent->LChild == n)
            parent->LChild = n->RChild;
        else
            parent->RChild = n->RChild;
    }
    n->RChild->Parent = parent;

    bst_node *tmp(n->RChild->LChild);
    n->Parent = n->RChild;
    n->RChild->LChild = n;
    n->RChild = tmp;
    if(tmp)
        tmp->Parent = n;

    // Have to refresh the node we just rotated, the other one will be refreshed
    //  automatically when we walk up the tree
    refresh_node_state(n);
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

    *node_ref = new_node;
    new_node->Parent = parent;

    // Now we need to ascend the tree to the root and update the heights
    walk_parents_update_heights_rebalance(parent);
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
            replacement_child = replacement->RChild;
        else if(replacement->LChild)
            replacement_child = replacement->LChild;

        if(node == replacement->Parent)
            start_height_adjustment = replacement;
        else
        {
            start_height_adjustment = replacement->Parent;
            switch(replacement->SideOfParent())
            {
            case RightSide:
                replacement->Parent->RChild = replacement_child;
                break;
            case LeftSide:
                replacement->Parent->LChild = replacement_child;
                break;
            default:
                break;
            }

            if(replacement_child)
                replacement_child->Parent = replacement->Parent;
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


    if(node->RChild && node->RChild != replacement)
        node->RChild->Parent = replacement;
    if(node->LChild && node->LChild != replacement)
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
            break;
        }
    }

    // Delete the node (set children to 0 so to not delete them)
    node->LChild = 0;
    node->RChild = 0;
    delete node;

    // Walk up the tree and update the height variables.
    walk_parents_update_heights_rebalance(start_height_adjustment);
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

void bst_node::refresh_node_state(bst_node *n)
{
    // Update the node's height cache
    if(!n->LChild && !n->RChild)
        n->Height = 0;
    else
    {
        const int lheight(n->LChild ? n->LChild->Height : 0);
        const int rheight(n->RChild ? n->RChild->Height : 0);
        n->Height = gMax(lheight, rheight) + 1;
    }

    // Update the left-most and right-most child caches
    n->LeftmostChild = n->LChild ? n->LChild->LeftmostChild : n;
    n->RightmostChild = n->RChild ? n->RChild->RightmostChild : n;
}

void bst_node::walk_parents_update_heights_rebalance(bst_node *n)
{
    if(n)
    {
        refresh_node_state(n);

        // Rebalance the node if it's unbalanced
        if(!n->Balanced())
            rebalance(n);

        walk_parents_update_heights_rebalance(n->Parent);
    }
}
