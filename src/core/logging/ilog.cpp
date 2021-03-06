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

#include "ilog.h"
#include <gutil/exception.h>
using namespace std;

NAMESPACE_GUTIL;


void ILog::LogException(const std::exception &std_ex) noexcept
{
    Exception<false> const *ex = dynamic_cast<Exception<false> const *>(&std_ex);
    Exception<true> const *ext_ex( dynamic_cast<Exception<true> const *>(&std_ex) );
    if(ext_ex){
        // If it's an extended exception
        Log(PrepareExceptionData(*ext_ex));
        if(ext_ex->GetInnerException())
            ILog::LogException(*ext_ex->GetInnerException());
    }
    else if(ex){
        // Normal exception
        Log(PrepareExceptionData(*ex));
    }
    else{
        // Some other type of std::exception that we don't know how to parse
        LoggingData d;
        d.MessageLevel = LogLevelError;
        d.Title = String::Format("%s caught", std_ex.what());
        Log(d);
    }
}

ILog::LoggingData ILog::PrepareExceptionData(const Exception<false> &e)
{
    LoggingData ret;
    ret.MessageLevel = LogLevelError;
    ret.Message = e.Message();
    ret.Title = String::Format("%s caught", e.what());
    return ret;
}

ILog::LoggingData ILog::PrepareExceptionData(const Exception<true> &e)
{
    LoggingData ret;
    ret.MessageLevel = LogLevelError;

    String data_string{""};
    const unordered_map<string, string> &keys( e.Data );
    if(keys.size() > 0){
        data_string = "\n\nException Data:";
        for(const auto &p : keys){
            data_string.Append(String::Format("\n\tKey: %s   Value: %s",
                                              p.first.data(),
                                              p.second.data()));
        }
    }

    ret.Message = String::Format("%s%s", e.Message().data(), data_string.ConstData());
    ret.Title = String::Format("%s caught%s",
                             e.what(),
                             e.GetInnerException() ? " (Inner exception follows immediately)" : "");
    return ret;
}


END_NAMESPACE_GUTIL;
