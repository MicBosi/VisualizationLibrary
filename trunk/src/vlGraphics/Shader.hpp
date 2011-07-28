/**************************************************************************************/
/*                                                                                    */
/*  Visualization Library                                                             */
/*  http://www.visualizationlibrary.org                                               */
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

#ifndef Shader_INCLUDE_ONCE
#define Shader_INCLUDE_ONCE

#include <vlCore/Vector4.hpp>
#include <vlCore/Matrix4.hpp>
#include <vlGraphics/link_config.hpp>
#include <vlGraphics/RenderState.hpp>
#include <vlGraphics/RenderStateSet.hpp>
#include <vlGraphics/EnableSet.hpp>
#include <vlGraphics/UniformSet.hpp>
#include <vlGraphics/Texture.hpp>
#include <vlGraphics/Scissor.hpp>
#include <vector>

namespace vl
{
  class Light;
  class ClipPlane;
  class Shader;
  //------------------------------------------------------------------------------
  // PixelTransfer
  //------------------------------------------------------------------------------
  /** RenderState wrapping the OpenGL function glPixelTransfer(), see also http://www.opengl.org/sdk/docs/man/xhtml/glPixelTransfer.xml for more information.
   * 
   * \sa Shader, Effect, Actor */
  class VLGRAPHICS_EXPORT PixelTransfer: public RenderState
  {
    VL_INSTRUMENT_CLASS(vl::PixelTransfer, RenderState)

  public:
    PixelTransfer()
    {
      VL_DEBUG_SET_OBJECT_NAME()
      mMapColor    = false;
      mMapStencil  = false;
      mIndexShift  = 0;
      mIndexOffset = 0;
      mRedScale    = 1;
      mGreenScale  = 1;
      mBlueScale   = 1;
      mAlphaScale  = 1;
      mDepthScale  = 1;
      mRedBias     = 0;
      mGreenBias   = 0;
      mBlueBias    = 0;
      mAlphaBias   = 0;
      mDepthBias   = 0;
      mPostColorMatrixRedScale   = 1;
      mPostColorMatrixGreenScale = 1;
      mPostColorMatrixBlueScale  = 1;
      mPostColorMatrixAlphaScale = 1;
      mPostColorMatrixRedBias    = 0;
      mPostColorMatrixGreenBias  = 0;
      mPostColorMatrixBlueBias   = 0;
      mPostColorMatrixAlphaBias  = 0;
      mPostConvolutionRedScale   = 1;
      mPostConvolutionGreenScale = 1;
      mPostConvolutionBlueScale  = 1;
      mPostConvolutionAlphaScale = 1;
      mPostConvolutionRedBias    = 0;
      mPostConvolutionGreenBias  = 0;
      mPostConvolutionBlueBias   = 0;
      mPostConvolutionAlphaBias  = 0;
    }

    virtual ERenderState type() const { return RS_PixelTransfer; }

    virtual void apply(const Camera*, OpenGLContext* ctx) const;

    bool mapColor() const { return mMapColor; }
    bool mapStencil() const { return mMapStencil; }
    int indexShift() const { return mIndexShift; }
    int indexOffset() const { return mIndexOffset; }
    float redScale() const { return mRedScale; }
    float greenScale() const { return mGreenScale; }
    float blueScale() const { return mBlueScale; }
    float alphaScale() const { return mAlphaScale; }
    float depthScale() const { return mDepthScale; }
    float redBias() const { return mRedBias; }
    float greenBias() const { return mGreenBias; }
    float blueBias() const { return mBlueBias; }
    float alphaBias() const { return mAlphaBias; }
    float depthBias() const { return mDepthBias; }
    float postColorMatrixRedScale() const { return mPostColorMatrixRedScale; }
    float postColorMatrixGreenScale() const { return mPostColorMatrixGreenScale; }
    float postColorMatrixBlueScale() const { return mPostColorMatrixBlueScale; }
    float postColorMatrixAlphaScale() const { return mPostColorMatrixAlphaScale; }
    float postColorMatrixRedBias() const { return mPostColorMatrixRedBias; }
    float postColorMatrixGreenBias() const { return mPostColorMatrixGreenBias; }
    float postColorMatrixBlueBias() const { return mPostColorMatrixBlueBias; }
    float postColorMatrixAlphaBias() const { return mPostColorMatrixAlphaBias; }
    float postConvolutionRedScale() const { return mPostConvolutionRedScale; }
    float postConvolutionGreenScale() const { return mPostConvolutionGreenScale; }
    float postConvolutionBlueScale() const { return mPostConvolutionBlueScale; }
    float postConvolutionAlphaScale() const { return mPostConvolutionAlphaScale; }
    float postConvolutionRedBias() const { return mPostConvolutionRedBias; }
    float postConvolutionGreenBias() const { return mPostConvolutionGreenBias; }
    float postConvolutionBlueBias() const { return mPostConvolutionBlueBias; }
    float postConvolutionAlphaBias() const { return mPostConvolutionAlphaBias; }

    void setMapColor(bool map_color) {  mMapColor = map_color; }
    void setMapStencil(bool map_stencil) {  mMapStencil = map_stencil; }
    void setIndexShift(int index_shift) {  mIndexShift = index_shift; }
    void setIndexOffset(int index_offset) {  mIndexOffset = index_offset; }
    void setRedScale(float red_scale) {  mRedScale = red_scale; }
    void setGreenScale(float green_scale) {  mGreenScale = green_scale; }
    void setBlueScale(float blue_scale) {  mBlueScale = blue_scale; }
    void setAlphaScale(float alpha_scale) {  mAlphaScale = alpha_scale; }
    void setDepthScale(float depth_scale) {  mDepthScale = depth_scale; }
    void setRedBias(float red_bias) {  mRedBias = red_bias; }
    void setGreenBias(float green_bias) {  mGreenBias = green_bias; }
    void setBlueBias(float blue_bias) {  mBlueBias = blue_bias; }
    void setAlphaBias(float alpha_bias) {  mAlphaBias = alpha_bias; }
    void setDepthBias(float depth_bias) {  mDepthBias = depth_bias; }
    void setPostColorMatrixRedScale(float scale) {  mPostColorMatrixRedScale = scale; }
    void setPostColorMatrixGreenScale(float scale) {  mPostColorMatrixGreenScale = scale; }
    void setPostColorMatrixBlueScale(float scale) {  mPostColorMatrixBlueScale = scale; }
    void setPostColorMatrixAlphaScale(float scale) {  mPostColorMatrixAlphaScale = scale; }
    void setPostColorMatrixRedBias(float bias) {  mPostColorMatrixRedBias = bias; }
    void setPostColorMatrixGreenBias(float bias) {  mPostColorMatrixGreenBias = bias; }
    void setPostColorMatrixBlueBias(float bias) {  mPostColorMatrixBlueBias = bias; }
    void setPostColorMatrixAlphaBias(float bias) {  mPostColorMatrixAlphaBias = bias; }
    void setPostConvolutionRedScale(float scale) {  mPostConvolutionRedScale = scale; }
    void setPostConvolutionGreenScale(float scale) {  mPostConvolutionGreenScale = scale; }
    void setPostConvolutionBlueScale(float scale) {  mPostConvolutionBlueScale = scale; }
    void setPostConvolutionAlphaScale(float scale) {  mPostConvolutionAlphaScale = scale; }
    void setPostConvolutionRedBias(float bias) {  mPostConvolutionRedBias = bias; }
    void setPostConvolutionGreenBias(float bias) {  mPostConvolutionGreenBias = bias; }
    void setPostConvolutionBlueBias(float bias) {  mPostConvolutionBlueBias = bias; }
    void setPostConvolutionAlphaBias(float bias) {  mPostConvolutionAlphaBias = bias; }

  protected:
    bool mMapColor;
    bool mMapStencil;
    int mIndexShift;
    int mIndexOffset;
    float mRedScale;
    float mGreenScale;
    float mBlueScale;
    float mAlphaScale;
    float mDepthScale;
    float mRedBias;
    float mGreenBias;
    float mBlueBias;
    float mAlphaBias;
    float mDepthBias;
    float mPostColorMatrixRedScale;
    float mPostColorMatrixGreenScale;
    float mPostColorMatrixBlueScale;
    float mPostColorMatrixAlphaScale;
    float mPostColorMatrixRedBias;
    float mPostColorMatrixGreenBias;
    float mPostColorMatrixBlueBias;
    float mPostColorMatrixAlphaBias;
    float mPostConvolutionRedScale;
    float mPostConvolutionGreenScale;
    float mPostConvolutionBlueScale;
    float mPostConvolutionAlphaScale;
    float mPostConvolutionRedBias;
    float mPostConvolutionGreenBias;
    float mPostConvolutionBlueBias;
    float mPostConvolutionAlphaBias;

  };
  //------------------------------------------------------------------------------
  // Hint
  //------------------------------------------------------------------------------
  /** RenderState wrapping the OpenGL function glHint(), see also http://www.opengl.org/sdk/docs/man/xhtml/glHint.xml for more information.
   * \sa Shader, Effect, Actor */
  class VLGRAPHICS_EXPORT Hint: public RenderState
  {
    VL_INSTRUMENT_CLASS(vl::Hint, RenderState)

  public:
    Hint(): mPerspectiveCorrectionHint(HM_DONT_CARE), mPointSmoothHint(HM_DONT_CARE), mLineSmoothHint(HM_DONT_CARE),
            mPolygonSmoothHint(HM_DONT_CARE), mFogHint(HM_DONT_CARE), mGenerateMipmapHint(HM_DONT_CARE)
    {
      VL_DEBUG_SET_OBJECT_NAME()
    }

    virtual ERenderState type() const { return RS_Hint; }

    virtual void apply(const Camera*, OpenGLContext* ctx) const;

    void setPerspectiveCorrectionHint(EHintMode mode) { mPerspectiveCorrectionHint = mode; }
    void setPolygonSmoohtHint(EHintMode mode) { mPolygonSmoothHint = mode; }
    void setLineSmoothHint(EHintMode mode) { mLineSmoothHint = mode; }
    void setPointSmoothHint(EHintMode mode) { mPointSmoothHint = mode; }
    void setFogHint(EHintMode mode) { mFogHint = mode; }
    void setGenerateMipmapHint(EHintMode mode) { mGenerateMipmapHint = mode; }

    EHintMode perspectiveCorrectionHint() const { return mPerspectiveCorrectionHint; }
    EHintMode polygonSmoohtHint() const { return mPolygonSmoothHint; }
    EHintMode lineSmoothHint() const { return mLineSmoothHint; }
    EHintMode pointSmoothHint() const { return mPointSmoothHint; }
    EHintMode fogHint() const { return mFogHint; }
    EHintMode generateMipmapHint() const { return mGenerateMipmapHint; }

  protected:
    EHintMode mPerspectiveCorrectionHint;
    EHintMode mPointSmoothHint;
    EHintMode mLineSmoothHint;
    EHintMode mPolygonSmoothHint;
    EHintMode mFogHint;
    EHintMode mGenerateMipmapHint;
  };
  //------------------------------------------------------------------------------
  // CullFace
  //------------------------------------------------------------------------------
  /** RenderState wrapping the OpenGL function glCullFace(), see also http://www.opengl.org/sdk/docs/man/xhtml/glCullFace.xml for more information.
   * \sa Shader, Effect, Actor, vl::EN_CULL_FACE */
  class VLGRAPHICS_EXPORT CullFace: public RenderState
  {
    VL_INSTRUMENT_CLASS(vl::CullFace, RenderState)

  public:
    CullFace(EPolygonFace cullface=PF_BACK): mFaceMode(cullface)
    {
      VL_DEBUG_SET_OBJECT_NAME()
    }
    virtual ERenderState type() const { return RS_CullFace; }
    virtual void apply(const Camera*, OpenGLContext* ctx) const;
    void set(EPolygonFace facemode) { mFaceMode = facemode; }
    EPolygonFace faceMode() const { return mFaceMode; }
  protected:
    EPolygonFace mFaceMode;
  };
  //------------------------------------------------------------------------------
  // FrontFace
  //------------------------------------------------------------------------------
  /** RenderState wrapping the OpenGL function glFrontFace(), see also http://www.opengl.org/sdk/docs/man/xhtml/glFrontFace.xml for more information.
   * \sa Shader, Effect, Actor */
  class VLGRAPHICS_EXPORT FrontFace: public RenderState
  {
    VL_INSTRUMENT_CLASS(vl::FrontFace, RenderState)

  public:
    FrontFace(EFrontFace frontface=FF_CCW): mFrontFace(frontface)
    {
      VL_DEBUG_SET_OBJECT_NAME()
    }
    virtual ERenderState type() const { return RS_FrontFace; }
    virtual void apply(const Camera*, OpenGLContext* ctx) const;
    void set(EFrontFace frontface) { mFrontFace = frontface; }
    EFrontFace frontFace() const { return mFrontFace; }
  protected:
    EFrontFace mFrontFace;
  };
  //------------------------------------------------------------------------------
  // DepthFunc
  //------------------------------------------------------------------------------
  /** RenderState wrapping the OpenGL function glDepthFunc(), see also http://www.opengl.org/sdk/docs/man/xhtml/glDepthFunc.xml for more information.
   * \sa Shader, Effect, Actor, vl::EN_DEPTH_TEST */
  class VLGRAPHICS_EXPORT DepthFunc: public RenderState
  {
    VL_INSTRUMENT_CLASS(vl::DepthFunc, RenderState)

  public:
    DepthFunc(EFunction depthfunc=FU_LESS): mDepthFunc(depthfunc)
    {
      VL_DEBUG_SET_OBJECT_NAME()
    }
    virtual ERenderState type() const { return RS_DepthFunc; }
    virtual void apply(const Camera*, OpenGLContext* ctx) const;
    void set(EFunction depthfunc) { mDepthFunc = depthfunc; }
    EFunction depthFunc() const { return mDepthFunc; }
  protected:
    EFunction mDepthFunc;
  };
  //------------------------------------------------------------------------------
  // DepthMask
  //------------------------------------------------------------------------------
  /** RenderState wrapping the OpenGL function glDepthMask(), see also http://www.opengl.org/sdk/docs/man/xhtml/glDepthMask.xml for more information.
   * \sa Shader, Effect, Actor */
  class VLGRAPHICS_EXPORT DepthMask: public RenderState
  {
    VL_INSTRUMENT_CLASS(vl::DepthMask, RenderState)

  public:
    DepthMask(bool depthmask=true): mDepthMask(depthmask)
    {
      VL_DEBUG_SET_OBJECT_NAME()
    }
    virtual ERenderState type() const { return RS_DepthMask; }
    virtual void apply(const Camera*, OpenGLContext* ctx) const;
    void set(bool depthmask) { mDepthMask = depthmask; }
    bool depthMask() const { return mDepthMask; }
  protected:
    bool mDepthMask;
  };
  //------------------------------------------------------------------------------
  // PolygonMode
  //------------------------------------------------------------------------------
  /** RenderState wrapping the OpenGL function glPolygonMode(), see also http://www.opengl.org/sdk/docs/man/xhtml/glPolygonMode.xml for more information.
   * \sa Shader, Effect, Actor */
  class VLGRAPHICS_EXPORT PolygonMode: public RenderState
  {
    VL_INSTRUMENT_CLASS(vl::PolygonMode, RenderState)

  public:
    PolygonMode(EPolygonMode frontface=PM_FILL, EPolygonMode backface=PM_FILL): mFrontFace(frontface), mBackFace(backface)
    {
      VL_DEBUG_SET_OBJECT_NAME()
    }
    virtual ERenderState type() const { return RS_PolygonMode; }
    virtual void apply(const Camera*, OpenGLContext* ctx) const;
    void set(EPolygonMode frontface, EPolygonMode backface) { mFrontFace = frontface; mBackFace = backface; }
    void setFrontFace(EPolygonMode frontface) { mFrontFace = frontface; }
    void setBackFace(EPolygonMode backface) { mBackFace = backface; }
    EPolygonMode frontFace() const { return mFrontFace; }
    EPolygonMode backFace() const { return mBackFace; }
  protected:
    EPolygonMode mFrontFace;
    EPolygonMode mBackFace;
  };
  //------------------------------------------------------------------------------
  // ShadeModel
  //------------------------------------------------------------------------------
  /** RenderState wrapping the OpenGL function glShadeModel(), see also http://www.opengl.org/sdk/docs/man/xhtml/glShadeModel.xml for more information.
   * \sa Shader, Effect, Actor */
  class VLGRAPHICS_EXPORT ShadeModel: public RenderState
  {
    VL_INSTRUMENT_CLASS(vl::ShadeModel, RenderState)

  public:
    ShadeModel(EShadeModel shademodel=SM_SMOOTH): mShadeModel(shademodel)
    {
      VL_DEBUG_SET_OBJECT_NAME()
    }
    virtual ERenderState type() const { return RS_ShadeModel; }
    virtual void apply(const Camera*, OpenGLContext* ctx) const;
    void set(EShadeModel shademodel) { mShadeModel = shademodel; }
    EShadeModel shadeModel() const { return mShadeModel; }
  protected:
    EShadeModel mShadeModel;
  };
  //------------------------------------------------------------------------------
  // BlendFunc
  //------------------------------------------------------------------------------
  /** RenderState wrapping the OpenGL function glBlendFunc(), see also http://www.opengl.org/sdk/docs/man/xhtml/glBlendFunc.xml for more information.
   * \sa Shader, Effect, Actor, vl::EN_BLEND */
  class VLGRAPHICS_EXPORT BlendFunc: public RenderState
  {
    VL_INSTRUMENT_CLASS(vl::BlendFunc, RenderState)

  public:
    BlendFunc(EBlendFactor src_rgb=BF_SRC_ALPHA, EBlendFactor dst_rgb=BF_ONE_MINUS_SRC_ALPHA, EBlendFactor src_alpha=BF_SRC_ALPHA, EBlendFactor dst_alpha=BF_ONE_MINUS_SRC_ALPHA):
      mSrcRGB(src_rgb), mDstRGB(dst_rgb), mSrcAlpha(src_alpha), mDstAlpha(dst_alpha)
    {
      VL_DEBUG_SET_OBJECT_NAME()
    }
    virtual ERenderState type() const { return RS_BlendFunc; }
    // if glBlendFuncSeparate is not supported uses RGB factor for both RGB and Alpha
    virtual void apply(const Camera*, OpenGLContext* ctx) const;
    void set(EBlendFactor src_rgb, EBlendFactor dst_rgb, EBlendFactor src_alpha, EBlendFactor dst_alpha) { mSrcRGB = src_rgb; mSrcAlpha = src_alpha; mDstRGB = dst_rgb; mDstAlpha = dst_alpha; }
    void set(EBlendFactor src_rgba, EBlendFactor dst_rgba) { mSrcRGB = src_rgba; mSrcAlpha = src_rgba; mDstRGB = dst_rgba; mDstAlpha = dst_rgba; }
    void setSrcRGB(EBlendFactor factor) { mSrcRGB = factor; }
    void setDstRGB(EBlendFactor factor) { mDstRGB = factor; }
    void setSrcAlpha(EBlendFactor factor) { mSrcAlpha = factor; }
    void setDstAlpha(EBlendFactor factor) { mDstAlpha = factor; }
    EBlendFactor srcRGB() const { return mSrcRGB; }
    EBlendFactor dstRGB() const { return mDstRGB; }
    EBlendFactor srcAlpha() const { return mSrcAlpha; }
    EBlendFactor dstAlpha() const { return mDstAlpha; }
  protected:
    EBlendFactor mSrcRGB;
    EBlendFactor mDstRGB;
    EBlendFactor mSrcAlpha;
    EBlendFactor mDstAlpha;
  };
  //------------------------------------------------------------------------------
  // BlendEquation
  //------------------------------------------------------------------------------
  /** RenderState wrapping the OpenGL function glBlendEquation()/glBlendEquationSeparate(), see also 
   * http://www.opengl.org/sdk/docs/man/xhtml/glBlendEquation.xml and 
   * http://www.opengl.org/sdk/docs/man/xhtml/glBlendEquationSeparate.xml for more information.
   * \sa Shader, Effect, Actor */
  class VLGRAPHICS_EXPORT BlendEquation: public RenderState
  {
    VL_INSTRUMENT_CLASS(vl::BlendEquation, RenderState)

  public:
    BlendEquation(EBlendEquation mode_rgb=BE_FUNC_ADD, EBlendEquation mode_alpha=BE_FUNC_ADD): mModeRGB(mode_rgb), mModeAlpha(mode_alpha)
    {
      VL_DEBUG_SET_OBJECT_NAME()
    }
    virtual ERenderState type() const { return RS_BlendEquation; }
    // if glBlendEquationSeparate is not supported uses RGB mode for both RGB and Alpha
    virtual void apply(const Camera*, OpenGLContext* ctx) const;
    void set(EBlendEquation mode_rgba) { mModeRGB = mode_rgba; mModeAlpha = mode_rgba; }
    void set(EBlendEquation mode_rgb, EBlendEquation mode_alpha) { mModeRGB = mode_rgb; mModeAlpha = mode_alpha; }
    EBlendEquation modeRGB() const { return mModeRGB; }
    EBlendEquation modeAlpha() const { return mModeAlpha; }
  protected:
    EBlendEquation mModeRGB;
    EBlendEquation mModeAlpha;
  };
  //------------------------------------------------------------------------------
  // SampleCoverage
  //------------------------------------------------------------------------------
  /** RenderState wrapping the OpenGL function glSampleCoverage(), see also http://www.opengl.org/sdk/docs/man/xhtml/glSampleCoverage.xml for more information.
   * \sa Shader, Effect, Actor, vl::EN_SAMPLE_ALPHA_TO_COVERAGE, vl::EN_SAMPLE_ALPHA_TO_ONE, vl::EN_SAMPLE_COVERAGE */
  class VLGRAPHICS_EXPORT SampleCoverage: public RenderState
  {
    VL_INSTRUMENT_CLASS(vl::SampleCoverage, RenderState)

  public:
    SampleCoverage(GLclampf value=1.0f, bool invert=false): mValue(value), mInvert(invert)
    {
      VL_DEBUG_SET_OBJECT_NAME()
    }
    virtual ERenderState type() const { return RS_SampleCoverage; }
    virtual void apply(const Camera*, OpenGLContext* ctx) const;
    void set(GLclampf value, bool invert) { mValue = value; mInvert = invert; }
    void setValue(GLclampf value) { mValue = value; }
    void setInvert(bool invert) { mInvert = invert; }
    GLclampf value() const { return mValue; }
    bool invert() const { return mInvert; }
  protected:
    GLclampf mValue;
    bool mInvert;
  };
  //------------------------------------------------------------------------------
  // AlphaFunc
  //------------------------------------------------------------------------------
  /** RenderState wrapping the OpenGL function glAlphaFunc(), see also http://www.opengl.org/sdk/docs/man/xhtml/glAlphaFunc.xml for more information.
   * \sa Shader, Effect, Actor, vl::EN_ALPHA_TEST */
  class VLGRAPHICS_EXPORT AlphaFunc: public RenderState
  {
    VL_INSTRUMENT_CLASS(vl::AlphaFunc, RenderState)

  public:
    AlphaFunc(EFunction alphafunc=FU_ALWAYS, float refvalue=0): mRefValue(refvalue), mAlphaFunc(alphafunc)
    {
      VL_DEBUG_SET_OBJECT_NAME()
    }
    virtual ERenderState type() const { return RS_AlphaFunc; }
    virtual void apply(const Camera*, OpenGLContext* ctx) const;
    void set(EFunction alphafunc, float ref_value) { mAlphaFunc = alphafunc; mRefValue = ref_value; }
    EFunction alphaFunc() const { return mAlphaFunc; }
    float refValue() const { return mRefValue; }
  protected:
    float mRefValue;
    EFunction mAlphaFunc;
  };
  //------------------------------------------------------------------------------
  // Material
  //------------------------------------------------------------------------------
  /** RenderState wrapping the OpenGL function glMaterial() and glColorMaterial(), see also http://www.opengl.org/sdk/docs/man/xhtml/glMaterial.xml and http://www.opengl.org/sdk/docs/man/xhtml/glColorMaterial.xml for more information.
    * Under OpenGL ES 1.x the front material properties are used for both front and back faces.
   * \sa Shader, Effect, Actor, vl::EN_LIGHTING */
  class VLGRAPHICS_EXPORT Material: public RenderState
  {
    VL_INSTRUMENT_CLASS(vl::Material, RenderState)

  public:
    Material();
    virtual ERenderState type() const { return RS_Material; }
    virtual void apply(const Camera*, OpenGLContext* ctx) const;

    void setTransparency(float alpha);
    void setFrontTransparency(float alpha);
    void setBackTransparency(float alpha);
    void setFrontFlatColor(const fvec4& color);
    void setBackFlatColor(const fvec4& color);
    void setFlatColor(const fvec4& color);

    void setAmbient(fvec4 color)          { mFrontAmbient   = mBackAmbient   = color; }
    void setDiffuse(fvec4 color)          { mFrontDiffuse   = mBackDiffuse   = color; }
    void setSpecular(fvec4 color)         { mFrontSpecular  = mBackSpecular  = color; }
    void setEmission(fvec4 color)         { mFrontEmission  = mBackEmission  = color; }
    void setShininess(float shininess) { mFrontShininess = mBackShininess = shininess; }

    void setFrontAmbient(fvec4 color) { mFrontAmbient = color; }
    void setFrontDiffuse(fvec4 color) { mFrontDiffuse = color; }
    void setFrontSpecular(fvec4 color) { mFrontSpecular = color; }
    void setFrontEmission(fvec4 color) { mFrontEmission = color; }
    void setFrontShininess(float shininess) { mFrontShininess=shininess; }
    fvec4 frontAmbient() const { return mFrontAmbient; }
    fvec4 frontDiffuse() const { return mFrontDiffuse; }
    fvec4 frontSpecular() const { return mFrontSpecular; }
    fvec4 frontEmission() const { return mFrontEmission; }
    float frontShininess() const { return mFrontShininess; }

    void setBackAmbient(fvec4 color) { mBackAmbient = color; }
    void setBackDiffuse(fvec4 color) { mBackDiffuse = color; }
    void setBackSpecular(fvec4 color) { mBackSpecular = color; }
    void setBackEmission(fvec4 color) { mBackEmission = color; }
    void setBackShininess(float shininess) { mBackShininess=shininess; }
    fvec4 backAmbient() const { return mBackAmbient; }
    fvec4 backDiffuse() const { return mBackDiffuse; }
    fvec4 backSpecular() const { return mBackSpecular; }
    fvec4 backEmission() const { return mBackEmission; }
    float backShininess() const { return mBackShininess; }

    // color material

    void setColorMaterial(EPolygonFace face, EColorMaterial color) { mColorMaterialFace = face; mColorMaterial = color; }
    EPolygonFace colorMaterialFace() const { return mColorMaterialFace; }
    EColorMaterial colorMaterial() const { return mColorMaterial; }
    void setColorMaterialEnabled(bool enabled) { mColorMaterialEnabled = enabled; }
    bool colorMaterialEnabled() const { return mColorMaterialEnabled; }

  protected:
    fvec4 mFrontAmbient;
    fvec4 mFrontDiffuse;
    fvec4 mFrontSpecular;
    fvec4 mFrontEmission;
    fvec4 mBackAmbient;
    fvec4 mBackDiffuse;
    fvec4 mBackSpecular;
    fvec4 mBackEmission;
    float mBackShininess;
    float mFrontShininess;
    // color material
    EPolygonFace mColorMaterialFace;
    EColorMaterial mColorMaterial;
    bool mColorMaterialEnabled;
  };
  //------------------------------------------------------------------------------
  // LightModel
  //------------------------------------------------------------------------------
  /** RenderState wrapping the OpenGL function glLightModel(), see also http://www.opengl.org/sdk/docs/man/xhtml/glLightModel.xml for more information.
   * \sa Shader, Effect, Actor, vl::EN_LIGHTING */
  class VLGRAPHICS_EXPORT LightModel: public RenderState
  {
    VL_INSTRUMENT_CLASS(vl::LightModel, RenderState)

  public:
    LightModel(): mAmbientColor(0.2f,0.2f,0.2f,1.0f), mColorControl(CC_SINGLE_COLOR), mLocalViewer(false), mTwoSide(false)
    {
      VL_DEBUG_SET_OBJECT_NAME()
    }
    virtual ERenderState type() const { return RS_LightModel; }
    virtual void apply(const Camera*, OpenGLContext* ctx) const;
    void setLocalViewer(bool localviewer) { mLocalViewer = localviewer; }
    void setTwoSide(bool twoside) { mTwoSide = twoside; }
    void setColorControl(EColorControl colorcontrol) { mColorControl = colorcontrol; }
    void setAmbientColor(fvec4 ambientcolor) { mAmbientColor = ambientcolor; }
    bool localViewer() const { return mLocalViewer; }
    bool twoSide() const { return mTwoSide; }
    EColorControl colorControl() const { return mColorControl; }
    fvec4 ambientColor() const { return mAmbientColor; }
  protected:
    fvec4 mAmbientColor;
    EColorControl mColorControl;
    bool mLocalViewer;
    bool mTwoSide;
  };
  //------------------------------------------------------------------------------
  // Fog
  //------------------------------------------------------------------------------
  /** RenderState wrapping the OpenGL function glFog(), see also http://www.opengl.org/sdk/docs/man/xhtml/glFog.xml for more information.
   * \sa Shader, Effect, Actor, vl::EN_FOG */
  class VLGRAPHICS_EXPORT Fog: public RenderState
  {
    VL_INSTRUMENT_CLASS(vl::Fog, RenderState)

  public:
    Fog(EFogMode mode=FM_LINEAR, fvec4 color=fvec4(0,0,0,0), float density=1, float start=0, float end=1):
      mColor(color), mMode(mode), mDensity(density), mStart(start), mEnd(end)
    {
      VL_DEBUG_SET_OBJECT_NAME()
    }
    virtual ERenderState type() const { return RS_Fog; }
    virtual void apply(const Camera*, OpenGLContext* ctx) const;
    void set(EFogMode mode, fvec4 color, float density, float start, float end) { mColor = color; mMode = mode; mDensity = density; mStart = start; mEnd = end; }
    void setColor(fvec4 color) { mColor = color; }
    void setMode(EFogMode mode) { mMode = mode; }
    void setDensity(float density) { mDensity = density; }
    void setStart(float start) { mStart = start; }
    void setEnd(float end) { mEnd = end; }
    fvec4 color() const { return mColor; }
    EFogMode mode() const { return mMode; }
    float density() const { return mDensity; }
    float start() const { return mStart; }
    float end() const { return mEnd; }
  protected:
    fvec4 mColor;
    EFogMode mMode;
    float mDensity;
    float mStart;
    float mEnd;
  };
  //------------------------------------------------------------------------------
  // PolygonOffset
  //------------------------------------------------------------------------------
  /** RenderState wrapping the OpenGL function glPolygonOffset(), see also http://www.opengl.org/sdk/docs/man/xhtml/glPolygonOffset.xml for more information.
   * \sa Shader, Effect, Actor, vl::EN_POLYGON_OFFSET_FILL, vl::EN_POLYGON_OFFSET_LINE, vl::EN_POLYGON_OFFSET_POINT */
  class VLGRAPHICS_EXPORT PolygonOffset: public RenderState
  {
    VL_INSTRUMENT_CLASS(vl::PolygonOffset, RenderState)

  public:
    PolygonOffset(): mFactor(0.0f), mUnits(0.0f)
    {
      VL_DEBUG_SET_OBJECT_NAME()
    }
    virtual ERenderState type() const { return RS_PolygonOffset; }
    PolygonOffset(float factor, float units): mFactor(factor), mUnits(units) {}
    virtual void apply(const Camera*, OpenGLContext* ctx) const;
    void set(float factor, float units) { mFactor = factor; mUnits = units; }
    void setFactor(float factor) { mFactor = factor; }
    void setUnits(float units) { mUnits = units; }
    float factor() const { return mFactor; }
    float units() const { return mUnits; }
  protected:
    float mFactor;
    float mUnits;
  };
  //------------------------------------------------------------------------------
  // LogicOp
  //------------------------------------------------------------------------------
  /** RenderState wrapping the OpenGL function glLogicOp(), see also http://www.opengl.org/sdk/docs/man/xhtml/glLogicOp.xml for more information.
   * \sa Shader, Effect, Actor, vl::EN_COLOR_LOGIC_OP */
  class VLGRAPHICS_EXPORT LogicOp: public RenderState
  {
    VL_INSTRUMENT_CLASS(vl::LogicOp, RenderState)

  public:
    LogicOp(ELogicOp logicop=LO_COPY): mLogicOp(logicop)
    {
      VL_DEBUG_SET_OBJECT_NAME()
    }
    virtual ERenderState type() const { return RS_LogicOp; }
    virtual void apply(const Camera*, OpenGLContext* ctx) const;
    void set(ELogicOp logicop) { mLogicOp = logicop; }
    ELogicOp logicOp() const { return mLogicOp; }
  protected:
    ELogicOp mLogicOp;
  };
  //------------------------------------------------------------------------------
  // DepthRange
  //------------------------------------------------------------------------------
  /** RenderState wrapping the OpenGL function glDepthRange(), see also http://www.opengl.org/sdk/docs/man/xhtml/glDepthRange.xml for more information.
   * \sa Shader, Effect, Actor, vl::EN_DEPTH_TEST */
  class VLGRAPHICS_EXPORT DepthRange: public RenderState
  {
    VL_INSTRUMENT_CLASS(vl::DepthRange, RenderState)

  public:
    DepthRange(): mZNear(0), mZFar(1.0f)
    {
      VL_DEBUG_SET_OBJECT_NAME()
    }
    DepthRange(float znear, float zfar): mZNear(znear), mZFar(zfar)
    {
      VL_DEBUG_SET_OBJECT_NAME()
    }
    virtual ERenderState type() const { return RS_DepthRange; }
    virtual void apply(const Camera*, OpenGLContext* ctx) const;
    void set(float znear, float zfar) { mZNear = znear; mZFar = zfar; }
    void setZNear(float znear) { mZNear = znear; }
    void setZFar(float zfar) { mZFar = zfar; }
    float zNear() const { return mZNear; }
    float zFar() const { return mZFar; }
  protected:
    float mZNear;
    float mZFar;
  };
  //------------------------------------------------------------------------------
  // LineWidth
  //------------------------------------------------------------------------------
  /** RenderState wrapping the OpenGL function glLineWidth(), see also http://www.opengl.org/sdk/docs/man/xhtml/glLineWidth.xml for more information.
   * \sa Shader, Effect, Actor */
  class VLGRAPHICS_EXPORT LineWidth: public RenderState
  {
    VL_INSTRUMENT_CLASS(vl::LineWidth, RenderState)

  public:
    LineWidth(float linewidth=1.0f): mLineWidth(linewidth)
    {
      VL_DEBUG_SET_OBJECT_NAME()
    }
    virtual ERenderState type() const { return RS_LineWidth; }
    virtual void apply(const Camera*, OpenGLContext* ctx) const;
    void set(float linewidth) { mLineWidth = linewidth; }
    float lineWidth() const { return mLineWidth; }
  protected:
    float mLineWidth;
  };
  //------------------------------------------------------------------------------
  // PointSize
  //------------------------------------------------------------------------------
  /** RenderState wrapping the OpenGL function glPointSize(), see also http://www.opengl.org/sdk/docs/man/xhtml/glPointSize.xml for more information.
   * \sa Shader, Effect, Actor, vl::EN_POINT_SMOOTH, vl::EN_POINT_SPRITE */
  class VLGRAPHICS_EXPORT PointSize: public RenderState
  {
    VL_INSTRUMENT_CLASS(vl::PointSize, RenderState)

  public:
    PointSize(float pointsize=1.0f): mPointSize(pointsize)
    {
      VL_DEBUG_SET_OBJECT_NAME()
    }
    virtual ERenderState type() const { return RS_PointSize; }
    virtual void apply(const Camera*, OpenGLContext* ctx) const;
    void set(float pointsize) { mPointSize = pointsize; }
    float pointSize() const { return mPointSize; }
  protected:
    float mPointSize;
  };
  //------------------------------------------------------------------------------
  // PolygonStipple
  //------------------------------------------------------------------------------
  /** RenderState wrapping the OpenGL function glPolygonStipple(), see also http://www.opengl.org/sdk/docs/man/xhtml/glPolygonStipple.xml for more information.
   * \sa Shader, Effect, Actor, vl::EN_POLYGON_STIPPLE */
  class VLGRAPHICS_EXPORT PolygonStipple: public RenderState
  {
    VL_INSTRUMENT_CLASS(vl::PolygonStipple, RenderState)

  public:
    PolygonStipple();
    PolygonStipple(const unsigned char* mask);
    virtual ERenderState type() const { return RS_PolygonStipple; }
    virtual void apply(const Camera*, OpenGLContext* ctx) const;
    void set(const unsigned char* mask);
    const unsigned char* mask() const { return mMask; }
  protected:
    unsigned char mMask[32*32/8];
  };
  //------------------------------------------------------------------------------
  // LineStipple
  //------------------------------------------------------------------------------
  /** RenderState wrapping the OpenGL function glLineStipple(), see also http://www.opengl.org/sdk/docs/man/xhtml/glLineStipple.xml for more information.
   * \sa Shader, Effect, Actor, vl::EN_LINE_STIPPLE */
  class VLGRAPHICS_EXPORT LineStipple: public RenderState
  {
    VL_INSTRUMENT_CLASS(vl::LineStipple, RenderState)

  public:
    LineStipple(int factor=1, GLushort pattern=~(GLushort)0): mFactor(factor), mPattern(pattern)
    {
      VL_DEBUG_SET_OBJECT_NAME()
    }
    virtual ERenderState type() const { return RS_LineStipple; }
    virtual void apply(const Camera*, OpenGLContext* ctx) const;
    void set(int factor, GLushort pattern) { mFactor = factor; mPattern = pattern; }
    void setFactor(int factor) { mFactor = factor; }
    void setPattern(GLushort pattern) { mPattern = pattern; }
    int factor() const { return mFactor; }
    GLushort pattern() const { return mPattern; }
  protected:
    int mFactor;
    GLushort mPattern;
  };
  //------------------------------------------------------------------------------
  // PointParameter
  //------------------------------------------------------------------------------
  /** RenderState wrapping the OpenGL function glPointParameter(), see also http://www.opengl.org/sdk/docs/man/xhtml/glPointParameter.xml for more information.
   * \sa Shader, Effect, Actor */
  class VLGRAPHICS_EXPORT PointParameter: public RenderState
  {
    VL_INSTRUMENT_CLASS(vl::PointParameter, RenderState)

  public:
    PointParameter(float sizemin=0, float sizemax=1024.0f, float fadethresholdsize=1.0f, fvec3 distanceattenuation=fvec3(1,0,0)):
      mDistanceAttenuation(distanceattenuation), mSizeMin(sizemin), mSizeMax(sizemax), mFadeThresholdSize(fadethresholdsize),
      mPointSpriteCoordOrigin(PPCO_UPPER_LEFT)
    {
      VL_DEBUG_SET_OBJECT_NAME()
    }
    virtual ERenderState type() const { return RS_PointParameter; }
    virtual void apply(const Camera*, OpenGLContext* ctx) const;
    void set(float sizemin, float sizemax, float fadethresholdsize, fvec3 distanceattenuation) { mDistanceAttenuation = distanceattenuation; mSizeMin = sizemin; mSizeMax = sizemax; mFadeThresholdSize = fadethresholdsize; }
    void setDistanceAttenuation(fvec3 attenuation) { mDistanceAttenuation = attenuation; }
    void setSizeMin(float sizemin) { mSizeMin = sizemin; }
    void setSizeMax(float sizemax) { mSizeMax = sizemax; }
    void setFadeThresholdSize(float threshold) { mFadeThresholdSize = threshold; }
    fvec3 distanceAttenuation() const { return mDistanceAttenuation; }
    float sizeMin() const { return mSizeMin; }
    float sizeMax() const { return mSizeMax; }
    float fadeThresholdSize() const { return mFadeThresholdSize; }
    EPointSpriteCoordOrigin pointSpriteCoordOrigin() const { return mPointSpriteCoordOrigin; }
    void setPointSpriteCoordOrigin(EPointSpriteCoordOrigin orig) { mPointSpriteCoordOrigin = orig; }
  protected:
    fvec3 mDistanceAttenuation;
    float mSizeMin;
    float mSizeMax;
    float mFadeThresholdSize;
    EPointSpriteCoordOrigin mPointSpriteCoordOrigin;
  };
  //------------------------------------------------------------------------------
  // StencilFunc
  //------------------------------------------------------------------------------
  /** Wraps the OpenGL functions glStencilFunc() and glStencilFuncSeparate(), see also 
   * http://www.opengl.org/sdk/docs/man/xhtml/glStencilFunc.xml and
   * http://www.opengl.org/sdk/docs/man/xhtml/glStencilFuncSeparate.xml for more information.
   * \sa Shader, Effect, Actor, StencilMask, StencilOp, vl::EN_STENCIL_TEST */
  class VLGRAPHICS_EXPORT StencilFunc: public RenderState
  {
    VL_INSTRUMENT_CLASS(vl::StencilFunc, RenderState)

  public:
    StencilFunc(EFunction function=FU_ALWAYS, int refvalue=0, unsigned int mask=~(unsigned int)0): 
        mFunction_Front(function), mFunction_Back(function), 
        mRefValue_Front(refvalue), mRefValue_Back(refvalue), 
        mMask_Front(mask),         mMask_Back(mask)
    {
      VL_DEBUG_SET_OBJECT_NAME()
    }
    virtual ERenderState type() const { return RS_StencilFunc; }
    virtual void apply(const Camera*, OpenGLContext* ctx) const;
    void set(EPolygonFace face, EFunction function, int refvalue, unsigned int mask) 
    { 
      if (face == PF_FRONT || face == PF_FRONT_AND_BACK)
      {
        mFunction_Front = function; 
        mRefValue_Front = refvalue; 
        mMask_Front     = mask; 
      }
      if (face == PF_BACK || face == PF_FRONT_AND_BACK)
      {
        mFunction_Back = function; 
        mRefValue_Back = refvalue; 
        mMask_Back     = mask; 
      }
    }
    EFunction function_Front() const { return mFunction_Front; }
    int refValue_Front() const { return mRefValue_Front; }
    unsigned int mask_Front() const { return mMask_Front; }
    EFunction function_Back() const { return mFunction_Back; }
    int refValue_Back() const { return mRefValue_Back; }
    unsigned int mask_Back() const { return mMask_Back; }
  protected:
    EFunction mFunction_Front;
    EFunction mFunction_Back;
    int     mRefValue_Front;
    int     mRefValue_Back;
    unsigned int    mMask_Front;
    unsigned int    mMask_Back;
  };
  //------------------------------------------------------------------------------
  // StencilOp
  //------------------------------------------------------------------------------
  /** RenderState wrapping the OpenGL function glStencilOp() and glStencilOpSeparate(), see also 
   * http://www.opengl.org/sdk/docs/man/xhtml/glStencilOp.xml and
   * http://www.opengl.org/sdk/docs/man/xhtml/glStencilOpSeparate.xml for more information.
   * \sa Shader, Effect, Actor, StencilMask, StencilFunc, vl::EN_STENCIL_TEST */
  class VLGRAPHICS_EXPORT StencilOp: public RenderState
  {
    VL_INSTRUMENT_CLASS(vl::StencilOp, RenderState)

  public:
    StencilOp(EStencilOp sfail=SO_KEEP, EStencilOp dpfail=SO_KEEP, EStencilOp dppass=SO_KEEP): 
        mSFail_Front(sfail),   mSFail_Back(sfail), 
        mDpFail_Front(dpfail), mDpFail_Back(dpfail), 
        mDpPass_Front(dppass), mDpPass_Back(dppass)
    {
      VL_DEBUG_SET_OBJECT_NAME()
    }
    virtual ERenderState type() const { return RS_StencilOp; }
    virtual void apply(const Camera*, OpenGLContext* ctx) const;
    void set(EPolygonFace face, EStencilOp sfail, EStencilOp dpfail, EStencilOp dppass) 
    { 
      if (face == PF_FRONT || face == PF_FRONT_AND_BACK)
      {
        mSFail_Front  = sfail; 
        mDpFail_Front = dpfail; 
        mDpPass_Front = dppass; 
      }
      if (face == PF_BACK || face == PF_FRONT_AND_BACK)
      {
        mSFail_Back  = sfail; 
        mDpFail_Back = dpfail; 
        mDpPass_Back = dppass; 
      }
    }
    EStencilOp sFail_Front()  const { return mSFail_Front;  }
    EStencilOp dpFail_Front() const { return mDpFail_Front; }
    EStencilOp dpPass_Front() const { return mDpPass_Front; }
    EStencilOp sFail_Back()   const { return mSFail_Front;  }
    EStencilOp dpFail_Back()  const { return mDpFail_Front; }
    EStencilOp dpPass_Back()  const { return mDpPass_Front; }
  protected:
    EStencilOp mSFail_Front;
    EStencilOp mSFail_Back;
    EStencilOp mDpFail_Front;
    EStencilOp mDpFail_Back;
    EStencilOp mDpPass_Front;
    EStencilOp mDpPass_Back;
  };
  //------------------------------------------------------------------------------
  // StencilMask
  //------------------------------------------------------------------------------
  /** RenderState wrapping the OpenGL function glStencilMask() and glStencilMaskSeparate(), see also 
   * http://www.opengl.org/sdk/docs/man/xhtml/glStencilMask.xml and
   * http://www.opengl.org/sdk/docs/man/xhtml/glStencilMaskSeparate.xml for more information.
   * \sa Shader, Effect, Actor, StencilOp, StencilFunc, vl::EN_STENCIL_TEST */
  class VLGRAPHICS_EXPORT StencilMask: public RenderState
  {
    VL_INSTRUMENT_CLASS(vl::StencilMask, RenderState)

  public:
    StencilMask(unsigned int mask=~(unsigned int)0): mMask_Front(mask), mMask_Back(mask)
    {
      VL_DEBUG_SET_OBJECT_NAME()
    }
    virtual ERenderState type() const { return RS_StencilMask; }
    virtual void apply(const Camera*, OpenGLContext* ctx) const;
    void set(EPolygonFace face, unsigned int mask) 
    { 
      if (face == PF_FRONT || face == PF_FRONT_AND_BACK)
        mMask_Front = mask; 
      if (face == PF_BACK  || face == PF_FRONT_AND_BACK)
        mMask_Back = mask;       
    }
    unsigned int mask_Front() const { return mMask_Front; }
    unsigned int mask_Back() const { return mMask_Back; }
  protected:
    unsigned int mMask_Front;
    unsigned int mMask_Back;
  };
  //------------------------------------------------------------------------------
  // BlendColor
  //------------------------------------------------------------------------------
  /** RenderState wrapping the OpenGL function glBlendColor(), see also http://www.opengl.org/sdk/docs/man/xhtml/glBlendColor.xml for more information.
   * \sa Shader, Effect, Actor */
  class VLGRAPHICS_EXPORT BlendColor: public RenderState
  {
    VL_INSTRUMENT_CLASS(vl::BlendColor, RenderState)

  public:
    BlendColor(fvec4 blendcolor=fvec4(0,0,0,0)): mBlendColor(blendcolor)
    {
      VL_DEBUG_SET_OBJECT_NAME()
    }
    virtual ERenderState type() const { return RS_BlendColor; }
    virtual void apply(const Camera*, OpenGLContext* ctx) const;
    void set(fvec4 blendcolor) { mBlendColor = blendcolor; }
    fvec4 blendColor() const { return mBlendColor; }
  protected:
    fvec4 mBlendColor;
  };
  //------------------------------------------------------------------------------
  // ColorMask
  //------------------------------------------------------------------------------
  /** RenderState wrapping the OpenGL function glColorMask(), see also http://www.opengl.org/sdk/docs/man/xhtml/glColorMask.xml for more information.
   * \sa Shader, Effect, Actor */
  class VLGRAPHICS_EXPORT ColorMask: public RenderState
  {
    VL_INSTRUMENT_CLASS(vl::ColorMask, RenderState)

  public:
    ColorMask(bool red=true, bool green=true, bool blue=true, bool alpha=true): mRed(red), mGreen(green), mBlue(blue), mAlpha(alpha)
    {
      VL_DEBUG_SET_OBJECT_NAME()
    }
    virtual ERenderState type() const { return RS_ColorMask; }
    virtual void apply(const Camera*, OpenGLContext* ctx) const;
    void set(bool red, bool green, bool blue, bool alpha) { mRed = red; mGreen = green; mBlue = blue; mAlpha = alpha; }
    void setRed(bool red) { mRed = red; }
    void setGreen(bool green) { mGreen = green; }
    void setBlue(bool blue) { mBlue = blue; }
    void setAlpha(bool alpha) { mAlpha = alpha; }
    bool red() const { return mRed; }
    bool green() const { return mGreen; }
    bool blue() const { return mBlue; }
    bool alpha() const { return mAlpha; }
  protected:
    bool mRed;
    bool mGreen;
    bool mBlue;
    bool mAlpha;
  };
  //------------------------------------------------------------------------------
  // TextureMatrix
  //------------------------------------------------------------------------------
  /** The TextureMatrix class uses a 4x4 matrix to transform the texture coordinates of a texture unit.
   *
   * \sa Shader, TextureSampler, Texture, TexGen, TexParameter, Effect, Actor */
  class VLGRAPHICS_EXPORT TextureMatrix: public TextureState
  {
    VL_INSTRUMENT_CLASS(vl::TextureMatrix, TextureState)

  public:
    TextureMatrix(int texunit) { mTextureSampler=texunit; mUseCameraRotationInverse = false; }
    virtual ERenderState type() const { return (ERenderState)(RS_TextureMatrix0 + mTextureSampler); }
    virtual void apply(const Camera*, OpenGLContext* ctx) const;
    const mat4& matrix() const { return mMatrix; }
    const TextureMatrix& setMatrix(const mat4& matrix)
    {
      mMatrix = matrix;
      return *this;
    }
    /** Set this to \a true when you want your cubemap to appear in world space rather than eye space.
     *
     * When setUseCameraRotationInverse() is true the texture matrix is computed as:
     * \code matrix()*camera->localMatrix().as3x3() \endcode
     * This way matrix() represents the transform of the texture in world coordinates, for example if you want to reorient 
     * your cubemap in world space or when you want to rotate the direction of one or more highlights prerendered in a cubemap. */
    void setUseCameraRotationInverse(bool use) { mUseCameraRotationInverse = use; }
    bool useCameraRotationInverse() const { return mUseCameraRotationInverse; }
  protected:
    mat4 mMatrix;
    bool mUseCameraRotationInverse;
  };
  //------------------------------------------------------------------------------
  // TexEnv
  //------------------------------------------------------------------------------
  /** RenderState wrapping the OpenGL function glTexEnv(), see also http://www.opengl.org/sdk/docs/man/xhtml/glTexEnv.xml for more information.
   *
   * \note
   * A TexParameter defines a set of variables associated to a Texture while 
   * TexGen and TexEnv define a set of variables associated to a TextureSampler.
   *
   * \sa Shader, TextureSampler, Texture, TexGen, TexParameter, Effect, Actor */
  class VLGRAPHICS_EXPORT TexEnv: public TextureState
  {
    VL_INSTRUMENT_CLASS(vl::TexEnv, TextureState)

  public:
    TexEnv(int texunit);
    virtual ERenderState type() const { return (ERenderState)(RS_TexEnv0 + mTextureSampler); }
    virtual void apply(const Camera*, OpenGLContext* ctx) const;

    void setMode(ETexEnvMode mode) { mMode = mode; }
    ETexEnvMode mode() const { return mMode; }
    void setCombineRGB(ETexEnvMode combineRGB) { mCombineRGB = combineRGB; }
    ETexEnvMode combineRGB() const { return mCombineRGB; }
    void setCombineAlpha(ETexEnvMode combineAlpha) { mCombineAlpha = combineAlpha; }
    ETexEnvMode combineAlpha() const { return mCombineAlpha; }
    void setColor(fvec4 color) { mColor = color; }
    fvec4 color() const { return mColor; }
    void setRGBScale(float rgbscale) { mRGBScale = rgbscale; }
    float rgbScale() const { return mRGBScale; }
    void setAlphaScale(float alphascale) { mAlphaScale = alphascale; }
    float alphaScale() const { return mAlphaScale; }

    void setSource0RGB(ETexEnvSource source) { mSource0RGB = source; }
    void setSource1RGB(ETexEnvSource source) { mSource1RGB = source; }
    void setSource2RGB(ETexEnvSource source) { mSource2RGB = source; }
    ETexEnvSource source0RGB() const { return mSource0RGB; }
    ETexEnvSource source1RGB() const { return mSource1RGB; }
    ETexEnvSource source2RGB() const { return mSource2RGB; }
    void setSource0Alpha(ETexEnvSource source) { mSource0Alpha = source; }
    void setSource1Alpha(ETexEnvSource source) { mSource1Alpha = source; }
    void setSource2Alpha(ETexEnvSource source) { mSource2Alpha = source; }
    ETexEnvSource source0Alpha() const { return mSource0Alpha; }
    ETexEnvSource source1Alpha() const { return mSource1Alpha; }
    ETexEnvSource source2Alpha() const { return mSource2Alpha; }

    void setOperand0RGB(ETexEnvOperand operand) { mOperand0RGB = operand; }
    void setOperand1RGB(ETexEnvOperand operand) { mOperand1RGB = operand; }
    void setOperand2RGB(ETexEnvOperand operand) { mOperand2RGB = operand; }
    ETexEnvOperand operand0RGB() const { return mOperand0RGB; }
    ETexEnvOperand operand1RGB() const { return mOperand1RGB; }
    ETexEnvOperand operand2RGB() const { return mOperand2RGB; }
    void setOperand0Alpha(ETexEnvOperand operand) { mOperand0Alpha = operand; }
    void setOperand1Alpha(ETexEnvOperand operand) { mOperand1Alpha = operand; }
    void setOperand2Alpha(ETexEnvOperand operand) { mOperand2Alpha = operand; }
    ETexEnvOperand operand0Alpha() const { return mOperand0Alpha; }
    ETexEnvOperand operand1Alpha() const { return mOperand1Alpha; }
    ETexEnvOperand operand2Alpha() const { return mOperand2Alpha; }

    void setPointSpriteCoordReplace(bool replace) { mPointSpriteCoordReplace = replace; }
    bool pointSpriteCoordReplace() const { return mPointSpriteCoordReplace; }

    void setLodBias(float lodbias) { mLodBias = lodbias; }
    float lodBias() const { return mLodBias; }

  public:
    fvec4 mColor;
    float mRGBScale;
    float mAlphaScale;
    ETexEnvMode mMode;
    ETexEnvMode mCombineRGB;
    ETexEnvMode mCombineAlpha;
    ETexEnvSource mSource0RGB;
    ETexEnvSource mSource1RGB;
    ETexEnvSource mSource2RGB;
    ETexEnvSource mSource0Alpha;
    ETexEnvSource mSource1Alpha;
    ETexEnvSource mSource2Alpha;
    ETexEnvOperand mOperand0RGB;
    ETexEnvOperand mOperand1RGB;
    ETexEnvOperand mOperand2RGB;
    ETexEnvOperand mOperand0Alpha;
    ETexEnvOperand mOperand1Alpha;
    ETexEnvOperand mOperand2Alpha;
    float mLodBias;
    bool mPointSpriteCoordReplace;
  };
  //------------------------------------------------------------------------------
  // TexGen
  //------------------------------------------------------------------------------
  /** RenderState wrapping the OpenGL function glTexGen(), see also http://www.opengl.org/sdk/docs/man/xhtml/glTexGen.xml for more information.
   *
   * \note
   * A TexParameter defines a set of variables associated to a Texture while 
   * TexGen and TexEnv define a set of variables associated to a TextureSampler.
   *
   * \sa Shader, TextureSampler, Texture, TexGen, TexParameter, Effect, Actor */
  class VLGRAPHICS_EXPORT TexGen: public TextureState
  {
    VL_INSTRUMENT_CLASS(vl::TexGen, TextureState)

  public:
    TexGen(int texunit);

    virtual ERenderState type() const { return (ERenderState)(RS_TexGen0 + mTextureSampler); }
    virtual void apply(const Camera*, OpenGLContext* ctx) const;

    void setEyePlaneS(fvec4 plane) { mEyePlaneS = plane; }
    void setObjectPlaneS(fvec4 plane) { mObjectPlaneS = plane; }
    void setGenModeS(ETexGenMode mode) { mGenModeS = mode; }
    fvec4 eyePlaneS() const { return mEyePlaneS; }
    fvec4 objectPlaneS() const { return mObjectPlaneS; }
    ETexGenMode genModeS() const { return mGenModeS; }

    void setEyePlaneT(fvec4 plane) { mEyePlaneT = plane; }
    void setObjectPlaneT(fvec4 plane) { mObjectPlaneT = plane; }
    void setGenModeT(ETexGenMode mode) { mGenModeT = mode; }
    fvec4 eyePlaneT() const { return mEyePlaneT; }
    fvec4 objectPlaneT() const { return mObjectPlaneT; }
    ETexGenMode genModeT() const { return mGenModeT; }

    void setEyePlaneR(fvec4 plane) { mEyePlaneR = plane; }
    void setObjectPlaneR(fvec4 plane) { mObjectPlaneR = plane; }
    void setGenModeR(ETexGenMode mode) { mGenModeR = mode; }
    fvec4 eyePlaneR() const { return mEyePlaneR; }
    fvec4 objectPlaneR() const { return mObjectPlaneR; }
    ETexGenMode genModeR() const { return mGenModeR; }

    void setEyePlaneQ(fvec4 plane) { mEyePlaneQ = plane; }
    void setObjectPlaneQ(fvec4 plane) { mObjectPlaneQ = plane; }
    void setGenModeQ(ETexGenMode mode) { mGenModeQ = mode; }
    fvec4 eyePlaneQ() const { return mEyePlaneQ; }
    fvec4 objectPlaneQ() const { return mObjectPlaneQ; }
    ETexGenMode genModeQ() const { return mGenModeQ; }

  public:
    fvec4 mEyePlaneS;
    fvec4 mObjectPlaneS;
    fvec4 mEyePlaneT;
    fvec4 mObjectPlaneT;
    fvec4 mEyePlaneR;
    fvec4 mObjectPlaneR;
    fvec4 mEyePlaneQ;
    fvec4 mObjectPlaneQ;
    ETexGenMode mGenModeS;
    ETexGenMode mGenModeT;
    ETexGenMode mGenModeR;
    ETexGenMode mGenModeQ;
  };
  //------------------------------------------------------------------------------
  // TextureSampler
  //------------------------------------------------------------------------------
  /** The TextureSampler class associates a Texture object to an OpenGL texture unit.
   *
   * \sa Texture, TexParameter, Shader, TextureMatrix, TexEnv, TexGen, Effect, Actor */
  class VLGRAPHICS_EXPORT TextureSampler: public TextureState
  {
    VL_INSTRUMENT_CLASS(vl::TextureSampler, TextureState)

  public:
    TextureSampler(int texunit)
    {
      VL_DEBUG_SET_OBJECT_NAME()
      mTextureSampler = texunit;
    }

    virtual ERenderState type() const { return (ERenderState)(RS_TextureSampler0 + textureUnit()); }
    virtual void apply(const Camera*, OpenGLContext* ctx) const;

    void setTexture(Texture* texture) { mTexture = texture; }
    Texture* texture() { return mTexture.get(); }
    const Texture* texture() const { return mTexture.get(); }

    bool hasTexture() const;

  protected:
    ref<Texture> mTexture;
  };
  //------------------------------------------------------------------------------
  // ShaderAnimator
  //------------------------------------------------------------------------------
  /** Callback object used to update/animate a Shader during the rendering. 
  The updateShader() method will be called whenever a visible object uses the 
  Shader to which the ShaderAnimator is bound.
  \sa Shader::setUpdater(); */
  class VLGRAPHICS_EXPORT ShaderAnimator: public Object
  {
    VL_INSTRUMENT_CLASS(vl::ShaderAnimator, Object)

  public:
    ShaderAnimator(): mEnabled(true) {}

    /** Reimplement this function to update/animate a Shader.
    \param shader the Shader to be updated.
    \param camera the camera used for the current rendering.
    \param cur_time the current animation time.
    \sa Shader::setShaderAnimator(); */
    virtual void updateShader(Shader* shader, Camera* camera, Real cur_time) = 0;

    /** Whether the ShaderAnimator is enabled or not. */
    void setEnabled(bool enable) { mEnabled = enable; }

    /** Whether the ShaderAnimator is enabled or not. */
    bool isEnabled() const { return mEnabled; }

  protected:
    bool mEnabled;
  };
  //------------------------------------------------------------------------------
  // Shader
  //------------------------------------------------------------------------------
  /** Manages most of the OpenGL rendering states responsible of the final aspect of the rendered objects.
   *
   * A Shader represents a rendering pass. Using the Effect class you can assign 
   * more rendering passes to an Actor. You can also specify different sets of 
   * rendering passes for each Effect LOD, for more information see Effect.
   *
   * \remarks
   * The Uniforms defined in the Actor and the ones defined in the Shader must not
   * overlap, that is, an Actor must  not define Uniforms present in the 
   * Shader and vice versa.
   *
   * \sa Effect, Actor */
  class VLGRAPHICS_EXPORT Shader: public Object
  {
    VL_INSTRUMENT_CLASS(vl::Shader, Object)

  public:
    /** Constructor. */
    Shader();
    
    /** Copy constructor. Calls \p operator=(other). */
    Shader(const Shader& other): Object(other) { operator=(other); }

    /** Destructor */
    virtual ~Shader();

    /** Copy RenderStates, Enables, Uniforms and Scissor from another Shader. 
      * Note that the destination Shader still keeps its own copy of the RenderStateSet, EnableSet and UniformSet objects. */
    Shader& operator=(const Shader& other) 
    {
      Object::operator=(other);

      // we don't copy the update time
      // mLastUpdateTime = other.mLastUpdateTime;

      if (other.mRenderStateSet.get())
      {
        if (mRenderStateSet.get() == NULL)
          mRenderStateSet = new RenderStateSet;
        *mRenderStateSet = *other.mRenderStateSet;
      }
      else
        mRenderStateSet = NULL;

      if (other.mEnableSet.get())
      {
        if (mEnableSet.get() == NULL)
          mEnableSet = new EnableSet;
        *mEnableSet = *other.mEnableSet;
      }
      else
        mEnableSet = NULL;

      if (other.mUniformSet.get())
      {
        if (mUniformSet.get() == NULL)
          mUniformSet = new UniformSet;
        *mUniformSet = *other.mUniformSet;
      }
      else
        mUniformSet = NULL;

      mScissor = other.mScissor;
      mShaderAnimator = other.mShaderAnimator;
          
      #if VL_SHADER_USER_DATA
        mShaderUserData = other.mShaderUserData;
      #endif

      return *this;
    }

    /** Equivalent to \p "Shader& operator=(const Shader&)" */
    Shader& copy(const Shader& other) 
    {
      return operator=(other);
    }

    //! Disables everything, erases all the render states, erases all the uniforms.
    void reset()
    {
      disableAll();
      eraseAllRenderStates();
      eraseAllUniforms();
    }

    // state getters

    /** Gets or creates a GLSLProgram and returns it. */
    GLSLProgram* gocGLSLProgram();

    /** Returns a GLSLProgram if it exists or NULL otherwise. */
    const GLSLProgram* getGLSLProgram() const;

    /** Returns a GLSLProgram if it exists or NULL otherwise. */
    GLSLProgram* getGLSLProgram();

    PixelTransfer* gocPixelTransfer();
    const PixelTransfer* getPixelTransfer() const { return static_cast<const PixelTransfer*>( getRenderStateSet()->renderState( RS_PixelTransfer) ); }
    PixelTransfer* getPixelTransfer() { return static_cast<PixelTransfer*>( getRenderStateSet()->renderState( RS_PixelTransfer) ); }

    Hint* gocHint();
    const Hint* getHint() const { return static_cast<const Hint*>( getRenderStateSet()->renderState( RS_Hint) ); }
    Hint* getHint() { return static_cast<Hint*>( getRenderStateSet()->renderState( RS_Hint) ); }

    CullFace* gocCullFace();
    const CullFace* getCullFace() const { return static_cast<const CullFace*>( getRenderStateSet()->renderState( RS_CullFace ) ); }
    CullFace* getCullFace() { return static_cast<CullFace*>( getRenderStateSet()->renderState( RS_CullFace ) ); }

    FrontFace* gocFrontFace();
    const FrontFace* getFrontFace() const { return static_cast<const FrontFace*>( getRenderStateSet()->renderState( RS_FrontFace ) ); }
    FrontFace* getFrontFace() { return static_cast<FrontFace*>( getRenderStateSet()->renderState( RS_FrontFace ) ); }

    DepthFunc* gocDepthFunc();
    const DepthFunc* getDepthFunc() const { return static_cast<const DepthFunc*>( getRenderStateSet()->renderState( RS_DepthFunc ) ); }
    DepthFunc* getDepthFunc() { return static_cast<DepthFunc*>( getRenderStateSet()->renderState( RS_DepthFunc ) ); }

    DepthMask* gocDepthMask();
    const DepthMask* getDepthMask() const { return static_cast<const DepthMask*>( getRenderStateSet()->renderState( RS_DepthMask ) ); }
    DepthMask* getDepthMask() { return static_cast<DepthMask*>( getRenderStateSet()->renderState( RS_DepthMask ) ); }

    ColorMask* gocColorMask();
    const ColorMask* getColorMask() const { return static_cast<const ColorMask*>( getRenderStateSet()->renderState( RS_ColorMask ) ); }
    ColorMask* getColorMask() { return static_cast<ColorMask*>( getRenderStateSet()->renderState( RS_ColorMask ) ); }

    PolygonMode* gocPolygonMode();
    const PolygonMode* getPolygonMode() const { return static_cast<const PolygonMode*>( getRenderStateSet()->renderState( RS_PolygonMode ) ); }
    PolygonMode* getPolygonMode() { return static_cast<PolygonMode*>( getRenderStateSet()->renderState( RS_PolygonMode ) ); }

    ShadeModel* gocShadeModel();
    const ShadeModel* getShadeModel() const { return static_cast<const ShadeModel*>( getRenderStateSet()->renderState( RS_ShadeModel ) ); }
    ShadeModel* getShadeModel() { return static_cast<ShadeModel*>( getRenderStateSet()->renderState( RS_ShadeModel ) ); }

    BlendEquation* gocBlendEquation();
    const BlendEquation* getBlendEquation() const { return static_cast<const BlendEquation*>( getRenderStateSet()->renderState( RS_BlendEquation ) ); }
    BlendEquation* getBlendEquation() { return static_cast<BlendEquation*>( getRenderStateSet()->renderState( RS_BlendEquation ) ); }

    AlphaFunc* gocAlphaFunc();
    const AlphaFunc* getAlphaFunc() const { return static_cast<const AlphaFunc*>( getRenderStateSet()->renderState( RS_AlphaFunc ) ); }
    AlphaFunc* getAlphaFunc() { return static_cast<AlphaFunc*>( getRenderStateSet()->renderState( RS_AlphaFunc ) ); }

    Material* gocMaterial();
    const Material* getMaterial() const { return static_cast<const Material*>( getRenderStateSet()->renderState( RS_Material ) ); }
    Material* getMaterial() { return static_cast<Material*>( getRenderStateSet()->renderState( RS_Material ) ); }

    LightModel* gocLightModel();
    const LightModel* getLightModel() const { return static_cast<const LightModel*>( getRenderStateSet()->renderState( RS_LightModel ) ); }
    LightModel* getLightModel() { return static_cast<LightModel*>( getRenderStateSet()->renderState( RS_LightModel ) ); }

    Fog* gocFog();
    const Fog* getFog() const { return static_cast<const Fog*>( getRenderStateSet()->renderState( RS_Fog ) ); }
    Fog* getFog() { return static_cast<Fog*>( getRenderStateSet()->renderState( RS_Fog ) ); }

    PolygonOffset* gocPolygonOffset();
    const PolygonOffset* getPolygonOffset() const { return static_cast<const PolygonOffset*>( getRenderStateSet()->renderState( RS_PolygonOffset ) ); }
    PolygonOffset* getPolygonOffset() { return static_cast<PolygonOffset*>( getRenderStateSet()->renderState( RS_PolygonOffset ) ); }

    LogicOp* gocLogicOp();
    const LogicOp* getLogicOp() const { return static_cast<const LogicOp*>( getRenderStateSet()->renderState( RS_LogicOp ) ); }
    LogicOp* getLogicOp() { return static_cast<LogicOp*>( getRenderStateSet()->renderState( RS_LogicOp ) ); }

    DepthRange* gocDepthRange();
    const DepthRange* getDepthRange() const { return static_cast<const DepthRange*>( getRenderStateSet()->renderState( RS_DepthRange ) ); }
    DepthRange* getDepthRange() { return static_cast<DepthRange*>( getRenderStateSet()->renderState( RS_DepthRange ) ); }

    LineWidth* gocLineWidth();
    const LineWidth* getLineWidth() const { return static_cast<const LineWidth*>( getRenderStateSet()->renderState( RS_LineWidth ) ); }
    LineWidth* getLineWidth() { return static_cast<LineWidth*>( getRenderStateSet()->renderState( RS_LineWidth ) ); }

    PointSize* gocPointSize();
    const PointSize* getPointSize() const { return static_cast<const PointSize*>( getRenderStateSet()->renderState( RS_PointSize ) ); }
    PointSize* getPointSize() { return static_cast<PointSize*>( getRenderStateSet()->renderState( RS_PointSize ) ); }

    LineStipple* gocLineStipple();
    const LineStipple* getLineStipple() const { return static_cast<const LineStipple*>( getRenderStateSet()->renderState( RS_LineStipple ) ); }
    LineStipple* getLineStipple() { return static_cast<LineStipple*>( getRenderStateSet()->renderState( RS_LineStipple ) ); }

    PolygonStipple* gocPolygonStipple();
    const PolygonStipple* getPolygonStipple() const { return static_cast<const PolygonStipple*>( getRenderStateSet()->renderState( RS_PolygonStipple ) ); }
    PolygonStipple* getPolygonStipple() { return static_cast<PolygonStipple*>( getRenderStateSet()->renderState( RS_PolygonStipple ) ); }

    PointParameter* gocPointParameter();
    const PointParameter* getPointParameter() const { return static_cast<const PointParameter*>( getRenderStateSet()->renderState( RS_PointParameter ) ); }
    PointParameter* getPointParameter() { return static_cast<PointParameter*>( getRenderStateSet()->renderState( RS_PointParameter ) ); }

    StencilFunc* gocStencilFunc();
    const StencilFunc* getStencilFunc() const { return static_cast<const StencilFunc*>( getRenderStateSet()->renderState( RS_StencilFunc ) ); }
    StencilFunc* getStencilFunc() { return static_cast<StencilFunc*>( getRenderStateSet()->renderState( RS_StencilFunc ) ); }

    StencilOp* gocStencilOp();
    const StencilOp* getStencilOp() const { return static_cast<const StencilOp*>( getRenderStateSet()->renderState( RS_StencilOp ) ); }
    StencilOp* getStencilOp() { return static_cast<StencilOp*>( getRenderStateSet()->renderState( RS_StencilOp ) ); }

    StencilMask* gocStencilMask();
    const StencilMask* getStencilMask() const { return static_cast<const StencilMask*>( getRenderStateSet()->renderState( RS_StencilMask ) ); }
    StencilMask* getStencilMask() { return static_cast<StencilMask*>( getRenderStateSet()->renderState( RS_StencilMask ) ); }

    BlendColor* gocBlendColor();
    const BlendColor* getBlendColor() const { return static_cast<const BlendColor*>( getRenderStateSet()->renderState( RS_BlendColor ) ); }
    BlendColor* getBlendColor() { return static_cast<BlendColor*>( getRenderStateSet()->renderState( RS_BlendColor ) ); }

    BlendFunc* gocBlendFunc();
    const BlendFunc* getBlendFunc() const { return static_cast<const BlendFunc*>( getRenderStateSet()->renderState( RS_BlendFunc ) ); }
    BlendFunc* getBlendFunc() { return static_cast<BlendFunc*>( getRenderStateSet()->renderState( RS_BlendFunc ) ); }

    SampleCoverage* gocSampleCoverage();
    const SampleCoverage* getSampleCoverage() const { return static_cast<const SampleCoverage*>( getRenderStateSet()->renderState( RS_SampleCoverage ) ); }
    SampleCoverage* getSampleCoverage() { return static_cast<SampleCoverage*>( getRenderStateSet()->renderState( RS_SampleCoverage ) ); }

    // indexed render states

    // lights

    Light* gocLight(int light_index);
    
    const Light* getLight(int light_index) const;
    
    Light* getLight(int light_index);

    // clip planes

    ClipPlane* gocClipPlane(int plane_index);
    
    const ClipPlane* getClipPlane(int plane_index) const;
    
    ClipPlane* getClipPlane(int plane_index);

    // texture unit

    TextureSampler* gocTextureSampler(int unit_index);
    
    const TextureSampler* getTextureSampler(int unit_index) const { return static_cast<const TextureSampler*>( getRenderStateSet()->renderState( (ERenderState)(RS_TextureSampler0+unit_index) ) ); }
    
    TextureSampler* getTextureSampler(int unit_index) { return static_cast<TextureSampler*>( getRenderStateSet()->renderState( (ERenderState)(RS_TextureSampler0+unit_index) ) ); }

    // tex env

    TexEnv* gocTexEnv(int unit_index);
    
    const TexEnv* getTexEnv(int unit_index) const { return static_cast<const TexEnv*>( getRenderStateSet()->renderState( (ERenderState)(RS_TexEnv0+unit_index) ) ); }
    
    TexEnv* getTexEnv(int unit_index) { return static_cast<TexEnv*>( getRenderStateSet()->renderState( (ERenderState)(RS_TexEnv0+unit_index) ) ); }

    // tex gen
    
    TexGen* gocTexGen(int unit_index);
    
    const TexGen* getTexGen(int unit_index) const { return static_cast<const TexGen*>( getRenderStateSet()->renderState( (ERenderState)(RS_TexGen0+unit_index) ) ); }
    
    TexGen* getTexGen(int unit_index) { return static_cast<TexGen*>( getRenderStateSet()->renderState( (ERenderState)(RS_TexGen0+unit_index) ) ); }

    // texture matrix

    TextureMatrix* gocTextureMatrix(int unit_index);
    
    const TextureMatrix* getTextureMatrix(int unit_index) const { return static_cast<const TextureMatrix*>( getRenderStateSet()->renderState( (ERenderState)(RS_TextureMatrix0+unit_index) ) ); }
    
    TextureMatrix* getTextureMatrix(int unit_index) { return static_cast<TextureMatrix*>( getRenderStateSet()->renderState( (ERenderState)(RS_TextureMatrix0+unit_index) ) ); }

    // enable methods

    void enable(EEnable capability)  { gocEnableSet()->enable(capability); }
    
    void disable(EEnable capability) { gocEnableSet()->disable(capability); }
    
    const std::vector<EEnable>& enables() const { return getEnableSet()->enables(); }
    
    int isEnabled(EEnable capability) const { if (!getEnableSet()) return false; return getEnableSet()->isEnabled(capability); }
    
    void disableAll() { if (getEnableSet()) getEnableSet()->disableAll(); }
    
    bool blendingEnabled() const { if (!getEnableSet()) return false; return getEnableSet()->blendingEnabled(); }

    // render states methods

    void setRenderState(RenderState* renderstate) { gocRenderStateSet()->setRenderState(renderstate); }
    
    const RenderState* renderState( ERenderState type ) const { if (!getRenderStateSet()) return NULL; return getRenderStateSet()->renderState(type); }
    
    RenderState* renderState( ERenderState type ) { return gocRenderStateSet()->renderState(type); }
    
    size_t renderStatesCount() const { return getRenderStateSet()->renderStatesCount(); }

    const ref<RenderState>* renderStates() const { return getRenderStateSet()->renderStates(); }

    ref<RenderState>* renderStates() { return getRenderStateSet()->renderStates(); }

    void eraseRenderState(ERenderState type) { gocRenderStateSet()->eraseRenderState(type); }
    
    void eraseRenderState(RenderState* rs) { if (rs) gocRenderStateSet()->eraseRenderState(rs->type()); }
    
    void eraseAllRenderStates() { if(getRenderStateSet()) getRenderStateSet()->eraseAllRenderStates(); }
    
    //! Returns the GLSLProgram associated to a Shader (if any)
    const GLSLProgram* glslProgram() const { if (!getRenderStateSet()) return NULL; return getRenderStateSet()->glslProgram(); }

    //! Returns the GLSLProgram associated to a Shader (if any)
    GLSLProgram* glslProgram() { return gocRenderStateSet()->glslProgram(); }

    // uniforms methods

    //! Equivalent to gocUniformSet()->setUniform(...)
    void setUniform(Uniform* uniform) { VL_CHECK(uniform); gocUniformSet()->setUniform(uniform); }
    
    //! Equivalent to gocUniformSet()->uniforms(...)
    const std::vector< ref<Uniform> >& uniforms() const { return getUniformSet()->uniforms(); }
    
    //! Equivalent to gocUniformSet()->eraseUniform(...)
    void eraseUniform(const std::string& name) { gocUniformSet()->eraseUniform(name); }
    
    //! Equivalent to gocUniformSet()->eraseUniform(...)
    void eraseUniform(const Uniform* uniform) { gocUniformSet()->eraseUniform(uniform); }
    
    //! Equivalent to gocUniformSet()->eraseAllUniforms(...)
    void eraseAllUniforms() { if (getUniformSet()) getUniformSet()->eraseAllUniforms(); }
    
    //! Equivalent to gocUniformSet()->gocUniform(...)
    Uniform* gocUniform(const std::string& name) { return gocUniformSet()->gocUniform(name); }
    
    //! Equivalent to gocUniformSet()->getUniform(...)
    Uniform* getUniform(const std::string& name) { return getUniformSet()->getUniform(name); }
    
    //! Equivalent to gocUniformSet()->getUniform(...)
    const Uniform* getUniform(const std::string& name) const { return getUniformSet()->getUniform(name); }

    // sets

    EnableSet* gocEnableSet() { if (!mEnableSet) mEnableSet = new EnableSet; return mEnableSet.get(); }
    
    EnableSet* getEnableSet() { return mEnableSet.get(); }
    
    const EnableSet* getEnableSet() const { return mEnableSet.get(); }
    
    RenderStateSet* gocRenderStateSet() { if (!mRenderStateSet) mRenderStateSet = new RenderStateSet; return mRenderStateSet.get(); }
    
    RenderStateSet* getRenderStateSet() { return mRenderStateSet.get(); }
    
    const RenderStateSet* getRenderStateSet() const { return mRenderStateSet.get(); }
    
    /**
     * Returns the UniformSet installed (creating it if no UniformSet has been installed)
     * \sa
     * - setUniform()
     * - uniforms()
     * - eraseUniform(const std::string& name)
     * - eraseUniform(const Uniform* uniform)
     * - eraseAllUniforms()
     * - getUniform()
    */
    UniformSet* gocUniformSet() { if (!mUniformSet) mUniformSet = new UniformSet; return mUniformSet.get(); }
    
    /**
     * Returns the UniformSet installed
     * \sa
     * - setUniform()
     * - uniforms()
     * - eraseUniform(const std::string& name)
     * - eraseUniform(const Uniform* uniform)
     * - eraseAllUniforms()
     * - getUniform()
    */
    UniformSet* getUniformSet() { return mUniformSet.get(); }
    
    /**
     * Returns the UniformSet installed
     * \sa
     * - setUniform()
     * - uniforms()
     * - eraseUniform(const std::string& name)
     * - eraseUniform(const Uniform* uniform)
     * - eraseAllUniforms()
     * - getUniform()
    */
    const UniformSet* getUniformSet() const { return mUniformSet.get(); }

    void setEnableSet(EnableSet* es) { mEnableSet = es; }
    
    void setRenderStateSet(RenderStateSet* rss) { mRenderStateSet = rss; }
    
    /**
     * Installs a new UniformSet
     * \sa
     * - setUniform()
     * - uniforms()
     * - eraseUniform(const std::string& name)
     * - eraseUniform(const Uniform* uniform)
     * - eraseAllUniforms()
     * - getUniform()
    */
    void setUniformSet(UniformSet* us) { mUniformSet = us; }

    /**
     * Sets the Scissor to be used when rendering an Actor.
     * \note
     * You can also define a Scissor on a per-Actor basis using the function Actor::setScissor(). 
     * In case both the Shader's and the Actor's Scissor are defined the Actor's Scissor is used.
     * \sa
     * - Scissor
     * - Actor::setScissor()
     */
    void setScissor(Scissor* scissor) { mScissor = scissor; }
    
    /**
     * Returns the Scissor to be used when rendering an Actor.
     * \sa
     * - Scissor
     * - Shader::setScissor()
     * - Actor::setScissor()
     */
    const Scissor* scissor() const { return mScissor.get(); }
    
    /**
     * Returns the Scissor to be used when rendering an Actor.
     * \sa
     * - Scissor
     * - Shader::setScissor()
     * - Actor::setScissor()
     */
    Scissor* scissor() { return mScissor.get(); }

    /** Installs the ShaderAnimator used to update/animate a Shader (see vl::ShaderAnimator documentation). */
    void setShaderAnimator(ShaderAnimator* animator) { mShaderAnimator = animator; }

    /** Returns the ShaderAnimator used to update/animate a Shader (see vl::ShaderAnimator documentation). */
    ShaderAnimator* shaderAnimator() { return mShaderAnimator.get(); }

    /** Returns the ShaderAnimator used to update/animate a Shader (see vl::ShaderAnimator documentation). */
    const ShaderAnimator* shaderAnimator() const { return mShaderAnimator.get(); }

    /** Last time this Actor was animated/updated using a shaderAnimator(). */
    Real lastUpdateTime() const { return mLastUpdateTime; }

    //! Used internally.
    void setLastUpdateTime(Real time) { mLastUpdateTime = time; }

#if VL_SHADER_USER_DATA
  public:
    void* shaderUserData() { return mShaderUserData; }
    const void* shaderUserData() const { return mShaderUserData; }
    void setShaderUserData(void* user_data) { mShaderUserData = user_data; }

  private:
    void* mShaderUserData;
#endif

  protected:
    ref<RenderStateSet> mRenderStateSet;
    ref<EnableSet> mEnableSet;
    ref<UniformSet> mUniformSet;
    ref<Scissor> mScissor;
    ref<ShaderAnimator> mShaderAnimator;
    Real mLastUpdateTime;
  };
}

#endif
