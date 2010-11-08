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

#ifndef IndexIterator_INCLUDE_ONCE
#define IndexIterator_INCLUDE_ONCE

#include <vl/Object.hpp>

namespace vl
{
//-----------------------------------------------------------------------------
// IndexIteratorAbstract
//-----------------------------------------------------------------------------
  class IndexIteratorAbstract: public Object
  {
  public:
    IndexIteratorAbstract(): mIndex(-1) {}

    int index() const { return mIndex; }
    virtual bool isEnd() const = 0;
    virtual bool next() = 0;

  protected:
    int mIndex;
  };
//-----------------------------------------------------------------------------
// IndexIterator
//-----------------------------------------------------------------------------
  class IndexIterator: public Object
  {
  public:
    void initialize(IndexIteratorAbstract* iterator) { mIterator = iterator; }
    int  index() { return mIterator->index(); }
    bool isEnd() { return mIterator->isEnd(); }
    bool next()  { return mIterator->next();  }

  protected:
    ref<IndexIteratorAbstract> mIterator;
  };
//-----------------------------------------------------------------------------
// IndexIteratorDrawArrays
//-----------------------------------------------------------------------------
  class IndexIteratorDrawArrays: public IndexIteratorAbstract
  {
  public:
    IndexIteratorDrawArrays()
    {
      initialize(0,0);
    }

    void initialize(int start, int count)
    {
      mStart  = start;
      mCount  = count;
      mCurPos = start;
      mIndex  = start;
    }

    virtual bool isEnd() const 
    { 
      return mCurPos == mStart + mCount; 
    }

    virtual bool next() 
    { 
      ++mCurPos; 
      mIndex = mCurPos;
      return true; 
    }

  protected:
    int mStart;
    int mCount;
    int mCurPos;
  };
//-----------------------------------------------------------------------------
// IndexIteratorElements
//-----------------------------------------------------------------------------
  template<class TArray>
  class IndexIteratorElements: public IndexIteratorAbstract
  {
  public:
    IndexIteratorElements()
    {
      initialize( NULL, NULL, NULL, 0, false, 0 );
    }

    void initialize( TArray* idx_array, const std::vector<GLint>* p_base_vertices, const std::vector<GLsizei>* p_vert_counts, 
                     int base_vert, bool prim_restart_on, unsigned int prim_restart_idx )
    {
      mArray = idx_array;
      mBaseVert = base_vert;
      mpBaseVertices = p_base_vertices;
      mpVertCounts   = p_vert_counts;
      mBaseCount     = 0;
      mBaseIdx       = 0;

      mPrimRestartEnabled = prim_restart_on;
      mPrimRestartIdx = prim_restart_idx;
      mCurPos = 0;
      if (mArray && mArray->size())
      {
        mIndex = mArray->at(0) + mBaseVert;
      }

      if (p_vert_counts)
      {
        VL_CHECK(p_base_vertices)
        VL_CHECK( p_base_vertices->size() == p_vert_counts->size() )

        mBaseCount = (*p_vert_counts)[mBaseIdx];

        mIndex = (*mpBaseVertices)[mBaseIdx];
      }
    }

    virtual bool isEnd() const 
    { 
      return mCurPos == (int)mArray->size(); 
    }

    virtual bool next() 
    {
      ++mCurPos;
      while( mCurPos < (int)mArray->size() && mArray->at(mCurPos) == mPrimRestartIdx && mPrimRestartEnabled  )
        ++mCurPos;
      if ( mCurPos < (int)mArray->size() )
      {
        mIndex = mArray->at(mCurPos) + mBaseVert;
        if (mpVertCounts)
        {
          VL_CHECK(mpBaseVertices)
          mBaseCount--;
          if (!mBaseCount)
          {
            mBaseIdx++;
            mBaseCount = (*mpVertCounts)[mBaseIdx];
          }
          mIndex += (*mpBaseVertices)[mBaseIdx];
        }

        return true; 
      }
      else
      {
        mIndex = -1;
        mCurPos = mArray->size();
        return false; 
      }
    }

  protected:
    ref<TArray> mArray;
    int mBaseVert;
    int mCurPos;
    bool mPrimRestartEnabled;
    unsigned int mPrimRestartIdx;
    const std::vector<GLint>* mpBaseVertices;
    const std::vector<GLsizei>* mpVertCounts;
    int mBaseCount;
    int mBaseIdx;
  };
//-----------------------------------------------------------------------------
}

#endif
