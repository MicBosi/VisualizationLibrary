/**************************************************************************************/
/*                                                                                    */
/*  Visualization Library                                                             */
/*  http://visualizationlibrary.org                                                   */
/*                                                                                    */
/*  Copyright (c) 2005-2020, Michele Bosi                                             */
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

#include "ioPNG.hpp"
#include <vlCore/LoadWriterManager.hpp>
#include <vlCore/VisualizationLibrary.hpp>
#include <vlCore/FileSystem.hpp>
#include <vlCore/VirtualFile.hpp>
#include <vlCore/Image.hpp>
#include "png.h"

using namespace vl;

namespace
{
  void png_read_vfile(png_structp png_ptr, png_bytep data, png_size_t byte_count)
  {
    VirtualFile* vfile = (VirtualFile*)png_get_io_ptr(png_ptr);
    vfile->read(data, byte_count);
  }
  void png_write_vfile(png_structp png_ptr, png_bytep data, png_size_t byte_count)
  {
    VirtualFile* vfile = (VirtualFile*)png_get_io_ptr(png_ptr);
    vfile->write(data,byte_count);
  }
  void png_flush_vfile(png_structp /*png_ptr*/)
  {
    // do nothing
  }
  void vl_error_fn(png_structp png_ptr, png_const_charp error_msg)
  {
    Log::error( Say("libPNG: %s\n") << error_msg);
    // return to the setjmp() in the *_guarded() functions below
    png_longjmp(png_ptr, 1);
  }
  void vl_warning_fn(png_structp /*png_ptr*/, png_const_charp warning_msg)
  {
    Log::warning( Say("libPNG: %s\n") << warning_msg);
  }

  // The functions below call setjmp(): they must not hold C++ objects with
  // non-trivial destructors, since libpng errors longjmp back into them.

  bool png_read_header_guarded(png_structp png_ptr, png_infop info_ptr, VirtualFile* file,
                               png_uint_32* width, png_uint_32* height, int* bit_depth, int* color_type)
  {
    if (setjmp(png_jmpbuf(png_ptr)))
      return false;

    png_set_read_fn(png_ptr, file, png_read_vfile);
    png_set_sig_bytes(png_ptr, 8);

    /* The call to png_read_info() gives us all of the information from the
    * PNG file before the first IDAT (image data chunk).  REQUIRED
    */
    png_read_info(png_ptr, info_ptr);

    png_get_IHDR(png_ptr, info_ptr, width, height, bit_depth, color_type, NULL/*&interlace_type*/, NULL, NULL);

    /* Extract multiple pixels with bit depths of 1, 2, and 4 from a single
    * byte into separate bytes (useful for paletted and grayscale images).
    */
    png_set_packing(png_ptr);

    /* Expand paletted colors into true RGB triplets */
    if (*color_type == PNG_COLOR_TYPE_PALETTE)
      png_set_palette_to_rgb(png_ptr);

    /* Expand grayscale images to the full 8 bits from 1, 2, or 4 bits/pixel */
    if (*color_type == PNG_COLOR_TYPE_GRAY && *bit_depth < 8)
      png_set_expand_gray_1_2_4_to_8(png_ptr);

    /* Expand paletted or RGB images with transparency to full alpha channels
    * so the data will be available as RGBA quartets.
    */
    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
      png_set_tRNS_to_alpha(png_ptr);

    /* Tell libpng to handle the gamma conversion for you. */
    double screen_gamma = 2.2; /* A good guess for a PC monitors in a dimly lit room */
    double image_gamma;
    if (png_get_gAMA(png_ptr, info_ptr, &image_gamma))
      png_set_gamma(png_ptr, screen_gamma, image_gamma);
    else
      png_set_gamma(png_ptr, screen_gamma, 1.0/screen_gamma/*0.45455*/);

    /* swap bytes of 16 bit files to least significant byte first */
    unsigned short bet = 0x00FF;
    bool little_endian_cpu = ((unsigned char*)&bet)[0] == 0xFF;
    if (little_endian_cpu && *bit_depth > 8)
      png_set_swap(png_ptr);

    png_set_interlace_handling(png_ptr);

    return true;
  }

  bool png_read_pixels_guarded(png_structp png_ptr, png_infop endinfo, png_bytepp rows)
  {
    if (setjmp(png_jmpbuf(png_ptr)))
      return false;

    png_read_image(png_ptr, rows);
    png_read_end(png_ptr, endinfo);
    return true;
  }

  bool png_write_guarded(png_structp png, png_infop info, png_bytepp rows,
                         int w, int h, int bit_depth, int color, int compression)
  {
    if (setjmp(png_jmpbuf(png)))
      return false;

    png_set_compression_level(png, compression);

    png_set_IHDR( png, info, w, h, bit_depth, color,
                  PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    unsigned short bet = 0x00FF;
    bool little_endian_cpu = ((unsigned char*)&bet)[0] == 0xFF;
    if (little_endian_cpu && bit_depth == 16)
    {
      png_set_rows(png, info, rows);
      png_write_png(png, info, PNG_TRANSFORM_SWAP_ENDIAN, NULL);
    }
    else
    {
      png_write_info(png, info);
      png_write_image(png, rows);
      png_write_end(png, NULL);
    }
    return true;
  }
}

//-----------------------------------------------------------------------------
ref<Image> vl::loadPNG(const String& path)
{
  ref<VirtualFile> file = defFileSystem()->locateFile(path);
  if ( !file )
  {
    Log::error( Say("File '%s' not found.\n") << path );
    return NULL;
  }
  else
    return loadPNG(file.get());
}
//-----------------------------------------------------------------------------
ref<Image> vl::loadPNG(VirtualFile* file)
{
  if ( !file->open(OM_ReadOnly) )
  {
    Log::error( Say("loadPNG: cannot load PNG file '%s'\n") << file->path() );
    return NULL;
  }

  png_structp png_ptr;
  png_infop info_ptr;
  png_infop endinfo;
  png_uint_32 width, height;
  int bit_depth, color_type;

  unsigned char header[8];
  int count = (int)file->read(header,8);
  if (count != 8 || png_sig_cmp(header, 0, 8) != 0)
  {
    file->close();
    return NULL;
  }

  png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, vl_error_fn, vl_warning_fn);
  if (png_ptr == NULL)
  {
    file->close();
    return NULL;
  }

  /* Allocate/initialize the memory for image information.  REQUIRED. */
  info_ptr = png_create_info_struct(png_ptr);
  endinfo  = png_create_info_struct(png_ptr);
  if (!info_ptr || !endinfo)
  {
    png_destroy_read_struct(&png_ptr, info_ptr ? &info_ptr : NULL, endinfo ? &endinfo : NULL);
    file->close();
    return NULL;
  }

  if (!png_read_header_guarded(png_ptr, info_ptr, file, &width, &height, &bit_depth, &color_type))
  {
    png_destroy_read_struct(&png_ptr, &info_ptr, &endinfo);
    file->close();
    return NULL;
  }

  ref<Image> img = new Image;
  img->setObjectName(file->path().toStdString().c_str());

  if (bit_depth == 16)
  {
    switch(color_type)
    {
      case PNG_COLOR_TYPE_GRAY:       img->allocate2D(width, height, 1, IF_LUMINANCE, IT_UNSIGNED_SHORT); break;
      case PNG_COLOR_TYPE_GRAY_ALPHA: img->allocate2D(width, height, 1, IF_LUMINANCE_ALPHA, IT_UNSIGNED_SHORT); break;
      case PNG_COLOR_TYPE_PALETTE:    img->allocate2D(width, height, 1, IF_RGB, IT_UNSIGNED_SHORT); break;
      case PNG_COLOR_TYPE_RGB:        img->allocate2D(width, height, 1, IF_RGB, IT_UNSIGNED_SHORT); break;
      case PNG_COLOR_TYPE_RGB_ALPHA:  img->allocate2D(width, height, 1, IF_RGBA, IT_UNSIGNED_SHORT); break;
      default:
        VL_TRAP()
        break;
    }
  }
  else
  {
    switch(color_type)
    {
      case PNG_COLOR_TYPE_GRAY:       img->allocate2D(width, height, 1, IF_LUMINANCE, IT_UNSIGNED_BYTE); break;
      case PNG_COLOR_TYPE_GRAY_ALPHA: img->allocate2D(width, height, 1, IF_LUMINANCE_ALPHA, IT_UNSIGNED_BYTE); break;
      case PNG_COLOR_TYPE_PALETTE:    img->allocate2D(width, height, 1, IF_RGBA, IT_UNSIGNED_BYTE); break;
      case PNG_COLOR_TYPE_RGB:        img->allocate2D(width, height, 1, IF_RGB, IT_UNSIGNED_BYTE); break;
      case PNG_COLOR_TYPE_RGB_ALPHA:  img->allocate2D(width, height, 1, IF_RGBA, IT_UNSIGNED_BYTE); break;
      default:
        VL_TRAP()
        break;
    }
  }

  // initialize row pointers
  std::vector<png_bytep> row_p;
  row_p.resize(height);
  for(unsigned i=0; i<height; ++i)
    row_p[height - 1 - i] = (png_bytep)img->pixels()+img->pitch()*i;

  bool ok = png_read_pixels_guarded(png_ptr, endinfo, &row_p[0]);

  /* clean up after the read, and free any memory allocated - REQUIRED */
  png_destroy_read_struct(&png_ptr, &info_ptr, &endinfo);

  file->close();
  if (!ok)
    return NULL;
  return img;
}
//-----------------------------------------------------------------------------
bool vl::isPNG(VirtualFile* file)
{
  if ( !file->open(OM_ReadOnly) )
  {
    // Log::error( Say("loadPNG: cannot load PNG file '%s'\n") << file->path() );
    return false;
  }

  unsigned char header[8];
  int count = (int)file->read(header,8);
  file->close();
  if (count == 8 && png_sig_cmp(header, 0, 8) == 0)
    return true;
  else
    return false;
}
//-----------------------------------------------------------------------------
bool vl::savePNG(const Image* src, const String& path, int compression)
{
  ref<DiskFile> file = new DiskFile(path);
  return savePNG(src, file.get(), compression);
}
//-----------------------------------------------------------------------------
bool vl::savePNG(const Image* src, VirtualFile* fout, int compression)
{
  /*if ( src->dimension() != ID_2D && src->dimension() != ID_1D && src->dimension() != ID_3D )
  {
    Log::error( Say("savePNG('%s'): can save only 1D, 2D and 3D images.\n") << fout->path() );
    return false;
  }*/

  int w = src->width();
  int h = src->height();
  int d = src->depth();
  if (h == 0) h=1;
  if (d == 0) d=1;
  if (src->isCubemap()) d=6;
  h = h*d;

  ref<Image> cimg;
  // convert to IT_UNSIGNED_SHORT if necessary
  if (src->type() == IT_FLOAT || src->type() == IT_SHORT)
  {
    cimg = src->convertType(IT_UNSIGNED_SHORT);
    src = cimg.get();
    if (!cimg)
    {
      Log::error( Say("savePNG('%s'): could not convert image to IT_UNSIGNED_SHORT.\n") << fout->path() );
      return false;
    }
  }

  bool format_ok = src->format() == IF_RGB || src->format() == IF_RGBA || src->format() == IF_LUMINANCE || src->format() == IF_ALPHA || src->format() == IF_LUMINANCE_ALPHA;
  if (!format_ok)
  {
    cimg = src->convertFormat(IF_RGBA);
    src = cimg.get();
    if (!cimg)
    {
      Log::error( Say("savePNG('%s'): could not convert image to IF_RGBA.\n") << fout->path() );
      return false;
    }
  }

  VL_CHECK(src->type() == IT_UNSIGNED_BYTE || src->type() == IT_UNSIGNED_SHORT)
  VL_CHECK(src->format() == IF_RGB || src->format() == IF_RGBA || src->format() == IF_LUMINANCE || src->format() == IF_ALPHA || src->format() == IF_LUMINANCE_ALPHA)

  if(!fout->open(OM_WriteOnly))
  {
    Log::error( Say("PNG: could not write to '%s'.\n") << fout->path() );
    return false;
  }

  png_structp png  = NULL;
  png_infop   info = NULL;

  png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, vl_error_fn, vl_warning_fn);
  if(!png)
  {
    fout->close();
    return false;
  }

  info = png_create_info_struct(png);
  if(!info)
  {
    png_destroy_write_struct(&png, NULL);
    fout->close();
    return false;
  }

  png_set_write_fn(png,fout,png_write_vfile,png_flush_vfile);

  std::vector< png_bytep > rows;
  rows.resize(h);
  for(int i=0, y = h-1; i<h; ++i,--y)
    rows[i] = (png_bytep)(src->pixels()+src->pitch()*y);

  int color;
  switch(src->format())
  {
      case IF_RGB:             color = PNG_COLOR_TYPE_RGB; break;
      case IF_RGBA:            color = PNG_COLOR_TYPE_RGB_ALPHA; break;
      case IF_ALPHA:
      case IF_LUMINANCE:       color = PNG_COLOR_TYPE_GRAY; break;
      case IF_LUMINANCE_ALPHA: color = PNG_COLOR_TYPE_GRAY_ALPHA; break;
      default:
        png_destroy_write_struct(&png,&info);
        fout->close();
        return false;
  }

  int bit_depth = src->type() == IT_UNSIGNED_SHORT ? 16 : 8;

  bool ok = png_write_guarded(png, info, &rows[0], w, h, bit_depth, color, compression);

  png_destroy_write_struct(&png,&info);

  fout->close();
  return ok;
}
//-----------------------------------------------------------------------------
