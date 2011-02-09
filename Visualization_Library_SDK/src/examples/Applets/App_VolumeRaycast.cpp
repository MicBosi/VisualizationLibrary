/*************************************************************************************/
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
/*************************************************************************************/

#include "BaseDemo.hpp"
// #include <vlVolume/RaycastVolume.hpp>
#include <vlVolume/VolumeUtils.hpp>
#include <vlGraphics/Light.hpp>
#include <vlGraphics/Text.hpp>
#include <vlGraphics/FontManager.hpp>
#include <vlGraphics/GLSL.hpp>
#include <vlGraphics/GeometryPrimitives.hpp>

using namespace vl;

namespace vl
{
  class RaycastVolume: public vl::ActorEventCallback
  {
  public:
    
    RaycastVolume();
    
    void onActorRenderStarted(vl::Actor* actor, vl::Real frame_clock, const vl::Camera* cam, vl::Renderable* renderable, const vl::Shader* shader, int pass);

    void onActorDelete(vl::Actor* ) {}

    //! Binds a RaycastVolume to an Actor so that the RaycastVolume can generate the viewport aligned slices' geometry for the Actor as appropriate.
    void bindActor(vl::Actor*);

    //! Updates the uniforms used by the vl::GLSLProgram to render the volume each time the onActorRenderStarted() method is called.
    virtual void updateUniforms(vl::Actor* actor, vl::Real clock, const vl::Camera* camera, vl::Renderable* rend, const vl::Shader* shader);
    
    //! Returns the vl::Geometry associated to a RaycastVolume and its bound Actor
    vl::Geometry* geometry() { return mGeometry.get(); }
    
    //! Returns the vl::Geometry associated to a RaycastVolume and its bound Actor
    const vl::Geometry* geometry() const { return mGeometry.get(); }
    
    //! Defines the dimensions of the box enclosing the volume
    void setBox(const vl::AABB& box);
    
    //! The dimensions of the box enclosing the volume
    const vl::AABB& box() const { return mBox; }
    
    //! Returns the coordinates assigned to each of the 8 box corners of the volume
    const vl::fvec3* vertCoords() const { return mVertCoord->begin(); }
    
    //! Returns the coordinates assigned to each of the 8 box corners of the volume
    vl::fvec3* vertCoords() { return mVertCoord->begin(); }
    
    //! Returns the texture coordinates assigned to each of the 8 box corners of the volume
    const vl::fvec3* texCoords() const { return mTexCoord->begin(); }
    
    //! Returns the texture coordinates assigned to each of the 8 box corners of the volume
    vl::fvec3* texCoords() { return mTexCoord->begin(); }
    
    //! Generates a default set of texture coordinates for the 8 box corners of the volume based on the given texture dimensions.
    void generateTextureCoordinates(const vl::ivec3& size);
    
    //! Generates a default set of texture coordinates for the 8 box corners of the volume based on the given texture dimensions.
    void generateTextureCoordinates(int width, int height, int depth) { generateTextureCoordinates(vl::ivec3(width,height,depth)); }

  protected:
    vl::ref<vl::Geometry> mGeometry;
    vl::AABB mBox;
    vl::ref<vl::ArrayFloat3> mTexCoord;
    vl::ref<ArrayFloat3> mVertCoord;
  };
}

RaycastVolume::RaycastVolume()
{
  // box geometry
  mGeometry = new Geometry;

  // install vertex coords array
  mVertCoord = new ArrayFloat3;
  mVertCoord->resize(8);
  mGeometry->setVertexArray(mVertCoord.get());

  // install index array
  ref<DrawElementsUInt> de = new DrawElementsUInt(PT_QUADS);
  mGeometry->drawCalls()->push_back(de.get());
  unsigned int de_indices[] = 
  {
    0,1,2,3, 1,5,6,2, 5,4,7,6, 4,0,3,7, 3,2,6,7, 4,5,1,0
  };
  de->indices()->resize(4*6);
  memcpy(de->indices()->ptr(), de_indices, sizeof(de_indices));

  // install texture coords array
  mTexCoord = new ArrayFloat3;
  mTexCoord->resize(8);
  mGeometry->setTexCoordArray(0,mTexCoord.get());

  // generate default texture coordinates
  fvec3 texc[] = 
  {
    fvec3(0,0,0), fvec3(1,0,0), fvec3(1,1,0), fvec3(0,1,0),
    fvec3(0,0,1), fvec3(1,0,1), fvec3(1,1,1), fvec3(0,1,1)
  };
  memcpy(mTexCoord->ptr(), texc, sizeof(texc));

  // default box dimensions and geometry
  setBox( AABB( vec3(0,0,0), vec3(1,1,1) ) );
}
//-----------------------------------------------------------------------------
/** Reimplement this method to update the uniform variables of your GLSL program before the volume is rendered.
 * By default updateUniforms() updates the position of up to 4 lights in object space. Such positions are stored in the
 * \p "uniform vec3 light_position[4]" variable.
 * The updateUniforms() method also fills the \p "uniform bool light_enable[4]" variable with a flag marking if the Nth 
 * light is active or not. These light values are computed based on the lights bound to the current Shader.
 * The updateUniforms() method also fills the \p "uniform vec3 eye_position" variable which contains the camera position in
 * object space, useful to compute specular highlights etc. */
void RaycastVolume::updateUniforms(vl::Actor*actor, vl::Real, const vl::Camera* camera, vl::Renderable*, const vl::Shader* shader)
{
  const GLSLProgram* glsl = shader->getGLSLProgram();
  VL_CHECK(glsl);

  if (glsl->getUniformLocation("light_position") != -1 && glsl->getUniformLocation("light_enable") != -1)
  {
    // computes up to 4 light positions (in object space) and enables

    int light_enable[4] = { 0,0,0,0 };
    fvec3 light_position[4];
    bool has_lights = false;

    for(int i=0; i<4; ++i)
    {
      const vl::Light* light = shader->getLight(i);
      light_enable[i] = light != NULL;
      if (light)
      {
        has_lights = true;
        // light position following transform
        if (light->followedTransform())
          light_position[i] = (fmat4)light->followedTransform()->worldMatrix() * light->position().xyz();
        // light position following camera
        else
          light_position[i] = ((fmat4)camera->inverseViewMatrix() * light->position()).xyz();

        // light position in object space
        if (actor->transform())
          light_position[i] = (fmat4)actor->transform()->worldMatrix().getInverse() * light_position[i];
      }
    }

    actor->gocUniform("light_position")->setUniform(4, light_position);
    actor->gocUniform("light_enable")->setUniform1i(4, light_enable);
  }

  if (glsl->getUniformLocation("eye_position") != -1)
  {
    // pass the eye position in object space

    // eye postion
    fvec3 eye = (fvec3)camera->inverseViewMatrix().getT();
    // world to object space
    if (actor->transform())
      eye = (fmat4)actor->transform()->worldMatrix().getInverse() * eye;
    actor->gocUniform("eye_position")->setUniform(eye);
  }

  if (glsl->getUniformLocation("eye_look") != -1)
  {
    // pass the eye look direction in object space

    // eye postion
    fvec3 look = -(fvec3)camera->inverseViewMatrix().getZ();
    // world to object space
    if (actor->transform())
    {
      look = (fmat4)actor->transform()->worldMatrix().getInverse() /*.getTransposed().getInverse()*/ * look;
    }
    actor->gocUniform("eye_look")->setUniform(look);
  }
}
//-----------------------------------------------------------------------------
void RaycastVolume::bindActor(Actor* actor)
{
  actor->actorEventCallbacks()->push_back( this );
  actor->setLod(0, mGeometry.get());
}
//-----------------------------------------------------------------------------
void RaycastVolume::onActorRenderStarted(Actor* actor, Real clock, const Camera* camera, Renderable* rend, const Shader* shader, int pass)
{
  if (pass>0)
    return;

  // setup uniform variables

  if (shader->getGLSLProgram())
    updateUniforms(actor, clock, camera, rend, shader);
}
//-----------------------------------------------------------------------------
void RaycastVolume::generateTextureCoordinates(const ivec3& size)
{
  if (!size.x() || !size.y() || !size.z())
  {
    Log::error("RaycastVolume::generateTextureCoordinates(): failed! The size passed does not represent a 3D image.\n");
    return;
  }
  float dx = 0.5f/size.x();
  float dy = 0.5f/size.y();
  float dz = 0.5f/size.z();
  float x0 = 0.0f + dx;
  float x1 = 1.0f - dx;
  float y0 = 0.0f + dy;
  float y1 = 1.0f - dy;
  float z0 = 0.0f + dz;
  float z1 = 1.0f - dz;
  fvec3 texc[] = 
  {
    fvec3(x0,y0,z1), fvec3(x1,y0,z1), fvec3(x1,y1,z1), fvec3(x0,y1,z1),
    fvec3(x0,y0,z0), fvec3(x1,y0,z0), fvec3(x1,y1,z0), fvec3(x0,y1,z0),
  };
  memcpy(mTexCoord->ptr(), texc, sizeof(texc));
}
//-----------------------------------------------------------------------------
void RaycastVolume::setBox(const AABB& box) 
{
  mBox = box; 
  // generate the box geometry
  float x0 = box.minCorner().x();
  float y0 = box.minCorner().y();
  float z0 = box.minCorner().z();
  float x1 = box.maxCorner().x();
  float y1 = box.maxCorner().y();
  float z1 = box.maxCorner().z();
  fvec3 box_verts[] = 
  {
    fvec3(x0,y0,z1), fvec3(x1,y0,z1), fvec3(x1,y1,z1), fvec3(x0,y1,z1), 
    fvec3(x0,y0,z0), fvec3(x1,y0,z0), fvec3(x1,y1,z0), fvec3(x0,y1,z0), 
  };
  memcpy(mVertCoord->ptr(), box_verts, sizeof(box_verts));
  mGeometry->setBoundsDirty(true);
}
/* ----- sliced volume visualization settings ----- */

/* volume visualization mode */
static enum { Isosurface_Mode, Isosurface_Transp_Mode, MIP_Mode } MODE = Isosurface_Mode;

/* If enabled, renders the volume using 3 animated lights. */
static bool DYNAMIC_LIGHTS = false;

/* If enabled, a white transfer function is used and 3 colored lights 
   are used to render the volume. */
static bool COLORED_LIGHTS = false;

/* Use a separate 3d texture with a precomputed gradient to speedup the fragment shader.
   Requires more memory (for the gradient texture) but can speedup the rendering. */
static bool PRECOMPUTE_GRADIENT = false;

/* The sample step used to render the volume, the smaller the number the better
  (and slower) the rendering will be. */
static const float SAMPLE_STEP = 1.0f / 520.0f;

/* Our applet used to render and interact with the volume. */
class App_VolumeRaycast: public BaseDemo
{
public:

  /* initialize the applet with a default volume */
  virtual void initEvent()
  {
    BaseDemo::initEvent();

    if (!(GLEW_VERSION_2_0||GLEW_VERSION_3_0||GLEW_VERSION_4_0))
    {
      vl::Log::error("OpenGL Shading Language not supported.\n");
      vl::Time::sleep(3000);
      exit(1);
    }

    // variable preconditions
    COLORED_LIGHTS &= DYNAMIC_LIGHTS;
 
    // lights to be used later
    mLight0 = new Light(0);
    mLight1 = new Light(1);
    mLight2 = new Light(2);

    // you can color the lights!
    if (DYNAMIC_LIGHTS && COLORED_LIGHTS)
    {
      mLight0->setAmbient(fvec4(0.1f, 0.1f, 0.1f, 1.0f));
      mLight1->setAmbient(fvec4(0.0f, 0.0f, 0.0f, 1.0f));
      mLight2->setAmbient(fvec4(0.0f, 0.0f, 0.0f, 1.0f));
      mLight0->setDiffuse(vl::gold);
      mLight1->setDiffuse(vl::green);
      mLight2->setDiffuse(vl::royalblue);
    }

    // light bulbs
    if (DYNAMIC_LIGHTS)
    {
      mLight0Tr = new Transform;
      mLight1Tr = new Transform;
      mLight2Tr = new Transform;
      defRendering()->as<Rendering>()->transform()->addChild( mLight0Tr.get() );
      defRendering()->as<Rendering>()->transform()->addChild( mLight1Tr.get() );
      defRendering()->as<Rendering>()->transform()->addChild( mLight2Tr.get() );
      mLight0->followTransform( mLight0Tr.get() );
      mLight1->followTransform( mLight1Tr.get() );
      mLight2->followTransform( mLight2Tr.get() );

      ref<Effect> fx_bulb = new Effect;
      fx_bulb->shader()->enable(EN_DEPTH_TEST);
      ref<Geometry> light_bulb = vl::makeIcosphere(vec3(0,0,0),1,1);
      sceneManager()->tree()->addActor( light_bulb.get(), fx_bulb.get(), mLight0Tr.get() );
      sceneManager()->tree()->addActor( light_bulb.get(), fx_bulb.get(), mLight1Tr.get() );
      sceneManager()->tree()->addActor( light_bulb.get(), fx_bulb.get(), mLight2Tr.get() );
    }

    ref<Effect> vol_fx = new Effect;
    vol_fx->shader()->enable(EN_CULL_FACE);
    vol_fx->shader()->enable(EN_DEPTH_TEST);
    vol_fx->shader()->enable(EN_BLEND);
    vol_fx->shader()->setRenderState( mLight0.get() );
    // add the other lights only if dynamic lights have to be displayed
    if (DYNAMIC_LIGHTS)
    {
      vol_fx->shader()->setRenderState( mLight1.get() );
      vol_fx->shader()->setRenderState( mLight2.get() );
    }

    // The GLSL program used to perform the actual rendering.
    mGLSL = vol_fx->shader()->gocGLSLProgram();
    mGLSL->gocUniform("sample_step")->setUniform(SAMPLE_STEP);
    mGLSL->attachShader( new GLSLVertexShader("/glsl/volume_luminance_light.vs") );
    if (MODE == Isosurface_Mode)
      mGLSL->attachShader( new GLSLFragmentShader("/glsl/volume_raycast_isosurface.fs") );
    else
    if (MODE == Isosurface_Transp_Mode)
      mGLSL->attachShader( new GLSLFragmentShader("/glsl/volume_raycast_isosurface_transp.fs") );
    else
    if (MODE == MIP_Mode)
      mGLSL->attachShader( new GLSLFragmentShader("/glsl/volume_raycast_mip.fs") );

    // transform and trackball setup
    mVolumeTr = new Transform;
    trackball()->setTransform( mVolumeTr.get() );

    // volume actor
    mVolumeAct = new Actor;
    mVolumeAct->setEffect( vol_fx.get() );
    mVolumeAct->setTransform(mVolumeTr.get());
    sceneManager()->tree()->addActor( mVolumeAct.get() );

    // RaycastVolume will generate the actual actor's geometry upon setBox() invocation
    mRaycastVolume = new vl::RaycastVolume;
    mRaycastVolume->bindActor(mVolumeAct.get());
    AABB volume_box( vec3(-10,-10,-10), vec3(+10,+10,+10) );
    mRaycastVolume->setBox(volume_box);

    // volume bounding box outline
    ref<Effect> fx_box = new Effect;
    fx_box->shader()->gocPolygonMode()->set(PM_LINE, PM_LINE);
    fx_box->shader()->enable(EN_DEPTH_TEST);
    ref<Geometry> box_outline = vl::makeBox(volume_box);
    box_outline->setColor(vl::red);
    sceneManager()->tree()->addActor( box_outline.get(), fx_box.get(), mVolumeTr.get() );

    // bias text
    mBiasText = new Text;
    mBiasText->setFont( defFontManager()->acquireFont("/font/bitstream-vera/VeraMono.ttf", 12) );
    mBiasText->setAlignment( AlignHCenter | AlignBottom);
    mBiasText->setViewportAlignment( AlignHCenter | AlignBottom );
    mBiasText->translate(0,5,0);
    mBiasText->setBackgroundEnabled(true);
    mBiasText->setBackgroundColor(fvec4(0,0,0,0.75));
    mBiasText->setColor(vl::white);
    ref<Effect> effect = new Effect;
    effect->shader()->enable(EN_BLEND);
    sceneManager()->tree()->addActor(mBiasText.get(), effect.get());

    // mic fixme: isosurface value, and other interpretation of it?
    // bias uniform
    mVolumeAct->gocUniform("val_threshold")->setUniform(0.5f);
    mAlphaBias = mVolumeAct->getUniform("val_threshold");

    // update alpha bias text
    mouseWheelEvent(0);

    // let's get started with the default volume!
    setupVolume( loadImage("/volume/VLTest.dat") );
  }

  /* load files drag&dropped in the window */
  void fileDroppedEvent(const std::vector<String>& files)
  {
    if(files.size() == 1) // if there is one file load it directly
    {      
      if (files[0].endsWith(".dat"))
      {
        ref<Image> vol_img = loadImage(files[0]);
        if (vol_img)
          setupVolume(vol_img);
      }
    }
    else // if there is more than one file load all the files and assemble a 3D image
    {      
      // sort files by their name
      std::vector<String> files_sorted = files;
      std::sort(files_sorted.begin(), files_sorted.end());
      // load the files
      std::vector< ref<Image> > images;
      for(unsigned int i=0; i<files_sorted.size(); ++i)
        images.push_back( loadImage(files_sorted[i]) );
      // assemble the volume
      ref<Image> vol_img = assemble3DImage(images);
      // set the volume
      if (vol_img)
        setupVolume(vol_img);
    }
  }

  /* visualize the given volume */
  void setupVolume(ref<Image> img)
  {
    Effect* vol_fx = mVolumeAct->effect();

    // mic fixme: used?
    //// remove shader uniforms
    //vol_fx->shader()->setUniformSet(NULL);
    //// remove GLSLProgram
    //vol_fx->shader()->eraseRenderState(vl::RS_GLSLProgram);
    //// keep texture unit #0
    //// vol_fx->shader()->eraseRenderState(RS_TextureUnit0); 
    //// remove texture unit #1 and #2
    //vol_fx->shader()->eraseRenderState(RS_TextureUnit1);
    //vol_fx->shader()->eraseRenderState(RS_TextureUnit2);

    if(img->format() == IF_LUMINANCE || true) // mic fixme
    {
      ref<Image> gradient;
      if (PRECOMPUTE_GRADIENT)
      {
        // note that this can take a while...
        gradient = vl::genGradientNormals(img.get());
      }

      ref<Image> trfunc;
      if (COLORED_LIGHTS)
        trfunc = vl::makeColorSpectrum(128, vl::white, vl::white); // let the lights color the volume
      else
        trfunc = vl::makeColorSpectrum(128, vl::blue, vl::royalblue, vl::green, vl::yellow, vl::crimson);
      // installs GLSLProgram
      vol_fx->shader()->setRenderState(mGLSL.get());
      // install volume image
      vol_fx->shader()->gocTextureUnit(0)->setTexture( new vl::Texture( img.get(), TF_LUMINANCE8, false, false ) );
      vol_fx->shader()->gocUniform("volume_texunit")->setUniform(0);
      mRaycastVolume->generateTextureCoordinates( img->width(), img->height(), img->depth() );
      // installs the transfer function as texture #1
      vol_fx->shader()->gocTextureUnit(1)->setTexture( new Texture( trfunc.get() ) );
      vol_fx->shader()->gocUniform("trfunc_texunit")->setUniform(1);
      vol_fx->shader()->gocUniform("trfunc_delta")->setUniform(0.5f/trfunc->width());
      // pre-computed gradient texture
      if (MODE == Isosurface_Mode || MODE == Isosurface_Transp_Mode)
      {
        if (PRECOMPUTE_GRADIENT)
        {
          vol_fx->shader()->gocUniform("precomputed_gradient")->setUniform(1);
          vol_fx->shader()->gocTextureUnit(2)->setTexture( new Texture( gradient.get(), TF_RGBA, false, false ) );
          vol_fx->shader()->gocUniform("gradient_texunit")->setUniform(2);
        }
        else
        {
          vol_fx->shader()->gocUniform("precomputed_gradient")->setUniform(0);
          // used to compute on the fly the normals based on the volume's gradient
          vol_fx->shader()->gocUniform("gradient_delta")->setUniform(fvec3(0.5f/img->width(), 0.5f/img->height(), 0.5f/img->depth()));
        }
      }

      // no need for alpha testing, we discard fragments inside the fragment shader
      vol_fx->shader()->disable(EN_ALPHA_TEST);
    }
    else
    {
      // mic fixme: how to interpret the other values?
      // RGBA -> RGB = used instead of transfer function, A = used as luminance.
      // RGB  -> ?
      Log::error("Only IF_LUMINANCE volumes are supported.\n");
    }

    mAlphaBias->setUniform(0.3f);
    updateText();
    openglContext()->update();
  }

  void updateText()
  {
    float bias = 0.0f;
    mAlphaBias->getUniform(&bias);
    mBiasText->setText(Say("Bias = %n") << bias);
  }

  void mouseWheelEvent(int val)
  {
    float alpha = 0.0f;
    mAlphaBias->getUniform(&alpha);
    alpha += val * 0.01f;
    alpha =  clamp(alpha, 0.0f, 1.0f);
    mAlphaBias->setUniform(alpha);

    // used for non GLSL mode volumes
    mVolumeAct->effect()->shader()->gocAlphaFunc()->set(FU_GEQUAL, alpha);
    
    updateText();
    openglContext()->update();
  }

  /* animate the lights */
  virtual void run()
  {
    if (DYNAMIC_LIGHTS)
    {
      mat4 mat;
      // light 0 transform.
      mat = mat4::getRotation( Time::currentTime()*43, 0,1,0 ) * mat4::getTranslation(20,20,20);
      mLight0Tr->setLocalMatrix(mat);
      // light 1 transform.
      mat = mat4::getRotation( Time::currentTime()*47, 0,1,0 ) * mat4::getTranslation(-20,0,0);
      mLight1Tr->setLocalMatrix(mat);
      // light 2 transform.
      mat = mat4::getRotation( Time::currentTime()*47, 0,1,0 ) * mat4::getTranslation(+20,0,0);
      mLight2Tr->setLocalMatrix(mat);
    }
  }

  protected:
    ref<Transform> mVolumeTr;
    ref<Transform> mLight0Tr;
    ref<Transform> mLight1Tr;
    ref<Transform> mLight2Tr;
    ref<Uniform> mAlphaBias;
    ref<Text> mBiasText;
    ref<Light> mLight0;
    ref<Light> mLight1;
    ref<Light> mLight2;
    ref<GLSLProgram> mGLSL;
    ref<Actor> mVolumeAct;
    ref<vl::RaycastVolume> mRaycastVolume;
};

// Have fun!

BaseDemo* Create_App_VolumeRaycast() { return new App_VolumeRaycast; }
