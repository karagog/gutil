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

using namespace GUtil;

template <typename T> void Custom::GSharedDataPointer<T>::Lock()
{
    LockForWrite();
}

template <typename T> void Custom::GSharedDataPointer<T>::LockForRead()
{
    _lock->lockForRead();
}

template <typename T> void Custom::GSharedDataPointer<T>::LockForWrite()
{
    _lock->lockForWrite();
}

template <typename T> bool Custom::GSharedDataPointer<T>::TryLock()
{
    return TryLockForWrite();
}

template <typename T> bool Custom::GSharedDataPointer<T>::TryLockForRead(int timeout)
{
    bool ret;

    if(timeout == -1)
        ret = _lock->tryLockForRead();
    else
        ret = _lock->tryLockForRead(timeout);

    return ret;
}

template <typename T> bool Custom::GSharedDataPointer<T>::TryLockForWrite(int timeout)
{
    bool ret;

    if(timeout == -1)
        ret = _lock->tryLockForWrite();
    else
        ret = _lock->tryLockForWrite(timeout);

    return ret;
}
