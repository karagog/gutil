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

#ifndef NODECHAIN_H
#define NODECHAIN_H

#include "gutil_macros.h"
#include <iterator>
GUTIL_BEGIN_CORE_NAMESPACE(DataObjects);


/** \file

    Describes a data representation that resembles a linked list, and doubly linked list.
    For use in other data objects which are represented as such.
*/


class node_t;
class bidirectional_node_t;

class node_link
{
public:
    node_link();
    ~node_link();

    node_t *NextNode;
};


class bidirectional_node_link
{
public:
    bidirectional_node_link();
    ~bidirectional_node_link();

    bidirectional_node_t *NextNode;
    bidirectional_node_t *PreviousNode;
};




class node_t :
        public node_link
{
public:
    node_t();

    void *Data;
};


class bidirectional_node_t :
        public bidirectional_node_link
{
public:
    bidirectional_node_t();

    void *Data;
};




/** A base class that knows how to iterate node chains. */
class forward_node_iterator
{
public:
    typedef std::forward_iterator_tag iterator_category;

    forward_node_iterator(node_t *n = 0);

    /** Advances the iterator */
    forward_node_iterator &operator++();

    /** Advances the iterator */
    forward_node_iterator operator++(int);

    /** Advances the iterator the specified number of items */
    forward_node_iterator &operator+=(int);

    /** Returns a copy of the iterator advanced the specified number of times. */
    forward_node_iterator operator+(int);

    bool operator == (const forward_node_iterator &) const;
    bool operator != (const forward_node_iterator &) const;

    /** Be careful when using this. */
    node_t *current;


private:
    void advance();
};


/** A base class that knows how to iterate node chains. */
class bidirectional_node_iterator
{
public:
    typedef std::bidirectional_iterator_tag iterator_category;

    bidirectional_node_iterator(bidirectional_node_t *n = 0);

    /** Advances the iterator */
    bidirectional_node_iterator &operator++();

    /** Advances the iterator */
    bidirectional_node_iterator operator++(int);

    /** Advances the iterator the specified number of items */
    bidirectional_node_iterator &operator+=(int);

    /** Returns a copy of the iterator advanced the specified number of times. */
    bidirectional_node_iterator operator+(int);

    /** Retreats the iterator */
    bidirectional_node_iterator &operator--();

    /** Retreats the iterator */
    bidirectional_node_iterator operator--(int);

    /** Retreats the iterator the specified number of items */
    bidirectional_node_iterator &operator-=(int);

    /** Returns a copy of the iterator retreated the specified number of times. */
    bidirectional_node_iterator operator-(int);

    bool operator == (const bidirectional_node_iterator &) const;
    bool operator != (const bidirectional_node_iterator &) const;

    /** Be careful when using this. */
    bidirectional_node_t *current;


private:
    void advance();
    void retreat();
};





GUTIL_END_CORE_NAMESPACE;

#endif // NODECHAIN_H
