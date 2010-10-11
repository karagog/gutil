#include "pubsubsystem.h"
#include "exception.h"
using namespace GQtUtil::Utils;

PubSubSystem::PubSubSystem(QObject *parent) :
    QObject(parent)
{
}

void PubSubSystem::PublishMessage(const QString& message, const QString& title)
{
    emit NotifyMessage(message, title);
}

void PubSubSystem::PublishWarning(const QString& message, const QString& title)
{
    emit NotifyWarning(message, title);
}

void PubSubSystem::PublishError(const QString& message, const QString& title)
{
    emit NotifyError(message, title);
}

void PubSubSystem::PublishProgress(int progress, int id)
{
    emit PublishProgress(progress, id);
}
