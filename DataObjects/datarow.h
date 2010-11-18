/*Copyright 2010 George Karagoulis
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef DATAROW_H
#define DATAROW_H

#include "shareddataobjects.h"
#include "Interfaces/iqxmlserializable.h"
#include "Custom/gshareddatapointer.h"
#include "Core/Interfaces/iequatable.h"
#include "Core/Interfaces/iclonable.h"
#include "Core/Interfaces/iupdatable.h"
#include "gutil_macros.h"
#include <QVariant>

// Derived classes can use this macro for convenience when declaring
//  property accessors
#define ROW_ACCESSORS( v ) PROPERTY_ACCESSORS( QVariant, v )


namespace GUtil
{
    namespace DataObjects
    {
        // Defines a row in a data table

        class DataRow :
                public Interfaces::IQXmlSerializable,
                public Core::Interfaces::IEquatable<DataRow>,
                public Core::Interfaces::IClonable<DataRow>,
                public Core::Interfaces::IUpdatable
        {
            friend class DataTable;
            friend class DataRowCollection;

        public:

            DataRow(const DataRow &);
            virtual ~DataRow();

            virtual DataRow &operator =(const DataRow &);
            bool operator ==(const DataRow &) const;
            bool operator !=(const DataRow &) const;
            QVariant &operator [](int index);
            QVariant &operator [](const QString &column_header);

            QVariant At(int index) const;

            DataTable Table() const;

            int Index() const;
            int ColumnCount() const;

            DataRow Clone() const;

            // IEquatable interface:
            virtual bool Equals(const DataRow &) const;


            // IQXmlSerializable interface:
            virtual void WriteXml(QXmlStreamWriter &) const;
            virtual void ReadXml(QXmlStreamReader &)
                    throw(Core::XmlException);


        protected:

            DataRow(const DataTable &dt, const QVariantList &values = QVariantList());

            // Derived classes can call this constructor with their own derived
            //  version of the shared data object
            DataRow(SharedRowData *);

            // Friend classes can access our data via this method
            SharedRowData &row_data() const;

            // Derived classes will have to implement some extension of the
            //  read/write xml functions
            virtual void write_xml_protected(QXmlStreamWriter &) const{}
            virtual void read_xml_protected(QXmlStreamReader &){}

            // IClonable interface:
            virtual DataRow &CloneTo(DataRow &) const;

            void set_number_of_columns(int);


        private:

            Custom::GSharedDataPointer<SharedRowData> _row_data;

        };
    }
}

#endif // DATAROW_H
