/*Copyright 2010-2013 George Karagoulis

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

#include "databaseutils.h"
#include "gutil_extendedexception.h"
#include <QStringList>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDatabase>
#include <QVariant>
USING_NAMESPACE_GUTIL;

#ifdef GUTIL_DEBUG2
#include "gutil_console.h"
#endif


NAMESPACE_GUTIL1(QT);


GUtil::String DatabaseUtils::InfoString(const QSqlQuery &q)
{
    int cnt(0);
    String tmp;
    foreach(QVariant v, q.boundValues().values())
    {
        ++cnt;
        tmp.Append( String::Format("  Bound value %d: %s\n", cnt, v.toString().toAscii().constData()) );
    }

    String ret = String::Format("Query String:    \"%s\"", q.lastQuery().toAscii().constData());
    if(!tmp.IsEmpty()){
        ret.Append(String::Format("\n\n%s", tmp.ConstData()));
    }
    return ret;
}

void DatabaseUtils::ThrowQueryException(const QSqlQuery &q)
{
    if(q.lastError().isValid())
    {
        DataTransportException<true> ex(q.lastError().text().toAscii().constData());
        ex.Data["query_text"] = q.lastQuery().toAscii().constData();

        QMap<QString, QVariant> bound_values( q.boundValues() );
        int cnt(1);

        ForEach(bound_values.keys(), [&](const QString &k){
            ex.Data[QString("Bound Value %1").arg(cnt++).toAscii().constData()] =
                    bound_values[k].toString().toAscii().constData();
        });

        THROW_GUTIL_EXCEPTION(ex);
    }
}

void DatabaseUtils::ExecuteQuery(QSqlQuery &q)
{
#ifdef GUTIL_DEBUG2
    GUtil::Console::WriteLine(InfoString(q));
#endif

    if(!q.exec())
        ThrowQueryException(q);

#ifdef GUTIL_DEBUG2
    if(q.isSelect())
        GUtil::Console::WriteLine(String::Format("Last Query returned %d results. \n\n", q.size()));
    else
        GUtil::Console::WriteLine(String::Format("Last Query modified %d rows. \n\n", q.numRowsAffected()));
#endif
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

    if(!s.isEmpty())
    {
        QStringList splitted = s.split('_');

        if(splitted.size() < 5)
            THROW_NEW_GUTIL_EXCEPTION2(Exception,
                                       QString("Invalid date string: %1").arg(s).toAscii().constData());

        d.setDate(QDate(splitted[0].toInt(), splitted[1].toInt(), splitted[2].toInt()));

        d.setTime(QTime(splitted[3].toInt(), splitted[4].toInt(),
                        splitted.length() > 5 ? splitted[5].toInt() : 0));
    }

    return d;
}


END_NAMESPACE_GUTIL1;
