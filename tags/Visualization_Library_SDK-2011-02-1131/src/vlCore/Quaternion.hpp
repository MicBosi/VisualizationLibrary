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

#include <vlCore/GLSLmath.hpp>

namespace vl
{
  //-----------------------------------------------------------------------------
  // Quaternion
  //-----------------------------------------------------------------------------
  /** Implements a Quaternion usually used to represent rotations and orientations. */
  template<typename T_scalar>
  class Quaternion
  {
  public:
    typedef T_scalar scalar_type;
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
      mXYZW.x() = (T_scalar)quat.xyzw().x();
      mXYZW.y() = (T_scalar)quat.xyzw().y();
      mXYZW.z() = (T_scalar)quat.xyzw().z();
      mXYZW.w() = (T_scalar)quat.xyzw().w();
    }
    //-----------------------------------------------------------------------------
    //! Constructor.
    explicit Quaternion(T_scalar x, T_scalar y, T_scalar z, T_scalar w)
    {
      mXYZW.x() = x;
      mXYZW.y() = y;
      mXYZW.z() = z;
      mXYZW.w() = w;
    }
    //-----------------------------------------------------------------------------
    //! Axis-angle constructor.
    explicit Quaternion(T_scalar degrees, const Vector3<T_scalar>& axis)
    {
      setFromAxisAngle(axis, degrees);
    }
    //-----------------------------------------------------------------------------
    //! Constructor from vec4.
    explicit Quaternion(const Vector4<T_scalar>& v)
    {
      mXYZW.x() = (T_scalar)v.x();
      mXYZW.y() = (T_scalar)v.y();
      mXYZW.z() = (T_scalar)v.z();
      mXYZW.w() = (T_scalar)v.w();
    }
    //-----------------------------------------------------------------------------
    //! Assignment operator.
    Quaternion& operator=(const Quaternion& q)
    {
      mXYZW.x() = q.x();
      mXYZW.y() = q.y();
      mXYZW.z() = q.z();
      mXYZW.w() = q.w();
      return *this;
    }
    //-----------------------------------------------------------------------------
    //! Assignment operator for vec4
    Quaternion& operator=(const Vector4<T_scalar>& v)
    {
      mXYZW.x() = (T_scalar)v.x();
      mXYZW.y() = (T_scalar)v.y();
      mXYZW.z() = (T_scalar)v.z();
      mXYZW.w() = (T_scalar)v.w();
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
      return !operator==(q);
    }
    //-----------------------------------------------------------------------------
    //! Lexicographic ordering
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
    //! Returns the internal vec4 used to contain the xyzw the quaternion components.
    const Vector4<T_scalar>& xyzw() const { return mXYZW; }
    //-----------------------------------------------------------------------------
    //! Returns the internal vec4 used to contain the xyzw the quaternion components.
    Vector4<T_scalar>& xyzw() { return mXYZW; }
    //-----------------------------------------------------------------------------
    T_scalar& x() { return mXYZW.x(); }
    //-----------------------------------------------------------------------------
    T_scalar& y() { return mXYZW.y(); }
    //-----------------------------------------------------------------------------
    T_scalar& z() { return mXYZW.z(); }
    //-----------------------------------------------------------------------------
    T_scalar& w() { return mXYZW.w(); }
    //-----------------------------------------------------------------------------
    const T_scalar& x() const { return mXYZW.x(); }
    //-----------------------------------------------------------------------------
    const T_scalar& y() const { return mXYZW.y(); }
    //-----------------------------------------------------------------------------
    const T_scalar& z() const { return mXYZW.z(); }
    //-----------------------------------------------------------------------------
    const T_scalar& w() const { return mXYZW.w(); }
    //-----------------------------------------------------------------------------
    Quaternion operator*(T_scalar val) const
    {
      Quaternion t = *this;
      t.x() *= val;
      t.y() *= val;
      t.z() *= val;
      t.w() *= val;
      return t;
    }
    //-----------------------------------------------------------------------------
    Quaternion& operator*=(T_scalar val)
    {
      x() *= val;
      y() *= val;
      z() *= val;
      w() *= val;
      return *this;
    }
    //-----------------------------------------------------------------------------
    Quaternion operator/(T_scalar val) const
    {
      Quaternion t = *this;
      val = (T_scalar)1.0 / val;
      t.x() *= val;
      t.y() *= val;
      t.z() *= val;
      t.w() *= val;
      return t;
    }
    //-----------------------------------------------------------------------------
    Quaternion& operator/=(T_scalar val)
    {
      val = (T_scalar)1.0 / val;
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
    //! Returns the negated quaternion.
    Quaternion operator-() const
    {
      return Quaternion(-x(), -y(), -z(), -w());
    }
    //-----------------------------------------------------------------------------
    //! Sets all the components of the quaternion to zero.
    Quaternion& setZero()
    {
      mXYZW.x() = 0;
      mXYZW.y() = 0;
      mXYZW.z() = 0;
      mXYZW.w() = 0;
      return *this;
    }
    //-----------------------------------------------------------------------------
    //! Returns the zero quaternion.
    static Quaternion getZero()
    {
      return Quaternion().setZero();
    }
    //-----------------------------------------------------------------------------
    //! Returns the zero quaternion.
    static Quaternion& getZero(Quaternion& q)
    {
      return q.setZero();
    }
    //-----------------------------------------------------------------------------
    //! Set the quaternion to no-rotation, i.e. Quaternion(0,0,0,1).
    Quaternion& setNoRotation()
    {
      mXYZW.x() = 0;
      mXYZW.y() = 0;
      mXYZW.z() = 0;
      mXYZW.w() = 1;
      return *this;
    }
    //-----------------------------------------------------------------------------
    //! Returns the no-rotation quaternion, i.e. Quaternion(0,0,0,1).
    static Quaternion getNoRotation()
    {
      return Quaternion();
    }
    //-----------------------------------------------------------------------------
    //! Returns the no-rotation quaternion, i.e. Quaternion(0,0,0,1).
    static Quaternion& getNoRotation(Quaternion& q)
    {
      return q.setNoRotation();
    }
    //-----------------------------------------------------------------------------
    //! Sets the quaternion to represent the rotation transforming \p from into \p to.
    Quaternion& setFromVectors(const Vector3<T_scalar>& from, const Vector3<T_scalar>& to);
    //-----------------------------------------------------------------------------
    //! Sets the quaternion to represent the rotation transforming \p from into \p to.
    static Quaternion getFromVectors(const Vector3<T_scalar>& from, const Vector3<T_scalar>& to)
    {
      return Quaternion().setFromVectors(from, to);
    }
    //-----------------------------------------------------------------------------
    //! Sets the quaternion to represent the rotation transforming \p from into \p to.
    static Quaternion& getFromVectors(Quaternion& q, const Vector3<T_scalar>& from, const Vector3<T_scalar>& to)
    {
      return q.setFromVectors(from, to);
    }
    //-----------------------------------------------------------------------------
    //! Creates a quaternion representing the given rotation matrix.
    //! see also http://www.gamasutra.com/features/19980703/quaternions_01.htm \n
    Quaternion& setFromMatrix(const Matrix4<T_scalar>& m);
    //-----------------------------------------------------------------------------
    //! Converts the given rotation matrix into a quaternion.
    static Quaternion getFromMatrix(const Matrix4<T_scalar>& m)
    {
      return Quaternion().setFromMatrix(m);
    }
    //-----------------------------------------------------------------------------
    //! Converts the given rotation matrix into a quaternion.
    static Quaternion& getFromMatrix(Quaternion& q, const Matrix4<T_scalar>& m)
    {
      return q.setFromMatrix(m);
    }
    //-----------------------------------------------------------------------------
    //! Creates a quaternion representing the given rotation matrix.
    //! see also http://www.gamasutra.com/features/19980703/quaternions_01.htm \n
    Quaternion& setFromMatrix(const Matrix3<T_scalar>& m);
    //-----------------------------------------------------------------------------
    //! Converts the given rotation matrix into a quaternion.
    static Quaternion getFromMatrix(const Matrix3<T_scalar>& m)
    {
      return Quaternion().setFromMatrix(m);
    }
    //-----------------------------------------------------------------------------
    //! Converts the given rotation matrix into a quaternion.
    static Quaternion& getFromMatrix(Quaternion& q, const Matrix3<T_scalar>& m)
    {
      return q.setFromMatrix(m);
    }
    //-----------------------------------------------------------------------------
    Quaternion& setFromEulerXYZ(T_scalar degX, T_scalar degY, T_scalar degZ);
    //-----------------------------------------------------------------------------
    static Quaternion getFromEulerXYZ(T_scalar degX, T_scalar degY, T_scalar degZ)
    {
      return Quaternion().setFromEulerXYZ(degX, degY, degZ);
    }
    //-----------------------------------------------------------------------------
    static Quaternion& getFromEulerXYZ(Quaternion& q, T_scalar degX, T_scalar degY, T_scalar degZ)
    {
      return q.setFromEulerXYZ(degX, degY, degZ);
    }
    //-----------------------------------------------------------------------------
    Quaternion& setFromEulerZYX(T_scalar degZ, T_scalar degY, T_scalar degX);
    //-----------------------------------------------------------------------------
    static Quaternion getFromEulerZYX(T_scalar degZ, T_scalar degY, T_scalar degX)
    {
      return Quaternion().setFromEulerZYX(degZ, degY, degX);
    }
    //-----------------------------------------------------------------------------
    static Quaternion& getFromEulerZYX(Quaternion& q, T_scalar degZ, T_scalar degY, T_scalar degX)
    {
      return q.setFromEulerZYX(degZ, degY, degX);
    }
    //-----------------------------------------------------------------------------
    Quaternion& setFromAxisAngle(const Vector3<T_scalar>& axis, T_scalar degrees);
    //-----------------------------------------------------------------------------
    static Quaternion getFromAxisAngle(const Vector3<T_scalar>& axis, T_scalar degrees)
    {
      return Quaternion().setFromAxisAngle(axis, degrees);
    }
    //-----------------------------------------------------------------------------
    static Quaternion& getFromAxisAngle(Quaternion& q, const Vector3<T_scalar>& axis, T_scalar degrees)
    {
      return q.setFromAxisAngle(axis, degrees);
    }
    //-----------------------------------------------------------------------------
    //! Converts a quaternion to an axis-angle representation.
    void toAxisAngle( Vector3<T_scalar>& axis, T_scalar& degrees ) const;
    //-----------------------------------------------------------------------------
    //! Converts a quaternion to a 4x4 rotation matrix.
    Matrix4<T_scalar> toMatrix4() const;
    //-----------------------------------------------------------------------------
    //! Converts a quaternion to a 4x4 rotation matrix.
    Matrix4<T_scalar>& toMatrix4(Matrix4<T_scalar>&) const;
    //-----------------------------------------------------------------------------
    //! Converts a quaternion to a 3x3 rotation matrix.
    Matrix3<T_scalar> toMatrix3() const;
    //-----------------------------------------------------------------------------
    //! Converts a quaternion to a 3x3 rotation matrix.
    Matrix3<T_scalar>& toMatrix3(Matrix3<T_scalar>&) const;
    //-----------------------------------------------------------------------------
    //! Returns the dot product between a quaternion and the given quaternion.
    T_scalar dot(const Quaternion& q) const
    {
      return x()*q.x() + y()*q.y() + z()*q.z() + w()*q.w();
    }
    //-----------------------------------------------------------------------------
    //! Returns the length of a quaternion.
    T_scalar length() const  { return mXYZW.length(); }
    //-----------------------------------------------------------------------------
    //! Normalizes a quaternion.
    //! \p len returns the original length of the quaternion.
    Quaternion& normalize(T_scalar* len=NULL) { mXYZW.normalize(len); return *this; }
    //-----------------------------------------------------------------------------
    //! Returns the normalized version of a quaternion.
    //! \p len returns the original length of the quaternion.
    Quaternion getNormalized(T_scalar* len=NULL) const { Quaternion t = *this; t.normalize(len); return t; }
    //-----------------------------------------------------------------------------
    //! Returns the normalized version of a quaternion.
    //! \p len returns the original length of the quaternion.
    Quaternion& getNormalized(Quaternion& q, T_scalar* len=NULL) const { q = *this; q.normalize(len); return q; }
    //-----------------------------------------------------------------------------
    //! Returns the squared length of a quaternion.
    T_scalar lengthSquared() const
    {
      return x()*x() + y()*y() + z()*z() + w()*w();
    }
    //-----------------------------------------------------------------------------
    //! Returns the conjugate of a quaternion.
    Quaternion getConjugate() const
    {
      return Quaternion(-x(), -y(), -z(), w());
    }
    //-----------------------------------------------------------------------------
    //! Returns the conjugate of a quaternion.
    Quaternion& getConjugate(Quaternion& q) const
    {
      q = Quaternion(-x(), -y(), -z(), w());
      return q;
    }
    //-----------------------------------------------------------------------------
    //! Returns the inverse of a quaternion.
    Quaternion getInverse() const
    {
      return getConjugate() / lengthSquared();
    }
    //-----------------------------------------------------------------------------
    //! Returns the inverse of a quaternion.
    Quaternion& getInverse(Quaternion& q) const
    {
      q = getConjugate() / lengthSquared();
      return q;
    }
    //-----------------------------------------------------------------------------
    //! Spherical linear interpolation of two quaternions.
    //! See also http://www.gamasutra.com/features/19980703/quaternions_01.htm \n
    //! Properties: NO commutative, YES torque-minimal, YES constant velocity.
    static Quaternion getSlerp(T_scalar t, const Quaternion& a, const Quaternion& b);
    //-----------------------------------------------------------------------------
    //! Spherical linear interpolation of two quaternions.
    //! See also http://www.gamasutra.com/features/19980703/quaternions_01.htm \n
    //! Properties: NO commutative, YES torque-minimal, YES constant velocity.
    static Quaternion& getSlerp(Quaternion& out, T_scalar t, const Quaternion& a, const Quaternion& b);
    //-----------------------------------------------------------------------------
    //! Spherical cubic interpolation of two quaternions
    static Quaternion getSquad(T_scalar t, const Quaternion& a, const Quaternion& p, const Quaternion& q, const Quaternion& b)
    {
      return getSlerp((T_scalar)2.0*t*((T_scalar)1.0-t), getSlerp(t,a,b), getSlerp(t,p,q));
    }
    //-----------------------------------------------------------------------------
    //! Spherical cubic interpolation of two quaternions
    static Quaternion& getSquad(Quaternion& out, T_scalar t, const Quaternion& a, const Quaternion& p, const Quaternion& q, const Quaternion& b)
    {
      return getSlerp(out, (T_scalar)2.0*t*((T_scalar)1.0-t), getSlerp(t,a,b), getSlerp(t,p,q));
    }
    //-----------------------------------------------------------------------------
    //! Normalized spherical interpolation of two quaternions.
    //! See also http://number-none.com/product/Understanding%20Slerp,%20Then%20Not%20Using%20It/ \n
    //! Properties: YES commutative, YES torque-minimal, NO constant velocity.
    static Quaternion getNlerp( T_scalar t, const Quaternion& a, const Quaternion& b )
    {
      Quaternion q = a + (b - a) * t;
      q.normalize();
      return q;
    }
    //-----------------------------------------------------------------------------
    //! Normalized spherical interpolation of two quaternions.
    //! See also http://number-none.com/product/Understanding%20Slerp,%20Then%20Not%20Using%20It/ \n
    //! Properties: YES commutative, YES torque-minimal, NO constant velocity.
    static Quaternion& getNlerp( Quaternion& out, T_scalar t, const Quaternion& a, const Quaternion& b )
    {
      out = a + (b - a) * t;
      out.normalize();
      return out;
    }
    //-----------------------------------------------------------------------------
    
  protected:
    Vector4<T_scalar> mXYZW;
  };
  //-----------------------------------------------------------------------------
  template<typename T_scalar>
  inline Quaternion<T_scalar> operator*(T_scalar r, const Quaternion<T_scalar>& q)
  {
    return q * r;
  }
  //-----------------------------------------------------------------------------
  template<typename T_scalar>
  inline Quaternion<T_scalar> operator*(const Quaternion<T_scalar>& q1, const Quaternion<T_scalar>& q2)
  {
    Quaternion<T_scalar> q;
    q.x() = q1.w() * q2.x() + q1.x() * q2.w() + q1.y() * q2.z() - q1.z() * q2.y();
    q.y() = q1.w() * q2.y() + q1.y() * q2.w() + q1.z() * q2.x() - q1.x() * q2.z();
    q.z() = q1.w() * q2.z() + q1.z() * q2.w() + q1.x() * q2.y() - q1.y() * q2.x();
    q.w() = q1.w() * q2.w() - q1.x() * q2.x() - q1.y() * q2.y() - q1.z() * q2.z();
    return q;
  }
  //-----------------------------------------------------------------------------
  // post multiplication
  template<typename T_scalar>
  inline Vector3<T_scalar> operator*(const Quaternion<T_scalar>&q, const Vector3<T_scalar>& v)
  {
    // Matrix conversion formula based implementation
    T_scalar x2 = q.x() * q.x();
    T_scalar y2 = q.y() * q.y();
    T_scalar z2 = q.z() * q.z();
    T_scalar xy = q.x() * q.y();
    T_scalar xz = q.x() * q.z();
    T_scalar yz = q.y() * q.z();
    T_scalar wx = q.w() * q.x();
    T_scalar wy = q.w() * q.y();
    T_scalar wz = q.w() * q.z();

    Vector3<T_scalar> r;
    r.x() = ( v.x()*(1.0f - 2.0f * (y2 + z2)) + v.y()*(2.0f * (xy - wz)) + v.z()*(2.0f * (xz + wy)) );
    r.y() = ( v.x()*(2.0f * (xy + wz)) + v.y()*(1.0f - 2.0f * (x2 + z2)) + v.z()*(2.0f * (yz - wx)) );
    r.z() = ( v.x()*(2.0f * (xz - wy)) + v.y()*(2.0f * (yz + wx)) + v.z()*(1.0f - 2.0f * (x2 + y2)) );
    return r;
  }
  //-----------------------------------------------------------------------------
  // post multiplication
  template<typename T_scalar>
  inline Vector4<T_scalar> operator*(const Quaternion<T_scalar>&q, const Vector4<T_scalar>& v)
  {
    return Vector4<T_scalar>( q * v.xyz(), v.w() );
  }
  //-----------------------------------------------------------------------------
  template<typename T_scalar>
  Quaternion<T_scalar>& Quaternion<T_scalar>::setFromEulerXYZ(T_scalar degX, T_scalar degY, T_scalar degZ )
  {
    *this = Quaternion<T_scalar>(degX, Vector3<T_scalar>(1,0,0)) * Quaternion<T_scalar>(degY, Vector3<T_scalar>(0,1,0)) * Quaternion<T_scalar>(degZ, Vector3<T_scalar>(0,0,1));
    return *this;
  }
  //-----------------------------------------------------------------------------
  template<typename T_scalar>
  Quaternion<T_scalar>& Quaternion<T_scalar>::setFromEulerZYX(T_scalar degZ, T_scalar degY, T_scalar degX )
  {
    *this = Quaternion<T_scalar>(degZ, Vector3<T_scalar>(0,0,1)) * Quaternion<T_scalar>(degY, Vector3<T_scalar>(0,1,0)) * Quaternion<T_scalar>(degX, Vector3<T_scalar>(1,0,0));
    return *this;
  }
  //-----------------------------------------------------------------------------
  template<typename T_scalar>
  Quaternion<T_scalar>& Quaternion<T_scalar>::setFromMatrix(const Matrix4<T_scalar>& m)
  {
    return setFromMatrix( m.get3x3() );
  }
  //-----------------------------------------------------------------------------
  template<typename T_scalar>
  Quaternion<T_scalar>& Quaternion<T_scalar>::setFromMatrix(const Matrix3<T_scalar>& m)
  {
    T_scalar tr, s, q[4];

    int next[3] = {1, 2, 0};

    tr = m.e(0,0) + m.e(1,1) + m.e(2,2);

    // check the diagonal
    if (tr + (T_scalar)1.0 > 0.0) 
    {
      s = vl::sqrt(tr + (T_scalar)1.0);
      w() = s / (T_scalar)2.0;
      s = (T_scalar)0.5 / s;
      x() = (m.e(2,1) - m.e(1,2)) * s;
      y() = (m.e(0,2) - m.e(2,0)) * s;
      z() = (m.e(1,0) - m.e(0,1)) * s;
    } 
    else 
    {    
      // diagonal is negative
      int i, j, k;
      i = 0;
      if (m.e(1,1) > m.e(0,0)) i = 1;
      if (m.e(2,2) > m.e(i,i)) i = 2;
      j = next[i];
      k = next[j];

      s = vl::sqrt((m.e(i,i) - (m.e(j,j) + m.e(k,k))) + (T_scalar)1.0);

      q[i] = s * (T_scalar)0.5;

      if (s != 0.0) 
        s = (T_scalar)0.5 / s;

      q[3] = (m.e(k,j) - m.e(j,k)) * s;
      q[j] = (m.e(j,i) + m.e(i,j)) * s;
      q[k] = (m.e(k,i) + m.e(i,k)) * s;

      x() = q[0];
      y() = q[1];
      z() = q[2];
      w() = q[3];
    }

    return *this;
  }
  //-----------------------------------------------------------------------------
  template<typename T_scalar>
  Quaternion<T_scalar>& Quaternion<T_scalar>::setFromAxisAngle( const Vector3<T_scalar>& axis, T_scalar degrees )
  {
    degrees *= (T_scalar)dDEG_TO_RAD;
    T_scalar sa2 = sin(degrees * (T_scalar)0.5);
    Vector3<T_scalar> na = axis;
    na.normalize();
    mXYZW.x() = na.x() * sa2;
    mXYZW.y() = na.y() * sa2;
    mXYZW.z() = na.z() * sa2;
    mXYZW.w() = cos(degrees * (T_scalar)0.5);
    return *this;
  }
  //-----------------------------------------------------------------------------
  template<typename T_scalar>
  void Quaternion<T_scalar>::toAxisAngle( Vector3<T_scalar>& axis, T_scalar& degrees ) const
  {
    T_scalar iscale = sqrt( x()*x() + y()*y() + z()*z() );
    if (iscale == 0)
    {
      axis.x() = 0;
      axis.y() = 0;
      axis.z() = 0;
      degrees  = 0;
    }
    else
    {
      iscale = T_scalar(1.0) / iscale;
      axis.x() = x() * iscale;
      axis.y() = y() * iscale;
      axis.z() = z() * iscale;
      VL_CHECK(w()>=-1.0 && w()<=+1.0)
      T_scalar tw = clamp(w(),(T_scalar)-1.0,(T_scalar)+1.0);
      degrees = acos( tw ) * (T_scalar)2.0 * (T_scalar)dRAD_TO_DEG;
    }
  }
  //-----------------------------------------------------------------------------
  template<typename T_scalar>
  Matrix4<T_scalar>& Quaternion<T_scalar>::toMatrix4(Matrix4<T_scalar>& out) const
  {
    T_scalar x2 = x() * x();
    T_scalar y2 = y() * y();
    T_scalar z2 = z() * z();
    T_scalar xy = x() * y();
    T_scalar xz = x() * z();
    T_scalar yz = y() * z();
    T_scalar wx = w() * x();
    T_scalar wy = w() * y();
    T_scalar wz = w() * z();

    return out = Matrix4<T_scalar>( 
      (1.0f - 2.0f * (y2 + z2)), (2.0f * (xy - wz)),        (2.0f * (xz + wy)),        0.0f,
      (2.0f * (xy + wz)),        (1.0f - 2.0f * (x2 + z2)), (2.0f * (yz - wx)),        0.0f,
      (2.0f * (xz - wy)),        (2.0f * (yz + wx)),        (1.0f - 2.0f * (x2 + y2)), 0.0f,
      0.0f,                      0.0f,                      0.0f,                      1.0f );
  }
  //-----------------------------------------------------------------------------
  template<typename T_scalar>
  Matrix4<T_scalar> Quaternion<T_scalar>::toMatrix4() const
  {
    Matrix4<T_scalar> out;
    return toMatrix4(out);
  }
  //-----------------------------------------------------------------------------
  template<typename T_scalar>
  Matrix3<T_scalar>& Quaternion<T_scalar>::toMatrix3(Matrix3<T_scalar>& out) const
  {
    T_scalar x2 = x() * x();
    T_scalar y2 = y() * y();
    T_scalar z2 = z() * z();
    T_scalar xy = x() * y();
    T_scalar xz = x() * z();
    T_scalar yz = y() * z();
    T_scalar wx = w() * x();
    T_scalar wy = w() * y();
    T_scalar wz = w() * z();

    return out = Matrix3<T_scalar>( 
      (1.0f - 2.0f * (y2 + z2)), (2.0f * (xy + wz)),        (2.0f * (xz - wy)),
      (2.0f * (xy - wz)),        (1.0f - 2.0f * (x2 + z2)), (2.0f * (yz + wx)),
      (2.0f * (xz + wy)),        (2.0f * (yz - wx)),        (1.0f - 2.0f * (x2 + y2)) );
  }
  //-----------------------------------------------------------------------------
  template<typename T_scalar>
  Matrix3<T_scalar> Quaternion<T_scalar>::toMatrix3() const
  {
    Matrix3<T_scalar> out;
    return toMatrix3(out);
  }
  //-----------------------------------------------------------------------------
  template<typename T_scalar>
  Quaternion<T_scalar> Quaternion<T_scalar>::getSlerp( T_scalar t, const Quaternion<T_scalar>& a, const Quaternion<T_scalar>& b )
  {
    Quaternion<T_scalar> q;
    getSlerp(q, t, a, b);
    return q;
  }
  //-----------------------------------------------------------------------------
  template<typename T_scalar>
  Quaternion<T_scalar>& Quaternion<T_scalar>::getSlerp( Quaternion<T_scalar>& out, T_scalar t, const Quaternion<T_scalar>& a, const Quaternion<T_scalar>& b )
  {
    T_scalar scale_a, scale_b, omega, sinom;
    T_scalar cosom = a.dot(b);

    Quaternion<T_scalar> b2(b);

    if ( cosom < 0 )
    { 
      cosom = -cosom; 
      b2 = -b;
    }

    // clamp rounding errors
    cosom = cosom > (T_scalar)1 ? (T_scalar)1 : cosom;

    if( cosom < (T_scalar)1.0 )
    {
      omega = acos(cosom);
      sinom = sin(omega);
      VL_CHECK(sinom != 0)
        scale_a = sin(((T_scalar)1.0-t) * omega) / sinom;
      scale_b = sin(t * omega) / sinom;
    }
    else
    {
      // linear interpolation for degenerate cases
      scale_a = (T_scalar)1.0 - t;
      scale_b = t;
    }

    return out = (a*scale_a) + (b2*scale_b);
  }
  //-----------------------------------------------------------------------------
  template<typename T_scalar>
  Quaternion<T_scalar>& Quaternion<T_scalar>::setFromVectors(const Vector3<T_scalar>& from, const Vector3<T_scalar>& to)
  {
    Vector3<T_scalar> a,b;
    a = from;
    b = to;
    a.normalize();
    b.normalize();
    Vector3<T_scalar> axis = cross(a,b);
    T_scalar len = 0;
    axis.normalize(&len);
    if(len)
    {
      T_scalar cosa = vl::dot(a,b);
      cosa = clamp(cosa, (T_scalar)-1.0, (T_scalar)+1.0);
      T_scalar alpha = acos( cosa );
      setFromAxisAngle(axis, alpha*(T_scalar)dRAD_TO_DEG);
    }
    else
      setNoRotation();
    return *this;
  }
  //-----------------------------------------------------------------------------
  typedef Quaternion<float>  fquat;
  typedef Quaternion<double> dquat;
  typedef Quaternion<Real>   quat;
  //-----------------------------------------------------------------------------
}

#endif
