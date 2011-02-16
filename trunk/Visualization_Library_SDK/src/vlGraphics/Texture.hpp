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

#include <vlCore/String.hpp>
#include <vlCore/Vector4.hpp>
#include <vlCore/Image.hpp>

namespace vl
{
  //------------------------------------------------------------------------------
  // TexParameter
  //------------------------------------------------------------------------------
  /** Wraps the OpenGL function glTexParameter(), see also http://www.opengl.org/sdk/docs/man/xhtml/glTexParameter.xml for more information.
   *
   * \note
   * A TexParameter defines a set of variables associated to a Texture while 
   * TexGen and TexEnv define a set of variables associated to a TextureUnit.
   *
   * \sa
   * - Texture::getTexParameter()
   * - Texture
   * - TextureUnit
   * - TexGen
   * - TexEnv
   * - Shader
   * - Effect
   * - Actor */
  class TexParameter
  {
    friend class Texture;
  public:
    TexParameter();
    virtual const char* className() { return "TexParameter"; }

    void apply(ETextureDimension dimension) const;

    ETexParamFilter minFilter()   const { return mMinFilter; }
    ETexParamFilter magFilter()   const { return mMagfilter; }
    ETexParamWrap wrapS()         const { return mWrapS; }
    ETexParamWrap wrapT()         const { return mWrapT; }
    ETexParamWrap wrapR()         const { return mWrapR; }
    fvec4 borderColor()           const { return mBorderColor; }
    float anisotropy()            const { return mAnisotropy; }
    bool generateMipmap()         const { return mGenerateMipmap; }
    ETexCompareMode compareMode() const { return mCompareMode; }
    ETexCompareFunc compareFunc() const { return mCompareFunc; }
    EDepthTextureMode depthTextureMode() const { return mDepthTextureMode; }

    void setMinFilter(ETexParamFilter minfilter) { mDirty = true; mMinFilter = minfilter; }
    void setMagFilter(ETexParamFilter magfilter);
    void setWrapS(ETexParamWrap texturewrap)     { mDirty = true; mWrapS = texturewrap; }
    void setWrapT(ETexParamWrap texturewrap)     { mDirty = true; mWrapT = texturewrap; }
    void setWrapR(ETexParamWrap texturewrap)     { mDirty = true; mWrapR = texturewrap; }
    void setBorderColor(fvec4 bordercolor)       { mDirty = true; mBorderColor = bordercolor; }
    void setAnisotropy(float anisotropy)         { mDirty = true; mAnisotropy = anisotropy; }
    void setGenerateMipmap(bool generate_mipmap) { mDirty = true; mGenerateMipmap = generate_mipmap; }
    void setTexCompareMode(ETexCompareMode mode) { mDirty = true; mCompareMode = mode; }
    void setTexCompareFunc(ETexCompareFunc func) { mDirty = true; mCompareFunc = func; }
    void setDepthTextureMode(EDepthTextureMode mode) { mDirty = true; mDepthTextureMode = mode; }

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

    mutable bool mDirty;
  };
  //------------------------------------------------------------------------------
  // Texture
  //------------------------------------------------------------------------------
  /** Wraps an OpenGL texture object representing and managing all the supported texture types.
   *
   * \remarks
   * Many of the parameters used to create a Textures are the same used by glTexImage1D/2D/3D etc. See the following for more information:
   * - http://www.opengl.org/sdk/docs/man/xhtml/glTexImage1D.xml
   * - http://www.opengl.org/sdk/docs/man/xhtml/glTexImage2D.xml
   * - http://www.opengl.org/sdk/docs/man/xhtml/glTexImage3D.xml
   *
   * \note
   * A TexParameter defines a set of variables associated to a Texture while 
   * TexGen and TexEnv define a set of variables associated to a TextureUnit.
   *
   * \sa
   * - getTexParameter() and TexParameter
   * - createTexture()
   * - setMipLevel()
   * - TextureUnit
   * - TexGen
   * - TexEnv
   * - Shader
   * - Effect
   * - Actor */
  class Texture: public Object
  {
  public:
    /** The SetupParams function wraps all the parameters needed to crate a Texture.
     * A SetupParams object is automatically bound to a Texture after calling prepareTexture2D() and similar functions.
     * Once the SetupParams are bound to a Texture calling Texture::createTexture() will create a new Texture according 
     * to what specified in the SetupParams objects. After Texture::createTexture() the SetupParams object is removed. */
    class SetupParams: public Object
    {
    public:
      SetupParams()
      {
        mDimension  = TD_TEXTURE_2D;
        mFormat     = TF_RGBA;
        mBorder     = false;
        mGenMipmaps = false;
        mWidth = mHeight = mDepth = 0;
      }

      const String& imagePath() const { return mImagePath; }
      Image* image() const { return mImage.get(); }
      ETextureDimension dimension() const { return mDimension; }
      ETextureFormat format() const { return mFormat; }
      bool border() const { return mBorder; }
      bool genMipmaps() const { return mGenMipmaps; }

      void setImagePath(const String& path) { mImagePath = path; }
      void setImage(Image* image) { mImage = image; }
      void setDimension(ETextureDimension dimension) { mDimension = dimension; }
      void setFormat(ETextureFormat format) { mFormat = format; }
      void setBorder(bool on) { mBorder = on; }
      void setGenMipmaps(bool on) { mGenMipmaps = on; }

      void setWidth(int w) { mWidth = w; }
      int width() const { return mWidth; }

      void setHeight(int h) { mHeight = h; }
      int height() const { return mHeight; }

      void setDepth(int d) { mDepth = d; }
      int depth() const { return mDepth; }

    protected:
      String mImagePath;
      ref<Image> mImage;
      ETextureDimension mDimension;
      ETextureFormat mFormat;
      int mWidth, mHeight, mDepth; // used when no image is specified.
      bool mBorder;
      bool mGenMipmaps;
    };

  public:
    virtual const char* className() { return "Texture"; }

    /** Constructs a texture from the specified file. 
    \note The OpenGL texture object is created immediately therefore an OpenGL context must be active when calling this constructor. */
    Texture(const String& image_path, ETextureFormat format = TF_RGBA, bool mipmaps = true, bool border=false);
    
    /** Constructs a texture from the specified image. 
    \note The OpenGL texture object is created immediately therefore an OpenGL context must be active when calling this constructor. */
    Texture(Image* image, ETextureFormat format = TF_RGBA, bool mipmaps = true, bool border=false);
    
    /** Constructs an empty 1D texture. 
    \note The OpenGL texture object is created immediately therefore an OpenGL context must be active when calling this constructor. */
    Texture(int width, ETextureFormat format = TF_RGBA, bool border=false);
    
    /** Constructs an empty 2D texture. 
    \note The OpenGL texture object is created immediately therefore an OpenGL context must be active when calling this constructor. */
    Texture(int width, int height, ETextureFormat format = TF_RGBA, bool border=false);
    
    /** Constructs an empty 3D texture. 
    \note The OpenGL texture object is created immediately therefore an OpenGL context must be active when calling this constructor. */
    Texture(int width, int height, int depth, ETextureFormat format = TF_RGBA, bool border=false);
    
    /** Constructs an null texture that can be initialized later using one of the \p prepareTexture*() functions or calling createTexture(). */
    Texture();

    /** Destructor. */
    ~Texture();

    /** The TexParameter object associated to a Texture. */
    TexParameter* getTexParameter() { return &mTexParameter; }

    /** The TexParameter object associated to a Texture. */
    const TexParameter* getTexParameter() const { return &mTexParameter; }

    /** Destroys the texture. */
    void destroyTexture();

    /** Creates a texture using the parameters specified by the last \p prepareTexture*() called. 
    \remarks You can retrieve the currently active texture creation settings using the setupParams() function. 
    \note The OpenGL texture object is created immediately therefore an OpenGL context must be active when calling this function. */
    bool createTexture();

    /** Creates an empty texture, with no mipmaps, of the specified type, format and dimensions.
    \note The OpenGL texture object is created immediately therefore an OpenGL context must be active when calling this function. */
    bool createTexture(ETextureDimension tex_dimension, ETextureFormat tex_format, int w, int h, int d, bool border);

    /** Copies the texture image to the specified mip-maping level. This function can be useful to 
    specify one by one the mipmapping images or to create texture animation effects.
    \param mip_level The mip-mapping level to be modified.
    \param img The Image containing the pixels to be copied into the specified mip-mapping level.
    \param gen_mipmaps If \p true automatically generates the mip-mapping images for the levels below \p mip_level. 
    \remarks Before calling this function one of the two createTexture() methods must have been called.
    \note The mip-map level is updated immediately therefore an OpenGL context must be active when calling this function. */
    bool setMipLevel(int mip_level, Image* img, bool gen_mipmaps);

    /** Prepares for creation an empty 1D texture. */
    void prepareTexture1D(int width, ETextureFormat format, bool border=false)
    {
      prepareTexture1D(NULL, format, false, border);
      mSetupParams->setWidth(width);
    }

    /** Prepares for creation a 1D texture from the specified file. */
    void prepareTexture1D(const String& image_path, ETextureFormat format, bool mipmaps=true, bool border=false)
    {
      prepareTexture1D(NULL, format, mipmaps, border);
      mSetupParams->setImagePath(image_path);
    }

    /** Prepares for creation a 1D texture from the specified image. */
    void prepareTexture1D(Image* image, ETextureFormat format, bool mipmaps=true, bool border=false)
    {
      mSetupParams = new SetupParams;
      mSetupParams->setImage(image);
      mSetupParams->setDimension(TD_TEXTURE_1D);
      mSetupParams->setFormat(format);
      mSetupParams->setGenMipmaps(mipmaps);
      mSetupParams->setBorder(border);
    }

    /** Prepares for creation an empty 2D texture. */
    void prepareTexture2D(int width, int height, ETextureFormat format, bool border=false)
    {
      prepareTexture2D(NULL, format, false, border);
      mSetupParams->setWidth(width);
      mSetupParams->setHeight(height);
    }
    
    /** Prepares for creation a 2D texture from the specified file. */
    void prepareTexture2D(const String& image_path, ETextureFormat format, bool mipmaps=true, bool border=false)
    {
      prepareTexture2D(NULL, format, mipmaps, border);
      mSetupParams->setImagePath(image_path);
    }
    
    /** Prepares for creation a 2D texture from the specified image. */
    void prepareTexture2D(Image* image, ETextureFormat format, bool mipmaps=true, bool border=false)
    {
      mSetupParams = new SetupParams;
      mSetupParams->setImage(image);
      mSetupParams->setDimension(TD_TEXTURE_2D);
      mSetupParams->setFormat(format);
      mSetupParams->setGenMipmaps(mipmaps);
      mSetupParams->setBorder(border);
    }
    
    /** Prepares for creation an empty 3D texture. */
    void prepareTexture3D(int width, int height, int depth, ETextureFormat format, bool border=false)
    {
      prepareTexture3D(NULL, format, false, border);
      mSetupParams->setWidth(width);
      mSetupParams->setHeight(height);
      mSetupParams->setDepth(depth);
    }
    
    /** Prepares for creation a 3D texture from the specified file. */
    void prepareTexture3D(const String& image_path, ETextureFormat format, bool mipmaps=true, bool border=false)
    {
      prepareTexture3D(NULL, format, mipmaps, border);
      mSetupParams->setImagePath(image_path);
    }
    
    /** Prepares for creation a 3D texture from the specified image. */
    void prepareTexture3D(Image* image, ETextureFormat format, bool mipmaps=true, bool border=false)
    {
      mSetupParams = new SetupParams;
      mSetupParams->setImage(image);
      mSetupParams->setDimension(TD_TEXTURE_3D);
      mSetupParams->setFormat(format);
      mSetupParams->setGenMipmaps(mipmaps);
      mSetupParams->setBorder(border);
    }
   
    /** Prepares for creation an empty cubemap texture. */
    void prepareTextureCubemap(int width, int height, ETextureFormat format, bool border=false)
    {
      prepareTextureCubemap(NULL, format, false, border);
      mSetupParams->setWidth(width);
      mSetupParams->setHeight(height);
    }
    
    /** Prepares for creation a cubemap texture from the specified file. */
    void prepareTextureCubemap(const String& image_path, ETextureFormat format, bool mipmaps=true, bool border=false)
    {
      prepareTextureCubemap(NULL, format, mipmaps, border);
      mSetupParams->setImagePath(image_path);
    }
    
    /** Prepares for creation a cubemap texture from the specified image. */
    void prepareTextureCubemap(Image* image, ETextureFormat format, bool mipmaps=true, bool border=false)
    {
      mSetupParams = new SetupParams;
      mSetupParams->setImage(image);
      mSetupParams->setDimension(TD_TEXTURE_CUBE_MAP);
      mSetupParams->setFormat(format);
      mSetupParams->setGenMipmaps(mipmaps);
      mSetupParams->setBorder(border);
    }
    
    /** Prepares for creation an empty 1D array texture. */
    void prepareTexture1DArray(int width, int count, ETextureFormat format)
    {
      prepareTexture1DArray(NULL, format, false);
      mSetupParams->setWidth(width);
      mSetupParams->setHeight(count);
    }
    
    /** Prepares for creation a 1d texture array from the specified file. */
    void prepareTexture1DArray(const String& image_path, ETextureFormat format, bool mipmaps=true)
    {
      prepareTexture1DArray(NULL, format, mipmaps);
      mSetupParams->setImagePath(image_path);
    }
    
    /** Prepares for creation a 1d texture array from the specified image. */
    void prepareTexture1DArray(Image* image, ETextureFormat format, bool mipmaps=true)
    {
      mSetupParams = new SetupParams;
      mSetupParams->setImage(image);
      mSetupParams->setDimension(TD_TEXTURE_1D_ARRAY);
      mSetupParams->setFormat(format);
      mSetupParams->setGenMipmaps(mipmaps);
      mSetupParams->setBorder(false);
    }
    
    /** Prepares for creation an empty 2D array texture. */
    void prepareTexture2DArray(int width, int height, int count, ETextureFormat format)
    {
      prepareTexture2DArray(NULL, format, false);
      mSetupParams->setWidth(width);
      mSetupParams->setHeight(height);
      mSetupParams->setDepth(count);
    }
    
    /** Prepares for creation a 2d texture array from the specified file. */
    void prepareTexture2DArray(const String& image_path, ETextureFormat format, bool mipmaps=true)
    {
      prepareTexture2DArray(NULL, format, mipmaps);
      mSetupParams->setImagePath(image_path);
    }
    
    /** Prepares for creation a 2d texture array from the specified image. */
    void prepareTexture2DArray(Image* image, ETextureFormat format, bool mipmaps=true)
    {
      mSetupParams = new SetupParams;
      mSetupParams->setImage(image);
      mSetupParams->setDimension(TD_TEXTURE_2D_ARRAY);
      mSetupParams->setFormat(format);
      mSetupParams->setGenMipmaps(mipmaps);
      mSetupParams->setBorder(false);
    }
    
    /** Prepares for creation an empty texture rectangle. */
    void prepareTextureRectangle(int width, int height, ETextureFormat format)
    {
      prepareTextureRectangle(NULL, format);
      mSetupParams->setWidth(width);
      mSetupParams->setHeight(height);
    }
    
    /** Prepares for creation a texture rectangle from the specified file. */
    void prepareTextureRectangle(const String& image_path, ETextureFormat format)
    {
      prepareTextureRectangle(NULL, format);
      mSetupParams->setImagePath(image_path);
    }
    
    /** Prepares for creation a texture rectangle from the specified image. */
    void prepareTextureRectangle(Image* image, ETextureFormat format)
    {
      mSetupParams = new SetupParams;
      mSetupParams->setImage(image);
      mSetupParams->setDimension(TD_TEXTURE_RECTANGLE);
      mSetupParams->setFormat(format);
      mSetupParams->setGenMipmaps(false);
      mSetupParams->setBorder(false);
    }

    /** Returns \p true if the current texture configuration seems valid. */
    bool isValid() const;

    /** OpenGL texture handle as returned by glGenTextures(). */
    unsigned int handle() const { return mHandle; }
    /** OpenGL texture handle as returned by glGenTextures(). */
    void setHandle(unsigned int id) { mHandle = id; }

    /** The texture type (1d, 2d, cubemap etc.) as specified by the \p target parameter of glTexImage*(). */
    void setDimension(ETextureDimension dimension) { mDimension = dimension; }
    /** The texture type (1d, 2d, cubemap etc.) as specified by the \p target parameter of glTexImage*(). */
    ETextureDimension dimension() const { return mDimension; }

    /** The texture internal format as pecified by the \p internalFormat parameter of glTexImage*(). */
    void setInternalFormat(ETextureFormat format) { mFormat = format; }
    /** The texture internal format as pecified by the \p internalFormat parameter of glTexImage*(). */
    ETextureFormat internalFormat() const { return mFormat; }

    /** The horizontal dimension of the texture in texels. */
    void setWidth(int x)  { mWidth = x; }
    /** The horizontal dimension of the texture in texels. */
    int width() const { return mWidth; }

    /** The vertical dimension of the texture in texels. */
    void setHeight(int y) { mHeight = y; }
    /** The vertical dimension of the texture in texels. */
    int height() const { return mHeight; }

    /** The z dimension of the texture in texels. */
    void setDepth(int z)  { mDepth = z; }
    /** The z dimension of the texture in texels. */
    int depth() const { return mDepth; }

    /** Whether the texture has a 1 pixel texture border or not. */
    void setBorder(bool border) { mBorder = border; }
    /** Whether the texture has a 1 pixel texture border or not. */
    bool border() const { return mBorder; }

    /** See SetupParams */
    const SetupParams* setupParams() const { return mSetupParams.get(); }

    /** See SetupParams */
    SetupParams* setupParams() { return mSetupParams.get(); }

    /** See SetupParams */
    void setSetupParams(SetupParams* setup_params) { mSetupParams = setup_params; }

    /** Checks whether the specified texture type, format and dimension combination is supported by the current OpenGL driver. */
    static bool supports(ETextureDimension tex_dimension, ETextureFormat tex_format, int mip_level, EImageDimension img_dimension, int w, int h, int d, bool border, bool verbose);

    /** Returns \p true if the specified format is compressed. */
    static bool isCompressedFormat(int format);

  private:
    Texture(const Texture& other): Object(other) {}
    void operator=(const Texture&) {}
    void reset();

  protected:
    unsigned int mHandle;
    TexParameter mTexParameter;
    ref<SetupParams> mSetupParams;
    ETextureFormat mFormat;
    ETextureDimension mDimension;
    int mWidth;
    int mHeight;
    int mDepth;
    bool mBorder;
  };
}

#endif
