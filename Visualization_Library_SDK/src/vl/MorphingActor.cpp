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

#include <vl/MorphingActor.hpp>

using namespace vl;

//-----------------------------------------------------------------------------
// MorphingCallback
//-----------------------------------------------------------------------------
void MorphingCallback::operator()(const Camera*, Actor* actor, Renderable*, const Shader* shader, int pass)
{
  // perform only on the first pass
  if (pass>0)
    return;

  MorphingActor* morph_act = dynamic_cast<MorphingActor*>(actor);

  VL_CHECK(morph_act)

  if (!morph_act->mAnimationStarted)
    return;

  VL_CHECK(morph_act->mFrame1 != -1)
  VL_CHECK(morph_act->mLastUpdate != -1)
  if (morph_act->mLastUpdate == -1 || morph_act->mFrame1 == -1)
    return;

  const GLSLProgram* glslprogram = shader->glslProgram();

  if (!glslprogram)
    return;

  // from here you can change uniforms or query uniform binding location

  if ( morph_act->glslVertexBlendEnabled() )
  {
    // vertex/normals frame 1
    morph_act->mGeometry->setVertexArray( morph_act->mVertexFrames[morph_act->mFrame1].get() );
    morph_act->mGeometry->setNormalArray(   morph_act->mNormalFrames[morph_act->mFrame1].get() );

    #if 1 // faster method:

      // vertex attrib and uniform animation
      if (morph_act->mVertex2_Binding == -1)
        morph_act->mVertex2_Binding = glslprogram->getAttribLocation("vertex2");

      if (morph_act->mNormal2_Binding == -1)
        morph_act->mNormal2_Binding = glslprogram->getAttribLocation("normal2");

      if (morph_act->mAnim_t_Binding  == -1)
        morph_act->mAnim_t_Binding = glslprogram->getUniformLocation("anim_t");

      // vertex/normals frame 2
      morph_act->mGeometry->setVertexAttributeArray( morph_act->mVertex2_Binding, false, false, morph_act->mVertexFrames[morph_act->mFrame2].get() );
      morph_act->mGeometry->setVertexAttributeArray( morph_act->mNormal2_Binding, false, false, morph_act->mNormalFrames[morph_act->mFrame2].get() );
      // frame interpolation ratio
      glUniform1fv(morph_act->mAnim_t_Binding, 1, &morph_act->mAnim_t);

    #else // slower but simpler method:

      // vertex/normals frame 2
      morph_act->mGeometry->setVertexAttributeArray( glslprogram->getAttribLocation("vertex2"), false, false, morph_act->mVertexFrames[morph_act->mFrame2].get() );
      morph_act->mGeometry->setVertexAttributeArray( glslprogram->getAttribLocation("normal2"), false, false, morph_act->mNormalFrames[morph_act->mFrame2].get() );
      // frame interpolation ratio
      glUniform1fv(glslprogram->getUniformLocation("anim_t"), 1, &morph_act->mAnim_t);
    #endif
  }
}
//-----------------------------------------------------------------------------
// MorphingActor
//-----------------------------------------------------------------------------
MorphingActor::MorphingActor()
{
  #ifndef NDEBUG
    mName = "MorphingActor";
  #endif

  renderingCallbacks()->push_back( new MorphingCallback );

  setAnimation(0,0,0);
  resetGLSLBindings();
  setGLSLVertexBlendEnabled(false);

  mAnim_t = 0.0f;
  mFrame1 = -1;
  mFrame2 = -1;
  mLastUpdate = -1;
}
//-----------------------------------------------------------------------------
void MorphingActor::init(ResourceDatabase* res_db)
{
  if (res_db->count<Geometry>() == 0)
    return;

  Geometry* geometry = res_db->get<Geometry>(0);
  mGeometry = geometry->shallowCopy();
  mVertices = new ArrayFVec3;
  mNormals  = new ArrayFVec3;
  lod(0)    = mGeometry.get();

  // setup Geometry vertex attributes

  // copy vertex frames

  for(unsigned i=0; i<res_db->count<ArrayAbstract>(); ++i)
  {
    ArrayFVec3* buffer = dynamic_cast<ArrayFVec3*>(res_db->get<ArrayAbstract>(i));
    if (buffer && buffer->name() == "vertex_frame")
      mVertexFrames.push_back(buffer);
    else
    if (buffer && buffer->name() == "normal_frame")
      mNormalFrames.push_back(buffer);
  }

  if (mVertexFrames.empty())
  {
    Log::error("MorphingActor::init(): no ArrayFVec3 named 'vertex_frame' found.\n");
    return;
  }

  if (mNormalFrames.empty())
  {
    Log::error("MorphingActor::init(): no ArrayFVec3 named 'normal_frame' found.\n");
    return;
  }

  if (mVertexFrames.size() != mNormalFrames.size())
  {
    Log::error("MorphingActor::init(): vertex frame count differs from normal frame count.\n");
    return;
  }

  // compute AABB using the first frame

  mGeometry->setVertexArray(mVertexFrames[0].get() );
  mGeometry->setNormalArray(mNormalFrames[0].get() );
  mGeometry->computeBounds();

  mGeometry->setVertexArray(mVertices.get());
  mGeometry->setNormalArray(mNormals.get());
}
//-----------------------------------------------------------------------------
void MorphingActor::blendFrames(int a, int b, float t)
{
  // allocate interpolation buffers
  if (mVertices->size() != mVertexFrames[0]->size() ||
      mNormals->size()  != mNormalFrames[0]->size() )
  {
    mVertices->resize( mVertexFrames[0]->size() );
    mNormals->resize(  mNormalFrames[0]->size() );

    //// create VBOs
    //if (mGeometry->vboEnabled())
    //{
    //  mVertices->gpuBuffer()->setBufferData(GBU_DYNAMIC_DRAW, false);
    //  mNormals ->gpuBuffer()->setBufferData(GBU_DYNAMIC_DRAW, false);
    //}
  }

  #if 1
    float Ha = 1-t;
    float Hb = t;
  #else
    float Ha = 2*t*t*t - 3*t*t + 1;
    float Hb = -2*t*t*t + 3*t*t;
  #endif

  for(size_t i=0; i<mVertices->size(); ++i)
  {
    mVertices->at(i) = mVertexFrames[ a ]->at(i)*Ha + mVertexFrames[ b ]->at(i)*Hb;
    mNormals->at(i)  = mNormalFrames[ a ]->at(i)*Ha + mNormalFrames[ b ]->at(i)*Hb;
  }

  if (mGeometry->vboEnabled() && (GLEW_VERSION_1_5||GLEW_ARB_vertex_buffer_object))
  {
    mVertices->gpuBuffer()->setBufferData(GBU_DYNAMIC_DRAW, false);
    mNormals ->gpuBuffer()->setBufferData(GBU_DYNAMIC_DRAW, false);
  }
}
//-----------------------------------------------------------------------------
void MorphingActor::update(int /*lod*/, Camera*, Real cur_time)
{
  if (!mAnimationStarted)
    return;

  mElapsedTime = cur_time - mAnimationStartTime;
  // 30 fps update using the CPU vertex blending or continuous update if using the GPU
  bool do_update = mLastUpdate == -1 || (mElapsedTime - mLastUpdate) > 1.0f/30.0f || glslVertexBlendEnabled();
  if ( do_update )
  {
    mLastUpdate = mElapsedTime;
    Real ft = mElapsedTime / mAnimationPeriod;
    ft = ft - (int)ft;

    int frame_count = mAnimationEnd - mAnimationStart + 1;
    mAnim_t  = (float)(ft * frame_count - (int)(ft * frame_count));
    mFrame1 = (int)(ft * frame_count);
    mFrame2 = (mFrame1 + 1) % frame_count;
    mFrame1 += mAnimationStart;
    mFrame2 += mAnimationStart;
    VL_CHECK(mFrame1 >= 0)
    VL_CHECK(mLastUpdate>=0)
    if (!glslVertexBlendEnabled())
      blendFrames(mFrame1, mFrame2, mAnim_t);
  }
}
//-----------------------------------------------------------------------------
void MorphingActor::setAnimation(int start, int end, float period)
{
  mFrame1 = -1;
  mFrame2 = -1;
  mLastUpdate = -1;
  mElapsedTime = 0;
  mAnimationStartTime = 0;
  mAnimationStart   = start;
  mAnimationEnd     = end;
  mAnimationPeriod  = period;
  mAnimationStarted = false;
}
//-----------------------------------------------------------------------------
void MorphingActor::startAnimation(Real start_time)
{
  mAnimationStarted = true;
  mFrame1 = -1;
  mFrame2 = -1;
  mLastUpdate = -1;
  mElapsedTime = 0;
  mAnimationStartTime = start_time;
}
//-----------------------------------------------------------------------------
void MorphingActor::stopAnimation()
{
  mAnimationStarted = false;
}
//-----------------------------------------------------------------------------
void MorphingActor::initFrom(MorphingActor* morph_act)
{
  mVertices = new ArrayFVec3;
  mNormals  = new ArrayFVec3;

  // copy vertex frames

  mVertexFrames = morph_act->mVertexFrames;
  mNormalFrames = morph_act->mNormalFrames;

  #if 0
    // Geometry sharing method: works only wiht GLSL

    // we can have a single shared Geometry since our MorphingCallback setups the
    // appropriate position and normal arrays for every Actor just before the rendering!
    mGeometry = morph_act->mGeometry;
  #else
    // Geometry copy method
    mGeometry = morph_act->mGeometry->shallowCopy();

    // compute AABB using the first frame

    mGeometry->setVertexArray(morph_act->mVertexFrames[0].get() );
    mGeometry->setNormalArray(morph_act->mNormalFrames[0].get() );
    mGeometry->computeBounds();

    mGeometry->setVertexArray(mVertices.get());
    mGeometry->setNormalArray(mNormals.get());
  #endif

  lod(0) = mGeometry.get();
  setAnimation(0,0,0);
}
//-----------------------------------------------------------------------------
void MorphingActor::resetGLSLBindings()
{
  mVertex2_Binding = -1;
  mNormal2_Binding = -1;
  mAnim_t_Binding  = -1;
}
//-----------------------------------------------------------------------------
