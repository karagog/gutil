#ifndef PROCESSTRANSPORT_H
#define PROCESSTRANSPORT_H
#include "abstractqiodevicetransport.h"

class QProcess;
class QString;
class QStringList;

namespace GUtil
{
    namespace DataAccess
    {
        // Conducts data transport between a processes via stdin/stdout
        class ProcessTransport : public AbstractQIODeviceTransport
        {
            Q_OBJECT
        public:
            explicit ProcessTransport(QProcess *, QObject *parent = 0);
            explicit ProcessTransport(const QString &exe_name,
                                      const QStringList &args,
                                      QObject *parent = 0);

        protected:
            QProcess &Process() const;

        private:
            void _init();
        };
    }
}

#endif // PROCESSTRANSPORT_H
