/*Copyright 2010 George
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef DATATABLECOLLECTION_H
#define DATATABLECOLLECTION_H

#include "gutil_macros.h"
#include "datatable.h"

GUTIL_BEGIN_NAMESPACE( DataObjects );


// You can create your own versions of DataTableCollection by following this example:

// These classes are defined elsewhere, but we don't want to force a reference to those headers,
//   so we declare these to represent the real classes
template <class T> class DataTableCollectionBase;

// This defines an alias, so if you make some derived version of DataTable, you can declare it
//   exactly in this way, substituting your derived class with 'DataTable' in the following line:
typedef DataTableCollectionBase<DataTable> DataTableCollection;


GUTIL_END_NAMESPACE

#endif // DATATABLECOLLECTION_H

