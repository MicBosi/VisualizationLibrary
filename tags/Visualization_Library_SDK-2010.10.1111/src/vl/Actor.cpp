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

#include <vl/Actor.hpp>

using namespace vl;

//-----------------------------------------------------------------------------
// Actor
//-----------------------------------------------------------------------------
Actor::~Actor()
{
  deleteOcclusionQuery();
}
//-----------------------------------------------------------------------------
void Actor::setLODs(Renderable* lod0, Renderable* lod1, Renderable* lod2, Renderable* lod3, Renderable* lod4, Renderable* lod5)
{
  if (lod0) { VL_CHECK(0<VL_MAX_ACTOR_LOD) lod(0) = lod0; }
  if (lod1) { VL_CHECK(1<VL_MAX_ACTOR_LOD) lod(1) = lod1; }
  if (lod2) { VL_CHECK(2<VL_MAX_ACTOR_LOD) lod(2) = lod2; }
  if (lod3) { VL_CHECK(3<VL_MAX_ACTOR_LOD) lod(3) = lod3; }
  if (lod4) { VL_CHECK(4<VL_MAX_ACTOR_LOD) lod(4) = lod4; }
  if (lod5) { VL_CHECK(5<VL_MAX_ACTOR_LOD) lod(5) = lod5; }
}
//-----------------------------------------------------------------------------
int Actor::evaluateLOD(Camera* camera)
{
  if (mLODEvaluator)
    return mLODEvaluator->evaluate(this, camera);
  else
    return 0;
}
//-----------------------------------------------------------------------------
void Actor::createOcclusionQuery()
{
  VL_CHECK_OGL();
  if (!mOcclusionQuery && (GLEW_ARB_occlusion_query || GLEW_VERSION_1_5))
    glGenQueries(1, &mOcclusionQuery);
  VL_CHECK_OGL();
  VL_CHECK(mOcclusionQuery)
}
//-----------------------------------------------------------------------------
void Actor::deleteOcclusionQuery()
{
  if(GLEW_ARB_occlusion_query || GLEW_VERSION_1_5)
  {
    if (mOcclusionQuery)
    {
      glDeleteQueries(1, &mOcclusionQuery);
      mOcclusionQuery = 0;
    }
  }
}
//-----------------------------------------------------------------------------
void Actor::computeBounds()
{
  if ( !lod(0) )
    return;

  if ( lod(0)->boundsDirty() )
    lod(0)->computeBounds();

  bool update = lod(0)->boundsUpdateTick() != mBoundsUpdateTick;

  if ( transform() && (update || transform()->worldMatrixUpdateTick() != mTransformUpdateTick) )
  {
    lod(0)->boundingBox().transformed( mAABB, transform()->worldMatrix() );
    mTransformUpdateTick = transform()->worldMatrixUpdateTick();
    mBoundsUpdateTick = lod(0)->boundsUpdateTick();
    mSphere = mAABB.isNull() ? Sphere() : mAABB;
  }
  else
  if (update)
  {
    mAABB   = lod(0)->boundingBox();
    mSphere = mAABB.isNull() ? Sphere() : mAABB;
    mBoundsUpdateTick = lod(0)->boundsUpdateTick();
  }
}
//-----------------------------------------------------------------------------
void Actor::setUniform(Uniform* uniform) 
{ 
  if (!uniformSet()) 
    setUniformSet(new UniformSet); 
  uniformSet()->setUniform(uniform); 
}
//-----------------------------------------------------------------------------
const std::vector< ref<Uniform> >& Actor::uniforms() const { return uniformSet()->uniforms(); }
//-----------------------------------------------------------------------------
void Actor::eraseUniform(const std::string& name) { uniformSet()->eraseUniform(name); }
//-----------------------------------------------------------------------------
void Actor::eraseUniform(const Uniform* uniform) { uniformSet()->eraseUniform(uniform); }
//-----------------------------------------------------------------------------
void Actor::eraseAllUniforms() { uniformSet()->eraseAllUniforms(); }
//-----------------------------------------------------------------------------
Uniform* Actor::gocUniform(const std::string& name) { return uniformSet()->gocUniform(name); }
//-----------------------------------------------------------------------------
Uniform* Actor::getUniform(const std::string& name) { return uniformSet()->getUniform(name); }
//-----------------------------------------------------------------------------
const Uniform* Actor::getUniform(const std::string& name) const { return uniformSet()->getUniform(name); }
//-----------------------------------------------------------------------------
