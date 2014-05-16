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

#include "cryptopp_rng.h"
#include "cryptopp/osrng.h"

namespace
{

struct d_t
{
    CryptoPP::AutoSeededX917RNG<CryptoPP::AES> rng;
};

}


NAMESPACE_GUTIL;


CryptoPP_RNG::CryptoPP_RNG()
{
    G_D_INIT();
}

CryptoPP_RNG::~CryptoPP_RNG()
{
    G_D_UNINIT();
}

void CryptoPP_RNG::Fill(GBYTE *b, GUINT32 l)
{
    G_D;
    d->rng.GenerateBlock(b, l);
}


END_NAMESPACE_GUTIL;
