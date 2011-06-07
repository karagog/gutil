/*Copyright 2011 George Karagoulis

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
#include "gutil_macros.h"
#include "Core/exception.h"
#include <QMetaType>
GUTIL_USING_NAMESPACE(Utils);

PubSubSystem::PubSubSystem(QObject *parent)
    :QObject(parent)
{
    // So passing exceptions through signals and slots will work
    qRegisterMetaType<GUtil::Core::Exception>("GUtil::Core::Exception");
}

void PubSubSystem::PublishMessage(const QString &msg)
{
    on_message_published(msg);

    emit NotifyMessage(msg);
}

void PubSubSystem::PublishError(const QString &msg)
{
    on_error_published(msg);

    emit NotifyError(msg);
}

void PubSubSystem::PublishProgress(int progress, int progress_id)
{
    on_progress_published(progress, progress_id);

    emit NotifyProgress(progress, progress_id);
}

void PubSubSystem::PublishException(const GUtil::Core::Exception &ex)
{
    on_exception_published(ex);

    emit NotifyException(ex);
}

void PubSubSystem::on_message_published(const QString &)
{
    // Subclasses can do something interesting here
}

void PubSubSystem::on_error_published(const QString &)
{

}

void PubSubSystem::on_progress_published(int, int)
{

}

void PubSubSystem::on_exception_published(const Core::Exception &)
{

}
