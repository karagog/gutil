/*Copyright 2010 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include "Custom/myxmlstreamreader.h"
#include <QTest>
using namespace GUtil::Custom;

class XmlHelpers_test : public QObject
{
Q_OBJECT
public:
    explicit XmlHelpers_test(QObject *parent = 0);

signals:

public slots:

private slots:
    void test_innerXml();
    void test_innerText();

};

XmlHelpers_test::XmlHelpers_test(QObject *parent) :
        QObject(parent)
{
}

void XmlHelpers_test::test_innerXml()
{
    QString xmlstr = "<t><inner a=\"attribute\"/><inner a=\"other\"/></t>";

    // Test a basic xml string
    {
        myXmlStreamReader rdr(xmlstr);

        QVERIFY(rdr.readNextStartElement());

        QString res = rdr.InnerXml();

        if(res != xmlstr)
        {
            qDebug(("Before: " + xmlstr).toStdString().c_str());
            qDebug(("After: " + res).toStdString().c_str());
        }

        QVERIFY(res == xmlstr);

        // Make sure the reader is pointing at the corresponding end element for the
        //  node we just read
        QVERIFY(rdr.tokenType() == myXmlStreamReader::EndElement);
        QVERIFY(rdr.name().toString() == "t");
    }

    // Test one of the inner elements of the tree
    {
        myXmlStreamReader rdr(xmlstr);

        QVERIFY(rdr.readNextStartElement());
        QVERIFY(rdr.readNextStartElement());

        QString res = rdr.InnerXml();

        QVERIFY(res == "<inner a=\"attribute\"/>");
        QVERIFY(rdr.tokenType() == myXmlStreamReader::EndElement);
        QVERIFY(rdr.name().toString() == "inner");
    }

    // Try a heavily nested one
    xmlstr = "<a><aa><aaa>Here is text</aaa><bbb attr=\"test\"><aaaa/></bbb></aa></a>";
    {
        myXmlStreamReader rdr(xmlstr);

        QVERIFY(rdr.readNextStartElement());

        QString res = rdr.InnerXml();

        if(res != xmlstr)
        {
            qDebug(("Before: " + xmlstr).toStdString().c_str());
            qDebug(("After: " + res).toStdString().c_str());
        }

        QVERIFY(res == xmlstr);
        QVERIFY(rdr.tokenType() == myXmlStreamReader::EndElement);
        QVERIFY(rdr.name().toString() == "a");
    }


    // Test another case with special characters
    xmlstr = "<a><aa><aaa>Here is&quot;text<aaaa lab=\"&quot;\"></aaaa></aaa>"
             "<bbb attr=\"test&amp;\"><aaaa/></bbb></aa></a>";
    {
        myXmlStreamReader rdr(xmlstr);

        QVERIFY(rdr.readNextStartElement());
        QVERIFY(rdr.readNextStartElement());
        //QVERIFY(rdr.readNextStartElement());

        QString res = rdr.InnerXml();

        QVERIFY(res == "<aa><aaa>Here is&quot;text<aaaa lab=\"&quot;\"/></aaa>"
                "<bbb attr=\"test&amp;\"><aaaa/></bbb></aa>");
        QVERIFY(rdr.tokenType() == myXmlStreamReader::EndElement);
        QVERIFY(rdr.name().toString() == "aa");

        myXmlStreamReader rdr2(res);
        rdr2.readNextStartElement();
        while(rdr2.readNext() != myXmlStreamReader::Characters)
        {}

        // Make sure it translates the escaped characters correctly
        QVERIFY(rdr2.text() == "Here is\"text");
    }
}

void XmlHelpers_test::test_innerText()
{
    QString xmlstr = "<t><inner a=\"attribute\"/><inner a=\"other\"/></t>";

    // Test a basic xml string
    {
        myXmlStreamReader rdr(xmlstr);

        QVERIFY(rdr.readNextStartElement());

        QString res = rdr.InnerText();

//        qDebug(("Before: " + xmlstr).toStdString().c_str());
//        qDebug(("After: " + res).toStdString().c_str());

        QVERIFY(res == "<inner a=\"attribute\"/><inner a=\"other\"/>");

        // Make sure the reader is pointing at the corresponding end element for the
        //  node we just read
        QVERIFY(rdr.tokenType() == myXmlStreamReader::EndElement);
        QVERIFY(rdr.name().toString() == "t");
    }

    // Test one of the inner elements of the tree
    {
        myXmlStreamReader rdr(xmlstr);

        QVERIFY(rdr.readNextStartElement());
        QVERIFY(rdr.readNextStartElement());

        QString res = rdr.InnerText();

        QVERIFY(res == "");
        QVERIFY(rdr.tokenType() == myXmlStreamReader::EndElement);
        QVERIFY(rdr.name().toString() == "inner");
    }

    // Try a heavily nested one
    xmlstr = "<a><aa><aaa>Here is text</aaa><bbb attr=\"test\"><aaaa/></bbb></aa></a>";
    {
        myXmlStreamReader rdr(xmlstr);

        QVERIFY(rdr.readNextStartElement());

        QString res = rdr.InnerText();

//        qDebug(("Before: " + xmlstr).toStdString().c_str());
//        qDebug(("After: " + res).toStdString().c_str());

        QVERIFY(res == "<aa><aaa>Here is text</aaa><bbb attr=\"test\"><aaaa/></bbb></aa>");
        QVERIFY(rdr.tokenType() == myXmlStreamReader::EndElement);
        QVERIFY(rdr.name().toString() == "a");
    }
}

QTEST_APPLESS_MAIN(XmlHelpers_test);

#include "xmlhelpers_test.moc"
