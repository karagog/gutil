/*Copyright 2010-2015 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef GUTIL_NO_DATABASE_FUNCTIONALITY


#include "databaselogmodel.h"
#include <gutil/databaseutils.h>
#include <gutil/databaselogger.h>
USING_NAMESPACE_GUTIL1(Qt);

DatabaseLogModel::DatabaseLogModel(const DatabaseLogger &logger, QObject *parent)
    :QSqlTableModel(parent, QSqlDatabase::database(logger.GetConnectionString()))
{
    setTable(logger.GetTableName());
}

QVariant DatabaseLogModel::data(const QModelIndex &idx, int role) const
{
    QVariant ret( QSqlTableModel::data(idx, role) );
    if(idx.column() == 0)
    {
       if(role == ::Qt::DisplayRole || role == ::Qt::EditRole)
           ret = DatabaseUtils::ConvertStringToDate(ret.toString());
    }
    else if(idx.column() == 1)
    {
        if(role == ::Qt::DisplayRole || role == ::Qt::EditRole)
        {
            switch((DatabaseLogger::LogLevelEnum)ret.toInt())
            {
            case ILog::LogLevelInfo:
                ret = "Info";
                break;
            case ILog::LogLevelWarning:
                ret = "Warning";
                break;
            case ILog::LogLevelError:
                ret = "Error";
                break;
            default:
                break;
            }
        }
    }
    return ret;
}

bool DatabaseLogModel::select()
{
    bool ret( QSqlTableModel::select() );
    removeColumn(5);
    removeColumn(4);

    setHeaderData(0, ::Qt::Horizontal, tr("Date"));
    setHeaderData(1, ::Qt::Horizontal, tr("Severity"));
    setHeaderData(2, ::Qt::Horizontal, tr("Message"));
    setHeaderData(3, ::Qt::Horizontal, tr("Title"));
    return ret;
}


#endif // GUTIL_NO_DATABASE_FUNCTIONALITY
