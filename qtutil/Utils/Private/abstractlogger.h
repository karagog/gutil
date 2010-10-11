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
            void LogMessage(const QString &message, const QString &title = QString::null);
            void LogError(const QString &message, const QString &title = QString::null);

            void LogException(const GUtil::Exception &exception);

            void Log(const QString &message, const QString &title, MessageTypeEnum);


        signals:
            // Every time we log a message
            void NotifyMessageLogged(const QString &, MessageTypeEnum);


        protected:
            explicit AbstractLogger(PubSubSystem *pss = 0, QObject *parent = 0);
            virtual ~AbstractLogger();

            // This function actually writes the message somewhere useful
            virtual void LogMessage_protected(const QString &, MessageTypeEnum) = 0;

            // These happen before/after logging
            virtual bool PreLogMessage();
            virtual void PostLogMessage();

            // You can customize your own logging format
            virtual QString PrepareLogMessage(const QString &, const QString &, MessageTypeEnum);


        private:
            PubSubSystem *_pubsub;

        };
    }
}

#endif // LOGGER_H
