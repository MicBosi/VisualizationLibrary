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

#ifndef Matrix3_INCLUDE_ONCE
#define Matrix3_INCLUDE_ONCE

#include <vl/Vector3.hpp>
#include <vl/Matrix2.hpp>

namespace vl
{
  //-----------------------------------------------------------------------------
  // Matrix3
  //-----------------------------------------------------------------------------
  /**
   * The Matrix3 class is a template class that implements a generic 3x3 matrix, see also vl::dmat3, vl::fmat3, vl::umat3, vl::imat3.
   * \sa Vector4, Vector3, Vector2, Matrix4, Matrix2
   */
  template<typename scalar_type>
  class Matrix3
  {
  public:
    //-----------------------------------------------------------------------------
    template<typename T>
    explicit Matrix3(const T& m)
    {
      e(0,0) = (scalar_type)m.e(0,0); e(0,1) = (scalar_type)m.e(0,1); e(0,2) = (scalar_type)m.e(0,2);
      e(1,0) = (scalar_type)m.e(1,0); e(1,1) = (scalar_type)m.e(1,1); e(1,2) = (scalar_type)m.e(1,2);
      e(2,0) = (scalar_type)m.e(2,0); e(2,1) = (scalar_type)m.e(2,1); e(2,2) = (scalar_type)m.e(2,2);
    }
    //-----------------------------------------------------------------------------
    Matrix3()
    {
      static const scalar_type I3d[] = { 
        (scalar_type)1, (scalar_type)0, (scalar_type)0,  
        (scalar_type)0, (scalar_type)1, (scalar_type)0, 
        (scalar_type)0, (scalar_type)0, (scalar_type)1, 
      };
      memcpy( mVec,I3d,sizeof(scalar_type)*9 );
    }
    //-----------------------------------------------------------------------------
    Matrix3(scalar_type n)
    {
      static const scalar_type I3d[] = { 
        (scalar_type)1, (scalar_type)0, (scalar_type)0,  
        (scalar_type)0, (scalar_type)1, (scalar_type)0, 
        (scalar_type)0, (scalar_type)0, (scalar_type)1, 
      };
      memcpy( mVec,I3d,sizeof(scalar_type)*9 );
      e(0,0) = n; e(1,1) = n; e(2,2) = n;
    }
    //-----------------------------------------------------------------------------
    explicit Matrix3( scalar_type e00, scalar_type e01, scalar_type e02,
                      scalar_type e10, scalar_type e11, scalar_type e12,
                      scalar_type e20, scalar_type e21, scalar_type e22 )
    {
      e(0,0) = e00; e(0,1) = e01; e(0,2) = e02; 
      e(1,0) = e10; e(1,1) = e11; e(1,2) = e12; 
      e(2,0) = e20; e(2,1) = e21; e(2,2) = e22; 
    }
    //-----------------------------------------------------------------------------
    const Vector3<scalar_type>& operator[](unsigned int i) const { VL_CHECK(i<3); return mVec[i]; }
    //-----------------------------------------------------------------------------
    Vector3<scalar_type>& operator[](unsigned int i) { VL_CHECK(i<3); return mVec[i]; }
    //-----------------------------------------------------------------------------
    Vector2<scalar_type> getX() const
    {
      Vector2<scalar_type> v;
      v.x() = e(0,0);
      v.y() = e(0,1);
      return v;
    }
    //-----------------------------------------------------------------------------
    Vector2<scalar_type> getY() const
    {
      Vector2<scalar_type> v;
      v.x() = e(1,0);
      v.y() = e(1,1);
      return v;
    }
    //-----------------------------------------------------------------------------
    Vector2<scalar_type> getT() const
    {
      Vector2<scalar_type> v;
      v.x() = e(2,0);
      v.y() = e(2,1);
      return v;
    }
    //-----------------------------------------------------------------------------
    Matrix3& setX(const Vector2<scalar_type>& v) 
    {
      e(0,0) = v.x();
      e(0,1) = v.y();
      return *this;
    }
    //-----------------------------------------------------------------------------
    Matrix3& setY(const Vector2<scalar_type>& v) 
    {
      e(1,0) = v.x();
      e(1,1) = v.y();
      return *this;
    }
    //-----------------------------------------------------------------------------
    Matrix3& setT(const Vector2<scalar_type>& v) 
    {
      e(2,0) = v.x();
      e(2,1) = v.y();
      return *this;
    }
    //-----------------------------------------------------------------------------
    bool operator==(const Matrix3& m) const 
    {
      return memcmp(m.mVec, mVec, sizeof(scalar_type)*9) == 0;
    }
    //-----------------------------------------------------------------------------
    bool operator!=(const Matrix3& m) const 
    {
      return !(*this == m);
    }
    //-----------------------------------------------------------------------------
    Matrix3& operator=(const Matrix3& m) 
    {
      memcpy(mVec, m.mVec, sizeof(scalar_type)*9);
      return *this;
    }
    //-----------------------------------------------------------------------------
    Matrix3 operator+(const Matrix3& m) const
    {
      Matrix3 t;
      for(int i=0; i<3; ++i)
        for(int j=0; j<3; ++j)
          t[i][j] = e(i,j) + m[i][j];

      return t;
    }
    //-----------------------------------------------------------------------------
    Matrix3& operator+=(const Matrix3& m)
    {
      return *this = *this + m;
    }
    //-----------------------------------------------------------------------------
    Matrix3 operator-(const Matrix3& m) const
    {
      Matrix3 t;
      for(int i=0; i<3; ++i)
        for(int j=0; j<3; ++j)
          t[i][j] = e(i,j) - m[i][j];

      return t;
    }
    //-----------------------------------------------------------------------------
    Matrix3& operator-=(const Matrix3& m)
    {
      return *this = *this - m;
    }
    //-----------------------------------------------------------------------------
    Matrix3& operator*=(const Matrix3& m)
    {
      return *this = *this * m;
    }
    //-----------------------------------------------------------------------------
    Matrix3 operator-() const
    {
      Matrix3 t;
      for(int i=0; i<3; ++i)
        for(int j=0; j<3; ++j)
          t[i][j] = -e(i,j);

      return t;
    }
    //-----------------------------------------------------------------------------
    Matrix3 operator+(scalar_type d) const
    {
      Matrix3 t;
      for(int i=0; i<3; ++i)
        for(int j=0; j<3; ++j)
          t[i][j] = e(i,j) + d;

      return t;
    }
    //-----------------------------------------------------------------------------
    Matrix3& operator+=(scalar_type d)
    {
      return *this = *this + d;
    }
    //-----------------------------------------------------------------------------
    Matrix3 operator-(scalar_type d) const
    {
      Matrix3 t;
      for(int i=0; i<3; ++i)
        for(int j=0; j<3; ++j)
          t[i][j] = e(i,j) - d;

      return t;
    }
    //-----------------------------------------------------------------------------
    Matrix3& operator-=(scalar_type d)
    {
      return *this = *this - d;
    }
    //-----------------------------------------------------------------------------
    Matrix3 operator*(scalar_type d) const
    {
      Matrix3 t;
      for(int i=0; i<3; ++i)
        for(int j=0; j<3; ++j)
          t[i][j] = e(i,j) * d;

      return t;
    }
    //-----------------------------------------------------------------------------
    Matrix3& operator*=(scalar_type d)
    {
      Matrix3 t;
      for(int i=0; i<3; ++i)
        for(int j=0; j<3; ++j)
          e(i,j) *= d;

      return *this;
    }
    //-----------------------------------------------------------------------------
    Matrix3 operator/(scalar_type d) const
    {
      d = (scalar_type)1 / d;
      Matrix3 t;
      for(int i=0; i<3; ++i)
        for(int j=0; j<3; ++j)
          t[i][j] = e(i,j) * d;

      return t;
    }
    //-----------------------------------------------------------------------------
    Matrix3& operator/=(scalar_type d)
    {
      d = (scalar_type)1 / d;
      Matrix3 t;
      for(int i=0; i<3; ++i)
        for(int j=0; j<3; ++j)
          e(i,j) *= d;

      return *this;
    }
    //-----------------------------------------------------------------------------
    bool isIdentity() const
    {
      Matrix3 i;
      return memcmp( ptr(), i.ptr(), sizeof(scalar_type)*9 ) == 0;
    }
    //-----------------------------------------------------------------------------
    Matrix2<scalar_type> get2x2() const
    {
      Matrix2<scalar_type> t;
      t.e(0,0) = e(0,0); t.e(0,1) = e(0,1);
      t.e(1,0) = e(1,0); t.e(1,1) = e(1,1);
      return t;
    }
    //-----------------------------------------------------------------------------
    //! This writes only on the upper 2x2 part of the matrix without touching the last row and column. 
    void set2x2(const Matrix2<scalar_type>& m)
    {
      e(0,0) = m.e(0,0); e(0,1) = m.e(0,1);
      e(1,0) = m.e(1,0); e(1,1) = m.e(1,1);
    }
    //-----------------------------------------------------------------------------
    scalar_type* ptr()
    {
      return &e(0,0);
    }
    //-----------------------------------------------------------------------------
    const scalar_type* ptr() const
    {
      return &e(0,0);
    }
    //-----------------------------------------------------------------------------
    Matrix3& transpose()
    {
      Matrix3 m;
      for(int i=0; i<3; ++i)
        for(int j=0; j<3; ++j) {
          m.mVec[i][j] = mVec[j][i];
        }
      memcpy(mVec, m.mVec, sizeof(scalar_type)*9);
      return *this;
    }
    //-----------------------------------------------------------------------------
    bool isNull() const
    {
      for(int i=0; i<3; ++i)
        for(int j=0; j<3; ++j)
          if( mVec[j][i] != 0 )
            return false;
      return true;
    }
    //-----------------------------------------------------------------------------
    void setIdentity()
    {
      *this = Matrix3();
    }
    //-----------------------------------------------------------------------------
    Matrix3 inverse(bool *invertible=NULL) const;
    //-----------------------------------------------------------------------------
    Matrix3& invert(bool *invertible=NULL)
    {
      *this = inverse(invertible);
      return *this;
    }
    //-----------------------------------------------------------------------------
    const scalar_type& e(int i, int j) const { return mVec[i][j]; }
    //-----------------------------------------------------------------------------
    scalar_type& e(int i, int j) { return mVec[i][j]; }
    //-----------------------------------------------------------------------------
    static Matrix3 rotation( scalar_type degrees );
    //-----------------------------------------------------------------------------
    Matrix3& rotate( scalar_type degrees )
    {
      *this = rotation(degrees) * *this;
      return *this;
    }
    //-----------------------------------------------------------------------------
    static Matrix3 translation( const Vector2<scalar_type>& v )
    {
      return translation( v.x(), v.y() );
    }
    //-----------------------------------------------------------------------------
    static Matrix3 translation( scalar_type x, scalar_type y )
    {
      Matrix3 tr;
      tr.e(2,0) = x;
      tr.e(2,1) = y;
      return tr;
    }
    //-----------------------------------------------------------------------------
    Matrix3& translate( scalar_type x, scalar_type y )
    {
      *this = *this * translation(x,y);
      return *this;
    }
    //-----------------------------------------------------------------------------
    Matrix3& translate( const Vector2<scalar_type>& v )
    {
      *this = translation(v) * *this;
      return *this;
    }
    //-----------------------------------------------------------------------------
    static Matrix3 scaling( scalar_type x, scalar_type y )
    {
      Matrix3 sc;
      sc.e(0,0) = x;
      sc.e(1,1) = y;
      return sc;
    }
    //-----------------------------------------------------------------------------
    Matrix3& scale( scalar_type x, scalar_type y )
    {
      *this = scaling(x,y) * *this;
      return *this;
    }
    //-----------------------------------------------------------------------------

  protected:
    Vector3<scalar_type> mVec[3];
  };

  //-----------------------------------------------------------------------------
  // OPERATORS
  //-----------------------------------------------------------------------------
  template<typename scalar_type>
  inline Matrix3<scalar_type> operator*(const Matrix3<scalar_type>& m2, const Matrix3<scalar_type>& m1)
  {
    Matrix3<scalar_type> t;
    t.e(0,0) = m1.e(0,0)*m2.e(0,0) + m1.e(0,1)*m2.e(1,0) + m1.e(0,2)*m2.e(2,0);
    t.e(1,0) = m1.e(1,0)*m2.e(0,0) + m1.e(1,1)*m2.e(1,0) + m1.e(1,2)*m2.e(2,0);
    t.e(2,0) = m1.e(2,0)*m2.e(0,0) + m1.e(2,1)*m2.e(1,0) + m1.e(2,2)*m2.e(2,0);

    t.e(0,1) = m1.e(0,0)*m2.e(0,1) + m1.e(0,1)*m2.e(1,1) + m1.e(0,2)*m2.e(2,1);
    t.e(1,1) = m1.e(1,0)*m2.e(0,1) + m1.e(1,1)*m2.e(1,1) + m1.e(1,2)*m2.e(2,1);
    t.e(2,1) = m1.e(2,0)*m2.e(0,1) + m1.e(2,1)*m2.e(1,1) + m1.e(2,2)*m2.e(2,1);

    t.e(0,2) = m1.e(0,0)*m2.e(0,2) + m1.e(0,1)*m2.e(1,2) + m1.e(0,2)*m2.e(2,2);
    t.e(1,2) = m1.e(1,0)*m2.e(0,2) + m1.e(1,1)*m2.e(1,2) + m1.e(1,2)*m2.e(2,2);
    t.e(2,2) = m1.e(2,0)*m2.e(0,2) + m1.e(2,1)*m2.e(1,2) + m1.e(2,2)*m2.e(2,2);

    return t;
  }
  //-----------------------------------------------------------------------------
  template<typename scalar_type>
  inline Matrix3<scalar_type> operator+(scalar_type d, const Matrix3<scalar_type>& m)
  {
    return m + d;
  }
  //-----------------------------------------------------------------------------
  template<typename scalar_type>
  inline Matrix3<scalar_type> operator*(scalar_type d, const Matrix3<scalar_type>& m)
  {
    return m * d;
  }
  //-----------------------------------------------------------------------------
  //! Post multiplication: matrix * column vector
  template<typename scalar_type>
  inline Vector3<scalar_type> operator*(const Matrix3<scalar_type>& m, const Vector3<scalar_type>& v)
  {
    Vector3<scalar_type> t;
    t.x() = v.x()*m.e(0,0) + v.y()*m.e(1,0) + v.z()*m.e(2,0);
    t.y() = v.x()*m.e(0,1) + v.y()*m.e(1,1) + v.z()*m.e(2,1);
    t.z() = v.x()*m.e(0,2) + v.y()*m.e(1,2) + v.z()*m.e(2,2);
    return t;
  }
  //-----------------------------------------------------------------------------
  //! Post multiplication: matrix * column vector
  //! The incoming vector is considered a Vector3<scalar_type> with the component z = 0
  template<typename scalar_type>
  inline Vector2<scalar_type> operator*(const Matrix3<scalar_type>& m, const Vector2<scalar_type>& v)
  {
    Vector2<scalar_type> t;
    t.x() = v.x()*m.e(0,0) + v.y()*m.e(1,0) /*+ 0*m.e(2,0)*/;
    t.y() = v.x()*m.e(0,1) + v.y()*m.e(1,1) /*+ 0*m.e(2,1)*/;
    return t;
  }
  //-----------------------------------------------------------------------------
  //! Pre multiplication: row vector * matrix
  template<typename scalar_type>
  inline Vector3<scalar_type> operator*(const Vector3<scalar_type>& v, const Matrix3<scalar_type>& m)
  {
    Vector3<scalar_type> t;
    t.x() = v.x()*m.e(0,0) + v.y()*m.e(0,1) + v.z()*m.e(0,2);
    t.y() = v.x()*m.e(1,0) + v.y()*m.e(1,1) + v.z()*m.e(1,2);
    t.z() = v.x()*m.e(2,0) + v.y()*m.e(2,1) + v.z()*m.e(2,2);
    return t;
  }
  //-----------------------------------------------------------------------------
  //! Pre multiplication: row vector * matrix
  //! The incoming vector is considered a Vector3<scalar_type> with the component z = 0
  template<typename scalar_type>
  inline Vector2<scalar_type> operator*(const Vector2<scalar_type>& v, const Matrix3<scalar_type>& m)
  {
    Vector2<scalar_type> t;
    t.x() = v.x()*m.e(0,0) + v.y()*m.e(0,1) /*+ 0*m.e(2,0)*/;
    t.y() = v.x()*m.e(1,0) + v.y()*m.e(1,1) /*+ 0*m.e(2,1)*/;
    return t;
  }
  //-----------------------------------------------------------------------------
  template<typename scalar_type>
  Matrix3<scalar_type> Matrix3<scalar_type>::rotation( scalar_type degrees )
  {
    Matrix3<scalar_type> rot;
    degrees = degrees * (scalar_type)dDEG_TO_RAD;
    scalar_type s = (scalar_type) sin( degrees );
    scalar_type c = (scalar_type) cos( degrees );
    rot.e(0,0) = (scalar_type)c;
    rot.e(1,1) = (scalar_type)c;
    rot.e(0,1) = (scalar_type)s;
    rot.e(1,0) = -(scalar_type)s;
    return rot;
  }
  //-----------------------------------------------------------------------------
  template<typename scalar_type>
  Matrix3<scalar_type> Matrix3<scalar_type>::inverse(bool *invertible) const
  {
    const scalar_type& a11 = e(0,0);
    const scalar_type& a12 = e(0,1);
    const scalar_type& a13 = e(0,2);
    const scalar_type& a21 = e(1,0);
    const scalar_type& a22 = e(1,1);
    const scalar_type& a23 = e(1,2);
    const scalar_type& a31 = e(2,0);
    const scalar_type& a32 = e(2,1);
    const scalar_type& a33 = e(2,2);

    scalar_type DET = a11*(a33*a22-a32*a23)-a21*(a33*a12-a32*a13)+a31*(a23*a12-a22*a13);

    if (invertible)
    {
      if ( DET == 0 )
        *invertible = false;
      else
        *invertible = true;
    }

    if ( DET == 0 )
      return Matrix3<scalar_type>(0,0,0,0,0,0,0,0,0);

    return Matrix3<scalar_type>(
      a33*a22-a32*a23,    -(a33*a12-a32*a13), a23*a12-a22*a13   , 
      -(a33*a21-a31*a23), a33*a11-a31*a13,    -(a23*a11-a21*a13),
      a32*a21-a31*a22,    -(a32*a11-a31*a12), a22*a11-a21*a12   ) / DET;
  }
  //-----------------------------------------------------------------------------

  //! A 3x3 matrix using \p GLdouble precision.
  typedef Matrix3<GLdouble> dmat3;
  //! A 3x3 matrix using \p GLfloat precision.
  typedef Matrix3<GLfloat>  fmat3;
  //! A 3x3 matrix using \p GLint precision.
  typedef Matrix3<GLint>    imat3;
  //! A 3x3 matrix using \p GLuint precision.
  typedef Matrix3<GLuint>  umat3;

  #if VL_PIPELINE_PRECISION == 2
    //! Defined as: \p 'typedef \p dmat3 \p mat3'. See also \ref VL_PIPELINE_PRECISION.
    typedef dmat3 mat3;
  #else
    //! Defined as: \p 'typedef \p fmat3 \p mat3'. See also \ref VL_PIPELINE_PRECISION.
    typedef fmat3 mat3;
  #endif
}

#endif
