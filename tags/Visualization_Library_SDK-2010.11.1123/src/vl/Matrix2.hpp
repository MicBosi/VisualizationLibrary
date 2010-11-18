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

#ifndef Matrix2_INCLUDE_ONCE
#define Matrix2_INCLUDE_ONCE

#include <vl/checks.hpp>
#include <vl/Vector2.hpp>
#include <cstring> // memcpy

namespace vl
{
  //-----------------------------------------------------------------------------
  // Matrix2
  //-----------------------------------------------------------------------------
  /**
   * The Matrix2 class is a template class that implements a generic 2x2 matrix, see also vl::dmat2, vl::fmat2, vl::umat2, vl::imat2.
   * \sa Vector4, Vector3, Vector2, Matrix4, Matrix3
   */
  template<typename T_scalar_type>
  class Matrix2
  {
  public:
    typedef T_scalar_type scalar_type;
    //-----------------------------------------------------------------------------
    template<typename T>
    explicit Matrix2(const T& m)
    {
      e(0,0) = (T_scalar_type)m.e(0,0); e(0,1) = (T_scalar_type)m.e(0,1);
      e(1,0) = (T_scalar_type)m.e(1,0); e(1,1) = (T_scalar_type)m.e(1,1);
    }
    //-----------------------------------------------------------------------------
    Matrix2()
    {
      static const T_scalar_type I2d[] = 
      { 
        (T_scalar_type)1, (T_scalar_type)0,
        (T_scalar_type)0, (T_scalar_type)1
      };
      memcpy( mVec, I2d, sizeof(T_scalar_type)*4 );
    }
    //-----------------------------------------------------------------------------
    Matrix2(T_scalar_type n)
    {
      static const T_scalar_type I2d[] = 
      { 
        (T_scalar_type)1, (T_scalar_type)0,
        (T_scalar_type)0, (T_scalar_type)1
      };
      memcpy( mVec, I2d, sizeof(T_scalar_type)*4 );
      e(0,0) = n; e(1,1) = n;
    }
    //-----------------------------------------------------------------------------
    explicit Matrix2( T_scalar_type e00, T_scalar_type e01,
                    T_scalar_type e10, T_scalar_type e11  )
    {
      e(0,0) = e00; e(0,1) = e01;
      e(1,0) = e10; e(1,1) = e11;
    }
    //-----------------------------------------------------------------------------
    Matrix2& fill(T_scalar_type val)
    {
      e(0,0) = e(0,1) = 
      e(1,0) = e(1,1) = val;
      return *this;
    }
    //-----------------------------------------------------------------------------
    T_scalar_type diff(const Matrix2& other)
    {
      T_scalar_type err = 0;
      for(int i=0; i<2; ++i)
        for(int j=0; j<2; ++j)
          if ( e(i,j) > other.e(i,j) ) // avoid fabs/abs
            err += e(i,j) - other.e(i,j);
          else
            err += other.e(i,j) - e(i,j);
      return err;
    }
    //-----------------------------------------------------------------------------
    const Vector2<T_scalar_type>& operator[](unsigned int i) const { VL_CHECK(i<2); return mVec[i]; }
    //-----------------------------------------------------------------------------
    Vector2<T_scalar_type>& operator[](unsigned int i) { VL_CHECK(i<2); return mVec[i]; }
    //-----------------------------------------------------------------------------
    bool operator==(const Matrix2& m) const 
    {
      return memcmp(m.mVec, mVec, sizeof(T_scalar_type)*4) == 0;
    }
    //-----------------------------------------------------------------------------
    bool operator!=(const Matrix2& m) const 
    {
      return !(*this == m);
    }
    //-----------------------------------------------------------------------------
    Matrix2& operator=(const Matrix2& m) 
    {
      memcpy(mVec, m.mVec, sizeof(T_scalar_type)*4);
      return *this;
    }
    //-----------------------------------------------------------------------------
    Matrix2 operator+(const Matrix2& m) const
    {
      Matrix2 t;
      for(int i=0; i<2; ++i)
        for(int j=0; j<2; ++j)
          t[i][j] = e(i,j) + m[i][j];

      return t;
    }
    //-----------------------------------------------------------------------------
    Matrix2& operator+=(const Matrix2& m)
    {
      return *this = *this + m;
    }
    //-----------------------------------------------------------------------------
    Matrix2 operator-(const Matrix2& m) const
    {
      Matrix2 t;
      for(int i=0; i<2; ++i)
        for(int j=0; j<2; ++j)
          t[i][j] = e(i,j) - m[i][j];

      return t;
    }
    //-----------------------------------------------------------------------------
    Matrix2& operator-=(const Matrix2& m)
    {
      return *this = *this - m;
    }
    //-----------------------------------------------------------------------------
    Matrix2& operator*=(const Matrix2& m)
    {
      return *this = *this * m;
    }
    //-----------------------------------------------------------------------------
    Matrix2 operator-() const
    {
      Matrix2 t;
      for(int i=0; i<2; ++i)
        for(int j=0; j<2; ++j)
          t[i][j] = -e(i,j);

      return t;
    }
    //-----------------------------------------------------------------------------
    Matrix2 operator+(T_scalar_type d) const
    {
      Matrix2 t;
      for(int i=0; i<2; ++i)
        for(int j=0; j<2; ++j)
          t[i][j] = e(i,j) + d;

      return t;
    }
    //-----------------------------------------------------------------------------
    Matrix2& operator+=(T_scalar_type d)
    {
      return *this = *this + d;
    }
    //-----------------------------------------------------------------------------
    Matrix2 operator-(T_scalar_type d) const
    {
      Matrix2 t;
      for(int i=0; i<2; ++i)
        for(int j=0; j<2; ++j)
          t[i][j] = e(i,j) - d;

      return t;
    }
    //-----------------------------------------------------------------------------
    Matrix2& operator-=(T_scalar_type d)
    {
      return *this = *this - d;
    }
    //-----------------------------------------------------------------------------
    Matrix2 operator*(T_scalar_type d) const
    {
      Matrix2 t;
      for(int i=0; i<2; ++i)
        for(int j=0; j<2; ++j)
          t[i][j] = e(i,j) * d;

      return t;
    }
    //-----------------------------------------------------------------------------
    Matrix2& operator*=(T_scalar_type d)
    {
      Matrix2 t;
      for(int i=0; i<2; ++i)
        for(int j=0; j<2; ++j)
          e(i,j) *= d;

      return *this;
    }
    //-----------------------------------------------------------------------------
    Matrix2 operator/(T_scalar_type d) const
    {
      d = (T_scalar_type)1 / d;
      Matrix2 t;
      for(int i=0; i<2; ++i)
        for(int j=0; j<2; ++j)
          t[i][j] = e(i,j) * d;

      return t;
    }
    //-----------------------------------------------------------------------------
    Matrix2& operator/=(T_scalar_type d)
    {
      d = (T_scalar_type)1 / d;
      Matrix2 t;
      for(int i=0; i<2; ++i)
        for(int j=0; j<2; ++j)
          e(i,j) *= d;

      return *this;
    }
    //-----------------------------------------------------------------------------
    bool isIdentity() const
    {
      Matrix2 i;
      return memcmp( ptr(), i.ptr(), sizeof(T_scalar_type)*4 ) == 0;
    }
    //-----------------------------------------------------------------------------
    T_scalar_type* ptr()
    {
      return &e(0,0);
    }
    //-----------------------------------------------------------------------------
    const T_scalar_type* ptr() const
    {
      return &e(0,0);
    }
    //-----------------------------------------------------------------------------
    Matrix2& transpose()
    {
      Matrix2 m;
      for(int i=0; i<2; ++i)
        for(int j=0; j<2; ++j)
          m.e(i,j) = e(j,i);
      memcpy(mVec, m.mVec, sizeof(T_scalar_type)*4);
      return *this;
    }
    //-----------------------------------------------------------------------------
    Matrix2 getTransposed() const
    {
      Matrix2 m;
      for(int i=0; i<2; ++i)
        for(int j=0; j<2; ++j)
          m.e(i,j) = e(j,i);
      return m;
    }
    //-----------------------------------------------------------------------------
    void getTransposed(Matrix2& dest) const
    {
      for(int i=0; i<2; ++i)
        for(int j=0; j<2; ++j)
          dest.e(i,j) = e(j,i);
    }
    //-----------------------------------------------------------------------------
    bool isNull() const
    {
      for(int i=0; i<2; ++i)
        for(int j=0; j<2; ++j)
          if( mVec[j][i] != 0 )
            return false;
      return true;
    }
    //-----------------------------------------------------------------------------
    void setIdentity()
    {
      *this = Matrix2();
    }
    //-----------------------------------------------------------------------------
    T_scalar_type getInverse(Matrix2& dest) const
    {
      if (&dest == this)
      {
        Matrix2 tmp;
        T_scalar_type det = getInverse(tmp);
        dest = tmp;
        return det;
      }
      else
      {
        const T_scalar_type& a11 = e(0,0); 
        const T_scalar_type& a12 = e(0,1); 
        const T_scalar_type& a21 = e(1,0); 
        const T_scalar_type& a22 = e(1,1); 

        dest.fill(0);

        T_scalar_type det = a11*a22-a12*a21;

        if (det != 0)
          dest = Matrix2(+a22, -a12, -a21, +a11) / det;

        return det;
      }
    }
    //-----------------------------------------------------------------------------
    Matrix2 getInverse(T_scalar_type *determinant=NULL) const
    {
      Matrix2 tmp;
      T_scalar_type det = getInverse(tmp);
      if (determinant)
        *determinant = det;
      return tmp;
    }
    //-----------------------------------------------------------------------------
    Matrix2& invert(T_scalar_type *determinant=NULL)
    {
      T_scalar_type det = getInverse(*this);
      if (determinant)
        *determinant = det;
      return *this;
    }
    //-----------------------------------------------------------------------------
    const T_scalar_type& e(int i, int j) const { return mVec[i][j]; }
    //-----------------------------------------------------------------------------
    T_scalar_type& e(int i, int j) { return mVec[i][j]; }
    //-----------------------------------------------------------------------------

  protected:
    Vector2<T_scalar_type> mVec[2];
  };

  //-----------------------------------------------------------------------------
  // OPERATORS
  //-----------------------------------------------------------------------------
  template<typename T_scalar_type>
  inline Matrix2<T_scalar_type> operator*(const Matrix2<T_scalar_type>& m2, const Matrix2<T_scalar_type>& m1)
  {
    Matrix2<T_scalar_type> t;
    t.e(0,0) = m1.e(0,0)*m2.e(0,0) + m1.e(0,1)*m2.e(1,0);
    t.e(1,0) = m1.e(1,0)*m2.e(0,0) + m1.e(1,1)*m2.e(1,0);

    t.e(0,1) = m1.e(0,0)*m2.e(0,1) + m1.e(0,1)*m2.e(1,1);
    t.e(1,1) = m1.e(1,0)*m2.e(0,1) + m1.e(1,1)*m2.e(1,1);

    return t;
  }
  //-----------------------------------------------------------------------------
  template<typename T_scalar_type>
  inline Matrix2<T_scalar_type> operator+(T_scalar_type d, const Matrix2<T_scalar_type>& m)
  {
    return m + d;
  }
  //-----------------------------------------------------------------------------
  template<typename T_scalar_type>
  inline Matrix2<T_scalar_type> operator*(T_scalar_type d, const Matrix2<T_scalar_type>& m)
  {
    return m * d;
  }
  //-----------------------------------------------------------------------------
  // post multiplication: matrix * column vector
  template<typename T_scalar_type>
  inline Vector2<T_scalar_type> operator*(const Matrix2<T_scalar_type>& m, const Vector2<T_scalar_type>& v)
  {
    Vector2<T_scalar_type> t;
    t.x() = v.x()*m.e(0,0) + v.y()*m.e(1,0);
    t.y() = v.x()*m.e(0,1) + v.y()*m.e(1,1);
    return t;
  }
  //-----------------------------------------------------------------------------
  // pre multiplication: row vector * matrix
  template<typename T_scalar_type>
  inline Vector2<T_scalar_type> operator*(const Vector2<T_scalar_type>& v, const Matrix2<T_scalar_type>& m)
  {
    Vector2<T_scalar_type> t;
    t.x() = v.x()*m.e(0,0) + v.y()*m.e(0,1);
    t.y() = v.x()*m.e(1,0) + v.y()*m.e(1,1);
    return t;
  }
  //-----------------------------------------------------------------------------

  //! A 2x2 matrix using \p GLdouble precision.
  typedef Matrix2<GLdouble> dmat2;
  //! A 2x2 matrix using \p GLfloat precision.
  typedef Matrix2<GLfloat>  fmat2;
  //! A 2x2 matrix using \p GLint precision.
  typedef Matrix2<GLint>    imat2;
  //! A 2x2 matrix using \p GLuint precision.
  typedef Matrix2<GLuint>  umat2;

  #if VL_PIPELINE_PRECISION == 2
    //! Defined as: \p 'typedef \p dmat2 \p mat2'. See also \ref VL_PIPELINE_PRECISION.
    typedef dmat2 mat2;
  #else
    //! Defined as: \p 'typedef \p fmat2 \p mat2'. See also \ref VL_PIPELINE_PRECISION.
    typedef fmat2 mat2;
  #endif
}

#endif
