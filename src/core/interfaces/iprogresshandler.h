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

#ifndef GUTIL_IPROGRESSHANDLER_H
#define GUTIL_IPROGRESSHANDLER_H

#include "gutil_macros.h"

NAMESPACE_GUTIL;


/** An interface that allows you to handle progress updates, and also cancel the operation. */
class IProgressHandler
{
public:

    /** Called periodically with an updated progress value between 0
     *  (started) and 100 (finished).
     *
     *  Implementations of this interface shall update with a value of 0
     *  when the operation starts, and 100 when the operation finishes successfully.
    */
    virtual void ProgressUpdated(int) = 0;

    /** Called periodically to ask if the operation should be cancelled.
     *  Return true if you want to cancel.
    */
    virtual bool CancelOperation(){ return false; }

};


END_NAMESPACE_GUTIL;

#endif // GUTIL_IPROGRESSHANDLER_H
