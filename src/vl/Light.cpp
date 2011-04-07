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

#include <vl/Light.hpp>
#include <vl/Transform.hpp>
#include <vl/Camera.hpp>
#include <vl/Log.hpp>
#include <vl/Say.hpp>

using namespace vl;

//------------------------------------------------------------------------------
// Light
//------------------------------------------------------------------------------
Light::Light(int light_index)
{
  #ifndef NDEBUG
    mObjectName = className();
  #endif
  if (light_index<0 && light_index>7)
    Log::error( Say("Light index %n out of range. The light index must be between 0 and 7.\n") << light_index );
  VL_CHECK(light_index>=0 && light_index<8) 
  mLightIndex = light_index;
  mAmbient = fvec4(0,0,0,1);
  mDiffuse = fvec4(1,1,1,1);
  mSpecular = fvec4(1,1,1,1);
  mPosition = fvec4(0,0,0,1);
  mSpotDirection = fvec3(0,0,-1);
  mSpotExponent = 0;
  mSpotCutoff = 180.0f;
  mConstantAttenuation   = 1.0f;
  mLinearAttenuation    = 0.0f;
  mQuadraticAttenuation = 0.0f;
  mFollowedTransform = NULL;
}
//------------------------------------------------------------------------------
void Light::setLightIndex(int light_index)
{ 
  if (light_index<0 && light_index>7)
    Log::error( Say("Light index %n out of range. The light index must be between 0 and 7.\n") << light_index );
  VL_CHECK(light_index>=0 && light_index<8) 
  mLightIndex = light_index; 
}
//------------------------------------------------------------------------------
void Light::apply(const Camera* camera, OpenGLContext* ctx) const
{
  if (camera)
  {
    glEnable (GL_LIGHT0 + lightIndex());

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    // follows the given node
    if ( followedTransform() )
      camera->applyModelViewMatrix( followedTransform()->worldMatrix() );
    else
    {
      // follows the camera
      /*glMatrixMode(GL_MODELVIEW);*/
      glLoadIdentity();
    }

    glLightfv(GL_LIGHT0+lightIndex(), GL_AMBIENT,  mAmbient.ptr());
    glLightfv(GL_LIGHT0+lightIndex(), GL_DIFFUSE,  mDiffuse.ptr());
    glLightfv(GL_LIGHT0+lightIndex(), GL_SPECULAR, mSpecular.ptr());
    glLightfv(GL_LIGHT0+lightIndex(), GL_POSITION, mPosition.ptr());

    glLightf(GL_LIGHT0+lightIndex(), GL_SPOT_CUTOFF, mSpotCutoff);

    // if its a spot light
    if (mSpotCutoff != 180.0f) 
    {
      VL_CHECK(mSpotCutoff>=0.0f && mSpotCutoff<=90.0f);
      glLightfv(GL_LIGHT0+lightIndex(), GL_SPOT_DIRECTION, mSpotDirection.ptr());
      glLightf(GL_LIGHT0+lightIndex(), GL_SPOT_EXPONENT, mSpotExponent);
    }

    // if positional or spot light compute the attenuation factors, that is
    // attenuation is useless of directional lights.
    if (mSpotCutoff != 180.0f || mPosition.w() != 0)
    {
      glLightf(GL_LIGHT0+lightIndex(), GL_CONSTANT_ATTENUATION, mConstantAttenuation);
      glLightf(GL_LIGHT0+lightIndex(), GL_LINEAR_ATTENUATION, mLinearAttenuation);
      glLightf(GL_LIGHT0+lightIndex(), GL_QUADRATIC_ATTENUATION, mQuadraticAttenuation);
    }

    /*glMatrixMode(GL_MODELVIEW);*/
    glPopMatrix();
  }
  else
  {
    glDisable(GL_LIGHT0 + lightIndex());
  }
}
//------------------------------------------------------------------------------
void Light::followTransform(Transform* transform) 
{ 
  mFollowedTransform = transform; 
}
//------------------------------------------------------------------------------
Transform* Light::followedTransform()
{ 
  return mFollowedTransform.get(); 
}
//------------------------------------------------------------------------------
const Transform* Light::followedTransform() const
{ 
  return mFollowedTransform.get(); 
}
//------------------------------------------------------------------------------
