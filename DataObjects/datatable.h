/*Copyright 2010 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef DATATABLE_H
#define DATATABLE_H

#include "Interfaces/iqxmlserializable.h"
#include "Interfaces/icollection.h"
#include "Core/Interfaces/iupdatable.h"
#include "Core/Interfaces/ireadonlyobject.h"
#include <QStringList>
#include <QAbstractTableModel>

namespace GUtil
{
    namespace DataObjects
    {
        class DataSet;
        class DataRow;

        // A class used to hold data and serialize
        //   the values to xml or access them conveniently with string keys
        class DataTable :   public QAbstractTableModel,
                            public Interfaces::IQXmlSerializable,
                            public Core::Interfaces::IReadOnlyObject,
                            public Core::Interfaces::IUpdatable
        {
            Q_OBJECT
        public:
            DataTable();

            QList<QByteArray> Values(const QString &key);
            QPair<QString, QByteArray> Value(int) const;

            virtual void SetValue(int, const QString &key, const QByteArray &value);

            void InsertValue(int, const QString &key, const QByteArray &value);
            QPair<QString, QByteArray> RemoveValue(int);
            virtual void SwapIndexes(int one, int two);


            // Interface for IQXmlSerializable
            virtual void WriteXml(QXmlStreamWriter &) const;
            virtual void ReadXml(QXmlStreamReader &)
                    throw(GUtil::Core::XmlException);

            virtual void SetReadOnly(bool readonly);


            // Interface for QAbstractTableModel
            virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
            virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;

            virtual QVariant data(const QModelIndex &index, int role) const;
            virtual bool setData(const QModelIndex &index, const QVariant &value, int role);

            virtual bool insertRows(int row, int count, const QModelIndex &parent);
            virtual bool removeRows(int row, int count, const QModelIndex &parent);

            virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;
            virtual Qt::ItemFlags flags(const QModelIndex &index) const;

        protected:
            DataTable(DataSet *ds);

        private:
            DataSet *_dataset;

            QList<DataRow *> _data;

            QStringList _keys;
            QStringList _labels;

        };


        class DataTableCollection : public Interfaces::ICollection<DataTable *>
        {
            friend class DataSet;

        protected:
            DataTableCollection(DataSet *);

            virtual void onAdd(const DataTable* &, int index);
            virtual void onRemove(const DataTable* &);

        private:
            DataSet *_ds;
        };
    }
}

#endif // DATATABLE_H
