/*Copyright 2010 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include "gsemaphore.h"
#include "Core/exception.h"
using namespace GUtil;

Custom::GSemaphore::GSemaphore(int init_val)
    :QSemaphore(init_val)
{
}

void Custom::GSemaphore::Up(int n)
{
    release(n);
}

void Custom::GSemaphore::Down(int n)
{
    acquire(n);
}

bool Custom::GSemaphore::IsEmpty()
{
    return available() == 0;
}
