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

#ifndef quat_INCLUDE_ONCE
#define quat_INCLUDE_ONCE

#include <vl/GLSLmath.hpp>

namespace vl
{
  //-----------------------------------------------------------------------------
  // quat
  //-----------------------------------------------------------------------------
  /**
   * The quat class implements a quaternion using vl::Real precision.
  */
  class quat
  {
  public:
    //-----------------------------------------------------------------------------
    quat()
    {
      setNoRotation();
    }
    //-----------------------------------------------------------------------------
    explicit quat(const vec4& v)
    {
      mXYZW.x() = v.x();
      mXYZW.y() = v.y();
      mXYZW.z() = v.z();
      mXYZW.w() = v.w();
    }
    //-----------------------------------------------------------------------------
    explicit quat(Real x, Real y, Real z, Real w)
    {
      mXYZW.x() = x;
      mXYZW.y() = y;
      mXYZW.z() = z;
      mXYZW.w() = w;
    }
    //-----------------------------------------------------------------------------
    explicit quat(Real degrees, const vec3& axis)
    {
      fromAxisAngle(degrees, axis);
    }
    //-----------------------------------------------------------------------------
    bool operator==(const quat& q) const
    {
      return x() == q.x() && y() == q.y() && z() == q.z() && w() == q.w();
    }
    //-----------------------------------------------------------------------------
    bool operator!=(const quat& q) const
    {
      return !(*this == q);
    }
    //-----------------------------------------------------------------------------
    //! lexicographic ordering
    bool operator<(const vec4& v) const
    {
      if (x() != v.x())
        return x() < v.x();
      if (y() != v.y())
        return y() < v.y();
      if (z() != v.z())
        return z() < v.z();
      else
        return w() < v.w();
    }
    //-----------------------------------------------------------------------------
    quat& setZero()
    {
      mXYZW.x() = 0;
      mXYZW.y() = 0;
      mXYZW.z() = 0;
      mXYZW.w() = 0;
      return *this;
    }
    //-----------------------------------------------------------------------------
    quat& setNoRotation()
    {
      mXYZW.x() = 0;
      mXYZW.y() = 0;
      mXYZW.z() = 0;
      mXYZW.w() = 1;
      return *this;
    }
    //-----------------------------------------------------------------------------
    quat& fromVectors(const vec4& from, const vec4& to)
    {
      *this = fromVectors( from.xyz(), to.xyz() );
      return *this;
    }
    //-----------------------------------------------------------------------------
    quat& fromVectors(const vec3& from, const vec3& to);
    //-----------------------------------------------------------------------------
    //! Creates a quaternion representing the given rotation matrix.
    //! see also http://www.gamasutra.com/features/19980703/quaternions_01.htm \n
    quat& fromMatrix(const mat4& m);
    //-----------------------------------------------------------------------------
    quat& fromEulerXYZ(Real degX, Real degY, Real degZ );
    //-----------------------------------------------------------------------------
    quat& fromEulerZYX(Real degZ, Real degY, Real degX );
    //-----------------------------------------------------------------------------
    quat& fromAxisAngle(Real degrees, const vec3& axis);
    //-----------------------------------------------------------------------------
    //! Expects a unit length quaternion.
    void toAxisAngle( vec4& axis, Real& angle ) const;
    //-----------------------------------------------------------------------------
    //! Expects a unit length quaternion.
    mat4 toMatrix() const;
    //-----------------------------------------------------------------------------
    const vec4& xyzw() const { return mXYZW; }
    //-----------------------------------------------------------------------------
    vec4& xyzw() { return mXYZW; }
    //-----------------------------------------------------------------------------
    Real& x() { return mXYZW.x(); }
    //-----------------------------------------------------------------------------
    Real& y() { return mXYZW.y(); }
    //-----------------------------------------------------------------------------
    Real& z() { return mXYZW.z(); }
    //-----------------------------------------------------------------------------
    Real& w() { return mXYZW.w(); }
    //-----------------------------------------------------------------------------
    const Real& x() const { return mXYZW.x(); }
    //-----------------------------------------------------------------------------
    const Real& y() const { return mXYZW.y(); }
    //-----------------------------------------------------------------------------
    const Real& z() const { return mXYZW.z(); }
    //-----------------------------------------------------------------------------
    const Real& w() const { return mXYZW.w(); }
    //-----------------------------------------------------------------------------
    quat operator*(Real real) const
    {
      quat t = *this;
      t.x() *= real;
      t.y() *= real;
      t.z() *= real;
      t.w() *= real;
      return t;
    }
    //-----------------------------------------------------------------------------
    quat& operator*=(Real real)
    {
      *this = *this * real;
      return *this;
    }
    //-----------------------------------------------------------------------------
    quat operator/(Real real) const
    {
      quat t = *this;
      real = (Real)1.0 / real;
      t.x() *= real;
      t.y() *= real;
      t.z() *= real;
      t.w() *= real;
      return t;
    }
    //-----------------------------------------------------------------------------
    quat& operator/=(Real real)
    {
      *this = *this / real;
      return *this;
    }
    //-----------------------------------------------------------------------------
    quat operator+(const quat& q) const
    {
      quat t = *this;
      t.x() += q.x();
      t.y() += q.y();
      t.z() += q.z();
      t.w() += q.w();
      return t; 
    }
    //-----------------------------------------------------------------------------
    quat& operator+=(const quat& q)
    {
      *this = *this + q;
      return *this;
    }
    //-----------------------------------------------------------------------------
    quat operator-(const quat& q) const
    {
      quat t = *this;
      t.x() -= q.x();
      t.y() -= q.y();
      t.z() -= q.z();
      t.w() -= q.w();
      return t; 
    }
    //-----------------------------------------------------------------------------
    quat& operator-=(const quat& q)
    {
      *this = *this - q;
      return *this;
    }
    //-----------------------------------------------------------------------------
    quat operator-() const
    {
      return quat(-x(), -y(), -z(), -w());
    }
    //-----------------------------------------------------------------------------
    Real dot(const quat& q) const
    {
      return x()*q.x() + y()*q.y() + z()*q.z() + w()*q.w();
    }
    //-----------------------------------------------------------------------------
    Real length() const  { return mXYZW.length(); }
    //-----------------------------------------------------------------------------
    const quat& normalize() { mXYZW.normalize(); return *this; }
    //-----------------------------------------------------------------------------
    quat getNormalized() const { quat t = *this; t.normalize(); return t; }
    //-----------------------------------------------------------------------------
    Real lengthSquared() const
    {
      return x()*x() + y()*y() + z()*z() + w()*w();
    }
    //-----------------------------------------------------------------------------
    quat conjugate() const
    {
      return quat(-x(), -y(), -z(), w());
    }
    //-----------------------------------------------------------------------------
    quat inverse() const
    {
      return conjugate() / lengthSquared();
    }
    //-----------------------------------------------------------------------------
    //! Spherical linear interpolation of two quaternions.
    //! See also http://www.gamasutra.com/features/19980703/quaternions_01.htm \n
    //! Properties: NO commutative, YES torque-minimal, YES constant velocity.
    quat slerp( Real t, const quat& a, const quat& b ) const;
    //-----------------------------------------------------------------------------
    //! Spherical cubic interpolation of two quaternions
    quat squad(Real t, const quat& a, const quat& p, const quat& q, const quat& b) const
    {
      return slerp((Real)2.0*t*((Real)1.0-t), slerp(t,p,q), slerp(t,a,b));
    }
    //-----------------------------------------------------------------------------
    //! Normalized spherical interpolation of two quaternions.
    //! See also http://number-none.com/product/Understanding%20Slerp,%20Then%20Not%20Using%20It/ \n
    //! Properties: YES commutative, YES torque-minimal, NO constant velocity.
    quat nlerp( Real t, const quat& a, const quat& b ) const
    {
      quat q;
      q = a + (b - a) * t;
      q.normalize();
      return q;
    }
    
  protected:
    vec4 mXYZW;
  };
  //-----------------------------------------------------------------------------
  // OPERATORS
  //-----------------------------------------------------------------------------
  inline quat operator*(Real r, const quat& q)
  {
    return q * r;
  }
  //-----------------------------------------------------------------------------
  inline quat operator*(const quat& q1, const quat& q2)
  {
    quat q;
    q.x() = q1.w() * q2.x() + q1.x() * q2.w() + q1.y() * q2.z() - q1.z() * q2.y();
    q.y() = q1.w() * q2.y() + q1.y() * q2.w() + q1.z() * q2.x() - q1.x() * q2.z();
    q.z() = q1.w() * q2.z() + q1.z() * q2.w() + q1.x() * q2.y() - q1.y() * q2.x();
    q.w() = q1.w() * q2.w() - q1.x() * q2.x() - q1.y() * q2.y() - q1.z() * q2.z();
    return q;
  }
  //-----------------------------------------------------------------------------
  // post multiplication
  inline vec3 operator*(const quat&q, const vec3& v)
  {
    // Matrix conversion formula based implementation
    Real x2 = q.x() * q.x();
    Real y2 = q.y() * q.y();
    Real z2 = q.z() * q.z();
    Real xy = q.x() * q.y();
    Real xz = q.x() * q.z();
    Real yz = q.y() * q.z();
    Real wx = q.w() * q.x();
    Real wy = q.w() * q.y();
    Real wz = q.w() * q.z();

    vec3 r;
    r.x() = ( v.x()*(1.0f - 2.0f * (y2 + z2)) + v.y()*(2.0f * (xy - wz)) + v.z()*(2.0f * (xz + wy)) );
    r.y() = ( v.x()*(2.0f * (xy + wz)) + v.y()*(1.0f - 2.0f * (x2 + z2)) + v.z()*(2.0f * (yz - wx)) );
    r.z() = ( v.x()*(2.0f * (xz - wy)) + v.y()*(2.0f * (yz + wx)) + v.z()*(1.0f - 2.0f * (x2 + y2)) );
    return r;
  }
  //-----------------------------------------------------------------------------
  // post multiplication
  inline vec4 operator*(const quat&q, const vec4& v)
  {
    return vec4( q * v.xyz(), v.w() );
  }
  //-----------------------------------------------------------------------------
}

#endif
