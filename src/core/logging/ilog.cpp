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

#include "ilog.h"
#include "gutil_exception.h"
using namespace std;

NAMESPACE_GUTIL;


void ILog::LogException(const std::exception &std_ex) noexcept
{
    LoggingData d;
    d.MessageLevel = LogLevelError;

    Exception<> const *ex = dynamic_cast<Exception<false> const *>(&std_ex);
    ExtendedException const *ext_ex( dynamic_cast<ExtendedException const *>(&std_ex) );
    if(ext_ex){
        // If it's an extended exception
        String data_string{""};
        const map<std::string, std::string> &keys( ext_ex->Data );
        if(keys.size() > 0){
            data_string = "\n\nException Data:";
            for(const auto &p : keys){
                data_string.Append(String::Format("\n\tKey: %s   Value: %s",
                                                  p.first.data(),
                                                  p.second.data()));
            }
        }

        d.Message = String::Format("%s%s", ex->Message(), data_string.ConstData());
        d.Title = String::Format("%s caught%s",
                                 ex->what(),
                                 ext_ex->GetInnerException() ? " (Inner exception follows immediately)" : "");
        Log(d);
        if(ext_ex->GetInnerException())
            ILog::LogException(*ext_ex->GetInnerException());
    }
    else if(ex){
        // Normal exception
        d.Message = ex->Message();
        d.Title = String::Format("%s caught", ex->what());
        Log(d);
    }
    else{
        // Some other type of std::exception that we don't know how to parse
        d.Title = String::Format("%s caught", std_ex.what());
        Log(d);
    }
}


END_NAMESPACE_GUTIL;
