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

#ifndef GUTIL_H
#define GUTIL_H

/** \file
    An all-inclusive, catch-all, include that will cover all your GUtil needs.

    If you #include this, you shouldn't need to include any other gutil header.

    You may have to link with GUtilCore and/or GUtil if you use it.

*/

#include "gutil_core.h"

#include "Qt/Utils/utils.h"
#include "Qt/Ui/ui.h"
#include "Qt/ModelViews/modelviews.h"
#include "Qt/Logging/logging.h"
#include "Qt/Interfaces/interfaces.h"
#include "Qt/DataObjects/dataobjects.h"
#include "Qt/DataModels/datamodels.h"
#include "Qt/DataAccess/dataaccess.h"
#include "Qt/Custom/custom.h"
#include "Qt/Controls/controls.h"
#include "Qt/BusinessObjects/businessobjects.h"









// Miscellaneous documentation
/**

    \namespace GUtil Encapsulates George's Utility Library.

    Any time you make use of GUtil you will be using something in this namespace.

    Classes and functions in the root GUtil namespace don't, by convention, depend on
    anything outside of the GUtilCore library.  If there is a class which depends on Qt,
    you should find it in GUtil::QT root.  If a class depends on CryptoPP, you should find it
    in the GUtil::CryptoPP root.

    The GUtil namespace is divided into sub-namespaces, organizing classes/functions into
    functional groups.  See the documentation for any of the namespaces for a
    deeper explanation of the types of classes or code they contain.


    \namespace GUtil::DataAccess Classes used for input/output.


    \namespace GUtil::DataObjects Classes to hold and represent data structures.

    (i.e. container classes like List or Map)


    \namespace GUtil::Interfaces Holds interface classes.

    The GUtil library does not have a strict interface convention.  Interfaces
    may or may not have pure virtual functions, although most do.  Interfaces
    may also contain some small implementation component, which is forbidden in some
    interface conventions.

    The rules I have for Interfaces:
    -# They must be as simple as possible, but no simpler!
            Don't have too many members to the interface.  Interfaces are supposed to be
            simplified abstractions, not over-engineered solutions.
    -# Interface classes begin with a capital I (IComparer, for example), and the header
            also starts with an 'i'.
    -# They must have dynamic type info (must have at least one virtual function)
    -# They must have a virtual destructor, to allow its superclass to be deleted by the interface.

    As long as you comply with these simple interface rules, then your class can qualify
    to be in the Interfaces namespace.

    \note Just because a class satisfies all of these conditions, it does NOT mean that it
    has to be in the Interfaces namespace.  i.e. There are interfaces existing outside this namespace.


    \namespace GUtil::Logging The GUtil logging module.

    Contains classes which enable logging functionality.


    \namespace GUtil::Utils Miscellaneous utility classes


    \namespace GUtil::QT GUtil's Qt-dependent classes and functions.

    This namespace is divided similarly with the base GUtil namespace, into modules
    like DataObjects, BusinessObjects, etc...  The only difference is that classes
    in the QT root are by definition dependent on Qt.


    \namespace GUtil::QT::Interfaces Interface module which depends on Qt.

    \sa GUtil::Interfaces


    \namespace GUtil::QT::Utils Miscellaneous utility classes, depending on Qt.


    \namespace GUtil::QT::Custom Classes that customize Qt classes.

    They provide extra functionality that I want and the Qt team won't implement/haven't
    yet implemented.


    \namespace GUtil::QT::UI User Interface widgets.

    Custom user-interfaces which will be useful for many applications.  Keep these
    interfaces simple; this module is designed for superficial interfaces.

    Ex. An about window, which only needs the text content customized.


    \namespace GUtil::QT::DataObjects Classes containing/representing to data.


    \namespace GUtil::QT::DataModels Data model classes that work with Qt's model-view framework.


    \namespace GUtil::QT::ModelViews Model view classes that work with Qt's model-view framework.


    \namespace GUtil::QT::DataAccess Classes relating to data transfer.


    \namespace GUtil::QT::Logging Logging framework, derived from the core functionality,

    that depends on some Qt classes.


    \namespace GUtil::QT::BusinessObjects Higher-level classes built atop the other libraries,
    which provide advanced functionality.


    \namespace GUtil::QT::Controls UI controls built with Qt's widget system.


    \namespace GUtil::QT::Images A library to contain common icons and image resources.

    Nothing is built into this library by default, you must add the resources to the .qrc file
    if you want to use them, that way you optimize the dll size and the GUtil build speed.

    You must use the Q_INIT_RESOURCES() macro when you want to use this library, with the
    argument being the qrc file name, without the .qrc appended.


    \namespace GUtil::Test Classes useful for testing applications and implementations.

    It links with Qt's Test library.  Normal applications don't need this library, it is just
    for testing situations.


    \namespace GUtil::CryptoPP Classes built upon the Crypto++ api

*/


#endif // GUTIL_H
