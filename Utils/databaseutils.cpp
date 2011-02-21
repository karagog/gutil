/*Copyright 2011 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include "databaseutils.h"
#include "Core/exception.h"
GUTIL_USING_NAMESPACE(Utils);


#ifdef DATABASE_FUNCTIONALITY

#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

void DatabaseUtils::ThrowQueryException(const QSqlQuery &q)
{
    if(q.lastError().isValid())
    {
        GUtil::Core::DataTransportException ex(q.lastError().text().toStdString());

        ex.SetData("query_text", q.executedQuery().toStdString());

        QMap<QString, QVariant> bound_values( q.boundValues() );
        int cnt(1);

        foreach(QString k, bound_values.keys())
            ex.SetData(QString("Bound Value %1").arg(cnt++).toStdString(),
                       bound_values[k].toString().toStdString());

        THROW_GUTIL_EXCEPTION(ex);
    }
}

#endif // DATABASE_FUNCTIONALITY
