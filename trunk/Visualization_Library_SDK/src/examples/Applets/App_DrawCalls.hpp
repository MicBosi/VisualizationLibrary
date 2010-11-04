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

#ifndef App_DrawCalls_INCLUDE_ONCE
#define App_DrawCalls_INCLUDE_ONCE

#include "BaseDemo.hpp"
#include "vlut/GeometryPrimitives.hpp"
#include "vl/SceneManagerActorTree.hpp"
#include "vl/Actor.hpp"
#include "vl/Effect.hpp"
#include "vl/Time.hpp"
#include "vl/Light.hpp"
#include "vl/RenderingTree.hpp"

namespace vl
{
  template<class TArray>
  class DrawElementsTriIterator
  {
  public:
    DrawElementsTriIterator(TArray* idx_array, EPrimitiveType prim_type, int base_vert, bool prim_restart_on, int prim_restart_idx)
    {
      mCurrentIndex = -1; // end
      mA = mB = mC = -1;
      mEven = true;
      mLastTriFanPoly = true;
      mIndex0 = 0;
      mEnd = 0;
      mArray            = idx_array;
      mPrimRestartIndex = prim_restart_idx;
      mPrimRestartOn    = prim_restart_on;
      mBaseVertex       = base_vert;
      mPrimType         = prim_type;
    }

    bool operator=(const DrawElementsTriIterator& other)
    {
      return mCurrentIndex == other.mCurrentIndex;
    }

    void initialize(int start=0, int end=-1)
    {
      VL_CHECK( start >= 0 )
      VL_CHECK( end <= (int)mArray->size() )

      if (end == -1)
        end = mArray->size();

      mCurrentIndex = -1; // end
      mA = mB = mC = -1;
      mEven = true;
      mLastTriFanPoly = true;
      mIndex0 = start;
      mEnd    = end;
      if (mArray->size())
      {
        switch(mPrimType)
        {
        case PT_TRIANGLES:
          mCurrentIndex = start;
          mA = mArray->at(start+0);
          mB = mArray->at(start+1);
          mC = mArray->at(start+2);
          break;
        case PT_TRIANGLE_STRIP:
          mCurrentIndex = start;
          mA = mArray->at(start+0);
          mB = mArray->at(start+1);
          mC = mArray->at(start+2);
          break;
        case PT_TRIANGLE_FAN:
          mCurrentIndex = start + 1;
          mA = mArray->at(start+0);
          mB = mArray->at(start+1);
          mC = mArray->at(start+2);
          break;
        case PT_POLYGON:
          mCurrentIndex = start + 1;
          mA = mArray->at(start+0);
          mB = mArray->at(start+1);
          mC = mArray->at(start+2);
          break;
        case PT_QUADS:
          mCurrentIndex = start;
          mA = mArray->at(start+0);
          mB = mArray->at(start+1);
          mC = mArray->at(start+2);
          break;
        case PT_QUAD_STRIP:
          mCurrentIndex = start;
          mA = mArray->at(start+0);
          mB = mArray->at(start+1);
          mC = mArray->at(start+2);
          break;
        }
      }
      // if we are not at the end then add base vertices
      if (mCurrentIndex != -1)
      {
        mA += mBaseVertex;
        mB += mBaseVertex;
        mC += mBaseVertex;
      }
      else
      {
        mA = mB = mC = -1;
      }
    }

    void next()
    {
      // reached the end
      if ( mCurrentIndex == -1 )
        return;

      switch(mPrimType)
      {

      case PT_TRIANGLES:
        mCurrentIndex += 3;
        // check for the end
        if ( mCurrentIndex >= mEnd )
          mCurrentIndex = -1;
        else
        if ( isPrimRestart(mCurrentIndex) )
        {
          mCurrentIndex += 1;
          mA = mArray->at(mCurrentIndex + 0);
          mB = mArray->at(mCurrentIndex + 1);
          mC = mArray->at(mCurrentIndex + 2);
        }
        else
        {
          mA = mArray->at(mCurrentIndex + 0);
          mB = mArray->at(mCurrentIndex + 1);
          mC = mArray->at(mCurrentIndex + 2);
        }
        break;

      case PT_QUAD_STRIP:
      case PT_TRIANGLE_STRIP:
        mCurrentIndex += 1;
        if ( mCurrentIndex + 2 >= mEnd )
          mCurrentIndex = -1;
        else
        if ( isPrimRestart(mCurrentIndex + 2) )
        {
          mCurrentIndex += 3;
          mEven = true;
          mA = mArray->at(mCurrentIndex + 0);
          mB = mArray->at(mCurrentIndex + 1);
          mC = mArray->at(mCurrentIndex + 2);
        }
        else
        {
          mEven = !mEven;
          if (mEven)
          {
            mA = mArray->at(mCurrentIndex + 0);
            mB = mArray->at(mCurrentIndex + 1);
            mC = mArray->at(mCurrentIndex + 2);
          }
          else
          {
            mA = mArray->at(mCurrentIndex + 0);
            mB = mArray->at(mCurrentIndex + 2);
            mC = mArray->at(mCurrentIndex + 1);
          }
        }
        break;

      case PT_TRIANGLE_FAN:
        mCurrentIndex += 1;
        if ( mCurrentIndex + 1 >= mEnd )
        {
          if (mLastTriFanPoly)
          {
            mA = mArray->at(mIndex0);
            mB = mArray->at(mCurrentIndex);
            mC = mArray->at(mIndex0 + 1);
            mLastTriFanPoly = false;
            mCurrentIndex--;
          }
          else
            mCurrentIndex = -1;
        }
        else
        if (isPrimRestart(mCurrentIndex + 1))
        {
          if (mLastTriFanPoly)
          {
            mA = mArray->at(mIndex0);
            mB = mArray->at(mCurrentIndex);
            mC = mArray->at(mIndex0 + 1);
            mLastTriFanPoly = false;
            mCurrentIndex--;
          }
          else
          {
            mLastTriFanPoly = true;
            mIndex0 = mCurrentIndex + 2;
            mCurrentIndex = mIndex0 + 1;
            mA = mArray->at(mIndex0);
            mB = mArray->at(mCurrentIndex + 0);
            mC = mArray->at(mCurrentIndex + 1);
          }
        }
        else
        {
          mA = mArray->at(mIndex0);
          mB = mArray->at(mCurrentIndex + 0);
          mC = mArray->at(mCurrentIndex + 1);
        }
        break;

      case PT_POLYGON:
        mCurrentIndex += 1;
        if ( mCurrentIndex + 1 >= mEnd )
        {
          mCurrentIndex = -1;
        }
        else
        if ( isPrimRestart(mCurrentIndex + 1) )
        {
          mIndex0 = mCurrentIndex + 2;
          mCurrentIndex = mIndex0 + 1;
          mA = mArray->at(mIndex0);
          mB = mArray->at(mCurrentIndex + 0);
          mC = mArray->at(mCurrentIndex + 1);
        }
        else
        {
          mA = mArray->at(mIndex0);
          mB = mArray->at(mCurrentIndex + 0);
          mC = mArray->at(mCurrentIndex + 1);
        }
        break;

      case PT_QUADS:
        mCurrentIndex += 2;
        if ( mCurrentIndex >= mEnd )
        {
          mCurrentIndex = -1;
        }
        else
        if ( isPrimRestart(mCurrentIndex) )
        {
          mCurrentIndex += 1;
          mEven = true;
          mA = mArray->at(mCurrentIndex+0);
          mB = mArray->at(mCurrentIndex+1);
          mC = mArray->at(mCurrentIndex+2);
        }
        else
        {
          mEven = !mEven;
          if ( mEven )
          {
            mA = mArray->at(mCurrentIndex+0);
            mB = mArray->at(mCurrentIndex+1);
            mC = mArray->at(mCurrentIndex+2);
          }
          else
          {
            mA = mArray->at(mCurrentIndex+0);
            mB = mArray->at(mCurrentIndex+1);
            mC = mArray->at(mCurrentIndex-2);
          }
        }
        break;
      }

      // if we are not at the end then add base vertices
      if (mCurrentIndex != -1)
      {
        mA += mBaseVertex;
        mB += mBaseVertex;
        mC += mBaseVertex;
      }
      else
      {
        mA = mB = mC = -1;
      }
    }

    void operator++() { next(); }

    bool isEnd() const { return mCurrentIndex == -1; }

  public:
    int a() const { return mA; }
    int b() const { return mB; }
    int c() const { return mC; }

  private:
    bool isPrimRestart(int i) const { return mPrimRestartOn && (int)mArray->at(i) == mPrimRestartIndex; }

  private:
    ref<TArray> mArray;
    int  mA, mB, mC;
    int  mCurrentIndex;
    int  mIndex0;
    int  mEnd;
    bool mEven;
    bool mLastTriFanPoly;
    bool mPrimRestartOn;
    int  mPrimRestartIndex;
    int  mBaseVertex;
    EPrimitiveType mPrimType;
  };
}

class App_DrawCalls: public BaseDemo
{
  void initEvent()
  {
    BaseDemo::initEvent();

    // fill the index buffer

    vl::ref<vl::DrawElementsUInt> de_u32 = new vl::DrawElementsUInt( vl::PT_POLYGON );

    // int idx[] = { 0,1,2,3,4,5, -1 };
    int idx[] = { 10,11,12,13, 0xFF, 20,21,22,23,24, 0xFF, 30,31,32,33,34, -1 };

    int count = 0;
    for(int i=0; idx[i] != -1; ++i)
      ++count;
    de_u32->indices()->resize( count );
    for(int i=0; idx[i] != -1; ++i)
      de_u32->indices()->at(i) = idx[i];

    // initialize the triangle iterator

    // mic fixme:
    // for multi-draw-elements: l'iteratore dovrebbe loopare tra i sub-draw-element ed usare questo.
    vl::DrawElementsTriIterator<vl::ArrayUInt> it(de_u32->indices(), de_u32->primitiveType(), 10, true, 0xFF );

    // query the triangle iterator

    for( it.initialize(); !it.isEnd(); ++it ) 
      printf("%d %d %d\n", it.a(), it.b(), it.c());

    printf("---\n");

    for( it.initialize(0,4); !it.isEnd(); ++it ) 
      printf("%d %d %d\n", it.a(), it.b(), it.c());

    for( it.initialize(5,10); !it.isEnd(); ++it ) 
      printf("%d %d %d\n", it.a(), it.b(), it.c());

    for( it.initialize(11,16); !it.isEnd(); ++it ) 
      printf("%d %d %d\n", it.a(), it.b(), it.c());

    exit(0);
  }

  virtual void run() {}

};

// Have fun!

#endif
