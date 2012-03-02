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

#ifndef MYFLATTREEVIEW_H
#define MYFLATTREEVIEW_H

#ifndef GUTIL_NO_GUI_FUNCTIONALITY

#include <QAbstractProxyModel>
#include "gutil_map.h"

namespace GUtil{ namespace QT{ namespace DataModels{


/** This class will represent a tree model as a list model, essentially
    throwing away the hierarchy and letting you access it like a list
*/
class FlatTreeModel :
        public QAbstractProxyModel
{
    Q_OBJECT
public:

    inline explicit FlatTreeModel(QObject *parent = 0) :QAbstractProxyModel(parent){}

    virtual void setSourceModel(QAbstractItemModel *);


public slots:

    void refreshSourceModel(){
        source_model_about_to_be_reset();
        source_model_reset();
    }


protected:

    /** Overridden from QAbstractProxyModel */
    virtual QModelIndex mapToSource(const QModelIndex &proxyIndex) const{
        return _index_map_to_source.At(proxyIndex.internalId());
    }

    /** Overridden from QAbstractProxyModel */
    virtual QModelIndex mapFromSource(const QModelIndex &sourceIndex) const{
        int t = _index_map_from_source.At(sourceIndex.internalId());
        if(t == 0 || !sourceIndex.isValid())
            return QModelIndex();
        return index(t, sourceIndex.column());
    }

    /** Overridden from QAbstractProxyModel */
    virtual QModelIndex parent(const QModelIndex &) const{ return QModelIndex(); }

    /** Overridden from QAbstractProxyModel */
    virtual QModelIndex index(int row, int column, const QModelIndex &par = QModelIndex()) const{
        if(par.isValid()) return QModelIndex();
        return createIndex(row, column, row);
    }

    /** Overridden from QAbstractProxyModel */
    virtual int rowCount(const QModelIndex &parent) const{
        if(parent.isValid()) return 0;
        return _total_rows;
    }

    /** Overridden from QAbstractProxyModel */
    virtual int columnCount(const QModelIndex &parent) const{
        return sourceModel() ? sourceModel()->columnCount(mapToSource(parent)) : 0;
    }

    /** Overridden from QAbstractProxyModel */
    virtual QVariant data(const QModelIndex &proxyIndex, int role) const{
        return mapToSource(proxyIndex).data(role);
    }


private slots:

    inline void source_model_about_to_be_reset(){ beginResetModel(); }
    void source_model_reset();

    void source_model_data_changed(const QModelIndex &ind1, const QModelIndex &ind2){
        emit dataChanged(mapFromSource(ind1), mapFromSource(ind2));
    }


private:

    GUtil::DataObjects::Map<int, QPersistentModelIndex> _index_map_to_source;
    GUtil::DataObjects::Map<int, int> _index_map_from_source;
    GUtil::DataObjects::Map<int, int> _child_record;

    int _total_rows;


    int _count_preceeding_indexes(const QModelIndex &) const;
    int _count_child_indexes(const QModelIndex &ind = QModelIndex());
    void _refresh_index_mapping(const QModelIndex &ind = QModelIndex());

    QModelIndex _map_from_source(const QModelIndex &sourceIndex){
        if(sourceModel()) return index(_count_preceeding_indexes(sourceIndex) - 1, sourceIndex.column());
        return QModelIndex();
    }

    void _refresh_child_record();

};


}}}


#endif // GUI_FUNCTIONALITY

#endif // MYFLATTREEVIEW_H
