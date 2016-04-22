/**************************************************************************************/
/*                                                                                    */
/*  Visualization Library                                                             */
/*  http://visualizationlibrary.org                                                   */
/*                                                                                    */
/*  Copyright (c) 2005-2011, Michele Bosi                                             */
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

// This software contains source code provided by NVIDIA Corporation.

#include <vlCore/GlobalSettings.hpp>
#include <vlGraphics/RendererVivid.hpp>
#include <vlGraphics/OpenGLContext.hpp>
#include <vlGraphics/GLSL.hpp>
#include <vlGraphics/RenderQueue.hpp>
#include <vlCore/Log.hpp>

#define SHADER_PATH "/vivid/glsl/"

#define MAX_DEPTH 1.0

namespace 
{
  const GLenum gDrawBuffers[] = {
    GL_COLOR_ATTACHMENT0,
    GL_COLOR_ATTACHMENT1,
    GL_COLOR_ATTACHMENT2,
    GL_COLOR_ATTACHMENT3,
    GL_COLOR_ATTACHMENT4,
    GL_COLOR_ATTACHMENT5,
    GL_COLOR_ATTACHMENT6
  };

  // MIC FIXME: remove these
  vl::ref<vl::Uniform> g_uniformAlpha = new vl::Uniform("Alpha");
  float g_Alpha = 0.6f;

  vl::ref<vl::Uniform> g_uniformBackgroundColor = new vl::Uniform("BackgroundColor");
  float g_white[3] = { 1.0, 1.0, 1.0 };
  float g_black[3] = { 0.0 };
  float *g_backgroundColor = g_white;
}

using namespace vl;

//------------------------------------------------------------------------------
// RendererVivid
//------------------------------------------------------------------------------
RendererVivid::RendererVivid()
{
  mRenderingMode = DualDepthPeeling;

  mNumPasses = 4;
  mUseQueryObject = true;
  mQueryID = 0;

  mImageSize = ivec2(0, 0);
  mPassCounter = 0;

  ref<ArrayFloat2> vert2 = new ArrayFloat2;
  mFullScreenQuad = new Geometry();
  mFullScreenQuad->setVertexArray(vert2.get());
  vert2->resize(4);
  const GLfloat vertices[] = { 
    0.0f, 0.0f, 
    1.0f, 0.0f, 
    1.0f, 1.0f, 
    0.0f, 1.0f,
  };
  memcpy(vert2->ptr(), vertices, vert2->bytesUsed());
  ref<DrawArrays> polys = new DrawArrays(vl::PT_QUADS, 0, 4);
  mFullScreenQuad->drawCalls().push_back( polys.get() );
}
//------------------------------------------------------------------------------
void RendererVivid::lazyInitialize()
{
  ivec2 fb_size = ivec2(framebuffer()->width(), framebuffer()->height());
  if (mImageSize == ivec2(0, 0)) 
  {
    mImageSize = fb_size;
    initDualPeelingRenderTargets();
    initFrontPeelingRenderTargets();
    initShaders();
    vl::glGenQueries(1, &mQueryID);
  } 
  else if (mImageSize != fb_size) 
  {
    mImageSize = fb_size;
	  deleteDualPeelingRenderTargets();
	  initDualPeelingRenderTargets();

	  deleteFrontPeelingRenderTargets();
	  initFrontPeelingRenderTargets();
  }
}
void RendererVivid::initShaders()
{
  printf("\nLoading shaders...\n");

  mShaderDualInit = new vl::GLSLProgram();
  mShaderDualPeel = new vl::GLSLProgram();
  mShaderDualBlend = new vl::GLSLProgram();
  mShaderDualFinal = new vl::GLSLProgram();
  mShaderFrontInit = new vl::GLSLProgram();
  mShaderFrontPeel = new vl::GLSLProgram();
  mShaderFrontBlend = new vl::GLSLProgram();
  mShaderFrontFinal = new vl::GLSLProgram();

  mShaderDualInit->attachShader( new vl::GLSLVertexShader( SHADER_PATH "dual_peeling_init_vertex.glsl" ) );
  mShaderDualInit->attachShader( new vl::GLSLFragmentShader(SHADER_PATH "dual_peeling_init_fragment.glsl" ) );
  mShaderDualInit->linkProgram();

  // mShaderDualPeel->attachShader( new vl::GLSLVertexShader( SHADER_PATH "shade_vertex.glsl" ) );
  mShaderDualPeel->attachShader( new vl::GLSLVertexShader( SHADER_PATH "dual_peeling_peel_vertex_ppdl.glsl" ) );
  mShaderDualPeel->attachShader( new vl::GLSLFragmentShader(SHADER_PATH "shade_fragment_ppdl.glsl" ) );
  mShaderDualPeel->attachShader( new vl::GLSLFragmentShader(SHADER_PATH "dual_peeling_peel_fragment_ppdl.glsl" ) );
  mShaderDualPeel->linkProgram();

  mShaderDualBlend->attachShader( new vl::GLSLVertexShader( SHADER_PATH "dual_peeling_blend_vertex.glsl" ) );
  mShaderDualBlend->attachShader( new vl::GLSLFragmentShader(SHADER_PATH "dual_peeling_blend_fragment.glsl" ) );
  mShaderDualBlend->linkProgram();

  mShaderDualFinal->attachShader( new vl::GLSLVertexShader( SHADER_PATH "dual_peeling_final_vertex.glsl" ) );
  mShaderDualFinal->attachShader( new vl::GLSLFragmentShader(SHADER_PATH "dual_peeling_final_fragment.glsl" ) );
  mShaderDualFinal->linkProgram();

  mShaderFrontInit->attachShader( new vl::GLSLVertexShader( SHADER_PATH "shade_vertex.glsl" ) );
  mShaderFrontInit->attachShader( new vl::GLSLVertexShader( SHADER_PATH "front_peeling_init_vertex.glsl" ) );
  mShaderFrontInit->attachShader( new vl::GLSLFragmentShader(SHADER_PATH "shade_fragment.glsl" ) );
  mShaderFrontInit->attachShader( new vl::GLSLFragmentShader(SHADER_PATH "front_peeling_init_fragment.glsl" ) );
  mShaderFrontInit->linkProgram();

  mShaderFrontPeel->attachShader( new vl::GLSLVertexShader( SHADER_PATH "shade_vertex.glsl" ) );
  mShaderFrontPeel->attachShader( new vl::GLSLVertexShader( SHADER_PATH "front_peeling_peel_vertex.glsl" ) );
  mShaderFrontPeel->attachShader( new vl::GLSLFragmentShader(SHADER_PATH "shade_fragment.glsl" ) );
  mShaderFrontPeel->attachShader( new vl::GLSLFragmentShader(SHADER_PATH "front_peeling_peel_fragment.glsl" ) );
  mShaderFrontPeel->linkProgram();

  mShaderFrontBlend->attachShader( new vl::GLSLVertexShader( SHADER_PATH "front_peeling_blend_vertex.glsl" ) );
  mShaderFrontBlend->attachShader( new vl::GLSLFragmentShader(SHADER_PATH "front_peeling_blend_fragment.glsl" ) );
  mShaderFrontBlend->linkProgram();

  mShaderFrontFinal->attachShader( new vl::GLSLVertexShader( SHADER_PATH "front_peeling_final_vertex.glsl" ) );
  mShaderFrontFinal->attachShader( new vl::GLSLFragmentShader(SHADER_PATH "front_peeling_final_fragment.glsl" ) );
  mShaderFrontFinal->linkProgram();

  printf("Shaders %s\n\n", this->shadersReady() ? "ready." : "error.");
}
//-----------------------------------------------------------------------------
void RendererVivid::initDualPeelingRenderTargets()
{
  glGenTextures(2, mDualDepthTexId);
  glGenTextures(2, mDualFrontBlenderTexId);
  glGenTextures(2, mDualBackTempTexId);
  vl::glGenFramebuffers(1, &mDualPeelingSingleFboId);
  for (int i = 0; i < 2; i++)
  {
	  glBindTexture(GL_TEXTURE_RECTANGLE, mDualDepthTexId[i]);
	  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP);
	  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP);
	  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	  glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RG32F, mImageSize.x(), mImageSize.y(), 0, GL_RGB, GL_FLOAT, 0);

	  glBindTexture(GL_TEXTURE_RECTANGLE, mDualFrontBlenderTexId[i]);
	  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP);
	  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP);
	  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	  glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA, mImageSize.x(), mImageSize.y(), 0, GL_RGBA, GL_FLOAT, 0);

	  glBindTexture(GL_TEXTURE_RECTANGLE, mDualBackTempTexId[i]);
	  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP);
	  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP);
	  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	  glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA, mImageSize.x(), mImageSize.y(), 0, GL_RGBA, GL_FLOAT, 0);
  }

  glGenTextures(1, &mDualBackBlenderTexId);
  glBindTexture(GL_TEXTURE_RECTANGLE, mDualBackBlenderTexId);
  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGB, mImageSize.x(), mImageSize.y(), 0, GL_RGB, GL_FLOAT, 0);

  vl::glGenFramebuffers(1, &mDualBackBlenderFboId);
  vl::glBindFramebuffer(GL_FRAMEBUFFER, mDualBackBlenderFboId);
  vl::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, mDualBackBlenderTexId, 0);

  vl::glBindFramebuffer(GL_FRAMEBUFFER, mDualPeelingSingleFboId);

  int j = 0;
	  vl::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, mDualDepthTexId[j], 0);
	  vl::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_RECTANGLE, mDualFrontBlenderTexId[j], 0);
	  vl::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_RECTANGLE, mDualBackTempTexId[j], 0);

  j = 1;
	  vl::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_RECTANGLE, mDualDepthTexId[j], 0);
	  vl::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_RECTANGLE, mDualFrontBlenderTexId[j], 0);
	  vl::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, GL_TEXTURE_RECTANGLE, mDualBackTempTexId[j], 0);

  vl::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, GL_TEXTURE_RECTANGLE, mDualBackBlenderTexId, 0);

  // Cleanup
  vl::glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_RECTANGLE, 0);

  VL_CHECK_OGL();
}

//--------------------------------------------------------------------------
void RendererVivid::deleteDualPeelingRenderTargets()
{
  vl::glDeleteFramebuffers(1, &mDualBackBlenderFboId);
  vl::glDeleteFramebuffers(1, &mDualPeelingSingleFboId);
  glDeleteTextures(2, mDualDepthTexId);
  glDeleteTextures(2, mDualFrontBlenderTexId);
  glDeleteTextures(2, mDualBackTempTexId);
  glDeleteTextures(1, &mDualBackBlenderTexId);
}

//--------------------------------------------------------------------------
void RendererVivid::initFrontPeelingRenderTargets()
{
  glGenTextures(2, mFrontDepthTexId);
  glGenTextures(2, mFrontColorTexId);
  vl::glGenFramebuffers(2, mFrontFboId);

  for (int i = 0; i < 2; i++)
  {
	  glBindTexture(GL_TEXTURE_RECTANGLE, mFrontDepthTexId[i]);
	  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP);
	  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP);
	  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	  glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_DEPTH_COMPONENT32F, mImageSize.x(), mImageSize.y(), 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	  glBindTexture(GL_TEXTURE_RECTANGLE, mFrontColorTexId[i]);
	  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP);
	  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP);
	  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	  glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA, mImageSize.x(), mImageSize.y(), 0, GL_RGBA, GL_FLOAT, 0);

	  vl::glBindFramebuffer(GL_FRAMEBUFFER, mFrontFboId[i]);
	  vl::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_RECTANGLE, mFrontDepthTexId[i], 0);
	  vl::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, mFrontColorTexId[i], 0);
  }

  glGenTextures(1, &mFrontColorBlenderTexId);
  glBindTexture(GL_TEXTURE_RECTANGLE, mFrontColorBlenderTexId);
  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA, mImageSize.x(), mImageSize.y(), 0, GL_RGBA, GL_FLOAT, 0);

  vl::glGenFramebuffers(1, &mFrontColorBlenderFboId);
  vl::glBindFramebuffer(GL_FRAMEBUFFER, mFrontColorBlenderFboId);
  vl::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_RECTANGLE, mFrontDepthTexId[0], 0);
  vl::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, mFrontColorBlenderTexId, 0);

  // Cleanup
  glBindTexture(GL_TEXTURE_RECTANGLE, 0);

  VL_CHECK_OGL();
}
//-------------------------------------------------------------------------------------------------
void RendererVivid::deleteFrontPeelingRenderTargets()
{
  vl::glDeleteFramebuffers(2, mFrontFboId);
  vl::glDeleteFramebuffers(1, &mFrontColorBlenderFboId);
  glDeleteTextures(2, mFrontDepthTexId);
  glDeleteTextures(2, mFrontColorTexId);
  glDeleteTextures(1, &mFrontColorBlenderTexId);
}
//-------------------------------------------------------------------------------------------------
bool RendererVivid::shadersReady() const {
  return mShaderDualInit->linked() &&
         mShaderDualPeel->linked() &&
         mShaderDualBlend->linked() &&
         mShaderDualFinal->linked() &&
         mShaderFrontInit->linked() &&
         mShaderFrontPeel->linked() &&
         mShaderFrontBlend->linked() &&
         mShaderFrontFinal->linked();
}
//-------------------------------------------------------------------------------------------------
void RendererVivid::drawFullScreenQuad()
{
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadMatrixf( vl::mat4::getOrtho2D( 0.0, 1.0, 0.0, 1.0 ).ptr() );
  mFullScreenQuad->render( NULL, NULL, NULL, framebuffer()->openglContext() );
  glPopMatrix();
}
//-------------------------------------------------------------------------------------------------
void RendererVivid::bindTexture(vl::GLSLProgram* glsl, GLenum target, std::string texname, GLuint texid, int texunit)
{
  vl::glActiveTexture(GL_TEXTURE0 + texunit);
	glBindTexture(target, texid);
	vl::glActiveTexture(GL_TEXTURE0);

  int current_glsl_program = -1;
  glGetIntegerv(GL_CURRENT_PROGRAM, &current_glsl_program); VL_CHECK_OGL();
  VL_CHECK(current_glsl_program == (int)glsl->handle())

	// setTextureUnit(texname, texunit);
  int location = glsl->getUniformLocation(texname.c_str());
	if (location == -1) {
    printf("Warning: Invalid texture %s\n", texname.c_str());
		return;
	}
  vl::glUniform1i(location, texunit);
}
//-------------------------------------------------------------------------------------------------
void RendererVivid::renderDualPeeling(const RenderQueue* render_queue, Camera* camera, real frame_clock)
{
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);

  // ---------------------------------------------------------------------
  // 1. Initialize Min-Max Depth Buffer
  // ---------------------------------------------------------------------

  vl::glBindFramebuffer(GL_FRAMEBUFFER, mDualPeelingSingleFboId);

  // Render targets 1 and 2 store the front and back colors
  // Clear to 0.0 and use MAX blending to filter written color
  // At most one front color and one back color can be written every pass
  vl::glDrawBuffers(2, &gDrawBuffers[1]);
  glClearColor(0, 0, 0, 0);
  glClear(GL_COLOR_BUFFER_BIT);

  // Render target 0 stores (-minDepth, maxDepth, alphaMultiplier)
  glDrawBuffer(gDrawBuffers[0]);
  glClearColor(-MAX_DEPTH, -MAX_DEPTH, 0, 0);
  glClear(GL_COLOR_BUFFER_BIT);
  vl::glBlendEquation(GL_MAX);

  mShaderDualInit->useProgram();
  mShaderDualInit->applyUniformSet();
  
  renderQueue(render_queue, camera, frame_clock); 

  vl::glUseProgram(0);

  VL_CHECK_OGL();

  // ---------------------------------------------------------------------
  // 2. Dual Depth Peeling + Blending
  // ---------------------------------------------------------------------

  // Since we cannot blend the back colors in the geometry passes,
  // we use another render target to do the alpha blending
  //vl::glBindFramebuffer(GL_FRAMEBUFFER, mDualBackBlenderFboId);
  glDrawBuffer(gDrawBuffers[6]);
  glClearColor(g_backgroundColor[0], g_backgroundColor[1], g_backgroundColor[2], 0);
  glClear(GL_COLOR_BUFFER_BIT);

  int currId = 0;

  for (int pass = 1; mUseQueryObject || pass < mNumPasses; pass++) {
	  currId = pass % 2;
	  int prevId = 1 - currId;
	  int bufId = currId * 3;
		
	  //vl::glBindFramebuffer(GL_FRAMEBUFFER, mDualPeelingFboId[currId]);
      
	  vl::glDrawBuffers(2, &gDrawBuffers[bufId+1]);
	  glClearColor(0, 0, 0, 0);
	  glClear(GL_COLOR_BUFFER_BIT);

	  glDrawBuffer(gDrawBuffers[bufId+0]);
	  glClearColor(-MAX_DEPTH, -MAX_DEPTH, 0, 0);
	  glClear(GL_COLOR_BUFFER_BIT);

	  // Render target 0: RG32F MAX blending
	  // Render target 1: RGBA MAX blending
	  // Render target 2: RGBA MAX blending
	  vl::glDrawBuffers(3, &gDrawBuffers[bufId+0]);
	  vl::glBlendEquation(GL_MAX);

	  mShaderDualPeel->useProgram();
	  bindTexture(mShaderDualPeel.get(), GL_TEXTURE_RECTANGLE, "DepthBlenderTex", mDualDepthTexId[prevId], 0);
	  bindTexture(mShaderDualPeel.get(), GL_TEXTURE_RECTANGLE, "FrontBlenderTex", mDualFrontBlenderTexId[prevId], 1);
	    
    // mShaderDualPeel.setUniform("Alpha", (float*)&g_Alpha, 1);
    mShaderDualPeel->setUniform(g_uniformAlpha.get());
    g_uniformAlpha->setUniform(1, (float*)&g_Alpha);
    mShaderDualPeel->applyUniformSet();
    
    renderQueue(render_queue, camera, frame_clock); 
	  
    vl::glUseProgram(0);

	  VL_CHECK_OGL();

	  // Full screen pass to alpha-blend the back color
	  glDrawBuffer(gDrawBuffers[6]);

	  vl::glBlendEquation(GL_FUNC_ADD);
	  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	  if (mUseQueryObject) {
      vl::glBeginQuery(GL_SAMPLES_PASSED_ARB, mQueryID);
	  }

	  mShaderDualBlend->useProgram();
	  bindTexture(mShaderDualBlend.get(), GL_TEXTURE_RECTANGLE, "TempTex", mDualBackTempTexId[currId], 0);
    mShaderDualBlend->applyUniformSet();
	  drawFullScreenQuad();
	  vl::glUseProgram(0);

	  VL_CHECK_OGL();

	  if (mUseQueryObject) {
		  vl::glEndQuery(GL_SAMPLES_PASSED_ARB);
		  GLuint sample_count;
		  vl::glGetQueryObjectuiv(mQueryID, GL_QUERY_RESULT_ARB, &sample_count);
		  if (sample_count == 0) {
			  break;
		  }
	  }
  }

  glDisable(GL_BLEND);

  // ---------------------------------------------------------------------
  // 3. Final Pass
  // ---------------------------------------------------------------------

  vl::glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glDrawBuffer(GL_BACK);

  mShaderDualFinal->useProgram();
  // bindTexture(mShaderDualFinal.get(), GL_TEXTURE_RECTANGLE, "DepthBlenderTex", mDualDepthTexId[currId], 0);
  bindTexture(mShaderDualFinal.get(), GL_TEXTURE_RECTANGLE, "FrontBlenderTex", mDualFrontBlenderTexId[currId], 1);
  bindTexture(mShaderDualFinal.get(), GL_TEXTURE_RECTANGLE, "BackBlenderTex", mDualBackBlenderTexId, 2);
  mShaderDualFinal->applyUniformSet();
  drawFullScreenQuad();
  vl::glUseProgram(0);

  // Cleanup
  vl::glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_RECTANGLE, 0);
  vl::glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_RECTANGLE, 0);
  vl::glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_RECTANGLE, 0);
  vl::glActiveTexture(GL_TEXTURE0);

  VL_CHECK_OGL();
}
//-------------------------------------------------------------------------------------------------
void RendererVivid::renderFrontToBackPeeling(const RenderQueue* render_queue, Camera* camera, real frame_clock)
{
  // ---------------------------------------------------------------------
  // 1. Initialize Min Depth Buffer
  // ---------------------------------------------------------------------

  vl::glBindFramebuffer(GL_FRAMEBUFFER, mFrontColorBlenderFboId);
  glDrawBuffer(gDrawBuffers[0]);

  glClearColor(0, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glEnable(GL_DEPTH_TEST);

  mShaderFrontInit->useProgram();
  // mShaderFrontInit.setUniform("Alpha", (float*)&g_Alpha, 1);
  mShaderFrontInit->setUniform(g_uniformAlpha.get());
  g_uniformAlpha->setUniform(1, (float*)&g_Alpha);    
  mShaderFrontInit->applyUniformSet();
  
  renderQueue(render_queue, camera, frame_clock); 

  vl::glUseProgram(0);

  VL_CHECK_OGL();

  // ---------------------------------------------------------------------
  // 2. Depth Peeling + Blending
  // ---------------------------------------------------------------------

  int numLayers = (mNumPasses - 1) * 2;
  for (int layer = 1; mUseQueryObject || layer < numLayers; layer++) {
	  int currId = layer % 2;
	  int prevId = 1 - currId;

	  vl::glBindFramebuffer(GL_FRAMEBUFFER, mFrontFboId[currId]);
	  glDrawBuffer(gDrawBuffers[0]);

	  glClearColor(0, 0, 0, 0);
	  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	  glDisable(GL_BLEND);
	  glEnable(GL_DEPTH_TEST);

	  if (mUseQueryObject) {
		  vl::glBeginQuery(GL_SAMPLES_PASSED_ARB, mQueryID);
	  }

	  mShaderFrontPeel->useProgram();
	  bindTexture(mShaderFrontPeel.get(), GL_TEXTURE_RECTANGLE, "DepthTex", mFrontDepthTexId[prevId], 0);
    // mShaderFrontPeel.setUniform("Alpha", (float*)&g_Alpha, 1);
    mShaderFrontPeel->setUniform(g_uniformAlpha.get());
    g_uniformAlpha->setUniform(1, (float*)&g_Alpha);    
	  mShaderFrontPeel->applyUniformSet();
    
    renderQueue(render_queue, camera, frame_clock); 

	  vl::glUseProgram(0);

	  if (mUseQueryObject) {
		  vl::glEndQuery(GL_SAMPLES_PASSED_ARB);
	  }

	  VL_CHECK_OGL();

	  vl::glBindFramebuffer(GL_FRAMEBUFFER, mFrontColorBlenderFboId);
	  glDrawBuffer(gDrawBuffers[0]);

	  glDisable(GL_DEPTH_TEST);
	  glEnable(GL_BLEND);

	  vl::glBlendEquation(GL_FUNC_ADD);
	  vl::glBlendFuncSeparate(GL_DST_ALPHA, GL_ONE, GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);
		
	  mShaderFrontBlend->useProgram();
	  bindTexture(mShaderFrontBlend.get(), GL_TEXTURE_RECTANGLE, "TempTex", mFrontColorTexId[currId], 0);
    mShaderFrontBlend->applyUniformSet();
	  drawFullScreenQuad();
	  vl::glUseProgram(0);

	  glDisable(GL_BLEND);

	  VL_CHECK_OGL();

	  if (mUseQueryObject) {
		  GLuint sample_count;
		  vl::glGetQueryObjectuiv(mQueryID, GL_QUERY_RESULT_ARB, &sample_count);
		  if (sample_count == 0) {
			  break;
		  }
	  }
  }

  // ---------------------------------------------------------------------
  // 3. Final Pass
  // ---------------------------------------------------------------------

  vl::glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glDrawBuffer(GL_BACK);
  glDisable(GL_DEPTH_TEST);

  mShaderFrontFinal->useProgram();

  // mShaderFrontFinal.setUniform("BackgroundColor", g_backgroundColor, 3);
  mShaderFrontFinal->setUniform(g_uniformBackgroundColor.get());
  g_uniformBackgroundColor->setUniform3f(1, g_backgroundColor);

  bindTexture(mShaderFrontFinal.get(), GL_TEXTURE_RECTANGLE, "ColorTex", mFrontColorBlenderTexId, 0);
  mShaderFrontFinal->applyUniformSet();
  drawFullScreenQuad();
  vl::glUseProgram(0);

  // cleanup
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  vl::glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_RECTANGLE, 0);

  VL_CHECK_OGL();
}
//-------------------------------------------------------------------------------------------------
void RendererVivid::renderQueue(const RenderQueue* render_queue, Camera* camera, real frame_clock, bool depth_peeling_on) {
  
  mPassCounter++;

  OpenGLContext* opengl_context = framebuffer()->openglContext();

  for(int itok=0; itok < render_queue->size(); ++itok)
  {
    const RenderToken* tok = render_queue->at(itok); VL_CHECK(tok);
    Actor* actor = tok->mActor; VL_CHECK(actor);

    if ( !isEnabled(actor->enableMask()) )
      continue;

    VL_CHECK_OGL()

    // --------------- shader setup ---------------

    const Shader* shader = tok->mShader;

    shader->getMaterial()->apply(0, NULL, NULL); VL_CHECK_OGL()
    shader->getLight(0)->apply(0, camera, NULL); VL_CHECK_OGL()
    shader->getLightModel()->apply(0, NULL, NULL);

    /* These two states are managed by the Depth Peeling algorithm */
    if (!depth_peeling_on) {
      shader->isEnabled(vl::EN_BLEND) ? glEnable(GL_BLEND) : glDisable(GL_BLEND); VL_CHECK_OGL()
      glDepthMask(shader->isEnabled(vl::EN_BLEND) ? GL_FALSE : GL_TRUE);
      shader->isEnabled(vl::EN_BLEND) ? glEnable(GL_BLEND) : glDisable(GL_BLEND); VL_CHECK_OGL()
      shader->isEnabled(vl::EN_DEPTH_TEST) ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST); VL_CHECK_OGL()
      shader->isEnabled(vl::EN_CULL_FACE) ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE); VL_CHECK_OGL()
      shader->isEnabled(vl::EN_LIGHTING) ? glEnable(GL_LIGHTING) : glDisable(GL_LIGHTING); VL_CHECK_OGL()
    }

    // --------------- Actor pre-render callback ---------------

    // here the user has still the possibility to modify the Actor's uniforms

    actor->dispatchOnActorRenderStarted( frame_clock, camera, tok->mRenderable, shader, /*ipass*/0 );

    VL_CHECK_OGL()

    // --------------- Transform setup ---------------

    // Update camera/actor model-view transform
    projViewTransfCallback()->updateMatrices( /*update_cm*/false, /*update_tr*/true, 0, camera, actor->transform() );

    VL_CHECK_OGL()

    // --------------- Actor rendering ---------------

    // also compiles display lists and updates BufferObjects if necessary
    tok->mRenderable->render( actor, shader, camera, opengl_context );

    VL_CHECK_OGL()
  }
}
//-------------------------------------------------------------------------------------------------
const RenderQueue* RendererVivid::render(const RenderQueue* render_queue, Camera* camera, real frame_clock)
{
  VL_CHECK_OGL()

  lazyInitialize();
  mPassCounter = 0;

  if (!shadersReady()) {
    return NULL;
  }

  // skip if renderer is disabled

  if (enableMask() == 0)
    return render_queue;

  // enter/exit behavior contract

  class InOutContract 
  {
    RendererVivid* mRenderer;
    std::vector<RenderStateSlot> mOriginalDefaultRS;
  public:
    InOutContract(RendererVivid* renderer, Camera* camera): mRenderer(renderer)
    {
      // increment the render tick.
      mRenderer->mRenderTick++;

      // render-target activation.
      // note: an OpenGL context can have multiple rendering targets!
      mRenderer->framebuffer()->activate();

      // viewport setup.
      camera->viewport()->setClearFlags( mRenderer->clearFlags() );
      camera->viewport()->activate();

      OpenGLContext* gl_context = renderer->framebuffer()->openglContext();

      // default render states override
      for(size_t i=0; i<renderer->overriddenDefaultRenderStates().size(); ++i)
      {
        // save overridden default render state to be restored later
        ERenderState type = renderer->overriddenDefaultRenderStates()[i].type();
        mOriginalDefaultRS.push_back(gl_context->defaultRenderState(type));
        // set new default render state
        gl_context->setDefaultRenderState(renderer->overriddenDefaultRenderStates()[i]);
      }

      // dispatch the renderer-started event.
      mRenderer->dispatchOnRendererStarted();

      // check user-generated errors.
      VL_CHECK_OGL()
    }

    ~InOutContract()
    {
      // dispatch the renderer-finished event
      mRenderer->dispatchOnRendererFinished();

      OpenGLContext* gl_context = mRenderer->framebuffer()->openglContext();

      // restore default render states
      for(size_t i=0; i<mOriginalDefaultRS.size(); ++i)
      {
        gl_context->setDefaultRenderState(mOriginalDefaultRS[i]);
      }

      VL_CHECK( !globalSettings()->checkOpenGLStates() || mRenderer->framebuffer()->openglContext()->isCleanState(true) );

      // check user-generated errors.
      VL_CHECK_OGL()

      // note: we don't reset the render target here
    }
  } contract(this, camera);


  OpenGLContext* opengl_context = framebuffer()->openglContext();

  // --------------- rendering ---------------

  // Update camera projection
  projViewTransfCallback()->updateMatrices( /*update_cm*/true, /*update_tr*/false, 0, camera, /*cur_transform*/NULL );

  // Disable scissor test
  glDisable(GL_SCISSOR_TEST);
  glDisable(GL_CULL_FACE);
  glDisable(GL_LIGHTING);
  glDisable(GL_NORMALIZE);
  glDepthMask(GL_TRUE);

  switch (mRenderingMode)
  {
  case FastRender: 
    renderQueue(render_queue, camera, frame_clock, false);
    break;
  case DualDepthPeeling: 
    renderDualPeeling(render_queue, camera, frame_clock);
    break;
  case FrontToBackDepthPeeling: 
    renderFrontToBackPeeling(render_queue, camera, frame_clock);
    break;
  }

  glDepthMask(GL_TRUE);
  glDisable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glDisable(GL_LIGHTING);
  glDisable(GL_LIGHT0);

  // clear enables
  // opengl_context->applyEnables( mDummyEnables.get() ); VL_CHECK_OGL();

  // clear render states
  opengl_context->applyRenderStates( mDummyStateSet.get(), NULL ); VL_CHECK_OGL();

  // enabled texture unit #0
  VL_glActiveTexture( GL_TEXTURE0 ); VL_CHECK_OGL();
  if (Has_Fixed_Function_Pipeline)
    VL_glClientActiveTexture( GL_TEXTURE0 ); VL_CHECK_OGL();

  // disable scissor test
  glDisable(GL_SCISSOR_TEST); VL_CHECK_OGL();

  // disable all vertex arrays, note this also calls "glBindBuffer(GL_ARRAY_BUFFER, 0)"
  opengl_context->bindVAS(NULL, false, false); VL_CHECK_OGL();

  return render_queue;
}
//-------------------------------------------------------------------------------------------------
