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


/** Represents a generic binary tree node.
    It can be used to represent any binary tree structure, in which each node has two children,
    and it also keeps track of its parent node.
*/
class binary_tree_node
{
public:
    /** A reference to the parent of this node. */
    binary_tree_node *Parent;

    /** A reference to the left child of this node. */
    binary_tree_node *LChild;

    /** A reference to the right child of this node. */
    binary_tree_node *RChild;

    /** A pointer to the data held by this node.
        You can reinterpret it as whatever the hell you want, I'm just going to keep track of it.
    */
    void *Data;

    /** Constructs a null node with all pointers zeroed. */
    inline binary_tree_node()
        :Parent(0),
          LChild(0),
          RChild(0),
          Data(0)
    {}

    /** We have to be able to delete our children which may be subclassed. */
    virtual ~binary_tree_node();


    /** Gives a way to specify the side of a node. */
    enum SideEnum
    {
        /** Represents a null value */
        NoSide,
        /** The left child of a node. */
        LeftSide,
        /** The right child of a node. */
        RightSide
    };

    /** Returns which side of my parent I'm on.  Merely a convenience function. */
    SideEnum SideOfParent() const;

};



GUTIL_END_CORE_NAMESPACE

#endif // BST_NODE_H
