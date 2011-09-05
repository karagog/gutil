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

#include "bst_t.h"
#include "Core/exception.h"
#include "gassert.h"
GUTIL_USING_CORE_NAMESPACE(DataObjects);
GUTIL_USING_CORE_NAMESPACE(Interfaces);

bst_t::bst_t(bst_t::void_wrapper *vw)
    :cmp(static_cast<IVoidComparer*>(vw)),
      root(0),
      data_access_wrapper(vw),
      m_size(0)
{}

bst_t::~bst_t()
{
    // Note: We delete the compare interface because it has a virtual destructor
    //  so the rest of the object will get deleted too.
    Clear();
    delete cmp;
}

bst_node *bst_t::add(const void *const v)
{
    bst_node *new_node;
    if(root)
    {
        // Find the place to insert, has to be a leaf node
        bst_node *cur( root );
        bst_node *next_cur( root );
        SideEnum insertion_side(LeftSide);
        while(next_cur)
        {
            cur = next_cur;
            int cmp_res( cmp->CompareVoid(v, cur->Data) );

            if(cmp_res < 0)
            {
                next_cur = cur->LChild;
                insertion_side = LeftSide;
            }
            else if(cmp_res > 0)
            {
                next_cur = cur->RChild;
                insertion_side = RightSide;
            }
            else
            {
                // There's an insertion collision
                THROW_NEW_GUTIL_EXCEPTION2(GUtil::Core::Exception,
                                           "Object already exists in BST.");
            }
        }

        new_node = new bst_node;
        switch(insertion_side)
        {
        case LeftSide:
            cur->LChild = new_node;
            break;
        case RightSide:
            cur->RChild = new_node;
            break;
        default:
            GASSERT(false);
        }
        new_node->Parent = cur;

        // Now we need to ascend the tree to the root and update the heights
        walk_parents_update_heights_rebalance(cur);

        _update_root_node();
    }
    else
    {
        // If the root is null, then this is the first item in the tree (easy case)
        root = new_node = new bst_node;
    }

    m_size++;

    new_node->Data = data_access_wrapper->CopyVoid(v);
    return new_node;
}

bool bst_t::remove(const const_iterator &iter)
{
    bst_node *cur(iter.current);
    if(cur)
    {
        // Remove it.  The deletion algorithm goes as follows:
        //  I need to find a replacement from the leaves of the tree to
        //  come take my place, then I will rebalance from the bottom of the tree.
        //  I will find a replacement as the left-most item on my right side, or my
        //  right-most item on my left side, whichever one is taller.

        // Find a replacement node
        bst_node *replacement(0);
        if(cur->Height > 0)
        {
            if(cur->HeightDifference() > 0)
            {
                // Left-heavy
                replacement = cur->LChild->RightmostChild;
            }
            else
            {
                // Right-heavy
                replacement = cur->RChild->LeftmostChild;
            }
        }


        if(root == cur)
        {
            // We have to pre-emptively adjust our root node, because we will be unable to
            //  find the new root once it's deleted.
            if(root->RChild)
                root = root->RChild;
            else if(root->LChild)
                root = root->LChild;
            else
                root = 0;
        }

        data_access_wrapper->DeleteVoid(cur->Data);

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

            if(cur == replacement->Parent)
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

            replacement->Parent = cur->Parent;
            if(replacement != cur->RChild)
                replacement->RChild = cur->RChild;
            if(replacement != cur->LChild)
                replacement->LChild = cur->LChild;
        }
        else
        {
            start_height_adjustment = cur->Parent;
        }


        if(cur->RChild && cur->RChild != replacement)
            cur->RChild->Parent = replacement;
        if(cur->LChild && cur->LChild != replacement)
            cur->LChild->Parent = replacement;
        if(cur->Parent)
        {
            switch(cur->SideOfParent())
            {
            case RightSide:
                cur->Parent->RChild = replacement;
                break;
            case LeftSide:
                cur->Parent->LChild = replacement;
                break;
            default:
                break;
            }
        }

        // Delete the node (set children to 0 so to not delete them)
        cur->LChild = 0;
        cur->RChild = 0;
        delete cur;

        // Walk up the tree and update the height variables.
        walk_parents_update_heights_rebalance(start_height_adjustment);

        _update_root_node();

        m_size--;
    }
    return cur;
}

bool bst_t::remove(const void *const v)
{
    return remove(const_iterator(search(v), *this));
}

bst_node *bst_t::search(const void *const v) const
{
    return search(v, data_access_wrapper);
}

bst_node *bst_t::search(const void *const v, const void_wrapper *vw) const
{
    bst_node *cur( root );
    while(cur)
    {
        int cmp_res( vw->CompareVoid(cur->Data, v) );

        if(cmp_res < 0)
            cur = cur->RChild;
        else if(cmp_res > 0)
            cur = cur->LChild;
        else
            break;
    }
    return cur;
}

void bst_t::Clear()
{
    if(root)
    {
        _cleanup_memory(root);
        root = 0;
        m_size = 0;
    }
}

void bst_t::_cleanup_memory(bst_node *n)
{
    if(n->LChild)
    {
        _cleanup_memory(n->LChild);
        n->LChild = 0;
    }
    if(n->RChild)
    {
        _cleanup_memory(n->RChild);
        n->RChild = 0;
    }
    data_access_wrapper->DeleteVoid(n->Data);
    delete n;
}

bst_node *bst_t::first() const
{
    return root ? root->LeftmostChild : 0;
}

bst_node *bst_t::last() const
{
    return root ? root->RightmostChild : 0;
}

void bst_t::_update_root_node()
{
    bst_node *n(root);
    while(n && (n = n->Parent))
        root = n;
}





bst_t::const_iterator::const_iterator()
    :current(0),
      cmp(0),
      mem_begin(0),
      mem_end(0)
{}

bst_t::const_iterator::const_iterator(bst_node *n, const bst_t &t)
    :current(n),
      cmp(t.cmp),
      mem_begin(0),
      mem_end(0)
{}

bst_t::const_iterator::const_iterator(const bst_t::const_iterator &o)
    :current(o.current),
      cmp(o.cmp),
      mem_begin(o.mem_begin),
      mem_end(o.mem_end)
{}



bool bst_t::const_iterator::operator == (const bst_t::const_iterator &o) const
{
    return current == o.current &&
            (current != 0 ||
             ((mem_begin == 0 && mem_end == 0) || (o.mem_begin == 0 && o.mem_end == 0) ||
                 (mem_begin == o.mem_begin && mem_end == o.mem_end)));
}

bool bst_t::const_iterator::operator != (const bst_t::const_iterator &o) const
{
    return !(*this == o);
}

bst_t::const_iterator::operator bool() const
{
    return current;
}

void bst_t::const_iterator::advance()
{
    if(current)
    {
        if(current->RChild)
            current = current->RChild->LeftmostChild;
        else
        {
            // Ascend current's parents to find the next greater element
            bst_node *cur(current);
            do
            {
                if(cur->SideOfParent() == LeftSide)
                {
                    current = cur->Parent;
                    break;
                }
            }while((cur = cur->Parent));

            if(!cur)
            {
                // We've hit the end of the BST
                mem_end = current;
                current = 0;
            }
        }
    }
    else if(mem_begin)
    {
        current = mem_begin;
        mem_begin = 0;
    }
    else
        THROW_NEW_GUTIL_EXCEPTION2(GUtil::Core::Exception,
                                   "Can't move iterator past the end of the container.");
}

void bst_t::const_iterator::retreat()
{
    if(current)
    {
        if(current->LChild)
            current = current->LChild->RightmostChild;
        else
        {
            // Ascend current's parents to find the next lesser element
            bst_node *cur(current);
            do
            {
                if(cur->SideOfParent() == RightSide)
                {
                    current = cur->Parent;
                    break;
                }
            }while((cur = cur->Parent));

            if(!cur)
            {
                // We've hit the beginning of the BST
                mem_begin = current;
                current = 0;
            }
        }
    }
    else if(mem_end)
    {
        current = mem_end;
        mem_end = 0;
    }
    else
        THROW_NEW_GUTIL_EXCEPTION2(GUtil::Core::Exception,
                                   "Can't move iterator before the beginning of the container.");
}

bst_t::const_iterator &bst_t::const_iterator::operator ++()
{
    advance();
    return *this;
}

bst_t::const_iterator bst_t::const_iterator::operator ++(int)
{
    const_iterator ret(*this);
    advance();
    return ret;
}

bst_t::const_iterator &bst_t::const_iterator::operator --()
{
    retreat();
    return *this;
}

bst_t::const_iterator bst_t::const_iterator::operator --(int)
{
    const_iterator ret(*this);
    retreat();
    return ret;
}

bool bst_t::const_iterator::operator <(const bst_t::const_iterator &o) const
{
    if(current && o.current)
    {
        return cmp->CompareVoid(current->Data, o.current->Data) < 0;
    }
    else
    {
        if(current == 0 && o.current == 0){
            // I am at the beginning, o is at the end
            if(mem_begin && o.mem_end)
                return true;
            return false;
        }
        else if(current == 0)
        {
            // I am at the beginning, so I come before o
            return mem_begin;
        }
        else
        {
            // o is at the end, so I come before it
            return o.mem_end;
        }
    }
}

bool bst_t::const_iterator::operator > (const const_iterator &o) const
{
    return !(*this <= o);
}

bool bst_t::const_iterator::operator <= (const const_iterator &o) const
{
    return *this == o || *this < o;
}

bool bst_t::const_iterator::operator >= (const const_iterator &o) const
{
    return !(*this < o);
}



void bst_t::walk_parents_update_heights_rebalance(bst_node *n)
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

void bst_t::refresh_node_state(bst_node *n)
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

void bst_t::rotate_right(bst_node *n)
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

void bst_t::rotate_left(bst_node *n)
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

void bst_t::rebalance(bst_node *n)
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

bst_node *bst_t::const_iterator::operator->()
{
    return current;
}

const bst_node *bst_t::const_iterator::operator->() const
{
    return current;
}
const bst_node &bst_t::const_iterator::operator *() const
{
    return *current;
}
bst_node &bst_t::const_iterator::operator *()
{
    return *current;
}
