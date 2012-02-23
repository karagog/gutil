/*Copyright 2010-2012 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifdef DATABASE_FUNCTIONALITY

#include "databaseutils.h"
#include "Core/extendedexception.h"
#include <QStringList>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDatabase>
#include <QVariant>

NAMESPACE_GUTIL2(QT, Utils);

void DatabaseUtils::ThrowQueryException(const QSqlQuery &q)
{
    if(q.lastError().isValid())
    {
        DataTransportException<true> ex(q.lastError().text().toAscii().constData());

        ex.SetData("query_text", q.executedQuery().toAscii().constData());

        QMap<QString, QVariant> bound_values( q.boundValues() );
        int cnt(1);

        foreach(QString k, bound_values.keys())
            ex.SetData(QString("Bound Value %1").arg(cnt++).toAscii().constData(),
                       bound_values[k].toString().toAscii().constData());

        THROW_GUTIL_EXCEPTION(ex);
    }
}

void DatabaseUtils::ExecuteScript(QSqlDatabase &db, const QString &script_sql)
{
    if(!db.transaction())
        THROW_NEW_GUTIL_EXCEPTION2(DataTransportException,
                                   QString("Unable to create a transaction: %1")
                                   .arg(db.lastError().text()).toAscii().constData());
    try
    {
        QSqlQuery q(db);
        int index(0);
        int prev_index(0);
        while((index = script_sql.indexOf(';', index)) != -1)
        {
            index++;
            q.prepare(script_sql.left(index).right(index - prev_index));

            if(!q.exec())
                ThrowQueryException(q);

            prev_index = index;
        }
    }
    catch(...)
    {
        db.rollback();
        throw;
    }

    if(!db.commit())
        THROW_NEW_GUTIL_EXCEPTION2(DataTransportException,
                                   QString("Transaction commit failed: %1")
                                   .arg(db.lastError().text()).toAscii().constData());
}

#endif // DATABASE_FUNCTIONALITY



QString DatabaseUtils::ConvertDateToString(const QDateTime &d, DateResolution dr)
{
    QString fmt("yyyy_MM_dd_hh_mm");

    if(dr == DR_Seconds)
        fmt.append("_ss");

    return d.toString(fmt);
}

QDateTime DatabaseUtils::ConvertStringToDate(const QString &s)
{
    QDateTime d;
    QStringList splitted = s.split('_');

    if(splitted.size() < 5)
        THROW_NEW_GUTIL_EXCEPTION2(Exception,
                                   QString("Invalid date string: %1").arg(s).toAscii().constData());

    d.setDate(QDate(splitted[0].toInt(), splitted[1].toInt(), splitted[2].toInt()));

    d.setTime(QTime(splitted[3].toInt(), splitted[4].toInt(),
                    splitted.length() > 5 ? splitted[5].toInt() : 0));

    return d;
}


END_NAMESPACE_GUTIL2;
