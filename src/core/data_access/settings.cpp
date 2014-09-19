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
#include "gutil_globals.h"
#include "gutil_globallogger.h"
#include "gutil_hash.h"
#include "gutil_file.h"
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
    :m_fileName(filename),
      m_command(cmd_reload),
      m_dirty(true),
      m_worker(&Settings::_worker_thread, this)     // Start the background worker
{}

Settings::~Settings()
{
    // Take down the background thread
    m_lock.lock();
    m_command = cmd_exit;
    m_lock.unlock();
    m_waitCondition.notify_one();
    m_worker.join();
}

void Settings::Reload()
{
    unique_lock<mutex> lkr(m_lock);
    m_command = cmd_reload;
    m_waitCondition.notify_one();
}

void Settings::_worker_thread()
{
    unique_lock<mutex> lkr(m_lock);
    while(1){
        // Wait for something to do
        worker_commands cur_command = cmd_sleep;
        m_waitCondition.wait(lkr, [&]{
            return (cur_command = (worker_commands)m_command) != cmd_sleep;
        });

        // Unlock while we process the request
        lkr.unlock();

        // We can break here if told to quit
        if(cur_command == cmd_exit)
            break;

        try{
            switch(cur_command)
            {
            case cmd_reload:
                _reload();
                on_reloaded();
                break;
            case cmd_write_changes:
                _commit_changes();
                on_changes_committed();
                break;
            default:
                break;
            }
        } catch(const exception &ex) {
            GlobalLogger().LogException(ex);
        }

        // Reclaim the lock
        m_lock.lock();
    }
}

void Settings::_reload()
{
    // Open the file and make sure it's valid
    File f(m_fileName);
    byte purported_hash[4]{};
    if(!f.Exists())
        return;

    // Read the header
    f.Open(File::OpenRead);
    const GUINT32 header_len = sizeof(SETTINGS_FILE_HEADER) - 1;
    String header = f.ReadLine(header_len);

    if(header.Length() != header_len + 1 ||
        0 != memcmp(SETTINGS_FILE_HEADER, header.ConstData(), header_len))
        THROW_NEW_GUTIL_EXCEPTION2(Exception, "Invalid file header");

    if(sizeof(purported_hash) != f.Read(purported_hash, sizeof(purported_hash), sizeof(purported_hash)))
        THROW_NEW_GUTIL_EXCEPTION2(Exception, "Invalid file header");

    // Now read in the file line by line and populate our data
    Hash hf;
    unordered_map<String, String> newdata;
    String line = f.ReadLine().Trimmed();
    while(!line.IsNull())
    {
        // Skip empty lines
        if(line.IsEmpty())
            continue;

        StringList sl = line.Split(':');
        if(sl.Length() != 2)
            THROW_NEW_GUTIL_EXCEPTION2(Exception, "Invalid line");

        // Convert from base64
        sl[0] = String::FromBase64(sl[0].Trimmed());
        sl[1] = String::FromBase64(sl[1].Trimmed());

        // Add the data to our checksum
        hf.AddData((byte const *)sl[0].ConstData(), sl[0].Length());
        hf.AddData((byte const *)sl[1].ConstData(), sl[1].Length());

        // Add the data to our data objects
        newdata.emplace(sl[0], sl[1]);

        // Read the next line
        line = f.ReadLine().Trimmed();
    }
    f.Close();

    byte calculated_hash[4];
    hf.Final(calculated_hash);
    if(0 != memcmp(purported_hash, calculated_hash, sizeof(calculated_hash)))
        THROW_NEW_GUTIL_EXCEPTION2(Exception, "Checksum mismatch. Was the file corrupted?");

    // Now that we have all the data and the checksum matches, update our data members
    //  with the new data we got
    unique_lock<mutex> lkr(m_lock);
    m_data = newdata;
    m_dirty = false;
    m_command = cmd_sleep;
    m_waitCondition.notify_one();
}

void Settings::_commit_changes()
{

}


END_NAMESPACE_GUTIL;
