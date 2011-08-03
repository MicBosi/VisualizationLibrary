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

#ifndef Flags_INCLUDE_ONCE
#define Flags_INCLUDE_ONCE

namespace vl
{
  //! Simple class to manage flags in a type safe manner.
  template <typename T_Flag>
  class Flags
  {
    unsigned long long mFlags;

    template <typename T> friend Flags<T> operator|(T flag1, T flag2);

    template <typename T> friend Flags<T> operator&(T flag1, T flag2);

  public:
    typedef T_Flag flag_type;

  public:
    Flags(): mFlags(0)  { }

    unsigned long long flags() const { return mFlags; }

    void reset() { mFlags = 0; }

    Flags operator|(const Flags& flag) const
    {
      Flags other = *this;
      other.mFlags |= flag.mFlags;
      return other;
    }

    Flags operator&(const Flags& flag) const
    {
      Flags other = *this;
      other.mFlags &= flag.mFlags;
      return other;
    }

    Flags operator^(const Flags& flag) const
    {
      Flags other = *this;
      other.mFlags ^= flag.mFlags;
      return other;
    }

    Flags operator-(const Flags& flag) const
    {
      Flags other = *this;
      other.mFlags &= ~flag.mFlags;
      return other;
    }

    // ---

    Flags& set(T_Flag flag)
    {
      mFlags |= flag;
      return *this;
    }

    Flags& unset(T_Flag flag)
    {
      mFlags &= ~(unsigned long long)flag;
      return *this;
    }

    Flags& operator=(T_Flag flag)
    {
      mFlags = flag;
      return this;
    }

    Flags operator|(T_Flag flag) const
    {
      Flags other = *this;
      other.mFlags |= flag;
      return other;
    }

    Flags operator&(T_Flag flag) const
    {
      Flags other = *this;
      other.mFlags &= flag;
      return other;
    }

    Flags operator^(T_Flag flag) const
    {
      Flags other = *this;
      other.mFlags ^= flag;
      return other;
    }

    Flags operator-(T_Flag flag) const
    {
      Flags other = *this;
      other.unset(flag);
      return other;
    }

    operator bool() const 
    {
      return mFlags != 0;
    }

  };

  template <typename T>
  Flags<T> operator|(T flag1, T flag2)
  {
    Flags<T> flags;
    flags.mFlags = (unsigned long long)flag1 | (unsigned long long)flag2;
    return flags;
  }

  template <typename T>
  Flags<T> operator&(T flag1, T flag2)
  {
    Flags<T> flags;
    flags.mFlags = (unsigned long long)flag1 & (unsigned long long)flag2;
    return flags;
  }

}

// example:
// 
//  enum MyFlags
// {
//   Flag1 = 0x1,
//   Flag2 = 0x2,
//   Flag3 = 0x4,
// };
// 
// template<MyFlags> Flags<MyFlags> operator|(MyFlags flag1, MyFlags flag2);
// template<MyFlags> Flags<MyFlags> operator&(MyFlags flag1, MyFlags flag2);

#endif
