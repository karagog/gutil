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

    /** The height of this ndoe, which is 1 + the max height of my two children */
    int Height;

    /** The difference in height of my two children (left - right) */
    int HeightDifference() const;

    /** A pointer to the data held by this node. */
    void *Data;


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


    /** Inserts a new node under the parent.

        On our level of abstraction, we are unable to know if the new node
        is greater/less than the parent, so we are guided by the SideEnum.

        \param parent The node to be the new node's parent
        \param new_node The new node to insert
        \param side The side of the parent on which to insert the new node.
    */
    static void Insert(bst_node *parent, bst_node *new_node, SideEnum side);


    /** Deletes a node from the tree.

        It expects you to have already found the replacement node.

        \param node The node to delete
        \param replacement The node that will replace the deleted node.
    */
    static void Delete(bst_node *node, bst_node *replacement);


    /** Returns whether the node is balanced, or in other words whether its children are not
        more than 1 node different in height.
    */
    bool Balanced() const;


    /** Returns which side of my parent I'm on. */
    SideEnum SideOfParent() const;


private:

    /** Rebalances a node.  Does nothing if the node is already balanced. */
    static void rebalance(bst_node *);

    static void rotate_right(bst_node *parent);
    static void rotate_left(bst_node *parent);

    static void update_height(bst_node *);

};


GUTIL_END_CORE_NAMESPACE

#endif // BST_NODE_H