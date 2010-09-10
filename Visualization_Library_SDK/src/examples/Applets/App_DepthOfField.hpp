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
#include "vl/FramebufferObject.hpp"

// note: this is just an experiment, don't look! :)

namespace vl
{
  class DOFRendering: public RenderingTree
  {
  public:
    DOFRendering()
    {
      mFocalCenter  = 10.0f;
      mDisplacement = 0.02f;
      mDisplacedCamera = new Camera;
      mFBODepthAttachment   = new FBODepthBufferAttachment(DT_DEPTH_COMPONENT);
      mFBOTextureAttachment = new FBOTexture2DAttachment(mFBOTexture.get(), 0, T2DT_TEXTURE_2D);
    }
    void render()
    {
      if ( enableMask() == 0 )
        return;

      dispatchRenderingCallbacks(RC_PreRendering);
      for(int i=0; i<subRenderings()->size(); ++i)
      {
        render(subRenderings()->at(i)->as<Rendering>(), 0); // center
        render(subRenderings()->at(i)->as<Rendering>(), 1); // left
        render(subRenderings()->at(i)->as<Rendering>(), 2); // right
      }
      dispatchRenderingCallbacks(RC_PostRendering);
    }

    void render(Rendering*rendering, int idx)
    {
      if (!rendering)
      {
        vl::Log::error("DOFRendering()::render(): the sub renderings of a DOFRendering must be of class Rendering.\n");
        return;
      }

      // fixme: avoid unnecessary animations, matrix transforms, shader/actor animations/updates etc.
      rendering->setUpdateTime(updateTime());

      // backup camera
      ref<Camera> camera_bk = rendering->camera();
      // copy camera and viewport settings
      *mDisplacedCamera = *camera_bk;
      // install displaced camera
      rendering->setCamera(mDisplacedCamera.get());
      // displace the camera
      mat4 m = camera_bk->inverseViewMatrix();
      vec3 eye  = m.getT();
      if (idx == 1) eye += m.getX()*mDisplacement;
      if (idx == 2) eye -= m.getX()*mDisplacement;
      vec3 look = m.getT() - m.getZ()*mFocalCenter;
      vec3 up   = m.getY();
      mDisplacedCamera->setInverseViewMatrix( mat4::lookAt(eye,look,up) );
      // backup render target
      ref<RenderTarget> target_bk = rendering->renderTarget();
      // update FBO size
      if ( !mFBOTexture || target_bk->width() != mFBOTexture->width() || target_bk->height() != mFBOTexture->height() )
      {
        mFBOTexture = new Texture(target_bk->width(), target_bk->height(), TF_RGBA);
        mFBOTextureAttachment->setTexture(mFBOTexture.get());
        mFBODepthAttachment->setWidth(target_bk->width());
        mFBODepthAttachment->setHeight(target_bk->height());
        mFBORenderTarget->setWidth(target_bk->width());
        mFBORenderTarget->setHeight(target_bk->height());
      }

      int w = rendering->renderTarget()->width();
      int h = rendering->renderTarget()->height();

      if (idx)
      {
        mDisplacedCamera->viewport()->setWidth (w/2);
        mDisplacedCamera->viewport()->setHeight(h/2);
      }
      else
      {
        mDisplacedCamera->viewport()->setWidth (w);
        mDisplacedCamera->viewport()->setHeight(h);
      }

      // install texture render target
      rendering->setRenderTarget(mFBORenderTarget.get());
      // render
      rendering->render();

      // restore original camera
      rendering->setCamera(camera_bk.get());
      // resetore render target
      rendering->setRenderTarget(target_bk.get());

      // copy texture render to DOF render target
      glViewport(0,0,rendering->renderTarget()->width(),rendering->renderTarget()->height());
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      gluOrtho2D(-0.5f,w-0.5f,-0.5f,h-0.5f);
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();

      if(idx == 0)
      {
        glClearColor(1.0f,0.0f,1.0f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_BLEND);
      }
      else
        glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glBindTexture(GL_TEXTURE_2D, mFBOTexture->handle());
      glEnable(GL_TEXTURE_2D);
      fvec2 vquad[] =
      {
        fvec2( -0.5f,  -0.5f),
        fvec2(w-0.5f,  -0.5f),
        fvec2(w-0.5f, h-0.5f),
        fvec2( -0.5f, h-0.5f),
      };
      fvec2 tquad[] = 
      {
        fvec2(   0, 0),
        fvec2(0.5f, 0),
        fvec2(0.5f, 0.5f),
        fvec2(   0, 0.5f),
      };
      if (idx == 0)
        for(int i=0; i<4; ++i) tquad[i] *= 2.0f;

      fvec4 cquad[] = 
      {
        fvec4(1.0f,1.0f,1.0f,0.5f),
        fvec4(1.0f,1.0f,1.0f,0.5f),
        fvec4(1.0f,1.0f,1.0f,0.5f),
        fvec4(1.0f,1.0f,1.0f,0.5f),
      };
      if (idx == 2)
        for(int i=0; i<4; ++i) cquad[i].a() = 0.3333f;

      glEnableClientState(GL_VERTEX_ARRAY);
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      glEnableClientState(GL_COLOR_ARRAY);
      glVertexPointer  ( 2, GL_FLOAT, 0, vquad[0].ptr() );
      glTexCoordPointer( 2, GL_FLOAT, 0, tquad[0].ptr() );
      glColorPointer   ( 4, GL_FLOAT, 0, cquad[0].ptr() );
        glDrawArrays(GL_QUADS, 0, 4);
      glDisableClientState(GL_VERTEX_ARRAY);
      glDisableClientState(GL_TEXTURE_COORD_ARRAY);
      glDisableClientState(GL_COLOR_ARRAY);

      glBindTexture(GL_TEXTURE_2D, 0);
      glDisable(GL_TEXTURE_2D);
      glDisable(GL_BLEND);
      VL_CHECK_OGL();
    }

    float focalCenter() const { return mFocalCenter; }
    void setFocalCenter(float fcenter) { mFocalCenter = fcenter; }

    float displacement() const { return mDisplacement; }
    void setDisplacement(float disp) { mDisplacement = disp; }

    void setRenderTarget(RenderTarget* rt) { mRenderTarget = rt; }
    RenderTarget* renderTarget() { return mRenderTarget.get(); }
    const RenderTarget* renderTarget() const { return mRenderTarget.get(); }

    void setFBORenderTarget(FBORenderTarget* fbo_rt) 
    { 
      fbo_rt->removeAllAttachments();
      mFBORenderTarget = fbo_rt; 
      mFBORenderTarget->addDepthAttachment( mFBODepthAttachment.get() );
      mFBORenderTarget->addTextureAttachment( AP_COLOR_ATTACHMENT0, mFBOTextureAttachment.get() );
      mFBORenderTarget->setDrawBuffer( RDB_COLOR_ATTACHMENT0 );
    }
    FBORenderTarget* fboRenderTarget() { return mFBORenderTarget.get(); }
    const FBORenderTarget* fboRenderTarget() const { return mFBORenderTarget.get(); }

  protected:
    float mFocalCenter;
    float mDisplacement;
    ref<RenderTarget> mRenderTarget;
    ref<Camera> mDisplacedCamera;
    // FBO stuff
    ref<FBORenderTarget> mFBORenderTarget;
    ref<FBODepthBufferAttachment> mFBODepthAttachment;
    ref<FBOTexture2DAttachment> mFBOTextureAttachment;
    ref<Texture> mFBOTexture;
  };
}

class App_DepthOfField: public BaseDemo
{
public:
  vl::ref<vl::DOFRendering> dof_rendering;
  void initEvent()
  {
    BaseDemo::initEvent();
    generateScene();

    dof_rendering = new vl::DOFRendering;
    dof_rendering->subRenderings()->push_back(vl::VisualizationLibrary::rendering());
    vl::VisualizationLibrary::setRendering(dof_rendering.get());
    dof_rendering->setFBORenderTarget(openglContext()->createFBORenderTarget().get());
  }

  void mouseWheelEvent(int a)
  {
    dof_rendering->setFocalCenter( dof_rendering->focalCenter() + a );
    if (dof_rendering->focalCenter() < 1.0f)
      dof_rendering->setFocalCenter( 1.0f );
    printf("dof_rendering->focalCenter() = %f\n", dof_rendering->focalCenter());
  }

  void generateScene()
  {
    const float size  = 100.0f;
    const int   objects = 25000;

    vl::ref<vl::Effect> house_fx = new vl::Effect;
    house_fx->shader()->gocLight(0);
    house_fx->shader()->gocLight(0)->setLinearAttenuation(0.025f/4.0f);
    house_fx->shader()->gocMaterial()->setColorMaterialEnabled(true);
    house_fx->shader()->enable(vl::EN_LIGHTING);
    house_fx->shader()->enable(vl::EN_DEPTH_TEST);

    vl::ref<vl::Geometry> ground = vlut::makeGrid( vl::vec3(size/2,0,size/2), size, size, 50, 50 );
    ground->setVBOEnabled(false);
    ground->computeNormals();
    sceneManager()->tree()->addActor(ground.get(),house_fx.get(),NULL);

    for(int i=0; i<objects; ++i)
    {
      float th = 6;
      float h = (float)vl::randomMinMax(1.0f, th);
      float w = (float)vl::randomMinMax(1.0f, 2.0f);
      float d = (float)vl::randomMinMax(1.0f, 2.0f);
      vl::ref<vl::Geometry> geom = vlut::makeBox(vl::vec3(vl::randomMinMax(0.0f,size),h/2.0f,vl::randomMinMax(0.0f,size)), w,h,d);
      geom->setVBOEnabled(false);
      geom->setDisplayListEnabled(false);
      geom->computeNormals();
      geom->setColorArray( vl::fvec4((float)vl::randomMinMax(0.0f,1.0f),(float)vl::randomMinMax(0.0f,1.0f),(float)vl::randomMinMax(0.0f,1.0f),1.0f ) );
      sceneManager()->tree()->addActor(geom.get(),house_fx.get(),NULL);
    }
  }
};

// Have fun!
