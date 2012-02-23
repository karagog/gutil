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

#ifndef GUTIL_DATABASELOGMODEL_H
#define GUTIL_DATABASELOGMODEL_H

#include <QSqlTableModel>

namespace GUtil{ namespace QT{
namespace Logging{
    class DatabaseLogger;
}

namespace DataModels{


/** A data model for database logs, so you can display them in
    Qt's modelview framework
*/
class DatabaseLogModel :
        public QSqlTableModel
{
    Q_OBJECT
public:

    /** Constructs a database log model with the given connection string */
    explicit DatabaseLogModel(const Logging::DatabaseLogger &, QObject *parent = 0);

    /** Overridden from QSqlTableModel */
    virtual QVariant data(const QModelIndex &idx, int role) const;

    /** Overridden from QSqlTableModel */
    virtual bool select();

};


}}}

#endif // GUTIL_DATABASELOGMODEL_H
