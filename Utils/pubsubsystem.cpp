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

#include "pubsubsystem.h"
using namespace GUtil;

Utils::PubSubSystem::PubSubSystem(QObject *parent)
    :QObject(parent)
{
}

void Utils::PubSubSystem::PublishMessage(const QByteArray &msg)
{
    on_message_published(msg);

    emit NotifyMessage(msg);
}

void Utils::PubSubSystem::on_message_published(const QByteArray &)
{
    // Subclasses can do something interesting here
}
