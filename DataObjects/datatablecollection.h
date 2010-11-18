/*Copyright 2010 George
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef DATATABLECOLLECTION_H
#define DATATABLECOLLECTION_H

#include "datatable.h"

GUTIL_BEGIN_NAMESPACE( DataObjects );


template <class DataTable> class DataTableCollectionBase :
        public Collection<DataTable>,
        public Core::Interfaces::IClonable< DataTableCollectionBase<DataTable> >
{
    friend class DataSet;
    friend class TableData;
    friend class SharedSetData;

public:

    DataSet Set() const;


protected:

    DataTableCollectionBase(SharedSetData *d){
        _set_data = d;
    }

    virtual ~DataTableCollectionBase(){}

    DataTableCollectionBase(const DataTableCollectionBase<DataTable> &o){
        o.CloneTo(*this);
    }

    // Protect our clonable interface
    virtual DataTableCollectionBase<DataTable> &CloneTo(DataTableCollectionBase<DataTable> &o) const{
        o.Clear();

        for(int i = 0; i < Collection<DataTable>::Size(); i++)
            o.Add(Collection<DataTable>::At(i).Clone());

        return o;
    }

    virtual void on_add(DataTable *t) const
    {
        // Remove ourselves from our parent set
        SharedSetData *sd;
        if((sd = t->table_data().SetData()))
            ((DataTableCollectionBase<DataTable> &)sd->Tables()).RemoveOne(*t);

        // Set our new set data to this one
        t->table_data().SetSetData(_set_data);
    }

    virtual void validate_new_item(const DataTable &t) const
            throw(Core::ValidationException)
    {
        if(Contains(t))
            THROW_NEW_GUTIL_EXCEPTION(Core::ValidationException,
                                      "Table already exists in data set");
    }

private:

    SharedSetData *_set_data;

};


// You can create your own versions of DataTableCollection by following this example:
typedef DataTableCollectionBase<DataTable> DataTableCollection;


GUTIL_END_NAMESPACE

#endif // DATATABLECOLLECTION_H

