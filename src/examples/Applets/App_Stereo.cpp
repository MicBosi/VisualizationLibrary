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

#include "BaseDemo.hpp"
#include <vlGraphics/GeometryPrimitives.hpp>
#include <vlGraphics/Light.hpp>
#include <vlGraphics/RenderingTree.hpp>
#include <vlGraphics/Rendering.hpp>
#include <vlGraphics/GLSL.hpp>

using namespace vl;

class App_Stereo: public BaseDemo
{
public:
  virtual String appletInfo()
  {
    return BaseDemo::appletInfo();
  }

  void updateCameras()
  {
    // viewports: no need to update them as they are shared

    //mMonoCamera->setFOV(45);
    //mMonoCamera->setNearPlane(.75);

    //mMonoCamera->setProjectionPerspective();
    //mLeftCamera->setProjectionPerspective();
    //mRightCamera->setProjectionPerspective();

    float aspect_ratio = (float)mMonoCamera->viewport()->width()/mMonoCamera->viewport()->height();
    float fov = mMonoCamera->fov()*fDEG_TO_RAD;
    float near_clip = mMonoCamera->nearPlane();
    float far_clip =  mMonoCamera->farPlane();
    float convergence = 20;
    float eye_separation = 1;

    float top, bottom, left, right, a, b, c;

    //top = near_clip * tan(fov/2);
    //bottom = -top;
    //a = aspect_ratio * tan(fov/2) * convergence;
    //b = a - eye_separation/2;
    //c = a + eye_separation/2;
    
    float radians = fov/2;
    float wd2 = near_clip * tan(radians);
    float ndfl = near_clip / convergence;
    top    =   wd2;
    bottom = - wd2;
    left  = - aspect_ratio * wd2 - 0.5 * eye_separation * ndfl;
    right =   aspect_ratio * wd2 - 0.5 * eye_separation * ndfl;

    // mic fixme
    printf("%f %f %f %f\n", left, right, bottom, top);

    // left
    //left = -b * near_clip/convergence;
    //right = c * near_clip/convergence;
    mLeftCamera->setProjectionFrustum(left, right, bottom, top, near_clip, far_clip);
    mLeftCamera->setViewMatrix( mMonoCamera->viewMatrix()*mat4::getTranslation(-eye_separation/2, 0, 0) );

    left  = - aspect_ratio * wd2 + 0.5 * eye_separation * ndfl;
    right =   aspect_ratio * wd2 + 0.5 * eye_separation * ndfl;

    // right
    //left = -c * near_clip/convergence;
    //right = b * near_clip/convergence;
    mRightCamera->setProjectionFrustum(left, right, bottom, top, near_clip, far_clip);
    mRightCamera->setViewMatrix( mMonoCamera->viewMatrix()*mat4::getTranslation(+eye_separation/2, 0, 0) );
  }

  void updateScene()
  {
    updateCameras();
  }

  void initEvent()
  {
    vl::Log::notify(appletInfo());

    // save opengl context
    OpenGLContext* gl_context = rendering()->as<Rendering>()->renderer()->framebuffer()->openglContext();
    // main camera used to generate the two left and right cameras
    mMonoCamera = rendering()->as<Rendering>()->camera();

    mMainRendering = new vl::RenderingTree;
    mLeftRendering = new vl::Rendering;
    mRightRendering = new vl::Rendering;
    mMainRendering ->subRenderings()->push_back(mLeftRendering.get());
    mMainRendering ->subRenderings()->push_back(mRightRendering.get());
    setRendering(mMainRendering.get());

    // set left/right scene manager
    mLeftRendering->sceneManagers()->push_back(sceneManager());
    mRightRendering->sceneManagers()->push_back(sceneManager());
    
    // save left/right cameras for later
    mLeftCamera = mLeftRendering->camera();
    mRightCamera = mRightRendering->camera();

    // share the same viewport so we have to update it only once
    mLeftCamera->setViewport( mMonoCamera->viewport() );
    mRightCamera->setViewport( mMonoCamera->viewport() );

    // create left and right FBOs
    ref<FramebufferObject> left_fbo = gl_context->createFramebufferObject(gl_context->width(), gl_context->height());
    ref<FramebufferObject> right_fbo = gl_context->createFramebufferObject(gl_context->width(), gl_context->height());

    vl::ref<vl::FBODepthBufferAttachment> fbo_depth_attach = new vl::FBODepthBufferAttachment(vl::DBF_DEPTH_COMPONENT);
    // mic fixme:
    // fbo_depth_attach->setWidth(gl_context->width());
    // fbo_depth_attach->setHeight(gl_context->height());
    left_fbo->addDepthAttachment( fbo_depth_attach.get() );
    right_fbo->addDepthAttachment( fbo_depth_attach.get() );

    ref<Texture> left_texture = new Texture(gl_context->width(), gl_context->height(), TF_RGBA, false);
    left_texture->getTexParameter()->setMagFilter(vl::TPF_LINEAR);
    left_texture->getTexParameter()->setMinFilter(vl::TPF_LINEAR); // mic fixme: must be TPF_NEAREST
    ref<FBOTexture2DAttachment> left_texture_attach = new FBOTexture2DAttachment(left_texture .get(), 0, vl::T2DT_TEXTURE_2D);
    left_fbo->addTextureAttachment(vl::AP_COLOR_ATTACHMENT0, left_texture_attach.get() );
    left_fbo->setDrawBuffer(vl::RDB_COLOR_ATTACHMENT0);

    ref<Texture> right_texture = new Texture(gl_context->width(), gl_context->height(), TF_RGBA, false);
    right_texture->getTexParameter()->setMagFilter(vl::TPF_LINEAR);
    right_texture->getTexParameter()->setMinFilter(vl::TPF_LINEAR); // mic fixme: must be TPF_NEAREST
    ref<FBOTexture2DAttachment> right_texture_attach = new FBOTexture2DAttachment(right_texture .get(), 0, vl::T2DT_TEXTURE_2D);
    right_fbo->addTextureAttachment(vl::AP_COLOR_ATTACHMENT0, right_texture_attach.get() );
    right_fbo->setDrawBuffer(vl::RDB_COLOR_ATTACHMENT0);

    // set left/right framebuffer
    mLeftRendering->renderer()->setFramebuffer(left_fbo.get());
    mRightRendering->renderer()->setFramebuffer(right_fbo.get());

    // mic fixme: 
    // - fai versione senza FBO? con rightFramebuffer()?
    // - fai una versione che usa direttament glColorMask()

    // setup compositing rendering
    mCompositingRendering = new Rendering;
    mCompositingRendering->renderer()->setFramebuffer(gl_context->framebuffer());
    mCompositingRendering->camera()->setModelingMatrix( mat4::getIdentity() );
    mCompositingRendering->camera()->viewport()->set(0, 0, gl_context->width(), gl_context->height());
    mCompositingRendering->camera()->viewport()->setClearColor(vl::royalblue);
    mCompositingRendering->camera()->setProjectionOrtho(0, 1, 0, 1, -1, +1);
    mMainRendering->subRenderings()->push_back(mCompositingRendering.get());
    // setup simple texture quad
    ref<Geometry> quad = new Geometry;
    ref<ArrayFloat3> verts = new ArrayFloat3;
    quad->setVertexArray(verts.get());
    verts->resize(4);
    verts->at(0) = fvec3(0, 0, 0);
    verts->at(1) = fvec3(1, 0, 0);
    verts->at(2) = fvec3(1, 1, 0);
    verts->at(3) = fvec3(0, 1, 0);
    ref<DrawArrays> da = new DrawArrays(PT_QUADS, 0, 4);
    quad->drawCalls()->push_back(da.get());
    // apply texture to quad
    ref<Effect> fx = new Effect;
    fx->shader()->gocTextureSampler(0)->setTexture(left_texture.get());
    fx->shader()->gocTextureSampler(1)->setTexture(right_texture.get());
    fx->shader()->gocGLSLProgram()->attachShader( new GLSLVertexShader("/glsl/stereo.vs") );
    fx->shader()->gocGLSLProgram()->attachShader( new GLSLFragmentShader("/glsl/stereo.fs") );
    fx->shader()->gocGLSLProgram()->gocUniform("left_channel")->setUniformI(0);
    fx->shader()->gocGLSLProgram()->gocUniform("right_channel")->setUniformI(1);
    // add the quad to the compositing rendering
    ref<SceneManagerActorTree> quad_scene_manager = new SceneManagerActorTree;
    quad_scene_manager->tree()->addActor(quad.get(), fx.get(), NULL);
    mCompositingRendering->sceneManagers()->push_back(quad_scene_manager.get());

    // ...
  
    // let the trackball rotate the mono camera
    trackball()->setCamera(mMonoCamera.get());
    trackball()->setTransform(NULL); // just for clarity

    // populates the scene
    setupScene();
  }

  // populates the scene
  void setupScene()
  {
    // setup common states
    ref<Light> camera_light = new Light;
    ref<EnableSet> enables = new EnableSet;
    enables->enable(EN_DEPTH_TEST);
    enables->enable(EN_LIGHTING);

    // red material fx
    ref<Effect> red_fx = new Effect;
    red_fx->shader()->setEnableSet(enables.get());
    red_fx->shader()->gocMaterial()->setDiffuse(gray);
    red_fx->shader()->setRenderState(camera_light.get(), 0);

    // green material fx
    ref<Effect> green_fx = new Effect;
    green_fx->shader()->setEnableSet(enables.get());
    green_fx->shader()->gocMaterial()->setDiffuse(gray);
    green_fx->shader()->setRenderState(camera_light.get(), 0);

    // blue material fx
    ref<Effect> yellow_fx = new Effect;
    yellow_fx->shader()->setEnableSet(enables.get());
    yellow_fx->shader()->gocMaterial()->setDiffuse(gray);
    yellow_fx->shader()->setRenderState(camera_light.get(), 0);

    // add box, cylinder, cone actors to the scene
    //ref<Geometry> geom1 = makeBox     (vec3(-7,0,0),5,5,5);
    //ref<Geometry> geom2 = makeCylinder(vec3(0,0,0), 5,5, 10,2, true, true);
    //ref<Geometry> geom3 = makeCone    (vec3(+7,0,0),5,5, 20, true);

    ref<Geometry> geom1 = makeCylinder(vec3(-7,0,0), 3,10, 10,2, true, true);
    ref<Geometry> geom2 = makeCylinder(vec3(0, 0,0), 3,10, 10,2, true, true);
    ref<Geometry> geom3 = makeCylinder(vec3(+7,0,0), 3,10, 10,2, true, true);

    // needed since we enabled the lighting
    geom1->computeNormals();
    geom2->computeNormals();
    geom3->computeNormals();

    // add the actors to the scene
    sceneManager()->tree()->addActor( geom1.get(), red_fx.get(),    NULL);
    sceneManager()->tree()->addActor( geom2.get(), green_fx.get(),  NULL);
    sceneManager()->tree()->addActor( geom3.get(), yellow_fx.get(), NULL);
  }

  void keyPressEvent(unsigned short ch, EKey key)
  {
    // toggle left/right rendering
    if (key == vl::Key_1)
    {
      mLeftRendering->setEnableMask(0xFFFFFFFF);
      mRightRendering->setEnableMask(0);
    }
    if (key == vl::Key_2)
    {
      mLeftRendering->setEnableMask(0);
      mRightRendering->setEnableMask(0xFFFFFFFF);
    }
    if (key == vl::Key_3)
    {
      mLeftRendering->setEnableMask(0xFFFFFFFF);
      mRightRendering->setEnableMask(0xFFFFFFFF);
    }
  }

  void resizeEvent(int w, int h)
  {
    mMonoCamera->viewport()->setWidth ( w );
    mMonoCamera->viewport()->setHeight( h );
    updateCameras();
  }

  void loadModel(const std::vector<String>& files)
  {
    // resets the scene
    sceneManager()->tree()->actors()->clear();

    for(unsigned int i=0; i<files.size(); ++i)
    {
      ref<ResourceDatabase> resource_db = loadResource(files[i],true);

      if (!resource_db || resource_db->count<Actor>() == 0)
      {
        Log::error("No data found.\n");
        continue;
      }

      std::vector< ref<Actor> > actors;
      resource_db->get<Actor>(actors);
      for(unsigned i=0; i<actors.size(); ++i)
      {
        ref<Actor> actor = actors[i].get();
        // define a reasonable Shader
        actor->effect()->shader()->setRenderState( new Light, 0 );
        actor->effect()->shader()->enable(EN_DEPTH_TEST);
        actor->effect()->shader()->enable(EN_LIGHTING);
        actor->effect()->shader()->gocLightModel()->setTwoSide(true);
        // add the actor to the scene
        sceneManager()->tree()->addActor( actor.get() );
      }
    }

    // position the camera to nicely see the objects in the scene
    trackball()->adjustView( sceneManager(), vec3(0,0,1)/*direction*/, vec3(0,1,0)/*up*/, 1.0f/*bias*/ );
  }

  // laod the files dropped in the window
  void fileDroppedEvent(const std::vector<String>& files) { loadModel(files); }

protected:
  ref<RenderingTree> mMainRendering;
  ref<Rendering> mLeftRendering;
  ref<Rendering> mRightRendering;
  ref<Rendering> mCompositingRendering;
  ref<Camera> mMonoCamera;
  ref<Camera> mLeftCamera;
  ref<Camera> mRightCamera;
};

// Have fun!

BaseDemo* Create_App_Stereo() { return new App_Stereo; }
