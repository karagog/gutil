
HEADERS += \
    logging/messageboxlogger.h \
    logging/databaselogger.h \
    logging/queuedlogger.h \
    logging/threadsafelogger.h

SOURCES += \
    logging/messageboxlogger.cpp \
    logging/databaselogger.cpp \
    logging/queuedlogger.cpp \
    logging/threadsafelogger.cpp

RESOURCES += \
    logging/sql_scripts.qrc

OTHER_FILES += \
    logging/create_log_database.sqlite.sql









