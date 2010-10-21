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

#include "streamtransport.h"
#include "Custom/gthread.h"
#include <iostream>
#include <QTimerEvent>
#include <QVariant>
#include <QtConcurrentRun>
using namespace GUtil;
using namespace std;

DataAccess::StreamTransport::StreamTransport(istream *is, ostream *os, QObject *parent) :
    AbstractDataTransportMechanism(parent)
{
    _pre_init();

    _stream_in = is;
    _stream_out = os;

    SetStopOnLineEnd(false);

    setTerminationEnabled(true);
    start();
}

DataAccess::StreamTransport::StreamTransport(iostream *ios, QObject *parent) :
    AbstractDataTransportMechanism(parent)
{
    _pre_init();

    _stream_inout = ios;

    start();
}

void DataAccess::StreamTransport::run()
{
    while(true)
    {
        string data;
        istream *is = get_istream();

        if(is != 0)
        {
            getline(*is, data);

            if(!(is->eof() || is->bad()))
                emit notifyNewData(QByteArray(data.c_str(), data.length()));
        }
    }
}

DataAccess::StreamTransport::~StreamTransport()
{
    terminate();
    wait();
}

void DataAccess::StreamTransport::_pre_init()
{
    _timer_id = -1;

    _stream_in = 0;
    _stream_out = 0;
    _stream_inout = 0;
}

void DataAccess::StreamTransport::send_data(const QByteArray &data) throw(Core::DataTransportException)
{
    ostream *os = get_ostream();

    if(os == 0)
        throw Core::DataTransportException("Output stream not set");

    os->write(data.constData(), data.length());
    if(os->bad())
        throw Core::DataTransportException("Stream write failed");
}

#define CHAR_BUFFER_SIZE 1024

QByteArray DataAccess::StreamTransport::receive_data()
        throw(Core::DataTransportException, Core::EndOfFileException)
{
    QByteArray ret;
    istream *is = get_istream();

    if(is == 0)
        throw Core::DataTransportException("Input stream not set");

    char c[CHAR_BUFFER_SIZE];
    std::string buf;

    if(_stop_on_line_end)
    {
        getline(*is, buf);
        if(!is->fail())
            ret.append(QByteArray(buf.c_str(), buf.length()));
    }
    else
    {
        is->read(c, CHAR_BUFFER_SIZE);
        while(is->gcount() > 0)
        {
            ret.append(c, is->gcount());
            if(is->eof())
                break;

            is->getline(c, CHAR_BUFFER_SIZE);
        }
    }

    if(is->bad())
        throw Core::DataTransportException("Stream read failed");

    return ret;
}

void DataAccess::StreamTransport::SetStopOnLineEnd(bool stp)
{
    _stop_on_line_end = stp;
}

void DataAccess::StreamTransport::update_has_data_variable(bool &has_data_variable) throw(GUtil::Core::DataTransportException)
{
    istream *is = get_istream();

    if(is == 0)
        throw Core::DataTransportException("Input stream not set");

    is->peek();
    has_data_variable = !(is->eof() || is->bad());
}

std::ostream *DataAccess::StreamTransport::get_ostream()
{
    if(_stream_inout == 0)
        return _stream_out;
    else
        return _stream_inout;
}

std::istream *DataAccess::StreamTransport::get_istream()
{
    if(_stream_inout == 0)
        return _stream_in;
    else
        return _stream_inout;
}
