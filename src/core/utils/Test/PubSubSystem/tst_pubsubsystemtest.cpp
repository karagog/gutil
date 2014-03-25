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
#include "gutil_pubsubsystem.h"
#include "gutil_console.h"
#include "gutil_strings.h"
USING_NAMESPACE_GUTIL;

class PubSubSystemTest : public QObject
{
    Q_OBJECT

public:
    PubSubSystemTest();

private Q_SLOTS:
    void test_basics();
};

PubSubSystemTest::PubSubSystemTest()
{
}


class TestPubSub : public PubSubSystem
{
    DECLARE_TOPIC2(JustGotPaid, int)
    DECLARE_TOPIC(AnybodyListening)
};

class TestSubscriber :
        public TestPubSub::JustGotPaidHandler,
        public PubSubSystem::ExceptionHandler,
        public TestPubSub::AnybodyListeningHandler
{
public:
    void HandleJustGotPaid(int dollars){
        MyMoney += dollars;
        Console::WriteLine(String::Format("Chaching!!   %d", MyMoney));
    }
    void HandleException(const Exception<> &ex){
        Console::WriteLine(String::Format("%s: %s", ex.What, ex.GetMessage()));
    }

    void HandleAnybodyListening(){
        Console::WriteLine("Yes, I'm listening");
        AnsweredAnybodyListening = true;
    }

    static int MyMoney;
    static bool AnsweredAnybodyListening;

};

int TestSubscriber::MyMoney = 0;
bool TestSubscriber::AnsweredAnybodyListening = false;


void PubSubSystemTest::test_basics()
{
    TestSubscriber sub;
    TestPubSub pub;
    pub.SubscribeExceptionHandler(&sub);

    // Register it twice; should get called twice
    pub.SubscribeJustGotPaidHandler(&sub);
    pub.SubscribeJustGotPaidHandler(&sub);

    pub.PublishJustGotPaid(100);

    NotImplementedException<false> ex("Ouch!");
    pub.PublishException(ex);


    // Nothing happens; nobody's listening
    pub.PublishAnybodyListening();
    QVERIFY(TestSubscriber::AnsweredAnybodyListening == false);

    pub.SubscribeAnybodyListeningHandler(&sub);
    pub.PublishAnybodyListening();
    QVERIFY(TestSubscriber::AnsweredAnybodyListening == true);
}

QTEST_APPLESS_MAIN(PubSubSystemTest);

#include "tst_pubsubsystemtest.moc"
