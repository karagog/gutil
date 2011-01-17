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

#ifndef ASYNCRESPONSE_H
#define ASYNCRESPONSE_H

#include "gutil_macros.h"
#include <QWaitCondition>
#include <QMutex>
#include <QSharedData>

GUTIL_BEGIN_NAMESPACE(Utils);


// An object to facilitate communicating an asynchronous response from one
//  thread to another.  Derive from it to hold more interesting types of responses
class AsyncResponse :
        public QSharedData
{
public:

    enum ResponseTypeEnum
    {
        None,
        Failure,
        Success
    };

    inline AsyncResponse()
        :_p_ResponseType(None)
    {}
    virtual ~AsyncResponse(){}

    PROPERTY(ResponseType, ResponseTypeEnum);

    QMutex Lock;
    QWaitCondition ForResponse;

};


GUTIL_END_NAMESPACE;

#endif // ASYNCRESPONSE_H
