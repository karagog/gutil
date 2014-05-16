/*Copyright 2014 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef GUTIL_CRYPTOPP_RNG_H
#define GUTIL_CRYPTOPP_RNG_H

#include "gutil_rng.h"

NAMESPACE_GUTIL;


class CryptoPP_RNG :
        public GUtil::AbstractRNG
{
    void *d;
public:
    CryptoPP_RNG();
    virtual ~CryptoPP_RNG();

    virtual void Fill(GBYTE *, GUINT32);
};


END_NAMESPACE_GUTIL;

#endif // GUTIL_CRYPTOPP_RNG_H
