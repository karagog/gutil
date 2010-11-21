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
#include "Interfaces/iqxmlserializable.h"
#include "Custom/gvariant.h"
#include "Custom/gshareddatapointer.h"
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
    type Get##name() const{ return (*this)[index]; } \
    void Set##name(const type &value){ (*this)[index] = value; } \
    enum{}

#define READONLY_ROW_PROPERTY( name, type, index ) \
    type Get##name() const{ return (*this)[index]; } \
            enum{}


namespace GUtil
{
    namespace DataObjects
    {
        // Defines a row in a data table

        class DataRow :
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
            DataRow(QSharedData *shared_row_data);



            // Derived classes must implement a deep copy for the shared data objects,
            //  if it's a derived class
            virtual void copy_shared_data(
                    Custom::GSharedDataPointer< QSharedData > dest) const;

            // Friend classes can access our data via these methods
            //   Note that derived classes should override these methods to provide
            //   access to their own version of the sharedRowData
            SharedRowData<DataRow> &row_data();
            const SharedRowData<DataRow> &row_data() const;


            // Derived classes may have to implement some extension of the
            //  read/write xml functions (only have to serialize the data that's
            //  not already serialized by the base DataRow class)
            virtual void write_xml_protected(QXmlStreamWriter &) const{}
            virtual void read_xml_protected(QXmlStreamReader &){}


        private:

            Custom::GSharedDataPointer< QSharedData > _row_data;

        };
    }
}

#endif // DATAROW_H
