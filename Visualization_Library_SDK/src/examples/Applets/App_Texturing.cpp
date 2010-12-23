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
#include "vl/Array.hpp"
#include "vl/Light.hpp"
#include "vl/GLSL.hpp"

/* 
 * You can find the documentatio for this example in the offical documentation at:
 * Quick Start Guides -> Texturing
 */

class App_Texturing: public BaseDemo
{
public:
  void multitexturing()
  {
    if (!vl::Has_GL_ARB_multitexture)
    {
      vl::Log::error("Multitexturing not supported.\n");
      return;
    }

    vl::ref<vl::Geometry> box = vlut::makeBox( vl::vec3(0,0,0), 5,5,5, true );
    box->computeNormals();
    box->setTexCoordArray(1, box->texCoordArray(0));

    mCubeRightTransform = new vl::Transform;
    mCubeLeftTransform = new vl::Transform;
    vl::VisualizationLibrary::rendering()->as<vl::Rendering>()->transform()->addChild(mCubeRightTransform.get());
    vl::VisualizationLibrary::rendering()->as<vl::Rendering>()->transform()->addChild(mCubeLeftTransform.get());

    vl::ref<vl::Image> img_holebox = vl::loadImage("/images/holebox.tif");
    vl::ref<vl::Image> img_detail  = vl::loadImage("/images/detail.tif");

    vl::ref<vl::Texture> tex_holebox = new vl::Texture(img_holebox.get(), vl::TF_RGBA, mMipmappingOn, false);
    vl::ref<vl::Texture> tex_detail  = new vl::Texture(img_detail.get(),  vl::TF_RGBA, mMipmappingOn, false);

    tex_holebox->getTexParameter()->setMagFilter(vl::TPF_LINEAR);
    tex_holebox->getTexParameter()->setMinFilter(vl::TPF_LINEAR_MIPMAP_LINEAR);
    tex_detail->getTexParameter()->setMagFilter(vl::TPF_LINEAR);
    tex_detail->getTexParameter()->setMinFilter(vl::TPF_LINEAR_MIPMAP_LINEAR);

    vl::ref<vl::Light> light = new vl::Light(0);

    vl::ref<vl::Effect> cube_right_fx = new vl::Effect;
    // to ensure the cubes are drawn after the textured quads
    cube_right_fx->setRenderRank(1);
    cube_right_fx->shader()->setRenderState( light.get() );
    cube_right_fx->shader()->enable(vl::EN_LIGHTING);
    cube_right_fx->shader()->enable(vl::EN_DEPTH_TEST);
    cube_right_fx->shader()->enable(vl::EN_BLEND);
    cube_right_fx->shader()->enable(vl::EN_ALPHA_TEST);
    cube_right_fx->shader()->gocAlphaFunc()->set(vl::FU_GEQUAL, 1.0f - 0.02f);
    cube_right_fx->shader()->gocLightModel()->setTwoSide(true);
    cube_right_fx->shader()->gocTextureUnit(0)->setTexture( tex_holebox.get() );

    vl::ref<vl::Effect> cube_left_fx = new vl::Effect;
    // to ensure the cubes are drawn after the textured quads
    cube_left_fx->setRenderRank(1);
    cube_left_fx->shader()->setRenderState( light.get() );
    cube_left_fx->shader()->enable(vl::EN_LIGHTING);
    cube_left_fx->shader()->enable(vl::EN_DEPTH_TEST);
    cube_left_fx->shader()->enable(vl::EN_BLEND);
    cube_left_fx->shader()->enable(vl::EN_ALPHA_TEST);
    cube_left_fx->shader()->gocAlphaFunc()->set(vl::FU_GEQUAL, 1.0f - 0.02f);
    cube_left_fx->shader()->gocLightModel()->setTwoSide(true);
    cube_left_fx->shader()->gocTextureUnit(0)->setTexture( tex_holebox.get() );
    cube_left_fx->shader()->gocTextureUnit(1)->setTexture( tex_detail.get() );
    cube_left_fx->shader()->gocTexEnv(1)->setMode( vl::TEM_MODULATE );

    sceneManager()->tree()->addActor( box.get(), cube_right_fx.get(), mCubeRightTransform.get() );
    sceneManager()->tree()->addActor( box.get(), cube_left_fx.get(),  mCubeLeftTransform.get() );
  }

  void texture3D()
  {
    vl::ref<vl::Image> img_volume = vl::loadImage("/volume/VLTest.dat");

    vl::ref<vl::Geometry> quad_3d = vlut::makeGrid( vl::vec3(0,0,0), 10, 10, 2, 2 );
    quad_3d->setColor(vlut::white);
    quad_3d->transform( vl::mat4::getRotation(90, 1,0,0), false );

    mTexCoords_3D = new vl::ArrayFloat3;
    quad_3d->setTexCoordArray(0, mTexCoords_3D.get());
    mTexCoords_3D->resize( 2*2 );
    quad_3d->setVBOEnabled(false);

    vl::ref<vl::Effect> fx_3d = new vl::Effect;

    if(vl::Has_GL_VERSION_1_2||vl::Has_GL_EXT_texture3D)
    {
      vl::ref<vl::Texture> texture_3d = new vl::Texture;
      texture_3d->prepareTexture3D( img_volume.get(), vl::TF_RGBA, mMipmappingOn, false );
      fx_3d->shader()->gocTextureUnit(0)->setTexture( texture_3d.get() );
      texture_3d->getTexParameter()->setMagFilter(vl::TPF_LINEAR);
      texture_3d->getTexParameter()->setMinFilter(vl::TPF_LINEAR_MIPMAP_LINEAR);
    }
    else
      vl::Log::error("Texture 3D not supported.\n");

    vl::Actor* act_3d = sceneManager()->tree()->addActor( quad_3d.get(), fx_3d.get(), new vl::Transform );
    act_3d->transform()->setLocalMatrix( vl::mat4::getTranslation(-6,+6,0) );
    act_3d->transform()->computeWorldMatrix();
  }

  void texture2DArray()
  {
    vl::ref<vl::Image> img_volume = vl::loadImage("/volume/VLTest.dat");
    m2DArraySize = img_volume->depth(); // save this to be used during the animation

    vl::ref<vl::Geometry> quad_2darray = vlut::makeGrid( vl::vec3(0,0,0), 10, 10, 2, 2 );
    quad_2darray->setColor(vlut::white);
    quad_2darray->transform( vl::mat4::getRotation(90, 1,0,0), false );

    mTexCoords_2DArray = new vl::ArrayFloat3;
    quad_2darray->setTexCoordArray(0, mTexCoords_2DArray.get());
    mTexCoords_2DArray->resize( 2*2 );
    quad_2darray->setVBOEnabled(false);

    vl::ref<vl::Effect> fx_2darray = new vl::Effect;

    if(vl::Has_GL_EXT_texture_array||vl::Has_GL_VERSION_3_0)
    {
      vl::ref<vl::Texture> texture_2darray = new vl::Texture;
      texture_2darray->prepareTexture2DArray( img_volume.get(), vl::TF_RGBA, mMipmappingOn );
      fx_2darray->shader()->gocTextureUnit(0)->setTexture( texture_2darray.get() );
      texture_2darray->getTexParameter()->setMagFilter(vl::TPF_LINEAR);
      texture_2darray->getTexParameter()->setMinFilter(vl::TPF_LINEAR_MIPMAP_LINEAR);
      
      // we need an OpenGL Shading Language program that uses 'sampler2DArray()' to access the texture!
      vl::GLSLProgram* glsl = fx_2darray->shader()->gocGLSLProgram();
      glsl->attachShader( new vl::GLSLFragmentShader("/glsl/texture_2d_array.fs") );
    }
    else
      vl::Log::error("Texture 2d array not supported.\n");

    vl::Actor* act_2darray = sceneManager()->tree()->addActor( quad_2darray.get(), fx_2darray.get(), new vl::Transform );
    act_2darray->transform()->setLocalMatrix( vl::mat4::getTranslation(+6,+6,0) );
    act_2darray->transform()->computeWorldMatrix();
  }

  void texture1DArray()
  {
    vl::ref<vl::Image> img_holebox = vl::loadImage("/images/holebox.tif");    
    m1DArraySize = img_holebox->height(); // save this to be used during the animation

    // create a grid with img_holebox->height() slices
    vl::ref<vl::Geometry> quad_1darray = vlut::makeGrid( vl::vec3(0,0,0), 10, 10, 2, img_holebox->height() );
    quad_1darray->setColor(vlut::white);
    quad_1darray->transform( vl::mat4::getRotation(90, 1,0,0), false );
    
    mTexCoords_1DArray = new vl::ArrayFloat2;
    quad_1darray->setTexCoordArray(0, mTexCoords_1DArray.get());
    mTexCoords_1DArray->resize( 2 * img_holebox->height() );
    quad_1darray->setVBOEnabled(false);

    vl::ref<vl::Effect> fx_1darray = new vl::Effect;

    if(vl::Has_GL_EXT_texture_array||vl::Has_GL_VERSION_3_0)
    {
      vl::ref<vl::Texture> texture_1darray = new vl::Texture;
      texture_1darray->prepareTexture1DArray( img_holebox.get(), vl::TF_RGBA, mMipmappingOn );
      fx_1darray->shader()->gocTextureUnit(0)->setTexture( texture_1darray.get() );
      texture_1darray->getTexParameter()->setMagFilter(vl::TPF_LINEAR);
      texture_1darray->getTexParameter()->setMinFilter(vl::TPF_LINEAR_MIPMAP_LINEAR);
      
      // we need an OpenGL Shading Language program that uses 'sampler1DArray()' to access the texture!
      vl::GLSLProgram* glsl = fx_1darray->shader()->gocGLSLProgram();
      glsl->attachShader( new vl::GLSLFragmentShader("/glsl/texture_1d_array.fs") );
    }
    else
      vl::Log::error("Texture 1d array not supported.\n");

    vl::Actor* act_1darray = sceneManager()->tree()->addActor( quad_1darray.get(), fx_1darray.get(), new vl::Transform );
    act_1darray->transform()->setLocalMatrix( vl::mat4::getTranslation(+6,-6,0) );
    act_1darray->transform()->computeWorldMatrix();
  }

  void textureRectangle()
  {
    vl::ref<vl::Image> img_holebox = vl::loadImage("/images/holebox.tif");    

    // generate non-normalized uv coordinates, i.e. from <0,0> to <img_holebox->width(),img_holebox->height()>
    vl::ref<vl::Geometry> quad_rect = vlut::makeGrid( vl::vec3(0,0,0), 10.0f, 10.0f, 2, 2, true, vl::fvec2(0,0), vl::fvec2((float)img_holebox->width(),(float)img_holebox->height()) );
    quad_rect->setColor(vlut::white);
    quad_rect->transform( vl::mat4::getRotation(90, 1,0,0), false );

    vl::ref<vl::Effect> fx_rect = new vl::Effect;

    if(vl::Has_GL_ARB_texture_rectangle||vl::Has_GL_NV_texture_rectangle||vl::Has_GL_VERSION_3_1)
    {
      vl::ref<vl::Texture> texture_rect = new vl::Texture;
      texture_rect->prepareTextureRectangle( img_holebox.get(), vl::TF_RGBA );
      fx_rect->shader()->gocTextureUnit(0)->setTexture( texture_rect.get() );
      // mipmaps not allowed with texture rectangle!
      texture_rect->getTexParameter()->setMagFilter(vl::TPF_LINEAR);
      texture_rect->getTexParameter()->setMinFilter(vl::TPF_LINEAR);
      // GL_REPEAT (the default) not allowed with texture rectangle!
      texture_rect->getTexParameter()->setWrapS(vl::TPW_CLAMP);
      texture_rect->getTexParameter()->setWrapT(vl::TPW_CLAMP);
      texture_rect->getTexParameter()->setWrapR(vl::TPW_CLAMP);
    }
    else
      vl::Log::error("Texture rectangle not supported.\n");

    vl::Actor* act_rect = sceneManager()->tree()->addActor( quad_rect.get(), fx_rect.get(), new vl::Transform );
    act_rect->transform()->setLocalMatrix( vl::mat4::getTranslation(-6,-6,0) );
    act_rect->transform()->computeWorldMatrix();
  }

  void sphericalMapping()
  {
    vl::ref<vl::Image> img_spheric = vl::loadImage("/images/spheremap_klimt.jpg");

    vl::ref<vl::Geometry> torus = vlut::makeTorus(vl::vec3(), 8,3, 40,40);
    // normals already present, needed by GL_SPHERE_MAP to work correctly!

    mFXSpheric = new vl::Effect;
    mFXSpheric->shader()->enable(vl::EN_DEPTH_TEST);
    mFXSpheric->shader()->enable(vl::EN_CULL_FACE);
    mFXSpheric->shader()->enable(vl::EN_LIGHTING);
    mFXSpheric->shader()->setRenderState( new vl::Light(0) );
    // to ensure the torus is drawn after the textured quads
    mFXSpheric->setRenderRank(1);

    vl::ref<vl::Texture> texture_spheric = new vl::Texture;
    texture_spheric->prepareTexture2D( img_spheric.get(), vl::TF_RGBA, mMipmappingOn, false );
    mFXSpheric->shader()->gocTextureUnit(0)->setTexture( texture_spheric.get() );
    texture_spheric->getTexParameter()->setAnisotropy(16.0);
    texture_spheric->getTexParameter()->setMagFilter(vl::TPF_LINEAR);
    texture_spheric->getTexParameter()->setMinFilter(vl::TPF_LINEAR_MIPMAP_LINEAR);
    // enable automatic texture generation for s,t
    mFXSpheric->shader()->gocTexGen(0)->setGenModeS(vl::TGM_SPHERE_MAP);
    mFXSpheric->shader()->gocTexGen(0)->setGenModeT(vl::TGM_SPHERE_MAP);

    mActSpheric = sceneManager()->tree()->addActor( torus.get(), mFXSpheric.get(), new vl::Transform );
    vl::VisualizationLibrary::rendering()->as<vl::Rendering>()->transform()->addChild( mActSpheric->transform() );
  }

  void cubeMapping()
  {
    // cube mapping, see also http://developer.nvidia.com/object/cube_map_ogl_tutorial.html

    vl::ref<vl::Image> img_cubemap = vl::loadCubemap(
      "/images/cubemap/cubemap00.png", // (x+) right
      "/images/cubemap/cubemap01.png", // (x-) left
      "/images/cubemap/cubemap02.png", // (y+) top
      "/images/cubemap/cubemap03.png", // (y-) bottom
      "/images/cubemap/cubemap04.png", // (z+) back
      "/images/cubemap/cubemap05.png");// (z-) front

    vl::ref<vl::Geometry> torus = vlut::makeTorus( vl::vec3(), 8,3, 40,40 );
    // normals already present, needed by GL_SPHERE_MAP to work correctly!

    mFXCubic = new vl::Effect;
    mFXCubic->shader()->enable(vl::EN_DEPTH_TEST);
    mFXCubic->shader()->enable(vl::EN_CULL_FACE);
    mFXCubic->shader()->enable(vl::EN_LIGHTING);
    mFXCubic->shader()->setRenderState( new vl::Light(0) );
    // to ensure the torus is drawn after the textured quads
    mFXCubic->setRenderRank(1);

    if (vl::Has_GL_VERSION_1_3||vl::Has_GL_ARB_texture_cube_map)
    {
      vl::ref<vl::Texture> texture_cubic = new vl::Texture;
      texture_cubic->prepareTextureCubemap( img_cubemap.get(), vl::TF_RGBA, mMipmappingOn, false );
      mFXCubic->shader()->gocTextureUnit(0)->setTexture( texture_cubic.get() );
      texture_cubic->getTexParameter()->setAnisotropy(16.0);
      texture_cubic->getTexParameter()->setMagFilter(vl::TPF_LINEAR);
      texture_cubic->getTexParameter()->setMinFilter(vl::TPF_LINEAR_MIPMAP_LINEAR);
      texture_cubic->getTexParameter()->setWrapS(vl::TPW_CLAMP_TO_EDGE);
      texture_cubic->getTexParameter()->setWrapT(vl::TPW_CLAMP_TO_EDGE);
      texture_cubic->getTexParameter()->setWrapR(vl::TPW_CLAMP_TO_EDGE);
      // enable automatic texture generation for s,t,r
      mFXCubic->shader()->gocTexGen(0)->setGenModeS(vl::TGM_REFLECTION_MAP);
      mFXCubic->shader()->gocTexGen(0)->setGenModeT(vl::TGM_REFLECTION_MAP);
      mFXCubic->shader()->gocTexGen(0)->setGenModeR(vl::TGM_REFLECTION_MAP);
      // texture matrix
      mFXCubic->shader()->gocTextureMatrix(0)->setUseCameraRotationInverse(true);
    }
    else
      vl::Log::error("Texture cubemap not supported.\n");

    mActCubic = sceneManager()->tree()->addActor( torus.get(), mFXCubic.get(), new vl::Transform );
    vl::VisualizationLibrary::rendering()->as<vl::Rendering>()->transform()->addChild( mActCubic->transform() );
  }

  void run()
  {
    // rotating cubes

    if (vl::Has_GL_ARB_multitexture)
    {
      mCubeRightTransform->setLocalMatrix( vl::mat4::getTranslation(+6,0,0) * vl::mat4::getRotation( vl::Time::currentTime()*45, 0, 1, 0) );
      mCubeLeftTransform ->setLocalMatrix( vl::mat4::getTranslation(-6,0,0) * vl::mat4::getRotation( vl::Time::currentTime()*45, 0, 1, 0) );
    }

    // 5 seconds period
    float t = sin( (float)vl::Time::currentTime()*vl::fPi*2.0f/5.0f) * 0.5f + 0.5f;
    t = t * (1.0f - 0.02f*2) + 0.02f;

    // 3d texture coordinates animation

    mTexCoords_3D->at(0) = vl::fvec3(0, 0, t);
    mTexCoords_3D->at(1) = vl::fvec3(0, 1, t);
    mTexCoords_3D->at(2) = vl::fvec3(1, 0, t);
    mTexCoords_3D->at(3) = vl::fvec3(1, 1, t);

    // 2d texture array coordinates animation

    mTexCoords_2DArray->at(0) = vl::fvec3(0, 0, t*m2DArraySize);
    mTexCoords_2DArray->at(1) = vl::fvec3(0, 1, t*m2DArraySize);
    mTexCoords_2DArray->at(2) = vl::fvec3(1, 0, t*m2DArraySize);
    mTexCoords_2DArray->at(3) = vl::fvec3(1, 1, t*m2DArraySize);

    // 1d texture array coordinates animation

    for(int i=0; i<m1DArraySize; ++i)
    {
      mTexCoords_1DArray->at(i*2+0) = vl::fvec2(0+t*0.02f*(i%2?+1.0f:-1.0f), (float)i);
      mTexCoords_1DArray->at(i*2+1) = vl::fvec2(1+t*0.02f*(i%2?+1.0f:-1.0f), (float)i);
    }

    // spherical mapped torus rotation

    mActSpheric->transform()->setLocalMatrix( vl::mat4::getTranslation(0,+6,0)*vl::mat4::getRotation(45*vl::Time::currentTime(),1,0,0) );
    mActSpheric->transform()->computeWorldMatrix();

    // cube mapped torus rotation

    mActCubic->transform()->setLocalMatrix( vl::mat4::getTranslation(0,-6,0)*vl::mat4::getRotation(45*vl::Time::currentTime(),1,0,0) );
    mActCubic->transform()->computeWorldMatrix();
  }

  virtual void mouseWheelEvent(int w)
  {
    mLodBias += w*0.3f;
    mLodBias = vl::clamp(mLodBias, 0.0f, 4.0f);

    mFXSpheric->shader()->gocTexEnv(0)->setLodBias(mLodBias);
    mFXCubic->shader()->gocTexEnv(0)->setLodBias(mLodBias);
  }

  virtual void initEvent()
  {
    BaseDemo::initEvent();

    trackball()->setTransform(vl::VisualizationLibrary::rendering()->as<vl::Rendering>()->transform());

    mMipmappingOn = true;
    mLodBias = 0.0;

    multitexturing();
    textureRectangle();
    texture3D();
    texture2DArray();
    texture1DArray();
    sphericalMapping();
    cubeMapping();
  }

protected:
  vl::ref<vl::Transform> mCubeRightTransform;
  vl::ref<vl::Transform> mCubeLeftTransform;
  vl::ref<vl::ArrayFloat3> mTexCoords_3D;
  vl::ref<vl::ArrayFloat3> mTexCoords_2DArray;
  vl::ref<vl::ArrayFloat2> mTexCoords_1DArray;
  int m1DArraySize;
  int m2DArraySize;
  vl::Actor* mActSpheric;
  vl::Actor* mActCubic;
  vl::ref<vl::Effect> mFXSpheric;
  vl::ref<vl::Effect> mFXCubic;
  float mLodBias;
  bool mMipmappingOn;
};

// Have fun!

BaseDemo* Create_App_Texturing() { return new App_Texturing; }
