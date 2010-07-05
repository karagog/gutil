#include "pubsubsystem.h"
#include "exception.h"
using namespace GQtUtil::Utils;

PubSubSystem::PubSubSystem(QObject *parent) :
    QObject(parent)
{
}

void PubSubSystem::publishMessage(QString title, QString message)
{
    emit notifyMessage(title, message);
}

void PubSubSystem::publishProgress(int progress, int id)
{
    emit publishProgress(progress, id);
}

void PubSubSystem::setData(int id, const QVariant &d)
{
    data_lock.lockForWrite();

    _data[id] = d;

    data_lock.unlock();
}

QVariant PubSubSystem::getData(int id)
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
