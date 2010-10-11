#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>

namespace GUtil
{
    namespace Core
    {
        class Exception;
    }

    namespace Utils
    {
        class PubSubSystem;

        class AbstractLogger : public QObject
        {
            Q_OBJECT
        public:
            enum MessageLevelEnum
            {
                Info = 0,
                Warning = 1,
                Error = 2
            };

            // Determines the level under which we won't log anything
            void SetMessageLevel(MessageLevelEnum);
            MessageLevelEnum GetMessageLevel();


        public slots:
            void LogMessage(const QString &message, const QString &title = QString::null);
            void LogWarning(const QString &message, const QString &title = QString::null);
            void LogError(const QString &message, const QString &title = QString::null);

            void LogException(const GUtil::Core::Exception &exception);

            void Log(const QString &message, const QString &title, MessageLevelEnum);


        signals:
            // Every time we log a message
            void NotifyMessageLogged(const QString &, MessageLevelEnum);


        protected:
            explicit AbstractLogger(PubSubSystem *pss = 0, QObject *parent = 0);
            virtual ~AbstractLogger();

            // This function actually writes the message somewhere useful
            virtual void LogMessage_protected(const QString &, MessageLevelEnum) = 0;

            // These happen before/after logging
            virtual bool PreLogMessage();
            virtual void PostLogMessage();

            // You can customize your own logging format
            virtual QString PrepareLogMessage(const QString &, const QString &, MessageLevelEnum);


        private:
            PubSubSystem *_pubsub;
            MessageLevelEnum _message_level;

        };
    }
}

#endif // LOGGER_H
