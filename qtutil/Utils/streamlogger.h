#ifndef STREAMLOGGER_H
#define STREAMLOGGER_H

#include "Private/abstractlogger.h"

namespace GQtUtil
{
    namespace Utils
    {
        class PubSubSystem;

        // Logs to an ostream
        class StreamLogger : public GQtUtil::Utils::AbstractLogger
        {
            Q_OBJECT
        public:
            // Default constructor outputs to stdout
            explicit StreamLogger(PubSubSystem *, QObject *parent = 0);


        protected:
            explicit StreamLogger(std::ostream *, PubSubSystem *, QObject *parent = 0);

            virtual void LogMessage_protected(const QString &, MessageTypeEnum);            

            virtual bool PrepareMessageStream();
            virtual void LogMessageToStream(const QString &);
            virtual void CloseMessageStream();

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
