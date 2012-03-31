/**************************************************************************************/
/*                                                                                    */
/*  Visualization Library                                                             */
/*  http://www.visualizationlibrary.org                                               */
/*                                                                                    */
/*  Copyright (c) 2005-2011, Michele Bosi                                             */
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

#ifndef NaryQuickSet_INCLUDE_ONCE
#define NaryQuickSet_INCLUDE_ONCE

#include <vlCore/Object.hpp>

namespace vl
{
  /** Data structure designed to quickly (O(1)) add and remove and iterate elements in a set. 
      Internally used by VL to efficiently deal with enables and render stats. */
  template<typename NameType, typename DataType, int MaxSetSize>
  class NaryQuickSet: public Object
  {
  public:
    NaryQuickSet()
    {
      reset();
    }

    void reset()
    {
      mSetSize = 0;
      memset(mEnumToSet, 0, sizeof(mEnumToSet));
      memset(mSetToEnum, 0, sizeof(mSetToEnum));
    }

    void clear()
    {
      // no need to touch anything else.. you clever VL!
      mSetSize = 0;
    }

    int size() const { return mSetSize; }

    DataType* begin() { return mSet; }
    DataType* end() { return mSet + mSetSize; }
    const DataType* begin() const { return mSet; }
    const DataType* end() const { return mSet + mSetSize; }

    void append(NameType name)
    {
      append(name, name);
    }

    // use this when you know that 'name' is not already in the set.
    void append(NameType name, DataType data)
    {
      // sovrascrivi 'name' in ogni caso
      int pos = mSetSize++;
      mEnumToSet[name] = pos;
      mSetToEnum[pos] = name;
      mSet[pos] = data;
    }

    void insert(NameType name)
    {
      insert(name, name);
    }

    void insert(NameType name, DataType data)
    {
      VL_CHECK(name < MaxSetSize)
      // sovrascrivi 'name' in ogni caso
      int pos = find(name);
      if (pos == -1)
      {
        pos = mSetSize++;
        VL_CHECK(pos < MaxSetSize)
        mEnumToSet[name] = pos;
        mSetToEnum[pos] = name;
      }
      mSet[pos] = data;
    }

    void erase(NameType name)
    {
      int pos = find(name);
      if (pos != -1)
      {
        // move the last object to the one being erased
        if (mSetSize>1)
        {
          // move data
          mSet[pos] = mSet[mSetSize-1];
          // move Enum
          mSetToEnum[pos] = mSetToEnum[mSetSize-1];
          // mark moved NameType to point to the new pos
          mEnumToSet[mSetToEnum[pos]] = pos;
        }
        mSetSize--;
        VL_CHECK(mSetSize >= 0)
      }
    }

    int find(NameType name) const
    {
      int pos = mEnumToSet[name];
      VL_CHECK(pos >= 0)
      VL_CHECK(pos < MaxSetSize)
      if (pos < mSetSize)
      {
        NameType e = mSetToEnum[pos];
        VL_CHECK(e >= 0)
        VL_CHECK(e < MaxSetSize)
        if (e == name)
          return pos;
        else
          return -1;
      }
      else
        return -1;
    }

    bool has(NameType name) const
    {
      return find(name) != -1;
    }

    const DataType& get(NameType name) const
    {
        VL_CHECK(name >= 0)
        VL_CHECK(name < MaxSetSize)
        VL_CHECK(mEnumToSet[name] >= 0)
        VL_CHECK(mEnumToSet[name] < MaxSetSize)
        return mSet[mEnumToSet[name]];
    }

  protected:
    // the actual data
    // --> in the case of EEnable we don't really need this but 
    // --> we keep it to have one single implementation of this class
    DataType mSet[MaxSetSize]; 
    // the number of elements in mSet and mSetToEnum
    int mSetSize;
    // given an index (< mSetSize) of a data returns it's NameType (== index in mEnumToSet)
    NameType mSetToEnum[MaxSetSize];
    // given a NameType gives where in mSet and mSetToEnum the data is.
    int mEnumToSet[MaxSetSize]; 
  };

}

#endif
