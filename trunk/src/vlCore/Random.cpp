/**************************************************************************************/
/*                                                                                    */
/*  Visualization Library                                                             */
/*  http://www.visualizationlibrary.org                                               */
/*                                                                                    */
/*  Copyright (c) 2005-2010, Michele Bosi                                             */
/*  All rights reserved.                                                              */
/*                                                                                    */
/*  Redistribution and use in source and binary forms, with or without modification,  */
/*  are permitted provided that the following conditions are met:                     */
/*                                                                                    */
/*  - Redistributions of source code must retain the above copyright notice, this     */
/*  list of conditions and the following disclaimer.                                  */
/*                                                                                    */
/*  - Redistributions in binary form must reproduce the above copyright notice, this  */
/*  list of conditions and the following disclaimer in the documentation and/or       */
/*  other materials provided with the distribution.                                   */
/*                                                                                    */
/*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND   */
/*  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED     */
/*  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE            */
/*  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR  */
/*  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES    */
/*  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      */
/*  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON    */
/*  ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT           */
/*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS     */
/*  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                      */
/*                                                                                    */
/**************************************************************************************/

#include <vlCore/Random.hpp>
#include <vlCore/Time.hpp>
#include <vlCore/Log.hpp>
#include <cstdlib>

#if defined(VL_PLATFORM_WINDOWS)
  #include <wincrypt.h>
#endif

using namespace vl;

//-----------------------------------------------------------------------------
Random::Random()
{
  VL_DEBUG_SET_OBJECT_NAME()
#if defined(_MSC_VER) || defined(__MINGW32__)
  hCryptProv = NULL;
  if( !CryptAcquireContext( (HCRYPTPROV*)&hCryptProv, NULL, NULL, PROV_RSA_FULL, 0) )
    hCryptProv = NULL;
#endif
}
//-----------------------------------------------------------------------------
Random::~Random()
{
#if defined(_MSC_VER) || defined(__MINGW32__)
  if( hCryptProv  )
    CryptReleaseContext( (HCRYPTPROV)hCryptProv, 0 );
#endif
}
//-----------------------------------------------------------------------------
bool Random::fillRandom(void* ptr, size_t bytes) const
{
#if defined(_MSC_VER) || defined(__MINGW32__)
  if( !(hCryptProv && CryptGenRandom( (HCRYPTPROV)hCryptProv, bytes, (BYTE*)ptr)) )
  {
    standardFillRandom(ptr, bytes);
    return false;
  }
  else
    return true;
#elif defined(__GNUG__) && !defined(__MINGW32__)
  FILE* fin = fopen("/dev/urandom", "rb");
  if (fin)
  {
    if ( fread(ptr, 1, bytes, fin) == bytes )
    {
      fclose(fin);
      return true;
    }
  }
  standardFillRandom(ptr, bytes);
  return false;
#else
  standardFillRandom(ptr, bytes);
  return false;
#endif
}
//-----------------------------------------------------------------------------
void Random::standardFillRandom(void* ptr, size_t bytes)
{
  unsigned char* cptr = (unsigned char*)ptr;
  memset(cptr, 0, bytes);
  for (size_t i=0; i<bytes; ++i)
  {
    unsigned int r = (unsigned int)rand();
    cptr[i] ^= (r>>0)  & 0xFF;
    cptr[i] ^= (r>>8)  & 0xFF;
    cptr[i] ^= (r>>16) & 0xFF;
    cptr[i] ^= (r>>12) & 0xFF;
  }

  vl::Log::warning("Random::standardFillRandom() is being used.\n");
}
//-----------------------------------------------------------------------------
void Random::standardRandomize()
{
  Time time;
  int stack_pos = 0;
  static int static_pos = 0;
  int* dyn_pos = new int[10]; delete [] dyn_pos;
  unsigned int rand_start = time.microsecond() ^ time.second() ^ time.minute() ^ time.hour() ^ 
                            time.dayOfMonth() ^ time.month() ^ time.year() ^ 
                            (unsigned long long)&static_pos ^ (unsigned long long)&stack_pos ^ (unsigned long long)dyn_pos;
  srand(rand_start);
}
//-----------------------------------------------------------------------------
