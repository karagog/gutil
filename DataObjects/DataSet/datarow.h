/*Copyright 2010 George Karagoulis
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef DATAROW_H
#define DATAROW_H

#include "DataObjects/DataSet/sharedrowdata.h"
#include "DataObjects/DataSet/datarowcollectionbase.h"
#include "DataObjects/explicitlysharedobject.h"
#include "Interfaces/iqxmlserializable.h"
#include "Custom/updatablegvariant.h"
#include "Core/Interfaces/iequatable.h"
#include "Core/Interfaces/iclonable.h"
#include "Core/Interfaces/ireadonlyobject.h"
#include "gutil_macros.h"
#include <QSet>

// Derived classes can use this macro for convenience when declaring
//  typed property accessors.
#define ROW_PROPERTY( name, type, index ) \
    type Get##name() const{ return (*this)[index].value<type>(); } \
    void Set##name(const type &value){ (*this)[index] = value; } \
    enum{}

#define READONLY_ROW_PROPERTY( name, type, index ) \
    type Get##name() const{ return (*this)[index].value<type>(); } \
            enum{}

// Derived classes override the table accessors to refer to their
//   type of table to which they belong
#define DECLARE_PARENT_TABLE_TYPE( table_type ) \
    inline table_type &Table(){ return (table_type &)row_data().Table(); } \
    inline const table_type &Table() const{ return (const table_type &)row_data().Table(); } \
    enum{}


GUTIL_BEGIN_NAMESPACE( DataObjects );


// Defines a row in a data table

class DataRow :
        public ExplicitlySharedObject<SharedRowData>,
        public Interfaces::IQXmlSerializable,
        public Core::Interfaces::IEquatable<DataRow>,
        public Core::Interfaces::IClonable<DataRow>,
        public Core::Interfaces::IReadOnlyObject,
        protected Core::Interfaces::IUpdatable
{
    template<class T> friend class DataTableBase;
    template<class T> friend class DataRowCollectionBase;
    friend class RowValueCollection;
    friend class SharedRowData;

public:

    DataRow(const DataRow &);
    virtual ~DataRow();

    virtual DataRow &operator =(const DataRow &);

    Custom::UpdatableGVariant &operator [](int index);
    const Custom::UpdatableGVariant &operator [](int index) const;
    Custom::UpdatableGVariant &operator [](const QString &column_header);
    const Custom::UpdatableGVariant &operator [](const QString &column_header) const;

    Custom::UpdatableGVariant &At(int index);
    const Custom::UpdatableGVariant &At(int index) const;

    int Index() const;
    int ColumnCount() const;

    // Our row data is an IUpdatable
    inline bool IsDirty() const{
        bool ret = false;

        for(int i = 0; !ret && i < ColumnCount(); i++)
            ret = At(i).IsDirty();

        return ret;
    }

    // Derived classes can follow this example when specifying
    //  their parent table type
    DECLARE_PARENT_TABLE_TYPE( DataTable );

    // This is used only as a guard in our template classes, to guarantee
    //  that the template class is derived from DataRow.
    enum IsDerivedFromDataRow{ DerivedFromDataRow };

    // IEquatable interface:
    virtual bool Equals(const DataRow &) const;

    // IClonable interface:
    virtual DataRow &CloneTo(DataRow &) const;

    // IQXmlSerializable interface:
    virtual void WriteXml(QXmlStreamWriter &) const;
    virtual void ReadXml(QXmlStreamReader &)
            throw(Core::XmlException);


protected:

    DataRow(const DataTable &dt,
            const Custom::GVariantList &values = Custom::GVariantList());

    void set_number_of_columns(int);



    // If you are subclassing DataRow, then you will have to make use of
    //   the following methods

    // Derived classes can call this constructor with their own derived
    //  version of the shared data object (you don't necessarily have to
    //  derive your own version...)
    DataRow(SharedRowData *shared_row_data);

    // Friend classes can access our data via these methods
    //   Note that derived classes should override these methods to provide
    //   access to their own version of the sharedRowData
    SharedRowData &row_data();
    const SharedRowData &row_data() const;

    // This is where we check for primary key violations
    void row_value_about_to_change(int, const Custom::GVariant &);

    virtual void commit_reject_changes(bool commit);

};


GUTIL_END_NAMESPACE


#include "datatablebase.h"


#endif // DATAROW_H
