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

#ifndef GUTIL_QT_SOURCESANDSINKS_H
#define GUTIL_QT_SOURCESANDSINKS_H

#include <gutil/iio.h>
#include <gutil/sourcesandsinks.h>
#include <QByteArray>

NAMESPACE_GUTIL1(Qt);


/** Inputs from a QByteArray */
class QByteArrayInput : public GUtil::ByteArrayInput
{
    // QByteArray's are explicitly shared, so copying them takes almost no time,
    //  and this guarantees that the data pointer we receive in the constructor
    //  will remain in existence, because if the input byte array is destroyed
    //  this will retain its data pointer.
    const QByteArray m_ba;
public:
    QByteArrayInput(const QByteArray &b)
        :GUtil::ByteArrayInput((const byte *)b.data(), b.length()),
         m_ba(b)
    {}
    virtual ~QByteArrayInput() {}
};

/** Outputs to a QByteArray. */
class QByteArrayOutput : public GUtil::IOutput
{
    QByteArray &m_ba;
public:
    QByteArrayOutput(QByteArray &b) :m_ba(b) {}
    virtual ~QByteArrayOutput() {}

    virtual GUINT32 WriteBytes(byte const *ba, GUINT32 len);
};


END_NAMESPACE_GUTIL1;

#endif // GUTIL_QT_SOURCESANDSINKS_H
