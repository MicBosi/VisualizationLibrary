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

#ifndef PixelLODEvaluator_INCLUDE_ONCE
#define PixelLODEvaluator_INCLUDE_ONCE

#include <vlGraphics/Actor.hpp>

namespace vl
{
  //-----------------------------------------------------------------------------
  // PixelLODEvaluator
  //-----------------------------------------------------------------------------
  /**
   * A LODEvaluator that computes the appropriate LOD based on the approximate
   * 2d area that an Actor covers on the screen.
   *
   * \sa
   * - LODEvaluator
   * - PixelLODEvaluator
  */
  class PixelLODEvaluator: public LODEvaluator
  {
  public:
    PixelLODEvaluator() 
    {
      #ifndef NDEBUG
        mObjectName = className();
      #endif
    }

    virtual const char* className() { return "PixelLODEvaluator"; }

    virtual int evaluate(Actor* actor, Camera* camera);

    const std::vector<float>& distanceRangeSet() { return mPixelRangeSet; }
    void clearPixelRangeSet() { mPixelRangeSet.clear(); }
    int distanceRangeCount() const { return (int)mPixelRangeSet.size(); }
    float distanceRange(int index) const { return mPixelRangeSet[index]; }

    void setPixelRangeSet(const std::vector<float>& distance_set);
    void addPixelRange(float distance);
    void setPixelRange(int index, float distance);

  protected:
    std::vector<float> mPixelRangeSet;
  };
}

#endif
