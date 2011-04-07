/**************************************************************************************/
/*                                                                                    */
/*  Visualization Library                                                             */
/*  http://www.visualizationlibrary.com                                               */
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

#ifndef Random_INCLUDE_ONCE
#define Random_INCLUDE_ONCE

#include <vlCore/Object.hpp>

namespace vl
{
  //! Random number generator.
  class Random: public Object
  {
  public:
    //! Constructor.
    Random();

    //! Destructor.
    virtual ~Random();

    //! Fills the specified buffer with random data generated using the best quality random number 
    //! generation facilities provided by the operating system. 
    //! Under Windows (including MinGW) \p CryptGenRandom is used, while under Unix-like operating systems \p /dev/urandom is used.
    //! If no special random number generation facility is detected the function falls back to standardFillRandom().
    //! \return This method returns \p false if the function had to fallback to standardFillRandom() otherwise returns \p true.
    virtual bool fillRandom(void* ptr, size_t bytes) const;

    //! Fills the specified buffer with random data generated using the standard rand() function.
    //! The method fillRandom() falls back to this function is no other high quality random number generation mechanism is detected.
    static void standardFillRandom(void* ptr, size_t bytes);

    //! Initializes the standard rand() random number generator using srand() with a reasonably unprobable value,
    //! based on the current time stamp and memory state.
    static void standardRandomize();

  private:
#if defined(_MSC_VER) || defined(__MINGW32__)
    void* hCryptProv;
#endif
  };
}

#endif
