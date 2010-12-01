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
#include "vlut/GeometryPrimitives.hpp"
#include "vl/RenderingTree.hpp"
#include "vl/CopyTexSubImage.hpp"
#include "vl/BlitFramebuffer.hpp"
#include "vl/SceneManager.hpp"
#include "vl/GLSL.hpp"
#include "vl/Light.hpp"

class App_FBO_MRT_GLSL: public BaseDemo
{
public:
  App_FBO_MRT_GLSL(int test): mTestNum(test) {}

  virtual void shutdown() {}

  virtual void run()
  {
    mX  = vl::Time::currentTime() * 45*2;
    mY  = vl::Time::currentTime() * 45*2.1f;
    mX2 = vl::Time::currentTime() * 45*2.2f;
    mY2 = vl::Time::currentTime() * 45*2.3f;
    mX3 = vl::Time::currentTime() * 45*2.4f;

    mTransfRing1->setLocalMatrix( vl::mat4::rotation(mX,  1,0,0) );
    mTransfRing2->setLocalMatrix( vl::mat4::rotation(mY,  0,1,0) );
    mTransfRing3->setLocalMatrix( vl::mat4::rotation(mX2, 1,0,0) );
    mTransfRing4->setLocalMatrix( vl::mat4::rotation(mY2, 0,1,0) );
    mTransfRing5->setLocalMatrix( vl::mat4::rotation(mX3, 1,0,0) );
  }

  virtual void initEvent()
  {
    BaseDemo::initEvent();

    if (!(GLEW_EXT_framebuffer_object||GLEW_ARB_framebuffer_object||GLEW_VERSION_3_0))
    {
      vl::Log::error("GL_ARB_framebuffer_object not supported.\n");
      vl::Time::sleep(3000);
      exit(1);
    }

    int fbo_size = 1024;
    vl::ref< vl::RenderTarget> opengl_window = vl::VisualizationLibrary::rendering()->as<vl::Rendering>()->renderer()->renderTarget();

    vl::ref<vl::RenderingTree> render_tree = new vl::RenderingTree;
    vl::VisualizationLibrary::setRendering(render_tree.get());
    mMainRendering = new vl::Rendering;
    mRTT_Rendering = new vl::Rendering;
    render_tree->subRenderings()->push_back(mRTT_Rendering.get());
    render_tree->subRenderings()->push_back(mMainRendering.get());
    mMainRendering->sceneManagers()->push_back(new vl::SceneManagerActorTree);
    mRTT_Rendering->sceneManagers()->push_back(new vl::SceneManagerActorTree);

    // 1st rendering

    mRTT_Rendering->camera()->viewport()->setClearColor( vlut::crimson );
    mRTT_Rendering->camera()->viewport()->setX(0);
    mRTT_Rendering->camera()->viewport()->setY(0);
    mRTT_Rendering->camera()->viewport()->setWidth(fbo_size);
    mRTT_Rendering->camera()->viewport()->setHeight(fbo_size);
    mRTT_Rendering->camera()->setProjectionAsPerspective();
    vl::mat4 m = vl::mat4::lookAt(vl::vec3(0,0,10.5f), vl::vec3(0,0,0), vl::vec3(0,1,0));
    mRTT_Rendering->camera()->setInverseViewMatrix(m);

    #if 1
      vl::ref<vl::FBODepthBufferAttachment> fbodepth = new vl::FBODepthBufferAttachment(vl::DT_DEPTH_COMPONENT);
    #else
      vl::ref<vl::FBODepthStencilBufferAttachment> fbodepth = new vl::FBODepthStencilBufferAttachment(vl::DST_DEPTH32F_STENCIL8);
    #endif

    vl::ref<vl::FBORenderTarget> fborendertarget = openglContext()->createFBORenderTarget(fbo_size, fbo_size);
    if ( mTestNum == 3 )
      fbodepth->setSamples(2);
    fborendertarget->addDepthAttachment( fbodepth.get() );
    mRTT_Rendering->renderer()->setRenderTarget( fborendertarget.get() );

    // 2nd rendering

    /* setup camera for 2nd rendering */
    vl::ref<vl::Camera> camera2 = new vl::Camera;
    camera2->viewport()->setClearColor( vlut::midnightblue );
    m = vl::mat4::lookAt(vl::vec3(0,15,25), vl::vec3(0,0,0), vl::vec3(0,1,0));
    camera2->setInverseViewMatrix(m);
    /* setup the 2nd rendering components */
    vl::ref<vl::Renderer> renderer2 = new vl::Renderer;
    mMainRendering->setRenderer( renderer2.get() );
    mMainRendering->setCamera( camera2.get() );
    mMainRendering->sceneManagers()->push_back( new vl::SceneManagerActorTree );
    /* pass the default render target to the 2nd rendering, the first will use FBO */
    mMainRendering->renderer()->setRenderTarget( opengl_window.get() );

    vl::ref<vl::Texture> texture1, texture2;
    vl::ref<vl::GLSLProgram> glsl;

    /* FBO rendert to texture */
    if( mTestNum == 0 )
    {
      texture1 = new vl::Texture(fbo_size,fbo_size,vl::TF_RGBA);
      texture2 = NULL; // just for clarity
      vl::ref<vl::FBOTexture2DAttachment> fbotexture1 = new vl::FBOTexture2DAttachment(texture1.get(), 0, vl::T2DT_TEXTURE_2D);
      fborendertarget->addTextureAttachment( vl::AP_COLOR_ATTACHMENT0, fbotexture1.get() );
      mRTT_Rendering->renderer()->renderTarget()->setDrawBuffer( vl::RDB_COLOR_ATTACHMENT0 );
    }
    else
    /* FBO render to texture MRT (multiple render target) */
    if( mTestNum == 1 )
    {
      glsl = new vl::GLSLProgram;

      glsl->attachShader( new vl::GLSLVertexShader("/glsl/perpixellight.vs") );
      if (GLEW_EXT_gpu_shader4||GLEW_VERSION_3_0)
      {
        vl::Log::info("Using glBindFragDataLocation()\n");
        glsl->attachShader( new vl::GLSLFragmentShader("/glsl/perpixellight_cartoon_mrt.fs") );
        glsl->bindFragDataLocation(0, "FragDataOutputA");
        glsl->bindFragDataLocation(1, "FragDataOutputB");
      }
      else
      {
        vl::Log::info("glBindFragDataLocation() not supported.\n");
        glsl->attachShader( new vl::GLSLFragmentShader("/glsl/perpixellight_cartoon_mrt.fs") );
      }

      texture1 = new vl::Texture(fbo_size,fbo_size,vl::TF_RGBA8);
      texture2 = new vl::Texture(fbo_size,fbo_size,vl::TF_RGBA8);
      vl::ref<vl::FBOTexture2DAttachment> fbotexture1 = new vl::FBOTexture2DAttachment(texture1.get(), 0, vl::T2DT_TEXTURE_2D);
      fborendertarget->addTextureAttachment( vl::AP_COLOR_ATTACHMENT0, fbotexture1.get() );

      vl::ref<vl::FBOTexture2DAttachment> fbotexture2 = new vl::FBOTexture2DAttachment(texture2.get(), 0, vl::T2DT_TEXTURE_2D);
      fborendertarget->addTextureAttachment( vl::AP_COLOR_ATTACHMENT1, fbotexture2.get() );
      mRTT_Rendering->renderer()->renderTarget()->setDrawBuffers(vl::RDB_COLOR_ATTACHMENT0, vl::RDB_COLOR_ATTACHMENT1);

      /* screen shot grabbing on attachment 0 */
      vl::ref<vl::ReadPixels> read_pixels_0 = new vl::ReadPixels;
      read_pixels_0->setup( 0, 0, fbo_size, fbo_size, vl::RDB_COLOR_ATTACHMENT0, false );
      mRTT_Rendering->onFinishedCallbacks()->push_back(read_pixels_0.get());
      read_pixels_0->setRemoveAfterCall(true);
      read_pixels_0->setSavePath("MRT-COLOR_ATTACHMENT0.tif");

      /* screen shot grabbing on attachment 1 */
      vl::ref<vl::ReadPixels> read_pixels_1 = new vl::ReadPixels;
      read_pixels_1->setup( 0, 0, fbo_size, fbo_size, vl::RDB_COLOR_ATTACHMENT1, false );
      mRTT_Rendering->onFinishedCallbacks()->push_back(read_pixels_1.get());
      read_pixels_1->setRemoveAfterCall(true);
      read_pixels_1->setSavePath("MRT-COLOR_ATTACHMENT1.tif");
    }
    else
    /* FBO rendert to color buffer + copy to texture */
    if ( mTestNum == 2)
    {
      texture1 = new vl::Texture(fbo_size, fbo_size, vl::TF_RGBA8, false); // note that mipmapping is off
      texture2 = NULL;
      vl::ref<vl::FBOColorBufferAttachment> fbocolor = new vl::FBOColorBufferAttachment(vl::CBF_RGBA8);
      fborendertarget->addColorAttachment( vl::AP_COLOR_ATTACHMENT0, fbocolor.get() );
      vl::ref<vl::CopyTexSubImage2D> copytex = new vl::CopyTexSubImage2D(0, 0,0, 0,0, fbo_size,fbo_size, texture1.get(), vl::T2DT_TEXTURE_2D, vl::RDB_COLOR_ATTACHMENT0);
      mRTT_Rendering->onFinishedCallbacks()->push_back(copytex.get());
      mRTT_Rendering->renderer()->renderTarget()->setDrawBuffer(vl::RDB_COLOR_ATTACHMENT0);
    }
    else
    /* FBO framebuffer blit/multisample */
    if( mTestNum == 3 )
    {
      if (!GLEW_EXT_framebuffer_multisample && !GLEW_EXT_framebuffer_blit)
      {
        vl::Log::error("GL_EXT_framebuffer_multisample or GLEW_EXT_framebuffer_blit not supported.\n");
        vl::Time::sleep(3000);
        exit(1);
      }

      vl::ref<vl::FBOColorBufferAttachment> fbocolor = new vl::FBOColorBufferAttachment(vl::CBF_RGBA);
      fbocolor->setSamples(2);
      fborendertarget->addColorAttachment( vl::AP_COLOR_ATTACHMENT0, fbocolor.get() );
      mRTT_Rendering->renderer()->renderTarget()->setDrawBuffer(vl::RDB_COLOR_ATTACHMENT0);

      // create a new FBO with 'texture1' as its color attachment

      vl::ref<vl::FBORenderTarget> fborendertarget2 = openglContext()->createFBORenderTarget(fbo_size, fbo_size);

      texture1 = new vl::Texture(fbo_size,fbo_size,vl::TF_RGBA);
      texture2 = NULL; // just for clarity
      vl::ref<vl::FBOTexture2DAttachment> fbotexture1 = new vl::FBOTexture2DAttachment(texture1.get(), 0, vl::T2DT_TEXTURE_2D);
      fborendertarget2->addTextureAttachment( vl::AP_COLOR_ATTACHMENT0, fbotexture1.get() );

      // BlitFramebuffer
      vl::ref<vl::BlitFramebuffer> blit_fbo = new vl::BlitFramebuffer;
      mRTT_Rendering->onFinishedCallbacks()->push_back(blit_fbo.get());
      blit_fbo->setLinearFilteringEnabled(false);
      blit_fbo->setBufferMask( vl::BB_COLOR_BUFFER_BIT );
      //the commented part does not work with multisampling enabled
      //int center = fbo_size / 2;
      //int side   = fbo_size / 4;
      //blit_fbo->setSrcRect( center-side, center-side, center+side, center+side );
      blit_fbo->setSrcRect( 0, 0, fbo_size, fbo_size );
      blit_fbo->setDstRect( 0, 0, fbo_size, fbo_size );
      blit_fbo->setReadFramebuffer( fborendertarget.get() );
      blit_fbo->setDrawFramebuffer( fborendertarget2.get() );

    }

    addRings(glsl.get());
    addCube(texture1.get(), texture2.get() );

    bindManipulators( mMainRendering.get() );
  }

  void addCube(vl::Texture* texture1, vl::Texture* texture2)
  {
    vl::ref<vl::Effect> effect = new vl::Effect;
    vl::ref<vl::Light> light = new vl::Light(0);
    light->setAmbient( vl::fvec4( .1f, .1f, .1f, 1.0f) );
    light->setSpecular( vl::fvec4( .1f, .1f, .1f, 1.0f) );
    effect->shader()->setRenderState( light.get() );
    effect->shader()->enable(vl::EN_LIGHTING);
    effect->shader()->enable(vl::EN_DEPTH_TEST);
    effect->shader()->gocLightModel()->setTwoSide(true);
    // effect->shader()->enable(vl::EN_CULL_FACE);

    effect->shader()->gocTextureUnit(0)->setTexture( texture1 );
    effect->shader()->gocTexEnv(0)->setMode(vl::TEM_MODULATE);

    vl::Real size = 50;
    vl::ref<vl::Geometry> ground;
    ground= vlut::makeGrid( vl::vec3(0,0,0), size*1.1f, size*1.1f, 2, 2, true, vl::fvec2(0,0), vl::fvec2(1,1) );
    ground->computeNormals();
    vl::ref<vl::Actor> ground_act = new vl::Actor(ground.get());
    mMainRendering->sceneManagers()->at(0)->as<vl::SceneManagerActorTree>()->tree()->addActor( ground_act.get() );
    ground_act->setEffect(effect.get());

    vl::ref<vl::Geometry> box1;
    if (texture2)
      box1= vlut::makeBox( vl::vec3(-7,5,0), 10,10,10);
    else
      box1= vlut::makeBox( vl::vec3(0,5,0), 10,10,10);
    box1->computeNormals();

    vl::ref<vl::Actor> box1_act = new vl::Actor(box1.get());
    mMainRendering->sceneManagers()->at(0)->as<vl::SceneManagerActorTree>()->tree()->addActor( box1_act.get() );
    box1_act->setEffect(effect.get());

    if (texture2)
    {
      vl::ref<vl::Effect> effect2 = new vl::Effect;
      effect2->shader()->setEnableSet( effect->shader()->gocEnableSet() );
      effect2->shader()->setRenderState( light.get() );
      effect2->shader()->gocTextureUnit(0)->setTexture( texture2 );
      effect2->shader()->gocTexEnv(0)->setMode(vl::TEM_MODULATE);

      vl::ref<vl::Geometry> box2;
      box2= vlut::makeBox( vl::vec3(+7,5,0), 10,10,10);
      box2->computeNormals();

      vl::ref<vl::Actor> box2_act = new vl::Actor(box2.get());
      mMainRendering->sceneManagers()->at(0)->as<vl::SceneManagerActorTree>()->tree()->addActor( box2_act.get() );
      box2_act->setEffect(effect2.get());
    }
  }

  vl::Actor* addActor(vl::Renderable* renderable, vl::Transform* tr, vl::Effect* eff, vl::Rendering* rendering)
  {
    return rendering->sceneManagers()->at(0)->as<vl::SceneManagerActorTree>()->tree()->addActor(renderable, eff, tr);
  }

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
    effect->shader()->gocMaterial()->setDiffuse( vlut::yellow );

    mTransfRing1 = new vl::Transform;
    vl::ref<vl::Geometry> ring1;
    ring1 = vlut::makeTorus( vl::vec3(0,0,0), 10,0.5, 20,100);
    ring1->computeNormals();
    addActor(ring1.get(), mTransfRing1.get(), effect.get(), mRTT_Rendering.get());

    mTransfRing2 = new vl::Transform;
    vl::ref<vl::Geometry> ring2;
    ring2= vlut::makeTorus( vl::vec3(0,0,0), 9,0.5, 20,100);
    ring2->computeNormals();
    addActor(ring2.get(), mTransfRing2.get(), effect.get(), mRTT_Rendering.get());

    mTransfRing3 = new vl::Transform;
    vl::ref<vl::Geometry> ring3;
    ring3= vlut::makeTorus( vl::vec3(0,0,0), 8,0.5, 20,100);
    ring3->computeNormals();
    addActor(ring3.get(), mTransfRing3.get(), effect.get(), mRTT_Rendering.get());

    mTransfRing4 = new vl::Transform;
    vl::ref<vl::Geometry> ring4;
    ring4= vlut::makeTorus( vl::vec3(0,0,0), 7,0.5, 20,100);
    ring4->computeNormals();
    addActor(ring4.get(), mTransfRing4.get(), effect.get(), mRTT_Rendering.get());

    mTransfRing5 = new vl::Transform;
    vl::ref<vl::Geometry> ring5;
    ring5= vlut::makeTorus( vl::vec3(0,0,0), 6,0.5, 20,100);
    ring5->computeNormals();
    addActor(ring5.get(), mTransfRing5.get(), effect.get(), mRTT_Rendering.get());

    mRTT_Rendering->transform()->addChild(mTransfRing1.get());
    mTransfRing1->addChild(mTransfRing2.get());
    mTransfRing2->addChild(mTransfRing3.get());
    mTransfRing3->addChild(mTransfRing4.get());
    mTransfRing4->addChild(mTransfRing5.get());
  }

  void resizeEvent(int w, int h)
  {
    vl::Camera* camera = mMainRendering->camera();
    camera->viewport()->setWidth ( w );
    camera->viewport()->setHeight( h );
    camera->setProjectionAsPerspective();
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

BaseDemo* Create_App_FBO_MRT_GLSL(int test) { return new App_FBO_MRT_GLSL(test); }
