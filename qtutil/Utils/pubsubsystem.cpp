#include "pubsubsystem.h"
#include "exception.h"
using namespace GQtUtil::Utils;

PubSubSystem::PubSubSystem(QObject *parent) :
    QObject(parent)
{
}

void PubSubSystem::PublishMessage(QString message, QString title)
{
    emit NotifyMessage(message, title);
}

void PubSubSystem::PublishProgress(int progress, int id)
{
    emit PublishProgress(progress, id);
}

void PubSubSystem::SetData(int id, const QVariant &d)
{
    data_lock.lockForWrite();

    _data[id] = d;

    data_lock.unlock();
}

QVariant PubSubSystem::GetData(int id)
{
    data_lock.lockForRead();

    if(!_data.contains(id))
    {
        data_lock.unlock();
        throw GUtil::Exception("ID not found");
    }

    QVariant ret = _data.value(id);

    data_lock.unlock();

    return ret;
}
