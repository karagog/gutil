/*Copyright 2012 George Karagoulis
  
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0
    
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include "gutil_undostack.h"
#include <QtCore/QString>
#include <QtTest/QtTest>
USING_NAMESPACE_GUTIL;


static int A;
static int B;
static int C;
static int D;


/** Declares classes that increment the static variables */
#define DECLARE_UNDOABLE_TEST_CLASS(STR) \
    class Up ## STR : public IUndoableAction \
    { \
    public: \
        void Do(){ STR++; } \
        void Undo(){ STR--; } \
        void Redo(){ Do(); } \
    }

DECLARE_UNDOABLE_TEST_CLASS(A);
DECLARE_UNDOABLE_TEST_CLASS(B);
DECLARE_UNDOABLE_TEST_CLASS(C);
DECLARE_UNDOABLE_TEST_CLASS(D);




class UndoStackTest : public QObject
{
    Q_OBJECT
    
public:
    UndoStackTest();
    
private Q_SLOTS:

    void test_doing();
    void test_undoing();
    void test_redoing();
    void test_undo_redo();
    void test_macro();


private:

    UndoStack undo_stack;

    inline void _clear_statics(){
        A = 0;
        B = 0;
        C = 0;
        D = 0;
    }

};


class UpA;
class UpB;
class UpC;
class UpD;


UndoStackTest::UndoStackTest()
{
}

void UndoStackTest::test_doing()
{
    _clear_statics();

    undo_stack.Do(new UpA);
    QVERIFY(1 == A);

    undo_stack.Do(new UpB);
    QVERIFY(1 == B);

    undo_stack.Do(new UpC);
    QVERIFY(1 == C);

    undo_stack.Do(new UpD);
    QVERIFY(1 == D);
}

void UndoStackTest::test_undoing()
{
    A = 1;
    B = 1;
    C = 1;
    D = 1;

    undo_stack.Undo();
    QVERIFY(0 == D);

    undo_stack.Undo();
    QVERIFY(0 == C);

    undo_stack.Undo();
    QVERIFY(0 == B);

    undo_stack.Undo();
    QVERIFY(0 == A);
}

void UndoStackTest::test_redoing()
{
    _clear_statics();

    undo_stack.Redo();
    QVERIFY(1 == A);

    undo_stack.Redo();
    QVERIFY(1 == B);

    undo_stack.Redo();
    QVERIFY(1 == C);

    undo_stack.Redo();
    QVERIFY(1 == D);
}

void UndoStackTest::test_undo_redo()
{
    _clear_statics();
    undo_stack.Clear();

    // Try undoing a command, and doing something over it.
    undo_stack.Do(new UpA);
    undo_stack.Do(new UpB);
    undo_stack.Do(new UpC);

    undo_stack.Undo();

    undo_stack.Do(new UpD);

    QVERIFY(1 == A);
    QVERIFY(1 == B);
    QVERIFY(0 == C);
    QVERIFY(1 == D);
}

void UndoStackTest::test_macro()
{
    _clear_statics();
    undo_stack.Clear();

    // Test rolling back a macro
    undo_stack.BeginMacro();
    {
        undo_stack.Do(new UpA);
        QVERIFY(1 == A);

        undo_stack.Do(new UpB);
        QVERIFY(1 == B);

        undo_stack.Do(new UpC);
        QVERIFY(1 == C);

        undo_stack.Do(new UpD);
        QVERIFY(1 == D);
    }
    undo_stack.RollbackMacro();

    QVERIFY(0 == A);
    QVERIFY(0 == B);
    QVERIFY(0 == C);
    QVERIFY(0 == D);


    // Test committing a macro
    undo_stack.BeginMacro();
    {
        undo_stack.Do(new UpA);
        QVERIFY(1 == A);

        undo_stack.Do(new UpB);
        QVERIFY(1 == B);

        undo_stack.Do(new UpC);
        QVERIFY(1 == C);

        undo_stack.Do(new UpD);
        QVERIFY(1 == D);
    }
    undo_stack.CommitMacro();

    QVERIFY(1 == A);
    QVERIFY(1 == B);
    QVERIFY(1 == C);
    QVERIFY(1 == D);


    // Try undoing the macro
    undo_stack.Undo();

    QVERIFY(0 == A);
    QVERIFY(0 == B);
    QVERIFY(0 == C);
    QVERIFY(0 == D);


    // Try redoing the macro
    undo_stack.Redo();

    QVERIFY(1 == A);
    QVERIFY(1 == B);
    QVERIFY(1 == C);
    QVERIFY(1 == D);
}



QTEST_APPLESS_MAIN(UndoStackTest)

#include "tst_undostacktest.moc"
