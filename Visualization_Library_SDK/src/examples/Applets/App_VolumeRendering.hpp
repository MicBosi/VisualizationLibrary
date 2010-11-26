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

#include "vlVolume/SlicedVolume.hpp"

class App_VolumeRendering: public BaseDemo
{
public:
  virtual void initEvent()
  {
    if (!(GLEW_ARB_shading_language_100||GLEW_VERSION_3_0))
    {
      vl::Log::error("OpenGL Shading Language not supported.\n");
      vl::Time::sleep(3000);
      exit(1);
    }

    BaseDemo::initEvent();

    vl::ref<vl::Effect> vol_fx = new vl::Effect;
    vol_fx->shader()->enable(vl::EN_DEPTH_TEST);
    vol_fx->shader()->enable(vl::EN_BLEND);

    // default GLSL settings
    mGLSL = vol_fx->shader()->gocGLSLProgram();
    mGLSL->attachShader( new vl::GLSLFragmentShader("/glsl/volume_luminance_light.fs") );
    mGLSL->attachShader( new vl::GLSLVertexShader("/glsl/volume_luminance_light.vs") );

    // general test setup
    mUseGLSL      = GLEW_VERSION_2_0 || GLEW_VERSION_3_0;
    mDynamicLight = true; // for GLSL mode only

    // transform and trackball steup
    mVolumeTr = new vl::Transform;
    vl::VisualizationLibrary::rendering()->as<vl::Rendering>()->transform()->addChild( mVolumeTr.get() );
    trackball()->setTransform( mVolumeTr.get() );

    // volume
    vl::AABB volume_box( vl::vec3(-10,-10,-10), vl::vec3(+10,+10,+10) );
    mVolumeAct = new vl::Actor;
    mVolumeAct->setEffect( vol_fx.get() );
    mSlicedVolume = new vlVolume::SlicedVolume;
    mSlicedVolume->bindActor(mVolumeAct.get());
    mSlicedVolume->setSliceCount(1024);

    mVolumeAct->setTransform(mVolumeTr.get());
    mSlicedVolume->setBox(volume_box);
    mVolumeAct->setRenderRank(1);
    sceneManager()->tree()->addActor( mVolumeAct.get() );

    // volume_box outline
    vl::ref<vl::Effect> fx_box = new vl::Effect;
    fx_box->shader()->gocPolygonMode()->set(vl::PM_LINE, vl::PM_LINE);
    fx_box->shader()->enable(vl::EN_DEPTH_TEST);
    vl::ref<vl::Geometry> box_outline = vlut::makeBox(volume_box);
    box_outline->setColor(vlut::red);
    sceneManager()->tree()->addActor( box_outline.get(), fx_box.get(), mVolumeTr.get() );

    // light bulb

    if (mDynamicLight)
    {
      mLight = mSlicedVolume->light();
      mLightTr = new vl::Transform;
      vl::VisualizationLibrary::rendering()->as<vl::Rendering>()->transform()->addChild( mLightTr.get() );
      mLight->followTransform( mLightTr.get() );

      vl::ref<vl::Effect> fx_bulb = new vl::Effect;
      fx_bulb->shader()->enable(vl::EN_DEPTH_TEST);
      vl::ref<vl::Geometry> light_bulb = vlut::makeIcosphere(vl::vec3(0,0,0),1,1);
      sceneManager()->tree()->addActor( light_bulb.get(), fx_bulb.get(), mLightTr.get() );
    }

    // bias text
    mBiasText = new vl::Text;
    mBiasText->setFont( vl::VisualizationLibrary::fontManager()->acquireFont("/font/bitstream-vera/VeraMono.ttf", 12) );
    mBiasText->setAlignment( vl::AlignHCenter | vl::AlignBottom);
    mBiasText->setViewportAlignment( vl::AlignHCenter | vl::AlignBottom );
    mBiasText->translate(0,5,0);
    mBiasText->setBackgroundEnabled(true);
    mBiasText->setBackgroundColor(vl::fvec4(0,0,0,0.75));
    mBiasText->setColor(vlut::white);
    vl::ref<vl::Effect> effect = new vl::Effect;
    effect->shader()->enable(vl::EN_BLEND);
    sceneManager()->tree()->addActor(mBiasText.get(), effect.get());

    // bias uniform
    mVolumeAct->gocUniform("val_threshold")->setUniform(0.5f);
    mAlphaBias = mVolumeAct->getUniform("val_threshold");

    // update alpha bias text
    mouseWheelEvent(0);
    
    setVolume( vl::loadImage("/volume/VLTest.dat") );
  }

  void fileDroppedEvent(const std::vector<vl::String>& files)
  {
    if(files.size() == 1) // if there is one file load it directly
    {      
      if (files[0].endsWith(".dat"))
      {
        vl::ref<vl::Image> vol_img = vl::loadImage(files[0]);
        if (vol_img)
          setVolume(vol_img);
      }
    }
    else // if there is more than one file load all the files and assemble a 3D image
    {      
      // sort files by their name
      std::vector<vl::String> files_sorted = files;
      std::sort(files_sorted.begin(), files_sorted.end());
      // load the files
      std::vector< vl::ref<vl::Image> > images;
      for(unsigned int i=0; i<files_sorted.size(); ++i)
        images.push_back( vl::loadImage(files_sorted[i]) );
      // assemble the volume
      vl::ref<vl::Image> vol_img = vl::assemble3DImage(images);
      // set the volume
      if (vol_img)
        setVolume(vol_img);
    }
  }

  void setVolume(vl::ref<vl::Image> img)
  {
    vl::Effect* vol_fx = mVolumeAct->effect();
    if(img->format() == vl::IF_LUMINANCE)
    {
      if (mUseGLSL) // use GLSL
      {        
        vl::Log::info("IF_LUMINANCE image and GLSL supported: lighting and the transfer function will be computed in realtime.\n");
        vl::ref<vl::Image> trfunc = vl::Image::makeColorSpectrum(128, vlut::black, vlut::blue, vlut::green, vlut::yellow, vlut::red);
        // enables GLSL usage
        // installs GLSLProgram
        vol_fx->shader()->setRenderState(mGLSL.get());
        // installs the transfer function as texture #1
        vol_fx->shader()->gocTextureUnit(1)->setTexture( new vl::Texture( trfunc.get() ) );  
        mVolumeAct->gocUniform("trfunc_delta")->setUniform(0.5f/trfunc->width());    
        mVolumeAct->gocUniform("volume_texunit")->setUniform(0);
        mVolumeAct->gocUniform("trfunc_texunit")->setUniform(1);
        mSlicedVolume->setVolumeImage(img.get(), vol_fx->shader());
        vol_fx->shader()->disable(vl::EN_ALPHA_TEST);
      }
      else // precompute transfer function and illumination
      {
        vl::Log::info("IF_LUMINANCE image and GLSL not supported: transfer function and lighting will be precomputed.\n");
        vl::ref<vl::Image> trfunc = vl::Image::makeColorSpectrum(128, vlut::black, vlut::blue, vlut::green, vlut::yellow, vlut::red);
        vl::ref<vl::Image> volume = vlVolume::SlicedVolume::genRGBAVolume(img.get(), trfunc.get(), vl::fvec3(1.0f,1.0f,0.0f));
        mSlicedVolume->setVolumeImage(volume.get(), vol_fx->shader());
        vol_fx->shader()->enable(vl::EN_ALPHA_TEST);
        vol_fx->shader()->gocAlphaFunc()->set(vl::FU_GEQUAL, 0.3f);
        // disable GLSL usage
        // remove GLSLProgram
        vol_fx->shader()->eraseRenderState(vl::RS_GLSLProgram);
        // remove texture unit #1
        vol_fx->shader()->eraseRenderState(vl::RS_TextureUnit1);
      }
    }
    else
    {
      vl::Log::info("Non IF_LUMINANCE image: not using GLSL.\n");
      // disable GLSL usage
      // remove GLSLProgram
      vol_fx->shader()->eraseRenderState(vl::RS_GLSLProgram);
      // remove texture unit #1
      vol_fx->shader()->eraseRenderState(vl::RS_TextureUnit1);
      mSlicedVolume->setVolumeImage(img.get(), vol_fx->shader());
      vol_fx->shader()->enable(vl::EN_ALPHA_TEST);
      vol_fx->shader()->gocAlphaFunc()->set(vl::FU_GEQUAL, 0.3f);
    }
    
    mAlphaBias->setUniform(0.3f);
    updateText();
    openglContext()->update();
  }

  void updateText()
  {
    float bias = 0.0f;
    mAlphaBias->getUniform(&bias);
    mBiasText->setText(vl::Say("Bias = %n") << bias);
  }

  void mouseWheelEvent(int val)
  {
    float alpha = 0.0f;
    mAlphaBias->getUniform(&alpha);
    alpha += val * 0.01f;
    alpha =  vl::clamp(alpha, 0.0f, 1.0f);
    mAlphaBias->setUniform(alpha);

    // used for non GLSL mode volumes
    mVolumeAct->effect()->shader()->gocAlphaFunc()->set(vl::FU_GEQUAL, alpha);
    
    updateText();
    openglContext()->update();
  }

  virtual void run()
  {
    if (mDynamicLight)
    {
      vl::mat4 mat = vl::mat4::rotation( vl::Time::currentTime()*45, 0,0,1 ) * vl::mat4::translation(20,0,0);
      mLightTr->setLocalMatrix(mat);
    }
  }

  protected:
    vl::ref<vl::Transform> mVolumeTr;
    vl::ref<vl::Transform> mLightTr;
    vl::ref<vl::Uniform> mAlphaBias;
    vl::ref<vl::Text> mBiasText;
    vl::ref<vl::Light> mLight;
    vl::ref<vl::GLSLProgram> mGLSL;
    vl::ref<vl::Actor> mVolumeAct;
    bool mDynamicLight;
    bool mUseGLSL;
    vl::ref<vlVolume::SlicedVolume> mSlicedVolume;
};

// Have fun!
