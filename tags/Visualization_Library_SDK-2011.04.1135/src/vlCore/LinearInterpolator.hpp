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

#ifndef LinearInterpolator_INCLUDE_ONCE
#define LinearInterpolator_INCLUDE_ONCE

#include <vlCore/Interpolator.hpp>

namespace vl
{
  /**
   * The LinearInterpolator class is a template class that implements linear interpolation.
   * \sa CatmullRomInterpolator, Interpolator and the \ref pagGuideInterpolators "Interpolators Tutorial" page.
   */
  template<typename T>
  class LinearInterpolator: public Object
  {
  public:
    virtual const char* className() { return "vl::LinearInterpolator"; }

    LinearInterpolator() 
    {
      #ifndef NDEBUG
        mObjectName = className();
      #endif
    }

    LinearInterpolator(const std::vector<T>& path): mPath(path) {}

    //! Samples the path at the given point. The \p t parameter must be in the range 0.0 ... 1.0 included.
    T computePoint(float t) const
    {
      t = vl::clamp(t,0.0f,1.0f);
      if (t == 0.0f)
        return mPath[0];
      if (t == 1.0f)
        return mPath.back();
      int p0 = int((mPath.size()-1)*t);
      int p1 = p0+1;
      if (p1 > (int)mPath.size()-1)
        p1 = (int)mPath.size()-1;
      float tt = (mPath.size()-1)*t - p0/*int((mPath.size()-1)*t)*/;
      return mPath[p0]*(1.0f-tt) + mPath[p1]*tt;
    }

    //! The control points defining the Catmull-Rom spline.
    //! Because of the Catmull-Rom formula the interpolated path must start and end with an extra control point 
    //! (one on each side) and cannot have less than 4 control points.
    void setPath(const std::vector<T>& path) { mPath = path; }
    //! The control points defining the Catmull-Rom spline.
    const std::vector<T>& path() const { return mPath; }
    //! The control points defining the Catmull-Rom spline.
    std::vector<T>& path() { return mPath; }

  protected:
    std::vector<T> mPath;
    std::vector<T> mLinearSpline;
  };

  typedef LinearInterpolator<float>     LinearInterpolatorFloat_T;
  typedef LinearInterpolator<vl::fvec2> LinearInterpolatorFVec2_T;
  typedef LinearInterpolator<vl::fvec3> LinearInterpolatorFVec3_T;
  typedef LinearInterpolator<vl::fvec4> LinearInterpolatorFVec4_T;
  typedef LinearInterpolator<double>    LinearInterpolatorDouble_T;
  typedef LinearInterpolator<vl::dvec2> LinearInterpolatorDVec2_T;
  typedef LinearInterpolator<vl::dvec3> LinearInterpolatorDVec3_T;
  typedef LinearInterpolator<vl::dvec4> LinearInterpolatorDVec4_T;

  //! Interpolates vl::fvec4 values using a LinearInterpolator.
  class LinearInterpolatorFVec4: public vl::InterpolatorFVec4
  {
  public:
    LinearInterpolatorFVec4(): mInterpolator( new LinearInterpolatorFVec4_T ) {}
    LinearInterpolatorFVec4(const std::vector<vl::fvec4>& path): mInterpolator( new LinearInterpolatorFVec4_T(path) ) {}
    vl::fvec4 computePoint(float t) const { return interpolator()->computePoint(t); }
    LinearInterpolatorFVec4_T* interpolator() { return mInterpolator.get(); }
    const LinearInterpolatorFVec4_T* interpolator() const { return mInterpolator.get(); }
    void setInterpolator(LinearInterpolatorFVec4_T* interpolator) { mInterpolator = interpolator; }
  protected:
    vl::ref<LinearInterpolatorFVec4_T> mInterpolator;
  };
  //! Interpolates vl::fvec3 values using a LinearInterpolator.
  class LinearInterpolatorFVec3: public vl::InterpolatorFVec3
  {
  public:
    LinearInterpolatorFVec3(): mInterpolator( new LinearInterpolatorFVec3_T ) {}
    LinearInterpolatorFVec3(const std::vector<vl::fvec3>& path): mInterpolator( new LinearInterpolatorFVec3_T(path) ) {}
    vl::fvec3 computePoint(float t) const { return interpolator()->computePoint(t); }
    LinearInterpolatorFVec3_T* interpolator() { return mInterpolator.get(); }
    const LinearInterpolatorFVec3_T* interpolator() const { return mInterpolator.get(); }
    void setInterpolator(LinearInterpolatorFVec3_T* interpolator) { mInterpolator = interpolator; }
  protected:
    vl::ref<LinearInterpolatorFVec3_T> mInterpolator;
  };
  //! Interpolates vl::fvec2 values using a LinearInterpolator.
  class LinearInterpolatorFVec2: public vl::InterpolatorFVec2
  {
  public:
    LinearInterpolatorFVec2(): mInterpolator( new LinearInterpolatorFVec2_T ) {}
    LinearInterpolatorFVec2(const std::vector<vl::fvec2>& path): mInterpolator( new LinearInterpolatorFVec2_T(path) ) {}
    vl::fvec2 computePoint(float t) const { return interpolator()->computePoint(t); }
    LinearInterpolatorFVec2_T* interpolator() { return mInterpolator.get(); }
    const LinearInterpolatorFVec2_T* interpolator() const { return mInterpolator.get(); }
    void setInterpolator(LinearInterpolatorFVec2_T* interpolator) { mInterpolator = interpolator; }
  protected:
    vl::ref<LinearInterpolatorFVec2_T> mInterpolator;
  };
  //! Interpolates \p float values using a LinearInterpolator.
  class LinearInterpolatorFloat: public vl::InterpolatorFloat
  {
  public:
    LinearInterpolatorFloat(): mInterpolator( new LinearInterpolatorFloat_T ) {}
    LinearInterpolatorFloat(const std::vector<float>& path): mInterpolator( new LinearInterpolatorFloat_T(path) ) {}
    float computePoint(float t) const { return interpolator()->computePoint(t); }
    LinearInterpolatorFloat_T* interpolator() { return mInterpolator.get(); }
    const LinearInterpolatorFloat_T* interpolator() const { return mInterpolator.get(); }
    void setInterpolator(LinearInterpolatorFloat_T* interpolator) { mInterpolator = interpolator; }
  protected:
    vl::ref<LinearInterpolatorFloat_T> mInterpolator;
  };
  //! Interpolates vl::dvec4 values using a LinearInterpolator.
  class LinearInterpolatorDVec4: public vl::InterpolatorDVec4
  {
  public:
    LinearInterpolatorDVec4(): mInterpolator( new LinearInterpolatorDVec4_T ) {}
    LinearInterpolatorDVec4(const std::vector<vl::dvec4>& path): mInterpolator( new LinearInterpolatorDVec4_T(path) ) {}
    vl::dvec4 computePoint(float t) const { return interpolator()->computePoint(t); }
    LinearInterpolatorDVec4_T* interpolator() { return mInterpolator.get(); }
    const LinearInterpolatorDVec4_T* interpolator() const { return mInterpolator.get(); }
    void setInterpolator(LinearInterpolatorDVec4_T* interpolator) { mInterpolator = interpolator; }
  protected:
    vl::ref<LinearInterpolatorDVec4_T> mInterpolator;
  };
  //! Interpolates vl::dvec3 values using a LinearInterpolator.
  class LinearInterpolatorDVec3: public vl::InterpolatorDVec3
  {
  public:
    LinearInterpolatorDVec3(): mInterpolator( new LinearInterpolatorDVec3_T ) {}
    LinearInterpolatorDVec3(const std::vector<vl::dvec3>& path): mInterpolator( new LinearInterpolatorDVec3_T(path) ) {}
    vl::dvec3 computePoint(float t) const { return interpolator()->computePoint(t); }
    LinearInterpolatorDVec3_T* interpolator() { return mInterpolator.get(); }
    const LinearInterpolatorDVec3_T* interpolator() const { return mInterpolator.get(); }
    void setInterpolator(LinearInterpolatorDVec3_T* interpolator) { mInterpolator = interpolator; }
  protected:
    vl::ref<LinearInterpolatorDVec3_T> mInterpolator;
  };
  //! Interpolates vl::dvec2 values using a LinearInterpolator.
  class LinearInterpolatorDVec2: public vl::InterpolatorDVec2
  {
  public:
    LinearInterpolatorDVec2(): mInterpolator( new LinearInterpolatorDVec2_T ) {}
    LinearInterpolatorDVec2(const std::vector<vl::dvec2>& path): mInterpolator( new LinearInterpolatorDVec2_T(path) ) {}
    vl::dvec2 computePoint(float t) const { return interpolator()->computePoint(t); }
    LinearInterpolatorDVec2_T* interpolator() { return mInterpolator.get(); }
    const LinearInterpolatorDVec2_T* interpolator() const { return mInterpolator.get(); }
    void setInterpolator(LinearInterpolatorDVec2_T* interpolator) { mInterpolator = interpolator; }
  protected:
    vl::ref<LinearInterpolatorDVec2_T> mInterpolator;
  };
  //! Interpolates \p double values using a LinearInterpolator.
  class LinearInterpolatorDouble: public vl::InterpolatorDouble
  {
  public:
    LinearInterpolatorDouble(): mInterpolator( new LinearInterpolatorDouble_T ) {}
    LinearInterpolatorDouble(const std::vector<double>& path): mInterpolator( new LinearInterpolatorDouble_T(path) ) {}
    double computePoint(float t) const { return interpolator()->computePoint(t); }
    LinearInterpolatorDouble_T* interpolator() { return mInterpolator.get(); }
    const LinearInterpolatorDouble_T* interpolator() const { return mInterpolator.get(); }
    void setInterpolator(LinearInterpolatorDouble_T* interpolator) { mInterpolator = interpolator; }
  protected:
    vl::ref<LinearInterpolatorDouble_T> mInterpolator;
  };
}

#endif
