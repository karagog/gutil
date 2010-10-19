#include "processtransport.h"
#include <QProcess>
using namespace GUtil;

DataAccess::ProcessTransport::ProcessTransport(QProcess *p, QObject *parent)
    :AbstractQIODeviceTransport(p, parent)
{
    _init();
}

DataAccess::ProcessTransport::ProcessTransport(
        const QString &exe_name,
        const QStringList &args,
        QObject *parent)
            :AbstractQIODeviceTransport(new QProcess(parent), parent)
{
    Process().start(exe_name, args);

    _init();
}

QProcess &DataAccess::ProcessTransport::Process() const
{
    return (QProcess &)IODevice();
}

void DataAccess::ProcessTransport::_init()
{
    Process().setReadChannel(QProcess::StandardOutput);
}
