/*Copyright 2010 George Karagoul
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0
        
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef IUPDATABLE_H
#define IUPDATABLE_H

namespace GUtil
{
    namespace Core
    {
        namespace Interfaces
        {
            class IUpdatable
            {
            public:
                IUpdatable(bool dirty = false);
                IUpdatable(const IUpdatable &);
                virtual ~IUpdatable();
                
                bool IsDirty() const;
                void SetDirty(bool);
                
                IUpdatable &operator =(const IUpdatable &);
                
            protected:
                virtual void onSetDirty(bool);

            private:
                bool _iupdatable_is_dirty;
            };
        }
    }
}

#endif // IUPDATABLE_H
