/*Copyright 2011 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include "list_p.h"
GUTIL_USING_CORE_NAMESPACE(DataObjects);

list_p::list_p(list_p::type_wrapper *tw)
    :data_wrapper(tw),
      d(0),
      m_size(0),
      m_pageCount(0)
{}

list_p::~list_p()
{
    clear();
    delete data_wrapper;
}

list_p::type_wrapper::~type_wrapper(){}

void list_p::clear()
{
    for(int p(0); p < m_pageCount; ++p)
    {
        const int row_bound( 1 << p );
        for(int r(0); r < row_bound; ++r)
            data_wrapper->DeleteVoid(d[p][r]);
        delete[] d[p];
    }

    delete[] d;
    m_size = 0;
    m_pageCount = 0;
}

void list_p::reserve(GUINT32 l)
{
    GUINT32 cur;
    int new_pages(0);
    while((cur = capacity()) < l)
    {
        ++m_pageCount;
        ++new_pages;
    }

    if(new_pages > 0)
    {
        // Have to resize our page array
        void ***bkp( d );
        d = new void **[m_pageCount];

        // Copy page pointers from the old array into the new one.
        for(int i(0); i < m_pageCount - new_pages; i++)
            d[i] = bkp[i];

        // Delete the old page array
        delete []bkp;

        // Allocate the new pages' memory
        for(int m(0); m < new_pages; ++m)
        {
            int p( m_pageCount - new_pages + m );
            d[p] = new void *[capacity(p) + 1];
        }
    }
}

void list_p::insert(const void *v, GUINT32 i)
{
    if(i < 0 || i > m_size) THROW_NEW_GUTIL_EXCEPTION(IndexOutOfRangeException);
    if((i + 1) > capacity()) reserve(i + 1);

    void *mem( data_wrapper->CopyVoid(v) );
    while(i <= m_size)
    {
        void *cpy(mem);
        void **cur( pointer_at(i) );

        mem = *cur;
        *cur = cpy;
        ++i;
    };
}

void list_p::remove(GUINT32 i)
{

}

void *list_p::at(GUINT32 i) throw(IndexOutOfRangeException)
{
    if(i < 0 || i >= m_size) THROW_NEW_GUTIL_EXCEPTION(IndexOutOfRangeException);

    // In the algorithm it's easiest to have a 1-based index.
    int pindx( MSB64(++i) );
    return d[pindx][ TRUNCATE_LEFT_32(i, 32 - pindx) ];
}

void const*list_p::at(GUINT32 i) const throw(IndexOutOfRangeException)
{
    if(i < 0 || i >= m_size) THROW_NEW_GUTIL_EXCEPTION(IndexOutOfRangeException);

    // In the algorithm it's easiest to have a 1-based index.
    int pindx( MSB64(++i) );
    return d[pindx][ TRUNCATE_LEFT_32(i, 32 - pindx) ];
}

void **list_p::pointer_at(GUINT32 i) throw(IndexOutOfRangeException)
{
    if(i < 0 || i >= m_size) THROW_NEW_GUTIL_EXCEPTION(IndexOutOfRangeException);

    // In the algorithm it's easiest to have a 1-based index.
    int pindx( MSB64(++i) );
    return &d[pindx][ TRUNCATE_LEFT_32(i, 32 - pindx) ];
}
