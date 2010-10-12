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

#include "databaselogger.h"
#include "pubsubsystem.h"
using namespace GQtUtil::Utils;

DatabaseLogger::DatabaseLogger(PubSubSystem *pss, QObject *parent) :
    AbstractLogger(pss, parent)
{
}

bool DatabaseLogger::PreLogMessage()
{
    // Set up database connection
    return true;
}

void DatabaseLogger::LogMessage_protected(const QString &msg, MessageLevelEnum)
{

}

void DatabaseLogger::PostLogMessage()
{
    // Close database connection
}
