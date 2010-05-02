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

#ifndef MYFLATTREEVIEW_H
#define MYFLATTREEVIEW_H

#include <QAbstractProxyModel>
#include <QMap>

namespace GUtil
{
    namespace QtUtil
    {
        // This class will represent a tree model as a list model, essentially
        //  throwing away the hierarchy and letting you access it like a list
        class myFlatTreeView : public QAbstractProxyModel
        {
            Q_OBJECT
        public:
            explicit myFlatTreeView(QObject *parent = 0);

            virtual void setSourceModel(QAbstractItemModel *);

        protected:
            virtual QModelIndex mapToSource(const QModelIndex &proxyIndex) const;
            virtual QModelIndex mapFromSource(const QModelIndex &sourceIndex) const;

            virtual QModelIndex parent(const QModelIndex &) const;
            virtual QModelIndex index(int row, int column, const QModelIndex &par = QModelIndex()) const;
            virtual int rowCount(const QModelIndex &parent) const;
            virtual int columnCount(const QModelIndex &parent) const;
            virtual QVariant data(const QModelIndex &proxyIndex, int role) const;

        private:
            int _count_preceeding_indexes(const QModelIndex &) const;
            int _count_child_indexes(const QModelIndex &) const;

            QMap<int, int> _child_record;
            void _refresh_child_record();

        private slots:
            void source_model_rows_inserted(const QModelIndex &, int, int);
            void source_model_rows_removed(const QModelIndex &, int, int);
            void _reset_model();
        };
    }
}

#endif // MYFLATTREEVIEW_H
