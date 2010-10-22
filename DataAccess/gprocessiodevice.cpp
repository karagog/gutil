#include "gprocessiodevice.h"
#include <QProcess>
using namespace GUtil;

DataAccess::GProcessIODevice::GProcessIODevice(QObject *parent)
    :GQIODevice(new QProcess(parent), parent)
{
    _init();
}

DataAccess::GProcessIODevice::GProcessIODevice(QProcess *p, QObject *parent)
    :GQIODevice(p, parent)
{
    _init();
}

DataAccess::GProcessIODevice::GProcessIODevice(
        const QString &exe_name,
        const QStringList &args,
        QObject *parent)
            :GQIODevice(new QProcess(parent), parent)
{
    Process().start(exe_name, args);

    _init();
}

QProcess &DataAccess::GProcessIODevice::Process() const
{
    return (QProcess &)IODevice();
}

void DataAccess::GProcessIODevice::_init()
{
    Process().setReadChannel(QProcess::StandardOutput);
}
