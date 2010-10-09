#ifndef STREAMLOGGER_H
#define STREAMLOGGER_H

#include "Private/abstractlogger.h"

namespace GQtUtil
{
    namespace Utils
    {
        class PubSubSystem;

        // Logs to an ostream
        class AbstractStreamLogger : public AbstractLogger
        {
            Q_OBJECT
        protected:
            explicit AbstractStreamLogger(std::ostream *, PubSubSystem *, QObject *parent = 0);

            virtual void LogMessage_protected(const QString &, MessageTypeEnum);

            static void LogMessageToStream(const QString &, std::ostream *);

            // Use this to access the stream
            std::ostream *Stream() const;


        private:
            void _init(std::ostream *os);

            std::ostream *_stream;
        };
    }
}

#endif // STREAMLOGGER_H
