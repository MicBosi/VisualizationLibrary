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
  // Quaternion
  //-----------------------------------------------------------------------------
  /** Implements a Quaternion usually used to represent rotations and orientations. */
  template<typename T_scalar_type>
  class Quaternion
  {
  public:
    typedef T_scalar_type scalar_type;
    //-----------------------------------------------------------------------------
    //! Constructor.
    Quaternion()
    {
      setNoRotation();
    }
    //-----------------------------------------------------------------------------
    //! Copy-constructor.
    template<typename T>
    explicit Quaternion(const Quaternion<T>& quat)
    {
      mXYZW.x() = (T_scalar_type)quat.xyzw().x();
      mXYZW.y() = (T_scalar_type)quat.xyzw().y();
      mXYZW.z() = (T_scalar_type)quat.xyzw().z();
      mXYZW.w() = (T_scalar_type)quat.xyzw().w();
    }
    //-----------------------------------------------------------------------------
    template<typename T>
    explicit Quaternion(T x, T y, T z, T w)
    {
      mXYZW.x() = (T_scalar_type)x;
      mXYZW.y() = (T_scalar_type)y;
      mXYZW.z() = (T_scalar_type)z;
      mXYZW.w() = (T_scalar_type)w;
    }
    //-----------------------------------------------------------------------------
    explicit Quaternion(T_scalar_type degrees, const Vector3<T_scalar_type>& axis)
    {
      fromAxisAngle(degrees, axis);
    }
    //-----------------------------------------------------------------------------
    template<typename T>
    explicit Quaternion(const Vector4<T>& v)
    {
      mXYZW.x() = (T_scalar_type)v.x();
      mXYZW.y() = (T_scalar_type)v.y();
      mXYZW.z() = (T_scalar_type)v.z();
      mXYZW.w() = (T_scalar_type)v.w();
    }
    //-----------------------------------------------------------------------------
    template<typename T>
    Quaternion& operator=(const Vector4<T>& v)
    {
      mXYZW.x() = (T_scalar_type)v.x();
      mXYZW.y() = (T_scalar_type)v.y();
      mXYZW.z() = (T_scalar_type)v.z();
      mXYZW.w() = (T_scalar_type)v.w();
      return *this;
    }
    //-----------------------------------------------------------------------------
    bool operator==(const Quaternion& q) const
    {
      return x() == q.x() && y() == q.y() && z() == q.z() && w() == q.w();
    }
    //-----------------------------------------------------------------------------
    bool operator!=(const Quaternion& q) const
    {
      return !(*this == q);
    }
    //-----------------------------------------------------------------------------
    //! lexicographic ordering
    bool operator<(const Quaternion& other) const
    {
      if (x() != other.x())
        return x() < other.x();
      if (y() != other.y())
        return y() < other.y();
      if (z() != other.z())
        return z() < other.z();
      else
        return w() < other.w();
    }
    //-----------------------------------------------------------------------------
    Quaternion& setZero()
    {
      mXYZW.x() = 0;
      mXYZW.y() = 0;
      mXYZW.z() = 0;
      mXYZW.w() = 0;
      return *this;
    }
    //-----------------------------------------------------------------------------
    Quaternion& setNoRotation()
    {
      mXYZW.x() = 0;
      mXYZW.y() = 0;
      mXYZW.z() = 0;
      mXYZW.w() = 1;
      return *this;
    }
    //-----------------------------------------------------------------------------
    Quaternion& fromVectors(const Vector4<T_scalar_type>& from, const Vector4<T_scalar_type>& to)
    {
      *this = fromVectors( from.xyz(), to.xyz() );
      return *this;
    }
    //-----------------------------------------------------------------------------
    Quaternion& fromVectors(const Vector3<T_scalar_type>& from, const Vector3<T_scalar_type>& to);
    //-----------------------------------------------------------------------------
    //! Creates a quaternion representing the given rotation matrix.
    //! see also http://www.gamasutra.com/features/19980703/quaternions_01.htm \n
    Quaternion& fromMatrix(const Matrix4<T_scalar_type>& m);
    //-----------------------------------------------------------------------------
    Quaternion& fromEulerXYZ(T_scalar_type degX, T_scalar_type degY, T_scalar_type degZ );
    //-----------------------------------------------------------------------------
    Quaternion& fromEulerZYX(T_scalar_type degZ, T_scalar_type degY, T_scalar_type degX );
    //-----------------------------------------------------------------------------
    Quaternion& fromAxisAngle(T_scalar_type degrees, const Vector3<T_scalar_type>& axis);
    //-----------------------------------------------------------------------------
    //! Expects a unit length quaternion.
    void toAxisAngle( Vector3<T_scalar_type>& axis, T_scalar_type& angle ) const;
    //-----------------------------------------------------------------------------
    //! Expects a unit length quaternion.
    Matrix4<T_scalar_type> toMatrix() const;
    //-----------------------------------------------------------------------------
    const Vector4<T_scalar_type>& xyzw() const { return mXYZW; }
    //-----------------------------------------------------------------------------
    Vector4<T_scalar_type>& xyzw() { return mXYZW; }
    //-----------------------------------------------------------------------------
    T_scalar_type& x() { return mXYZW.x(); }
    //-----------------------------------------------------------------------------
    T_scalar_type& y() { return mXYZW.y(); }
    //-----------------------------------------------------------------------------
    T_scalar_type& z() { return mXYZW.z(); }
    //-----------------------------------------------------------------------------
    T_scalar_type& w() { return mXYZW.w(); }
    //-----------------------------------------------------------------------------
    const T_scalar_type& x() const { return mXYZW.x(); }
    //-----------------------------------------------------------------------------
    const T_scalar_type& y() const { return mXYZW.y(); }
    //-----------------------------------------------------------------------------
    const T_scalar_type& z() const { return mXYZW.z(); }
    //-----------------------------------------------------------------------------
    const T_scalar_type& w() const { return mXYZW.w(); }
    //-----------------------------------------------------------------------------
    Quaternion operator*(T_scalar_type val) const
    {
      Quaternion t = *this;
      t.x() *= val;
      t.y() *= val;
      t.z() *= val;
      t.w() *= val;
      return t;
    }
    //-----------------------------------------------------------------------------
    Quaternion& operator*=(T_scalar_type val)
    {
      x() *= val;
      y() *= val;
      z() *= val;
      w() *= val;
      return *this;
    }
    //-----------------------------------------------------------------------------
    Quaternion operator/(T_scalar_type val) const
    {
      Quaternion t = *this;
      val = (T_scalar_type)1.0 / val;
      t.x() *= val;
      t.y() *= val;
      t.z() *= val;
      t.w() *= val;
      return t;
    }
    //-----------------------------------------------------------------------------
    Quaternion& operator/=(T_scalar_type val)
    {
      val = (T_scalar_type)1.0 / val;
      x() *= val;
      y() *= val;
      z() *= val;
      w() *= val;
      return *this;
    }
    //-----------------------------------------------------------------------------
    Quaternion operator+(const Quaternion& q) const
    {
      Quaternion t = *this;
      t.x() += q.x();
      t.y() += q.y();
      t.z() += q.z();
      t.w() += q.w();
      return t; 
    }
    //-----------------------------------------------------------------------------
    Quaternion& operator+=(const Quaternion& q)
    {
      x() += q.x();
      y() += q.y();
      z() += q.z();
      w() += q.w();
      return *this;
    }
    //-----------------------------------------------------------------------------
    Quaternion operator-(const Quaternion& q) const
    {
      Quaternion t = *this;
      t.x() -= q.x();
      t.y() -= q.y();
      t.z() -= q.z();
      t.w() -= q.w();
      return t; 
    }
    //-----------------------------------------------------------------------------
    Quaternion& operator-=(const Quaternion& q)
    {
      x() -= q.x();
      y() -= q.y();
      z() -= q.z();
      w() -= q.w();
      return *this;
    }
    //-----------------------------------------------------------------------------
    Quaternion operator-() const
    {
      return Quaternion(-x(), -y(), -z(), -w());
    }
    //-----------------------------------------------------------------------------
    T_scalar_type dot(const Quaternion& q) const
    {
      return x()*q.x() + y()*q.y() + z()*q.z() + w()*q.w();
    }
    //-----------------------------------------------------------------------------
    T_scalar_type length() const  { return mXYZW.length(); }
    //-----------------------------------------------------------------------------
    const Quaternion& normalize() { mXYZW.normalize(); return *this; }
    //-----------------------------------------------------------------------------
    Quaternion getNormalized() const { Quaternion t = *this; t.normalize(); return t; }
    //-----------------------------------------------------------------------------
    T_scalar_type lengthSquared() const
    {
      return x()*x() + y()*y() + z()*z() + w()*w();
    }
    //-----------------------------------------------------------------------------
    Quaternion conjugate() const
    {
      return Quaternion(-x(), -y(), -z(), w());
    }
    //-----------------------------------------------------------------------------
    Quaternion inverse() const
    {
      return conjugate() / lengthSquared();
    }
    //-----------------------------------------------------------------------------
    //! Spherical linear interpolation of two quaternions.
    //! See also http://www.gamasutra.com/features/19980703/quaternions_01.htm \n
    //! Properties: NO commutative, YES torque-minimal, YES constant velocity.
    Quaternion slerp( T_scalar_type t, const Quaternion& a, const Quaternion& b ) const;
    //-----------------------------------------------------------------------------
    //! Spherical cubic interpolation of two quaternions
    Quaternion squad(T_scalar_type t, const Quaternion& a, const Quaternion& p, const Quaternion& q, const Quaternion& b) const
    {
      return slerp((T_scalar_type)2.0*t*((T_scalar_type)1.0-t), slerp(t,p,q), slerp(t,a,b));
    }
    //-----------------------------------------------------------------------------
    //! Normalized spherical interpolation of two quaternions.
    //! See also http://number-none.com/product/Understanding%20Slerp,%20Then%20Not%20Using%20It/ \n
    //! Properties: YES commutative, YES torque-minimal, NO constant velocity.
    Quaternion nlerp( T_scalar_type t, const Quaternion& a, const Quaternion& b ) const
    {
      Quaternion q;
      q = a + (b - a) * t;
      q.normalize();
      return q;
    }
    
  protected:
    Vector4<T_scalar_type> mXYZW;
  };
  //-----------------------------------------------------------------------------
  template<typename T_scalar_type>
  inline Quaternion<T_scalar_type> operator*(T_scalar_type r, const Quaternion<T_scalar_type>& q)
  {
    return q * r;
  }
  //-----------------------------------------------------------------------------
  template<typename T_scalar_type>
  inline Quaternion<T_scalar_type> operator*(const Quaternion<T_scalar_type>& q1, const Quaternion<T_scalar_type>& q2)
  {
    Quaternion<T_scalar_type> q;
    q.x() = q1.w() * q2.x() + q1.x() * q2.w() + q1.y() * q2.z() - q1.z() * q2.y();
    q.y() = q1.w() * q2.y() + q1.y() * q2.w() + q1.z() * q2.x() - q1.x() * q2.z();
    q.z() = q1.w() * q2.z() + q1.z() * q2.w() + q1.x() * q2.y() - q1.y() * q2.x();
    q.w() = q1.w() * q2.w() - q1.x() * q2.x() - q1.y() * q2.y() - q1.z() * q2.z();
    return q;
  }
  //-----------------------------------------------------------------------------
  // post multiplication
  template<typename T_scalar_type>
  inline Vector3<T_scalar_type> operator*(const Quaternion<T_scalar_type>&q, const Vector3<T_scalar_type>& v)
  {
    // Matrix conversion formula based implementation
    T_scalar_type x2 = q.x() * q.x();
    T_scalar_type y2 = q.y() * q.y();
    T_scalar_type z2 = q.z() * q.z();
    T_scalar_type xy = q.x() * q.y();
    T_scalar_type xz = q.x() * q.z();
    T_scalar_type yz = q.y() * q.z();
    T_scalar_type wx = q.w() * q.x();
    T_scalar_type wy = q.w() * q.y();
    T_scalar_type wz = q.w() * q.z();

    Vector3<T_scalar_type> r;
    r.x() = ( v.x()*(1.0f - 2.0f * (y2 + z2)) + v.y()*(2.0f * (xy - wz)) + v.z()*(2.0f * (xz + wy)) );
    r.y() = ( v.x()*(2.0f * (xy + wz)) + v.y()*(1.0f - 2.0f * (x2 + z2)) + v.z()*(2.0f * (yz - wx)) );
    r.z() = ( v.x()*(2.0f * (xz - wy)) + v.y()*(2.0f * (yz + wx)) + v.z()*(1.0f - 2.0f * (x2 + y2)) );
    return r;
  }
  //-----------------------------------------------------------------------------
  // post multiplication
  template<typename T_scalar_type>
  inline Vector4<T_scalar_type> operator*(const Quaternion<T_scalar_type>&q, const Vector4<T_scalar_type>& v)
  {
    return Vector4<T_scalar_type>( q * v.xyz(), v.w() );
  }
  //-----------------------------------------------------------------------------
  template<typename T_scalar_type>
  Quaternion<T_scalar_type>& Quaternion<T_scalar_type>::fromEulerXYZ(T_scalar_type degX, T_scalar_type degY, T_scalar_type degZ )
  {
    *this = Quaternion<T_scalar_type>(degX, Vector3<T_scalar_type>(1,0,0)) * Quaternion<T_scalar_type>(degY, Vector3<T_scalar_type>(0,1,0)) * Quaternion<T_scalar_type>(degZ, Vector3<T_scalar_type>(0,0,1));
    return *this;
  }
  //-----------------------------------------------------------------------------
  template<typename T_scalar_type>
  Quaternion<T_scalar_type>& Quaternion<T_scalar_type>::fromEulerZYX(T_scalar_type degZ, T_scalar_type degY, T_scalar_type degX )
  {
    *this = Quaternion<T_scalar_type>(degZ, Vector3<T_scalar_type>(0,0,1)) * Quaternion<T_scalar_type>(degY, Vector3<T_scalar_type>(0,1,0)) * Quaternion<T_scalar_type>(degX, Vector3<T_scalar_type>(1,0,0));
    return *this;
  }
  //-----------------------------------------------------------------------------
  template<typename T_scalar_type>
  Quaternion<T_scalar_type>& Quaternion<T_scalar_type>::fromMatrix(const Matrix4<T_scalar_type>& m)
  {
    T_scalar_type tr, s, q[4];

    int next[3] = {1, 2, 0};

    tr = m[0][0] + m[1][1] + m[2][2];

    // check the diagonal
    if (tr > 0.0) 
    {
      s = vl::sqrt(tr + (T_scalar_type)1.0);
      w() = s / (T_scalar_type)2.0;
      s = (T_scalar_type)0.5 / s;
      x() = (m[1][2] - m[2][1]) * s;
      y() = (m[2][0] - m[0][2]) * s;
      z() = (m[0][1] - m[1][0]) * s;
    } 
    else 
    {    
      // diagonal is negative
      int i, j, k;
      i = 0;
      if (m[1][1] > m[0][0]) i = 1;
      if (m[2][2] > m[i][i]) i = 2;
      j = next[i];
      k = next[j];

      s = vl::sqrt((m[i][i] - (m[j][j] + m[k][k])) + (T_scalar_type)1.0);

      q[i] = s * (T_scalar_type)0.5;

      if (s != 0.0) 
        s = (T_scalar_type)0.5 / s;

      q[3] = (m[j][k] - m[k][j]) * s;
      q[j] = (m[i][j] + m[j][i]) * s;
      q[k] = (m[i][k] + m[k][i]) * s;

      x() = q[0];
      y() = q[1];
      z() = q[2];
      w() = q[3];
    }

    return *this;
  }
  //-----------------------------------------------------------------------------
  template<typename T_scalar_type>
  Quaternion<T_scalar_type>& Quaternion<T_scalar_type>::fromAxisAngle(T_scalar_type degrees, const Vector3<T_scalar_type>& axis)
  {
    degrees *= (T_scalar_type)dDEG_TO_RAD;
    T_scalar_type sa2 = sin(degrees * (T_scalar_type)0.5);
    Vector3<T_scalar_type> na = axis;
    na.normalize();
    mXYZW.x() = na.x() * sa2;
    mXYZW.y() = na.y() * sa2;
    mXYZW.z() = na.z() * sa2;
    mXYZW.w() = cos(degrees * (T_scalar_type)0.5);
    return *this;
  }
  //-----------------------------------------------------------------------------
  template<typename T_scalar_type>
  void Quaternion<T_scalar_type>::toAxisAngle( Vector3<T_scalar_type>& axis, T_scalar_type& angle ) const
  {
    T_scalar_type iscale = inversesqrt( x()*x() + y()*y() + z()*z() );
    axis.x() = x() * iscale;
    axis.y() = y() * iscale;
    axis.z() = z() * iscale;
    VL_CHECK(w()>=-1.0 && w()<=+1.0)
    T_scalar_type tw = clamp(w(),(T_scalar_type)-1.0,(T_scalar_type)+1.0);
    angle = acos( tw ) * (T_scalar_type)2.0;
  }
  //-----------------------------------------------------------------------------
  template<typename T_scalar_type>
  Matrix4<T_scalar_type> Quaternion<T_scalar_type>::toMatrix() const
  {
    T_scalar_type x2 = x() * x();
    T_scalar_type y2 = y() * y();
    T_scalar_type z2 = z() * z();
    T_scalar_type xy = x() * y();
    T_scalar_type xz = x() * z();
    T_scalar_type yz = y() * z();
    T_scalar_type wx = w() * x();
    T_scalar_type wy = w() * y();
    T_scalar_type wz = w() * z();
   
    return Matrix4<T_scalar_type>( 
      (1.0f - 2.0f * (y2 + z2)), (2.0f * (xy + wz)),        (2.0f * (xz - wy)),        0.0f,
      (2.0f * (xy - wz)),        (1.0f - 2.0f * (x2 + z2)), (2.0f * (yz + wx)),        0.0f,
      (2.0f * (xz + wy)),        (2.0f * (yz - wx)),        (1.0f - 2.0f * (x2 + y2)), 0.0f,
      0.0f,                      0.0f,                      0.0f,                      1.0f );

  }
  //-----------------------------------------------------------------------------
  template<typename T_scalar_type>
  Quaternion<T_scalar_type> Quaternion<T_scalar_type>::slerp( T_scalar_type t, const Quaternion<T_scalar_type>& a, const Quaternion<T_scalar_type>& b ) const
  {
    T_scalar_type scale_a, scale_b, omega, sinom;
    T_scalar_type cosom = a.dot(b);
    
    Quaternion<T_scalar_type> b2(b);
    
    if ( cosom < 0.0 )
    { 
        cosom = -cosom; 
        b2 = -b;
    }

    // acos(x) is defined only for -1 ... +1
    if( cosom >= -1.0 && cosom <= 1.0)
    {
      omega = acos(cosom);
      sinom = sin(omega);
      scale_a = sin( ((T_scalar_type)1.0-t) * omega) / sinom;
      scale_b = sin(  t * omega) / sinom;
    }
    else
    {
      // linear interpolation for degenerate cases
      scale_a = (T_scalar_type)1.0 - t;
      scale_b = t;
    }

    return (a*scale_a) + (b2*scale_b);
  }
  //-----------------------------------------------------------------------------
  template<typename T_scalar_type>
  Quaternion<T_scalar_type>& Quaternion<T_scalar_type>::fromVectors(const Vector3<T_scalar_type>& from, const Vector3<T_scalar_type>& to)
  {
    Vector3<T_scalar_type> a,b;
    a = from;
    b = to;
    a.normalize();
    b.normalize();
    T_scalar_type cosa = vl::dot(a,b);
    cosa = clamp(cosa, (T_scalar_type)-1.0, (T_scalar_type)+1.0);
    if (cosa == 0.0)
      *this = Quaternion<T_scalar_type>().setNoRotation();
    else
    {
      Vector3<T_scalar_type> axis,n2;
      axis = cross(a,b);
      axis.normalize();
      T_scalar_type alpha = acos( cosa );
      *this = Quaternion<T_scalar_type>().fromAxisAngle(alpha*(T_scalar_type)dDEG_TO_RAD, axis);
    }
    return *this;
  }
  //-----------------------------------------------------------------------------
  typedef Quaternion<float>  fquat;
  typedef Quaternion<double> dquat;
  typedef Quaternion<Real>   quat;
  //-----------------------------------------------------------------------------
}

#endif
