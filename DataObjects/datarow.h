/*Copyright 2010 George Karagoulis
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef DATAROW_H
#define DATAROW_H

#include "sharedrowdata.h"
#include "datarowcollectionbase.h"
#include "explicitlysharedobject.h"
#include "Interfaces/iqxmlserializable.h"
#include "Custom/gvariant.h"
#include "Core/Interfaces/iequatable.h"
#include "Core/Interfaces/iclonable.h"
#include "Core/Interfaces/iupdatable.h"
#include "Core/Interfaces/ireadonlyobject.h"
#include "gutil_macros.h"
#include <QVariant>

// Derived classes can use this macro for convenience when declaring
//  property accessors.  The type must have a cast operator defined in GVariant,
//  or you can define your own constructor from a GVariant if you have a custom type
#define ROW_PROPERTY( name, type, index ) \
    type Get##name() const{ return (*this)[index].value<type>(); } \
    void Set##name(const type &value){ (*this)[index].setValue(value); } \
    enum{}

#define READONLY_ROW_PROPERTY( name, type, index ) \
    type Get##name() const{ return (*this)[index].value<type>(); } \
            enum{}


namespace GUtil
{
    namespace DataObjects
    {
        // Defines a row in a data table

        class DataRow :
                protected ExplicitlySharedObject<SharedRowData>,
                public Interfaces::IQXmlSerializable,
                public Core::Interfaces::IEquatable<DataRow>,
                public Core::Interfaces::IClonable<DataRow>,
                public Core::Interfaces::IUpdatable,
                public Core::Interfaces::IReadOnlyObject
        {
            template<class T> friend class DataTableBase;
            template<class T> friend class DataRowCollectionBase;

        public:

            DataRow(const DataRow &);
            virtual ~DataRow();

            virtual DataRow &operator =(const DataRow &);
            bool operator ==(const DataRow &) const;
            bool operator !=(const DataRow &) const;

            Custom::GVariant &operator [](int index);
            const Custom::GVariant &operator [](int index) const;
            Custom::GVariant &operator [](const QString &column_header);
            const Custom::GVariant &operator [](const QString &column_header) const;

            Custom::GVariant &At(int index);
            const Custom::GVariant &At(int index) const;

            int Index() const;
            int ColumnCount() const;

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

        };
    }
}


#include "datatablebase.h"


#endif // DATAROW_H
