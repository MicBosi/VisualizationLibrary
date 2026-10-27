/**************************************************************************************/
/*                                                                                    */
/*  Visualization Library                                                             */
/*  http://www.visualizationlibrary.org                                               */
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

#include <vlGraphics/CoreFont.hpp>
#include <vlGraphics/OpenGL.hpp>
#include <vlGraphics/FontManager.hpp>
#include <vlCore/Log.hpp>
#include <vlCore/Say.hpp>
#include <vlCore/FileSystem.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

using namespace vl;

// FreeType error table construction start ------------------------------------------
// taken from "fterrors.h" example
#undef __FTERRORS_H__
#define FT_ERRORDEF( e, v, s )  { e, s },
#define FT_ERROR_START_LIST     {
#define FT_ERROR_END_LIST       { 0, 0 } };

const struct
{
 int          err_code;
 const char*  err_msg;
} ft_errors[] =

#include FT_ERRORS_H
// FreeType error table construction end ------------------------------------------

const char* get_ft_error_message_core(int error)
{
  int i=0;
  while( ft_errors[i].err_msg && ft_errors[i].err_code != error )
    ++i;
  return ft_errors[i].err_msg;
}

//-----------------------------------------------------------------------------
// Glyph
//-----------------------------------------------------------------------------
CoreGlyph::~CoreGlyph()
{
  /*if (mTextureHandle)
  {
    glDeleteTextures(1, &mTextureHandle);
    mTextureHandle = 0;
  }*/
}
//-----------------------------------------------------------------------------
// Font
//-----------------------------------------------------------------------------
CoreFont::CoreFont(FontManager* fm)
{
  VL_DEBUG_SET_OBJECT_NAME()
  mFontManager = fm;
  mHeight  = 0;
  mFT_Face = NULL;
  mSmooth  = false;
  mFreeTypeLoadForceAutoHint = true;
  //mTextureHandle = 0;
  setSize(32);
}
//-----------------------------------------------------------------------------
CoreFont::CoreFont(FontManager* fm, const String& font_file, int size)
{
  VL_DEBUG_SET_OBJECT_NAME()
  mFontManager = fm;
  mHeight  = 0;
  mFT_Face = NULL;
  mSmooth  = false;
  mFreeTypeLoadForceAutoHint = true;
  //mTextureHandle = 0;
  loadFont(font_file);
  setSize(size);
}
//-----------------------------------------------------------------------------
CoreFont::~CoreFont()
{
  releaseFreeTypeData();
}
//-----------------------------------------------------------------------------
void CoreFont::releaseFreeTypeData()
{
  if (mFT_Face)
  {
    if (!mFontManager->freeTypeLibrary())
    {
      vl::Log::error("Font::releaseFreeTypeData(): mFontManager->freeTypeLibrary() is NULL!\n");
      VL_TRAP()
    }
    else
    {
      FT_Done_Face(mFT_Face);
    }
    mFT_Face = NULL;
  }
}
//-----------------------------------------------------------------------------
void CoreFont::setSize(int size)
{
  if(mSize != size)
  {
    mSize = size;
    // removes all the cached glyphs
    //mGlyphMap.clear();
  }
}
//-----------------------------------------------------------------------------
void CoreFont::loadFont(const String& path)
{
  if(path == mFilePath)
    return;

  mFilePath = path;
  // removes all the cached glyphs
  mGlyphMap.clear();

  // remove FreeType font face object
  if (mFT_Face)
  {
    FT_Done_Face(mFT_Face);
    mFT_Face = NULL;
  }

  FT_Error error = 0;

  ref<VirtualFile> font_file = defFileSystem()->locateFile( filePath() );

  if (!font_file)
    Log::error( Say("Font::loadFont('%s'): font file not found.\n") << filePath() );

  if ( font_file && font_file->load(mMemoryFile) )
  {
    if ( (int)mMemoryFile.size() == font_file->size() )
    {
      error = FT_New_Memory_Face( (FT_Library)mFontManager->freeTypeLibrary(),
                                  (FT_Byte*)&mMemoryFile[0],
                                  (int)mMemoryFile.size(),
                                  0,
                                  &mFT_Face );
    }
    else
      Log::error( Say("Font::loadFont('%s'): could not read file.\n") << filePath() );
  }

  if (error)
  {
    Log::error(Say("FT_New_Face error (%s): %s\n") << filePath() << get_ft_error_message_core(error) );
    VL_TRAP()
    return;
  }
  
  createTexture();
}
//-----------------------------------------------------------------------------
CoreGlyph* CoreFont::glyph(int character)
{
  ref<CoreGlyph>& glyph = mGlyphMap[character];

  if (glyph.get() == NULL)
  {
	Log::error(Say("Error during gettin glyph\n"));
	return NULL;
  }

  return glyph.get();
}
//-----------------------------------------------------------------------------
void CoreFont::setSmooth(bool smooth)
{
  mSmooth = smooth;
  if(mAtlas->handle() == 0)
	return;

  vl::TexParameter* parameters = mAtlas->getTexParameter();

  //glBindTexture( GL_TEXTURE_2D, mAtlas->handle() );
  if (smooth)
  {
      parameters->setMagFilter(vl::TPF_LINEAR);
      parameters->setMinFilter(vl::TPF_LINEAR);
      parameters->setWrapS(vl::TPW_CLAMP);
      parameters->setWrapT(vl::TPW_CLAMP);

    /*glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );*/
  }
  else
  {
      parameters->setMagFilter(vl::TPF_NEAREST);
      parameters->setMinFilter(vl::TPF_NEAREST);
      parameters->setWrapS(vl::TPW_CLAMP);
      parameters->setWrapT(vl::TPW_CLAMP);

    /*glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );*/
  }
  //glBindTexture( GL_TEXTURE_2D, 0 );
  /*std::map<int, ref<Glyph> >::iterator it = mGlyphMap.begin();
  for(; it != mGlyphMap.end(); ++it )
  {
    const ref<Glyph>& glyph = it->second;
    if (glyph->textureHandle() == 0)
      continue;

    glBindTexture( GL_TEXTURE_2D, glyph->textureHandle() );
    if (smooth)
    {
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
    }
    else
    {
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
    }
  }
  glBindTexture( GL_TEXTURE_2D, 0 );*/
}
//-----------------------------------------------------------------------------
void CoreFont::createTexture()
{
  /*if(mTextureHandle != 0)
  {
    glDeleteTextures(1, &mTextureHandle);
	mTextureHandle = 0;
  }*/
  
  
  FT_Error error = 0;

  error = FT_Set_Char_Size(
            mFT_Face, /* handle to face object           */
            0,        /* char_width in 1/64th of points  */
            mSize*64, /* char_height in 1/64th of points */
            96,       /* horizontal device resolution    */
            96 );     /* vertical device resolution      */
 
  if(error)
  {
    // Log::error(Say("FT_Set_Char_Size error: %s\n") << get_ft_error_message(error) );
    if ( (mFT_Face->face_flags & FT_FACE_FLAG_SCALABLE) == 0 && mFT_Face->num_fixed_sizes)
    {
      // look for the size which is less or equal to the given size

	  int best_match_index = -1;
      int best_match_size  = 0;
      for( int i=0; i < mFT_Face->num_fixed_sizes; ++i )
      {
        int size = mFT_Face->available_sizes[i].y_ppem/64;
        // skip bigger characters
        if (size <= mSize)
        {
          if (best_match_index == -1 || (mSize - size) < (mSize - best_match_size) )
          {
            best_match_index = i;
            best_match_size  = size;
          }
        }
      }

	  if (best_match_index == -1)
        best_match_index = 0;

      error = FT_Select_Size(mFT_Face, best_match_index);
      if (error)
        Log::error(Say("FT_Select_Size error (%s): %s\n") << filePath() << get_ft_error_message_core(error) );
      VL_CHECK(!error)
    }
    // else
    {
      Log::error(Say("FT_Set_Char_Size error (%s): %s\n") << filePath() << get_ft_error_message_core(error) );
      VL_TRAP()
      return;
    }
  }

  mHeight = mFT_Face->size->metrics.height / 64.0f;
	
  // for now, only ASCII characters
  FT_GlyphSlot g = mFT_Face->glyph;
  int w, h;
  w = h = 0;
  int margin = 1;
  for(int i = 32; i < 128; ++i)
  {
	error = FT_Load_Char(mFT_Face, i, freeTypeLoadForceAutoHint() ? FT_LOAD_FORCE_AUTOHINT : FT_LOAD_DEFAULT);
	if(error)
	{
	  Log::error(Say("FT_Load_Char error (%s): %s\n") << filePath() << get_ft_error_message_core(error) );
	  continue;
	}
	
	error = FT_Render_Glyph(
            mFT_Face->glyph,  /* glyph slot */
            FT_RENDER_MODE_NORMAL ); /* render mode: FT_RENDER_MODE_MONO or FT_RENDER_MODE_NORMAL */
	
	if(error)
    {
      // Log::error(Say("FT_Render_Glyph error: %s") << get_ft_error_message(error) );
      // VL_TRAP()
      error = FT_Load_Glyph(
                mFT_Face,/* handle to face object */
                0,       /* glyph index           */
                FT_LOAD_DEFAULT ); /* load flags, see below */
      
      error = FT_Render_Glyph(
                mFT_Face->glyph,  /* glyph slot */
                FT_RENDER_MODE_NORMAL ); /* render mode: FT_RENDER_MODE_MONO or FT_RENDER_MODE_NORMAL */
    }

    if(error)
    {
      Log::error(Say("FT_Render_Glyph error (%s): %s\n") << filePath() << get_ft_error_message_core(error) );
      VL_TRAP()
      return;
    }
	
	if(g->bitmap.width == 0)
		w += 2 * margin;
	else
		w += g->bitmap.width + 2 * margin;
		
    h = std::max(h, g->bitmap.rows);
  }
  
  int texsize[] = { 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 0 };
  int max_tex_size = 0;
  glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_tex_size);

  //int w=0, h=0, margin = 1;
  
  for(int i = 0; texsize[i]; ++i)
  {
	if( texsize[i] >= w + margin * 2 || texsize[i+1] > max_tex_size)
	{
		w = texsize[i];
		break;
	}
  }
    
  for(int i = 0; texsize[i]; ++i)
  {
	if( texsize[i] >= h + margin * 2 || texsize[i+1] > max_tex_size)
	{
		h = texsize[i];
		break;
	}
  }
  VL_CHECK(w)
  VL_CHECK(h)

  ref<Image> img = new Image;
  img->allocate2D(w, h, 1, IF_RED, IT_UNSIGNED_BYTE);

  // init to all transparent white
  for(unsigned char *px = img->pixels(), *end = px + img->requiredMemory(); px<end; px+=1)
  {
    px[0] = 0x0;
  }
  
  /*glGenTextures(1, &mTextureHandle);

  VL_glActiveTexture(GL_TEXTURE0);

  glBindTexture(GL_TEXTURE_2D, mTextureHandle);
  
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, img->width(), img->height(), 0, img->format(), img->type(), 0 );  VL_CHECK_OGL(); */
  
  unsigned int x = 0;
  // for now, only ASCII characters
  for(int i = 32; i < 128; ++i)
  {
	if(FT_Load_Char(mFT_Face, i, freeTypeLoadForceAutoHint() ? FT_LOAD_FORCE_AUTOHINT : FT_LOAD_DEFAULT))
	  continue;
	
	error = FT_Render_Glyph(
            mFT_Face->glyph,  /* glyph slot */
            FT_RENDER_MODE_NORMAL ); /* render mode: FT_RENDER_MODE_MONO or FT_RENDER_MODE_NORMAL */

    if(g->bitmap.width != 0)
    {
        ref<Image> imgGlyph = new Image;
        //imgGlyph->allocate2D(g->bitmap.width + 2 * margin, g->bitmap.rows + 2 * margin, 1, IF_RED, IT_UNSIGNED_BYTE);
        imgGlyph->allocate2D(g->bitmap.width + 2 * margin, h, 1, IF_RED, IT_UNSIGNED_BYTE);	// Dans la texture carre, w == h (w correspond a la longueur de la texture globale)

        // init to all transparent white
        for(unsigned char *px = imgGlyph->pixels(), *end = px + imgGlyph->requiredMemory(); px<end; px+=1)
        {
          px[0] = 0x0;
        }

        // maps the glyph on the texture leaving a 1px margin

        int ind = 0;
        for(int yg=0; yg<g->bitmap.rows; yg++)
        {
          for(int xg=0; xg<g->bitmap.width; xg++)
          {
              //imgGlyph->pixels()[ind++] = g->bitmap.buffer[yg * g->bitmap.width + xg];
             
            int offset_1 = (xg+margin) * 1 + (h-1-yg-margin) * imgGlyph->pitch();
            int offset_2 = 0;
            if (g->bitmap.pixel_mode == FT_PIXEL_MODE_MONO)
              offset_2 = xg / 8 + yg * ::abs(g->bitmap.pitch);
            else
              offset_2 = xg + yg * g->bitmap.pitch;

    #if (1)
            if (g->bitmap.pixel_mode == FT_PIXEL_MODE_MONO)
              imgGlyph->pixels()[ offset_1/*+3*/ ] = (g->bitmap.buffer[ offset_2 ] >> (7-x%8)) & 0x1 ? 0xFF : 0x0;
            else
              imgGlyph->pixels()[ offset_1/*+3*/ ] = g->bitmap.buffer[ offset_2 ];
    #else
              // debug code
            img->pixels()[ offset_1+0 ] = g->bitmap.buffer[ offset_2 ]; // 0xFF;
            img->pixels()[ offset_1+1 ] = g->bitmap.buffer[ offset_2 ]; // 0xFF;
            img->pixels()[ offset_1+2 ] = g->bitmap.buffer[ offset_2 ]; // 0xFF;
            img->pixels()[ offset_1+3 ] = 0xFF; // face->glyph->bitmap.buffer[ offset_2 ];
    #endif
          }
        }
	
        img->copySubImage(imgGlyph.get(), RectI(0, 0, imgGlyph->width(), imgGlyph->height()), ivec2(x, 0));
    }
	//glTexSubImage2D(GL_TEXTURE_2D, 0, x, 0, g->bitmap.width, g->bitmap.rows, GL_RGBA, GL_UNSIGNED_BYTE, g->bitmap.buffer);
    //glTexSubImage2D(GL_TEXTURE_2D, 0, x, 0, imgGlyph->width(), imgGlyph->height(), GL_RED, GL_UNSIGNED_BYTE, g->bitmap.buffer/*imgGlyph->pixels()*/);

	
    CoreGlyph* glyph = new CoreGlyph;
    glyph->setFont(this);
    glyph->setAdvance(fvec2(g->advance.x >> 6, g->advance.y >> 6) );
    glyph->setWidth(g->bitmap.width);
    glyph->setHeight(g->bitmap.rows);
    glyph->setLeft(g->bitmap_left);
    glyph->setTop(g->bitmap_top);
	if(g->bitmap.width != 0)
	{
		glyph->setS0(x /(float)(w-1));
		glyph->setT0( 1 );
		//glyph->setS1(static_cast< float >(x) / static_cast< float >(w));
		glyph->setS1((margin*2 + x + glyph->width() ) /(float)(w-1));
		glyph->setT1( 1 -(margin*2 + glyph->height() ) /(float)(h-1) );
		//glyph->setT1((margin*2 + glyph->height() ) /(float)(h-1) );
		x += g->bitmap.width + 2 * margin;
	}
	else
	{
		glyph->setS0(x /(float)(w-1));
		glyph->setT0( 1 );
		glyph->setS1(x /(float)(w-1));
		glyph->setT1( 1 -(margin*2 + glyph->height() ) /(float)(h-1) );
		x += 1;
    }

    mGlyphMap[i] = glyph;
	
	}
	// tmps
	mAtlas = new Texture(img.get(), TF_RED, false);
  vl::TexParameter* parameters = mAtlas->getTexParameter();
  parameters->setMagFilter(vl::TPF_NEAREST);
  parameters->setMinFilter(vl::TPF_NEAREST);
  parameters->setWrapS(vl::TPW_CLAMP);
  parameters->setWrapT(vl::TPW_CLAMP);
  
  
  
}
