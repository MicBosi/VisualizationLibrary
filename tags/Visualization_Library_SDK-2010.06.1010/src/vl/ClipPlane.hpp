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

#ifndef ClipPlane_INCLUDE_ONCE
#define ClipPlane_INCLUDE_ONCE

#include <vl/Plane.hpp>
#include <vl/Camera.hpp>
#include <vl/RenderState.hpp>

namespace vl
{
  //-----------------------------------------------------------------------------
  // ClipPlane
  //-----------------------------------------------------------------------------
  /**
   * Wraps the OpenGL function glClipPlane().
   *
   * \sa Shader, Effect, Actor
  */
  class ClipPlane: public RenderState
  {
  public:
    ClipPlane(int plane_index, Real o=0.0f, vec3 n=vec3(0,0,0))
    { 
      #ifndef NDEBUG
        mName = "ClipPlane";
      #endif
      mPlaneIndex = plane_index;
      mPlane.setNormal(n); 
      mPlane.setOrigin(o); 
    }

    ClipPlane(int plane_index, const vec3& o, const vec3& n)
    {
      #ifndef NDEBUG
        mName = "ClipPlane";
      #endif
      mPlaneIndex = plane_index;
      mPlane.setNormal(n); 
      mPlane.setOrigin(dot(o, n)); 
    }

    virtual const char* className() { return "ClipPlane"; }

    virtual void disable() const { VL_CHECK(planeIndex()>=0 && planeIndex()<6); glDisable(GL_CLIP_PLANE0 + planeIndex()); }
    virtual void enable () const { VL_CHECK(planeIndex()>=0 && planeIndex()<6); glEnable (GL_CLIP_PLANE0 + planeIndex()); }

    virtual ERenderState type() const { return (ERenderState)(RS_ClipPlane0 + planeIndex()); }
    virtual void apply(const Camera* camera) const
    // void applyClipPlane(const mat4& mat, int index = -1)
    {
      VL_CHECK(planeIndex()>=0 && planeIndex()<6); 
      // we do our own transformations

      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();

      glLoadIdentity();

      mat4 mat;
      if ( followedTransform() )
        mat = camera->viewMatrix() * followedTransform()->worldMatrix();

      vec3 pt1 = mPlane.normal() * mPlane.origin();
        vec3 pt2 = mPlane.normal() * mPlane.origin() + mPlane.normal();

      pt1 = mat * pt1;
      pt2 = mat * pt2;

      vec3 n = pt2 - pt1;
      Real orig = dot(n, pt1);

      double equation[] = 
      {
        (double)n.x(),
        (double)n.y(),
        (double)n.z(),
        -(double)orig
      };

      glClipPlane(GL_CLIP_PLANE0 + planeIndex(), equation);

      glPopMatrix();
    }

    void followTransform(Transform* transform) { mFollowedTransform = transform; }

    Transform* followedTransform() { return mFollowedTransform.get(); }
    const Transform* followedTransform() const { return mFollowedTransform.get(); }

    void setPlaneIndex(int plane_index) { mPlaneIndex = plane_index; }
    int planeIndex() const { return mPlaneIndex; }

    const Plane& plane() const { return mPlane; }
    void setPlane(const Plane& plane) { mPlane = plane; }

  protected:
    int mPlaneIndex;
    ref<Transform> mFollowedTransform;
    Plane mPlane;
  };
  //-----------------------------------------------------------------------------
}

#endif
