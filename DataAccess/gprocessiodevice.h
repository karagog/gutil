#ifndef PROCESSTRANSPORT_H
#define PROCESSTRANSPORT_H
#include "gqiodevice.h"

class QProcess;
class QString;
class QStringList;

namespace GUtil
{
    namespace DataAccess
    {
        // Conducts data transport between a processes via stdin/stdout
        class GProcessIODevice : public GQIODevice
        {
            Q_OBJECT
        public:
            explicit GProcessIODevice(QObject *parent = 0);
            explicit GProcessIODevice(QProcess *p, QObject *parent = 0);
            explicit GProcessIODevice(const QString &exe_name,
                                      const QStringList &args,
                                      QObject *parent = 0);

            QProcess &Process() const;

        private:
            void _init();
        };
    }
}

#endif // PROCESSTRANSPORT_H
