/*=============================================================================

  vlVivid is part of NifTK: A software platform for medical image computing.

  Copyright (c) University College London (UCL). All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

  See NifTK license for details.

=============================================================================*/

// This software contains source code provided by NVIDIA Corporation.

#include <vlCore/GlobalSettings.hpp>
#include <vlVivid/VividRenderer.hpp>
#include <vlVivid/VividRendering.hpp>
#include <vlGraphics/OpenGLContext.hpp>
#include <vlGraphics/GLSL.hpp>
#include <vlGraphics/RenderQueue.hpp>
#include <vlGraphics/CalibratedCamera.hpp>
#include <vlCore/Log.hpp>
#include <vlCore/Time.hpp>

#define SHADER_PATH "/vivid/glsl/"

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
}

using namespace vl;

//------------------------------------------------------------------------------
// VividRenderer
//------------------------------------------------------------------------------
VividRenderer::VividRenderer(VividRendering* vivid_rendering)
{
  VL_DEBUG_SET_OBJECT_NAME()

  mVividRendering = vivid_rendering;

  setEnableMask( Vivid::VividEnableMask );

  // Depth peeling settings
  mDepthPeelingPasses = 4;
  mUseQueryObject = false;
  mQueryID = 0;

  mImageSize = ivec2(0, 0);
  mPassCounter = 0;

  mFullScreenQuad = new Geometry();
  ref<ArrayFloat2> fsq_vert2 = new ArrayFloat2;
  fsq_vert2->resize(4);
  mFullScreenQuad->setVertexArray( fsq_vert2.get() );
  const GLfloat vertices[] = {
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,
  };
  memcpy( fsq_vert2->ptr(), vertices, fsq_vert2->bytesUsed() );
  mFullScreenQuad->drawCalls().push_back( new DrawArrays( PT_QUADS, 0, 4 ) );

  mBackgroundQuad = new Geometry();
  ref<ArrayFloat2> bgq_vert2 = new ArrayFloat2;
  bgq_vert2->resize( 4 );
  bgq_vert2->setUsage( BU_DYNAMIC_DRAW );
  mBackgroundQuad->setVertexArray( bgq_vert2.get() );
  ref<ArrayFloat2> bgq_tex = new ArrayFloat2;
  bgq_tex->resize( 4 );
  //  1---2 image-top
  //  |   |
  //  0---3 image-bottom
  const GLfloat tex_coords[] = {
    0.0f, 1.0f,
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
  };
  memcpy( bgq_tex->ptr(), tex_coords, bgq_tex->bytesUsed() );
  mBackgroundQuad->setTexCoordArray( 0, bgq_tex.get() );
  mBackgroundQuad->drawCalls().push_back( new DrawArrays( PT_QUADS, 0, 4 ) );

  mStencilFBO = 0;

  mDepthFBOTex = new Texture();
  mStencilTexture = new Texture();
}
VividRenderer::~VividRenderer()
{
  deleteGLBuffers();
}
//------------------------------------------------------------------------------
void VividRenderer::lazyInitialize()
{
  ivec2 fb_size = ivec2( framebuffer()->width(), framebuffer()->height() );
  if (mImageSize == ivec2(0, 0)) {
    // First initialization
    mImageSize = fb_size;
    glGenQueries(1, &mQueryID);
    initGLBuffers();
    initShaders();
  } else
  if ( mImageSize.x() < fb_size.x() || mImageSize.y() < fb_size.y() ) {
    // Resize only if larger
    mImageSize = fb_size;
    // Reallocate all off-screen buffers
    deleteGLBuffers();
    initGLBuffers();
  }
}
// ----------------------------------------------------------------------------
void VividRenderer::initShaders()
{
  //printf("\nLoading shaders...\n");
  //float t0 = Time::currentTime();

  mShaderFrontInit_S = new GLSLProgram();
  //mShaderFrontPeel_S = new GLSLProgram();
  // mShaderOutline3D = new GLSLProgram();
  mShaderOutline2D = new GLSLProgram();
  mShaderFrontInit = new GLSLProgram();
  mShaderFrontPeel = new GLSLProgram();
  mShaderFrontBlend = new GLSLProgram();
  mShaderFrontFinal = new GLSLProgram();

  mShaderOutline2D->attachShader( new GLSLVertexShader( SHADER_PATH "front_peeling/outline2d.vs" ) );
  mShaderOutline2D->attachShader( new GLSLFragmentShader( SHADER_PATH "front_peeling/outline2d.fs" ) );
  mShaderOutline2D->linkProgram();

  //mShaderOutline3D->attachShader( new GLSLVertexShader( SHADER_PATH "front_peeling/shade_outline.vs" ) );
  //mShaderOutline3D->attachShader( new GLSLFragmentShader(SHADER_PATH "front_peeling/outline3d.fs" ) );
  //mShaderOutline3D->attachShader( new GLSLGeometryShader(SHADER_PATH "front_peeling/outline.gs" ) );
  //mShaderOutline3D->linkProgram();

  mShaderFrontInit_S->attachShader( new GLSLVertexShader( SHADER_PATH "front_peeling/shade_outline.vs" ) );
  mShaderFrontInit_S->attachShader( new GLSLFragmentShader(SHADER_PATH "front_peeling/shade.fs" ) );
  mShaderFrontInit_S->attachShader( new GLSLFragmentShader(SHADER_PATH "front_peeling/init.fs" ) );
  mShaderFrontInit_S->attachShader( new GLSLGeometryShader(SHADER_PATH "front_peeling/outline.gs" ) );
  mShaderFrontInit_S->gocUniform("vl_Vivid.outline3DRendering")->setUniformI( 1 );
  mShaderFrontInit_S->linkProgram();

  //mShaderFrontPeel_S->attachShader( new GLSLVertexShader( SHADER_PATH "front_peeling/shade_outline.vs" ) );
  //mShaderFrontPeel_S->attachShader( new GLSLFragmentShader(SHADER_PATH "front_peeling/shade.fs" ) );
  //mShaderFrontPeel_S->attachShader( new GLSLFragmentShader(SHADER_PATH "front_peeling/peel.fs" ) );
  //mShaderFrontPeel_S->attachShader( new GLSLGeometryShader(SHADER_PATH "front_peeling/outline.gs" ) );
  //mShaderFrontPeel_S->gocUniform("vl_Vivid.outline3DRendering")->setUniformI( 1 );
  //mShaderFrontPeel_S->linkProgram();

  mShaderFrontInit->attachShader( new GLSLVertexShader( SHADER_PATH "front_peeling/shade.vs" ) );
  mShaderFrontInit->attachShader( new GLSLFragmentShader(SHADER_PATH "front_peeling/shade.fs" ) );
  mShaderFrontInit->attachShader( new GLSLFragmentShader(SHADER_PATH "front_peeling/init.fs" ) );
  mShaderFrontInit->gocUniform("vl_Vivid.outline3DRendering")->setUniformI( 0 );
  mShaderFrontInit->linkProgram();

  mShaderFrontPeel->attachShader( new GLSLVertexShader( SHADER_PATH "front_peeling/shade.vs" ) );
  mShaderFrontPeel->attachShader( new GLSLFragmentShader(SHADER_PATH "front_peeling/shade.fs" ) );
  mShaderFrontPeel->attachShader( new GLSLFragmentShader(SHADER_PATH "front_peeling/peel.fs" ) );
  mShaderFrontPeel->gocUniform("vl_Vivid.outline3DRendering")->setUniformI( 0 );
  mShaderFrontPeel->linkProgram();

  mShaderFrontBlend->attachShader( new GLSLVertexShader( SHADER_PATH "front_peeling/blend.vs" ) );
  mShaderFrontBlend->attachShader( new GLSLFragmentShader(SHADER_PATH "front_peeling/blend.fs" ) );
  mShaderFrontBlend->linkProgram();

  mShaderFrontFinal->attachShader( new GLSLVertexShader( SHADER_PATH "front_peeling/final.vs" ) );
  mShaderFrontFinal->attachShader( new GLSLFragmentShader(SHADER_PATH "front_peeling/final.fs" ) );
  mShaderFrontFinal->linkProgram();

  // printf( "Shaders %s (%.1fs)\n\n", this->shadersReady() ? "ready." : "error.", Time::currentTime() - t0 );
}
//--------------------------------------------------------------------------
void VividRenderer::initGLBuffers()
{
  if ( ! mBackgroundTexSampler ) {
    // We could use mipmaps generation because the time to generate the mipmap in the GPU seems negligible
    // compared to the time necessary to upload the image.
    // We don't enable mipmaps mainly for simplicity and future compatibility to in-GPU updated textures.
    ref< Texture > texture = new Texture( 32, 32, TF_RGBA, false );
    texture->getTexParameter()->setGenerateMipmap( false );
    texture->getTexParameter()->setMagFilter( vl::TPF_LINEAR );
    texture->getTexParameter()->setMinFilter( vl::TPF_LINEAR );
    texture->getTexParameter()->setWrapS( vl::TPW_CLAMP_TO_EDGE );
    texture->getTexParameter()->setWrapT( vl::TPW_CLAMP_TO_EDGE );
    mBackgroundTexSampler = new TextureSampler;
    mBackgroundTexSampler->setTexture( texture.get() );
  }

  // Depth Peeling

  glGenFramebuffers(2, mFrontFboId);
  glGenTextures(2, mFrontDepthTexId);
  glGenTextures(2, mFrontColorTexId);

  for (int i = 0; i < 2; i++)
  {
    glBindTexture(GL_TEXTURE_RECTANGLE, mFrontDepthTexId[i]);
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_DEPTH_COMPONENT, mImageSize.x(), mImageSize.y(), 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    glBindTexture(GL_TEXTURE_RECTANGLE, mFrontColorTexId[i]);
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA, mImageSize.x(), mImageSize.y(), 0, GL_RGBA, GL_FLOAT, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, mFrontFboId[i]);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_RECTANGLE, mFrontDepthTexId[i], 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, mFrontColorTexId[i], 0);
  }

  glGenTextures(1, &mFrontColorBlenderTexId);
  glBindTexture(GL_TEXTURE_RECTANGLE, mFrontColorBlenderTexId);
  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA, mImageSize.x(), mImageSize.y(), 0, GL_RGBA, GL_FLOAT, 0);

  glGenFramebuffers(1, &mFrontColorBlenderFboId);
  glBindFramebuffer(GL_FRAMEBUFFER, mFrontColorBlenderFboId);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_RECTANGLE, mFrontDepthTexId[0], 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, mFrontColorBlenderTexId, 0);

  // Stencil FBO

  glGenFramebuffers( 1, &mStencilFBO );

  // regenerate new texture
  mStencilTexture->createTextureRectangle( mImageSize.x(), mImageSize.y(), TF_RGBA ); // TF_LUMINANCE not supported by Intel drivers
  glBindTexture( GL_TEXTURE_RECTANGLE, mStencilTexture->handle() );
  glTexParameteri( GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
  glTexParameteri( GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
  glTexParameteri( GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
  glTexParameteri( GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
  glBindTexture( GL_TEXTURE_RECTANGLE, 0 );

  glBindFramebuffer(GL_FRAMEBUFFER, mStencilFBO );
  glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, mStencilTexture->handle(), 0 );
  glBindFramebuffer(GL_FRAMEBUFFER, 0 );

  // Depth FBO for volume rendering

  glGenFramebuffers(1, &mDepthFBO);

  // regenerate new texture
  mDepthFBOTex->createTextureRectangle( mImageSize.x(), mImageSize.y(), TF_DEPTH_COMPONENT );
  mDepthFBOTex->getTexParameter()->setWrap( TPW_CLAMP );
  mDepthFBOTex->getTexParameter()->setMinFilter( TPF_NEAREST );
  mDepthFBOTex->getTexParameter()->setMagFilter( TPF_NEAREST );

  glBindFramebuffer(GL_FRAMEBUFFER, mDepthFBO);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_RECTANGLE, mDepthFBOTex->handle(), 0);

  // Cleanup

  glBindTexture(GL_TEXTURE_RECTANGLE, 0);

  VL_CHECK_OGL();
}
//-------------------------------------------------------------------------------------------------
void VividRenderer::deleteGLBuffers()
{
  // Depth peeling
  glDeleteFramebuffers( 2, mFrontFboId );
  glDeleteFramebuffers( 1, &mFrontColorBlenderFboId );
  glDeleteTextures( 2, mFrontDepthTexId );
  glDeleteTextures( 2, mFrontColorTexId );
  glDeleteTextures( 1, &mFrontColorBlenderTexId );
  // Vivid Stencil
  glDeleteFramebuffers( 1, &mStencilFBO );
  // Vivid Depth
  glDeleteFramebuffers( 1, &mDepthFBO );
}
//-------------------------------------------------------------------------------------------------
bool VividRenderer::shadersReady() const {
  return mShaderFrontInit->linked() &&
         mShaderFrontPeel->linked() &&
         mShaderFrontBlend->linked() &&
         mShaderFrontFinal->linked();
}
//-------------------------------------------------------------------------------------------------
void VividRenderer::drawFullScreenQuad()
{
  int current_glsl_program = -1;
  glGetIntegerv(GL_CURRENT_PROGRAM, &current_glsl_program); VL_CHECK_OGL();
  VL_CHECK( current_glsl_program != -1 );

  int vl_ModelViewProjectionMatrix = glGetUniformLocation(current_glsl_program, "vl_ModelViewProjectionMatrix");
  VL_CHECK( vl_ModelViewProjectionMatrix != -1 );

  mat4 proj = mat4::getOrtho2D( 0.0, 1.0, 0.0, 1.0 );
  glUniformMatrix4fv( vl_ModelViewProjectionMatrix, 1, GL_FALSE, proj.ptr() ); VL_CHECK_OGL();

  mFullScreenQuad->render( NULL, NULL, NULL, framebuffer()->openglContext() );
}
//-------------------------------------------------------------------------------------------------
void VividRenderer::bindTexture( GLSLProgram* glsl, GLenum target, std::string texname, GLuint texid, int texunit )
{
  VL_CHECK( texunit <= Vivid::TextureUnit3 );

  glActiveTexture(GL_TEXTURE0 + texunit);
  glBindTexture(target, texid);
  glActiveTexture(GL_TEXTURE0);

  int current_glsl_program = -1;
  glGetIntegerv(GL_CURRENT_PROGRAM, &current_glsl_program); VL_CHECK_OGL();
  VL_CHECK(current_glsl_program == (int)glsl->handle())

  // setTextureUnit(texname, texunit);
  int location = glsl->getUniformLocation(texname.c_str());
  if (location == -1) {
    printf("Warning: Invalid texture %s\n", texname.c_str());
    return;
  }
  glUniform1i(location, texunit);
}
//-------------------------------------------------------------------------------------------------
void VividRenderer::renderControl(const RenderQueue* render_queue, Camera* camera, real frame_clock) {
  glUseProgram(0);

  vec4 bgcolor = camera->viewport()->clearColor();
  glClearColor( bgcolor[0], bgcolor[1], bgcolor[2], 0 );
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  renderQueue( render_queue, NULL, camera, frame_clock, false );
}
//-------------------------------------------------------------------------------------------------
void VividRenderer::renderStencil(Camera* camera) {
  VL_CHECK_OGL();
  VL_CHECK( mStencilFBO );
  VL_CHECK( glIsEnabled( GL_SCISSOR_TEST ) );

  // Bind Stencil Texture Framebuffer Object

  glBindFramebuffer( GL_FRAMEBUFFER, mStencilFBO ); VL_CHECK_OGL();
  glDrawBuffer( GL_COLOR_ATTACHMENT0 ); VL_CHECK_OGL();

  // Setup Viewport

  // *** viewport has already been setup outside ***

  // Clear color buffer

  glDisable( GL_SCISSOR_TEST );
  glClearColor( 0, 0, 0, 1 ); VL_CHECK_OGL();
  glClear( GL_COLOR_BUFFER_BIT ); VL_CHECK_OGL();

  // Use standard OpenGL pipeline

  glUseProgram(0); VL_CHECK_OGL();

  // Double check main states are clean

  VL_CHECK( ! glIsEnabled( GL_LIGHTING ) );
  VL_CHECK( ! glIsEnabled( GL_DEPTH_TEST ) );
  VL_CHECK( ! glIsEnabled( GL_BLEND ) );
  VL_CHECK( ! glIsEnabled( GL_CULL_FACE ) );

  glColor4f( 1.0f, 1.0f, 1.0f, 1.0f ); VL_CHECK_OGL();

  // Render stencil geometry

  for( size_t i = 0; i < mVividRendering->stencilActors().size(); ++i ) {
    Actor* actor     = mVividRendering->stencilActors()[ i ].get();
    Renderable* geom = actor->lod(0);
    Transform* tr    = actor->transform();
    // Setup legacy matrices
    projViewTransfCallback()->updateMatrices( /*update_cm*/true, /*update_tr*/true, NULL, camera, tr ); VL_CHECK_OGL()
    geom->render( NULL, NULL, camera, framebuffer()->openglContext() ); VL_CHECK_OGL();
  }

  // Blit texture to main framebuffer
  // This is only needed to make the stencil texture visible in StencilRender mode.

  glBindFramebuffer( GL_READ_FRAMEBUFFER, mStencilFBO ); VL_CHECK_OGL();
  glReadBuffer( GL_COLOR_ATTACHMENT0 ); VL_CHECK_OGL();

  glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 ); VL_CHECK_OGL();
  glDrawBuffer( GL_BACK ); VL_CHECK_OGL();

  ivec2 fb_size = ivec2( framebuffer()->width(), framebuffer()->height() );
  ivec2 tx_size = ivec2( framebuffer()->width(), framebuffer()->height() );
  glBlitFramebuffer( 0, 0, tx_size.x()-1, tx_size.y()-1,
                     0, 0, fb_size.x()-1, fb_size.y()-1,
                     GL_COLOR_BUFFER_BIT,
                     GL_LINEAR); VL_CHECK_OGL();

  glBindFramebuffer( GL_FRAMEBUFFER, 0 ); VL_CHECK_OGL();
  glReadBuffer( GL_BACK ); VL_CHECK_OGL();
  glDrawBuffer( GL_BACK ); VL_CHECK_OGL();
  glEnable( GL_SCISSOR_TEST );
}
//-------------------------------------------------------------------------------------------------
void VividRenderer::renderFrontToBackPeeling(const RenderQueue* render_queue, Camera* camera, real frame_clock)
{
  // ---------------------------------------------------------------------
  // 0. Initialize Renderwise Uniforms
  // ---------------------------------------------------------------------

  mShaderOutline2D->gocUniform("vl_Vivid.opacity")->setUniform( mVividRendering->opacity() );

  //mShaderOutline3D->gocUniform("vl_Vivid.outline.eyeOffset")->setUniform( mVividRendering->outline3DEyeOffset() );
  //mShaderOutline3D->gocUniform("vl_Vivid.outline.clipOffset")->setUniform( mVividRendering->outline3DClipOffset() );
  //mShaderOutline3D->gocUniform("vl_Vivid.opacity")->setUniform( mVividRendering->opacity() );

  mShaderFrontInit_S->gocUniform("vl_Vivid.outline.eyeOffset")->setUniform( mVividRendering->outline3DEyeOffset() );
  mShaderFrontInit_S->gocUniform("vl_Vivid.outline.clipOffset")->setUniform( mVividRendering->outline3DClipOffset() );

  //mShaderFrontPeel_S->gocUniform("vl_Vivid.outline.eyeOffset")->setUniform( mOutline3DEyeOffset );
  //mShaderFrontPeel_S->gocUniform("vl_Vivid.outline.clipOffset")->setUniform( mOutline3DClipOffset );

  mShaderFrontFinal->gocUniform("vl_Vivid.opacity")->setUniform( mVividRendering->opacity() );
  mShaderFrontFinal->gocUniform("vl_Vivid.stencil.enabled")->setUniformI( mVividRendering->isStencilEnabled() );
  mShaderFrontFinal->gocUniform("vl_Vivid.stencil.smoothness")->setUniform( mVividRendering->stencilSmoothness() );
  mShaderFrontFinal->gocUniform("vl_Vivid.stencil.backgroundColor")->setUniform( mVividRendering->stencilBackgroundColor() );

  // ---------------------------------------------------------------------
  // 1. Initialize Min Depth Buffer
  // ---------------------------------------------------------------------

  glBindFramebuffer( GL_FRAMEBUFFER, mFrontColorBlenderFboId );
  glDrawBuffer( gDrawBuffers[0] );

  glClearColor( 0, 0, 0, 1 );
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  glDisable( GL_BLEND );
  glEnable( GL_DEPTH_TEST );

  // Fill
  mShaderFrontInit->useProgram();
  mShaderFrontInit->applyUniformSet();
  int translucent_count = renderQueue( render_queue, mShaderFrontInit.get(), camera, frame_clock, true, false );

  // Outline 3D
  mShaderFrontInit_S->useProgram();
  mShaderFrontInit_S->applyUniformSet();
  renderQueue( render_queue, mShaderFrontInit_S.get(), camera, frame_clock, true, true );

  glUseProgram( 0 );

  // Save depth buffer for later

  glBindFramebuffer( GL_DRAW_FRAMEBUFFER, mDepthFBO );
  glDrawBuffer( GL_NONE );
  ivec2 fb_size = ivec2( framebuffer()->width(), framebuffer()->height() );
  glBlitFramebuffer( 0, 0, fb_size.x()-1, fb_size.y()-1,
                     0, 0, fb_size.x()-1, fb_size.y()-1,
                     GL_DEPTH_BUFFER_BIT,
                     GL_NEAREST); VL_CHECK_OGL();

  // restore depth peeling settings
  glBindFramebuffer( GL_FRAMEBUFFER, mFrontColorBlenderFboId );
  glDrawBuffer( gDrawBuffers[0] );

  VL_CHECK_OGL();

  // ---------------------------------------------------------------------
  // 2. Depth Peeling + Blending
  // ---------------------------------------------------------------------

  bool throttle = 0 == translucent_count && mVividRendering->isDepthPeelingAutoThrottleEnabled();
  GLuint samples_passed;
  int numLayers = (mDepthPeelingPasses - 1) * 2;
  for (int layer = 1; ! throttle && ( mUseQueryObject || layer < numLayers); layer++) {
    int currId = layer % 2;
    int prevId = 1 - currId;

    glBindFramebuffer( GL_FRAMEBUFFER, mFrontFboId[currId] );
    glDrawBuffer( gDrawBuffers[0] );

    glClearColor( 0, 0, 0, 0 );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);

    if ( mUseQueryObject ) {
      glBeginQuery( GL_ANY_SAMPLES_PASSED, mQueryID );
    }

    // Fill
    mShaderFrontPeel->useProgram();
    bindTexture( mShaderFrontPeel.get(), GL_TEXTURE_RECTANGLE, "DepthTex", mFrontDepthTexId[prevId], Vivid::TextureUnit0 );
    mShaderFrontPeel->applyUniformSet();
    renderQueue( render_queue, mShaderFrontPeel.get(), camera, frame_clock, true, false );

    /*
    // Outline 3D
    mShaderFrontPeel_S->useProgram();
    bindTexture( mShaderFrontPeel_S.get(), GL_TEXTURE_RECTANGLE, "DepthTex", mFrontDepthTexId[prevId], Vivid::TextureUnit0 );
    mShaderFrontPeel_S->applyUniformSet();
    renderQueue( render_queue, mShaderFrontPeel_S.get(), camera, frame_clock, true, true );
    */

    glUseProgram( 0 );

    if ( mUseQueryObject ) {
      glEndQuery( GL_ANY_SAMPLES_PASSED  );
    }

    VL_CHECK_OGL();

    glBindFramebuffer( GL_FRAMEBUFFER, mFrontColorBlenderFboId );
    glDrawBuffer(gDrawBuffers[0]);

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);

    glBlendEquation(GL_FUNC_ADD);
    glBlendFuncSeparate( GL_DST_ALPHA, GL_ONE, GL_ZERO, GL_ONE_MINUS_SRC_ALPHA );

    mShaderFrontBlend->useProgram();
    bindTexture( mShaderFrontBlend.get(), GL_TEXTURE_RECTANGLE, "TempTex", mFrontColorTexId[currId], Vivid::TextureUnit0 );
    mShaderFrontBlend->applyUniformSet();

    drawFullScreenQuad();

    glUseProgram(0);

    glDisable(GL_BLEND);

    VL_CHECK_OGL();

    if ( mUseQueryObject ) {
      glGetQueryObjectuiv( mQueryID, GL_QUERY_RESULT, &samples_passed );
      if ( samples_passed == 0 ) {
        break;
      }
    }
  }

  // ---------------------------------------------------------------------
  // 3. Final Pass
  // ---------------------------------------------------------------------

  glBindFramebuffer( GL_FRAMEBUFFER, 0 );
  glDrawBuffer( GL_BACK );
  glDisable( GL_DEPTH_TEST );
  glDepthMask( GL_FALSE );

  // clear & render the background
  // scissor disabled: clear entire window, not just viewport and don't clip background against scissor/viewport.
  VL_CHECK( camera->viewport()->isScissorEnabled() );
  glDisable( GL_SCISSOR_TEST );
    vec4 bgc = camera->viewport()->clearColor();
    glClearColor( bgc.r(), bgc.g(), bgc.b(), bgc.a() );
    glClearDepth( camera->viewport()->clearDepth() );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    if ( mVividRendering->backgroundImageEnabled() ) {
      renderBackgroundImage( camera );
    }
  glEnable( GL_SCISSOR_TEST );

  glEnable( GL_BLEND );
  glBlendEquation( GL_FUNC_ADD );
  glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

  mShaderFrontFinal->useProgram();
  bindTexture( mShaderFrontFinal.get(), GL_TEXTURE_RECTANGLE, "ColorTex", mFrontColorBlenderTexId, Vivid::TextureUnit0 );
  bindTexture( mShaderFrontFinal.get(), GL_TEXTURE_RECTANGLE, "vl_Vivid.stencil.texture", mStencilTexture->handle(), Vivid::TextureUnit1 );
  mShaderFrontFinal->applyUniformSet(); VL_CHECK_OGL();

  drawFullScreenQuad();

  // 2D Outlines

  render2DOutlines( render_queue, camera, 0 );

  // 3D Outlines & Slicing
#if 0
  // draw buffer
  glBindFramebuffer( GL_FRAMEBUFFER, 0 );
  glDrawBuffer( GL_BACK );
  // viewport setup
  camera->viewport()->setClearFlags( CF_DO_NOT_CLEAR );
  camera->viewport()->activate();
  mShaderOutline3D->useProgram();
  mShaderOutline3D->applyUniformSet();
  renderQueue( render_queue, mShaderOutline3D.get(), camera, frame_clock, true, true );
#endif

  // Cleanup

  glUseProgram(0);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_RECTANGLE, 0);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_RECTANGLE, 0);
  glActiveTexture(GL_TEXTURE0);

  // Restore depth buffer

  glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 ); VL_CHECK_OGL();
  glBindFramebuffer( GL_READ_FRAMEBUFFER, mDepthFBO ); VL_CHECK_OGL();
  glDrawBuffer( GL_NONE ); VL_CHECK_OGL();
  // ivec2 fb_size = ivec2( framebuffer()->width(), framebuffer()->height() );
  glBlitFramebuffer( 0, 0, fb_size.x()-1, fb_size.y()-1,
                     0, 0, fb_size.x()-1, fb_size.y()-1,
                     GL_DEPTH_BUFFER_BIT,
                     GL_NEAREST); VL_CHECK_OGL();

  // restore defaults
  glBindFramebuffer( GL_FRAMEBUFFER, 0 );
  glDrawBuffer( GL_BACK );

  VL_CHECK_OGL();
}
//-------------------------------------------------------------------------------------------------
void VividRenderer::renderStandard(const RenderQueue* render_queue, Camera* camera, real frame_clock) {
  setEnableMask( Vivid::StandardEnableMask );

  Renderer::renderRaw( render_queue, camera, frame_clock );

  setEnableMask( Vivid::VividEnableMask );
}
//-------------------------------------------------------------------------------------------------
void VividRenderer::render2DOutlines(const RenderQueue* render_queue, Camera* camera, real frame_clock) {
  OpenGLContext* opengl_context = framebuffer()->openglContext();

  glUseProgram( 0 );
  glDisable( GL_BLEND );
  glDisable( GL_DEPTH_TEST ); // don't care about depth
  glDepthMask( GL_FALSE );    // don't care about depth
  glDisable( GL_LIGHTING );
  glDisable( GL_FOG );
  glDisable( GL_CULL_FACE );
  glDisable( GL_LINE_SMOOTH );
  glHint( GL_LINE_SMOOTH_HINT, GL_DONT_CARE );
  glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

  // Update legacy matrices
  projViewTransfCallback()->updateMatrices( /*update_cm*/true, /*update_tr*/false, NULL, camera, NULL );

  for(int itok=0; itok < render_queue->size(); ++itok)
  {
    const RenderToken* tok = render_queue->at(itok); VL_CHECK(tok);
    Actor* actor = tok->mActor; VL_CHECK(actor);

    if ( ! isEnabled( actor ) )
      continue;

    VL_CHECK_OGL()

    // --------------- shader setup ---------------

    const Shader* shader = tok->mShader;

    int silh_mode = shader->getUniform("vl_Vivid.renderMode") ? shader->getUniform("vl_Vivid.renderMode")->getUniformI() : 0;
    switch ( silh_mode ) {
    case 0:
      // Polygons only
    case 1:
      // Outline only
    case 2:
      // Polygons + Outline
    case 3:
      // Slice only
      continue;
    case 4:
      // Image-space outlines
    case 5:
      // Polygons + Image-space outlines
      break;
    default:
      Log::error( Say("vl_Vivid.renderMode unexpected value: %n\n") << silh_mode );
      VL_TRAP();
      continue;
    }

    VL_CHECK_OGL()

    // --------------- Actor pre-render callback ---------------

    // here the user has still the possibility to modify the Actor's uniforms

    actor->dispatchOnActorRenderStarted( frame_clock, camera, tok->mRenderable, shader, /*ipass*/0 );

    VL_CHECK_OGL()

    // --------------- Transform setup ---------------

    // Update legacy matrices
    projViewTransfCallback()->updateMatrices( /*update_cm*/false, /*update_tr*/true, NULL, camera, actor->transform() );

    VL_CHECK_OGL()

    // --------------- Actor rendering ---------------

    // bind offscreen framebuffer
    glBindFramebuffer( GL_FRAMEBUFFER, mStencilFBO );
    glDrawBuffer( GL_COLOR_ATTACHMENT0 );
    // use default program, no alpha
    glDisable( GL_BLEND );
    glUseProgram( 0 );
    // white: outside the scissor so we don't get the `capping` effect
    VL_CHECK( glIsEnabled( GL_SCISSOR_TEST ) );
    glDisable( GL_SCISSOR_TEST );
      glClearColor( 1, 1, 1, 1 );
      glClear( GL_COLOR_BUFFER_BIT );
    glEnable( GL_SCISSOR_TEST );
    // black: inside the scissor
    glClearColor( 0, 0, 0, 0 );
    glClear( GL_COLOR_BUFFER_BIT );
    // white: render geometry
    glColor4f( 1, 1, 1, 1 );
    tok->mRenderable->render( actor, shader, camera, opengl_context );
    // bind previous framebuffer
    glBindFramebuffer( GL_FRAMEBUFFER, 0 ); VL_CHECK_OGL();
    glDrawBuffer( GL_BACK ); VL_CHECK_OGL();
    // draw quad using outline.glsl
    glEnable( GL_BLEND );
    mShaderOutline2D->useProgram();
    bindTexture( mShaderOutline2D.get(), GL_TEXTURE_RECTANGLE, "vl_Vivid.stencil.texture", mStencilTexture->handle(), Vivid::TextureUnit0 );
    mShaderOutline2D->applyUniformSet(); VL_CHECK_OGL();
    mShaderOutline2D->applyUniformSet( shader->getUniformSet() );
    VL_CHECK( shader->getUniform( "vl_Vivid.outline.color" ) );
    VL_CHECK( shader->getUniform( "vl_Vivid.outline.width" ) );
    drawFullScreenQuad();

    VL_CHECK_OGL()
  }

  // rest defaults

  glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
  glDisable( GL_BLEND );
  glDisable( GL_DEPTH_TEST );
  glDepthMask( GL_TRUE );
  glDisable( GL_LIGHTING );
  glDisable( GL_FOG );
  glDisable( GL_CULL_FACE );
  glDisable( GL_LINE_SMOOTH );
  glHint( GL_LINE_SMOOTH_HINT, GL_DONT_CARE );
  glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
  glDisable( GL_POLYGON_OFFSET_FILL );
}
//-------------------------------------------------------------------------------------------------
int VividRenderer::renderQueue(const RenderQueue* render_queue, GLSLProgram* cur_glsl, Camera* camera, real frame_clock, bool depth_peeling_on, bool outline_on ) {
  mPassCounter++;

  int translucent_actor_count = 0;

  OpenGLContext* opengl_context = framebuffer()->openglContext();

  // Update matrices
  projViewTransfCallback()->updateMatrices( /*update_cm*/true, /*update_tr*/false, cur_glsl, camera, NULL );

  glEnable( GL_POINT_SPRITE );
  if ( outline_on ) {
    glEnable( GL_LINE_SMOOTH );
    glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
  }

  for(int itok=0; itok < render_queue->size(); ++itok)
  {
    const RenderToken* tok = render_queue->at(itok); VL_CHECK(tok);
    Actor* actor = tok->mActor; VL_CHECK(actor);

    if ( ! isEnabled( actor ) )
      continue;

    VL_CHECK_OGL()

    // --------------- shader setup ---------------

    const Shader* shader = tok->mShader;

    int silh_mode = shader->getUniform("vl_Vivid.renderMode") ? shader->getUniform("vl_Vivid.renderMode")->getUniformI() : 0;
    switch ( silh_mode ) {
    case 0:
      // Polygons only
      if ( outline_on ) {
        continue;
      }
      break;
    case 1:
      // Outline only
      if ( ! outline_on ) {
        continue;
      }
      break;
    case 2:
      // Polygons + Outline
      break;
    case 3:
      // Slice only
      if ( ! outline_on ) {
        continue;
      }
      break;
    case 4:
      // Image-space outline
      continue;
    case 5:
      // Polygons + Image-space outline
      if ( outline_on ) {
        continue;
      }
      break;
    default:
      Log::error( Say("vl_Vivid.renderMode unexpected value: %n\n") << silh_mode );
      VL_TRAP();
      continue;
    }

    // texture mapping
    if( shader->getUniform("vl_Vivid.enableTextureMapping")->getUniformI() == 1 ) {
      // enable texture
      shader->getTextureSampler( Vivid::UserTexture )->apply( Vivid::UserTexture, NULL, opengl_context );
      // set texture dimension
      switch( shader->getTextureSampler( Vivid::UserTexture )->texture()->dimension() ) {
        case vl::TD_TEXTURE_1D: const_cast<Shader*>(shader)->getUniform("vl_Vivid.textureDimension")->setUniformI( 1 ); break;
        case vl::TD_TEXTURE_2D: const_cast<Shader*>(shader)->getUniform("vl_Vivid.textureDimension")->setUniformI( 2 ); break;
        case vl::TD_TEXTURE_3D: const_cast<Shader*>(shader)->getUniform("vl_Vivid.textureDimension")->setUniformI( 3 ); break;
        // error
        default: const_cast<Shader*>(shader)->getUniform("vl_Vivid.textureDimension")->setUniformI( 0 ); break;
      }

      VL_CHECK_OGL();
    } else {
      TextureSampler::reset( Vivid::UserTexture, opengl_context );
    }

    // Simple translucent actor detection
    // This works in the simplest but usual cases where:
    // - Lighting is on and material diffuse alpha is < 1
    // - Outline color alpha is < 1
    // - All vertex colors alphas are < 1
    // - Texture has transparent texels, ie alpha == 0 (point sprites, alpha masking etc.)
    // Does not detect:
    // - Some but not all vertex color alphas are < 1
    // - Texture has translucent texels, ie 0 < alpha < 1 (as opposed to 0 <= alpha <= 1)

    vec4 vc;
    Geometry* geom = dynamic_cast<Geometry*>( tok->mRenderable );
    if ( geom ) {
      ArrayFloat4* ca = geom->colorArray()->as<ArrayFloat4>();
      vc = ca && ca->size() ? ca->at(0) : vl::white;
    }
    if ( shader->getUniform("vl_Vivid.material.diffuse")->getUniform4F().a() < 1 ||
         shader->getUniform("vl_Vivid.outline.color")->getUniform4F().a() < 1 ||
         shader->getUniform("vl_Vivid.fog.target")->getUniformI() == vl::Vivid::Alpha ||          // note: it could be disabled
         shader->getUniform("vl_Vivid.smartClip[0].target")->getUniformI() == vl::Vivid::Alpha || // note: it could be disabled
         shader->getUniform("vl_Vivid.smartClip[1].target")->getUniformI() == vl::Vivid::Alpha ||
         shader->getUniform("vl_Vivid.smartClip[2].target")->getUniformI() == vl::Vivid::Alpha ||
         shader->getUniform("vl_Vivid.smartClip[3].target")->getUniformI() == vl::Vivid::Alpha ||
         vc.a() < 1 ) {
      ++translucent_actor_count;
    }

    shader->getPointSize()->apply(0, NULL, opengl_context); VL_CHECK_OGL()
    shader->getPolygonMode()->apply(0, NULL, opengl_context); VL_CHECK_OGL()
    if ( ! outline_on ) {
      shader->getLineWidth()->apply(0, NULL, opengl_context);
    }
    if ( cur_glsl && shader->getUniformSet() ) {
      cur_glsl->applyUniformSet( shader->getUniformSet() );
      if ( outline_on ) {
        if ( shader->getUniformSet()->getUniform("vl_Vivid.outline.width") ) {
          glLineWidth( shader->getUniformSet()->getUniform("vl_Vivid.outline.width")->getUniformF() );
        } else {
          // this should never happen
          glLineWidth( 1 );
          VL_TRAP();
        }
      }
    }

    /* These two states are managed by the Depth Peeling algorithm */
    if ( ! depth_peeling_on ) {
      shader->isEnabled(EN_BLEND) ? glEnable(GL_BLEND) : glDisable(GL_BLEND); VL_CHECK_OGL()
      shader->getDepthMask() ? glDepthMask( shader->getDepthMask()->depthMask() ) : glDepthMask( GL_TRUE );
      shader->isEnabled(EN_DEPTH_TEST) ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST); VL_CHECK_OGL()
      shader->isEnabled(EN_CULL_FACE) ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE); VL_CHECK_OGL()
      shader->isEnabled(EN_LIGHTING) ? glEnable(GL_LIGHTING) : glDisable(GL_LIGHTING); VL_CHECK_OGL()
      shader->isEnabled(EN_FOG) ? glEnable(GL_FOG) : glDisable(GL_FOG); VL_CHECK_OGL()
    }

    // --------------- Actor pre-render callback ---------------

    // here the user has still the possibility to modify the Actor's uniforms

    actor->dispatchOnActorRenderStarted( frame_clock, camera, tok->mRenderable, shader, /*ipass*/0 );

    VL_CHECK_OGL()

    // --------------- Transform setup ---------------

    // Update matrices
    projViewTransfCallback()->updateMatrices( /*update_cm*/false, /*update_tr*/true, cur_glsl, camera, actor->transform() );

    VL_CHECK_OGL()

    // --------------- Actor rendering ---------------

    VL_CHECK( ! cur_glsl || cur_glsl->validateProgram() );

    // also compiles display lists and updates BufferObjects if necessary
    tok->mRenderable->render( actor, shader, camera, opengl_context );

    VL_CHECK_OGL()
  }

  TextureSampler::reset(Vivid::UserTexture, opengl_context);

  glDisable( GL_POINT_SPRITE );
  if ( outline_on ) {
    glHint( GL_LINE_SMOOTH_HINT, GL_DONT_CARE );
    glDisable( GL_LINE_SMOOTH );
    glLineWidth( 1 );
  }

  glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

  return translucent_actor_count;
}
//-------------------------------------------------------------------------------------------------
const RenderQueue* VividRenderer::render(const RenderQueue* render_queue, Camera* camera, real frame_clock)
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
    VividRenderer* mRenderer;
    std::vector<RenderStateSlot> mOriginalDefaultRS;
  public:
    InOutContract(VividRenderer* renderer, Camera* camera): mRenderer(renderer)
    {
      // increment the render tick.
      mRenderer->mRenderTick++;

      // render-target activation.
      // note: an OpenGL context can have multiple rendering targets!
      mRenderer->framebuffer()->activate();

      // viewport setup
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

  // Update legacy camera projection
  projViewTransfCallback()->updateMatrices( /*update_cm*/true, /*update_tr*/false, NULL, camera, /*cur_transform*/NULL );

  glDisable(GL_COLOR_MATERIAL);
  // glDisable(GL_SCISSOR_TEST);
  glDepthMask(GL_TRUE);
  glDisable(GL_CULL_FACE);
  glDisable(GL_LIGHTING);
  glDisable(GL_LIGHT0);
  glDisable(GL_NORMALIZE);
  glDisable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_FOG);
  glDisable(GL_POINT_SMOOTH);

  switch ( mVividRendering->renderingMode() )
  {
  case Vivid::FastRender:
    renderControl( render_queue, camera, frame_clock );
    break;
  case Vivid::StencilRender:
    renderStencil( camera );
    break;
  case Vivid::DepthPeeling:
    if ( mVividRendering->isStencilEnabled() ) {
      renderStencil( camera );
    }
    renderFrontToBackPeeling( render_queue, camera, frame_clock );
    break;
  }

  glDisable(GL_POINT_SMOOTH);
  glDisable(GL_COLOR_MATERIAL);
  // glDisable(GL_SCISSOR_TEST);
  glDepthMask(GL_TRUE);
  glDisable(GL_CULL_FACE);
  glDisable(GL_LIGHTING);
  glDisable(GL_LIGHT0);
  glDisable(GL_NORMALIZE);
  glDisable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_FOG);

  // clear enables
  opengl_context->applyEnables( mDummyEnables.get() ); VL_CHECK_OGL();

  // clear render states
  opengl_context->applyRenderStates( mDummyStateSet.get(), NULL ); VL_CHECK_OGL();

  // enabled texture unit #0
  VL_glActiveTexture( GL_TEXTURE0 ); VL_CHECK_OGL();
  if (Has_Fixed_Function_Pipeline) {
    VL_glClientActiveTexture( GL_TEXTURE0 ); VL_CHECK_OGL();
  }

  // disable scissor test
  glDisable( GL_SCISSOR_TEST ); VL_CHECK_OGL();

  // disable all vertex arrays, note this also calls "glBindBuffer(GL_ARRAY_BUFFER, 0)"
  opengl_context->bindVAS( NULL, false, false ); VL_CHECK_OGL();

  // Render Volumes

  VL_CHECK( this->framebuffer()->openglContext()->isCleanState( true ) );
  renderStandard( render_queue, camera, frame_clock );

  return render_queue;
}
//-------------------------------------------------------------------------------------------------
void VividRenderer::renderBackgroundImage( Camera* camera ) {
  class SaveStates {
    GLint mViewport[4];
    GLint mCurrentProgram;
    GLboolean mDepthMask;
    GLboolean mBlendOn;
    GLboolean mDepthTestOn;
    GLboolean mLightingOn;
  public:
    void enable(GLint state, GLboolean on) {
      if ( on ) {
        glEnable( state );
      } else {
        glDisable( state );
      }
    }

    SaveStates() {
      glMatrixMode( GL_MODELVIEW );
      glPushMatrix();

      glMatrixMode( GL_PROJECTION );
      glPushMatrix();

      glGetIntegerv( GL_VIEWPORT, mViewport );

      glGetIntegerv( GL_CURRENT_PROGRAM, &mCurrentProgram ); VL_CHECK_OGL();
      glGetBooleanv( GL_DEPTH_WRITEMASK, &mDepthMask ); VL_CHECK_OGL();
      mBlendOn = glIsEnabled( GL_BLEND );
      mDepthTestOn = glIsEnabled( GL_DEPTH_TEST );
      mLightingOn = glIsEnabled( GL_LIGHTING );

      glUseProgram( 0 );
      glDepthMask( GL_FALSE );
      glDisable( GL_BLEND );
      glDisable( GL_DEPTH_TEST );
      glDisable( GL_LIGHTING );
    }

    ~SaveStates() {
      glMatrixMode( GL_MODELVIEW );
      glPopMatrix();

      glMatrixMode( GL_PROJECTION );
      glPopMatrix();

      glViewport( mViewport[0], mViewport[1], mViewport[2], mViewport[3] );

      glUseProgram( mCurrentProgram );
      glDepthMask( mDepthMask );
      enable( GL_BLEND, mBlendOn );
      enable( GL_DEPTH_TEST, mDepthTestOn );
      enable( GL_LIGHTING, mLightingOn );
    }
  };
  SaveStates save_matrices;

  // initialize background quad (FIXME: we could do this in a vertex shader)

  CalibratedCamera* camera_cal = dynamic_cast<CalibratedCamera*>( camera );
  // camera_cal->setScreenSize( framebuffer()->width(), framebuffer()->height() );
  // camera_cal->setCalibratedImageSize( mBackgroundImage->width(), mBackgroundImage->height(), 1 );

  VL_CHECK( camera_cal );
  float ipa = camera_cal->pixelAspectRatio(); // image pixel aspect ratio
  float sw = (float) framebuffer()->width();
  float sh = (float) framebuffer()->height();
  float iw = (float) mVividRendering->backgroundTexSampler()->texture()->width();
  float ih = (float) mVividRendering->backgroundTexSampler()->texture()->height() / ipa;

  // allow aspect ratio

  float bg_hw = 0; // background half width
  float bg_hh = 0; // background half height
  float sa = sw / sh;

  bg_hw = iw / sw; // background half width
  bg_hh = ih / sw * sa; // background half height

  // expand out
  if ( sw/iw > sh/ih ) {
    bg_hw *= sw/iw;
    bg_hh *= sw/iw;
  } else {
    bg_hw *= sh/ih;
    bg_hh *= sh/ih;
  }

  // fit horizontally
  if ( bg_hw > 1.0f ) {
    bg_hh /= bg_hw;
    bg_hw /= bg_hw;
  }

  // fit vertically
  if ( bg_hh > 1.0f ) {
    bg_hw /= bg_hh;
    bg_hh /= bg_hh;
  }

  ArrayFloat2* bgq_vert2 = mBackgroundQuad->vertexArray()->as<ArrayFloat2>();
  const GLfloat vertices[] = {
    -bg_hw, -bg_hh,
    -bg_hw, +bg_hh,
    +bg_hw, +bg_hh,
    +bg_hw, -bg_hh,
  };
  memcpy( bgq_vert2->ptr(), vertices, bgq_vert2->bytesUsed() );
  bgq_vert2->updateBufferObject();

  // init 2D projection and viewport for background image on the fly based on calibrated camera info

  glViewport( 0, 0, framebuffer()->width(), framebuffer()->height() );

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glMatrixMode(GL_PROJECTION);
  mat4 proj = mat4::getOrtho( -1, +1, -1, +1, -1, 1 );
  glLoadMatrixf( proj.ptr() );

  // render centered background image (test various sizes)

  // vertex color
  glColor4d( 1.0, 1.0, 1.0, 1.0 );
  // enable texture 0
  mVividRendering->backgroundTexSampler()->apply( Vivid::TextureUnit0, camera, framebuffer()->openglContext() );
  // render the  quad
  mBackgroundQuad->render( NULL, NULL, NULL, framebuffer()->openglContext() );
  // disable texture 0
  TextureSampler::reset( 0, framebuffer()->openglContext() );
}
