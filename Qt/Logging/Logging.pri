
HEADERS += \
    Logging/messageboxlogger.h \
    Logging/databaselogger.h \
    Logging/queuedlogger.h

SOURCES += \
    Logging/messageboxlogger.cpp \
    Logging/databaselogger.cpp \
    Logging/queuedlogger.cpp

RESOURCES += \
    Logging/sql_scripts.qrc

OTHER_FILES += \
    Logging/create_log_database.sqlite.sql









