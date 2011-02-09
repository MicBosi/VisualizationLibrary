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

#ifndef Sphere_INCLUDE_ONCE
#define Sphere_INCLUDE_ONCE

#include <vlCore/AABB.hpp>

namespace vl
{
//-----------------------------------------------------------------------------
// Sphere
//-----------------------------------------------------------------------------
  /**
   * The Sphere class defines a sphere using a center and a radius using vl::Real precision.
  */
  class Sphere
  {
  public:
    //! Constructor.
    Sphere(): mRadius(-1) { }

    //! Constructor.
    Sphere(const vec3& center, Real radius): mCenter(center), mRadius(radius) {}

    //! Constructor.
    Sphere(const AABB& aabb) { *this = aabb; }

    void setNull()  { mRadius =-1.0f; mCenter = vec3(0,0,0); }
    void setPoint() { mRadius = 0.0f; /*mCenter = vec3(0,0,0);*/ }
    bool isNull()  const { return mRadius <  0.0f; }
    bool isPoint() const { return mRadius == 0.0f; }
    void setCenter(const vec3& center) { mCenter = center; }
    const vec3& center() const { return mCenter; }
    void setRadius( Real radius ) { mRadius = radius; }
    Real radius() const { return mRadius; }

    bool includes(const Sphere& other) const
    {
      if (isNull())
        return false;
      else
      if (other.isNull())
        return true;
      else
      {
        Real distance = (center() - other.center()).length();
        return radius() >= distance + other.radius();
      }
    }

    bool operator==(const Sphere& other) const 
    {
      return mCenter == other.mCenter && mRadius == other.mRadius;
    }

    bool operator!=(const Sphere& other) const
    {
      return !operator==(other);
    }
    
    Sphere& operator=(const AABB& aabb)
    {
      if (aabb.isNull())
        setNull();
      else
      {
        // center the sphere on the aabb center
        mCenter = aabb.center();
        // half of the maximum diagonal
        mRadius = (aabb.minCorner() - aabb.maxCorner()).length() / (Real)2.0;
      }
      return *this;
    }

    Sphere operator+(const Sphere& other)
    {
      Sphere t = *this;
      return t += other;
    }

    const Sphere& operator+=(const Sphere& other)
    {
      if (this->isNull())
        *this = other;
      else
      if (other.includes(*this))
      {
        *this = other;
      }
      else
      if (!other.isNull() && !this->includes(other) )
      {
        vec3 v = other.center() - this->center();
        if (v.isNull())
        {
          // the center remains the same
          // sets the maximum radius
          setRadius( radius() > other.radius() ? radius() : other.radius() );
        }
        else
        {
          v.normalize();
          vec3 p0 = this->center() - v * this->radius();
          vec3 p1 = other.center() + v * other.radius();
          setCenter( (p0 + p1)*(Real)0.5 );
          setRadius( (p0 - p1).length()*(Real)0.5 );
        }
      }

      return *this;
    }

    void transformed(Sphere& sphere, const mat4& mat) const 
    {
      sphere.setNull();
      if ( !isNull() )
      {
        sphere.mCenter = mat * center();
        // vec3 p = center() + vec3( (Real)0.577350269189625840, (Real)0.577350269189625840, (Real)0.577350269189625840 ) * radius();
        // p = mat * p;
        // p = p - sphere.center();
        // sphere.setRadius(p.length());
        vec3 p0 = center() + vec3(radius(),0,0);
        vec3 p1 = center() + vec3(0,radius(),0);
        vec3 p2 = center() + vec3(0,0,radius());
        p0 = mat * p0;
        p1 = mat * p1;
        p2 = mat * p2;
        Real d0 = (p0 - sphere.mCenter).lengthSquared();
        Real d1 = (p1 - sphere.mCenter).lengthSquared();
        Real d2 = (p2 - sphere.mCenter).lengthSquared();
        sphere.mRadius = d0>d1 ? (d0>d2?d0:d2) : (d1>d2?d1:d2);
        sphere.mRadius = ::sqrt(sphere.mRadius);
      }
    }

    Sphere transformed(const mat4& mat) const 
    {
      Sphere sphere;
      transformed(sphere, mat);
      return sphere;
    }

  protected:
    vec3 mCenter;
    Real mRadius;
  };
}

#endif
