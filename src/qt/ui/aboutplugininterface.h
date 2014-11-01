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

#ifndef ABOUTPLUGININTERFACE_H
#define ABOUTPLUGININTERFACE_H

/** \file

    Describes a plugin interface you can use to implement your own about window
    plugin.  You can see the about gutil plugin for an example.
*/

#include <gutil/macros.h>
#include <QtPlugin>

class QWidget;

NAMESPACE_GUTIL1(Qt);


/** The abstract plugin interface for showing and setting up an about window */
class IAboutGUtil
{
public:

    /** Shows the GUtil about window */
    virtual void ShowAboutGUtil(QWidget *parent = 0) = 0;

};


END_NAMESPACE_GUTIL1;

// We have to declare this outside the namespace
Q_DECLARE_INTERFACE(GUtil::Qt::IAboutGUtil, "GUtil.IAboutGUtil")

#endif // ABOUTPLUGININTERFACE_H
