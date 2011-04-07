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

#include "BaseDemo.hpp"
#include <vlGraphics/GeometryPrimitives.hpp>
#include <vlGraphics/RenderingTree.hpp>
#include <vlGraphics/CopyTexSubImage.hpp>
#include <vlGraphics/BlitFramebuffer.hpp>
#include <vlGraphics/SceneManager.hpp>
#include <vlGraphics/GLSL.hpp>
#include <vlGraphics/Light.hpp>

class App_Framebuffer_Object: public BaseDemo
{
  static const int mFBO_Size = 1024;

public:
  App_Framebuffer_Object(int test): mTestNum(test) {}

  /*
  This example demonstates the use of the render-to-texture (RTT) technique using framebuffer objects.
  */
  void initTest_FBO_Render_To_Texture()
  {
    // Setup dual rendering

    vl::ref<vl::RenderingTree> render_tree = new vl::RenderingTree;
    setRendering(render_tree.get());
    mMainRendering = new vl::Rendering;
    mRTT_Rendering = new vl::Rendering;
    render_tree->subRenderings()->push_back(mRTT_Rendering.get());
    render_tree->subRenderings()->push_back(mMainRendering.get());
    mMainRendering->sceneManagers()->push_back(new vl::SceneManagerActorTree);
    mRTT_Rendering->sceneManagers()->push_back(new vl::SceneManagerActorTree);

    // RTT rendering

    mRTT_Rendering->camera()->viewport()->setClearColor( vl::crimson );
    mRTT_Rendering->camera()->viewport()->set(0, 0, mFBO_Size, mFBO_Size);
    mRTT_Rendering->camera()->setProjectionAsPerspective();
    vl::mat4 m = vl::mat4::getLookAt(vl::vec3(0,0,10.5f), vl::vec3(0,0,0), vl::vec3(0,1,0));
    mRTT_Rendering->camera()->setInverseViewMatrix(m);

    /* use a framebuffer object as render target */
    vl::ref<vl::FBORenderTarget> fbo_render_target = openglContext()->createFBORenderTarget(mFBO_Size, mFBO_Size);
    mRTT_Rendering->renderer()->setRenderTarget( fbo_render_target.get() );

    /* bind a depth buffer */
    vl::ref<vl::FBODepthBufferAttachment> fbo_depth_attachm = new vl::FBODepthBufferAttachment(vl::DBF_DEPTH_COMPONENT);
    fbo_render_target->addDepthAttachment( fbo_depth_attachm.get() );

    /* use texture as color buffer */
    vl::ref<vl::Texture> texture = new vl::Texture(mFBO_Size, mFBO_Size, vl::TF_RGBA);
    vl::ref<vl::FBOTexture2DAttachment> fbo_tex_attachm = new vl::FBOTexture2DAttachment(texture.get(), 0, vl::T2DT_TEXTURE_2D);
    fbo_render_target->addTextureAttachment( vl::AP_COLOR_ATTACHMENT0, fbo_tex_attachm.get() );
    mRTT_Rendering->renderer()->renderTarget()->setDrawBuffer( vl::RDB_COLOR_ATTACHMENT0 );
  
    // Main rendering

    /* setup camera */
    mMainRendering->camera()->viewport()->setClearColor( vl::midnightblue );
    mMainRendering->camera()->viewport()->set(0, 0, openglContext()->renderTarget()->width(), openglContext()->renderTarget()->height());
    m = vl::mat4::getLookAt(vl::vec3(0,15,25), vl::vec3(0,0,0), vl::vec3(0,1,0));
    mMainRendering->camera()->setInverseViewMatrix(m);

    /* use the opengl window as render target */
    mMainRendering->renderer()->setRenderTarget( openglContext()->renderTarget() );

    /* populate the scene */
    addRings(NULL);
    addCube(texture.get(), NULL);
  }

  /*
  This example demonstates the use of the multiple-render-target tecnique to render on two textures simultaneously.
  */
  void initTest_FBO_Render_To_Texture_MRT()
  {
    // Setup dual rendering

    vl::ref<vl::RenderingTree> render_tree = new vl::RenderingTree;
    setRendering(render_tree.get());
    mMainRendering = new vl::Rendering;
    mRTT_Rendering = new vl::Rendering;
    render_tree->subRenderings()->push_back(mRTT_Rendering.get());
    render_tree->subRenderings()->push_back(mMainRendering.get());
    mMainRendering->sceneManagers()->push_back(new vl::SceneManagerActorTree);
    mRTT_Rendering->sceneManagers()->push_back(new vl::SceneManagerActorTree);

    // RTT rendering

    mRTT_Rendering->camera()->viewport()->setClearColor( vl::crimson );
    mRTT_Rendering->camera()->viewport()->set(0, 0, mFBO_Size, mFBO_Size);
    mRTT_Rendering->camera()->setProjectionAsPerspective();
    vl::mat4 m = vl::mat4::getLookAt(vl::vec3(0,0,10.5f), vl::vec3(0,0,0), vl::vec3(0,1,0));
    mRTT_Rendering->camera()->setInverseViewMatrix(m);

    /* use a framebuffer object as render target */
    vl::ref<vl::FBORenderTarget> fbo_render_target = openglContext()->createFBORenderTarget(mFBO_Size, mFBO_Size);
    mRTT_Rendering->renderer()->setRenderTarget( fbo_render_target.get() );

    /* bind a depth buffer */
    vl::ref<vl::FBODepthBufferAttachment> fbo_depth_attachm = new vl::FBODepthBufferAttachment(vl::DBF_DEPTH_COMPONENT);
    fbo_render_target->addDepthAttachment( fbo_depth_attachm.get() );

    /* use a per-pixel-light GLSL shader which renders on two color attachments at the same time */
    vl::ref<vl::GLSLProgram> glsl = new vl::GLSLProgram;
    glsl->attachShader( new vl::GLSLVertexShader("/glsl/perpixellight.vs") );
    if (GLEW_EXT_gpu_shader4||GLEW_VERSION_3_0)
    {
      vl::Log::info("using glBindFragDataLocation()\n");
      // see fragment shader sources for the details
      glsl->attachShader( new vl::GLSLFragmentShader("/glsl/perpixellight_cartoon_mrt2.fs") );
      glsl->bindFragDataLocation(0, "FragDataOutputA"); // out varying variable writing on color attachment #0
      glsl->bindFragDataLocation(1, "FragDataOutputB"); // out varying variable writing on color attachment #1
    }
    else
    {
      vl::Log::info("glBindFragDataLocation() not supported.\n");
      // see fragment shader sources for the details
      glsl->attachShader( new vl::GLSLFragmentShader("/glsl/perpixellight_cartoon_mrt.fs") );
    }

    /* use two textures as color attachment #0 and #1 */
    vl::ref<vl::Texture> texture1 = new vl::Texture(mFBO_Size, mFBO_Size, vl::TF_RGBA8);
    vl::ref<vl::Texture> texture2 = new vl::Texture(mFBO_Size, mFBO_Size, vl::TF_RGBA8);
    vl::ref<vl::FBOTexture2DAttachment> fbo_tex_attachm1 = new vl::FBOTexture2DAttachment(texture1.get(), 0, vl::T2DT_TEXTURE_2D);
    vl::ref<vl::FBOTexture2DAttachment> fbo_tex_attachm2 = new vl::FBOTexture2DAttachment(texture2.get(), 0, vl::T2DT_TEXTURE_2D);
    fbo_render_target->addTextureAttachment( vl::AP_COLOR_ATTACHMENT0, fbo_tex_attachm1.get() );
    fbo_render_target->addTextureAttachment( vl::AP_COLOR_ATTACHMENT1, fbo_tex_attachm2.get() );

    /* draw on colorbuffers #0 and #1 */
    mRTT_Rendering->renderer()->renderTarget()->setDrawBuffers(vl::RDB_COLOR_ATTACHMENT0, vl::RDB_COLOR_ATTACHMENT1);

    /* screen shot grabbing on color attachment #0 */
    vl::ref<vl::ReadPixels> read_pixels_0 = new vl::ReadPixels;
    read_pixels_0->setup( 0, 0, mFBO_Size, mFBO_Size, vl::RDB_COLOR_ATTACHMENT0, false );
    mRTT_Rendering->onFinishedCallbacks()->push_back(read_pixels_0.get());
    read_pixels_0->setRemoveAfterCall(true);
    read_pixels_0->setSavePath("MRT-COLOR_ATTACHMENT0.tif");

    /* screen shot grabbing on color attachment #1 */
    vl::ref<vl::ReadPixels> read_pixels_1 = new vl::ReadPixels;
    read_pixels_1->setup( 0, 0, mFBO_Size, mFBO_Size, vl::RDB_COLOR_ATTACHMENT1, false );
    mRTT_Rendering->onFinishedCallbacks()->push_back(read_pixels_1.get());
    read_pixels_1->setRemoveAfterCall(true);
    read_pixels_1->setSavePath("MRT-COLOR_ATTACHMENT1.tif");

    // Main rendering

    /* setup camera */
    mMainRendering->camera()->viewport()->setClearColor( vl::midnightblue );
    mMainRendering->camera()->viewport()->set(0, 0, openglContext()->renderTarget()->width(), openglContext()->renderTarget()->height());
    m = vl::mat4::getLookAt(vl::vec3(0,15,25), vl::vec3(0,0,0), vl::vec3(0,1,0));
    mMainRendering->camera()->setInverseViewMatrix(m);

    /* use the opengl window as render target */
    mMainRendering->renderer()->setRenderTarget( openglContext()->renderTarget() );

    /* populate the scene */
    addRings(glsl.get());
    addCube(texture1.get(), texture2.get() );
  }

  /*
  This example shows how to perform off-screen rendering on a color buffer. 
  The content of the colorbuffer is then copied to a texture using CopyTexSubImage2D.
  */
  void initTest_FBO_Render_To_Color_Buffer_And_Copy_To_Texture()
  {
    // Setup dual rendering

    vl::ref<vl::RenderingTree> render_tree = new vl::RenderingTree;
    setRendering(render_tree.get());
    mMainRendering = new vl::Rendering;
    mRTT_Rendering = new vl::Rendering;
    render_tree->subRenderings()->push_back(mRTT_Rendering.get());
    render_tree->subRenderings()->push_back(mMainRendering.get());
    mMainRendering->sceneManagers()->push_back(new vl::SceneManagerActorTree);
    mRTT_Rendering->sceneManagers()->push_back(new vl::SceneManagerActorTree);

    // RTT rendering

    mRTT_Rendering->camera()->viewport()->setClearColor( vl::crimson );
    mRTT_Rendering->camera()->viewport()->set(0, 0, mFBO_Size, mFBO_Size);
    mRTT_Rendering->camera()->setProjectionAsPerspective();
    vl::mat4 m = vl::mat4::getLookAt(vl::vec3(0,0,10.5f), vl::vec3(0,0,0), vl::vec3(0,1,0));
    mRTT_Rendering->camera()->setInverseViewMatrix(m);

    /* use a framebuffer object as render target */
    vl::ref<vl::FBORenderTarget> fbo_render_target = openglContext()->createFBORenderTarget(mFBO_Size, mFBO_Size);
    mRTT_Rendering->renderer()->setRenderTarget( fbo_render_target.get() );

    /* bind a depth buffer */
    vl::ref<vl::FBODepthBufferAttachment> fbo_depth_attachm = new vl::FBODepthBufferAttachment(vl::DBF_DEPTH_COMPONENT);
    fbo_render_target->addDepthAttachment( fbo_depth_attachm.get() );

    /* bind a normal color buffer as color attachment #0 */
    vl::ref<vl::FBOColorBufferAttachment> fbo_col_buf_attachm = new vl::FBOColorBufferAttachment(vl::CBF_RGBA8);
    fbo_render_target->addColorAttachment( vl::AP_COLOR_ATTACHMENT0, fbo_col_buf_attachm.get() );
    mRTT_Rendering->renderer()->renderTarget()->setDrawBuffer(vl::RDB_COLOR_ATTACHMENT0);

    /* at the end of the rendering copy the color attachment pixels into the texture */
    vl::ref<vl::Texture> texture = new vl::Texture(mFBO_Size, mFBO_Size, vl::TF_RGBA8, false); // note that mipmapping is off
    vl::ref<vl::CopyTexSubImage2D> copytex = new vl::CopyTexSubImage2D(0, 0,0, 0,0, mFBO_Size,mFBO_Size, texture.get(), vl::T2DT_TEXTURE_2D, vl::RDB_COLOR_ATTACHMENT0);
    mRTT_Rendering->onFinishedCallbacks()->push_back(copytex.get());
  
    // Main rendering

    /* setup camera */
    mMainRendering->camera()->viewport()->setClearColor( vl::midnightblue );
    mMainRendering->camera()->viewport()->set(0, 0, openglContext()->renderTarget()->width(), openglContext()->renderTarget()->height());
    m = vl::mat4::getLookAt(vl::vec3(0,15,25), vl::vec3(0,0,0), vl::vec3(0,1,0));
    mMainRendering->camera()->setInverseViewMatrix(m);

    /* use the opengl window as render target */
    mMainRendering->renderer()->setRenderTarget( openglContext()->renderTarget() );

    /* populate the scene */
    addRings(NULL);
    addCube(texture.get(), NULL);
  }

  /*
  This example shows how to perform off-screen rendering on a multisampled color buffer and then copy 
  the content of such buffer to a non-multisampled texture using the vl:BlitFramebuffer callback object.
  */
  void initTest_FBO_Framebuffer_Blit_Multisample()
  {
    if (!(GLEW_EXT_framebuffer_multisample||GLEW_EXT_framebuffer_blit||GLEW_VERSION_3_0))
    {
      vl::Log::error("GL_EXT_framebuffer_multisample or GLEW_EXT_framebuffer_blit not supported.\n");
      vl::Time::sleep(3000);
      exit(1);
    }

    const int samples = 4;

    // Setup dual rendering

    vl::ref<vl::RenderingTree> render_tree = new vl::RenderingTree;
    setRendering(render_tree.get());
    mMainRendering = new vl::Rendering;
    mRTT_Rendering = new vl::Rendering;
    render_tree->subRenderings()->push_back(mRTT_Rendering.get());
    render_tree->subRenderings()->push_back(mMainRendering.get());
    mMainRendering->sceneManagers()->push_back(new vl::SceneManagerActorTree);
    mRTT_Rendering->sceneManagers()->push_back(new vl::SceneManagerActorTree);

    // RTT rendering

    mRTT_Rendering->camera()->viewport()->setClearColor( vl::crimson );
    mRTT_Rendering->camera()->viewport()->set(0, 0, mFBO_Size, mFBO_Size);
    mRTT_Rendering->camera()->setProjectionAsPerspective();
    vl::mat4 m = vl::mat4::getLookAt(vl::vec3(0,0,10.5f), vl::vec3(0,0,0), vl::vec3(0,1,0));
    mRTT_Rendering->camera()->setInverseViewMatrix(m);

    /* use a framebuffer object as render target */
    vl::ref<vl::FBORenderTarget> fbo_render_target = openglContext()->createFBORenderTarget(mFBO_Size, mFBO_Size);
    mRTT_Rendering->renderer()->setRenderTarget( fbo_render_target.get() );

    /* bind a multisampled depth buffer */
    vl::ref<vl::FBODepthBufferAttachment> fbo_depth_attachm = new vl::FBODepthBufferAttachment(vl::DBF_DEPTH_COMPONENT);
    fbo_depth_attachm->setSamples(samples);
    fbo_render_target->addDepthAttachment( fbo_depth_attachm.get() );

    /* bind a multisampled color buffer */
    vl::ref<vl::FBOColorBufferAttachment> fbo_col_buf_attachm = new vl::FBOColorBufferAttachment(vl::CBF_RGBA);
    fbo_col_buf_attachm->setSamples(samples);
    fbo_render_target->addColorAttachment( vl::AP_COLOR_ATTACHMENT0, fbo_col_buf_attachm.get() );
    mRTT_Rendering->renderer()->renderTarget()->setDrawBuffer(vl::RDB_COLOR_ATTACHMENT0);

    /* create a new FBO with 'texture' as its color attachment */
    vl::ref<vl::FBORenderTarget> fbo_rt_texture = openglContext()->createFBORenderTarget(mFBO_Size, mFBO_Size);
    vl::ref<vl::Texture> texture = new vl::Texture(mFBO_Size, mFBO_Size, vl::TF_RGBA);
    vl::ref<vl::FBOTexture2DAttachment> fbo_tex_attachm = new vl::FBOTexture2DAttachment(texture.get(), 0, vl::T2DT_TEXTURE_2D);
    fbo_rt_texture->addTextureAttachment( vl::AP_COLOR_ATTACHMENT0, fbo_tex_attachm.get() );

    /* setup vl::BlitFramebuffer: blits the content of the multisampled color buffer into a non-multisampled texture */
    vl::ref<vl::BlitFramebuffer> blit_fbo = new vl::BlitFramebuffer;
    mRTT_Rendering->onFinishedCallbacks()->push_back(blit_fbo.get());
    blit_fbo->setLinearFilteringEnabled(false);
    blit_fbo->setBufferMask( vl::BB_COLOR_BUFFER_BIT );
    //the commented part does not work with multisampling enabled (samples > 0)
    //int center = mFBO_Size / 2;
    //int side   = mFBO_Size / 4;
    //blit_fbo->setSrcRect( center-side, center-side, center+side, center+side );
    blit_fbo->setSrcRect( 0, 0, mFBO_Size, mFBO_Size );
    blit_fbo->setDstRect( 0, 0, mFBO_Size, mFBO_Size );
    blit_fbo->setReadFramebuffer( fbo_render_target.get() );
    blit_fbo->setDrawFramebuffer( fbo_rt_texture.get() );

    // Main rendering

    /* setup camera */
    mMainRendering->camera()->viewport()->setClearColor( vl::midnightblue );
    mMainRendering->camera()->viewport()->set(0, 0, openglContext()->renderTarget()->width(), openglContext()->renderTarget()->height());
    m = vl::mat4::getLookAt(vl::vec3(0,15,25), vl::vec3(0,0,0), vl::vec3(0,1,0));
    mMainRendering->camera()->setInverseViewMatrix(m);

    /* use the opengl window as render target */
    mMainRendering->renderer()->setRenderTarget( openglContext()->renderTarget() );

    /* populate the scene */
    addRings(NULL);
    addCube(texture.get(), NULL);
  }

  /*
  This example demonstrates how to perform render-to-texture without using framebuffer objects.
  First the scene with the ring is rendered normally on the screen. The pixels on the screen are then copied to 
  a texture using the vl::CopyTexSubImage2D callback object. Finally the scene with the cube can be rendered
  using the previously generated texture.
  */
  void initTest_RTT_Legacy()
  {
    // Setup dual rendering

    vl::ref<vl::RenderingTree> render_tree = new vl::RenderingTree;
    setRendering(render_tree.get());
    mMainRendering = new vl::Rendering;
    mRTT_Rendering = new vl::Rendering;
    render_tree->subRenderings()->push_back(mRTT_Rendering.get());
    render_tree->subRenderings()->push_back(mMainRendering.get());
    mMainRendering->sceneManagers()->push_back(new vl::SceneManagerActorTree);
    mRTT_Rendering->sceneManagers()->push_back(new vl::SceneManagerActorTree);

    /* both render to the screen */
    mMainRendering->renderer()->setRenderTarget( openglContext()->renderTarget() );
    mRTT_Rendering->renderer()->setRenderTarget( openglContext()->renderTarget() );

    // Render to texture rendering

    /* setup render-to-texture rendering: render to screen then copies to texture. */
    mRTT_Rendering->camera()->viewport()->setClearColor( vl::crimson );
    mRTT_Rendering->camera()->viewport()->set(0,0,512,512);
    mRTT_Rendering->camera()->setProjectionAsPerspective();
    vl::mat4 m = vl::mat4::getLookAt( vl::vec3(0,0,10.5f), vl::vec3(0,0,0), vl::vec3(0,1,0) );
    mRTT_Rendering->camera()->setInverseViewMatrix( m );

    /* install copy to texture callback */
    vl::ref<vl::Texture> texture = new vl::Texture( 512, 512, vl::TF_RGBA, false );
    vl::ref<vl::CopyTexSubImage2D> copytex = new vl::CopyTexSubImage2D( 0, 0,0, 0,0, 512,512, texture.get() );
    mRTT_Rendering->onFinishedCallbacks()->push_back( copytex.get() );
    
    // Main rendering

    /* setup main rendering */
    mMainRendering->camera()->viewport()->setClearColor( vl::midnightblue );
    mMainRendering->camera()->viewport()->set(0,0,512,512);
    mMainRendering->camera()->setProjectionAsPerspective();
    m = vl::mat4::getLookAt(vl::vec3(0,15,25), vl::vec3(0,0,0), vl::vec3(0,1,0));
    mMainRendering->camera()->setInverseViewMatrix( m );

    /* populate the scene */
    addCube(texture.get(), NULL);
    addRings(NULL);
  }

  void addCube(vl::Texture* texture1, vl::Texture* texture2)
  {
    vl::ref<vl::Light> light = new vl::Light(0);
    light->setAmbient( vl::fvec4( .1f, .1f, .1f, 1.0f) );
    light->setSpecular( vl::fvec4( .1f, .1f, .1f, 1.0f) );
    vl::ref<vl::Effect> effect1 = new vl::Effect;
    effect1->shader()->setRenderState( light.get() );
    effect1->shader()->gocLightModel()->setTwoSide(true);
    effect1->shader()->enable(vl::EN_LIGHTING);
    effect1->shader()->enable(vl::EN_DEPTH_TEST);
    effect1->shader()->gocTextureUnit(0)->setTexture( texture1 );
    effect1->shader()->gocTexEnv(0)->setMode(vl::TEM_MODULATE);

    // ground plane
    const vl::Real size = 50;
    vl::ref<vl::Geometry> ground = vl::makeGrid( vl::vec3(0,0,0), size, size, 2, 2, true, vl::fvec2(0,0), vl::fvec2(1,1) );
    ground->setNormal( vl::fvec3(0,1,0) );
    mMainRendering->sceneManagers()->at(0)->as<vl::SceneManagerActorTree>()->tree()->addActor( ground.get(), effect1.get() );

    if (texture2)
    {
      // box #1
      vl::ref<vl::Geometry> box1 = vl::makeBox( vl::vec3(-7,5,0), 10,10,10);
      box1->computeNormals();
      mMainRendering->sceneManagers()->at(0)->as<vl::SceneManagerActorTree>()->tree()->addActor( box1.get(), effect1.get() );

      // box #2
      vl::ref<vl::Effect> effect2 = new vl::Effect;
      effect2->shader()->copy(*effect1->shader());
      vl::ref<vl::TextureUnit> texture_unit = new vl::TextureUnit(0);
      texture_unit->setTexture(texture2);
      effect2->shader()->setRenderState(texture_unit.get());

      vl::ref<vl::Geometry> box2 = vl::makeBox( vl::vec3(+7,5,0), 10,10,10);
      box2->computeNormals();
      mMainRendering->sceneManagers()->at(0)->as<vl::SceneManagerActorTree>()->tree()->addActor( box2.get(), effect2.get() );
    }
    else
    {
      // box #1
      vl::ref<vl::Geometry> box1 = vl::makeBox( vl::vec3(0,5,0), 10,10,10);
      box1->computeNormals();
      mMainRendering->sceneManagers()->at(0)->as<vl::SceneManagerActorTree>()->tree()->addActor( box1.get(), effect1.get() );
    }

  }

  // helper function
  vl::Actor* addActor(vl::Rendering* rendering, vl::Renderable* renderable, vl::Effect* eff, vl::Transform* tr)
  {
    return rendering->sceneManagers()->at(0)->as<vl::SceneManagerActorTree>()->tree()->addActor(renderable, eff, tr);
  }

  // populates ring scene on mRTT_Rendering
  void addRings(vl::GLSLProgram* glsl)
  {
    vl::ref<vl::Effect> effect = new vl::Effect;
    effect->shader()->setRenderState(glsl);
    vl::ref<vl::Light> light = new vl::Light(0);
    light->setAmbient( vl::fvec4( .1f, .1f, .1f, 1.0f) );
    light->setSpecular( vl::fvec4( .9f, .9f, .9f, 1.0f) );
    effect->shader()->setRenderState( light.get() );
    effect->shader()->enable(vl::EN_LIGHTING);
    effect->shader()->enable(vl::EN_DEPTH_TEST);
    effect->shader()->enable(vl::EN_CULL_FACE);
    effect->shader()->gocMaterial()->setDiffuse( vl::yellow );

    mTransfRing1 = new vl::Transform;
    vl::ref<vl::Geometry> ring1;
    ring1 = vl::makeTorus( vl::vec3(0,0,0), 10,0.5, 20,100);
    addActor(mRTT_Rendering.get(), ring1.get(), effect.get(), mTransfRing1.get());

    mTransfRing2 = new vl::Transform;
    vl::ref<vl::Geometry> ring2;
    ring2= vl::makeTorus( vl::vec3(0,0,0), 9,0.5, 20,100);
    addActor(mRTT_Rendering.get(), ring2.get(), effect.get(), mTransfRing2.get());

    mTransfRing3 = new vl::Transform;
    vl::ref<vl::Geometry> ring3;
    ring3= vl::makeTorus( vl::vec3(0,0,0), 8,0.5, 20,100);
    addActor(mRTT_Rendering.get(), ring3.get(), effect.get(), mTransfRing3.get());

    mTransfRing4 = new vl::Transform;
    vl::ref<vl::Geometry> ring4;
    ring4= vl::makeTorus( vl::vec3(0,0,0), 7,0.5, 20,100);
    addActor(mRTT_Rendering.get(), ring4.get(), effect.get(), mTransfRing4.get());

    mTransfRing5 = new vl::Transform;
    vl::ref<vl::Geometry> ring5;
    ring5= vl::makeTorus( vl::vec3(0,0,0), 6,0.5, 20,100);
    addActor(mRTT_Rendering.get(), ring5.get(), effect.get(), mTransfRing5.get());

    // update transform hierarchy every frame
    mRTT_Rendering->transform()->addChild(mTransfRing1.get());
    mTransfRing1->addChild(mTransfRing2.get());
    mTransfRing2->addChild(mTransfRing3.get());
    mTransfRing3->addChild(mTransfRing4.get());
    mTransfRing4->addChild(mTransfRing5.get());
  }

  virtual void initEvent()
  {
    BaseDemo::initEvent();

    if (!(GLEW_EXT_framebuffer_object||GLEW_ARB_framebuffer_object||GLEW_VERSION_3_0||GLEW_VERSION_4_0))
    {
      vl::Log::error("GL_ARB_framebuffer_object not supported.\n");
      vl::Time::sleep(3000);
      exit(1);
    }

    switch(mTestNum)
    {
    case 0: initTest_FBO_Render_To_Texture(); 
      break;
    case 1: initTest_FBO_Render_To_Texture_MRT(); 
      break;
    case 2: initTest_FBO_Render_To_Color_Buffer_And_Copy_To_Texture(); 
      break;
    case 3: initTest_FBO_Framebuffer_Blit_Multisample(); 
      break;
    case 4: initTest_RTT_Legacy();
      break;
    default:
      break;
    }

    bindManipulators( mMainRendering->camera() );
  }

  void resizeEvent(int w, int h)
  {
    mMainRendering->camera()->viewport()->setWidth ( w );
    mMainRendering->camera()->viewport()->setHeight( h );
    mMainRendering->camera()->setProjectionAsPerspective();
  }

  virtual void updateScene()
  {
    mX  = vl::Time::currentTime() * 45*2;
    mY  = vl::Time::currentTime() * 45*2.1f;
    mX2 = vl::Time::currentTime() * 45*2.2f;
    mY2 = vl::Time::currentTime() * 45*2.3f;
    mX3 = vl::Time::currentTime() * 45*2.4f;

    mTransfRing1->setLocalMatrix( vl::mat4::getRotation(mX,  1,0,0) );
    mTransfRing2->setLocalMatrix( vl::mat4::getRotation(mY,  0,1,0) );
    mTransfRing3->setLocalMatrix( vl::mat4::getRotation(mX2, 1,0,0) );
    mTransfRing4->setLocalMatrix( vl::mat4::getRotation(mY2, 0,1,0) );
    mTransfRing5->setLocalMatrix( vl::mat4::getRotation(mX3, 1,0,0) );
  }

protected:
  int mTestNum;
  vl::Real mX, mY, mX2, mY2, mX3;
  vl::ref<vl::Transform> mTransfRing1;
  vl::ref<vl::Transform> mTransfRing2;
  vl::ref<vl::Transform> mTransfRing3;
  vl::ref<vl::Transform> mTransfRing4;
  vl::ref<vl::Transform> mTransfRing5;
  vl::ref<vl::Rendering> mMainRendering;
  vl::ref<vl::Rendering> mRTT_Rendering;
};

// Have fun!

BaseDemo* Create_App_Framebuffer_Object(int test) { return new App_Framebuffer_Object(test); }
