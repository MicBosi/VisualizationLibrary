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

#include <vl/ClipPlane.hpp>

using namespace vl;

//-----------------------------------------------------------------------------
ClipPlane::ClipPlane(int plane_index, Real o, vec3 n)
{ 
  #ifndef NDEBUG
    mObjectName = className();
  #endif
  mPlaneIndex = plane_index;
  mPlane.setNormal(n); 
  mPlane.setOrigin(o); 
}
//-----------------------------------------------------------------------------
ClipPlane::ClipPlane(int plane_index, const vec3& o, const vec3& n)
{
  #ifndef NDEBUG
    mObjectName = className();
  #endif
  mPlaneIndex = plane_index;
  mPlane.setNormal(n); 
  mPlane.setOrigin(dot(o, n)); 
}
//-----------------------------------------------------------------------------
void ClipPlane::apply(const Camera* camera, OpenGLContext*) const
{
  VL_CHECK(planeIndex()>=0 && planeIndex()<6);
  
  // we do our own transformations

  if (camera)
  {
    glEnable(GL_CLIP_PLANE0 + planeIndex());

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
  else
  {
    glDisable(GL_CLIP_PLANE0 + planeIndex());
  }
}
//-----------------------------------------------------------------------------
