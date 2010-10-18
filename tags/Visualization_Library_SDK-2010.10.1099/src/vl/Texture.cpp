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

#include <vl/Texture.hpp>
#include <vl/checks.hpp>
#include <vl/Image.hpp>
#include <vl/math3D.hpp>
#include <vl/Say.hpp>
#include <vl/Log.hpp>

using namespace vl;

//-----------------------------------------------------------------------------
// Texture
//-----------------------------------------------------------------------------
Texture::~Texture()
{
  if (mHandle)
  glDeleteTextures(1, &mHandle);
  mHandle = 0;
}
//-----------------------------------------------------------------------------
Texture::Texture(int width, ETextureFormat format, bool border)
{
  VL_CHECK_OGL()
  #ifndef NDEBUG
    mObjectName = className();
  #endif
  setDimension(TD_TEXTURE_1D);
  setInternalFormat(format);
  setBorder(border);
  setWidth(width);
  setHeight(0);
  setDepth(0);
  mHasMipmaps = false;
  glGenTextures( 1, &mHandle ); VL_CHECK_OGL() VL_CHECK(handle())
  glBindTexture( dimension(), mHandle ); VL_CHECK_OGL()
  int brd = border?2:0;
  glTexImage1D( dimension(), 0, format+brd, width, border?1:0, GL_RGBA/*not used*/, GL_UNSIGNED_BYTE/*not used*/, NULL); VL_CHECK_OGL()
  glBindTexture( dimension(), 0 ); VL_CHECK_OGL()
}
//-----------------------------------------------------------------------------
Texture::Texture(int width, int height, ETextureFormat format, bool border)
{
  VL_CHECK_OGL()
  #ifndef NDEBUG
    mObjectName = className();
  #endif
  setDimension(TD_TEXTURE_2D);
  setInternalFormat(format);
  setBorder(border);
  setWidth(width);
  setHeight(height);
  setDepth(0);
  mHasMipmaps = false;
  glGenTextures(1, &mHandle); VL_CHECK_OGL() VL_CHECK(handle())
  glBindTexture(dimension(), mHandle); VL_CHECK_OGL()
  int brd = border?2:0;
  glTexImage2D(dimension(), 0, format, width+brd, height+brd, border?1:0, GL_RGBA/*not used*/, GL_UNSIGNED_BYTE/*not used*/, NULL); VL_CHECK_OGL()
  glBindTexture( dimension(), 0 ); VL_CHECK_OGL()
}
//-----------------------------------------------------------------------------
Texture::Texture(int width, int height, int depth, ETextureFormat format, bool border)
{
  VL_CHECK_OGL()
  #ifndef NDEBUG
    mObjectName = className();
  #endif
  setDimension(TD_TEXTURE_3D);
  setInternalFormat(format);
  setBorder(border);
  setWidth(width);
  setHeight(height);
  setDepth(depth);
  mHasMipmaps = false;
  if (GLEW_VERSION_1_2)
  {
    glGenTextures( 1, &mHandle ); VL_CHECK_OGL() VL_CHECK(handle())
    glBindTexture( dimension(), mHandle ); VL_CHECK_OGL()
    int brd = border?2:0;
    glTexImage3D( dimension(), 0, format, width+brd, height+brd, depth+brd, border?1:0, GL_RGBA/*not used*/, GL_UNSIGNED_BYTE/*not used*/, NULL); VL_CHECK_OGL()
    glBindTexture( dimension(), 0 ); VL_CHECK_OGL()
  }
  else
    Log::error("3D textures require OpenGL 1.2\n");
}
//-----------------------------------------------------------------------------
Texture::Texture(Image* image, ETextureFormat format, bool mipmaps , bool border):
  mHandle(0), mFormat(format), mDimension(TD_TEXTURE_2D), mWidth(0), mHeight(0), mDepth(0), mBorder(border), mHasMipmaps(false)
{
  #ifndef NDEBUG
    mObjectName = className();
  #endif

  if (image && image->isValid())
  {
    switch(image->dimension())
    {
    case ID_1D:      setupTexture1D(image, format, mipmaps, border); break;
    case ID_2D:      setupTexture2D(image, format, mipmaps, border); break;
    case ID_3D:      setupTexture3D(image, format, mipmaps, border); break;
    case ID_Cubemap: setupTextureCubemap(image, format, mipmaps, border); break;
    default:
      break;
    }
  }
  else
    Log::bug("Texture construnctor called with an invalid Image!\n");
}
//-----------------------------------------------------------------------------
Texture::Texture(const String& image_path, ETextureFormat format, bool mipmaps , bool border):
  mHandle(0), mFormat(format), mDimension(TD_TEXTURE_2D), mWidth(0), mHeight(0), mDepth(0), mBorder(border), mHasMipmaps(false)
{
  #ifndef NDEBUG
    mObjectName = className();
  #endif

  ref<Image> image = vl::loadImage(image_path);

  if (image && image->isValid())
  {
    switch(image->dimension())
    {
    case ID_1D:      setupTexture1D(image.get(), format, mipmaps, border); break;
    case ID_2D:      setupTexture2D(image.get(), format, mipmaps, border); break;
    case ID_3D:      setupTexture3D(image.get(), format, mipmaps, border); break;
    case ID_Cubemap: setupTextureCubemap(image.get(), format, mipmaps, border); break;
    default:
      break;
    }
  }
  else
    Log::bug("Texture construnctor called with an invalid Image!\n");
}
//-----------------------------------------------------------------------------
Texture::Texture():
  mHandle(0), mFormat(TF_RGBA), mDimension(TD_TEXTURE_2D), mWidth(0), mHeight(0), mDepth(0), mBorder(false), mHasMipmaps(false)
{
  #ifndef NDEBUG
    mObjectName = className();
  #endif
}
//-----------------------------------------------------------------------------
bool Texture::isValid() const
{
  bool x = mWidth != 0 && mHeight == 0 && mDepth == 0;
  bool y = mWidth != 0 && mHeight != 0 && mDepth == 0;
  bool z = mWidth != 0 && mHeight != 0 && mDepth != 0;
  return handle() != 0 && (x|y|z);
}
//-----------------------------------------------------------------------------
bool Texture::supports(ETextureDimension texture_dim, ETextureFormat format, bool border, const Image* image, bool verbose)
{
  glGetError();

  // cubemaps

  if ( image->isCubemap() )
  {
    if (!GLEW_ARB_texture_cube_map && !GLEW_VERSION_1_3)
    {
      if (verbose) Log::error("Texture::createTexture(): texture cubemap not supported.\n");
      return false;
    }

    if ( image->width() != image->height() )
    {
      Log::error("Texture::supports(): cubemaps must have square faces.\n");
      return false;
    }
  }

  // texture arrays

  if ( texture_dim == TD_TEXTURE_1D_ARRAY || texture_dim == TD_TEXTURE_2D_ARRAY )
  {
    if (border)
    {
      Log::error("Texture::supports(): you cannot create a texture array with borders.\n");
      return false;
    }

    if ( (image->dimension() != ID_2D && texture_dim == TD_TEXTURE_1D_ARRAY) || ( image->dimension() != ID_3D && texture_dim == TD_TEXTURE_2D_ARRAY ) )
    {
      Log::error("Texture::supports(): the image dimensions are not suitable to create a texture array. To create a 1D texture array you need a 2D image and to create a 2D texture array you need a 3D image.\n");
      return false;
    }

    if(!(GLEW_EXT_texture_array||GLEW_VERSION_3_0))
    {
      if (verbose) Log::error("Texture::supports(): texture array not supported.\n");
      return false;
    }

  }

  // texture rectangle

  // TODO: support OpenGL 3.1
  if (texture_dim == TD_TEXTURE_RECTANGLE)
  {
    if (!(GLEW_ARB_texture_rectangle||GLEW_EXT_texture_rectangle||GLEW_NV_texture_rectangle/*TODO:||GLEW_VERSION_3_1*/))
    {
      if (verbose) Log::error("Texture::supports(): texture rectangle not supported.\n");
      return false;
    }

    if (border)
    {
      Log::error("Texture::supports(): ARB_texture_rectangle extension does not allow textures borders\n");
      return false;
    }
  }

  // compressed textures

  // if the original is compressed I cannot request a decompressed texture
  if ( isCompressedFormat(image->format()) && format != (int)image->format() )
  {
    Log::error("Texture::supports(): when the source image is compressed the texture format must be of the same type of the image.\n");
    return false;
  }

  if (isCompressedFormat(image->format()) && !(GLEW_ARB_texture_compression || GLEW_VERSION_1_3))
  {
    if (verbose) Log::error("Texture::createTexture(): texture compression not supported.\n");
    return false;
  }

  int width = 0;
  const int xsize = image->width();
  const int ysize = image->height();
  const int zsize = image->depth();

  // note: no special checking is done for texture arrays

  if (image->dimension() == ID_Cubemap)
  {
    if ( isCompressedFormat(format) )
    {
      glTexImage2D(GL_PROXY_TEXTURE_CUBE_MAP, 0, format, xsize + (border?2:0), ysize + (border?2:0), border?1:0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    }
    else
    {
      glTexImage2D(GL_PROXY_TEXTURE_CUBE_MAP, 0, format, xsize + (border?2:0), ysize + (border?2:0), border?1:0, image->format(), image->type(), NULL);
    }
    glTexImage2D(GL_PROXY_TEXTURE_CUBE_MAP, 0, format, xsize + (border?2:0), ysize + (border?2:0), border?1:0, image->format(), image->type(), NULL);
    glGetTexLevelParameteriv(GL_PROXY_TEXTURE_CUBE_MAP, 0, GL_TEXTURE_WIDTH, &width);
  }
  else
  if (image->dimension() == ID_3D)
  {
    if ( isCompressedFormat(format) )
    {
      glTexImage3D(GL_PROXY_TEXTURE_3D, 0, format, xsize + (border?2:0), ysize + (border?2:0), zsize + (border?2:0), border?1:0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    }
    else
    {
      glTexImage3D(GL_PROXY_TEXTURE_3D, 0, format, xsize + (border?2:0), ysize + (border?2:0), zsize + (border?2:0), border?1:0, image->format(), image->type(), NULL);
    }
    glGetTexLevelParameteriv(GL_PROXY_TEXTURE_3D, 0, GL_TEXTURE_WIDTH, &width);
  }
  else
  if (image->dimension() == ID_2D)
  {
    if (texture_dim == TD_TEXTURE_RECTANGLE)
    {
      if ( isCompressedFormat(format) )
      {
        glTexImage2D(GL_PROXY_TEXTURE_RECTANGLE_EXT, 0, format, xsize, ysize, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
      }
      else
      {
        glTexImage2D(GL_PROXY_TEXTURE_RECTANGLE_EXT, 0, format, xsize, ysize, 0, image->format(), image->type(), NULL);
      }
      glGetTexLevelParameteriv(GL_PROXY_TEXTURE_RECTANGLE_EXT, 0, GL_TEXTURE_WIDTH, &width);
    }
    else
    {
      if ( isCompressedFormat(format) )
      {
        glTexImage2D(GL_PROXY_TEXTURE_2D, 0, format, xsize + (border?2:0), ysize + (border?2:0), border?1:0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
      }
      else
      {
        glTexImage2D(GL_PROXY_TEXTURE_2D, 0, format, xsize + (border?2:0), ysize + (border?2:0), border?1:0, image->format(), image->type(), NULL);
      }
      glGetTexLevelParameteriv(GL_PROXY_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
    }
  }
  else
  if (image->dimension() == ID_1D)
  {
    if ( isCompressedFormat(format) )
    {
      glTexImage1D(GL_PROXY_TEXTURE_1D, 0, format, xsize + (border?2:0), border?1:0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    }
    else
    {
      glTexImage1D(GL_PROXY_TEXTURE_1D, 0, format, xsize + (border?2:0), border?1:0, image->format(), image->type(), NULL);
    }
    glGetTexLevelParameteriv(GL_PROXY_TEXTURE_1D, 0, GL_TEXTURE_WIDTH, &width);
  }

  GLenum err = glGetError();
  bool ok = !err && width != 0;
  if (!ok && verbose)
    Log::error( Say("Texture::supports() failed: w=%n h=%n d=%n\n") << image->width() << image->height() << image->depth() );

  return ok;
}
//-----------------------------------------------------------------------------
bool Texture::createTexture()
{
  VL_CHECK_OGL()

  if (!setupParams())
    return false;

  ref<Image> image      = setupParams()->image();
  ETextureFormat format = setupParams()->format();
  ETextureDimension dim = setupParams()->dimension();
  bool has_mipmaps          = setupParams()->hasMipmaps();
  bool border           = setupParams()->border();
  if (!image)
    if (!setupParams()->imagePath().empty())
      image = loadImage(setupParams()->imagePath());

  mSetupParams = NULL;

  if (image.get() == NULL || !image->isValid() || image->pixels() == NULL)
  {
    Log::error("Texture::createTexture() could not acquire a valid Image.\n");
    return false;
  }

  if (dim == TD_TEXTURE_RECTANGLE)
  {
    if (has_mipmaps)
    {
      Log::error("Texture::createTexture(): ARB_texture_rectangle extension does not allow mipmapped textures\n");
      return false;
    }
  }

  setObjectName( image->objectName() );

  int xsize = image->width();
  int ysize = image->height();
  int zsize = image->depth();
  xsize = xsize > 0 ? xsize : 1;
  ysize = ysize > 0 ? ysize : 1;
  zsize = zsize > 0 ? zsize : 1;

  if ( !supports(dim , format, border, image.get()) )
  {
    Log::error("Texture::createTexture(): the format requested is not supported.\n");
    VL_CHECK_OGL()
    return false;
  }

  VL_CHECK_OGL()

  unsigned char *tex_pixels = NULL;
  tex_pixels = (unsigned char*)image->pixels();

  setDimension( dim );
  setInternalFormat(format);
  setBorder(border);
  setWidth(xsize);
  setHeight(ysize);
  setDepth(zsize);
  mHasMipmaps = has_mipmaps;

  if (mHandle == 0)
    glGenTextures( 1, &mHandle );
  glBindTexture( dimension(), mHandle );

  int is_compressed = (format == (int)image->format()) && isCompressedFormat(format);
  int brd = border ? 1:0;

  glPixelStorei( GL_UNPACK_ALIGNMENT, image->byteAlignment() );

  // defines the general texture without the actual data

  // create mip maps

  int gl_generate_mipmap = 0;
  if (GLEW_SGIS_generate_mipmap || GLEW_VERSION_1_4)
    glGetTexParameteriv( dimension(), GL_GENERATE_MIPMAP, &gl_generate_mipmap );

  if ( has_mipmaps && image->mipmaps().empty() ) // generate mipmaps
  {
    switch( dimension() )
    {
    case TD_TEXTURE_1D:
    {
      // use GL_GENERATE_MIPMAP
      if (GLEW_SGIS_generate_mipmap || GLEW_VERSION_1_4)
      {
        glTexParameteri(dimension(), GL_GENERATE_MIPMAP, GL_TRUE);
        if (is_compressed)
          glCompressedTexImage1D( GL_TEXTURE_1D, 0, format, xsize+2*brd, brd, image->requiredMemory(), tex_pixels );
        else
          glTexImage1D( GL_TEXTURE_1D, 0, format, xsize+2*brd, brd, image->format(), image->type(), tex_pixels );
        glTexParameteri(dimension(), GL_GENERATE_MIPMAP, gl_generate_mipmap);
        VL_CHECK_OGL()
      }
      // use gluBuild*DMipmaps
      else
      {
        if (is_compressed)
        {
          Log::bug("Texture::createTexture(): \n"
                   "GL_GENERATE_MIPMAP not supported: cannot generate mipmaps from a compressed format using gluBuild1DMipmaps. "
                   "If you are loading the compressed image from a DDS file be sure it includes all the mipmaps. "
                   "Note that you can still generate mipmapped compressed textures from an uncompressed Image source. \n");
        }
        else
        {
          // glTexImage1D( GL_TEXTURE_1D, 0, format, xsize+2*brd, brd, image->format(), image->type(), NULL ); VL_CHECK_OGL()
          gluBuild1DMipmaps( GL_TEXTURE_1D, format, xsize, image->format(), image->type(), tex_pixels  ); 
          VL_CHECK_OGL()
        }
      }
      break;
    }
    case TD_TEXTURE_RECTANGLE:
    {
      Log::bug("Texture::createTexture(): \n"
               "ARB_texture_rectangle extension does not allow mipmapped textures\n");
      break;
    }
    case TD_TEXTURE_1D_ARRAY:
    case TD_TEXTURE_2D:
    {
      // use GL_GENERATE_MIPMAP
      if (GLEW_SGIS_generate_mipmap || GLEW_VERSION_1_4)
      {
        glTexParameteri( dimension(), GL_GENERATE_MIPMAP, GL_TRUE );
        if (is_compressed)
          glCompressedTexImage2D( dimension(), 0, format, xsize+2*brd, ysize+2*brd, brd, image->requiredMemory(), tex_pixels );
        else
          glTexImage2D( dimension(), 0, format, xsize+2*brd, ysize+2*brd, brd, image->format(), image->type(), tex_pixels );
        glTexParameteri( dimension(), GL_GENERATE_MIPMAP, gl_generate_mipmap );
        VL_CHECK_OGL()
      }
      // use gluBuild*DMipmaps
      else
      {
        if (is_compressed)
        {
          Log::bug("Texture::createTexture(): \n"
                   "GL_GENERATE_MIPMAP not supported: cannot generate mipmaps from a compressed format using gluBuild2DMipmaps. "
                   "If you are loading the compressed image from a DDS file be sure it includes all the mipmaps. "
                   "Note that you can still generate mipmapped compressed textures from an uncompressed Image source. \n");
        }
        else
        {
          // glTexImage2D( dimension(), 0, format, xsize+2*brd, ysize+2*brd, brd, image->format(), image->type(), NULL ); VL_CHECK_OGL()
          gluBuild2DMipmaps( dimension(), format, xsize, ysize, image->format(), image->type(), tex_pixels  ); 
          VL_CHECK_OGL()
        }
      }
      break;
    }
    case TD_TEXTURE_CUBE_MAP:
    {
      // use GL_GENERATE_MIPMAP
      if (GLEW_SGIS_generate_mipmap || GLEW_VERSION_1_4)
      {
        glTexParameteri(dimension(), GL_GENERATE_MIPMAP, GL_TRUE);
        if (is_compressed)
        {
          glCompressedTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, format, xsize+2*brd, ysize+2*brd, brd, image->requiredMemory() / 6, image->pixelsXP() );
          glCompressedTexImage2D( GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, format, xsize+2*brd, ysize+2*brd, brd, image->requiredMemory() / 6, image->pixelsXN() );
          glCompressedTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, format, xsize+2*brd, ysize+2*brd, brd, image->requiredMemory() / 6, image->pixelsYP() );
          glCompressedTexImage2D( GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, format, xsize+2*brd, ysize+2*brd, brd, image->requiredMemory() / 6, image->pixelsYN() );
          glCompressedTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, format, xsize+2*brd, ysize+2*brd, brd, image->requiredMemory() / 6, image->pixelsZP() );
          glCompressedTexImage2D( GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, format, xsize+2*brd, ysize+2*brd, brd, image->requiredMemory() / 6, image->pixelsZN() );
        }
        else
        {
          glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, format, xsize+2*brd, ysize+2*brd, brd, image->format(), image->type(), image->pixelsXP() );
          glTexImage2D( GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, format, xsize+2*brd, ysize+2*brd, brd, image->format(), image->type(), image->pixelsXN() );
          glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, format, xsize+2*brd, ysize+2*brd, brd, image->format(), image->type(), image->pixelsYP() );
          glTexImage2D( GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, format, xsize+2*brd, ysize+2*brd, brd, image->format(), image->type(), image->pixelsYN() );
          glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, format, xsize+2*brd, ysize+2*brd, brd, image->format(), image->type(), image->pixelsZP() );
          glTexImage2D( GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, format, xsize+2*brd, ysize+2*brd, brd, image->format(), image->type(), image->pixelsZN() );
        }
        glTexParameteri(dimension(), GL_GENERATE_MIPMAP, gl_generate_mipmap);
        VL_CHECK_OGL()
      }
      // use gluBuild2DMipmaps
      else
      {
        if (is_compressed)
        {
          Log::bug("Texture::createTexture(): \n"
                   "GL_GENERATE_MIPMAP not supported: cannot generate mipmaps from a compressed format using gluBuild2DMipmaps. "
                   "If you are loading the compressed image from a DDS file be sure it includes all the mipmaps. "
                   "Note that you can still generate mipmapped compressed textures from an uncompressed Image source. \n");
        }
        else
        {
          gluBuild2DMipmaps( GL_TEXTURE_CUBE_MAP_POSITIVE_X, format, xsize, ysize, image->format(), image->type(), image->pixelsXP()  );
          gluBuild2DMipmaps( GL_TEXTURE_CUBE_MAP_NEGATIVE_X, format, xsize, ysize, image->format(), image->type(), image->pixelsXN() );
          gluBuild2DMipmaps( GL_TEXTURE_CUBE_MAP_POSITIVE_Y, format, xsize, ysize, image->format(), image->type(), image->pixelsYP() );
          gluBuild2DMipmaps( GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, format, xsize, ysize, image->format(), image->type(), image->pixelsYN() );
          gluBuild2DMipmaps( GL_TEXTURE_CUBE_MAP_POSITIVE_Z, format, xsize, ysize, image->format(), image->type(), image->pixelsZP() );
          gluBuild2DMipmaps( GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, format, xsize, ysize, image->format(), image->type(), image->pixelsZN() );
          VL_CHECK_OGL()
        }
      }
      break;
    }
    case TD_TEXTURE_2D_ARRAY:
    case TD_TEXTURE_3D:
      // use GL_GENERATE_MIPMAP
      if (GLEW_SGIS_generate_mipmap || GLEW_VERSION_1_4)
      {
        glTexParameteri( dimension(), GL_GENERATE_MIPMAP, GL_TRUE);
        if (is_compressed)
          glCompressedTexImage3D( dimension(), 0, format, xsize+2*brd, ysize+2*brd, zsize+2*brd, brd, image->requiredMemory(), tex_pixels );
        else
          glTexImage3D( dimension(), 0, format, xsize+2*brd, ysize+2*brd, zsize+2*brd, brd, image->format(), image->type(), tex_pixels );
        glTexParameteri(dimension(), GL_GENERATE_MIPMAP, gl_generate_mipmap);
        VL_CHECK_OGL()
      }
      else
      {
        Log::bug("Texture::createTexture(): \n"
                 "GL_GENERATE_MIPMAP not supported: cannot generate mipmaps for a 3d texture. \n"
                 "If you are loading the image from a DDS file be sure it includes all the mipmaps. \n");
      }
      // gluBuild3DMipmaps( dimension(), format, xsize, ysize, zsize, image->format(), image->type(), tex_pixels  ); VL_CHECK_OGL()
      break;
    default:
      VL_TRAP()
    }
  }
  else // use existing mipmaps
  if ( has_mipmaps && !image->mipmaps().empty() )
  {
    switch( dimension() )
    {
    case TD_TEXTURE_1D:
    {
      // create texture with mipmaps
      std::vector< ref<Image> > mipmaps = image->mipmaps();
      mipmaps.insert(mipmaps.begin(), image);
      for(int i=0; i<(int)mipmaps.size(); ++i)
      {
        int width = mipmaps[i]->width();
        if (is_compressed)
        {
          glCompressedTexImage1D( GL_TEXTURE_1D, i, format, width+2*brd, brd, mipmaps[i]->requiredMemory(), mipmaps[i]->pixels() ); VL_CHECK_OGL()
        }
        else
        {
          glTexImage1D( GL_TEXTURE_1D, i, format, width+2*brd, brd, mipmaps[i]->format(), mipmaps[i]->type(), mipmaps[i]->pixels() ); VL_CHECK_OGL()
        }
      }
      break;
    }
    case TD_TEXTURE_RECTANGLE:
    {
      Log::bug("Texture::createTexture(): \n"
               "ARB_texture_rectangle extension does not allow mipmapped textures\n");
      break;
    }
    case TD_TEXTURE_1D_ARRAY:
    case TD_TEXTURE_2D:
    {
      // create texture with mipmaps
      std::vector< ref<Image> > mipmaps = image->mipmaps();
      mipmaps.insert(mipmaps.begin(), image);
      for(int i=0; i<(int)mipmaps.size(); ++i)
      {
        int width = mipmaps[i]->width();
        int height = mipmaps[i]->height();
        if (is_compressed)
        {
          glCompressedTexImage2D( dimension(), i, format, width+2*brd, height+2*brd, brd, mipmaps[i]->requiredMemory(), mipmaps[i]->pixels() ); VL_CHECK_OGL()
        }
        else
        {
          glTexImage2D( dimension(), i, format, width+2*brd, height+2*brd, brd, mipmaps[i]->format(), mipmaps[i]->type(), mipmaps[i]->pixels() ); VL_CHECK_OGL()
        }
      }
      break;
    }
    case TD_TEXTURE_CUBE_MAP:
    {
      // create texture with mipmaps
      std::vector< ref<Image> > mipmaps = image->mipmaps();
      mipmaps.insert(mipmaps.begin(), image);
      for(int i_mip=0; i_mip<(int)mipmaps.size(); ++i_mip)
      {
        int width = mipmaps[i_mip]->width();
        int height = mipmaps[i_mip]->height();
        if (is_compressed)
        {
          glCompressedTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X, i_mip, format, width+2*brd, height+2*brd, brd, mipmaps[i_mip]->requiredMemory() / 6, mipmaps[i_mip]->pixelsXP() ); 
          glCompressedTexImage2D( GL_TEXTURE_CUBE_MAP_NEGATIVE_X, i_mip, format, width+2*brd, height+2*brd, brd, mipmaps[i_mip]->requiredMemory() / 6, mipmaps[i_mip]->pixelsXN() ); 
          glCompressedTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_Y, i_mip, format, width+2*brd, height+2*brd, brd, mipmaps[i_mip]->requiredMemory() / 6, mipmaps[i_mip]->pixelsYP() ); 
          glCompressedTexImage2D( GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, i_mip, format, width+2*brd, height+2*brd, brd, mipmaps[i_mip]->requiredMemory() / 6, mipmaps[i_mip]->pixelsYN() ); 
          glCompressedTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_Z, i_mip, format, width+2*brd, height+2*brd, brd, mipmaps[i_mip]->requiredMemory() / 6, mipmaps[i_mip]->pixelsZP() ); 
          glCompressedTexImage2D( GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, i_mip, format, width+2*brd, height+2*brd, brd, mipmaps[i_mip]->requiredMemory() / 6, mipmaps[i_mip]->pixelsZN() ); 
        }
        else
        {
          glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X, i_mip, format, width+2*brd, height+2*brd, brd, mipmaps[i_mip]->format(), mipmaps[i_mip]->type(), mipmaps[i_mip]->pixelsXP() ); 
          glTexImage2D( GL_TEXTURE_CUBE_MAP_NEGATIVE_X, i_mip, format, width+2*brd, height+2*brd, brd, mipmaps[i_mip]->format(), mipmaps[i_mip]->type(), mipmaps[i_mip]->pixelsXN() ); 
          glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_Y, i_mip, format, width+2*brd, height+2*brd, brd, mipmaps[i_mip]->format(), mipmaps[i_mip]->type(), mipmaps[i_mip]->pixelsYP() ); 
          glTexImage2D( GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, i_mip, format, width+2*brd, height+2*brd, brd, mipmaps[i_mip]->format(), mipmaps[i_mip]->type(), mipmaps[i_mip]->pixelsYN() ); 
          glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_Z, i_mip, format, width+2*brd, height+2*brd, brd, mipmaps[i_mip]->format(), mipmaps[i_mip]->type(), mipmaps[i_mip]->pixelsZP() ); 
          glTexImage2D( GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, i_mip, format, width+2*brd, height+2*brd, brd, mipmaps[i_mip]->format(), mipmaps[i_mip]->type(), mipmaps[i_mip]->pixelsZN() ); 
        }
        VL_CHECK_OGL()
      }
      break;
    }
    case TD_TEXTURE_2D_ARRAY:
    case TD_TEXTURE_3D:
    {
      // create texture with mipmaps
      std::vector< ref<Image> > mipmaps = image->mipmaps();
      mipmaps.insert(mipmaps.begin(), image);
      for(int i=0; i<(int)mipmaps.size(); ++i)
      {
        int width = mipmaps[i]->width();
        int height = mipmaps[i]->height();
        int depth = mipmaps[i]->depth();
        if (is_compressed)
        {
          glCompressedTexImage3D( dimension(), i, format, width+2*brd, height+2*brd, depth+2*brd, brd, mipmaps[i]->requiredMemory(), mipmaps[i]->pixels() ); 
          VL_CHECK_OGL()
        }
        else
        {
          glTexImage3D( dimension(), i, format, width+2*brd, height+2*brd, depth+2*brd, brd, mipmaps[i]->format(), mipmaps[i]->type(), mipmaps[i]->pixels() ); 
          VL_CHECK_OGL()
        }
      }
      break;
    }

    default:
      VL_TRAP()
    }
  }
  else // no mipmaps
  {
    switch( dimension() )
    {
      case TD_TEXTURE_1D:
      {
        if (is_compressed)
        {
          glCompressedTexImage1D( GL_TEXTURE_1D, 0, format, xsize+2*brd, brd, image->requiredMemory(), image->pixels() ); VL_CHECK_OGL()
        }
        else
        {
          glTexImage1D( GL_TEXTURE_1D, 0, format, xsize+2*brd, brd, image->format(), image->type(), image->pixels() ); VL_CHECK_OGL()
        }
      }
      break;
      case TD_TEXTURE_RECTANGLE:
      {
        if (is_compressed)
        {
          glCompressedTexImage2D( GL_TEXTURE_RECTANGLE_EXT, 0, format, xsize, ysize, 0, image->requiredMemory(), image->pixels() ); VL_CHECK_OGL()
        }
        else
        {
          glTexImage2D( GL_TEXTURE_RECTANGLE_EXT, 0, format, xsize, ysize, 0, image->format(), image->type(), image->pixels() ); VL_CHECK_OGL()
        }
      }
      break;
      case TD_TEXTURE_1D_ARRAY:
      case TD_TEXTURE_2D:
      {
        if (is_compressed)
        {
          glCompressedTexImage2D( dimension(), 0, format, xsize+2*brd, ysize+2*brd, brd, image->requiredMemory(), image->pixels() ); VL_CHECK_OGL()
        }
        else
        {
          glTexImage2D( dimension(), 0, format, xsize+2*brd, ysize+2*brd, brd, image->format(), image->type(), image->pixels() ); VL_CHECK_OGL()
        }
      }
      break;
      case TD_TEXTURE_CUBE_MAP:
      {
        if (is_compressed)
        {
          glCompressedTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, format, xsize+2*brd, ysize+2*brd, brd, image->requiredMemory() / 6, image->pixelsXP() );
          glCompressedTexImage2D( GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, format, xsize+2*brd, ysize+2*brd, brd, image->requiredMemory() / 6, image->pixelsXN() );
          glCompressedTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, format, xsize+2*brd, ysize+2*brd, brd, image->requiredMemory() / 6, image->pixelsYP() );
          glCompressedTexImage2D( GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, format, xsize+2*brd, ysize+2*brd, brd, image->requiredMemory() / 6, image->pixelsYN() );
          glCompressedTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, format, xsize+2*brd, ysize+2*brd, brd, image->requiredMemory() / 6, image->pixelsZP() );
          glCompressedTexImage2D( GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, format, xsize+2*brd, ysize+2*brd, brd, image->requiredMemory() / 6, image->pixelsZN() );
        }
        else
        {
          glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, format, xsize+2*brd, ysize+2*brd, brd, image->format(), image->type(), image->pixelsXP() );
          glTexImage2D( GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, format, xsize+2*brd, ysize+2*brd, brd, image->format(), image->type(), image->pixelsXN() );
          glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, format, xsize+2*brd, ysize+2*brd, brd, image->format(), image->type(), image->pixelsYP() );
          glTexImage2D( GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, format, xsize+2*brd, ysize+2*brd, brd, image->format(), image->type(), image->pixelsYN() );
          glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, format, xsize+2*brd, ysize+2*brd, brd, image->format(), image->type(), image->pixelsZP() );
          glTexImage2D( GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, format, xsize+2*brd, ysize+2*brd, brd, image->format(), image->type(), image->pixelsZN() );
        }
      }
      break;
    case TD_TEXTURE_2D_ARRAY:
    case TD_TEXTURE_3D:
      {
        if (is_compressed)
        {
          glCompressedTexImage3D( dimension(), 0, format, xsize+2*brd, ysize+2*brd, zsize+2*brd, brd, image->requiredMemory(), image->pixels() ); 
          VL_CHECK_OGL()
        }
        else
        {
          glTexImage3D( dimension(), 0, format, xsize+2*brd, ysize+2*brd, zsize+2*brd, brd, image->format(), image->type(), image->pixels() ); 
          VL_CHECK_OGL()
        }
      }
      break;

      default:
        VL_TRAP()
    }
  }

  // the new VL policy requires a clean state.

  glBindTexture( dimension(), 0 ); VL_CHECK_OGL()

  // restore default GL_UNPACK_ALIGNMENT
  glPixelStorei(GL_UNPACK_ALIGNMENT, 4); VL_CHECK_OGL()
  return true;
}
//-----------------------------------------------------------------------------
bool Texture::isCompressedFormat(int format)
{
  int comp[] =
  {
    TF_COMPRESSED_ALPHA,
    TF_COMPRESSED_INTENSITY,
    TF_COMPRESSED_LUMINANCE,
    TF_COMPRESSED_LUMINANCE_ALPHA,
    TF_COMPRESSED_RGB,
    TF_COMPRESSED_RGBA,

    // 3DFX_texture_compression_FXT1
    TF_COMPRESSED_RGB_FXT1_3DFX,
    TF_COMPRESSED_RGBA_FXT1_3DFX,

    // EXT_texture_compression_s3tc
    TF_COMPRESSED_RGB_S3TC_DXT1,
    TF_COMPRESSED_RGBA_S3TC_DXT1,
    TF_COMPRESSED_RGBA_S3TC_DXT3,
    TF_COMPRESSED_RGBA_S3TC_DXT5,

    // GL_EXT_texture_compression_latc
    TF_COMPRESSED_LUMINANCE_LATC1,
    TF_COMPRESSED_SIGNED_LUMINANCE_LATC1,
    TF_COMPRESSED_LUMINANCE_ALPHA_LATC2,
    TF_COMPRESSED_SIGNED_LUMINANCE_ALPHA_LATC2,

    // EXT_texture_compression_rgtc
    TF_COMPRESSED_RED_RGTC1,
    TF_COMPRESSED_SIGNED_RED_RGTC1,
    TF_COMPRESSED_RED_GREEN_RGTC2,
    TF_COMPRESSED_SIGNED_RED_GREEN_RGTC2,

    0
  };

  for(int i=0; comp[i]; ++i)
  {
    if(comp[i] == format)
      return true;
  }

  return false;
}
