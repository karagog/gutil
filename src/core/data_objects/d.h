/*Copyright 2014 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef GUTIL_D_H
#define GUTIL_D_H

/** \file This file describes a mechanism for hiding your class' internal data structure.  The only
 *  thing that is needed here is one #define, but I will describe how to use this because it is so useful.
 *
 *  The basic concept is that your class only has one member, which is a void pointer (void *),
 *  and in your implementation you cast it as a type that is declared in your .cpp file so nobody
 *  using your class cares about the structure of your data.
 *
 *  Here is a quick example (and note which names you need to copy in order to use this implementation):
 *
 * foo.h:
 *
 *  class foo
 *  {
 *  // A pointer to your class' data (nobody using your class knows the structure)
 *  //  Note: The name must match
 *  void *d;
 *
 *  void func();
 *  };
 *
 *
 * foo.cpp:
 *
 *  #include "gutil_d.h"
 *
 *  // Your class' data type (the name must match, but of course the class members are up to you)
 *  struct d_t
 *  {
 *    int data1;
 *    char *data2;
 *  };
 *
 *  void foo::func()
 *  {
 *    // Instantiates a temporary variable called "d"
 *    G_D;
 *
 *    // Now you can refer to your data struct as d
 *    d->data1 = 0;
 *    d->data2 = 0;
 *  }
*/



#ifdef G_D
    #undef G_D
#endif


/** Instantiates a temporary variable called "d" that is cast as your class internal data type. */
#define G_D  d_t *d = reinterpret_cast<d_t *>(this->d)


/** You must call this in your constructor to initialize your class data. */
#define G_D_INIT()      this->d = new d_t


/** You must call this in your destructor to take down your class data. */
#define G_D_UNINIT()    delete reinterpret_cast<d_t *>(this->d)


#endif // GUTIL_D_H
