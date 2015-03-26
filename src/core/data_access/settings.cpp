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

#include "settings.h"
#include <gutil/globals.h>
#include <gutil/globallogger.h>
#include <gutil/hash.h>
#include <gutil/file.h>
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
    unique_lock<mutex> lkr(m_lock);
    m_waitCondition.wait(lkr, [&]{ return m_command == cmd_sleep; });

    // Write any changes if we're still dirty
    if(m_dirty){
        m_command = cmd_write_changes;
        m_waitCondition.notify_one();
        m_waitCondition.wait(lkr, [&]{ return m_command == cmd_sleep; });
    }

    m_command = cmd_exit;
    m_lock.unlock();
    m_waitCondition.notify_one();
    m_worker.join();
}

bool Settings::IsDirty()
{
    unique_lock<mutex> lkr(m_lock);
    return m_dirty && m_command == cmd_sleep;
}

void Settings::Reload()
{
    unique_lock<mutex> lkr(m_lock);

    // One command at a time; block until worker is idle
    m_waitCondition.wait(lkr, [this]{ return m_command == cmd_sleep; });
    m_command = cmd_reload;
    m_waitCondition.notify_one();
}

void Settings::CommitChanges()
{
    unique_lock<mutex> lkr(m_lock);

    // One command at a time; block until worker is idle
    m_waitCondition.wait(lkr, [this]{ return m_command == cmd_sleep; });
    m_command = cmd_write_changes;
    m_prevData = m_data;
    m_waitCondition.notify_one();
}

void Settings::RejectChanges()
{
    unique_lock<mutex> lkr(m_lock);
    m_data = m_prevData;
    m_dirty = false;
}

void Settings::SetValue(const String &key, const String &data)
{
    unique_lock<mutex> lkr(m_lock);
    m_data[key] = data;
    m_dirty = true;
}

void Settings::SetValues(std::initializer_list<std::pair<const String, String>> il)
{
    unique_lock<mutex> lkr(m_lock);
    m_data.insert(il);
    m_dirty = true;
}

bool Settings::Contains(const String &key)
{
    unique_lock<mutex> lkr(m_lock);
    // If we're reloading we'll wait for the update
    m_waitCondition.wait(lkr, [this]{ return m_command != cmd_reload; });
    return m_data.find(key) != m_data.end();
}

void Settings::Remove(const String &key)
{
    unique_lock<mutex> lkr(m_lock);
    // If we're reloading we'll wait for the update
    m_waitCondition.wait(lkr, [this]{ return m_command != cmd_reload; });
    auto i = m_data.find(key);
    if(i != m_data.end()){
        m_data.erase(i);
        m_dirty = true;
    }
}

String Settings::GetValue(const String &key)
{
    String ret;
    unique_lock<mutex> lkr(m_lock);
    // If we're reloading we'll wait for the update
    m_waitCondition.wait(lkr, [this]{ return m_command != cmd_reload; });
    auto i = m_data.find(key);
    if(i != m_data.end())
        ret = i->second;
    return ret;
}

StringList Settings::GetValues(const StringList &keys)
{
    StringList ret;
    unique_lock<mutex> lkr(m_lock);
    // If we're reloading we'll wait for the update
    m_waitCondition.wait(lkr, [this]{ return m_command != cmd_reload; });
    for(auto k : keys){
        auto i = m_data.find(k);
        if(i == m_data.end())
            ret.Append(String());
        else
            ret.Append(i->second);
    }
    return ret;
}

unordered_map<String, String> Settings::AllData()
{
    unique_lock<mutex> lkr(m_lock);
    // If we're reloading we'll wait for the update
    m_waitCondition.wait(lkr, [this]{ return m_command != cmd_reload; });
    return m_data;
}

StringList Settings::Keys()
{
    StringList ret;
    unique_lock<mutex> lkr(m_lock);
    // If we're reloading we'll wait for the update
    m_waitCondition.wait(lkr, [this]{ return m_command != cmd_reload; });
    for(auto k : m_data)
        ret.Append(k.first);
    return ret;
}

void Settings::_worker_thread()
{
    unique_lock<mutex> lkr(m_lock);
    while(1){
        // Wait for something to do
        m_waitCondition.wait(lkr, [&]{
            return m_command != cmd_sleep;
        });

        // We can break here if told to quit
        if(m_command == cmd_exit)
            break;

        try{
            switch((worker_commands)m_command)
            {
            case cmd_reload:
                _reload();
                m_prevData = m_data;
                on_reloaded();
                break;
            case cmd_write_changes:
                _commit_changes();
                on_changes_written();
                break;
            default:
                break;
            }
        } catch(const exception &ex) {
            GlobalLogger().LogException(ex);
        }
    }
}

void Settings::_reload()
{
    // Unlock while we read the file
    m_lock.unlock();
    unordered_map<String, String> newdata;

    // Be sure to reclaim the lock at the end
    finally([&]{
        m_lock.lock();
        m_data = newdata;
        m_dirty = false;
        m_command = cmd_sleep;
        m_waitCondition.notify_all();
    });

    // Open the file and make sure it's valid
    if(!File::Exists(m_fileName))
        return;

    // Read the header
    File f(m_fileName, File::OpenRead);
    const GUINT32 header_len = sizeof(SETTINGS_FILE_HEADER) - 1;
    String header = f.ReadLine(header_len);

    if(header.Length() != header_len ||
            0 != memcmp(SETTINGS_FILE_HEADER, header.ConstData(), header_len))
        throw Exception<>("Invalid file header");

    byte purported_hash[4];
    if(sizeof(purported_hash) != f.Read(purported_hash, sizeof(purported_hash), sizeof(purported_hash)))
        throw Exception<>("Invalid file header");

    // Now read in the file line by line and populate our data
    Hash hf;
    String line = f.ReadLine().Trimmed();
    while(!line.IsNull())
    {
        // Skip empty lines
        if(!line.IsEmpty()){
            StringList sl = line.Split(':');
            if(sl.Length() != 2)
                throw Exception<>("Invalid line");

            // Convert from base64
            sl[0] = String::FromBase64(sl[0].Trimmed());
            sl[1] = String::FromBase64(sl[1].Trimmed());

            // Add the data to our checksum
            hf.AddData((byte const *)sl[0].ConstData(), sl[0].Length());
            hf.AddData((byte const *)sl[1].ConstData(), sl[1].Length());

            // Add the data to our data objects
            newdata.emplace(sl[0], sl[1]);
        }

        // Read the next line
        line = f.ReadLine().Trimmed();
    }

    byte calculated_hash[4];
    hf.Final(calculated_hash);
    if(0 != memcmp(purported_hash, calculated_hash, sizeof(calculated_hash)))
        throw Exception<>("Checksum mismatch. Was the settings file corrupted?");
}

void Settings::_commit_changes()
{
    // Copy the data so we can release the lock
    unordered_map<String, String> data(m_data);

    // We'll mark ourselves clean because we will immediately commit to disk
    // and this allows the main thread to safely modify the map while we work.
    bool was_dirty = m_dirty;
    m_dirty = false;

    // Release the lock while we write the file
    m_lock.unlock();

    // Be sure to reclaim the lock at the end, and re-mark ourselves dirty if
    //  something fails
    bool restore_dirty = true;
    finally([&]{
        m_lock.lock();
        if(restore_dirty)
            m_dirty = was_dirty || m_dirty; // Could have been marked dirty while working
        m_command = cmd_sleep;
        m_waitCondition.notify_all();
    });

    // Write the header
    File f(m_fileName, File::OpenReadWriteTruncate);
    f.Write(String::Format("%s\n", SETTINGS_FILE_HEADER));

    // Reserve space for the checksum
    int checksum_pos = f.Pos();
    f.Write("word\n");

    // Now write the file line by line, computing the hash as we go
    Hash hf;
    for(const auto &p : data){
        f.Write(p.first.ToBase64());
        f.Write(":");
        f.Write(p.second.ToBase64());
        f.Write("\n");

        // Add the plaintext values to the hash
        hf.AddData((const byte *)p.first.ConstData(), p.first.Length());
        hf.AddData((const byte *)p.second.ConstData(), p.second.Length());
    }

    // Now go back and write the hash
    GUINT32 hash_final = hf.Final();
    byte tb;
    f.Seek(checksum_pos);
    f.Write(&(tb = (hash_final >> 24)), 1);
    f.Write(&(tb = (hash_final >> 16) & 0x0FF), 1);
    f.Write(&(tb = (hash_final >> 8) & 0x0FF), 1);
    f.Write(&(tb = hash_final & 0x0FF), 1);

    // Mission success
    restore_dirty = false;
}


END_NAMESPACE_GUTIL;
