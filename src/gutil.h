/*Copyright 2010-2013 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef GUTIL_H
#define GUTIL_H

/** \file

    Just contains miscellaneous documentation for GUtil
*/


/**

    \namespace GUtil Encapsulates George's Utility Library.

    Any time you make use of GUtil you will be using something in this namespace.

    Classes and functions in the root GUtil namespace don't, by convention, depend on
    anything outside of the GUtilCore library.  If there is a class which depends on Qt,
    you should find it in GUtil::Qt root.  If a class depends on CryptoPP, you should find it
    in the GUtil::CryptoPP root.

    The GUtil namespace is divided into sub-namespaces, organizing classes/functions into
    functional groups.  See the documentation for any of the namespaces for a
    deeper explanation of the types of classes or code they contain.


    \namespace GUtil::Games Classes related to games.

    Dice, cards, etc...  Business objects relating to games, but not dependent on Qt.


    \namespace GUtil::Qt GUtil's Qt-dependent classes and functions.

    This namespace is divided similarly with the base GUtil namespace, into modules
    like DataObjects, BusinessObjects, etc...  The only difference is that classes
    in the QT root are by definition dependent on Qt.


    \namespace GUtil::Test Classes useful for testing applications and implementations.

    It links with Qt's Test library.  Normal applications don't need this library, it is just
    for testing situations.


*/


#endif // GUTIL_H
