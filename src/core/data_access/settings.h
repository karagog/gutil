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

#ifndef GUTIL_SETTINGS_H
#define GUTIL_SETTINGS_H

#include "gutil_file.h"
#include <unordered_map>
#include <memory>

namespace std{
class mutex;
class condition_variable;
class thread;
}

NAMESPACE_GUTIL;
class String;


/** A class to hold settings data that will persist between runs of your
    program. It uses caching and and does its data access on a background
    thread for optimal response time.
*/
class Settings
{
    GUTIL_DISABLE_COPY(Settings);
public:

    explicit Settings(const char *filename);
    virtual ~Settings();

    /** Returns true if the cache is dirty and needs to be written to disk. */
    bool IsDirty() const;

    /** Writes cached changes to disk. */
    void CommitChanges();

    /** Loads the settings from the file, overwriting what's in the cache. */
    void Reload();


protected:

    virtual void on_changes_committed() {}
    virtual void on_reloaded() {}


private:

    void _worker_thread();
    void _reload();

    File m_file;
    std::unique_ptr<std::mutex> m_lock;
    std::unique_ptr<std::condition_variable> m_waitCondition;
    std::unique_ptr<std::thread> m_worker;
    std::unique_ptr<std::unordered_map<String, String>> m_data;
    int m_command;
    bool m_dirty = false;
};


END_NAMESPACE_GUTIL;

#endif // GUTIL_SETTINGS_H
