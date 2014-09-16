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

#include "settings.h"
#include "gutil_strings.h"
#include "gutil_globals.h"
#include "gutil_globallogger.h"
#include <mutex>
#include <condition_variable>
#include <thread>
using namespace std;

static const char SETTINGS_FILE_HEADER[] = "# GUtil Persistent Settings";

enum worker_commands
{
    cmd_sleep,
    cmd_reload,
    cmd_write_changes,
    cmd_exit
};

NAMESPACE_GUTIL;


Settings::Settings(const char *filename)
    :m_file(filename),
      m_command(cmd_sleep),
      m_dirty(true)
{
    // Open the file and make sure it's valid
    bool existed = m_file.Exists();
    m_file.Open(File::OpenReadWrite);
    if(existed){
        // Read the header
        GUINT32 header_len = sizeof(SETTINGS_FILE_HEADER) - 1;
        String header = m_file.ReadLine();

        // If the header is unexpected, then we throw here
        if(header.Length() != header_len + 1 ||
            0 != memcmp(SETTINGS_FILE_HEADER, header.ConstData(), header_len))
            THROW_NEW_GUTIL_EXCEPTION2(Exception, "Invalid file header");
    }

    m_lock.reset(new mutex);
    m_waitCondition.reset(new condition_variable);
    m_data.reset(new unordered_map<String, String>);

    // Now start the background thread
    m_worker.reset(new thread(&Settings::_worker_thread, this));
}

Settings::~Settings()
{
    // Take down the background thread
    m_lock->lock();
    m_command = cmd_exit;
    m_lock->unlock();
    m_waitCondition->notify_one();
    m_worker->join();
}

void Settings::Reload()
{
    unique_lock<mutex> lkr(*m_lock);
    m_command = cmd_reload;
    m_waitCondition->notify_one();
}

void Settings::_worker_thread()
{
    unique_lock<mutex> lkr(*m_lock);
    Forever([&]{
        // Wait for something to do
        m_waitCondition->wait(lkr, [this]{ return m_command != cmd_sleep; });
        try{
            switch((worker_commands)m_command)
            {
            case cmd_reload:
                _reload();
                break;
            default:
                break;
            }
        } catch(const exception &ex) {
            // Trap all exceptions; do not let the worker crash our program!
            GlobalLogger::LogException(ex);
        }
        return m_command != cmd_exit;
    });
}

void Settings::_reload()
{

}


END_NAMESPACE_GUTIL;
