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

#ifndef Texture_INCUDE_DEFINE
#define Texture_INCUDE_DEFINE

#include <vl/String.hpp>
#include <vl/Vector4.hpp>
#include <vl/Image.hpp>

namespace vl
{
  //------------------------------------------------------------------------------
  // TexParameter
  //------------------------------------------------------------------------------
  /**
   * Wraps the OpenGL function glTexParameter(), see also http://www.opengl.org/sdk/docs/man/xhtml/glTexParameter.xml for more information.
   *
   * \note
   * A TexParameter defines a set of variables associated to a Texture while 
   * TexGen and TexEnv define a set of variables associated to a TextureUnit.
   *
   * \sa
   * - TextureUnit
   * - Texture
   * - TexGen
   * - TexEnv
   * - Shader
   * - Effect
   * - Actor
  */
  class TexParameter: public Object
  {
  public:
    TexParameter();
    virtual const char* className() { return "TexParameter"; }

    void apply(ETextureDimension dimension);

    ETexParamFilter minFilter() const { return mMinFilter; }
    ETexParamFilter magFilter() const { return mMagfilter; }
    ETexParamWrap wrapS()       const { return mWrapS; }
    ETexParamWrap wrapT()       const { return mWrapT; }
    ETexParamWrap wrapR()       const { return mWrapR; }
    fvec4 borderColor()         const { return mBorderColor; }
    float anisotropy()        const { return mAnisotropy; }
    bool generateMipmap()       const { return mGenerateMipmap; }
    ETexCompareMode compareMode() const { return mCompareMode; }
    ETexCompareFunc compareFunc() const { return mCompareFunc; }
    EDepthTextureMode depthTextureMode() const { return mDepthTextureMode; }

    void setMinFilter(ETexParamFilter minfilter) { mMinFilter = minfilter; mDirty = true; }
    void setMagFilter(ETexParamFilter magfilter);
    void setWrapS(ETexParamWrap texturewrap)     { mWrapS = texturewrap; mDirty = true; }
    void setWrapT(ETexParamWrap texturewrap)     { mWrapT = texturewrap; mDirty = true; }
    void setWrapR(ETexParamWrap texturewrap)     { mWrapR = texturewrap; mDirty = true; }
    void setBorderColor(fvec4 bordercolor)       { mBorderColor = bordercolor; mDirty = true; }
    void setAnisotropy(float anisotropy)       { mAnisotropy = anisotropy; mDirty = true; }
    void setGenerateMipmap(bool generate_mipmap) { mGenerateMipmap = generate_mipmap; }
    void setTexCompareMode(ETexCompareMode mode) { mCompareMode = mode; }
    void setTexCompareFunc(ETexCompareFunc func) { mCompareFunc = func; }
    void setDepthTextureMode(EDepthTextureMode mode) { mDepthTextureMode = mode; }

    bool dirty() const { return mDirty; }

  protected:
    ETexParamFilter mMinFilter;
    ETexParamFilter mMagfilter;
    ETexParamWrap mWrapS;
    ETexParamWrap mWrapT;
    ETexParamWrap mWrapR;
    ETexCompareMode mCompareMode;
    ETexCompareFunc mCompareFunc;
    EDepthTextureMode mDepthTextureMode;
    fvec4 mBorderColor;
    float mAnisotropy;
    bool mGenerateMipmap;

    bool mDirty;
  };
  //------------------------------------------------------------------------------
  // Texture
  //------------------------------------------------------------------------------
  /**
   * Wraps an OpenGL texture object.
   *
   * \note
   * A TexParameter defines a set of variables associated to a Texture while 
   * TexGen and TexEnv define a set of variables associated to a TextureUnit.
   *
   * \sa
   * - TextureUnit
   * - Texture
   * - TexParameter
   * - TexGen
   * - TexEnv
   * - Shader
   * - Effect
   * - Actor
  */
  class Texture: public Object
  {
  public:
    /**
     * The SetupParams function wraps all the parameters neede to crate a Texture.
     * A SetupParams object is automatically bound to a Texture after calling setupTexture2D() and similar functions.
     * Once the SetupParams are bound to a Texture calling Texture::createTexture() will create a new Texture according 
     * to what specified in the SetupParams objects. After Texture::createTexture() the SetupParams object is removed.
     */
    class SetupParams: public Object
    {
    public:
      SetupParams()
      {
        mDimension  = TD_TEXTURE_2D;
        mFormat     = TF_RGBA;
        mBorder     = false;
        mHasMipmaps = true;
      }

      const String& imagePath() const { return mImagePath; }
      Image* image() const { return mImage.get(); }
      ETextureDimension dimension() const { return mDimension; }
      ETextureFormat format() const { return mFormat; }
      bool border() const { return mBorder; }
      bool hasMipmaps() const { return mHasMipmaps; }

      void setImagePath(const String& path) { mImagePath = path; }
      void setImage(Image* image) { mImage = image; }
      void setDimension(ETextureDimension dimension) { mDimension = dimension; }
      void setFormat(ETextureFormat format) { mFormat = format; }
      void setBorder(bool on) { mBorder = on; }
      void setHasMipmaps(bool on) { mHasMipmaps = on; }

    protected:
      String mImagePath;
      ref<Image> mImage;
      ETextureDimension mDimension;
      ETextureFormat mFormat;
      bool mBorder;
      bool mHasMipmaps;
    };

  public:
    virtual const char* className() { return "Texture"; }
    Texture(const String& image_path, ETextureFormat format = TF_RGBA, bool mipmaps = true, bool border=false);
    Texture(Image* image, ETextureFormat format = TF_RGBA, bool mipmaps = true, bool border=false);
    Texture(int width, ETextureFormat format = TF_RGBA, bool border=false);
    Texture(int width, int height, ETextureFormat format = TF_RGBA, bool border=false);
    Texture(int width, int height, int depth, ETextureFormat format = TF_RGBA, bool border=false);
    Texture();
    virtual ~Texture();

    TexParameter* getTexParameter() const { return mTexParameter.get(); }

    //! Performs the actual creation of the texture.
    //! \sa
    //! - setupTexture2D()
    //! - SetupParams
    bool createTexture();

    void setupTexture1D(const String& image_path, ETextureFormat format=TF_RGBA, bool mipmaps=true, bool border=false)
    {
      setupTexture1D(NULL, format, mipmaps, border);
      mSetupParams->setImagePath(image_path);
    }
    void setupTexture1D(Image* image, ETextureFormat format=TF_RGBA, bool mipmaps=true, bool border=false)
    {
      mSetupParams = new SetupParams;
      mSetupParams->setImage(image);
      mSetupParams->setDimension(TD_TEXTURE_1D);
      mSetupParams->setFormat(format);
      mSetupParams->setHasMipmaps(mipmaps);
      mSetupParams->setBorder(border);
    }
    void setupTexture2D(const String& image_path, ETextureFormat format=TF_RGBA, bool mipmaps=true, bool border=false)
    {
      setupTexture2D(NULL, format, mipmaps, border);
      mSetupParams->setImagePath(image_path);
    }
    void setupTexture2D(Image* image, ETextureFormat format=TF_RGBA, bool mipmaps=true, bool border=false)
    {
      mSetupParams = new SetupParams;
      mSetupParams->setImage(image);
      mSetupParams->setDimension(TD_TEXTURE_2D);
      mSetupParams->setFormat(format);
      mSetupParams->setHasMipmaps(mipmaps);
      mSetupParams->setBorder(border);
    }
    void setupTexture3D(const String& image_path, ETextureFormat format=TF_RGBA, bool mipmaps=true, bool border=false)
    {
      setupTexture3D(NULL, format, mipmaps, border);
      mSetupParams->setImagePath(image_path);
    }
    void setupTexture3D(Image* image, ETextureFormat format=TF_RGBA, bool mipmaps=true, bool border=false)
    {
      mSetupParams = new SetupParams;
      mSetupParams->setImage(image);
      mSetupParams->setDimension(TD_TEXTURE_3D);
      mSetupParams->setFormat(format);
      mSetupParams->setHasMipmaps(mipmaps);
      mSetupParams->setBorder(border);
    }
    void setupTextureCubemap(const String& image_path, ETextureFormat format=TF_RGBA, bool mipmaps=true, bool border=false)
    {
      setupTextureCubemap(NULL, format, mipmaps, border);
      mSetupParams->setImagePath(image_path);
    }
    void setupTextureCubemap(Image* image, ETextureFormat format=TF_RGBA, bool mipmaps=true, bool border=false)
    {
      mSetupParams = new SetupParams;
      mSetupParams->setImage(image);
      mSetupParams->setDimension(TD_TEXTURE_CUBE_MAP);
      mSetupParams->setFormat(format);
      mSetupParams->setHasMipmaps(mipmaps);
      mSetupParams->setBorder(border);
    }
    void setupTexture1DArray(const String& image_path, ETextureFormat format=TF_RGBA, bool mipmaps=true)
    {
      setupTexture1DArray(NULL, format, mipmaps);
      mSetupParams->setImagePath(image_path);
    }
    void setupTexture1DArray(Image* image, ETextureFormat format=TF_RGBA, bool mipmaps=true)
    {
      mSetupParams = new SetupParams;
      mSetupParams->setImage(image);
      mSetupParams->setDimension(TD_TEXTURE_1D_ARRAY);
      mSetupParams->setFormat(format);
      mSetupParams->setHasMipmaps(mipmaps);
      mSetupParams->setBorder(false);
    }
    void setupTexture2DArray(const String& image_path, ETextureFormat format=TF_RGBA, bool mipmaps=true)
    {
      setupTexture2DArray(NULL, format, mipmaps);
      mSetupParams->setImagePath(image_path);
    }
    void setupTexture2DArray(Image* image, ETextureFormat format=TF_RGBA, bool mipmaps=true)
    {
      mSetupParams = new SetupParams;
      mSetupParams->setImage(image);
      mSetupParams->setDimension(TD_TEXTURE_2D_ARRAY);
      mSetupParams->setFormat(format);
      mSetupParams->setHasMipmaps(mipmaps);
      mSetupParams->setBorder(false);
    }
    void setupTextureRectangle(const String& image_path, ETextureFormat format=TF_RGBA)
    {
      setupTextureRectangle(NULL, format);
      mSetupParams->setImagePath(image_path);
    }
    void setupTextureRectangle(Image* image, ETextureFormat format=TF_RGBA)
    {
      mSetupParams = new SetupParams;
      mSetupParams->setImage(image);
      mSetupParams->setDimension(TD_TEXTURE_RECTANGLE);
      mSetupParams->setFormat(format);
      mSetupParams->setHasMipmaps(false);
      mSetupParams->setBorder(false);
    }

    bool isValid() const;
    bool hasMipMaps() const { return mHasMipmaps; }

    void setHandle(unsigned int id) { mHandle = id; }
    void setDimension(ETextureDimension dimension) { mDimension = dimension; }
    void setWidth(int x)  { mWidth = x; }
    void setHeight(int y) { mHeight = y; }
    void setDepth(int z)  { mDepth = z; }
    void setBorder(bool border) { mBorder = border; }
    void setInternalFormat(ETextureFormat format) { mFormat = format; }

    unsigned int handle() const { return mHandle; }
    ETextureDimension dimension() const { return mDimension; }
    int width() const { return mWidth; }
    int height() const { return mHeight; }
    int depth() const { return mDepth; }
    bool border() const { return mBorder; }
    ETextureFormat internalFormat() const { return mFormat; }

    //! See SetupParams
    const SetupParams* setupParams() const { return mSetupParams.get(); }
    //! See SetupParams
    SetupParams* setupParams() { return mSetupParams.get(); }
    //! See SetupParams
    void setSetupParams(SetupParams* setup_params) { mSetupParams = setup_params; }

    static bool supports(ETextureDimension texture_dim, ETextureFormat format, bool border, const Image* image, bool verbose=true);
    // static bool supports(ETextureDimension texture_dim, ETextureFormat format, bool border, const Image* image, bool verbose=true);
    static bool isCompressedFormat(int format);

  private:
    Texture(const Texture& other): Object(other) {}
    void operator=(const Texture&) {}

  protected:
    unsigned int mHandle;
    ref<TexParameter> mTexParameter;
    ref<SetupParams> mSetupParams;
    ETextureFormat mFormat;
    ETextureDimension mDimension;
    int mWidth;
    int mHeight;
    int mDepth;
    bool mBorder;
    bool mHasMipmaps;
  };
}

#endif
