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

#ifndef BST_NODE_H
#define BST_NODE_H

#include "gutil_macros.h"

GUTIL_BEGIN_CORE_NAMESPACE(DataObjects);



/** Gives the BST a way to specify the side of a node. */
enum SideEnum
{
    /** Represents a null value */
    NoSide,

    /** The left child of a node. */
    LeftSide,

    /** The right child of a node. */
    RightSide
};



/** Represents a single node of the binary search tree.

    This class is supposed to be private.  You should not need to use this class
    directly.

    \sa BinarySearchTree
*/
class bst_node
{
public:
    /** Constructs a default node with all pointers set to 0. */
    bst_node();

    /** Deletes a bst_node and all its children. */
    ~bst_node();

    /** A reference to the parent of this node. */
    bst_node *Parent;

    /** A reference to the left child of this node. */
    bst_node *LChild;

    /** A reference to the right child of this node. */
    bst_node *RChild;

    /** A reference to my most leftist child (min of my children).
        This makes it possible to get to the begin() and end() of the tree
        in constant time, and also makes iteration much faster
    */
    bst_node *LeftmostChild;

    /** A reference to my most rightist child (max of my children).
        This makes it possible to get to the begin() and end() of the tree
        in constant time, and also makes iteration much faster
    */
    bst_node *RightmostChild;

    /** The height of this node, which is 1 + the max of the heights of my two children */
    int Height;

    /** The difference in height of my two children (left - right) */
    int HeightDifference() const;

    /** A pointer to the data held by this node. */
    void *Data;

    /** Returns whether the node is balanced, or in other words whether its children are not
        more than 1 node different in height.
    */
    bool Balanced() const;


    /** Returns which side of my parent I'm on. */
    SideEnum SideOfParent() const;

};


GUTIL_END_CORE_NAMESPACE

#endif // BST_NODE_H
