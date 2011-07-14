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

#ifndef DistanceLODEvaluator_INCLUDE_ONCE
#define DistanceLODEvaluator_INCLUDE_ONCE

#include <vlGraphics/Actor.hpp>

namespace vl
{
  //-----------------------------------------------------------------------------
  // DistanceLODEvaluator
  //-----------------------------------------------------------------------------
  /**
   * A LODEvaluator that computes the appropriate LOD based on the distance of an 
   * Actor from the Camera.
   *
   * \sa
   * - LODEvaluator
   * - PixelLODEvaluator
  */
  class DistanceLODEvaluator: public LODEvaluator
  {
    VL_INSTRUMENT_CLASS(vl::DistanceLODEvaluator, LODEvaluator)

  public:
    DistanceLODEvaluator() 
    {
      VL_DEBUG_SET_OBJECT_NAME()
    }
    virtual int evaluate(Actor* actor, Camera* camera)
    {
      if (mDistanceRangeSet.empty())
        return 0;

      vec3 center = actor->transform() ? actor->transform()->worldMatrix() * actor->lod(0)->boundingBox().center() : actor->lod(0)->boundingBox().center();
      double dist = (camera->localMatrix().getT() - center).length();

      // we assume the distances are sorted in increasing order
      int i=0;
      for(; i<(int)mDistanceRangeSet.size(); ++i)
      {
        if (dist<mDistanceRangeSet[i])
          return i;
      }

      return i; // == mDistanceRangeSet.size()
    }

    const std::vector<double>& distanceRangeSet() { return mDistanceRangeSet; }
    void clearDistanceRangeSet() { mDistanceRangeSet.clear(); }
    int distanceRangeCount() const { return (int)mDistanceRangeSet.size(); }
    double distanceRange(int index) const { return mDistanceRangeSet[index]; }

    void setDistanceRangeSet(const std::vector<double>& distance_set) { mDistanceRangeSet = distance_set; std::sort(mDistanceRangeSet.begin(), mDistanceRangeSet.end()); }
    void addDistanceRange(double distance) { mDistanceRangeSet.push_back(distance); std::sort(mDistanceRangeSet.begin(), mDistanceRangeSet.end()); }
    void setDistanceRange(int index, double distance) { mDistanceRangeSet[index] = distance; std::sort(mDistanceRangeSet.begin(), mDistanceRangeSet.end()); }

  protected:
    std::vector<double> mDistanceRangeSet;
  };
}

#endif
