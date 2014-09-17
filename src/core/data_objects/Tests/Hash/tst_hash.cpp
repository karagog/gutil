/*Copyright 2010-2012 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include <QtCore/QString>
#include <QtTest/QtTest>
#include "gutil_hash.h"
#include "gutil_strings.h"
#include "gutil_id.h"
#include <set>
#include <ctime>
USING_NAMESPACE_GUTIL;
using namespace std;

class HashTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void test_basics();
    void test_collisions();
    void test_performance();
};

void HashTest::test_basics()
{
    Hash h;
    qDebug(QString("The hash object is %1 bytes large").arg(sizeof(h)).toAscii());
    QVERIFY(4 == h.Size);
    QVERIFY(Hash::Size == h.Size);
    
    // The null hash is 0
    QVERIFY(h.Final() == 0);
    QVERIFY(Hash::ComputeHash(NULL, 0) == 0);
    
    {
        // Make sure hashes of all single bytes are unique
        set<GUINT32> hashes;
        for(int i = 0; i < 0x100; ++i){
            byte tmp = i;
            GUINT32 hash = Hash::ComputeHash(&tmp, 1);
            QVERIFY(hashes.find(hash) == hashes.end());
            hashes.insert(hash);
        }
        
        // Make sure hashing the same input gives us the same output
        for(int i = 0; i < 0x100; ++i){
            byte tmp = i;
            GUINT32 hash = Hash::ComputeHash(&tmp, 1);
            QVERIFY(hashes.find(hash) != hashes.end());
        }
    }
    
    // Hashes one way are not the same as hashes the other way
    {
        set<GUINT32> hashes;
        for(byte c = 'a'; c <= 'z'; ++c){
            for(byte c2 = c + 1; c2 <= 'z'; ++c2){
                h.AddData(&c, 1);
                h.AddData(&c2, 1);
                GUINT32 hash1 = h.Final();
                
                h.AddData(&c2, 1);
                h.AddData(&c, 1);
                GUINT32 hash2 = h.Final();
                
                QVERIFY(hash1 != hash2);
                QVERIFY(hashes.find(hash1) == hashes.end());
                QVERIFY(hashes.find(hash2) == hashes.end());
                hashes.insert(hash1);
                hashes.insert(hash2);
            }
        }
    }
    
    // Hashes of repeat characters don't repeat hashes
    {
        set<GUINT32> hashes;
        for(int i = 1; i < 10000; i++){
            String s('a', i);
            GUINT32 hash = Hash::ComputeHash((byte const *)s.ConstData(), i);
            
            QVERIFY2(hashes.find(hash) == hashes.end(), QString("%1: %2").arg(i).arg(hash, 8, 16).toAscii());
            hashes.insert(hash);
        }
    }
}

void HashTest::test_collisions()
{
    // Do a collision check. It should be practically impossible to get a collision
    double my_avg = 0;
    const int ITERATION_COUNT = 25;
    for(int i = 0; i < ITERATION_COUNT; ++i)
    {
        //qDebug("Iteration %d", i);
        set<GUINT32> hashes;
        GUINT32 cnt = 0;
        bool collision = false;
        Hash hf;
        forever{
            cnt++;
            Id<16> id(true);
            hf.AddData(id.ConstData(), id.Size);
            GUINT32 hash = hf.Final();
            collision = hashes.find(hash) != hashes.end();
            if(!collision)
                hashes.insert(hash);
            else
                break;
        }
        
        //qDebug(QString("Did %1 hashes before a collision").arg(cnt).toAscii());
        //QVERIFY(cnt > 10000);
        my_avg += cnt;
    }
    my_avg /= ITERATION_COUNT;
    qDebug("My average number of hashes before collision was: %f", my_avg);
    QVERIFY(my_avg > 50000);
    
    // Compare our results with the standard hash
    double their_avg = 0;
    for(int i = 0; i < ITERATION_COUNT; ++i)
    {
        set<GUINT32> hashes;
        GUINT32 cnt = 0;
        bool collision = false;
        std::hash<string> sh;
        forever{
            cnt++;
            Id<16> id(true);
            string s((char const *)id.ConstData(), id.Size);
            GUINT32 hash = sh(s);
            collision = hashes.find(hash) != hashes.end();
            if(!collision)
                hashes.insert(hash);
            else
                break;
        }
        their_avg += cnt;
        //qDebug(QString("Standard hash did %1 hashes before a collision").arg(cnt).toAscii());
    }
    their_avg /= ITERATION_COUNT;
    qDebug("The average number of standard hashes before collision was: %f", their_avg);
}

void HashTest::test_performance()
{
    const int ITERATION_COUNT = 1000000;
    {
        Hash hf;
        clock_t t = clock();
        for(int i = 0; i < ITERATION_COUNT; ++i)
        {
            Id<16> id(true);
            hf.AddData(id.ConstData(), id.Size);
            GUINT32 hash = hf.Final();
            GUTIL_UNUSED(hash);
        }
        clock_t e = clock();
        qDebug("Number of ticks to do %d hashes was: %d", ITERATION_COUNT, (int)(e - t));
    }
    
    {
        std::hash<string> hf;
        clock_t t = clock();
        for(int i = 0; i < ITERATION_COUNT; ++i)
        {
            Id<16> id(true);
            string s((char const *)id.ConstData(), id.Size);
            GUINT32 hash = hf(s);
            GUTIL_UNUSED(hash);
        }
        clock_t e = clock();
        qDebug("Number of ticks to do %d standard hashes was: %d", ITERATION_COUNT, (int)(e - t));
    }
}


QTEST_APPLESS_MAIN(HashTest);

#include "tst_hash.moc"
