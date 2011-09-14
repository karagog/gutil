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

#include "bst_p.h"
#include "Core/exception.h"
#include "gassert.h"
GUTIL_USING_CORE_NAMESPACE(DataObjects);
GUTIL_USING_CORE_NAMESPACE(Interfaces);

bst_p::bst_p(bst_p::void_wrapper *vw)
    :root(0),
      data_access_wrapper(vw),
      m_size(0)
{}

bst_p::~bst_p()
{
    // Note: We delete the compare interface because it has a virtual destructor
    //  so the rest of the object will get deleted too.
    Clear();
    delete data_access_wrapper;
}

bst_p::void_wrapper::~void_wrapper(){}

bst_p::bst_node *bst_p::add(const void *const v)
{
    bst_node *new_node;
    if(root)
    {
        // Find the place to insert, has to be a leaf node
        bst_node *cur( root );
        bst_node *next_cur( root );
        binary_tree_node::SideEnum insertion_side(binary_tree_node::LeftSide);
        while(next_cur)
        {
            cur = next_cur;
            int cmp_res( data_access_wrapper->CompareVoid(cur->Data, v) );

            if(cmp_res < 0)
            {
                next_cur = static_cast<bst_node *>(cur->RChild);
                insertion_side = binary_tree_node::RightSide;
            }
            else if(cmp_res > 0)
            {
                next_cur = static_cast<bst_node *>(cur->LChild);
                insertion_side = binary_tree_node::LeftSide;
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
        case binary_tree_node::LeftSide:
            cur->LChild = new_node;
            break;
        case binary_tree_node::RightSide:
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

bool bst_p::remove(const const_iterator &iter)
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
                replacement = static_cast<bst_node *>(cur->LChild)->RightmostChild;
            }
            else
            {
                // Right-heavy
                replacement = static_cast<bst_node *>(cur->RChild)->LeftmostChild;
            }
        }


        if(root == cur)
        {
            // We have to pre-emptively adjust our root node, because we will be unable to
            //  find the new root once it's deleted.
            if(root->RChild)
                root = static_cast<bst_node *>(root->RChild);
            else if(root->LChild)
                root = static_cast<bst_node *>(root->LChild);
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
                replacement_child = static_cast<bst_node *>(replacement->RChild);
            else if(replacement->LChild)
                replacement_child = static_cast<bst_node *>(replacement->LChild);

            if(cur == replacement->Parent)
                start_height_adjustment = replacement;
            else
            {
                start_height_adjustment = static_cast<bst_node *>(replacement->Parent);
                switch(replacement->SideOfParent())
                {
                case binary_tree_node::RightSide:
                    replacement->Parent->RChild = replacement_child;
                    break;
                case binary_tree_node::LeftSide:
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
            start_height_adjustment = static_cast<bst_node *>(cur->Parent);
        }


        if(cur->RChild && cur->RChild != replacement)
            cur->RChild->Parent = replacement;
        if(cur->LChild && cur->LChild != replacement)
            cur->LChild->Parent = replacement;
        if(cur->Parent)
        {
            switch(cur->SideOfParent())
            {
            case binary_tree_node::RightSide:
                cur->Parent->RChild = replacement;
                break;
            case binary_tree_node::LeftSide:
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

bool bst_p::remove(const void *const v)
{
    return remove(const_iterator(search(v), data_access_wrapper));
}

bst_p::bst_node *bst_p::search(const void *const v) const
{
    return search(v, data_access_wrapper);
}

bst_p::bst_node *bst_p::search(const void *const v, const IVoidComparer *vw) const
{
    bst_node *cur( root );
    while(cur)
    {
        int cmp_res( vw->CompareVoid(cur->Data, v) );

        if(cmp_res < 0)
            cur = static_cast<bst_node *>(cur->RChild);
        else if(cmp_res > 0)
            cur = static_cast<bst_node *>(cur->LChild);
        else
            break;
    }
    return cur;
}

void bst_p::Clear()
{
    if(root)
    {
        _cleanup_memory(root);
        root = 0;
        m_size = 0;
    }
}

void bst_p::_cleanup_memory(binary_tree_node *n)
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

bst_p::bst_node *bst_p::first() const
{
    return root ? root->LeftmostChild : 0;
}

bst_p::bst_node *bst_p::last() const
{
    return root ? root->RightmostChild : 0;
}

void bst_p::_update_root_node()
{
    binary_tree_node *n(root);
    while(n && (n = n->Parent))
        root = static_cast<bst_node *>(n);
}





bst_p::const_iterator::const_iterator()
    :current(0),
      cmp(0),
      mem_begin(0),
      mem_end(0)
{}

bst_p::const_iterator::const_iterator(bst_node *n, const IVoidComparer *const vc)
    :current(n),
      cmp(vc),
      mem_begin(0),
      mem_end(0)
{}


bool bst_p::const_iterator::operator == (const bst_p::const_iterator &o) const
{
    return current == o.current &&
            (current != 0 ||
             ((mem_begin == 0 && mem_end == 0) || (o.mem_begin == 0 && o.mem_end == 0) ||
                 (mem_begin == o.mem_begin && mem_end == o.mem_end)));
}

bst_p::const_iterator::operator bool() const
{
    return current;
}

void bst_p::const_iterator::advance()
{
    if(current)
    {
        if(current->RChild)
            current = static_cast<bst_node *>(current->RChild)->LeftmostChild;
        else
        {
            binary_tree_node *jump_to(current);
            while(jump_to && jump_to->SideOfParent() != binary_tree_node::LeftSide)
                jump_to = jump_to->Parent;

            if(jump_to)
                current = static_cast<bst_node *>(jump_to->Parent);
            else
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
}

void bst_p::const_iterator::retreat()
{
    if(current)
    {
        if(current->LChild)
            current = static_cast<bst_node *>(current->LChild)->RightmostChild;
        else
        {
            binary_tree_node *jump_to(current);
            while(jump_to && jump_to->SideOfParent() != binary_tree_node::RightSide)
                jump_to = jump_to->Parent;

            if(jump_to)
                current = static_cast<bst_node *>(jump_to->Parent);
            else
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
}

bst_p::const_iterator &bst_p::const_iterator::operator ++()
{
    advance();
    return *this;
}

bst_p::const_iterator bst_p::const_iterator::operator ++(int)
{
    const_iterator ret(*this);
    advance();
    return ret;
}

bst_p::const_iterator &bst_p::const_iterator::operator --()
{
    retreat();
    return *this;
}

bst_p::const_iterator bst_p::const_iterator::operator --(int)
{
    const_iterator ret(*this);
    retreat();
    return ret;
}

bool bst_p::const_iterator::operator <(const bst_p::const_iterator &o) const
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

bool bst_p::const_iterator::operator > (const const_iterator &o) const
{
    return !(*this <= o);
}

bool bst_p::const_iterator::operator <= (const const_iterator &o) const
{
    return *this == o || *this < o;
}

bool bst_p::const_iterator::operator >= (const const_iterator &o) const
{
    return !(*this < o);
}



void bst_p::walk_parents_update_heights_rebalance(bst_node *n, bool already_balanced)
{
    if(n)
    {
        refresh_node_state(n);

        // Rebalance the node if it's unbalanced
        if(!already_balanced && !n->Balanced())
        {
            rebalance(n);
            already_balanced = true;
        }

        walk_parents_update_heights_rebalance(static_cast<bst_node *>(n->Parent), already_balanced);
    }
}

void bst_p::refresh_node_state(bst_node *n)
{
    // Update the node's height cache
    if(!n->LChild && !n->RChild)
        n->Height = 0;
    else
    {
        const int lheight(n->LChild ? static_cast<bst_node *>(n->LChild)->Height : 0);
        const int rheight(n->RChild ? static_cast<bst_node *>(n->RChild)->Height : 0);
        n->Height = gMax(lheight, rheight) + 1;
    }

    // Update the left-most and right-most child caches
    n->LeftmostChild = n->LChild ? static_cast<bst_node *>(n->LChild)->LeftmostChild : n;
    n->RightmostChild = n->RChild ? static_cast<bst_node *>(n->RChild)->RightmostChild : n;
}

void bst_p::rotate_right(binary_tree_node *n)
{
    binary_tree_node *parent(n->Parent);
    if(parent)
    {
        if(parent->LChild == n)
            parent->LChild = n->LChild;
        else
            parent->RChild = n->LChild;
    }
    n->LChild->Parent = parent;

    binary_tree_node *tmp(n->LChild->RChild);
    n->Parent = n->LChild;
    n->LChild->RChild = n;
    n->LChild = tmp;
    if(tmp)
        tmp->Parent = n;

    // Have to refresh the node we just rotated, the other one will be refreshed
    //  automatically when we walk up the tree
    refresh_node_state(static_cast<bst_node *>(n));
}

void bst_p::rotate_left(binary_tree_node *n)
{
    binary_tree_node *parent(n->Parent);
    if(parent)
    {
        if(parent->LChild == n)
            parent->LChild = n->RChild;
        else
            parent->RChild = n->RChild;
    }
    n->RChild->Parent = parent;

    binary_tree_node *tmp(n->RChild->LChild);
    n->Parent = n->RChild;
    n->RChild->LChild = n;
    n->RChild = tmp;
    if(tmp)
        tmp->Parent = n;

    // Have to refresh the node we just rotated, the other one will be refreshed
    //  automatically when we walk up the tree
    refresh_node_state(static_cast<bst_node *>(n));
}

void bst_p::rebalance(binary_tree_node *n)
{
    int height_difference = static_cast<bst_node *>(n)->HeightDifference();
    if(height_difference > 1)
    {
        // The node is left-heavy

        // Check if it's LR imbalance so we can resolve that first.
        if(static_cast<bst_node *>(n->LChild)->HeightDifference() < 0)
            rotate_left(static_cast<bst_node *>(n->LChild));

        // Now that the LR imbalance is fixed, do the LL rebalance.
        rotate_right(n);
    }
    else if(height_difference < -1)
    {
        // The node is right-heavy

        // Check if it's RL imbalance so we can resolve that first.
        if(static_cast<bst_node *>(n->RChild)->HeightDifference() > 0)
            rotate_right(n->RChild);

        // Now that the RL imbalance is fixed, do the RR rebalance.
        rotate_left(n);
    }
}

bst_p::bst_node *bst_p::const_iterator::operator->()
{
    return current;
}

const bst_p::bst_node *bst_p::const_iterator::operator->() const
{
    return current;
}
const bst_p::bst_node &bst_p::const_iterator::operator *() const
{
    return *current;
}
bst_p::bst_node &bst_p::const_iterator::operator *()
{
    return *current;
}