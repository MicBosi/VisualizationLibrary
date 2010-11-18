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

#include "vl/DistanceLODEvaluator.hpp"
#include "vl/PixelLODEvaluator.hpp"

class App_ShaderLOD: public BaseDemo
{
  int mTestNumber;
public:
  App_ShaderLOD(int test): mTestNumber(test) {}

  virtual void shutdown() {}

  class MyShaderAnimator1: public vl::ShaderAnimator
  {
  public:
    MyShaderAnimator1(): mAnimTime(0) {}
    void updateShader(vl::Shader* shader, vl::Camera* , vl::Real cur_time)
    {
      mAnimTime = cur_time;
      float t = (float)(sin( mAnimTime*vl::fPi )+ 1.0f) / 2.0f;
      vl::fvec4 col = vlut::red*t + vlut::blue*(1-t);
      shader->gocMaterial()->setFrontDiffuse(col);
    }
    vl::Real mAnimTime;
  };

  class MyShaderAnimator2: public vl::ShaderAnimator
  {
  public:
    MyShaderAnimator2(): mAnimTime(0) {}
    void updateShader(vl::Shader* shader, vl::Camera* , vl::Real cur_time)
    {
      mAnimTime = cur_time;
      float t = (float)(sin( mAnimTime*vl::fPi )+ 1.0f) / 2.0f;
      vl::fvec4 col = vlut::yellow*t + vlut::green*(1-t);
      shader->gocMaterial()->setFrontEmission(col*0.6f);
      shader->gocMaterial()->setFrontDiffuse(vlut::white*0.4f);
      shader->gocMaterial()->setFrontAmbient(vlut::black);
      shader->gocMaterial()->setFrontSpecular(vlut::black);
    }
    vl::Real mAnimTime;
  };

  class BlinkShaderAnimator: public vl::ShaderAnimator
  {
  public:
    void updateShader(vl::Shader* shader, vl::Camera*, vl::Real /*cur_time*/)
    {
      int c = (int)( vl::Time::currentTime() * 15.0 ) % 2;
      vl::fvec4 color;
      if (c == 0) color = vlut::gold;
      if (c == 1) color = vlut::red;
      // if (c == 2) color = vlut::green;
      shader->gocMaterial()->setFlatColor( color );
    }
  };

  class TexRotShaderAnimator: public vl::ShaderAnimator
  {
  public:
    void updateShader(vl::Shader* shader, vl::Camera*, vl::Real /*cur_time*/)
    {
      vl::mat4 mat;
      mat.translate(-0.5,-0.5,0.0f);
      mat.rotate( vl::Time::currentTime() * 90, 0, 0, 1 );
      mat.translate(+0.5,+0.5,0.0f);
      shader->gocTextureMatrix(1)->setMatrix( mat );
    }
  };

  class WaveActorAnimator: public vl::ActorAnimator
  {
  public:
    WaveActorAnimator(vl::Actor* actor)
    {
      mAnimTime   = 0;
      mLastUpdate = 0;
      mLastUpdatedLod = -1;

      vl::ref<vl::Geometry> geom;

      // LOD 0
      geom = vlut::makeGrid( vl::vec3(0,0,0), 20, 20, 50, 50 );
      geom->computeNormals();
      geom->setColorArray(vlut::aqua);

      actor->lod(0) = geom.get();

      geom->setVBOEnabled(true);
      if (GLEW_ARB_vertex_buffer_object)
      {
        geom->vertexArray()->gpuBuffer()->setBufferData(vl::GBU_DYNAMIC_DRAW,false);
        geom->normalArray()->gpuBuffer()->setBufferData(vl::GBU_DYNAMIC_DRAW,false);
      }

      // LOD 1
      geom = vlut::makeGrid( vl::vec3(0,0,0), 20, 20, 15, 15 );
      geom->computeNormals();
      geom->setColorArray(vlut::aqua);
      actor->lod(1) = geom.get();

      geom->setVBOEnabled(true);
      if (GLEW_ARB_vertex_buffer_object)
      {
        geom->vertexArray()->gpuBuffer()->setBufferData(vl::GBU_DYNAMIC_DRAW,false);
        geom->normalArray()->gpuBuffer()->setBufferData(vl::GBU_DYNAMIC_DRAW,false);
      }

      // LOD 2
      geom = vlut::makeGrid( vl::vec3(0,0,0), 20, 20, 8, 8 );
      geom->computeNormals();
      geom->setColorArray(vlut::aqua);
      actor->lod(2) = geom.get();

      if (GLEW_ARB_vertex_buffer_object)
      {
        geom->vertexArray()->gpuBuffer()->setBufferData(vl::GBU_DYNAMIC_DRAW,false);
        geom->normalArray()->gpuBuffer()->setBufferData(vl::GBU_DYNAMIC_DRAW,false);
      }
    }

    void updateActor(vl::Actor* actor, int lodi, vl::Camera* , vl::Real cur_time)
    {
      // the beauty of this function is that in a few lines of code
      // we update 3 different LOD levels over 3 different fps rates

      mAnimTime = cur_time;

      // fps rate also follows the LOD
      vl::Real fps = ( 30.0f - lodi * 10.0f );

      if ( cur_time - mLastUpdate > 1.0f/fps || mLastUpdatedLod != lodi )
      {
        mLastUpdate = cur_time;
        mLastUpdatedLod = lodi;

        // note: this returns the current LOD geometry
        vl::ref<vl::Geometry> geom = dynamic_cast<vl::Geometry*>( actor->lod(lodi).get() );
        vl::ref<vl::ArrayFVec3> vecarr3 = dynamic_cast<vl::ArrayFVec3*>( geom->vertexArray() );
        vl::fvec3* vec = (vl::fvec3*)vecarr3->ptr();
        vl::vec3 center = actor->lod(lodi)->boundingBox().center();

        for(size_t i=0; i<vecarr3->size(); ++i)
        {
          vec[i].y() = 0;
          vl::Real d = (vl::vec3(vec[i])-center).length();
          vec[i].y() = (float)cos( -mAnimTime * vl::fPi*1.0f + d*1.5 );
        }
        geom->computeNormals();
        if (GLEW_ARB_vertex_buffer_object)
        {
          geom->vertexArray()->gpuBuffer()->setBufferData(vl::GBU_DYNAMIC_DRAW, false);
          geom->normalArray()->gpuBuffer()->setBufferData(vl::GBU_DYNAMIC_DRAW, false);
        }
        geom->setBoundsDirty(true);
      }
    }

  protected:
    vl::Real mAnimTime;
    vl::Real mLastUpdate;
    int mLastUpdatedLod;
  };

  void run()
  {
    // vl::VisualizationLibrary::rendering()->as<vl::Rendering>()->camera()->setNearFarClippingPlanesOptimized(false);

    if ( mTestNumber == 2 )
      return;
    else
    if ( mTestNumber == 4 )
    {
      vl::Real t = sin( vl::Time::currentTime() * vl::fPi * 2.0f / 8.0f ) * 0.5f + 0.5f;
      vl::Real x = 5+150*t;
      vl::vec3 eye( x, t*20+10, 0 );
      vl::mat4 m;
      m = vl::mat4::lookAt( eye, vl::vec3(0,6,0), vl::vec3(0,1,0) );
      vl::VisualizationLibrary::rendering()->as<vl::Rendering>()->camera()->setInverseViewMatrix(m);
    }
    else
    if ( mTestNumber == 0 )
    {
      vl::Real t = sin( vl::Time::currentTime() * vl::fPi * 2.0f / 5.0f ) * 0.5f + 0.5f;
      vl::Real t2 = sin( vl::Time::currentTime() * vl::fPi * 2.0f / 7.0f ) * 0.5f + 0.5f;
      vl::Real x = 50+30*t2;
      vl::vec3 eye( x, t*5-2.5f, 0 );
      eye = vl::mat4::rotation( vl::Time::currentTime() * 5.0f, 0, 1, 0 ) * eye;
      vl::mat4 m;
      m = vl::mat4::lookAt( eye, vl::vec3(0,0,0), vl::vec3(0,1,0) );
      vl::VisualizationLibrary::rendering()->as<vl::Rendering>()->camera()->setInverseViewMatrix(m);
    }
    else
    {
      vl::Real t = sin( vl::Time::currentTime() * vl::fPi * 2.0f / 7.0f ) * 0.5f + 0.5f;
      vl::Real x = t * 150;
      x += 5;
      vl::vec3 eye( x, 0, 0 );
      eye = vl::mat4::rotation( vl::Time::currentTime() * 30.0f, 0, 1, 0 ) * eye;
      eye += vl::vec3(0,5+90*t,0);
      vl::mat4 m;
      m = vl::mat4::lookAt( eye, vl::vec3(0,0,0), vl::vec3(0,1,0) );
      vl::VisualizationLibrary::rendering()->as<vl::Rendering>()->camera()->setInverseViewMatrix(m);
    }
  }

  void initEvent()
  {
    BaseDemo::initEvent();
    vl::ref<vl::Light> light = new vl::Light(0);

    if (mTestNumber == 0) // alpha multipassing
    {
      int  actor_count = 20;

      vl::ref<vl::Effect> effect = new vl::Effect;

      effect->shader()->enable(vl::EN_DEPTH_TEST);
      effect->shader()->enable(vl::EN_CULL_FACE);
      effect->shader()->enable(vl::EN_LIGHTING);
      effect->shader()->setRenderState( light.get() );
      effect->shader()->enable(vl::EN_BLEND);
      effect->shader()->gocMaterial()->setFrontDiffuse( vl::fvec4(1.0f, 0.0f, 0.0f, 0.6f) );

      vl::ref<vl::Shader> wirepass = new vl::Shader;
      effect->lod(0)->push_back(wirepass.get());

      /* wire pass */
      wirepass->enable(vl::EN_DEPTH_TEST);
      wirepass->enable(vl::EN_CULL_FACE);
      wirepass->enable(vl::EN_LIGHTING);
      wirepass->setRenderState( light.get() );
      wirepass->enable(vl::EN_BLEND);
      wirepass->enable(vl::EN_LINE_SMOOTH);
      wirepass->gocPolygonMode()->set(vl::PM_LINE, vl::PM_LINE);
      wirepass->gocHint()->setLineSmoothHint(vl::HM_NICEST);
      wirepass->gocLineWidth()->set(1.5f);
      wirepass->gocMaterial()->setFlatColor( vlut::green );

      vl::ref<vl::Geometry> geom_0;
      geom_0= vlut::makeIcosphere(vl::vec3(0,0,0), 6, 2);
      geom_0->computeNormals();
      geom_0->setColorArray(vlut::white);

      for(int i=0;i<actor_count; i++)
      {
        vl::ref<vl::Transform> tr = new vl::Transform;
        vl::Real t = 360.0f / actor_count * i;
        vl::vec3 v = vl::mat4::rotation(t, 0,1,0) * vl::vec3(30,0,0);
        tr->setLocalMatrix( vl::mat4::translation(v) );
        vl::VisualizationLibrary::rendering()->as<vl::Rendering>()->transform()->addChild(tr.get());
        sceneManager()->tree()->addActor( geom_0.get(), effect.get(), tr.get() );
      }
    }
    else
    if (mTestNumber == 1) // geometrical LOD
    {
      enum { DISTANCE_LOD, PIXEL_LOD } lod_type = DISTANCE_LOD;
      int actor_count = 20;
      int side = 20;

      vl::ref<vl::Effect> effect = new vl::Effect;

      effect->shader()->enable(vl::EN_DEPTH_TEST);
      effect->shader()->enable(vl::EN_CULL_FACE);
      effect->shader()->enable(vl::EN_LIGHTING);
      effect->shader()->gocMaterial()->setColorMaterialEnabled(true);
      effect->shader()->setRenderState( light.get() );

      vl::ref<vl::Geometry> geom;
      geom= vlut::makeIcosphere( vl::vec3(0,0,0), 2, 3 );
      geom->computeNormals();
      geom->setColorArray(vl::fvec4(0,1,0,1));

      vl::ref<vl::Geometry> geom_B;
      geom_B= vlut::makeIcosphere( vl::vec3(0,0,0), 2, 1 );
      geom_B->computeNormals();
      geom_B->setColorArray(vl::fvec4(1,1,0,1));

      vl::ref<vl::Geometry> geom_C;
      geom_C= vlut::makeIcosphere( vl::vec3(0,0,0), 2, 0 );
      geom_C->computeNormals();
      geom_C->setColorArray(vl::fvec4(1,0,0,1));

      vl::ref<vl::LODEvaluator> lod;

      if (lod_type == DISTANCE_LOD)
      {
        vl::ref<vl::DistanceLODEvaluator> dlod = new vl::DistanceLODEvaluator;
        dlod->addDistanceRange(50);
        dlod->addDistanceRange(150);
        lod = dlod;
      }
      else
      {
        vl::ref<vl::PixelLODEvaluator> plod = new vl::PixelLODEvaluator;
        plod->addPixelRange(250);
        plod->addPixelRange(2500);
        lod = plod;
      }

      for(int i=0;i<actor_count; i++)
      {
        vl::ref<vl::Transform> tr = new vl::Transform;
        tr->setLocalMatrix( vl::mat4::translation(rand()%side - side/2.0f, rand()%side - side/2.0f, rand()%side - side/2.0f) );
        vl::VisualizationLibrary::rendering()->as<vl::Rendering>()->transform()->addChild(tr.get());
        vl::ref<vl::Actor> act = sceneManager()->tree()->addActor( geom.get(), effect.get(), tr.get() );
        act->lod(1) = geom_B.get();
        act->lod(2) = geom_C.get();
        act->setLODEvaluator(lod.get());
      }

    }
    else
    if (mTestNumber == 3) // geometry: multipassing, LOD, animation
    {
      // this test also shows how is possible to synchronize the Geometry LOD with the Sader LOD

      int  actor_count = 20;
      bool geom_lod = true;


      vl::ref<vl::Shader> wire_sh = new vl::Shader;
      vl::ref<vl::Shader> fill_sh = new vl::Shader;

      /* wire pass */
      wire_sh->gocMaterial()->setFlatColor( vlut::royalblue );
      wire_sh->disable(vl::EN_DEPTH_TEST);

      fill_sh->gocPolygonMode()->set(vl::PM_FILL, vl::PM_FILL);
      fill_sh->gocMaterial()->setFrontDiffuse( vlut::white );

      vl::ref<vl::Effect> fx1 = new vl::Effect;
      vl::ref<vl::Effect> fx2 = new vl::Effect;

      fill_sh->enable(vl::EN_DEPTH_TEST);
      fill_sh->enable(vl::EN_CULL_FACE);
      fill_sh->enable(vl::EN_LIGHTING);
      fill_sh->setRenderState( light.get() );

      wire_sh->enable(vl::EN_DEPTH_TEST);
      // wire_sh->enable(vl::EN_CULL_FACE);
      wire_sh->enable(vl::EN_LIGHTING);
      wire_sh->setRenderState( light.get() );
      wire_sh->enable(vl::EN_BLEND);
      wire_sh->gocPolygonMode()->set(vl::PM_LINE, vl::PM_LINE);
      wire_sh->gocLineWidth()->set(1.0f);
      wire_sh->enable(vl::EN_LINE_SMOOTH);
      wire_sh->gocHint()->setLineSmoothHint(vl::HM_NICEST);

      vl::ref<vl::ShaderSequence> pass1 = new vl::ShaderSequence;

      fx2->setLOD( 0, wire_sh.get() );

      fx1->setLOD( 0, fill_sh.get(), wire_sh.get() );
      fx1->setLOD( 1, fill_sh.get() );
      fx1->setLOD( 2, wire_sh.get() );

      vl::ref<vl::Geometry> geom_0;
      geom_0= vlut::makeIcosphere(vl::vec3(0,0,0), 6, 2);
      geom_0->computeNormals();
      geom_0->setColorArray(vlut::white);

      vl::ref<vl::Geometry> geom_1;
      geom_1= vlut::makeBox(vl::vec3(0,0,0), 6,6,6);
      geom_1->computeNormals();
      geom_1->setColorArray(vlut::white);

      vl::ref<vl::Geometry> geom_2;
      geom_2= vlut::makePyramid(vl::vec3(0,0,0), 6, 6);
      geom_2->computeNormals();
      geom_2->setColorArray(vlut::white);

      vl::ref<vl::DistanceLODEvaluator> lod = new vl::DistanceLODEvaluator;
      lod->addDistanceRange(70);
      lod->addDistanceRange(150);
      fx1->setLODEvaluator(lod.get());

      vl::ref<vl::Actor> wave_act = new vl::Actor;
      wave_act->setActorAnimator( new WaveActorAnimator(wave_act.get()) );
      wave_act->setLODEvaluator(lod.get());
      wave_act->setEffect(fx2.get());
      sceneManager()->tree()->addActor(wave_act.get());

      for(int i=0;i<actor_count; i++)
      {
        vl::ref<vl::Transform> tr = new vl::Transform;
        vl::Real t = 360.0f / actor_count * i;
        vl::vec3 v = vl::mat4::rotation(t, 0,1,0) * vl::vec3(20,0,0);
        tr->setLocalMatrix( vl::mat4::translation(v) );
        vl::VisualizationLibrary::rendering()->as<vl::Rendering>()->transform()->addChild(tr.get());
        vl::ref<vl::Actor> act = sceneManager()->tree()->addActor( geom_0.get(), fx1.get(), tr.get() );
        if (geom_lod)
        {
          act->lod(1) = geom_1.get();
          act->lod(2) = geom_2.get();
          act->setLODEvaluator(lod.get());
        }
      }
    }
    else
    if (mTestNumber == 4) // shader: multipassing + LOD + animation
    {
      vl::ref<vl::Effect> effect = new vl::Effect;

      vl::ref<vl::Texture> texture;
      vl::ref<vl::Shader> tex_rot_shader1 = new vl::Shader;
      tex_rot_shader1->setShaderAnimator( new TexRotShaderAnimator );
      texture = new vl::Texture;
      texture->setupTexture2D("/images/holebox.tif");
      tex_rot_shader1->gocTextureUnit(0)->setTexture( texture.get() );
      texture = new vl::Texture;
      texture->setupTexture2D("/images/star2.tif");
      tex_rot_shader1->gocTextureUnit(1)->setTexture( texture.get() );
      tex_rot_shader1->gocTextureUnit(1)->texture()->getTexParameter()->setWrapS(vl::TPW_REPEAT);
      tex_rot_shader1->gocTextureUnit(1)->texture()->getTexParameter()->setWrapT(vl::TPW_REPEAT);

      vl::ref<vl::Shader> tex_rot_shader2 = new vl::Shader;
      tex_rot_shader2->setShaderAnimator(new TexRotShaderAnimator);
      texture = new vl::Texture;
      texture->setupTexture2D("/images/holebox.tif");
      tex_rot_shader2->gocTextureUnit(0)->setTexture( texture.get() );

      vl::ref<vl::Shader> blink = new vl::Shader;
      blink->setShaderAnimator(new BlinkShaderAnimator);
      blink->gocPolygonMode()->set(vl::PM_LINE,vl::PM_LINE);
      blink->gocFrontFace()->set(vl::FF_CW);
      blink->gocLineWidth()->set(3.0f);

      tex_rot_shader1->enable(vl::EN_DEPTH_TEST);
      tex_rot_shader1->enable(vl::EN_CULL_FACE);
      tex_rot_shader1->enable(vl::EN_LIGHTING);
      tex_rot_shader1->setRenderState( light.get() );

      blink->enable(vl::EN_DEPTH_TEST);
      blink->enable(vl::EN_CULL_FACE);
      blink->enable(vl::EN_LIGHTING);
      blink->setRenderState( light.get() );

      tex_rot_shader2->enable(vl::EN_DEPTH_TEST);
      tex_rot_shader2->enable(vl::EN_CULL_FACE);
      tex_rot_shader2->enable(vl::EN_LIGHTING);
      tex_rot_shader2->setRenderState( light.get() );

      effect->setLOD( 0, tex_rot_shader1.get(), blink.get() );
      effect->setLOD( 1, tex_rot_shader2.get(), blink.get() );
      effect->setLOD( 2, tex_rot_shader2.get() );

      vl::ref<vl::DistanceLODEvaluator> lod = new vl::DistanceLODEvaluator;
      lod->addDistanceRange(60);
      lod->addDistanceRange(100);
      effect->setLODEvaluator(lod.get());

      vl::ref<vl::Geometry> box = vlut::makeBox( vl::vec3(0,0,0), 5,5,5);
      box->computeNormals();
      //if (GLEW_ARB_multitexture)
        box->setTexCoordArray(1, box->texCoordArray(0));

      for(int i=0; i<24; ++i)
      {
        vl::ref<vl::Transform> tr = new vl::Transform;
        vl::VisualizationLibrary::rendering()->as<vl::Rendering>()->transform()->addChild(tr.get());
        vl::mat4 mat;
        mat.translate(30,0,0);
        mat.rotate( 360.0f / 20.0f * i, 0, 1, 0);
        tr->setLocalMatrix( mat );
        sceneManager()->tree()->addActor(box.get(), effect.get(), tr.get());
      }

      sceneManager()->tree()->addActor(box.get(), effect.get());
    }
  }
};

// Have fun!
