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

#ifndef GUTIL_DATATABLE_H
#define GUTIL_DATATABLE_H

#include "gutil_collection.h"
#include "gutil_map.h"
#include "gutil_strings.h"
#include "gutil_pairlist.h"
#include "gutil_smartpointer.h"
#include "gutil_iupdatable.h"

NAMESPACE_GUTIL1(DataObjects);


template<class T>class DataTable;

class DataColumn
{
public:
    PROPERTY(Key, String);
    PROPERTY(Name, String);

    inline DataColumn(){}
    inline DataColumn(const char *key) :_p_Key(key){}
    inline DataColumn(const String &key) :_p_Key(key){}
    inline DataColumn(const String &key, const String &name) :_p_Key(key), _p_Name(name){}

    inline bool operator == (const char *key) const{ return GetKey() == key; }
    inline bool operator != (const char *key) const{ return GetKey() != key; }

    inline bool operator == (const String &key) const{ return GetKey() == key; }
    inline bool operator != (const String &key) const{ return GetKey() != key; }

    inline bool operator == (const DataColumn &col) const{ return GetKey() == col.GetKey(); }
    inline bool operator != (const DataColumn &col) const{ return GetKey() != col.GetKey(); }
};


class DataColumnCollection :
        public Collection<DataColumn>
{
public:
    inline DataColumnCollection(){}
    inline explicit DataColumnCollection(int num_cols)
        :Collection<DataColumn>(num_cols)
    {
        for(int i(0); i < num_cols; ++i)
            Add(String::FromInt(i));
    }
    inline explicit DataColumnCollection(const Vector<String> &col_names)
        :Collection<DataColumn>(col_names.Count())
    {
        for(Vector<String>::const_iterator iter(col_names.begin()); iter != col_names.end(); ++iter)
            Add(*iter);
    }
protected:
    virtual void pre_add(DataColumn &item, int indx){
        if(Contains(item))
            THROW_NEW_GUTIL_EXCEPTION2(ValidationException, String::Format("The column key '%s' is already in the table", item.GetKey().ConstData()));
    }
    virtual void pre_item_changed(const DataColumn &value_before, const DataColumn &new_value, int indx){
        if(Contains(new_value))
            THROW_NEW_GUTIL_EXCEPTION2(ValidationException,
                                       String::Format("The column key '%s' is already in the table", new_value.GetKey().ConstData()));
    }
};


template<class T>
class DataRow :
        public Utils::SharedData,
        public Interfaces::IUpdatable,
        protected ResizableCollection<T>
{
    friend class DataTable<T>;
    DataColumnCollection const *m_columns;
    Map<String, int> const *m_columnMap;
public:

    inline DataRow(const DataRow<T> &o)
        :Utils::SharedData(), Interfaces::IUpdatable(),
          ResizableCollection<T>(o),
          m_columns(o.m_columns),
          m_columnMap(o.m_columnMap),
          _p_Table(0)
    {}

    /** Creates a null row.  You should create a row with the table's CreateRow function. */
    inline DataRow() :m_columns(0), m_columnMap(0), _p_Table(0){}

    PROPERTY_POINTER(Table, DataTable<T>);

    inline int ColumnCount() const{ return ResizableCollection<T>::Count(); }

    inline DataTable<T> const &Table() const{ return *_p_Table; }

    inline const T &operator [](int col) const{ return Collection<T>::operator [](col); }
    inline typename Collection<T>::ItemReference operator [](int col){ return Collection<T>::operator [](col); }

    inline const T &operator [](const String &col_name) const{
        if(!m_columnMap->Contains(col_name)) THROW_NEW_GUTIL_EXCEPTION(IndexOutOfRangeException);
        return operator [](m_columnMap->At(col_name));
    }
    inline typename Collection<T>::ItemReference operator [](const String &col_name){
        if(!m_columnMap->Contains(col_name)) THROW_NEW_GUTIL_EXCEPTION(IndexOutOfRangeException);
        return operator [](m_columnMap->At(col_name));
    }

    class iterator : public ResizableCollection<T>::iterator
    {
    public:
        inline iterator(const typename ResizableCollection<T>::iterator &iter) :ResizableCollection<T>::iterator(iter){}
        inline iterator(const iterator &iter) :ResizableCollection<T>::iterator(iter){}
    };

    class const_iterator : public ResizableCollection<T>::const_iterator
    {
    public:
        inline const_iterator(const typename ResizableCollection<T>::const_iterator &i) :ResizableCollection<T>::const_iterator(i){}
        inline const_iterator(const iterator &i) :ResizableCollection<T>::const_iterator(i){}
        inline const_iterator(const const_iterator &i) :ResizableCollection<T>::const_iterator(i){}
    };

    inline iterator begin(){ return ResizableCollection<T>::begin(); }
    inline const_iterator begin() const{ return ResizableCollection<T>::begin(); }
    inline iterator end(){ return ResizableCollection<T>::end(); }
    inline const_iterator end() const{ return ResizableCollection<T>::end(); }

    inline iterator rbegin(){ return ResizableCollection<T>::rbegin(); }
    inline const_iterator rbegin() const{ return ResizableCollection<T>::rbegin(); }
    inline iterator rend(){ return ResizableCollection<T>::rend(); }
    inline const_iterator rend() const{ return ResizableCollection<T>::rend(); }


protected:

    inline DataRow(const DataTable<T> *dt, int num_cols)
        :ResizableCollection<T>(num_cols),
          m_columns(&dt->Columns()),
          m_columnMap(&dt->ColumnMap()),
          _p_Table(0)
    {
        ResizableCollection<T>::Resize(num_cols);
    }

    virtual void pre_item_changed(const T &value_before, const T &new_value, int indx){
        MakeDirty();
    }

    virtual void on_make_dirty(){
        // Mark the table dirty when a row becomes dirty
        if(_p_Table)
            _p_Table->MakeDirty();
    }

};


/** Implements a table of data.

    The DataTable provides the following features:
    * Row observability
    * Can commit/undo changes to the table
*/
template<class T>class DataTable :
        public Interfaces::IUpdatable,
        private Collection< Utils::SharedSmartPointer< DataRow<T> > >
{
    friend class DataRow<T>;
public:

    inline DataTable(){}
    inline DataTable(GINT32 num_cols){
        int col(0);
        String col_key;
        while(num_cols-- != 0)
        {
            col_key = String::FromInt(col++);
            m_columns.Add(col_key);
            column_map.Insert(col_key, col);
        }
    }
    inline DataTable(const Vector<String> &column_names)
    { _init_columns(column_names); }
    inline DataTable(const String &name, const Vector<String> &column_names)
        :m_name(name)
    { _init_columns(column_names); }

    /** Conducts a deep-copy of the table. */
    inline DataTable(const DataTable<T> &o)
        :Interfaces::IUpdatable(),
          Collection< Utils::SharedSmartPointer< DataRow<T> > >(o),
          m_columns(o.m_columns),
          column_map(o.column_map),
          m_name(o.m_name)
    {
        // Detach all rows from the other table
        for(typename List< Utils::SharedSmartPointer< DataRow<T> > >::iterator iter(RowsBegin());
            iter != RowsEnd(); ++iter)
            iter->Detach();
    }
    /** Conducts a deep-copy of the table. */
    inline DataTable &operator = (const DataTable<T> &o){
        Clear();
        new(this) DataTable<T>(o);
        return *this;
    }


    inline void Clear(){
        Collection< Utils::SharedSmartPointer< DataRow<T> > >::Clear();
        m_columns.Clear();
        column_map.Clear();
    }

    inline GUINT32 RowCount() const{
        return Collection< Utils::SharedSmartPointer< DataRow<T> > >::Count();
    }
    inline int ColumnCount() const{
        return column_map.Size();
    }

    inline GINT32 ColumnIndex(const String &col_key){
        if(!column_map.Contains(col_key))
            return -1;
        return column_map[col_key];
    }

    inline Vector<String> ColumnKeys() const{ return column_map.Keys(); }


    inline Collection< Utils::SharedSmartPointer< DataRow<T> > > &Rows(){ return *this; }
    inline Collection< Utils::SharedSmartPointer< DataRow<T> > > const &Rows() const{ return *this; }


    inline String const &Name() const{ return m_name; }
    inline void SetName(const String &n){ MakeDirty(); m_name = n; }


    /** An iterator through the rows. */
    class RowIterator :
            public Collection< Utils::SharedSmartPointer< DataRow<T> > >::iterator
    {
        friend class DataTable<T>;
    public:

        inline RowIterator(const typename Collection< Utils::SharedSmartPointer< DataRow<T> > >::iterator &iter) :Collection< Utils::SharedSmartPointer< DataRow<T> > >::iterator(iter){}
        inline RowIterator(const RowIterator &iter) :Collection< Utils::SharedSmartPointer< DataRow<T> > >::iterator(iter){}

        inline DataRow<T> const &operator *() const{ return *Collection< Utils::SharedSmartPointer< DataRow<T> > >::iterator::operator *(); }
        inline DataRow<T> const *operator ->() const{ return Collection< Utils::SharedSmartPointer< DataRow<T> > >::iterator::operator *().ConstData(); }

        inline T const &operator [](int indx) const{ return Collection< Utils::SharedSmartPointer< DataRow<T> > >::iterator::operator *()->operator [](indx); }
        inline typename Collection<T>::ItemReference operator [](int indx){ return Collection< Utils::SharedSmartPointer< DataRow<T> > >::iterator::operator *()->operator [](indx); }

    };

    /** A const-iterator through the rows. */
    class ConstRowIterator :
            public Collection< Utils::SharedSmartPointer< DataRow<T> > >::const_iterator
    {
        friend class DataTable<T>;
    public:

        inline ConstRowIterator(const RowIterator &iter) :Collection< Utils::SharedSmartPointer< DataRow<T> > >::const_iterator(iter){}
        inline ConstRowIterator(const typename Collection< Utils::SharedSmartPointer< DataRow<T> > >::const_iterator &iter) :Collection< Utils::SharedSmartPointer< DataRow<T> > >::const_iterator(iter){}
        inline ConstRowIterator(const ConstRowIterator &iter) :Collection< Utils::SharedSmartPointer< DataRow<T> > >::const_iterator(iter){}

        inline DataRow<T> const &operator *() const{ return *Collection< Utils::SharedSmartPointer< DataRow<T> > >::const_iterator::operator *(); }
        inline DataRow<T> const *operator ->() const{ return Collection< Utils::SharedSmartPointer< DataRow<T> > >::const_iterator::operator *().ConstData(); }

        inline T const &operator [](int indx) const{ return Collection< Utils::SharedSmartPointer< DataRow<T> > >::const_iterator::operator *()->operator [](indx); }

    };

    inline RowIterator RowsBegin(){ return Collection< Utils::SharedSmartPointer< DataRow<T> > >::begin(); }
    inline ConstRowIterator RowsBegin() const{ return Collection< Utils::SharedSmartPointer< DataRow<T> > >::begin(); }
    inline RowIterator RowsEnd(){ return Collection< Utils::SharedSmartPointer< DataRow<T> > >::end(); }
    inline ConstRowIterator RowsEnd() const{ return Collection< Utils::SharedSmartPointer< DataRow<T> > >::end(); }

    inline RowIterator RowsRBegin(){ return Collection< Utils::SharedSmartPointer< DataRow<T> > >::rbegin(); }
    inline ConstRowIterator RowsRBegin() const{ return Collection< Utils::SharedSmartPointer< DataRow<T> > >::rbegin(); }
    inline RowIterator RowsREnd(){ return Collection< Utils::SharedSmartPointer< DataRow<T> > >::rend(); }
    inline ConstRowIterator RowsREnd() const{ return Collection< Utils::SharedSmartPointer< DataRow<T> > >::rend(); }

    /** Returns an iterator to the first row whose column equals the given value.
        If no match is found, then an iterator equal to the end is returned.
    */
    inline RowIterator FindRow(int column_index, const T &value){
        RowIterator iter(RowsBegin());
        for(; iter != RowsEnd(); ++iter)
            if(iter->operator[](column_index) == value) break;
        return iter;
    }

    /** Returns an iterator to the first row whose column equals the given value.
        If no match is found, then an iterator equal to the end is returned.
    */
    inline ConstRowIterator FindRow(int column_index, const T &value) const{
        ConstRowIterator iter(RowsBegin());
        for(; iter != RowsEnd(); ++iter)
            if(iter[column_index] == value) break;
        return iter;
    }

    /** Returns an iterator to the first row whose columns match the provided values.
        If no match is found, then an iterator equal to the end is returned.
    */
    inline RowIterator FindRow(const PairList<int, T> &values){
        RowIterator iter(RowsBegin());
        bool equals(false);
        for(; iter != RowsEnd(); ++iter)
        {
            equals = true;
            for(typename PairList<int, T>::const_iterator piter(values.begin());
                equals && piter != values.end(); ++piter)
            {
                if(iter[piter->First].Value() != piter->Second)
                    equals = false;
            }
            if(equals) break;
        }
        return iter;
    }

    /** Returns a const iterator to the first row whose columns match the provided values.
        If no match is found, then an iterator equal to the end is returned.
    */
    inline ConstRowIterator FindRow(const PairList<int, T> &values) const{
        ConstRowIterator iter(RowsBegin());
        bool equals(false);
        for(; !equals && iter != RowsEnd(); ++iter)
        {
            equals = true;
            for(typename PairList<int, T>::const_iterator piter(values.begin());
                equals && piter != values.end(); ++piter)
            {
                if(iter[piter->First] != piter->Second)
                    equals = false;
            }
            if(equals) break;
        }
        return iter;
    }

    /** Returns an iterator to the first row whose column equals the given value.
        If no match is found, then an iterator equal to the rend is returned.
    */
    inline RowIterator FindLastRow(int column_index, const T &value);
    /** Returns an iterator to the first row whose column equals the given value.
        If no match is found, then an iterator equal to the rend is returned.
    */
    inline ConstRowIterator FindLastRow(int column_index, const T &value) const;

    /** Returns an iterator to the first row whose columns match the provided values.
        If no match is found, then an iterator equal to the rend is returned.
    */
    inline RowIterator FindLastRow(const PairList<int, T> &values){
        RowIterator iter(RowsRBegin());
        bool equals(false);
        for(; !equals && iter != RowsREnd(); --iter)
        {
            equals = true;
            for(typename PairList<int, T>::const_iterator piter(values.begin());
                equals && piter != values.end(); ++piter)
            {
                if(iter[piter->First].Value() != piter->Second)
                    equals = false;
            }
        }
        return iter;
    }

    /** Returns a const iterator to the first row whose columns match the provided values.
        If no match is found, then an iterator equal to the rend is returned.
    */
    inline ConstRowIterator FindLastRow(const PairList<int, T> &values) const{
        ConstRowIterator iter(RowsRBegin());
        bool equals(false);
        for(; !equals && iter != RowsREnd(); --iter)
        {
            equals = true;
            for(typename PairList<int, T>::const_iterator piter(values.begin());
                equals && piter != values.end(); ++piter)
            {
                if(iter[piter->First] != piter->Second)
                    equals = false;
            }
        }
        return iter;
    }


    /** Creates a new row, which you can then add to the table with AddRow() */
    inline Utils::SharedSmartPointer< DataRow<T> > CreateRow() const{
        return new DataRow<T>(this, ColumnCount());
    }
    /** Creates a new row from a vector of values, which you can then add to the table with AddRow() */
    inline Utils::SharedSmartPointer< DataRow<T> > CreateRow(const Vector<T> &v) const{
        DataRow<T> *ret( new DataRow<T>(this, ColumnCount()) );
        for(int i(0); i < ColumnCount(); ++i)
            ret->operator [](i).SetValue(v[i]);
        return ret;
    }

    /** Creates a new blank row and adds it to the table. */
    inline Utils::SharedSmartPointer< DataRow<T> > AddRow(){
        DataRow<T> *dr( new DataRow<T>(this, ColumnCount()) );
        InsertRow(dr, RowCount());
        return dr;
    }
    /** Creates a new row from a vector of values and adds it to the table. */
    inline Utils::SharedSmartPointer< DataRow<T> > AddRow(const Vector<T> &v){
        Utils::SharedSmartPointer< DataRow<T> > dr( CreateRow(v) );
        InsertRow(dr.Data(), RowCount());
        return dr;
    }
    /** Adds a row created with CreateRow() to the table */
    inline Utils::SharedSmartPointer< DataRow<T> > AddRow(DataRow<T> *dr){
        InsertRow(dr, RowCount());
        return dr;
    }
    /** Inserts a row at a specific index */
    inline void InsertRow(DataRow<T> *dr, GUINT32 index){
        Collection< Utils::SharedSmartPointer< DataRow<T> > >::Insert(dr, index);
    }

    /** Removes the row at the given index */
    inline void RemoveRow(GUINT32 index){ Rows().Remove(index); }
    /** Removes the given row from the table */
    inline void RemoveRow(const DataRow<T> *dr){ Rows().RemoveOne(dr); }
    /** Removes the given row from the table */
    inline void RemoveRow(const RowIterator &iter){ RemoveRow(iter.Index()); }

    inline DataRow<T> &operator [](GUINT32 indx){ return *Collection< Utils::SharedSmartPointer< DataRow<T> > >::value_at(indx); }
    inline DataRow<T> const &operator [](GUINT32 indx) const{ return *Collection< Utils::SharedSmartPointer< DataRow<T> > >::operator [](indx); }
    inline DataRow<T> &operator [](GINT32 indx){ return *Collection< Utils::SharedSmartPointer< DataRow<T> > >::value_at(indx); }
    inline DataRow<T> const &operator [](GINT32 indx) const{ return *Collection< Utils::SharedSmartPointer< DataRow<T> > >::operator [](indx); }

    inline DataRow<T> &operator [](const String &col_name){
        if(!column_map.Contains(col_name)) THROW_NEW_GUTIL_EXCEPTION(IndexOutOfRangeException);
        return *Collection< Utils::SharedSmartPointer< DataRow<T> > >::operator [](column_map[col_name]);
    }
    DataRow<T> const &operator [](const String &col_name) const{
        if(!column_map.Contains(col_name)) THROW_NEW_GUTIL_EXCEPTION(IndexOutOfRangeException);
        return *Collection< Utils::SharedSmartPointer< DataRow<T> > >::operator [](column_map.At(col_name));
    }


    inline const DataColumnCollection &Columns() const{ return m_columns; }
    inline DataColumnCollection &Columns(){ return m_columns; }

    /** Returns a mapping of column keys to their indexes. */
    inline const Map<String, int> &ColumnMap() const{ return column_map; }


    inline void AddColumn(const String &key, const String &name = String())
    { InsertColumn(key, name, m_columns.Count()); }

    inline void InsertColumn(const DataColumn &dc, int index)
    { InsertColumn(dc.GetKey(), dc.GetName(), index); }
    inline void InsertColumn(const String &key, const String &name, int index){
        m_columns.Insert(DataColumn(key, name), index);
        column_map.Insert(key, index);
        for(int i(index + 1); i < m_columns.Count(); ++i)
            column_map[m_columns[i]->GetKey()] = i;
        for(typename List< Utils::SharedSmartPointer< DataRow<T> > >::iterator iter(RowsBegin());
            iter != RowsEnd(); ++iter)
            (*iter)->Insert(T(), index);
    }

    inline void RemoveColumn(int index){
        column_map.Remove(m_columns[index].Value().GetKey());
        m_columns.Remove(index);
    }
    inline void RemoveColumn(const String &name){
        if(!column_map.Contains(name)) THROW_NEW_GUTIL_EXCEPTION(IndexOutOfRangeException);
        RemoveColumn(column_map[name]);
    }


    /** Compares each element of the tables to see if they're equal. */
    bool operator == (const DataTable<T> &o) const{
        bool ret = false;
        if(RowCount() == o.RowCount() && ColumnCount() == o.ColumnCount())
        {
            ret = true;
            typename DataColumnCollection::const_iterator citer1(Columns().begin());
            typename DataColumnCollection::const_iterator citer2(o.Columns().begin());
            for(; ret && citer1 != Columns().end(); ++citer1, ++citer2)
            {
                if(citer1->GetKey() != citer2->GetKey() ||
                        citer1->GetName() != citer2->GetName())
                    ret = false;
            }

            ConstRowIterator iter1(RowsBegin());
            ConstRowIterator iter2(o.RowsBegin());
            for(; ret && iter1 != RowsEnd(); ++iter1, ++iter2)
            {
                typename DataRow<T>::const_iterator riter1(iter1->begin());
                typename DataRow<T>::const_iterator riter2(iter2->begin());
                for(; riter1 != iter1->end(); ++riter1, ++riter2)
                {
                    if(*riter1 != *riter2)
                    {
                        ret = false;
                        break;
                    }
                }
            }
        }
        return ret;
    }
    /** Compares each element of the tables to see if they're not equal. */
    bool operator != (const DataTable<T> &o) const{
        bool ret = true;
        if(RowCount() == o.RowCount() && ColumnCount() == o.ColumnCount())
        {
            ret = false;
            typename DataColumnCollection::const_iterator citer1(Columns().begin());
            typename DataColumnCollection::const_iterator citer2(o.Columns().begin());
            for(; !ret && citer1 != Columns().end(); ++citer1, ++citer2)
            {
                if(citer1->GetKey() != citer2->GetKey() ||
                        citer1->GetName() != citer2->GetName())
                    ret = true;
            }

            ConstRowIterator iter1(RowsBegin());
            ConstRowIterator iter2(o.RowsBegin());
            for(; !ret && iter1 != RowsEnd(); ++iter1, ++iter2)
            {
                typename DataRow<T>::const_iterator riter1(iter1->begin());
                typename DataRow<T>::const_iterator riter2(iter2->begin());
                for(; riter1 != iter1->end(); ++riter1, ++riter2)
                {
                    if(*riter1 != *riter2)
                    {
                        ret = true;
                        break;
                    }
                }
            }
        }
        return ret;
    }


protected:

    virtual void pre_add(Utils::SharedSmartPointer<DataRow<T> > &item, int indx){
        // Can do some validation on the row if you wish
        MakeDirty();
        item->SetTable(this);
    }

    virtual void commit_reject_changes(bool c){
        IUpdatable::commit_reject_changes(c);
        for(typename List< Utils::SharedSmartPointer< DataRow<T> > >::iterator iter(RowsBegin());
            iter != RowsEnd(); ++iter)
        {
            if((*iter)->IsDirty())
            {
                if(c)
                    (*iter)->CommitChanges();
                else
                    (*iter)->RejectChanges();
            }
        }
    }


private:

    DataColumnCollection m_columns;
    Map<String, int> column_map;
    String m_name;

    inline void _init_columns(const Vector<String> &keys){
        int col(0);
        for(typename Vector<String>::const_iterator iter(keys.begin());
            iter != keys.end(); ++iter, ++col)
            column_map.Insert(*iter, col);
    }

};





END_NAMESPACE_GUTIL1;

#endif // GUTIL_DATATABLE_H