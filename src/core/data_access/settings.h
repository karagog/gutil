/*Copyright 2014-2015 George Karagoulis

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

#include <gutil/string.h>
#include <unordered_map>
#include <mutex>
#include <condition_variable>
#include <thread>

namespace GUtil{
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
    ~Settings();

    /** Returns the file name. */
    const String &FileName() const{ return m_fileName; }

    /** Returns true if the cache is dirty and needs to be written to disk. */
    bool IsDirty();

    /** Writes cached changes to disk. */
    void CommitChanges();

    /** Throws away changes and goes back to the state at last commit. */
    void RejectChanges();

    /** Loads the settings from the file, overwriting what's in the cache. */
    void Reload();

    /** Returns the keys that are in the settings. */
    StringList Keys();

    /** Returns true if the key is in the settings. */
    bool Contains(const String &key);

    /** Removes the key from the settings. */
    void Remove(const String &key);

    /** Returns the data for the given key.
     *  \returns The data for the key. If the key is not in the settings, it returns
     *  a null string. The return could also be an empty string, if the data was empty.
    */
    String GetValue(const String &key);

    /** Returns the list of values for the given keys. Use this to avoid locking and
     *  relocking a bunch of times if you need lots of values.
    */
    StringList GetValues(const StringList &keys);

    /** Returns all the key-value mappints */
    std::unordered_map<String, String> AllData();

    void SetValue(const String &key, const String &data);
    void SetValues(std::initializer_list<std::pair<const String, String>>);


protected:

    virtual void on_reloaded(){}
    virtual void on_changes_written(){}


private:

    void _worker_thread();
    void _reload();
    void _commit_changes();

    const String m_fileName;
    std::unordered_map<String, String> m_data;
    std::unordered_map<String, String> m_prevData;
    int m_command;
    bool m_dirty;

    std::mutex m_lock;
    std::condition_variable m_waitCondition;
    std::thread m_worker;
};


}

#endif // GUTIL_SETTINGS_H
