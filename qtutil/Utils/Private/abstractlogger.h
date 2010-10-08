#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>

namespace GUtil
{
    class Exception;
}

namespace GQtUtil
{
    namespace Utils
    {
        class PubSubSystem;

        class AbstractLogger : public QObject
        {
            Q_OBJECT
        public:
            enum MessageTypeEnum
            {
                Message,
                Error
            };


        public slots:
            void LogMessage(const QString &);
            void LogMessage(const QString &, const QString &);

            void LogError(const QString &);
            void LogError(const QString &, const QString &);

            void LogException(const GUtil::Exception &);

            void Log(const QString &, const QString &, MessageTypeEnum);


        signals:
            // Every time we log a message
            void NotifyMessageLogged(const QString &, MessageTypeEnum);


        protected:
            explicit AbstractLogger(PubSubSystem *, QObject *parent = 0);

            // You can customize your own logger object by overriding these functions
            virtual QString PrepareLogMessage(const QString &, const QString &, MessageTypeEnum);

            // This function actually writes the message somewhere useful
            virtual void LogMessage_protected(const QString &, MessageTypeEnum) = 0;

        private:
            PubSubSystem *_pubsub;

        };
    }
}

#endif // LOGGER_H
