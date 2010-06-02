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

#ifndef GLSL_MATH_INCLUDE_ONCE
#define GLSL_MATH_INCLUDE_ONCE

/**
  \file GLSLmath.hpp Implements the OpenGL Shading Language convenience functions for scalar and vector operations.

  This functions are particularly useful when you want to port C++ code to GLSL and vice versa, or when you want to quickly 
  prototype in C++ an algorithm that will be ported later to GLSL. 
  
  Note that most of this functions take as arguments not only \p int, \p unsigned \p int, \p float and \p double variables but also their vector counterparts like \p fvec4, \p dvec4, \p ivec4, \p uvec4, \p fvec3, \p fvec2 etc.
  For example you can do the following:
\code
// clamp a float value
float f = someValueFloat()
f = vl::clamp(f, 0.0f, 1.0f);

// clamp a vector value
vl::fvec4 v = someValueVec4();
v = vl::clamp(v, vl::fvec4(1,2,3,4), vl::fvec4(5,6,7,8));

// the same goes for functions like mix(), min(), max(), sin(), cos(), floor() etc!
\endcode

  This module also implements other convenience functions like isnan(), isinf(), isinf_pos(), isinf_neg().

  The functions are divided in the following categories:

  \par Angle and trigonometry functions
  radians(), degrees(), sin(), cos(), tan(), asin(), acos(), atan()

  \par Hyperbolic functions
  sinh(), cosh(), tanh(), asinh(), acosh(), atanh()

  \par Exponential functions
  pow(), exp(), log(), exp2(), log2(), log10(), sqrt(), inversesqrt(), 

  \par Common functions
  abs(), sign(), floor(), trunc(), round(), roundEven(), ceil(), fract(), mod(), modf(), min(), max(), clamp(), mix(), step(), smoothstep(), isnan(), isinf()

  \par Geometric functions
  length(), distance(), dot(), cross(), normalize(), faceforward(), reflect(), refract()

  \par Matrix functions
  matrixCompMult(), outerProduct(), transpose(), 

  \par Vector relational functions
  lessThan(), lessThanEqual(), greaterThan(), greaterThanEqual(), equal(), notEqual(), any(), all(), not()

  \sa
  For more information please refer to the official OpenGL Shading Language manual and specifications.
  See also the official <a target=_blank href=http://www.opengl.org/documentation/specs>OpenGL & OpenGL Utility Specifications</a> page.

 */

#include <cmath>
#include <limits>
#include <vl/Vector4.hpp>
#include <vl/Matrix4.hpp>

#undef min
#undef max

namespace vl
{
  // hyperbolic functions not implemented in Visual C++

  // taken from http://support.microsoft.com/kb/625449/it

  inline float asinh(float x) { return log(x+VL_FLOAT_SQRT(x*x+1.0f)); }
  inline double asinh(double x) { return log(x+sqrt(x*x+1.0)); }

  inline float acosh(float x)
  {
    // must be x>=1, if not return Nan (Not a Number)
    if(!(x>=1.0f)) return VL_FLOAT_SQRT(-1.0f);

    // return only the positive result (as sqrt does).
    return log(x+VL_FLOAT_SQRT(x*x-1.0f));
  }
  inline double acosh(double x)
  {
    // must be x>=1, if not return Nan (Not a Number)
    if(!(x>=1.0)) return sqrt(-1.0);

    // return only the positive result (as sqrt does).
    return log(x+sqrt(x*x-1.0));
  }

  inline float atanh(float x)
  {
    // must be x>-1, x<1, if not return Nan (Not a Number)
    if(!(x>-1.0f && x<1.0f)) return VL_FLOAT_SQRT(-1.0f);

    return log((1.0f+x)/(1.0f-x))/2.0f;
  }
  inline double atanh(double x)
  {
    // must be x>-1, x<1, if not return Nan (Not a Number)
    if(!(x>-1.0 && x<1.0)) return sqrt(-1.0);

    return log((1.0+x)/(1.0-x))/2.0;
  }

  // isinf, isnan functions not implemented in Visual C++

  template<typename T> inline bool isnan(T value) { return !(value == value); }
  template<typename T> inline bool isinf(T value) { return value < std::numeric_limits<T>::min() || value > std::numeric_limits<T>::max(); }
  template<typename T> inline bool isinf_pos(T value) { return value > std::numeric_limits<T>::max(); }
  template<typename T> inline bool isinf_neg(T value) { return value < std::numeric_limits<T>::min(); }

  //-----------------------------------------------------------------------------
  // GLSL functions
  //-----------------------------------------------------------------------------

  inline float modf(float a, float& intpart);
  inline double modf(double a, double& intpart);

  // --------------- angle and trigonometric functions ---------------

  // --------------- radians ---------------

  inline float radians(float degrees) { return degrees * fDEG_TO_RAD; }
  inline fvec2 radians(const fvec2& degrees) {
    return fvec2( degrees.x() * fDEG_TO_RAD,
                  degrees.y() * fDEG_TO_RAD );
  }
  inline fvec3 radians(const fvec3& degrees) {
    return fvec3( degrees.x() * fDEG_TO_RAD,
                  degrees.y() * fDEG_TO_RAD,
                  degrees.z() * fDEG_TO_RAD );
  }
  inline fvec4 radians(const fvec4& degrees) {
    return fvec4( degrees.x() * fDEG_TO_RAD,
                  degrees.y() * fDEG_TO_RAD,
                  degrees.z() * fDEG_TO_RAD,
                  degrees.w() * fDEG_TO_RAD );
  }

  // .........................................

  inline double radians(double degrees) { return degrees * dRAD_TO_DEG; }
  inline dvec2 radians(const dvec2& degrees) {
    return dvec2( degrees.x() * dRAD_TO_DEG,
                  degrees.y() * dRAD_TO_DEG );
  }
  inline dvec3 radians(const dvec3& degrees) {
    return dvec3( degrees.x() * dRAD_TO_DEG,
                  degrees.y() * dRAD_TO_DEG,
                  degrees.z() * dRAD_TO_DEG );
  }
  inline dvec4 radians(const dvec4& degrees) {
    return dvec4( degrees.x() * dRAD_TO_DEG,
                  degrees.y() * dRAD_TO_DEG,
                  degrees.z() * dRAD_TO_DEG,
                  degrees.w() * dRAD_TO_DEG );
  }

  // --------------- degrees ---------------

  inline float degrees(float radians) { return radians * fRAD_TO_DEG; }
  inline fvec2 degrees(const fvec2& radians) {
    return fvec2( radians.x() * fRAD_TO_DEG,
                  radians.y() * fRAD_TO_DEG );
  }
  inline fvec3 degrees(const fvec3& radians) {
    return fvec3( radians.x() * fRAD_TO_DEG,
                  radians.y() * fRAD_TO_DEG,
                  radians.z() * fRAD_TO_DEG );
  }
  inline fvec4 degrees(const fvec4& radians) {
    return fvec4( radians.x() * fRAD_TO_DEG,
                  radians.y() * fRAD_TO_DEG,
                  radians.z() * fRAD_TO_DEG,
                  radians.w() * fRAD_TO_DEG );
  }

  // .........................................

  inline double degrees(double radians) { return radians * dRAD_TO_DEG; }
  inline dvec2 degrees(const dvec2& radians) {
    return dvec2( radians.x() * dRAD_TO_DEG,
                  radians.y() * dRAD_TO_DEG );
  }
  inline dvec3 degrees(const dvec3& radians) {
    return dvec3( radians.x() * dRAD_TO_DEG,
                  radians.y() * dRAD_TO_DEG,
                  radians.z() * dRAD_TO_DEG );
  }
  inline dvec4 degrees(const dvec4& radians) {
    return dvec4( radians.x() * dRAD_TO_DEG,
                  radians.y() * dRAD_TO_DEG,
                  radians.z() * dRAD_TO_DEG,
                  radians.w() * dRAD_TO_DEG );
  }

  // --------------- sin ---------------

  inline float sin(float a) { return ::sin(a); }
  inline fvec2 sin(const fvec2& angle) {
    return fvec2( ::sin(angle.x()),
                  ::sin(angle.y()) );
  }
  inline fvec3 sin(const fvec3& angle) {
    return fvec3( ::sin(angle.x()),
                  ::sin(angle.y()),
                  ::sin(angle.z()) );
  }
  inline fvec4 sin(const fvec4& angle) {
    return fvec4( ::sin(angle.x()),
                  ::sin(angle.y()),
                  ::sin(angle.z()),
                  ::sin(angle.w()) );
  }

  // ...................................

  inline double sin(double a) { return ::sin(a); }
  inline dvec2 sin(const dvec2& angle) {
    return dvec2( ::sin(angle.x()),
                  ::sin(angle.y()) );
  }
  inline dvec3 sin(const dvec3& angle) {
    return dvec3( ::sin(angle.x()),
                  ::sin(angle.y()),
                  ::sin(angle.z()) );
  }
  inline dvec4 sin(const dvec4& angle) {
    return dvec4( ::sin(angle.x()),
                  ::sin(angle.y()),
                  ::sin(angle.z()),
                  ::sin(angle.w()) );
  }

  // --------------- cos ---------------

  inline float cos(float a) { return ::cos(a); }
  inline fvec2 cos(const fvec2& angle) {
    return fvec2( ::cos(angle.x()),
                  ::cos(angle.y()) );
  }
  inline fvec3 cos(const fvec3& angle) {
    return fvec3( ::cos(angle.x()),
                  ::cos(angle.y()),
                  ::cos(angle.z()) );
  }
  inline fvec4 cos(const fvec4& angle) {
    return fvec4( ::cos(angle.x()),
                  ::cos(angle.y()),
                  ::cos(angle.z()),
                  ::cos(angle.w()) );
  }

  // ...................................

  inline double cos(double a) { return ::cos(a); }
  inline dvec2 cos(const dvec2& angle) {
    return dvec2( ::cos(angle.x()),
                  ::cos(angle.y()) );
  }
  inline dvec3 cos(const dvec3& angle) {
    return dvec3( ::cos(angle.x()),
                  ::cos(angle.y()),
                  ::cos(angle.z()) );
  }
  inline dvec4 cos(const dvec4& angle) {
    return dvec4( ::cos(angle.x()),
                  ::cos(angle.y()),
                  ::cos(angle.z()),
                  ::cos(angle.w()) );
  }

  // --------------- tan ---------------

  inline float tan(float a) { return ::tan(a); }
  inline fvec2 tan(const fvec2& angle) {
    return fvec2( ::tan(angle.x()),
                  ::tan(angle.y()) );
  }
  inline fvec3 tan(const fvec3& angle) {
    return fvec3( ::tan(angle.x()),
                  ::tan(angle.y()),
                  ::tan(angle.z()) );
  }
  inline fvec4 tan(const fvec4& angle) {
    return fvec4( ::tan(angle.x()),
                  ::tan(angle.y()),
                  ::tan(angle.z()),
                  ::tan(angle.w()) );
  }

  // ...................................

  inline double tan(double a) { return ::tan(a); }
  inline dvec2 tan(const dvec2& angle) {
    return dvec2( ::tan(angle.x()),
                  ::tan(angle.y()) );
  }
  inline dvec3 tan(const dvec3& angle) {
    return dvec3( ::tan(angle.x()),
                  ::tan(angle.y()),
                  ::tan(angle.z()) );
  }
  inline dvec4 tan(const dvec4& angle) {
    return dvec4( ::tan(angle.x()),
                  ::tan(angle.y()),
                  ::tan(angle.z()),
                  ::tan(angle.w()) );
  }

  // --------------- asin ---------------

  inline float asin(float a) { return ::asin(a); }
  inline fvec2 asin(const fvec2& angle) {
    return fvec2( ::asin(angle.x()),
                  ::asin(angle.y()) );
  }
  inline fvec3 asin(const fvec3& angle) {
    return fvec3( ::asin(angle.x()),
                  ::asin(angle.y()),
                  ::asin(angle.z()) );
  }
  inline fvec4 asin(const fvec4& angle) {
    return fvec4( ::asin(angle.x()),
                  ::asin(angle.y()),
                  ::asin(angle.z()),
                  ::asin(angle.w()) );
  }

  // ...................................

  inline double asin(double a) { return ::asin(a); }
  inline dvec2 asin(const dvec2& angle) {
    return dvec2( ::asin(angle.x()),
                  ::asin(angle.y()) );
  }
  inline dvec3 asin(const dvec3& angle) {
    return dvec3( ::asin(angle.x()),
                  ::asin(angle.y()),
                  ::asin(angle.z()) );
  }
  inline dvec4 asin(const dvec4& angle) {
    return dvec4( ::asin(angle.x()),
                  ::asin(angle.y()),
                  ::asin(angle.z()),
                  ::asin(angle.w()) );
  }

  // --------------- acos ---------------

  inline float acos(float a) { return ::acos(a); }
  inline fvec2 acos(const fvec2& angle) {
    return fvec2( ::acos(angle.x()),
                  ::acos(angle.y()) );
  }
  inline fvec3 acos(const fvec3& angle) {
    return fvec3( ::acos(angle.x()),
                  ::acos(angle.y()),
                  ::acos(angle.z()) );
  }
  inline fvec4 acos(const fvec4& angle) {
    return fvec4( ::acos(angle.x()),
                  ::acos(angle.y()),
                  ::acos(angle.z()),
                  ::acos(angle.w()) );
  }

  // ...................................

  inline double acos(double a) { return ::acos(a); }
  inline dvec2 acos(const dvec2& angle) {
    return dvec2( ::acos(angle.x()),
                  ::acos(angle.y()) );
  }
  inline dvec3 acos(const dvec3& angle) {
    return dvec3( ::acos(angle.x()),
                  ::acos(angle.y()),
                  ::acos(angle.z()) );
  }
  inline dvec4 acos(const dvec4& angle) {
    return dvec4( ::acos(angle.x()),
                  ::acos(angle.y()),
                  ::acos(angle.z()),
                  ::acos(angle.w()) );
  }

  // --------------- atan ---------------

  inline float atan(float a) { return ::atan(a); }
  inline fvec2 atan(const fvec2& a, const fvec2& b) {
    return fvec2( ::atan2(a.x(), b.x()),
                  ::atan2(a.y(), b.y()) );
  }
  inline fvec3 atan(const fvec3& a, const fvec3& b) {
    return fvec3( ::atan2(a.x(), b.x()),
                  ::atan2(a.y(), b.y()),
                  ::atan2(a.z(), b.z()) );
  }
  inline fvec4 atan(const fvec4& a, const fvec4& b) {
    return fvec4( ::atan2(a.x(), b.x()),
                  ::atan2(a.y(), b.y()),
                  ::atan2(a.z(), b.z()),
                  ::atan2(a.w(), b.w()) );
  }

  // ...................................

  inline double atan(double a) { return ::atan(a); }
  inline dvec2 atan(const dvec2& a, const dvec2& b) {
    return dvec2( ::atan2(a.x(), b.x()),
                  ::atan2(a.y(), b.y()) );
  }
  inline dvec3 atan(const dvec3& a, const dvec3& b) {
    return dvec3( ::atan2(a.x(), b.x()),
                  ::atan2(a.y(), b.y()),
                  ::atan2(a.z(), b.z()) );
  }
  inline dvec4 atan(const dvec4& a, const dvec4& b) {
    return dvec4( ::atan2(a.x(), b.x()),
                  ::atan2(a.y(), b.y()),
                  ::atan2(a.z(), b.z()),
                  ::atan2(a.w(), b.w()) );
  }

  // --------------- hyperbolic functions ---------------

  // --------------- sinh ---------------

  inline float sinh(float a) { return (exp(a) - exp(-a)) / 2.0f; }
  inline fvec2 sinh(const fvec2& a) { return fvec2( sinh(a.x()), sinh(a.y()) ); }
  inline fvec3 sinh(const fvec3& a) { return fvec3( sinh(a.x()), sinh(a.y()), sinh(a.z()) ); }
  inline fvec4 sinh(const fvec4& a) { return fvec4( sinh(a.x()), sinh(a.y()), sinh(a.z()), sinh(a.w()) ); }

  // .....................................

  inline double sinh(double a) { return (exp(a) - exp(-a)) / 2.0; }
  inline dvec2 sinh(const dvec2& a) { return dvec2( sinh(a.x()), sinh(a.y()) ); }
  inline dvec3 sinh(const dvec3& a) { return dvec3( sinh(a.x()), sinh(a.y()), sinh(a.z()) ); }
  inline dvec4 sinh(const dvec4& a) { return dvec4( sinh(a.x()), sinh(a.y()), sinh(a.z()), sinh(a.w()) ); }

  // --------------- cosh ---------------

  inline float cosh(float a) { return (exp(a) + exp(-a)) / 2.0f; }
  inline fvec2 cosh(const fvec2& a) { return fvec2( cosh(a.x()), cosh(a.y()) ); }
  inline fvec3 cosh(const fvec3& a) { return fvec3( cosh(a.x()), cosh(a.y()), cosh(a.z()) ); }
  inline fvec4 cosh(const fvec4& a) { return fvec4( cosh(a.x()), cosh(a.y()), cosh(a.z()), cosh(a.w()) ); }

  // .....................................

  inline double cosh(double a) { return (exp(a) + exp(-a)) / 2.0; }
  inline dvec2 cosh(const dvec2& a) { return dvec2( cosh(a.x()), cosh(a.y()) ); }
  inline dvec3 cosh(const dvec3& a) { return dvec3( cosh(a.x()), cosh(a.y()), cosh(a.z()) ); }
  inline dvec4 cosh(const dvec4& a) { return dvec4( cosh(a.x()), cosh(a.y()), cosh(a.z()), cosh(a.w()) ); }

  // --------------- tanh ---------------

  inline float tanh(float a) { return sinh(a) / cosh(a); }
  inline fvec2 tanh(const fvec2& a) { return fvec2( tanh(a.x()), tanh(a.y()) ); }
  inline fvec3 tanh(const fvec3& a) { return fvec3( tanh(a.x()), tanh(a.y()), tanh(a.z()) ); }
  inline fvec4 tanh(const fvec4& a) { return fvec4( tanh(a.x()), tanh(a.y()), tanh(a.z()), tanh(a.w()) ); }

  // .....................................

  inline double tanh(double a) { return sinh(a) / cosh(a); }
  inline dvec2 tanh(const dvec2& a) { return dvec2( tanh(a.x()), tanh(a.y()) ); }
  inline dvec3 tanh(const dvec3& a) { return dvec3( tanh(a.x()), tanh(a.y()), tanh(a.z()) ); }
  inline dvec4 tanh(const dvec4& a) { return dvec4( tanh(a.x()), tanh(a.y()), tanh(a.z()), tanh(a.w()) ); }

  // --------------- asinh ---------------

  inline fvec2 asinh(const fvec2& a) { return fvec2( asinh(a.x()), asinh(a.y()) ); }
  inline fvec3 asinh(const fvec3& a) { return fvec3( asinh(a.x()), asinh(a.y()), asinh(a.z()) ); }
  inline fvec4 asinh(const fvec4& a) { return fvec4( asinh(a.x()), asinh(a.y()), asinh(a.z()), asinh(a.w()) ); }

  // .....................................

  inline dvec2 asinh(const dvec2& a) { return dvec2( asinh(a.x()), asinh(a.y()) ); }
  inline dvec3 asinh(const dvec3& a) { return dvec3( asinh(a.x()), asinh(a.y()), asinh(a.z()) ); }
  inline dvec4 asinh(const dvec4& a) { return dvec4( asinh(a.x()), asinh(a.y()), asinh(a.z()), asinh(a.w()) ); }

  // --------------- acosh ---------------

  inline fvec2 acosh(const fvec2& a) { return fvec2( acosh(a.x()), acosh(a.y()) ); }
  inline fvec3 acosh(const fvec3& a) { return fvec3( acosh(a.x()), acosh(a.y()), acosh(a.z()) ); }
  inline fvec4 acosh(const fvec4& a) { return fvec4( acosh(a.x()), acosh(a.y()), acosh(a.z()), acosh(a.w()) ); }

  // .....................................

  inline dvec2 acosh(const dvec2& a) { return dvec2( acosh(a.x()), acosh(a.y()) ); }
  inline dvec3 acosh(const dvec3& a) { return dvec3( acosh(a.x()), acosh(a.y()), acosh(a.z()) ); }
  inline dvec4 acosh(const dvec4& a) { return dvec4( acosh(a.x()), acosh(a.y()), acosh(a.z()), acosh(a.w()) ); }

  // --------------- atanh ---------------

  inline fvec2 atanh(const fvec2& a) { return fvec2( atanh(a.x()), atanh(a.y()) ); }
  inline fvec3 atanh(const fvec3& a) { return fvec3( atanh(a.x()), atanh(a.y()), atanh(a.z()) ); }
  inline fvec4 atanh(const fvec4& a) { return fvec4( atanh(a.x()), atanh(a.y()), atanh(a.z()), atanh(a.w()) ); }

  // .....................................

  inline dvec2 atanh(const dvec2& a) { return dvec2( atanh(a.x()), atanh(a.y()) ); }
  inline dvec3 atanh(const dvec3& a) { return dvec3( atanh(a.x()), atanh(a.y()), atanh(a.z()) ); }
  inline dvec4 atanh(const dvec4& a) { return dvec4( atanh(a.x()), atanh(a.y()), atanh(a.z()), atanh(a.w()) ); }

  // --------------- exponential functions ---------------

  // --------------- pow ---------------

  inline float pow(float a, float b) { return ::pow(a,b); }
  inline fvec2 pow(const fvec2& a, const fvec2& b) {
    return fvec2( ::pow(a.x(),b.x()),
                  ::pow(a.y(),b.y()) );
  }
  inline fvec3 pow(const fvec3& a, const fvec3& b) {
    return fvec3( ::pow(a.x(),b.x()),
                  ::pow(a.y(),b.y()),
                  ::pow(a.z(),b.z()) );
  }
  inline fvec4 pow(const fvec4& a, const fvec4& b) {
    return fvec4( ::pow(a.x(),b.x()),
                  ::pow(a.y(),b.y()),
                  ::pow(a.z(),b.z()),
                  ::pow(a.w(),b.w()) );
  }

  // ...................................

  inline double pow(double a, double b) { return ::pow(a,b); }
  inline dvec2 pow(const dvec2& a, const dvec2& b) {
    return dvec2( ::pow(a.x(),b.x()),
                  ::pow(a.y(),b.y()) );
  }
  inline dvec3 pow(const dvec3& a, const dvec3& b) {
    return dvec3( ::pow(a.x(),b.x()),
                  ::pow(a.y(),b.y()),
                  ::pow(a.z(),b.z()) );
  }
  inline dvec4 pow(const dvec4& a, const dvec4& b) {
    return dvec4( ::pow(a.x(),b.x()),
                  ::pow(a.y(),b.y()),
                  ::pow(a.z(),b.z()),
                  ::pow(a.w(),b.w()) );
  }

  // --------------- exp ---------------

  inline float exp(float a) { return ::exp(a); }
  inline fvec2 exp(const fvec2& a) {
    return fvec2( ::exp(a.x()),
                  ::exp(a.y()) );
  }
  inline fvec3 exp(const fvec3& a) {
    return fvec3( ::exp(a.x()),
                  ::exp(a.y()),
                  ::exp(a.z()) );
  }
  inline fvec4 exp(const fvec4& a) {
    return fvec4( ::exp(a.x()),
                  ::exp(a.y()),
                  ::exp(a.z()),
                  ::exp(a.w()) );
  }

  // ...................................

  inline double exp(double a) { return ::exp(a); }
  inline dvec2 exp(const dvec2& a) {
    return dvec2( ::exp(a.x()),
                  ::exp(a.y()) );
  }
  inline dvec3 exp(const dvec3& a) {
    return dvec3( ::exp(a.x()),
                  ::exp(a.y()),
                  ::exp(a.z()) );
  }
  inline dvec4 exp(const dvec4& a) {
    return dvec4( ::exp(a.x()),
                  ::exp(a.y()),
                  ::exp(a.z()),
                  ::exp(a.w()) );
  }

  // --------------- log ---------------

  inline float log(float a) { return ::log(a); }
  inline fvec2 log(const fvec2& a) {
    return fvec2( ::log(a.x()),
                  ::log(a.y()) );
  }
  inline fvec3 log(const fvec3& a) {
    return fvec3( ::log(a.x()),
                  ::log(a.y()),
                  ::log(a.z()) );
  }
  inline fvec4 log(const fvec4& a) {
    return fvec4( ::log(a.x()),
                  ::log(a.y()),
                  ::log(a.z()),
                  ::log(a.w()) );
  }

  // ...................................

  inline double log(double a) { return ::log(a); }
  inline dvec2 log(const dvec2& a) {
    return dvec2( ::log(a.x()),
                  ::log(a.y()) );
  }
  inline dvec3 log(const dvec3& a) {
    return dvec3( ::log(a.x()),
                  ::log(a.y()),
                  ::log(a.z()) );
  }
  inline dvec4 log(const dvec4& a) {
    return dvec4( ::log(a.x()),
                  ::log(a.y()),
                  ::log(a.z()),
                  ::log(a.w()) );
  }

  // --------------- exp2 ---------------

  inline float exp2(float a) { return ::pow(2.0f, a); }
  inline fvec2 exp2(const fvec2& a) {
    return fvec2( ::pow(2.0f, a.x()),
                  ::pow(2.0f, a.y()) );
  }
  inline fvec3 exp2(const fvec3& a) {
    return fvec3( ::pow(2.0f, a.x()),
                  ::pow(2.0f, a.y()),
                  ::pow(2.0f, a.z()) );
  }
  inline fvec4 exp2(const fvec4& a) {
    return fvec4( ::pow(2.0f, a.x()),
                  ::pow(2.0f, a.y()),
                  ::pow(2.0f, a.z()),
                  ::pow(2.0f, a.w()) );
  }

  // ...................................

  inline double exp2(double a) { return ::pow(2.0, a); }
  inline dvec2 exp2(const dvec2& a) {
    return dvec2( ::pow(2.0, a.x()),
                  ::pow(2.0, a.y()) );
  }
  inline dvec3 exp2(const dvec3& a) {
    return dvec3( ::pow(2.0, a.x()),
                  ::pow(2.0, a.y()),
                  ::pow(2.0, a.z()) );
  }
  inline dvec4 exp2(const dvec4& a) {
    return dvec4( ::pow(2.0, a.x()),
                  ::pow(2.0, a.y()),
                  ::pow(2.0, a.z()),
                  ::pow(2.0, a.w()) );
  }

  // --------------- log2 ---------------

  inline float log2(float a) { return ::log10(a) / ::log10(2.0f); }
  inline fvec2 log2(const fvec2& a) {
    return fvec2( log2(a.x()),
                  log2(a.y()) );
  }
  inline fvec3 log2(const fvec3& a) {
    return fvec3( log2(a.x()),
                  log2(a.y()),
                  log2(a.z()) );
  }
  inline fvec4 log2(const fvec4& a) {
    return fvec4( log2(a.x()),
                  log2(a.y()),
                  log2(a.z()),
                  log2(a.w()) );
  }

  // ...................................

  inline double log2(double a) { return log10(a) / log10(2.0); }
  inline dvec2 log2(const dvec2& a) {
    return dvec2( log2(a.x()),
                  log2(a.y()) );
  }
  inline dvec3 log2(const dvec3& a) {
    return dvec3( log2(a.x()),
                  log2(a.y()),
                  log2(a.z()) );
  }
  inline dvec4 log2(const dvec4& a) {
    return dvec4( log2(a.x()),
                  log2(a.y()),
                  log2(a.z()),
                  log2(a.w()) );
  }

  // --------------- log10 ---------------

  // this is not present in the GLSL standard

  inline float log10(float a) { return ::log10(a); }
  inline fvec2 log10(const fvec2& a) {
    return fvec2( ::log10(a.x()),
                  ::log10(a.y()) );
  }
  inline fvec3 log10(const fvec3& a) {
    return fvec3( ::log10(a.x()),
                  ::log10(a.y()),
                  ::log10(a.z()) );
  }
  inline fvec4 log10(const fvec4& a) {
    return fvec4( ::log10(a.x()),
                  ::log10(a.y()),
                  ::log10(a.z()),
                  ::log10(a.w()) );
  }

  // ...................................

  inline double log10(double a) { return ::log10(a); }
  inline dvec2 log10(const dvec2& a) {
    return dvec2( ::log10(a.x()),
                  ::log10(a.y()) );
  }
  inline dvec3 log10(const dvec3& a) {
    return dvec3( ::log10(a.x()),
                  ::log10(a.y()),
                  ::log10(a.z()) );
  }
  inline dvec4 log10(const dvec4& a) {
    return dvec4( ::log10(a.x()),
                  ::log10(a.y()),
                  ::log10(a.z()),
                  ::log10(a.w()) );
  }

  // --------------- sqrt ---------------

  inline float sqrt(float a) { return VL_FLOAT_SQRT(a); }
  inline fvec2 sqrt(const fvec2& a) {
    return fvec2( VL_FLOAT_SQRT(a.x()),
                  VL_FLOAT_SQRT(a.y()) );
  }
  inline fvec3 sqrt(const fvec3& a) {
    return fvec3( VL_FLOAT_SQRT(a.x()),
                  VL_FLOAT_SQRT(a.y()),
                  VL_FLOAT_SQRT(a.z()) );
  }
  inline fvec4 sqrt(const fvec4& a) {
    return fvec4( VL_FLOAT_SQRT(a.x()),
                  VL_FLOAT_SQRT(a.y()),
                  VL_FLOAT_SQRT(a.z()),
                  VL_FLOAT_SQRT(a.w()) );
  }

  // ...................................

  inline double sqrt(double a) { return ::sqrt(a); }
  inline dvec2 sqrt(const dvec2& a) {
    return dvec2( ::sqrt(a.x()),
                  ::sqrt(a.y()) );
  }
  inline dvec3 sqrt(const dvec3& a) {
    return dvec3( ::sqrt(a.x()),
                  ::sqrt(a.y()),
                  ::sqrt(a.z()) );
  }
  inline dvec4 sqrt(const dvec4& a) {
    return dvec4( ::sqrt(a.x()),
                  ::sqrt(a.y()),
                  ::sqrt(a.z()),
                  ::sqrt(a.w()) );
  }

  // --------------- inversesqrt ---------------

  inline float inversesqrt(float a) { return VL_FLOAT_INVSQRT(a); }
  inline fvec2 inversesqrt(const fvec2& a) {
    return fvec2( VL_FLOAT_INVSQRT(a.x()),
                  VL_FLOAT_INVSQRT(a.y()) );
  }
  inline fvec3 inversesqrt(const fvec3& a) {
    return fvec3( VL_FLOAT_INVSQRT(a.x()),
                  VL_FLOAT_INVSQRT(a.y()),
                  VL_FLOAT_INVSQRT(a.z()) );
  }
  inline fvec4 inversesqrt(const fvec4& a) {
    return fvec4( VL_FLOAT_INVSQRT(a.x()),
                  VL_FLOAT_INVSQRT(a.y()),
                  VL_FLOAT_INVSQRT(a.z()),
                  VL_FLOAT_INVSQRT(a.w()) );
  }

  // ...................................

  inline double inversesqrt(double a) { return ::sqrt(a); }
  inline dvec2 inversesqrt(const dvec2& a) {
    return dvec2( 1.0 / ::sqrt(a.x()),
                  1.0 / ::sqrt(a.y()) );
  }
  inline dvec3 inversesqrt(const dvec3& a) {
    return dvec3( 1.0 / ::sqrt(a.x()),
                  1.0 / ::sqrt(a.y()),
                  1.0 / ::sqrt(a.z()) );
  }
  inline dvec4 inversesqrt(const dvec4& a) {
    return dvec4( 1.0 / ::sqrt(a.x()),
                  1.0 / ::sqrt(a.y()),
                  1.0 / ::sqrt(a.z()),
                  1.0 / ::sqrt(a.w()) );
  }

  // --------------- common functions ---------------

  // --------------- abs ---------------

  inline float abs(float a) { return a >= 0 ? a : -a; }
  inline fvec2 abs(const fvec2& a)
  {
    return fvec2( a.x() >= 0 ? a.x() : -a.x(), a.y() >= 0 ? a.y() : -a.y() );
  }
  inline fvec3 abs(const fvec3& a)
  {
    return fvec3( a.x() >= 0 ? a.x() : -a.x(), a.y() >= 0 ? a.y() : -a.y(),  a.z() >= 0 ? a.z() : -a.z() );
  }
  inline fvec4 abs(const fvec4& a)
  {
    return fvec4( a.x() >= 0 ? a.x() : -a.x(), a.y() >= 0 ? a.y() : -a.y(), a.z() >= 0 ? a.z() : -a.z(), a.w() >= 0 ? a.w() : -a.w() );
  }

  // ....................................

  inline double abs(double a) { return a >= 0 ? a : -a; }
  inline dvec2 abs(const dvec2& a)
  {
    return dvec2( a.x() >= 0 ? a.x() : -a.x(), a.y() >= 0 ? a.y() : -a.y() );
  }
  inline dvec3 abs(const dvec3& a)
  {
    return dvec3( a.x() >= 0 ? a.x() : -a.x(), a.y() >= 0 ? a.y() : -a.y(),  a.z() >= 0 ? a.z() : -a.z() );
  }
  inline dvec4 abs(const dvec4& a)
  {
    return dvec4( a.x() >= 0 ? a.x() : -a.x(), a.y() >= 0 ? a.y() : -a.y(), a.z() >= 0 ? a.z() : -a.z(), a.w() >= 0 ? a.w() : -a.w() );
  }

  // ....................................

  inline int abs(int a) { return a >= 0 ? a : -a; }
  inline ivec2 abs(const ivec2& a)
  {
    return ivec2( a.x() >= 0 ? a.x() : -a.x(), a.y() >= 0 ? a.y() : -a.y() );
  }
  inline ivec3 abs(const ivec3& a)
  {
    return ivec3( a.x() >= 0 ? a.x() : -a.x(), a.y() >= 0 ? a.y() : -a.y(),  a.z() >= 0 ? a.z() : -a.z() );
  }
  inline ivec4 abs(const ivec4& a)
  {
    return ivec4( a.x() >= 0 ? a.x() : -a.x(), a.y() >= 0 ? a.y() : -a.y(), a.z() >= 0 ? a.z() : -a.z(), a.w() >= 0 ? a.w() : -a.w() );
  }

  // --------------- sign ---------------

  inline float sign(float a) { return a > 0 ? 1.0f : a == 0 ? 0 : -1.0f; }
  inline fvec2 sign(const fvec2& a)
  {
    return fvec2( a.x() > 0 ? 1.0f : a.x() == 0 ? 0 : -1.0f,
                 a.y() > 0 ? 1.0f : a.y() == 0 ? 0 : -1.0f );
  }
  inline fvec3 sign(const fvec3& a)
  {
    return fvec3( a.x() > 0 ? 1.0f : a.x() == 0 ? 0 : -1.0f,
                 a.y() > 0 ? 1.0f : a.y() == 0 ? 0 : -1.0f,
                 a.z() > 0 ? 1.0f : a.z() == 0 ? 0 : -1.0f );
  }
  inline fvec4 sign(const fvec4& a)
  {
    return fvec4( a.x() > 0 ? 1.0f : a.x() == 0 ? 0 : -1.0f,
                 a.y() > 0 ? 1.0f : a.y() == 0 ? 0 : -1.0f,
                 a.z() > 0 ? 1.0f : a.z() == 0 ? 0 : -1.0f,
                 a.w() > 0 ? 1.0f : a.w() == 0 ? 0 : -1.0f );
  }

  // .....................................

  inline double sign(double a) { return a > 0 ? 1.0f : a == 0 ? 0 : -1.0f; }
  inline dvec2 sign(const dvec2& a)
  {
    return dvec2( a.x() > 0 ? 1.0f : a.x() == 0 ? 0 : -1.0f,
                 a.y() > 0 ? 1.0f : a.y() == 0 ? 0 : -1.0f );
  }
  inline dvec3 sign(const dvec3& a)
  {
    return dvec3( a.x() > 0 ? 1.0f : a.x() == 0 ? 0 : -1.0f,
                 a.y() > 0 ? 1.0f : a.y() == 0 ? 0 : -1.0f,
                 a.z() > 0 ? 1.0f : a.z() == 0 ? 0 : -1.0f );
  }
  inline dvec4 sign(const dvec4& a)
  {
    return dvec4( a.x() > 0 ? 1.0f : a.x() == 0 ? 0 : -1.0f,
                 a.y() > 0 ? 1.0f : a.y() == 0 ? 0 : -1.0f,
                 a.z() > 0 ? 1.0f : a.z() == 0 ? 0 : -1.0f,
                 a.w() > 0 ? 1.0f : a.w() == 0 ? 0 : -1.0f );
  }

  // .....................................

  inline int sign(int a) { return a > 0 ? 1 : a == 0 ? 0 : -1; }
  inline ivec2 sign(const ivec2& a)
  {
    return ivec2( a.x() > 0 ? 1 : a.x() == 0 ? 0 : -1,
                 a.y() > 0 ? 1 : a.y() == 0 ? 0 : -1 );
  }
  inline ivec3 sign(const ivec3& a)
  {
    return ivec3( a.x() > 0 ? 1 : a.x() == 0 ? 0 : -1,
                 a.y() > 0 ? 1 : a.y() == 0 ? 0 : -1,
                 a.z() > 0 ? 1 : a.z() == 0 ? 0 : -1 );
  }
  inline ivec4 sign(const ivec4& a)
  {
    return ivec4( a.x() > 0 ? 1 : a.x() == 0 ? 0 : -1,
                 a.y() > 0 ? 1 : a.y() == 0 ? 0 : -1,
                 a.z() > 0 ? 1 : a.z() == 0 ? 0 : -1,
                 a.w() > 0 ? 1 : a.w() == 0 ? 0 : -1 );
  }

  // --------------- floor ---------------

  inline float floor(float a) { return ::floor(a); }
  inline fvec2 floor(const fvec2& a) {
    return fvec2( ::floor(a.x()),
                  ::floor(a.y()) );
  }
  inline fvec3 floor(const fvec3& a) {
    return fvec3( ::floor(a.x()),
                  ::floor(a.y()),
                  ::floor(a.z()) );
  }
  inline fvec4 floor(const fvec4& a) {
    return fvec4( ::floor(a.x()),
                  ::floor(a.y()),
                  ::floor(a.z()),
                  ::floor(a.w()) );
  }
  // ......................................

  inline double floor(double a) { return ::floor(a); }
  inline dvec2 floor(const dvec2& a) {
    return dvec2( ::floor(a.x()),
                  ::floor(a.y()) );
  }
  inline dvec3 floor(const dvec3& a) {
    return dvec3( ::floor(a.x()),
                  ::floor(a.y()),
                  ::floor(a.z()) );
  }
  inline dvec4 floor(const dvec4& a) {
    return dvec4( ::floor(a.x()),
                  ::floor(a.y()),
                  ::floor(a.z()),
                  ::floor(a.w()) );
  }

  // --------------- trunc ---------------

  inline float fract(float);
  inline float trunc(float a) { return a - fract(a); }
  inline fvec2 trunc(const fvec2& a) {
    return fvec2( a.x() - fract(a.x()),
                  a.y() - fract(a.y()) );
  }
  inline fvec3 trunc(const fvec3& a) {
    return fvec3( a.x() - fract(a.x()),
                  a.y() - fract(a.y()),
                  a.z() - fract(a.z()) );
  }
  inline fvec4 trunc(const fvec4& a) {
    return fvec4( a.x() - fract(a.x()),
                  a.y() - fract(a.y()),
                  a.z() - fract(a.z()),
                  a.w() - fract(a.w()) );
  }

  // .....................................

  inline double fract(double);
  inline double trunc(double a) { return a - fract(a); }
  inline dvec2 trunc(const dvec2& a) {
    return dvec2( a.x() - fract(a.x()),
                  a.y() - fract(a.y()) );
  }
  inline dvec3 trunc(const dvec3& a) {
    return dvec3( a.x() - fract(a.x()),
                  a.y() - fract(a.y()),
                  a.z() - fract(a.z()) );
  }
  inline dvec4 trunc(const dvec4& a) {
    return dvec4( a.x() - fract(a.x()),
                  a.y() - fract(a.y()),
                  a.z() - fract(a.z()),
                  a.w() - fract(a.w()) );
  }

  // --------------- round ---------------

  inline float round(float x) { return ((x - floor(x)) >= 0.5) ? ceil(x) : floor(x); }
  inline fvec2 round(const fvec2& a) {
    return fvec2( round(a.x()),
                  round(a.y()) );
  }
  inline fvec3 round(const fvec3& a) {
    return fvec3( round(a.x()),
                  round(a.y()),
                  round(a.z()) );
  }
  inline fvec4 round(const fvec4& a) {
    return fvec4( round(a.x()),
                  round(a.y()),
                  round(a.z()),
                  round(a.w()) );
  }

  // .........................................

  inline double round(double x) { return ((x - floor(x)) >= 0.5) ? ceil(x) : floor(x); }
  inline dvec2 round(const dvec2& a) {
    return dvec2( round(a.x()),
                  round(a.y()) );
  }
  inline dvec3 round(const dvec3& a) {
    return dvec3( round(a.x()),
                  round(a.y()),
                  round(a.z()) );
  }
  inline dvec4 round(const dvec4& a) {
    return dvec4( round(a.x()),
                  round(a.y()),
                  round(a.z()),
                  round(a.w()) );
  }

  // --------------- roundEven ---------------

  inline float roundEven(float a, float epsilon = 0.00001f)
  {
    if( a < 0.0f )
      return -roundEven(-a);
    else
    {
      float intpart;
      modf( a, intpart );

      // 0.5 case
      if ((a -(intpart + 0.5f)) < epsilon)
      {
        // is even
        if (::fmod(intpart, 2.0f) < epsilon)
          return intpart;
        else
        // is odd
          return ceil(intpart + 0.5f);
      }
      else
      // all the other cases
        return round(a);
    }
  }
  inline fvec2 roundEven(const fvec2& a, float epsilon = 0.00001f) {
    return fvec2( roundEven(a.x(), epsilon),
                  roundEven(a.y(), epsilon) );
  }
  inline fvec3 roundEven(const fvec3& a, float epsilon = 0.00001f) {
    return fvec3( roundEven(a.x(), epsilon),
                  roundEven(a.y(), epsilon),
                  roundEven(a.z(), epsilon) );
  }
  inline fvec4 roundEven(const fvec4& a, float epsilon = 0.00001f) {
    return fvec4( roundEven(a.x(), epsilon),
                  roundEven(a.y(), epsilon),
                  roundEven(a.z(), epsilon),
                  roundEven(a.w(), epsilon) );
  }

  // ....................................

  inline double roundEven(double a, double epsilon = 0.00001)
  {
    if( a < 0.0 )
      return -roundEven(-a);
    else
    {
      double intpart;
      modf( a, intpart );

      // 0.5 case
      if ((a -(intpart + 0.5)) < epsilon)
      {
        // is even
        if (::fmod(intpart, 2.0) < epsilon)
          return intpart;
        else
        // is odd
          return ceil(intpart + 0.5);
      }
      else
      // all the other cases
        return round(a);
    }
  }
  inline dvec2 roundEven(const dvec2& a, double epsilon = 0.00001) {
    return dvec2( roundEven(a.x(), epsilon),
                  roundEven(a.y(), epsilon) );
  }
  inline dvec3 roundEven(const dvec3& a, double epsilon = 0.00001) {
    return dvec3( roundEven(a.x(), epsilon),
                  roundEven(a.y(), epsilon),
                  roundEven(a.z(), epsilon) );
  }
  inline dvec4 roundEven(const dvec4& a, double epsilon = 0.00001) {
    return dvec4( roundEven(a.x(), epsilon),
                  roundEven(a.y(), epsilon),
                  roundEven(a.z(), epsilon),
                  roundEven(a.w(), epsilon) );
  }

  // --------------- ceil ---------------

  inline float ceil(float a) { return ::ceil(a); }
  inline fvec2 ceil(const fvec2& a) {
    return fvec2( ::ceil(a.x()),
                  ::ceil(a.y()) );
  }
  inline fvec3 ceil(const fvec3& a) {
    return fvec3( ::ceil(a.x()),
                  ::ceil(a.y()),
                  ::ceil(a.z()) );
  }
  inline fvec4 ceil(const fvec4& a) {
    return fvec4( ::ceil(a.x()),
                  ::ceil(a.y()),
                  ::ceil(a.z()),
                  ::ceil(a.w()) );
  }

  // ......................................

  inline double ceil(double a) { return ::ceil(a); }
  inline dvec2 ceil(const dvec2& a) {
    return dvec2( ::ceil(a.x()),
                  ::ceil(a.y()) );
  }
  inline dvec3 ceil(const dvec3& a) {
    return dvec3( ::ceil(a.x()),
                  ::ceil(a.y()),
                  ::ceil(a.z()) );
  }
  inline dvec4 ceil(const dvec4& a) {
    return dvec4( ::ceil(a.x()),
                  ::ceil(a.y()),
                  ::ceil(a.z()),
                  ::ceil(a.w()) );
  }

  // --------------- fract ---------------

  inline float fract(float a) { return a - floor(a); }
  inline fvec2 fract(const fvec2& a) { return a - floor(a); }
  inline fvec3 fract(const fvec3& a) { return a - floor(a); }
  inline fvec4 fract(const fvec4& a) { return a - floor(a); }

  // .....................................

  inline double fract(double a) { return a - floor(a); }
  inline dvec2 fract(const dvec2& a) { return a - floor(a); }
  inline dvec3 fract(const dvec3& a) { return a - floor(a); }
  inline dvec4 fract(const dvec4& a) { return a - floor(a); }

  // --------------- mod ---------------

  inline float mod(float a, float b) { return a - b * floor(a/b); }
  inline fvec2 mod(const fvec2& a, float b) { return a - b * floor(a/b); }
  inline fvec3 mod(const fvec3& a, float b) { return a - b * floor(a/b); }
  inline fvec4 mod(const fvec4& a, float b) { return a - b * floor(a/b); }
  inline fvec2 mod(const fvec2& a, const fvec2& b) { return a - b * floor(a/b); }
  inline fvec3 mod(const fvec3& a, const fvec3& b) { return a - b * floor(a/b); }
  inline fvec4 mod(const fvec4& a, const fvec4& b) { return a - b * floor(a/b); }

  // ....................................

  inline double mod(double a, double b) { return a - b * floor(a/b); }
  inline dvec2 mod(const dvec2& a, double b) { return a - b * floor(a/b); }
  inline dvec3 mod(const dvec3& a, double b) { return a - b * floor(a/b); }
  inline dvec4 mod(const dvec4& a, double b) { return a - b * floor(a/b); }
  inline dvec2 mod(const dvec2& a, const dvec2& b) { return a - b * floor(a/b); }
  inline dvec3 mod(const dvec3& a, const dvec3& b) { return a - b * floor(a/b); }
  inline dvec4 mod(const dvec4& a, const dvec4& b) { return a - b * floor(a/b); }

  // --------------- modf ---------------

  inline float modf(float a, float& intpart) {
    #if defined(_MSC_VER)
      return ::modf(a,&intpart);
    #else
      double dintpart = intpart;
      float r = (float)::modf((double)a,&dintpart);
      intpart = (float)dintpart;
      return r;
    #endif
  }
  inline fvec2 modf(const fvec2& a, fvec2& intpart) {
    return fvec2( modf(a.x(), intpart.x()),
                  modf(a.y(), intpart.y()) );
  }
  inline fvec3 modf(const fvec3& a, fvec3& intpart) {
    return fvec3( modf(a.x(), intpart.x()),
                  modf(a.y(), intpart.y()),
                  modf(a.z(), intpart.z()) );
  }
  inline fvec4 modf(const fvec4& a, fvec4& intpart) {
    return fvec4( modf(a.x(), intpart.x()),
                  modf(a.y(), intpart.y()),
                  modf(a.z(), intpart.z()),
                  modf(a.w(), intpart.w()) );
  }

  // ...................................

  inline double modf(double a, double& intpart) { return ::modf(a, &intpart); }
  inline dvec2 modf(const dvec2& a, dvec2& intpart) {
    return dvec2( modf(a.x(), intpart.x()),
                  modf(a.y(), intpart.y()) );
  }
  inline dvec3 modf(const dvec3& a, dvec3& intpart) {
    return dvec3( modf(a.x(), intpart.x()),
                  modf(a.y(), intpart.y()),
                  modf(a.z(), intpart.z()) );
  }
  inline dvec4 modf(const dvec4& a, dvec4& intpart) {
    return dvec4( modf(a.x(), intpart.x()),
                  modf(a.y(), intpart.y()),
                  modf(a.z(), intpart.z()),
                  modf(a.w(), intpart.w()) );
  }

  // --------------- min ---------------

  inline float min(float a, float b) { return a < b ? a : b; }
  inline fvec2 min(const fvec2& a, const fvec2& b)
  {
    return fvec2( a.x() < b.x() ? a.x() : b.x(),
                  a.y() < b.y() ? a.y() : b.y());
  }
  inline fvec3 min(const fvec3& a, const fvec3& b)
  {
    return fvec3( a.x() < b.x() ? a.x() : b.x(),
                  a.y() < b.y() ? a.y() : b.y(),
                  a.z() < b.z() ? a.z() : b.z() );
  }
  inline fvec4 min(const fvec4& a, const fvec4& b)
  {
    return fvec4( a.x() < b.x() ? a.x() : b.x(),
                  a.y() < b.y() ? a.y() : b.y(),
                  a.z() < b.z() ? a.z() : b.z(),
                  a.w() < b.w() ? a.w() : b.w() );
  }
  inline fvec2 min(const fvec2& a, float b)
  {
    return fvec2( a.x() < b ? a.x() : b,
                  a.y() < b ? a.y() : b);
  }
  inline fvec3 min(const fvec3& a, float b)
  {
    return fvec3( a.x() < b ? a.x() : b,
                  a.y() < b ? a.y() : b,
                  a.z() < b ? a.z() : b );
  }
  inline fvec4 min(const fvec4& a, float b)
  {
    return fvec4( a.x() < b ? a.x() : b,
                  a.y() < b ? a.y() : b,
                  a.z() < b ? a.z() : b,
                  a.w() < b ? a.w() : b );
  }

  // .....................................

  inline double min(double a, double b) { return a < b ? a : b; }
  inline dvec2 min(const dvec2& a, const dvec2& b)
  {
    return dvec2( a.x() < b.x() ? a.x() : b.x(),
                  a.y() < b.y() ? a.y() : b.y());
  }
  inline dvec3 min(const dvec3& a, const dvec3& b)
  {
    return dvec3( a.x() < b.x() ? a.x() : b.x(),
                  a.y() < b.y() ? a.y() : b.y(),
                  a.z() < b.z() ? a.z() : b.z() );
  }
  inline dvec4 min(const dvec4& a, const dvec4& b)
  {
    return dvec4( a.x() < b.x() ? a.x() : b.x(),
                  a.y() < b.y() ? a.y() : b.y(),
                  a.z() < b.z() ? a.z() : b.z(),
                  a.w() < b.w() ? a.w() : b.w() );
  }
  inline dvec2 min(const dvec2& a, double b)
  {
    return dvec2( a.x() < b ? a.x() : b,
                  a.y() < b ? a.y() : b);
  }
  inline dvec3 min(const dvec3& a, double b)
  {
    return dvec3( a.x() < b ? a.x() : b,
                  a.y() < b ? a.y() : b,
                  a.z() < b ? a.z() : b );
  }
  inline dvec4 min(const dvec4& a, double b)
  {
    return dvec4( a.x() < b ? a.x() : b,
                  a.y() < b ? a.y() : b,
                  a.z() < b ? a.z() : b,
                  a.w() < b ? a.w() : b );
  }

  // .....................................

  inline int min(int a, int b) { return a < b ? a : b; }
  inline ivec2 min(const ivec2& a, const ivec2& b)
  {
    return ivec2( a.x() < b.x() ? a.x() : b.x(),
                  a.y() < b.y() ? a.y() : b.y());
  }
  inline ivec3 min(const ivec3& a, const ivec3& b)
  {
    return ivec3( a.x() < b.x() ? a.x() : b.x(),
                  a.y() < b.y() ? a.y() : b.y(),
                  a.z() < b.z() ? a.z() : b.z() );
  }
  inline ivec4 min(const ivec4& a, const ivec4& b)
  {
    return ivec4( a.x() < b.x() ? a.x() : b.x(),
                  a.y() < b.y() ? a.y() : b.y(),
                  a.z() < b.z() ? a.z() : b.z(),
                  a.w() < b.w() ? a.w() : b.w() );
  }
  inline ivec2 min(const ivec2& a, int b)
  {
    return ivec2( a.x() < b ? a.x() : b,
                  a.y() < b ? a.y() : b);
  }
  inline ivec3 min(const ivec3& a, int b)
  {
    return ivec3( a.x() < b ? a.x() : b,
                  a.y() < b ? a.y() : b,
                  a.z() < b ? a.z() : b );
  }
  inline ivec4 min(const ivec4& a, int b)
  {
    return ivec4( a.x() < b ? a.x() : b,
                  a.y() < b ? a.y() : b,
                  a.z() < b ? a.z() : b,
                  a.w() < b ? a.w() : b );
  }

  // .....................................

  inline unsigned int min(unsigned int a, unsigned int b) { return a < b ? a : b; }
  inline uvec2 min(const uvec2& a, const uvec2& b)
  {
    return uvec2( a.x() < b.x() ? a.x() : b.x(),
                  a.y() < b.y() ? a.y() : b.y());
  }
  inline uvec3 min(const uvec3& a, const uvec3& b)
  {
    return uvec3( a.x() < b.x() ? a.x() : b.x(),
                  a.y() < b.y() ? a.y() : b.y(),
                  a.z() < b.z() ? a.z() : b.z() );
  }
  inline uvec4 min(const uvec4& a, const uvec4& b)
  {
    return uvec4( a.x() < b.x() ? a.x() : b.x(),
                  a.y() < b.y() ? a.y() : b.y(),
                  a.z() < b.z() ? a.z() : b.z(),
                  a.w() < b.w() ? a.w() : b.w() );
  }
  inline uvec2 min(const uvec2& a, unsigned int b)
  {
    return uvec2( a.x() < b ? a.x() : b,
                  a.y() < b ? a.y() : b);
  }
  inline uvec3 min(const uvec3& a, unsigned int b)
  {
    return uvec3( a.x() < b ? a.x() : b,
                  a.y() < b ? a.y() : b,
                  a.z() < b ? a.z() : b );
  }
  inline uvec4 min(const uvec4& a, unsigned int b)
  {
    return uvec4( a.x() < b ? a.x() : b,
                  a.y() < b ? a.y() : b,
                  a.z() < b ? a.z() : b,
                  a.w() < b ? a.w() : b );
  }

  // --------------- max ---------------

  inline float max(float a, float b) { return a > b ? a : b; }
  inline fvec2 max(const fvec2& a, const fvec2& b)
  {
    return fvec2( a.x() > b.x() ? a.x() : b.x(),
                  a.y() > b.y() ? a.y() : b.y());
  }
  inline fvec3 max(const fvec3& a, const fvec3& b)
  {
    return fvec3( a.x() > b.x() ? a.x() : b.x(),
                  a.y() > b.y() ? a.y() : b.y(),
                  a.z() > b.z() ? a.z() : b.z() );
  }
  inline fvec4 max(const fvec4& a, const fvec4& b)
  {
    return fvec4( a.x() > b.x() ? a.x() : b.x(),
                  a.y() > b.y() ? a.y() : b.y(),
                  a.z() > b.z() ? a.z() : b.z(),
                  a.w() > b.w() ? a.w() : b.w() );
  }
  inline fvec2 max(const fvec2& a, float b)
  {
    return fvec2( a.x() > b ? a.x() : b,
                  a.y() > b ? a.y() : b);
  }
  inline fvec3 max(const fvec3& a, float b)
  {
    return fvec3( a.x() > b ? a.x() : b,
                  a.y() > b ? a.y() : b,
                  a.z() > b ? a.z() : b );
  }
  inline fvec4 max(const fvec4& a, float b)
  {
    return fvec4( a.x() > b ? a.x() : b,
                  a.y() > b ? a.y() : b,
                  a.z() > b ? a.z() : b,
                  a.w() > b ? a.w() : b );
  }

  // ...................................

  inline double max(double a, double b) { return a > b ? a : b; }
  inline dvec2 max(const dvec2& a, const dvec2& b)
  {
    return dvec2( a.x() > b.x() ? a.x() : b.x(),
                  a.y() > b.y() ? a.y() : b.y());
  }
  inline dvec3 max(const dvec3& a, const dvec3& b)
  {
    return dvec3( a.x() > b.x() ? a.x() : b.x(),
                  a.y() > b.y() ? a.y() : b.y(),
                  a.z() > b.z() ? a.z() : b.z() );
  }
  inline dvec4 max(const dvec4& a, const dvec4& b)
  {
    return dvec4( a.x() > b.x() ? a.x() : b.x(),
                  a.y() > b.y() ? a.y() : b.y(),
                  a.z() > b.z() ? a.z() : b.z(),
                  a.w() > b.w() ? a.w() : b.w() );
  }
  inline dvec2 max(const dvec2& a, double b)
  {
    return dvec2( a.x() > b ? a.x() : b,
                  a.y() > b ? a.y() : b);
  }
  inline dvec3 max(const dvec3& a, double b)
  {
    return dvec3( a.x() > b ? a.x() : b,
                  a.y() > b ? a.y() : b,
                  a.z() > b ? a.z() : b );
  }
  inline dvec4 max(const dvec4& a, double b)
  {
    return dvec4( a.x() > b ? a.x() : b,
                  a.y() > b ? a.y() : b,
                  a.z() > b ? a.z() : b,
                  a.w() > b ? a.w() : b );
  }

  // ...................................

  inline int max(int a, int b) { return a > b ? a : b; }
  inline ivec2 max(const ivec2& a, const ivec2& b)
  {
    return ivec2( a.x() > b.x() ? a.x() : b.x(),
                  a.y() > b.y() ? a.y() : b.y());
  }
  inline ivec3 max(const ivec3& a, const ivec3& b)
  {
    return ivec3( a.x() > b.x() ? a.x() : b.x(),
                  a.y() > b.y() ? a.y() : b.y(),
                  a.z() > b.z() ? a.z() : b.z() );
  }
  inline ivec4 max(const ivec4& a, const ivec4& b)
  {
    return ivec4( a.x() > b.x() ? a.x() : b.x(),
                  a.y() > b.y() ? a.y() : b.y(),
                  a.z() > b.z() ? a.z() : b.z(),
                  a.w() > b.w() ? a.w() : b.w() );
  }
  inline ivec2 max(const ivec2& a, int b)
  {
    return ivec2( a.x() > b ? a.x() : b,
                  a.y() > b ? a.y() : b);
  }
  inline ivec3 max(const ivec3& a, int b)
  {
    return ivec3( a.x() > b ? a.x() : b,
                  a.y() > b ? a.y() : b,
                  a.z() > b ? a.z() : b );
  }
  inline ivec4 max(const ivec4& a, int b)
  {
    return ivec4( a.x() > b ? a.x() : b,
                  a.y() > b ? a.y() : b,
                  a.z() > b ? a.z() : b,
                  a.w() > b ? a.w() : b );
  }

  // ...................................

  inline unsigned int max(unsigned int a, unsigned int b) { return a > b ? a : b; }
  inline uvec2 max(const uvec2& a, const uvec2& b)
  {
    return uvec2( a.x() > b.x() ? a.x() : b.x(),
                  a.y() > b.y() ? a.y() : b.y());
  }
  inline uvec3 max(const uvec3& a, const uvec3& b)
  {
    return uvec3( a.x() > b.x() ? a.x() : b.x(),
                  a.y() > b.y() ? a.y() : b.y(),
                  a.z() > b.z() ? a.z() : b.z() );
  }
  inline uvec4 max(const uvec4& a, const uvec4& b)
  {
    return uvec4( a.x() > b.x() ? a.x() : b.x(),
                  a.y() > b.y() ? a.y() : b.y(),
                  a.z() > b.z() ? a.z() : b.z(),
                  a.w() > b.w() ? a.w() : b.w() );
  }
  inline uvec2 max(const uvec2& a, unsigned int b)
  {
    return uvec2( a.x() > b ? a.x() : b,
                  a.y() > b ? a.y() : b);
  }
  inline uvec3 max(const uvec3& a, unsigned int b)
  {
    return uvec3( a.x() > b ? a.x() : b,
                  a.y() > b ? a.y() : b,
                  a.z() > b ? a.z() : b );
  }
  inline uvec4 max(const uvec4& a, unsigned int b)
  {
    return uvec4( a.x() > b ? a.x() : b,
                  a.y() > b ? a.y() : b,
                  a.z() > b ? a.z() : b,
                  a.w() > b ? a.w() : b );
  }

  // --------------- clamp ---------------

  inline float clamp(float x, float minval, float maxval) { return min(max(x,minval),maxval); }
  inline fvec2 clamp(const fvec2& x, float minval, float maxval) { return min(max(x,minval),maxval); }
  inline fvec3 clamp(const fvec3& x, float minval, float maxval) { return min(max(x,minval),maxval); }
  inline fvec4 clamp(const fvec4& x, float minval, float maxval) { return min(max(x,minval),maxval); }
  inline fvec2 clamp(const fvec2& x, const fvec2& minval, const fvec2& maxval) { return min(max(x,minval),maxval); }
  inline fvec3 clamp(const fvec3& x, const fvec3& minval, const fvec3& maxval) { return min(max(x,minval),maxval); }
  inline fvec4 clamp(const fvec4& x, const fvec4& minval, const fvec4& maxval) { return min(max(x,minval),maxval); }

  // ...................................

  inline double clamp(double x, double minval, double maxval) { return min(max(x,minval),maxval); }
  inline dvec2 clamp(const dvec2& x, double minval, double maxval) { return min(max(x,minval),maxval); }
  inline dvec3 clamp(const dvec3& x, double minval, double maxval) { return min(max(x,minval),maxval); }
  inline dvec4 clamp(const dvec4& x, double minval, double maxval) { return min(max(x,minval),maxval); }
  inline dvec2 clamp(const dvec2& x, const dvec2& minval, const dvec2& maxval) { return min(max(x,minval),maxval); }
  inline dvec3 clamp(const dvec3& x, const dvec3& minval, const dvec3& maxval) { return min(max(x,minval),maxval); }
  inline dvec4 clamp(const dvec4& x, const dvec4& minval, const dvec4& maxval) { return min(max(x,minval),maxval); }

  // ...................................

  inline int clamp(int x, int minval, int maxval) { return min(max(x,minval),maxval); }
  inline ivec2 clamp(const ivec2& x, int minval, int maxval) { return min(max(x,minval),maxval); }
  inline ivec3 clamp(const ivec3& x, int minval, int maxval) { return min(max(x,minval),maxval); }
  inline ivec4 clamp(const ivec4& x, int minval, int maxval) { return min(max(x,minval),maxval); }
  inline ivec2 clamp(const ivec2& x, const ivec2& minval, const ivec2& maxval) { return min(max(x,minval),maxval); }
  inline ivec3 clamp(const ivec3& x, const ivec3& minval, const ivec3& maxval) { return min(max(x,minval),maxval); }
  inline ivec4 clamp(const ivec4& x, const ivec4& minval, const ivec4& maxval) { return min(max(x,minval),maxval); }

  // ...................................

  inline unsigned int clamp(unsigned int x, unsigned int minval, unsigned int maxval) { return min(max(x,minval),maxval); }
  inline uvec2 clamp(const uvec2& x, unsigned int minval, unsigned int maxval) { return min(max(x,minval),maxval); }
  inline uvec3 clamp(const uvec3& x, unsigned int minval, unsigned int maxval) { return min(max(x,minval),maxval); }
  inline uvec4 clamp(const uvec4& x, unsigned int minval, unsigned int maxval) { return min(max(x,minval),maxval); }
  inline uvec2 clamp(const uvec2& x, const uvec2& minval, const uvec2& maxval) { return min(max(x,minval),maxval); }
  inline uvec3 clamp(const uvec3& x, const uvec3& minval, const uvec3& maxval) { return min(max(x,minval),maxval); }
  inline uvec4 clamp(const uvec4& x, const uvec4& minval, const uvec4& maxval) { return min(max(x,minval),maxval); }

  // --------------- mix ---------------

  inline float mix(float a, float b, float t) { return a*(1.0f-t) + b*t; }
  inline fvec2 mix(const fvec2& a, const fvec2& b, float t) { return a*(1.0f-t) + b*t; }
  inline fvec3 mix(const fvec3& a, const fvec3& b, float t) { return a*(1.0f-t) + b*t; }
  inline fvec4 mix(const fvec4& a, const fvec4& b, float t) { return a*(1.0f-t) + b*t; }
  inline fvec2 mix(const fvec2& a, const fvec2& b, const fvec2& t)
  {
    return fvec2( a.x()*(1.0f-t.x()) + b.x()*t.x(),
                 a.y()*(1.0f-t.y()) + b.y()*t.y() );
  }
  inline fvec3 mix(const fvec3& a, const fvec3& b, const fvec3& t)
  {
    return fvec3( a.x()*(1.0f-t.x()) + b.x()*t.x(),
                 a.y()*(1.0f-t.y()) + b.y()*t.y(),
                 a.z()*(1.0f-t.z()) + b.z()*t.z() );
  }
  inline fvec4 mix(const fvec4& a, const fvec4& b, const fvec4& t)
  {
    return fvec4( a.x()*(1.0f-t.x()) + b.x()*t.x(),
                 a.y()*(1.0f-t.y()) + b.y()*t.y(),
                 a.z()*(1.0f-t.z()) + b.z()*t.z(),
                 a.w()*(1.0f-t.w()) + b.w()*t.w() );
  }

  // ....................................

  inline double mix(double a, double b, double t) { return a*(1.0f-t) + b*t; }
  inline dvec2 mix(const dvec2& a, const dvec2& b, double t) { return a*(1.0f-t) + b*t; }
  inline dvec3 mix(const dvec3& a, const dvec3& b, double t) { return a*(1.0f-t) + b*t; }
  inline dvec4 mix(const dvec4& a, const dvec4& b, double t) { return a*(1.0f-t) + b*t; }
  inline dvec2 mix(const dvec2& a, const dvec2& b, const dvec2& t)
  {
    return dvec2( a.x()*(1.0f-t.x()) + b.x()*t.x(),
                 a.y()*(1.0f-t.y()) + b.y()*t.y() );
  }
  inline dvec3 mix(const dvec3& a, const dvec3& b, const dvec3& t)
  {
    return dvec3( a.x()*(1.0f-t.x()) + b.x()*t.x(),
                 a.y()*(1.0f-t.y()) + b.y()*t.y(),
                 a.z()*(1.0f-t.z()) + b.z()*t.z() );
  }
  inline dvec4 mix(const dvec4& a, const dvec4& b, const dvec4& t)
  {
    return dvec4( a.x()*(1.0f-t.x()) + b.x()*t.x(),
                 a.y()*(1.0f-t.y()) + b.y()*t.y(),
                 a.z()*(1.0f-t.z()) + b.z()*t.z(),
                 a.w()*(1.0f-t.w()) + b.w()*t.w() );
  }

  // --------------- step ---------------

  inline float step( float edge, float a ) { if (a<edge) return 0.0f; else return 1.0f; }
  inline fvec2 step( const fvec2& edge, const fvec2& a )
  {
    return fvec2( a.x()<edge.x() ? 0.0f : 1.0f,
                 a.y()<edge.y() ? 0.0f : 1.0f );
  }
  inline fvec3 step( const fvec3& edge, const fvec3& a )
  {
    return fvec3( a.x()<edge.x() ? 0.0f : 1.0f,
                 a.y()<edge.y() ? 0.0f : 1.0f,
                 a.z()<edge.z() ? 0.0f : 1.0f );
  }
  inline fvec4 step( const fvec4& edge, const fvec4& a )
  {
    return fvec4( a.x()<edge.x() ? 0.0f : 1.0f,
                 a.y()<edge.y() ? 0.0f : 1.0f,
                 a.z()<edge.z() ? 0.0f : 1.0f,
                 a.w()<edge.w() ? 0.0f : 1.0f );
  }

  // ..........................................

  inline double step( double edge, double a ) { if (a<edge) return 0.0f; else return 1.0f; }
  inline dvec2 step( const dvec2& edge, const dvec2& a )
  {
    return dvec2( a.x()<edge.x() ? 0.0f : 1.0f,
                 a.y()<edge.y() ? 0.0f : 1.0f );
  }
  inline dvec3 step( const dvec3& edge, const dvec3& a )
  {
    return dvec3( a.x()<edge.x() ? 0.0f : 1.0f,
                 a.y()<edge.y() ? 0.0f : 1.0f,
                 a.z()<edge.z() ? 0.0f : 1.0f );
  }
  inline dvec4 step( const dvec4& edge, const dvec4& a )
  {
    return dvec4( a.x()<edge.x() ? 0.0f : 1.0f,
                 a.y()<edge.y() ? 0.0f : 1.0f,
                 a.z()<edge.z() ? 0.0f : 1.0f,
                 a.w()<edge.w() ? 0.0f : 1.0f );
  }
  // --------------- smoothstep ---------------

  inline float smoothstep(float edge0, float edge1, float a)
  {
    float t = clamp( (a - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    return t * t * (3.0f - 2.0f * t);
  }
  inline fvec2 smoothstep(const fvec2& edge0, const fvec2& edge1, const fvec2& a)
  {
    fvec2 v;
    float t;
    t = clamp( (a.x() - edge0.x()) / (edge1.x() - edge0.x()), 0.0f, 1.0f); v.x() = t * t * (3.0f - 2.0f * t);
    t = clamp( (a.y() - edge0.y()) / (edge1.y() - edge0.y()), 0.0f, 1.0f); v.y() = t * t * (3.0f - 2.0f * t);
    return v;
  }
  inline fvec3 smoothstep(const fvec3& edge0, const fvec3& edge1, const fvec3& a)
  {
    fvec3 v;
    float t;
    t = clamp( (a.x() - edge0.x()) / (edge1.x() - edge0.x()), 0.0f, 1.0f); v.x() = t * t * (3.0f - 2.0f * t);
    t = clamp( (a.y() - edge0.y()) / (edge1.y() - edge0.y()), 0.0f, 1.0f); v.y() = t * t * (3.0f - 2.0f * t);
    t = clamp( (a.z() - edge0.z()) / (edge1.z() - edge0.z()), 0.0f, 1.0f); v.z() = t * t * (3.0f - 2.0f * t);
    return v;
  }
  inline fvec4 smoothstep(const fvec4& edge0, const fvec4& edge1, const fvec4& a)
  {
    fvec4 v;
    float t;
    t = clamp( (a.x() - edge0.x()) / (edge1.x() - edge0.x()), 0.0f, 1.0f); v.x() = t * t * (3.0f - 2.0f * t);
    t = clamp( (a.y() - edge0.y()) / (edge1.y() - edge0.y()), 0.0f, 1.0f); v.y() = t * t * (3.0f - 2.0f * t);
    t = clamp( (a.z() - edge0.z()) / (edge1.z() - edge0.z()), 0.0f, 1.0f); v.z() = t * t * (3.0f - 2.0f * t);
    t = clamp( (a.w() - edge0.w()) / (edge1.w() - edge0.w()), 0.0f, 1.0f); v.w() = t * t * (3.0f - 2.0f * t);
    return v;
  }

  // ..........................................

  inline double smoothstep(double edge0, double edge1, double a)
  {
    double t = clamp( (a - edge0) / (edge1 - edge0), 0.0, 1.0);
    return t * t * (3.0 - 2.0 * t);
  }
  inline dvec2 smoothstep(const dvec2& edge0, const dvec2& edge1, const dvec2& a)
  {
    dvec2 v;
    double t;
    t = clamp( (a.x() - edge0.x()) / (edge1.x() - edge0.x()), 0.0, 1.0); v.x() = t * t * (3.0 - 2.0 * t);
    t = clamp( (a.y() - edge0.y()) / (edge1.y() - edge0.y()), 0.0, 1.0); v.y() = t * t * (3.0 - 2.0 * t);
    return v;
  }
  inline dvec3 smoothstep(const dvec3& edge0, const dvec3& edge1, const dvec3& a)
  {
    dvec3 v;
    double t;
    t = clamp( (a.x() - edge0.x()) / (edge1.x() - edge0.x()), 0.0, 1.0); v.x() = t * t * (3.0 - 2.0 * t);
    t = clamp( (a.y() - edge0.y()) / (edge1.y() - edge0.y()), 0.0, 1.0); v.y() = t * t * (3.0 - 2.0 * t);
    t = clamp( (a.z() - edge0.z()) / (edge1.z() - edge0.z()), 0.0, 1.0); v.z() = t * t * (3.0 - 2.0 * t);
    return v;
  }
  inline dvec4 smoothstep(const dvec4& edge0, const dvec4& edge1, const dvec4& a)
  {
    dvec4 v;
    double t;
    t = clamp( (a.x() - edge0.x()) / (edge1.x() - edge0.x()), 0.0, 1.0); v.x() = t * t * (3.0 - 2.0 * t);
    t = clamp( (a.y() - edge0.y()) / (edge1.y() - edge0.y()), 0.0, 1.0); v.y() = t * t * (3.0 - 2.0 * t);
    t = clamp( (a.z() - edge0.z()) / (edge1.z() - edge0.z()), 0.0, 1.0); v.z() = t * t * (3.0 - 2.0 * t);
    t = clamp( (a.w() - edge0.w()) / (edge1.w() - edge0.w()), 0.0, 1.0); v.w() = t * t * (3.0 - 2.0 * t);
    return v;
  }

  // --------------- isnan ---------------

  inline ivec2 isnan(const fvec2& a) { return ivec2( isnan(a.x()), isnan(a.y()) ); }
  inline ivec3 isnan(const fvec3& a) { return ivec3( isnan(a.x()), isnan(a.y()), isnan(a.z()) ); }
  inline ivec4 isnan(const fvec4& a) { return ivec4( isnan(a.x()), isnan(a.y()), isnan(a.z()), isnan(a.w()) ); }

  // --------------- isinf ---------------

  inline ivec2 isinf(const fvec2& a) { return ivec2( isinf(a.x()), isinf(a.y()) ); }
  inline ivec3 isinf(const fvec3& a) { return ivec3( isinf(a.x()), isinf(a.y()), isinf(a.z()) ); }
  inline ivec4 isinf(const fvec4& a) { return ivec4( isinf(a.x()), isinf(a.y()), isinf(a.z()), isinf(a.w()) ); }

  // --------------- geometric functions ---------------

  // --------------- length ---------------

  inline float length(float v) { return v; }
  inline float length(const fvec2& v) { return v.length(); }
  inline float length(const fvec3& v) { return v.length(); }
  inline float length(const fvec4& v) { return v.length(); }

  // ....................................

  inline double length(double v) { return v; }
  inline double length(const dvec2& v) { return v.length(); }
  inline double length(const dvec3& v) { return v.length(); }
  inline double length(const dvec4& v) { return v.length(); }

  // ....................................

  inline float length(int v) { return (float)v; }
  inline float length(const ivec2& v) { return fvec2(v).length(); }
  inline float length(const ivec3& v) { return fvec3(v).length(); }
  inline float length(const ivec4& v) { return fvec4(v).length(); }

  // ....................................

  inline float length(unsigned int v) { return (float)v; }
  inline float length(const uvec2& v) { return fvec2(v).length(); }
  inline float length(const uvec3& v) { return fvec3(v).length(); }
  inline float length(const uvec4& v) { return fvec4(v).length(); }

  // --------------- distance ---------------

  inline float distance(float p0, float p1) { return length(p0-p1); }
  inline float distance(const fvec2& p0, const fvec2& p1) { return length(p0-p1); }
  inline float distance(const fvec3& p0, const fvec3& p1) { return length(p0-p1); }
  inline float distance(const fvec4& p0, const fvec4& p1) { return length(p0-p1); }

  // ....................................

  inline double distance(double p0, double p1) { return length(p0-p1); }
  inline double distance(const dvec2& p0, const dvec2& p1) { return length(p0-p1); }
  inline double distance(const dvec3& p0, const dvec3& p1) { return length(p0-p1); }
  inline double distance(const dvec4& p0, const dvec4& p1) { return length(p0-p1); }

  // ....................................

  inline float distance(int p0, int p1) { return length(p0-p1); }
  inline float distance(const ivec2& p0, const ivec2& p1) { return length(p0-p1); }
  inline float distance(const ivec3& p0, const ivec3& p1) { return length(p0-p1); }
  inline float distance(const ivec4& p0, const ivec4& p1) { return length(p0-p1); }

  // ....................................

  inline float distance(unsigned int p0, unsigned int p1) { return length(p0-p1); }
  inline float distance(const uvec2& p0, const uvec2& p1) { return length(p0-p1); }
  inline float distance(const uvec3& p0, const uvec3& p1) { return length(p0-p1); }
  inline float distance(const uvec4& p0, const uvec4& p1) { return length(p0-p1); }

  // --------------- dot ---------------

  inline float dot(float a, float b) { return a*b; }
  inline float dot(const fvec2& v1, const fvec2& v2) { return v1.x()*v2.x() + v1.y()*v2.y(); }
  inline float dot(const fvec3& v1, const fvec3& v2) { return v1.x()*v2.x() + v1.y()*v2.y() + v1.z()*v2.z(); }
  inline float dot(const fvec4& v1, const fvec4& v2) { return v1.x()*v2.x() + v1.y()*v2.y() + v1.z()*v2.z() + v1.w()*v2.w(); }

  // ....................................

  inline double dot(double a, double b) { return a*b; }
  inline double dot(const dvec2& v1, const dvec2& v2) { return v1.x()*v2.x() + v1.y()*v2.y(); }
  inline double dot(const dvec3& v1, const dvec3& v2) { return v1.x()*v2.x() + v1.y()*v2.y() + v1.z()*v2.z(); }
  inline double dot(const dvec4& v1, const dvec4& v2) { return v1.x()*v2.x() + v1.y()*v2.y() + v1.z()*v2.z() + v1.w()*v2.w(); }

  // ....................................

  inline float dot(int a, int b) { return (float)a*b; }
  inline float dot(const ivec2& v1, const ivec2& v2) { return (float)(v1.x()*v2.x() + v1.y()*v2.y()); }
  inline float dot(const ivec3& v1, const ivec3& v2) { return (float)(v1.x()*v2.x() + v1.y()*v2.y() + v1.z()*v2.z()); }
  inline float dot(const ivec4& v1, const ivec4& v2) { return (float)(v1.x()*v2.x() + v1.y()*v2.y() + v1.z()*v2.z() + v1.w()*v2.w()); }

  // ....................................

  inline float dot(unsigned int a, unsigned int b) { return (float)a*b; }
  inline float dot(const uvec2& v1, const uvec2& v2) { return (float)(v1.x()*v2.x() + v1.y()*v2.y()); }
  inline float dot(const uvec3& v1, const uvec3& v2) { return (float)(v1.x()*v2.x() + v1.y()*v2.y() + v1.z()*v2.z()); }
  inline float dot(const uvec4& v1, const uvec4& v2) { return (float)(v1.x()*v2.x() + v1.y()*v2.y() + v1.z()*v2.z() + v1.w()*v2.w()); }

  // --------------- cross ---------------

  inline fvec3 cross(const fvec3& v1, const fvec3& v2)
  {
    fvec3 t;
    t.x() = v1.y()*v2.z() - v1.z()*v2.y() ;
    t.y() = v1.z()*v2.x() - v1.x()*v2.z() ;
    t.z() = v1.x()*v2.y() - v1.y()*v2.x() ;
    return t;
  }

  // ......................................

  inline dvec3 cross(const dvec3& v1, const dvec3& v2)
  {
    dvec3 t;
    t.x() = v1.y()*v2.z() - v1.z()*v2.y() ;
    t.y() = v1.z()*v2.x() - v1.x()*v2.z() ;
    t.z() = v1.x()*v2.y() - v1.y()*v2.x() ;
    return t;
  }

  // --------------- normalize ---------------

  inline float normalize(float) { return 1; }
  inline fvec2 normalize(const fvec2& v) { fvec2 t = v; t.normalize(); return t; }
  inline fvec3 normalize(const fvec3& v) { fvec3 t = v; t.normalize(); return t; }
  inline fvec4 normalize(const fvec4& v) { fvec4 t = v; t.normalize(); return t; }

  // .........................................

  inline double normalize(double) { return 1; }
  inline dvec2 normalize(const dvec2& v) { dvec2 t = v; t.normalize(); return t; }
  inline dvec3 normalize(const dvec3& v) { dvec3 t = v; t.normalize(); return t; }
  inline dvec4 normalize(const dvec4& v) { dvec4 t = v; t.normalize(); return t; }

  // --------------- faceforward ---------------

  inline float faceforward(float N, float I, float Nref) { if ( dot(Nref,I) < 0 ) return N ; else return -N; }
  inline fvec2 faceforward(const fvec2& N, const fvec2& I, const fvec2& Nref) { if ( dot(Nref,I) < 0 ) return N ; else return -N; }
  inline fvec3 faceforward(const fvec3& N, const fvec3& I, const fvec3& Nref) { if ( dot(Nref,I) < 0 ) return N ; else return -N; }
  inline fvec4 faceforward(const fvec4& N, const fvec4& I, const fvec4& Nref) { if ( dot(Nref,I) < 0 ) return N ; else return -N; }

  // ...........................................

  inline double faceforward(double N, double I, double Nref) { if ( dot(Nref,I) < 0 ) return N ; else return -N; }
  inline dvec2 faceforward(const dvec2& N, const dvec2& I, const dvec2& Nref) { if ( dot(Nref,I) < 0 ) return N ; else return -N; }
  inline dvec3 faceforward(const dvec3& N, const dvec3& I, const dvec3& Nref) { if ( dot(Nref,I) < 0 ) return N ; else return -N; }
  inline dvec4 faceforward(const dvec4& N, const dvec4& I, const dvec4& Nref) { if ( dot(Nref,I) < 0 ) return N ; else return -N; }

  // --------------- reflect ----------------

  inline float reflect(float I, float N) { return I-2.0f*dot(N,I)*N; }
  inline fvec2 reflect(const fvec2& I, const fvec2& N) { return I-2.0f*dot(N,I)*N; }
  inline fvec3 reflect(const fvec3& I, const fvec3& N) { return I-2.0f*dot(N,I)*N; }
  inline fvec4 reflect(const fvec4& I, const fvec4& N) { return I-2.0f*dot(N,I)*N; }

  inline double reflect(double I, double N) { return I-2.0*dot(N,I)*N; }
  inline dvec2 reflect(const dvec2& I, const dvec2& N) { return I-2.0*dot(N,I)*N; }
  inline dvec3 reflect(const dvec3& I, const dvec3& N) { return I-2.0*dot(N,I)*N; }
  inline dvec4 reflect(const dvec4& I, const dvec4& N) { return I-2.0*dot(N,I)*N; }

  // --------------- refract ---------------

  inline float refract(float I, float N, float eta)
  {
    float k = 1.0f - eta * eta * (1.0f - dot(N, I) * dot(N, I));
    if (k < 0.0f)
      return 0.0f;
    else
      return eta * I - (eta * dot(N, I) + VL_FLOAT_SQRT(k)) * N;
  }
  inline fvec2 refract(const fvec2& I, const fvec2& N, float eta)
  {
    float k = 1.0f - eta * eta * (1.0f - dot(N, I) * dot(N, I));
    if (k < 0.0f)
      return fvec2(0,0);
    else
      return eta * I - (eta * dot(N, I) + VL_FLOAT_SQRT(k)) * N;
  }
  inline fvec3 refract(const fvec3& I, const fvec3& N, float eta)
  {
    float k = 1.0f - eta * eta * (1.0f - dot(N, I) * dot(N, I));
    if (k < 0.0f)
      return fvec3(0,0,0);
    else
      return eta * I - (eta * dot(N, I) + VL_FLOAT_SQRT(k)) * N;
  }
  inline fvec4 refract(const fvec4& I, const fvec4& N, float eta)
  {
    float k = 1.0f - eta * eta * (1.0f - dot(N, I) * dot(N, I));
    if (k < 0.0f)
      return fvec4(0,0,0,0);
    else
      return eta * I - (eta * dot(N, I) + VL_FLOAT_SQRT(k)) * N;
  }

  // ...................................

  inline double refract(double I, double N, double eta)
  {
    double k = 1.0 - eta * eta * (1.0 - dot(N, I) * dot(N, I));
    if (k < 0.0)
      return 0.0;
    else
      return eta * I - (eta * dot(N, I) + ::sqrt(k)) * N;
  }
  inline dvec2 refract(const dvec2& I, const dvec2& N, double eta)
  {
    double k = 1.0 - eta * eta * (1.0 - dot(N, I) * dot(N, I));
    if (k < 0.0)
      return dvec2(0,0);
    else
      return eta * I - (eta * dot(N, I) + ::sqrt(k)) * N;
  }
  inline dvec3 refract(const dvec3& I, const dvec3& N, double eta)
  {
    double k = 1.0 - eta * eta * (1.0 - dot(N, I) * dot(N, I));
    if (k < 0.0)
      return dvec3(0,0,0);
    else
      return eta * I - (eta * dot(N, I) + ::sqrt(k)) * N;
  }
  inline dvec4 refract(const dvec4& I, const dvec4& N, double eta)
  {
    double k = 1.0 - eta * eta * (1.0 - dot(N, I) * dot(N, I));
    if (k < 0.0)
      return dvec4(0,0,0,0);
    else
      return eta * I - (eta * dot(N, I) + ::sqrt(k)) * N;
  }

  // --------------- matrix functions ---------------

  // --------------- matrixCompMult ---------------

  inline fmat2 matrixCompMult(const fmat2& a, const fmat2& b)
  {
    fmat2 t;
    for(int i=0; i<2; ++i)
      for(int j=0; j<2; ++j)
        t[i][j] = a[i][j] * b[i][j];
    return t;
  }
  inline fmat3 matrixCompMult(const fmat3& a, const fmat3& b)
  {
    fmat3 t;
    for(int i=0; i<3; ++i)
      for(int j=0; j<3; ++j)
        t[i][j] = a[i][j] * b[i][j];
    return t;
  }
  inline fmat4 matrixCompMult(const fmat4& a, const fmat4& b)
  {
    fmat4 t;
    for(int i=0; i<4; ++i)
      for(int j=0; j<4; ++j)
        t[i][j] = a[i][j] * b[i][j];
    return t;
  }

 // .........................................

 inline dmat2 matrixCompMult(const dmat2& a, const dmat2& b)
  {
    dmat2 t;
    for(int i=0; i<2; ++i)
      for(int j=0; j<2; ++j)
        t[i][j] = a[i][j] * b[i][j];
    return t;
  }
  inline dmat3 matrixCompMult(const dmat3& a, const dmat3& b)
  {
    dmat3 t;
    for(int i=0; i<3; ++i)
      for(int j=0; j<3; ++j)
        t[i][j] = a[i][j] * b[i][j];
    return t;
  }
  inline dmat4 matrixCompMult(const dmat4& a, const dmat4& b)
  {
    dmat4 t;
    for(int i=0; i<4; ++i)
      for(int j=0; j<4; ++j)
        t[i][j] = a[i][j] * b[i][j];
    return t;
  }

  // --------------- outerProduct ---------------

  inline fmat2 outerProduct(const fvec2& a, const fvec2& b)
  {
    fmat2 m;
    for(int i=0; i<2; ++i)
      for(int j=0; j<2; ++j)
        m[j][i] = a[i] * b[j];
    return m;
  }
  inline fmat3 outerProduct(const fvec3& a, const fvec3& b)
  {
    fmat3 m;
    for(int i=0; i<3; ++i)
      for(int j=0; j<3; ++j)
        m[j][i] = a[i] * b[j];
    return m;
  }
  inline fmat4 outerProduct(const fvec4& a, const fvec4& b)
  {
    fmat4 m;
    for(int i=0; i<4; ++i)
      for(int j=0; j<4; ++j)
        m[j][i] = a[i] * b[j];
    return m;
  }

  // ..............................................

  inline dmat2 outerProduct(const dvec2& a, const dvec2& b)
  {
    dmat2 m;
    for(int i=0; i<2; ++i)
      for(int j=0; j<2; ++j)
        m[j][i] = a[i] * b[j];
    return m;
  }
  inline dmat3 outerProduct(const dvec3& a, const dvec3& b)
  {
    dmat3 m;
    for(int i=0; i<3; ++i)
      for(int j=0; j<3; ++j)
        m[j][i] = a[i] * b[j];
    return m;
  }
  inline dmat4 outerProduct(const dvec4& a, const dvec4& b)
  {
    dmat4 m;
    for(int i=0; i<4; ++i)
      for(int j=0; j<4; ++j)
        m[j][i] = a[i] * b[j];
    return m;
  }

  // --------------- transpose ---------------

  inline fmat2 transpose(const fmat2& a)
  {
    fmat2 t;
    for(int i=0; i<2; ++i)
      for(int j=0; j<2; ++j)
        t[i][j] = a[j][i];
    return t;
  }
  inline fmat3 transpose(const fmat3& a)
  {
    fmat3 t;
    for(int i=0; i<3; ++i)
      for(int j=0; j<3; ++j)
        t[i][j] = a[j][i];
    return t;
  }
  inline fmat4 transpose(const fmat4& a)
  {
    fmat4 t;
    for(int i=0; i<4; ++i)
      for(int j=0; j<4; ++j)
        t[i][j] = a[j][i];
    return t;
  }

  // ............................................

  inline dmat2 transpose(const dmat2& a)
  {
    dmat2 t;
    for(int i=0; i<2; ++i)
      for(int j=0; j<2; ++j)
        t[i][j] = a[j][i];
    return t;
  }
  inline dmat3 transpose(const dmat3& a)
  {
    dmat3 t;
    for(int i=0; i<3; ++i)
      for(int j=0; j<3; ++j)
        t[i][j] = a[j][i];
    return t;
  }
  inline dmat4 transpose(const dmat4& a)
  {
    dmat4 t;
    for(int i=0; i<4; ++i)
      for(int j=0; j<4; ++j)
        t[i][j] = a[j][i];
    return t;
  }

  // --------------- vector relational functions ---------------

  // --------------- lessThan ---------------

  inline ivec4 lessThan(const fvec4& a, const fvec4& b) {
    return ivec4( a.x() < b.x() ? 1 : 0,
                  a.y() < b.y() ? 1 : 0,
                  a.z() < b.z() ? 1 : 0,
                  a.w() < b.w() ? 1 : 0 );
  }

  inline ivec3 lessThan(const fvec3& a, const fvec3& b) {
    return ivec3( a.x() < b.x() ? 1 : 0,
                  a.y() < b.y() ? 1 : 0,
                  a.z() < b.z() ? 1 : 0 );
  }

  inline ivec2 lessThan(const fvec2& a, const fvec2& b) {
    return ivec2( a.x() < b.x() ? 1 : 0,
                  a.y() < b.y() ? 1 : 0 );
  }

  // .............................................

  inline ivec4 lessThan(const dvec4& a, const dvec4& b) {
    return ivec4( a.x() < b.x() ? 1 : 0,
                  a.y() < b.y() ? 1 : 0,
                  a.z() < b.z() ? 1 : 0,
                  a.w() < b.w() ? 1 : 0 );
  }

  inline ivec3 lessThan(const dvec3& a, const dvec3& b) {
    return ivec3( a.x() < b.x() ? 1 : 0,
                  a.y() < b.y() ? 1 : 0,
                  a.z() < b.z() ? 1 : 0 );
  }

  inline ivec2 lessThan(const dvec2& a, const dvec2& b) {
    return ivec2( a.x() < b.x() ? 1 : 0,
                  a.y() < b.y() ? 1 : 0 );
  }

  // .............................................

  inline ivec4 lessThan(const ivec4& a, const ivec4& b) {
    return ivec4( a.x() < b.x() ? 1 : 0,
                  a.y() < b.y() ? 1 : 0,
                  a.z() < b.z() ? 1 : 0,
                  a.w() < b.w() ? 1 : 0 );
  }

  inline ivec3 lessThan(const ivec3& a, const ivec3& b) {
    return ivec3( a.x() < b.x() ? 1 : 0,
                  a.y() < b.y() ? 1 : 0,
                  a.z() < b.z() ? 1 : 0 );
  }

  inline ivec2 lessThan(const ivec2& a, const ivec2& b) {
    return ivec2( a.x() < b.x() ? 1 : 0,
                  a.y() < b.y() ? 1 : 0 );
  }

  // .............................................

  inline ivec4 lessThan(const uvec4& a, const uvec4& b) {
    return ivec4( a.x() < b.x() ? 1 : 0,
                  a.y() < b.y() ? 1 : 0,
                  a.z() < b.z() ? 1 : 0,
                  a.w() < b.w() ? 1 : 0 );
  }

  inline ivec3 lessThan(const uvec3& a, const uvec3& b) {
    return ivec3( a.x() < b.x() ? 1 : 0,
                  a.y() < b.y() ? 1 : 0,
                  a.z() < b.z() ? 1 : 0 );
  }

  inline ivec2 lessThan(const uvec2& a, const uvec2& b) {
    return ivec2( a.x() < b.x() ? 1 : 0,
                  a.y() < b.y() ? 1 : 0 );
  }

  // --------------- lessThanEqual ---------------

  inline ivec4 lessThanEqual(const fvec4& a, const fvec4& b) {
    return ivec4( a.x() <= b.x() ? 1 : 0,
                  a.y() <= b.y() ? 1 : 0,
                  a.z() <= b.z() ? 1 : 0,
                  a.w() <= b.w() ? 1 : 0 );
  }

  inline ivec3 lessThanEqual(const fvec3& a, const fvec3& b) {
    return ivec3( a.x() <= b.x() ? 1 : 0,
                  a.y() <= b.y() ? 1 : 0,
                  a.z() <= b.z() ? 1 : 0 );
  }

  inline ivec2 lessThanEqual(const fvec2& a, const fvec2& b) {
    return ivec2( a.x() <= b.x() ? 1 : 0,
                  a.y() <= b.y() ? 1 : 0 );
  }

  // .............................................

  inline ivec4 lessThanEqual(const dvec4& a, const dvec4& b) {
    return ivec4( a.x() <= b.x() ? 1 : 0,
                  a.y() <= b.y() ? 1 : 0,
                  a.z() <= b.z() ? 1 : 0,
                  a.w() <= b.w() ? 1 : 0 );
  }

  inline ivec3 lessThanEqual(const dvec3& a, const dvec3& b) {
    return ivec3( a.x() <= b.x() ? 1 : 0,
                  a.y() <= b.y() ? 1 : 0,
                  a.z() <= b.z() ? 1 : 0 );
  }

  inline ivec2 lessThanEqual(const dvec2& a, const dvec2& b) {
    return ivec2( a.x() <= b.x() ? 1 : 0,
                  a.y() <= b.y() ? 1 : 0 );
  }

  // .............................................

  inline ivec4 lessThanEqual(const ivec4& a, const ivec4& b) {
    return ivec4( a.x() <= b.x() ? 1 : 0,
                  a.y() <= b.y() ? 1 : 0,
                  a.z() <= b.z() ? 1 : 0,
                  a.w() <= b.w() ? 1 : 0 );
  }

  inline ivec3 lessThanEqual(const ivec3& a, const ivec3& b) {
    return ivec3( a.x() <= b.x() ? 1 : 0,
                  a.y() <= b.y() ? 1 : 0,
                  a.z() <= b.z() ? 1 : 0 );
  }

  inline ivec2 lessThanEqual(const ivec2& a, const ivec2& b) {
    return ivec2( a.x() <= b.x() ? 1 : 0,
                  a.y() <= b.y() ? 1 : 0 );
  }

  // .............................................

  inline ivec4 lessThanEqual(const uvec4& a, const uvec4& b) {
    return ivec4( a.x() <= b.x() ? 1 : 0,
                  a.y() <= b.y() ? 1 : 0,
                  a.z() <= b.z() ? 1 : 0,
                  a.w() <= b.w() ? 1 : 0 );
  }

  inline ivec3 lessThanEqual(const uvec3& a, const uvec3& b) {
    return ivec3( a.x() <= b.x() ? 1 : 0,
                  a.y() <= b.y() ? 1 : 0,
                  a.z() <= b.z() ? 1 : 0 );
  }

  inline ivec2 lessThanEqual(const uvec2& a, const uvec2& b) {
    return ivec2( a.x() <= b.x() ? 1 : 0,
                  a.y() <= b.y() ? 1 : 0 );
  }

  // --------------- greaterThan ---------------

  inline ivec4 greaterThan(const fvec4& a, const fvec4& b) {
    return ivec4( a.x() > b.x() ? 1 : 0,
                  a.y() > b.y() ? 1 : 0,
                  a.z() > b.z() ? 1 : 0,
                  a.w() > b.w() ? 1 : 0 );
  }

  inline ivec3 greaterThan(const fvec3& a, const fvec3& b) {
    return ivec3( a.x() > b.x() ? 1 : 0,
                  a.y() > b.y() ? 1 : 0,
                  a.z() > b.z() ? 1 : 0 );
  }

  inline ivec2 greaterThan(const fvec2& a, const fvec2& b) {
    return ivec2( a.x() > b.x() ? 1 : 0,
                  a.y() > b.y() ? 1 : 0 );
  }

  // .............................................

  inline ivec4 greaterThan(const dvec4& a, const dvec4& b) {
    return ivec4( a.x() > b.x() ? 1 : 0,
                  a.y() > b.y() ? 1 : 0,
                  a.z() > b.z() ? 1 : 0,
                  a.w() > b.w() ? 1 : 0 );
  }

  inline ivec3 greaterThan(const dvec3& a, const dvec3& b) {
    return ivec3( a.x() > b.x() ? 1 : 0,
                  a.y() > b.y() ? 1 : 0,
                  a.z() > b.z() ? 1 : 0 );
  }

  inline ivec2 greaterThan(const dvec2& a, const dvec2& b) {
    return ivec2( a.x() > b.x() ? 1 : 0,
                  a.y() > b.y() ? 1 : 0 );
  }

  // .............................................

  inline ivec4 greaterThan(const ivec4& a, const ivec4& b) {
    return ivec4( a.x() > b.x() ? 1 : 0,
                  a.y() > b.y() ? 1 : 0,
                  a.z() > b.z() ? 1 : 0,
                  a.w() > b.w() ? 1 : 0 );
  }

  inline ivec3 greaterThan(const ivec3& a, const ivec3& b) {
    return ivec3( a.x() > b.x() ? 1 : 0,
                  a.y() > b.y() ? 1 : 0,
                  a.z() > b.z() ? 1 : 0 );
  }

  inline ivec2 greaterThan(const ivec2& a, const ivec2& b) {
    return ivec2( a.x() > b.x() ? 1 : 0,
                  a.y() > b.y() ? 1 : 0 );
  }

  // .............................................

  inline ivec4 greaterThan(const uvec4& a, const uvec4& b) {
    return ivec4( a.x() > b.x() ? 1 : 0,
                  a.y() > b.y() ? 1 : 0,
                  a.z() > b.z() ? 1 : 0,
                  a.w() > b.w() ? 1 : 0 );
  }

  inline ivec3 greaterThan(const uvec3& a, const uvec3& b) {
    return ivec3( a.x() > b.x() ? 1 : 0,
                  a.y() > b.y() ? 1 : 0,
                  a.z() > b.z() ? 1 : 0 );
  }

  inline ivec2 greaterThan(const uvec2& a, const uvec2& b) {
    return ivec2( a.x() > b.x() ? 1 : 0,
                  a.y() > b.y() ? 1 : 0 );
  }

  // --------------- greaterThanEqual ---------------

  inline ivec4 greaterThanEqual(const fvec4& a, const fvec4& b) {
    return ivec4( a.x() >= b.x() ? 1 : 0,
                  a.y() >= b.y() ? 1 : 0,
                  a.z() >= b.z() ? 1 : 0,
                  a.w() >= b.w() ? 1 : 0 );
  }

  inline ivec3 greaterThanEqual(const fvec3& a, const fvec3& b) {
    return ivec3( a.x() >= b.x() ? 1 : 0,
                  a.y() >= b.y() ? 1 : 0,
                  a.z() >= b.z() ? 1 : 0 );
  }

  inline ivec2 greaterThanEqual(const fvec2& a, const fvec2& b) {
    return ivec2( a.x() >= b.x() ? 1 : 0,
                  a.y() >= b.y() ? 1 : 0 );
  }

  // .............................................

  inline ivec4 greaterThanEqual(const dvec4& a, const dvec4& b) {
    return ivec4( a.x() >= b.x() ? 1 : 0,
                  a.y() >= b.y() ? 1 : 0,
                  a.z() >= b.z() ? 1 : 0,
                  a.w() >= b.w() ? 1 : 0 );
  }

  inline ivec3 greaterThanEqual(const dvec3& a, const dvec3& b) {
    return ivec3( a.x() >= b.x() ? 1 : 0,
                  a.y() >= b.y() ? 1 : 0,
                  a.z() >= b.z() ? 1 : 0 );
  }

  inline ivec2 greaterThanEqual(const dvec2& a, const dvec2& b) {
    return ivec2( a.x() >= b.x() ? 1 : 0,
                  a.y() >= b.y() ? 1 : 0 );
  }

  // .............................................

  inline ivec4 greaterThanEqual(const ivec4& a, const ivec4& b) {
    return ivec4( a.x() >= b.x() ? 1 : 0,
                  a.y() >= b.y() ? 1 : 0,
                  a.z() >= b.z() ? 1 : 0,
                  a.w() >= b.w() ? 1 : 0 );
  }

  inline ivec3 greaterThanEqual(const ivec3& a, const ivec3& b) {
    return ivec3( a.x() >= b.x() ? 1 : 0,
                  a.y() >= b.y() ? 1 : 0,
                  a.z() >= b.z() ? 1 : 0 );
  }

  inline ivec2 greaterThanEqual(const ivec2& a, const ivec2& b) {
    return ivec2( a.x() >= b.x() ? 1 : 0,
                  a.y() >= b.y() ? 1 : 0 );
  }

  // .............................................

  inline ivec4 greaterThanEqual(const uvec4& a, const uvec4& b) {
    return ivec4( a.x() >= b.x() ? 1 : 0,
                  a.y() >= b.y() ? 1 : 0,
                  a.z() >= b.z() ? 1 : 0,
                  a.w() >= b.w() ? 1 : 0 );
  }

  inline ivec3 greaterThanEqual(const uvec3& a, const uvec3& b) {
    return ivec3( a.x() >= b.x() ? 1 : 0,
                  a.y() >= b.y() ? 1 : 0,
                  a.z() >= b.z() ? 1 : 0 );
  }

  inline ivec2 greaterThanEqual(const uvec2& a, const uvec2& b) {
    return ivec2( a.x() >= b.x() ? 1 : 0,
                  a.y() >= b.y() ? 1 : 0 );
  }

  // --------------- equal ---------------

  inline ivec4 equal(const fvec4& a, const fvec4& b) {
    return ivec4( a.x() == b.x() ? 1 : 0,
                  a.y() == b.y() ? 1 : 0,
                  a.z() == b.z() ? 1 : 0,
                  a.w() == b.w() ? 1 : 0 );
  }

  inline ivec3 equal(const fvec3& a, const fvec3& b) {
    return ivec3( a.x() == b.x() ? 1 : 0,
                  a.y() == b.y() ? 1 : 0,
                  a.z() == b.z() ? 1 : 0 );
  }

  inline ivec2 equal(const fvec2& a, const fvec2& b) {
    return ivec2( a.x() == b.x() ? 1 : 0,
                  a.y() == b.y() ? 1 : 0 );
  }

  // .............................................

  inline ivec4 equal(const dvec4& a, const dvec4& b) {
    return ivec4( a.x() == b.x() ? 1 : 0,
                  a.y() == b.y() ? 1 : 0,
                  a.z() == b.z() ? 1 : 0,
                  a.w() == b.w() ? 1 : 0 );
  }

  inline ivec3 equal(const dvec3& a, const dvec3& b) {
    return ivec3( a.x() == b.x() ? 1 : 0,
                  a.y() == b.y() ? 1 : 0,
                  a.z() == b.z() ? 1 : 0 );
  }

  inline ivec2 equal(const dvec2& a, const dvec2& b) {
    return ivec2( a.x() == b.x() ? 1 : 0,
                  a.y() == b.y() ? 1 : 0 );
  }

  // .............................................

  inline ivec4 equal(const ivec4& a, const ivec4& b) {
    return ivec4( a.x() == b.x() ? 1 : 0,
                  a.y() == b.y() ? 1 : 0,
                  a.z() == b.z() ? 1 : 0,
                  a.w() == b.w() ? 1 : 0 );
  }

  inline ivec3 equal(const ivec3& a, const ivec3& b) {
    return ivec3( a.x() == b.x() ? 1 : 0,
                  a.y() == b.y() ? 1 : 0,
                  a.z() == b.z() ? 1 : 0 );
  }

  inline ivec2 equal(const ivec2& a, const ivec2& b) {
    return ivec2( a.x() == b.x() ? 1 : 0,
                  a.y() == b.y() ? 1 : 0 );
  }

  // .............................................

  inline ivec4 equal(const uvec4& a, const uvec4& b) {
    return ivec4( a.x() == b.x() ? 1 : 0,
                  a.y() == b.y() ? 1 : 0,
                  a.z() == b.z() ? 1 : 0,
                  a.w() == b.w() ? 1 : 0 );
  }

  inline ivec3 equal(const uvec3& a, const uvec3& b) {
    return ivec3( a.x() == b.x() ? 1 : 0,
                  a.y() == b.y() ? 1 : 0,
                  a.z() == b.z() ? 1 : 0 );
  }

  inline ivec2 equal(const uvec2& a, const uvec2& b) {
    return ivec2( a.x() == b.x() ? 1 : 0,
                  a.y() == b.y() ? 1 : 0 );
  }

  // --------------- notEqual ---------------

  inline ivec4 notEqual(const fvec4& a, const fvec4& b) {
    return ivec4( a.x() != b.x() ? 1 : 0,
                  a.y() != b.y() ? 1 : 0,
                  a.z() != b.z() ? 1 : 0,
                  a.w() != b.w() ? 1 : 0 );
  }

  inline ivec3 notEqual(const fvec3& a, const fvec3& b) {
    return ivec3( a.x() != b.x() ? 1 : 0,
                  a.y() != b.y() ? 1 : 0,
                  a.z() != b.z() ? 1 : 0 );
  }

  inline ivec2 notEqual(const fvec2& a, const fvec2& b) {
    return ivec2( a.x() != b.x() ? 1 : 0,
                  a.y() != b.y() ? 1 : 0 );
  }

  // .............................................

  inline ivec4 notEqual(const dvec4& a, const dvec4& b) {
    return ivec4( a.x() != b.x() ? 1 : 0,
                  a.y() != b.y() ? 1 : 0,
                  a.z() != b.z() ? 1 : 0,
                  a.w() != b.w() ? 1 : 0 );
  }

  inline ivec3 notEqual(const dvec3& a, const dvec3& b) {
    return ivec3( a.x() != b.x() ? 1 : 0,
                  a.y() != b.y() ? 1 : 0,
                  a.z() != b.z() ? 1 : 0 );
  }

  inline ivec2 notEqual(const dvec2& a, const dvec2& b) {
    return ivec2( a.x() != b.x() ? 1 : 0,
                  a.y() != b.y() ? 1 : 0 );
  }

  // .............................................

  inline ivec4 notEqual(const ivec4& a, const ivec4& b) {
    return ivec4( a.x() != b.x() ? 1 : 0,
                  a.y() != b.y() ? 1 : 0,
                  a.z() != b.z() ? 1 : 0,
                  a.w() != b.w() ? 1 : 0 );
  }

  inline ivec3 notEqual(const ivec3& a, const ivec3& b) {
    return ivec3( a.x() != b.x() ? 1 : 0,
                  a.y() != b.y() ? 1 : 0,
                  a.z() != b.z() ? 1 : 0 );
  }

  inline ivec2 notEqual(const ivec2& a, const ivec2& b) {
    return ivec2( a.x() != b.x() ? 1 : 0,
                  a.y() != b.y() ? 1 : 0 );
  }

  // .............................................

  inline ivec4 notEqual(const uvec4& a, const uvec4& b) {
    return ivec4( a.x() != b.x() ? 1 : 0,
                  a.y() != b.y() ? 1 : 0,
                  a.z() != b.z() ? 1 : 0,
                  a.w() != b.w() ? 1 : 0 );
  }

  inline ivec3 notEqual(const uvec3& a, const uvec3& b) {
    return ivec3( a.x() != b.x() ? 1 : 0,
                  a.y() != b.y() ? 1 : 0,
                  a.z() != b.z() ? 1 : 0 );
  }

  inline ivec2 notEqual(const uvec2& a, const uvec2& b) {
    return ivec2( a.x() != b.x() ? 1 : 0,
                  a.y() != b.y() ? 1 : 0 );
  }

  // --------------- any ---------------

  inline bool any(const ivec2& a) { return a.x() != 0 || a.y() != 0; }
  inline bool any(const ivec3& a) { return a.x() != 0 || a.y() != 0 || a.z() != 0; }
  inline bool any(const ivec4& a) { return a.x() != 0 || a.y() != 0 || a.z() != 0 || a.w() != 0; }

  // --------------- all ---------------

  inline bool all(const ivec2& a) { return a.x() != 0 && a.y() != 0; }
  inline bool all(const ivec3& a) { return a.x() != 0 && a.y() != 0 && a.z() != 0; }
  inline bool all(const ivec4& a) { return a.x() != 0 && a.y() != 0 && a.z() != 0 && a.w() != 0; }

  // --------------- not ---------------

#if defined(_MSC_VER)
  inline ivec2 not(const ivec2& a) { return ivec2( a.x() != 0 ? 0 : 1, a.y() != 0 ? 0 : 1); }
  inline ivec3 not(const ivec3& a) { return ivec3( a.x() != 0 ? 0 : 1, a.y() != 0 ? 0 : 1, a.z() != 0 ? 0 : 1); }
  inline ivec4 not(const ivec4& a) { return ivec4( a.x() != 0 ? 0 : 1, a.y() != 0 ? 0 : 1, a.z() != 0 ? 0 : 1, a.w() != 0 ? 0 : 1 ); }
#endif
}

#endif
