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
#include <iostream>
#include <QTimerEvent>
using namespace GUtil;
using namespace std;

DataAccess::DataTransports::StreamTransport::StreamTransport(istream *is, ostream *os, QObject *parent) :
    AbstractDataTransportMechanism(parent)
{
    _pre_init();

    _stream_in = is;
    _stream_out = os;
}

DataAccess::DataTransports::StreamTransport::StreamTransport(iostream *ios, QObject *parent) :
    AbstractDataTransportMechanism(parent)
{
    _pre_init();

    _stream_inout = ios;
}

void DataAccess::DataTransports::StreamTransport::_pre_init()
{
    _timer_id = -1;
    _polling_interval = _default_polling_interval;

    _stream_in = 0;
    _stream_out = 0;
    _stream_inout = 0;
}

void DataAccess::DataTransports::StreamTransport::SetIStreamPollingEnabled(bool val)
{
    if(val && _timer_id == -1)
    {
        _timer_id = startTimer(_polling_interval);
    }
    else if(!val && _timer_id != -1)
    {
        killTimer(_timer_id);
        _timer_id = -1;
    }
}

void DataAccess::DataTransports::StreamTransport::SetIStreamPollingInterval(int new_interval)
{
    _polling_interval = new_interval;

    // Reset the polling
    SetIStreamPollingEnabled(false);
    SetIStreamPollingEnabled(true);
}

void DataAccess::DataTransports::StreamTransport::timerEvent(QTimerEvent *ev)
{
    // This is the function that polls whether the istream has data
    if(ev->timerId() == _timer_id)
    {
        trigger_update_has_data_available();
    }
}

void DataAccess::DataTransports::StreamTransport::send_data(const QByteArray &data) throw(Core::DataTransportException)
{
    ostream *os = get_ostream();

    if(os == 0)
        throw Core::DataTransportException("Output stream not set");

    os->write(data.constData(), data.length());
    if(os->bad())
        throw Core::DataTransportException("Stream write failed");
}

QByteArray DataAccess::DataTransports::StreamTransport::receive_data()
        throw(Core::DataTransportException, Core::EndOfFileException)
{
    QByteArray ret;
    char c;
    istream *is = get_istream();

    if(is == 0)
        throw Core::DataTransportException("Input stream not set");

    is->get(c);
    while(!(is->eof() || is->bad()))
    {
        // Break on new line
        if(c == '\n')
            break;

        ret.append(c);
        is->get(c);
    }

    if(is->bad())
        throw Core::DataTransportException("Stream read failed");

    return ret;
}

void DataAccess::DataTransports::StreamTransport::update_has_data_variable(bool &has_data_variable) throw(GUtil::Core::DataTransportException)
{
    istream *is = get_istream();

    if(is == 0)
        throw Core::DataTransportException("Input stream not set");

    is->peek();
    has_data_variable = !(is->eof() || is->bad());
}

std::ostream *DataAccess::DataTransports::StreamTransport::get_ostream()
{
    if(_stream_inout == 0)
        return _stream_out;
    else
        return _stream_inout;
}

std::istream *DataAccess::DataTransports::StreamTransport::get_istream()
{
    if(_stream_inout == 0)
        return _stream_in;
    else
        return _stream_inout;
}
