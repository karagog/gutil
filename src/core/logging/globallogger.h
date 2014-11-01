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

#ifndef GUTIL_GLOBALLOGGER_H
#define GUTIL_GLOBALLOGGER_H

#include <gutil/ilog.h>

/** \file    

    This file declares the interface to the global logging facility. The global logger is
    accessible everywhere in your program, so code at every level is able to
    log messages to the same place without caring about who is actually doing the logging.
    
    By default, logging to GlobalLogger() does nothing until you set it with SetGlobalLogger().
    
    Whichever logger you set will be protected and owned by a QueuedLogger, so even background threads will be
    able to safely log without blocking your frontend code.
        
    In the current API there is no way to know if a
    global logger was set, because ideally nobody needs to care. Everyone just logs to it and trusts that there
    is a device somewhere doing the logging, but it shouldn't affect them if there isn't.
    
    There can be only one global logger, so if you need to log the same messages to multiple locations, you can
    do this by setting a GroupLogger.
    
    DO NOT SET a QueuedLogger as the global logger, as this would be redundant and hurt performance.
    
    After you set a global logger, it is your responsibility to un-set it at the end of your program. Make sure to do it
    after all potential threads who may log to it have finished.
    
    \note By automatically protecting the global log with a QueuedLogger, this guarantees controlled access throughout your code,
        because you don't know who is using it (could be called from inside a library you know nothing about).
*/

NAMESPACE_GUTIL;


/** Returns the global logger.  Logging to this forwards everything to the logger that was set with SetGlobalLogger(),
    or does nothing if there was no logger set.
    
    Logging calls to this logger are thread-safe (assuming nobody changes the logger mid-flight), 
    and will be queued and processed sequentially on a background thread.
*/
extern ILog &GlobalLogger();

/** Sets the global logger (may be NULL). It will be automatically deleted when you overwrite it with a subsequent call,
    but will NOT be automatically deleted otherwise, due to a deadlocking issue with the std::thread implementation if you
    try to destroy a global thread object during application takedown. Therefore if you forget to clean up after it, you will
    have a minor memory leak, but your application won't hang at the end.

    This function is NOT thread safe, so you should only use it once at the beginning of your program, before any threads
    have started up, and at the end after all threads have finished.
*/
extern void SetGlobalLogger(ILog *);


END_NAMESPACE_GUTIL;

#endif // GUTIL_GLOBALLOGGER_H
