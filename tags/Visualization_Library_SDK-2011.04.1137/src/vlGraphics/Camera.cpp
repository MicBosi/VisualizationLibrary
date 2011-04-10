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

#include <vlGraphics/Camera.hpp>
#include <vlGraphics/OpenGL.hpp>
#include <vlCore/AABB.hpp>
#include <vlCore/Log.hpp>
#include <vlCore/Say.hpp>

#undef near
#undef far

using namespace vl;

//-----------------------------------------------------------------------------
// Camera
//-----------------------------------------------------------------------------
Camera::Camera()
{
  #ifndef NDEBUG
    mObjectName = className();
  #endif
  mFrustum.planes().resize(6);
  mFOV = 60.0;
  mNearPlane = (Real)0.05;
  mFarPlane  = (Real)10000.0;
  mViewport = new Viewport;

  mProjectionMatrix  = mat4::getPerspective(fov(), 640.0f/480.0f, nearPlane(), farPlane());
}
//-----------------------------------------------------------------------------
void Camera::applyModelViewMatrix(const mat4& model_matrix) const
{
  /* some OpenGL drivers (ATI) require this instead of the more general (and mathematically correct) viewMatrix() */
  mat4 viewm = viewMatrix();
  viewm.e(3,0) = 0.0;
  viewm.e(3,1) = 0.0;
  viewm.e(3,2) = 0.0;
  viewm.e(3,3) = 1.0;

  glMatrixMode(GL_MODELVIEW);
#if 0
  VL_glLoadMatrix( viewm.ptr() );
  VL_glMultMatrix( matrix.ptr() );
#elif 0
  viewm = viewm * matrix;
  VL_glLoadMatrix( viewm.ptr() );
#else
  VL_glLoadMatrix( (viewm * model_matrix).ptr() );
#endif
}
//-----------------------------------------------------------------------------
void Camera::applyProjMatrix() const
{
  // projection matrix
  glMatrixMode( GL_PROJECTION );
  VL_glLoadMatrix( projectionMatrix().ptr() );
}
//-----------------------------------------------------------------------------
void Camera::applyViewMatrix() const
{
  /* some OpenGL drivers (ATI) require this instead of the more general (and mathematically correct) viewMatrix() */
  mat4 viewm = viewMatrix();
  viewm.e(3,0) = 0.0;
  viewm.e(3,1) = 0.0;
  viewm.e(3,2) = 0.0;
  viewm.e(3,3) = 1.0;
  glMatrixMode(GL_MODELVIEW);
  VL_glLoadMatrix( viewm.ptr() );
}
//-----------------------------------------------------------------------------
void Camera::computeNearFarOptimizedProjMatrix(const Sphere& scene_bounding_sphere)
{
  // near/far clipping planes optimization
  if (!scene_bounding_sphere.isNull())
  {
    // compute the sphere in camera coordinates
    Sphere camera_sphere;
    scene_bounding_sphere.transformed(camera_sphere, viewMatrix());
    mNearPlane = -(camera_sphere.center().z() + camera_sphere.radius() * (Real)1.01);
    mFarPlane  = -(camera_sphere.center().z() - camera_sphere.radius() * (Real)1.01);
    #if 0
      far  = max(far,  (Real)1.0e-5);
      near = max(near, (Real)1.0e-6);
    #else
      // prevents z-thrashing when very large objects are zoomed a lot
      Real ratio = camera_sphere.radius() * (Real)2.01 / (Real)2000.0;
      mNearPlane = max(mNearPlane, ratio*1);
      mFarPlane  = max(mFarPlane,  ratio*2);
    #endif
    // supports only perspective projection matrices
    setProjectionAsPerspective();
  }
}
//-----------------------------------------------------------------------------
void Camera::adjustView(const AABB& aabb, const vec3& dir, const vec3& up, Real bias)
{
  VL_CHECK(bias >= 0)
  if (bias < 0)
    vl::Log::bug("Camera::adjustView(): 'bias' must be >= 0.\n");

  vec3 center = aabb.center();

  Sphere sphere(aabb);
  const vec3& C = inverseViewMatrix().getT();
  const vec3& V = -inverseViewMatrix().getZ();
  const Real  R = sphere.radius();

  // extract the frustum planes based on the current view and projection matrices
  mat4 viewproj = projectionMatrix() * viewMatrix();
  Frustum frustum; frustum.planes().resize(6);
  extractPlanes( &frustum.planes()[0], viewproj );
  // iterate over left/right/top/bottom clipping planes. the planes are in world coords.
  Real max_t = 0;
  for(int i=0; i<4; ++i)
  {
    const vec3& O = frustum.plane(i).origin() * frustum.plane(i).normal();
    const vec3& N = frustum.plane(i).normal();
    Real t = - (R + dot(O,N) - dot(C,N)) / dot(N,V);
    VL_CHECK(t>=0)
    if (t > max_t)
      max_t = t;
  }
  Real dist = max_t;
  mat4 m = mat4::getLookAt(center+dir*dist*bias,center,up);
  setInverseViewMatrix(m);
}
//-----------------------------------------------------------------------------
void Camera::computeFrustumPlanes()
{
  // build modelview matrix
  mat4 viewproj = projectionMatrix() * viewMatrix();
  // frustum plane extraction
  mFrustum.planes().resize(6);
  extractPlanes( &mFrustum.planes()[0], viewproj );
}
//-----------------------------------------------------------------------------
void Camera::setProjectionAsFrustum(Real left, Real right, Real bottom, Real top, Real near, Real far)
{
  setFOV(-1);
  setNearPlane(near);
  setFarPlane(far);
  mProjectionMatrix = mat4::getFrustum(left, right, bottom, top, near, far);
}
//-----------------------------------------------------------------------------
void Camera::setProjectionAsPerspective(Real fov, Real near, Real far)
{
  setFOV(fov);
  setNearPlane(near);
  setFarPlane(far);
  mProjectionMatrix = mat4::getPerspective(fov, aspectRatio(), near, far);
}
//-----------------------------------------------------------------------------
void Camera::setProjectionAsPerspective()
{
  mProjectionMatrix = mat4::getPerspective(fov(), aspectRatio(), nearPlane(), farPlane());
}
//-----------------------------------------------------------------------------
void Camera::setProjectionAsOrtho(Real offset)
{
  setProjectionMatrix( 
    mat4::getOrtho(
      offset, (Real)mViewport->width()  + offset,
      offset, (Real)mViewport->height() + offset,
      nearPlane(), farPlane())
  );
}
//-----------------------------------------------------------------------------
void Camera::setProjectionAsOrtho2D(Real offset)
{
  setProjectionMatrix( 
    mat4::getOrtho(
      offset, (Real)mViewport->width()  + offset,
      offset, (Real)mViewport->height() + offset,
      -1, +1)
  );
}
//-----------------------------------------------------------------------------
void Camera::setViewMatrixAsLookAt( const vec3& eye, const vec3& center, const vec3& up)
{
  mat4 m = mat4::getLookAt(eye, center, up);
  // this sets both the frame matrix and the view matrix
  setInverseViewMatrix(m);
}
//-----------------------------------------------------------------------------
void Camera::getViewMatrixAsLookAt( vec3& eye, vec3& look, vec3& up, vec3& right) const
{
  mInverseViewMatrix.getAsLookAt(eye,look,up,right);
}
//-----------------------------------------------------------------------------
bool Camera::project(const vec4& in, vec4& out) const
{
  out = mProjectionMatrix * mViewMatrix * in;

  if (out.w() == 0.0f)
    return false;

  out.x() /= out.w();
  out.y() /= out.w();
  out.z() /= out.w();

  // map to range 0-1
  out.x() = out.x() * 0.5f + 0.5f;
  out.y() = out.y() * 0.5f + 0.5f;
  out.z() = out.z() * 0.5f + 0.5f;

  // map to viewport
  out.x() = out.x() * mViewport->width()  + mViewport->x();
  out.y() = out.y() * mViewport->height() + mViewport->y();
  return true;
}
//-----------------------------------------------------------------------------
bool Camera::unproject(const vec3& win, vec4& out) const
{
    vec4 v;
    v.x() = win.x();
    v.y() = win.y();
    v.z() = win.z();
    v.w() = 1.0;

    // map from viewport to 0-1
    v.x() = (v.x() - mViewport->x()) / mViewport->width();
    v.y() = (v.y() - mViewport->y()) / mViewport->height();

    // map to range -1 to 1
    v.x() = v.x() * 2.0f - 1.0f;
    v.y() = v.y() * 2.0f - 1.0f;
    v.z() = v.z() * 2.0f - 1.0f;

    Real det=0;
    mat4 inverse = (mProjectionMatrix * mViewMatrix).getInverse(&det); // mic fixme all inverse here
    if (!det)
      return false;

    v = inverse * v;
    if (v.w() == 0.0)
      return false;

    out = v / v.w();
    return true;
}
//-----------------------------------------------------------------------------
bool Camera::unproject(std::vector<vec3>& win) const
{
  Real det=0;
  mat4 inverse = (mProjectionMatrix * mViewMatrix).getInverse(&det);
  if (!det)
    return false;

  bool ok = true;
  for(unsigned i=0; i<win.size(); ++i)
  {
    vec4 v;
    v = vec4( win[i], 1.0 );

    // map from viewport to 0-1
    v.x() = (v.x() - mViewport->x()) / mViewport->width();
    v.y() = (v.y() - mViewport->y()) / mViewport->height();

    // map to range -1 to 1
    v.x() = v.x() * 2.0f - 1.0f;
    v.y() = v.y() * 2.0f - 1.0f;
    v.z() = v.z() * 2.0f - 1.0f;

    v = inverse * v;
    if (v.w() == 0.0)
    {
      ok = false;
      continue;
    }

    v = v / v.w();
    win[i] = v.xyz();
  }
  return ok;
}
//-----------------------------------------------------------------------------
Ray Camera::computeRay(int winx, int winy)
{
  vl::vec4 out;
  if (!unproject( vl::vec3((Real)winx,(Real)winy,0), out ))
    return Ray();
  else
  {
    vl::Ray ray;
    ray.setOrigin(out.xyz());
    ray.setDirection( (out.xyz() - inverseViewMatrix().getT()).normalize() );
    return ray;
  }
}
//-----------------------------------------------------------------------------
Frustum Camera::computeRayFrustum(int winx, int winy)
{
  /*
      n3
    D-----C
    |     |
  n4|  O  |n2
    |     |
    A-----B
      n1
  */
  // compute the frustum passing through the adjacent pixels
  vl::vec4 A1,B1,C1,D1;
  vl::vec4 A2,B2,C2,D2;
  unproject( vl::vec3((Real)winx-1,(Real)winy-1,0),    A1 );
  unproject( vl::vec3((Real)winx+1,(Real)winy-1,0),    B1 );
  unproject( vl::vec3((Real)winx+1,(Real)winy+1,0),    C1 );
  unproject( vl::vec3((Real)winx-1,(Real)winy+1,0),    D1 );
  unproject( vl::vec3((Real)winx-1,(Real)winy-1,0.1f), A2 );
  unproject( vl::vec3((Real)winx+1,(Real)winy-1,0.1f), B2 );
  unproject( vl::vec3((Real)winx+1,(Real)winy+1,0.1f), C2 );
  unproject( vl::vec3((Real)winx-1,(Real)winy+1,0.1f), D2 );

  vec3 n1 = -cross(A2.xyz()-A1.xyz(),B1.xyz()-A1.xyz());
  vec3 n2 = -cross(B2.xyz()-B1.xyz(),C1.xyz()-B1.xyz());
  vec3 n3 = -cross(C2.xyz()-C1.xyz(),D1.xyz()-C1.xyz());
  vec3 n4 = -cross(D2.xyz()-D1.xyz(),A1.xyz()-D1.xyz());
  Frustum frustum;
  frustum.planes().push_back( Plane( A1.xyz(), n1 ) );
  frustum.planes().push_back( Plane( B1.xyz(), n2 ) );
  frustum.planes().push_back( Plane( C1.xyz(), n3 ) );
  frustum.planes().push_back( Plane( D1.xyz(), n4 ) );
  return frustum;
}
//-----------------------------------------------------------------------------
