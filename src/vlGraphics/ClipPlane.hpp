/**************************************************************************************/
/*                                                                                    */
/*  Visualization Library                                                             */
/*  http://www.visualizationlibrary.org                                               */
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

#include <vlCore/Plane.hpp>
#include <vlGraphics/Camera.hpp>
#include <vlGraphics/RenderState.hpp>

namespace vl
{
  //-----------------------------------------------------------------------------
  // ClipPlane
  //-----------------------------------------------------------------------------
  /**
   * Wraps the OpenGL function glClipPlane(). See also http://www.opengl.org/sdk/docs/man/xhtml/glClipPlane.xml
   *
   * \sa Shader, Effect, Actor
  */
  class VLGRAPHICS_EXPORT ClipPlane: public RenderState
  {
    VL_INSTRUMENT_CLASS(vl::ClipPlane, RenderState)

  public:
    /** Constructor. */
    ClipPlane(int plane_index, Real o=0.0f, vec3 n=vec3(0,0,0));
    /** Constructor. */
    ClipPlane(int plane_index, const vec3& o, const vec3& n);
    /** Returns RS_ClipPlane0 + planeIndex() */
    virtual ERenderState type() const { return (ERenderState)(RS_ClipPlane0 + planeIndex()); }
    /** Applies the light render states. */
    virtual void apply(const Camera* camera, OpenGLContext*) const;
    /** Attach the light to a vl::Transform. */
    void followTransform(Transform* transform) { mFollowedTransform = transform; }
    /** Returns the vl::Transform to which the Light is attached. */
    Transform* followedTransform() { return mFollowedTransform.get(); }
    /** Returns the vl::Transform to which the Light is attached. */
    const Transform* followedTransform() const { return mFollowedTransform.get(); }
    /** Defines the index of the plane this ClipPlane applies.
      * OpenGL supports up to 6 custom clip planes at the same time thus the index must be
      * a number between 0 and 5. */
    void setPlaneIndex(int plane_index) { mPlaneIndex = plane_index; }
    /** Returns the index of the plane this ClipPlane applies.
      * OpenGL supports up to 6 custom clip planes at the same time thus the index will be
      * a number between 0 and 5. */
    int planeIndex() const { return mPlaneIndex; }
    /** Returns the actual plane used to perform the clipping. */
    const Plane& plane() const { return mPlane; }
    /** Defines the actual plane used to perform the clipping. */
    void setPlane(const Plane& plane) { mPlane = plane; }

  protected:
    int mPlaneIndex;
    ref<Transform> mFollowedTransform;
    Plane mPlane;
  };
  //-----------------------------------------------------------------------------
}

#endif
