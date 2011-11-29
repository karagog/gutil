/*Copyright Copyright 2011 George Karagoulis
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef DATAROW_H
#define DATAROW_H

#include "DataObjects/DataSet/sharedrowdata.h"
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


GUTIL_BEGIN_NAMESPACE( DataObjects );


class TableData;
class DataRowCollection;

// Defines a row in a data table
class DataRow :
        public ExplicitlySharedObject<SharedRowData>,
        public Interfaces::IQXmlSerializable,
        public Core::Interfaces::IEquatable<DataRow>,
        public Core::Interfaces::IClonable<DataRow>,
        public Core::Interfaces::IReadOnlyObject
{
    friend class DataRowCollection;
    friend class DataColumnCollection;
    friend class RowData;
    friend class DataTable;
    friend class TableData;
public:

    DataRow(const DataRow &, bool clone = false);
    virtual ~DataRow(){}

    DataRow &operator =(const DataRow &);

    Custom::GVariant &operator [](int index);
    const Custom::GVariant &operator [](int index) const;
    Custom::GVariant &operator [](const QString &column_header);
    const Custom::GVariant &operator [](const QString &column_header) const;

    Custom::GVariant &At(int index);
    const Custom::GVariant &At(int index) const;

    int Index() const;
    int ColumnCount() const;

    inline bool IsDirty() const{
        return row_data().IsDirty();
    }


    // IEquatable interface:
    virtual bool Equals(const DataRow &) const;

    // IClonable interface:
    virtual DataRow &CloneTo(DataRow &) const;

    // IQXmlSerializable interface:
    virtual void WriteXml(QXmlStreamWriter &) const;
    virtual void ReadXml(QXmlStreamReader &)
            throw(Core::XmlException<true>);


protected:

    DataRow(TableData *,
            const Custom::GVariantVector &values = Custom::GVariantVector());
    DataRow(SharedRowData *);

    //void set_number_of_columns(int);
    void column_inserted(int);
    void column_removed(int);


    // Friend classes can access our data via these methods
    //   Note that derived classes should override these methods to provide
    //   access to their own version of the RowData
    SharedRowData &row_data();
    const SharedRowData &row_data() const;

    virtual void commit_reject_changes(bool commit);

};


GUTIL_END_NAMESPACE


#endif // DATAROW_H
