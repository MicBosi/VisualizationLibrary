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

#include <vlGraphics/CoreText.hpp>
#include <vlGraphics/OpenGLContext.hpp>
#include <vlGraphics/Actor.hpp>
#include <vlGraphics/GLSL.hpp>
#include <vlCore/Log.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

using namespace vl;

// mic fixme: implement me.

// Goals:
// - Run on OpenGL Core/ES.
// - Achieve a much greater rendering speed.
// - In order to do this we might sacrifice some of the usability of vl::Text

// Guidelines:
// - Viewport alignment, text transformation, transform tracking done externally.
// - Does not manipulate the GL_PROJECTION and GL_MODELVIEW matrices.
// - Text geometry and bounding boxes should be pre-computed on text change.
// - Line splitting should not be done at rendering time.
// - Should use only OpenGL Core routines.
// - Should use texture atlases and perform lazy texture binding.
// - Cleaner left to right / right to left text reversing.
// - Outline rendering should use 2 pass with enlarged glyphs instead of 5 passes or precompute an high quality outline texture.
// - Avoid using doubles and floats if possible, use integer and Rect rather floats and AABBs.

CoreText::~CoreText() 
{
	glDeleteBuffers(1, &mBufferID);	// if the buffer wasn't create, the id is 0 and will be ignored by the function
}

void CoreText::setText(const String& text)
{ 
  mText = text; 
	
  // mic fixme:
  // - lines split and lines dimensions (linebox) should be precomputed on text set!
  // - or convert this function to generate a pre-computed rendering-list!
  //  |  
  //  v
  // is now

  // split the text in different lines

  VL_CHECK(mText.length())

  mLines.push_back( String() );
  for(int i=0; i<mText.length(); ++i)
  {
    if (mText[i] == '\n')
    {
      /*if(!mLines.empty())
	  {
		AABB aabb = computeRawBoundingRect(mLines.back());
		mDimensionsLines.push_back(aabb);
		// compute total bounding rect
		mRawBoundingRect += aabb;
      }*/
      // start new line
      mLines.push_back( String() );
    }
    else
      mLines.back() += mText[i];
  }

  for(size_t ilines = 0; ilines < mLines.size(); ++ilines)
  {
      if(!mLines.at(ilines).empty())
      {
          AABB aabb = computeRawBoundingRect(mLines.at(ilines));
          mDimensionsLines.push_back(aabb);
          // compute total bounding rect
          mRawBoundingRect += aabb;
      }
  }
  
  //vl::ref< Texture > texture = textTexture(mText);
  
  // we create the buffer
  glGenBuffers(1, &mBufferID);

  glBindBuffer(GL_ARRAY_BUFFER, mBufferID);

  // with the number of lines and the number of character per lines, we can compute the size of the buffer
  glBufferData(GL_ARRAY_BUFFER, (mText.length() * (4 * 3 + 4 * 2) + 4 * 3) * sizeof(GL_FLOAT), 0, GL_STATIC_DRAW);	// x,y,z,s,t per char ( with 4 vertex ) and 4 vertex for the background and the border
  
  // Fill the buffer
  
  //for(int c=0; c<mText.length(); ++c)
  //{
	  // with the text
	  
	  // with the background and the border
	  vec3 a,b,c,d;
	  AABB bbox = boundingRect(); // mic fixme: this guy recomputes the bounds again instead of using the precomputed one!!
	  a = bbox.minCorner();
	  b.x() = (float)bbox.maxCorner().x();
	  b.y() = (float)bbox.minCorner().y();
	  c = bbox.maxCorner();
	  d.x() = (float)bbox.minCorner().x();
	  d.y() = (float)bbox.maxCorner().y();
	  // set z to 0
	  a.z() = b.z() = c.z() = d.z() = 0;
	  
	  float data[mText.length() * (4 * 3 + 4 * 2) + 4 * 3];
	  unsigned int indexData = 0;
	  
	  FT_Long has_kerning = FT_HAS_KERNING( font()->mFT_Face );
  FT_UInt previous = 0;

 /* int w = camera->viewport()->width();
  int h = camera->viewport()->height();

  if (w < 1) w = 1;
  if (h < 1) h = 1;*/

  fvec2 pen(0,0);
  
  // split the text in different lines

  VL_CHECK(text.length())

  std::vector< String > lines;
  lines.push_back( String() );
  for(int i=0; i<text.length(); ++i)
  {
    if (text[i] == '\n')
    {
      // start new line
      lines.push_back( String() );
    }
    else
      lines.back() += text[i];
  }
	int applied_margin = backgroundEnabled() || borderEnabled() ? margin() : 0;
	
  for(unsigned iline=0; iline<lines.size(); iline++)
  {
    // strip spaces at the beginning and at the end of the line
    if (textAlignment() == TextAlignJustify)
      lines[iline].trim();

    //AABB linebox = rawboundingRect( lines[iline] );
	AABB linebox = mDimensionsLines[iline];
    int displace = 0;
    int just_space = 0;
    int just_remained_space = 0;
    int space_count = 0;
    for(int c=0; c<(int)lines[iline].length(); c++)
      if ( lines[iline][c] == ' ' )
        space_count++;

    if (space_count && textAlignment() == TextAlignJustify)
    {
      /*just_space          = int(rbbox.width() - linebox.width()) / space_count;
      just_remained_space = int(rbbox.width() - linebox.width()) % space_count;*/
	  just_space          = int(mRawBoundingRect.width() - linebox.width()) / space_count;
      just_remained_space = int(mRawBoundingRect.width() - linebox.width()) % space_count;
    }

    if (layout() == RightToLeftText)
    {
      if (textAlignment() == TextAlignRight)
        displace = 0;
      else
      if (textAlignment() == TextAlignLeft)
		displace = - int(mRawBoundingRect.width() - linebox.width());
        //displace = - int(rbbox.width() - linebox.width());
      else
      if (textAlignment() == TextAlignCenter)
		displace = - int((mRawBoundingRect.width() - linebox.width()) / 2.0f);
        //displace = - int((rbbox.width() - linebox.width()) / 2.0f);
    }
    if (layout() == LeftToRightText)
    {
      if (textAlignment() == TextAlignRight)
		displace = int(mRawBoundingRect.width() - linebox.width());
        //displace = int(rbbox.width() - linebox.width());
      else
      if (textAlignment() == TextAlignLeft)
        displace = 0;
      else
      if (textAlignment() == TextAlignCenter)
		displace = + int((mRawBoundingRect.width() - linebox.width()) / 2.0f);
        //displace = + int((rbbox.width() - linebox.width()) / 2.0f);
    }

    // this is needed so that empty strings generate empty lines
    // note that puttig '\n\n\n\n' at the beginning of a text generates
    // a wrong rendering (see it with background box activated).
    if (iline != 0 && !lines[iline].length())
    {
      pen.y() -= mFont->mHeight;
      pen.x()  = 0;
    }
    else
    for(int c=0; c<(int)lines[iline].length(); c++)
    {
      if (c == 0 && iline != 0)
      {
        pen.y() -= mFont->mHeight;
        pen.x()  = 0;
      }

      const CoreGlyph* glyph = mFont->glyph( lines[iline][c] );

      if (!glyph)
        continue;

      if ( kerningEnabled() && has_kerning && previous && glyph->glyphIndex() )
      {
        FT_Vector delta; delta.y = 0;
        if (layout() == LeftToRightText)
        {
          FT_Get_Kerning( font()->mFT_Face, previous, glyph->glyphIndex(), FT_KERNING_DEFAULT, &delta );
          pen.x() += delta.x / 64.0f;
        }
        else
        if (layout() == RightToLeftText)
        {
          FT_Get_Kerning( font()->mFT_Face, glyph->glyphIndex(), previous, FT_KERNING_DEFAULT, &delta );
          pen.x() -= delta.x / 64.0f;
        }
        pen.y() += delta.y / 64.0f;
      }
      previous = glyph->glyphIndex();

      //if (glyph->textureHandle())
      {
        //glBindTexture( GL_TEXTURE_2D, glyph->textureHandle() );

        /*texc[0] = glyph->s0();
        texc[1] = glyph->t1();
        
        texc[2] = glyph->s1();
        texc[3] = glyph->t1();

        texc[4] = glyph->s1();
        texc[5] = glyph->t0();
        
        texc[6] = glyph->s0();
        texc[7] = glyph->t0();

        int left = layout() == RightToLeftText ? -glyph->left() : +glyph->left();

        // triangle strip layout

        vect[0].x() = pen.x() + glyph->width()*0 + left -1;
        vect[0].y() = pen.y() + glyph->height()*0 + glyph->top() - glyph->height() -1;

        vect[1].x() = pen.x() + glyph->width()*1 + left +1;
        vect[1].y() = pen.y() + glyph->height()*0 + glyph->top() - glyph->height() -1;

        vect[2].x() = pen.x() + glyph->width()*1 + left +1;
        vect[2].y() = pen.y() + glyph->height()*1 + glyph->top() - glyph->height() +1;

        vect[3].x() = pen.x() + glyph->width()*0 + left -1;
        vect[3].y() = pen.y() + glyph->height()*1 + glyph->top() - glyph->height() +1;*/
		
		vec3 vect[4];
		vec2 texcoord[4];
		
		// coord
		/*data[indexData++] = pen.x() + glyph->width()*0 + left -1;
        data[indexData++] = pen.y() + glyph->height()*0 + glyph->top() - glyph->height() -1;
		data[indexData++] = 0;
		// texcoord
		data[indexData++] = glyph->s0;
		data[indexData++] = glyph->t1;

        data[indexData++] = pen.x() + glyph->width()*1 + left +1;
        data[indexData++] = pen.y() + glyph->height()*0 + glyph->top() - glyph->height() -1;
		data[indexData++] = 0;
		// texcoord
		data[indexData++] = glyph->s1;
		data[indexData++] = glyph->t1;

        data[indexData++] = pen.x() + glyph->width()*1 + left +1;
        data[indexData++] = pen.y() + glyph->height()*1 + glyph->top() - glyph->height() +1;
		data[indexData++] = 0;
		// texcoord
		data[indexData++] = glyph->s1;
		data[indexData++] = glyph->t0;

        data[indexData++] = pen.x() + glyph->width()*0 + left -1;
        data[indexData++] = pen.y() + glyph->height()*1 + glyph->top() - glyph->height() +1;		
		data[indexData++] = 0;
		// texcoord
		data[indexData++] = glyph->s0;
		data[indexData++] = glyph->t0;*/
		
		int left = layout() == RightToLeftText ? -glyph->left() : +glyph->left();
		
		vect[0].x() = pen.x() + glyph->width()*0 + left -1;
        vect[0].y() = pen.y() + glyph->height()*0 + glyph->top() - glyph->height() -1;
		vect[0].z() = 0;
		// texcoord
		texcoord[0].x() = glyph->s0();
		texcoord[0].y() = glyph->t1();

        vect[1].x() = pen.x() + glyph->width()*1 + left +1;
        vect[1].y() = pen.y() + glyph->height()*0 + glyph->top() - glyph->height() -1;
		vect[1].z() = 0;
		// texcoord
		texcoord[1].x() = glyph->s1();
		texcoord[1].y() = glyph->t1();

        vect[2].x() = pen.x() + glyph->width()*1 + left +1;
        vect[2].y() = pen.y() + glyph->height()*1 + glyph->top() - glyph->height() +1;
		vect[2].z() = 0;
		// texcoord
		texcoord[2].x() = glyph->s1();
		texcoord[2].y() = glyph->t0();

        vect[3].x() = pen.x() + glyph->width()*0 + left -1;
        vect[3].y() = pen.y() + glyph->height()*1 + glyph->top() - glyph->height() +1;		
		vect[3].z() = 0;
		// texcoord
		texcoord[3].x() = glyph->s0();
		texcoord[3].y() = glyph->t0();
		

        if (layout() == RightToLeftText)
        {
  #if (1)
          vect[0].x() -= glyph->width()-1 +2;
          vect[1].x() -= glyph->width()-1 +2;
          vect[2].x() -= glyph->width()-1 +2;
          vect[3].x() -= glyph->width()-1 +2;
  #endif
        }

        /*vect[0].y() -= mFont->mHeight;
        vect[1].y() -= mFont->mHeight;
        vect[2].y() -= mFont->mHeight;
        vect[3].y() -= mFont->mHeight;*/

  #if (1)
        // normalize coordinate orgin to the bottom/left corner
        vect[0] -= (fvec3)bbox.minCorner();
        vect[1] -= (fvec3)bbox.minCorner();
        vect[2] -= (fvec3)bbox.minCorner();
        vect[3] -= (fvec3)bbox.minCorner();
  #endif

  #if (1)
        vect[0].x() += applied_margin + displace;
        vect[1].x() += applied_margin + displace;
        vect[2].x() += applied_margin + displace;
        vect[3].x() += applied_margin + displace;

        vect[0].y() += applied_margin;
        vect[1].y() += applied_margin;
        vect[2].y() += applied_margin;
        vect[3].y() += applied_margin;
  #endif

        // apply offset for outline rendering
     /*   vect[0].x() += offset.x();
        vect[0].y() += offset.y();
        vect[1].x() += offset.x();
        vect[1].y() += offset.y();
        vect[2].x() += offset.x();
        vect[2].y() += offset.y();
        vect[3].x() += offset.x();
        vect[3].y() += offset.y();*/

        // alignment
        /*for(int i=0; i<4; ++i)
        {
          if (alignment() & AlignHCenter)
          {
            VL_CHECK( !(alignment() & AlignRight) )
            VL_CHECK( !(alignment() & AlignLeft) )
            vect[i].x() -= (int)(bbox.width() / 2.0f);
          }

          if (alignment() & AlignRight)
          {
            VL_CHECK( !(alignment() & AlignHCenter) )
            VL_CHECK( !(alignment() & AlignLeft) )
            vect[i].x() -= (int)bbox.width();
          }

          if (alignment() & AlignTop)
          {
            VL_CHECK( !(alignment() & AlignBottom) )
            VL_CHECK( !(alignment() & AlignVCenter) )
            vect[i].y() -= (int)bbox.height();
          }

          if (alignment() & AlignVCenter)
          {
            VL_CHECK( !(alignment() & AlignTop) )
            VL_CHECK( !(alignment() & AlignBottom) )
            vect[i].y() -= int(bbox.height() / 2.0);
          }
        }*/

		for(int i = 0; i < 4; ++i)
		{
			data[indexData++] = vect[i].x();
			data[indexData++] = vect[i].y();
			data[indexData++] = vect[i].z();
			data[indexData++] = texcoord[i].x();
			data[indexData++] = texcoord[i].y();
		}
        
      }

      if (just_space && lines[iline][c] == ' ' && iline != lines.size()-1)
      {
        if (layout() == LeftToRightText)
        {
          pen.x() += just_space + (just_remained_space?1:0);
          // pen.y() += glyph->advance().y();
        }
        else
        if (layout() == RightToLeftText)
        {
          pen.x() -= just_space + (just_remained_space?1:0);
          // pen.y() -= glyph->advance().y();
        }
        if(just_remained_space)
          just_remained_space--;
      }

      if (layout() == LeftToRightText)
      {
        pen.x() += glyph->advance().x();
        // pen.y() += glyph->advance().y();
      }
      else
      if (layout() == RightToLeftText)
      {
        pen.x() -= glyph->advance().x();
        // pen.y() -= glyph->advance().y();
      }

    }
  }
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  

	  /*fvec3 vect[] = { (fvec3)a, (fvec3)b, (fvec3)c, (fvec3)d };
	  
	  //glBufferSubData(GL_ARRAY_BUFFER, 0, mText.length() * 4 * 3 * sizeof(GL_FLOAT), &vect[0]);

	  //float texc[mText.length() * 4 * 2];
	  float texc[] = {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f};
	  //getCoordinates(texc);

	  //glBufferSubData(GL_ARRAY_BUFFER, (mText.length() * 4 * 3) * sizeof(GL_FLOAT), 4 * 2 * sizeof(GL_FLOAT), texc);

	  float data[20];
	  int index = 0;
	  for(int i = 0; i < 4; ++i)
	  {
		  data[index++] = vect[i].x();
		  data[index++] = vect[i].y();
		  data[index++] = vect[i].z();
		  data[index++] = texc[i * 2];
		  data[index++] = texc[i * 2 + 1];
	  }

	  glBufferSubData(GL_ARRAY_BUFFER, 0, (mText.length() * 4 * 5) * sizeof(GL_FLOAT), data);*/
    glBufferSubData(GL_ARRAY_BUFFER, 0, (mText.length() * (4 * 3 + 4 * 2)) * sizeof(GL_FLOAT), data);
  //}

  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

//-----------------------------------------------------------------------------
void CoreText::render_Implementation(const Actor* actor, const Shader* shader, const Camera* camera, OpenGLContext* gl_context) const
{
  // bind Vertex Attrib Set

  //bool vbo_on = Has_BufferObject && isBufferObjectEnabled() && !isDisplayListEnabled();

  gl_context->bindVAS(NULL, false, false);

  VL_CHECK(font())

  if (!font() || !font()->mFT_Face)
    return;

  if ( text().empty() )
    return;

  // Lighting can be enabled or disabled.
  // glDisable(GL_LIGHTING);

  // Blending must be enabled explicity by the vl::Shader, also to perform z-sort.
  // glEnable(GL_BLEND);

  // Trucchetto che usiamo per evitare z-fighting:
  // Pass #1 - fill color and stencil
  // - disable depth write mask
  // - depth test can be enabled or not by the user
  // - depth func can be choosen by the user
  // - render in the order: background, border, shadow, outline, text
  // Pass #2 - fill z-buffer
  // - enable depth write mask
  // - disable color mask
  // - disable stencil
  // - drawing background and border

  glBindBuffer(GL_ARRAY_BUFFER, mBufferID);
  
  // Get or Create the uniform for the sampler 2D.
  /*if(shader && shader->glslProgram() && glyph->textureHandle())
	shader->glslProgram()->gocUniform("TextSampler")->setUniform(glyph()->textureHandle());*/
  
  // Pass #1

  // disable z-writing
  GLboolean depth_mask=0;
  glGetBooleanv(GL_DEPTH_WRITEMASK, &depth_mask);
  glDepthMask(GL_FALSE);

  // background
  if (backgroundEnabled())
    renderBackground( actor, camera );

  // border
  if (borderEnabled())
    renderBorder( actor, camera );

  // to have the most correct results we should render the text twice one for color and stencil, the other for the z-buffer

  // shadow render
  if (shadowEnabled())
    renderText( actor, camera, shadowColor(), shadowVector() );
  // outline render
  if (outlineEnabled())
  {
    renderText( actor, camera, outlineColor(), fvec2(-1,0) );
    renderText( actor, camera, outlineColor(), fvec2(+1,0) );
    renderText( actor, camera, outlineColor(), fvec2(0,-1) );
    renderText( actor, camera, outlineColor(), fvec2(0,+1) );
  }
  // text render
  renderText( actor, camera, color(), fvec2(0,0) );

  // Pass #2
  // fills the z-buffer (not the stencil buffer): approximated to the text bbox

  // restores depth mask
  glDepthMask(depth_mask);

  if (depth_mask)
  {
    // disables writing to the color buffer
    GLboolean color_mask[4];
    glGetBooleanv(GL_COLOR_WRITEMASK, color_mask);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

    // disable writing to the stencil buffer
    int stencil_front_mask=0;
    glGetIntegerv(GL_STENCIL_WRITEMASK, &stencil_front_mask);
    int stencil_back_mask=0;
    if (Has_GL_Version_2_0)
      glGetIntegerv(GL_STENCIL_BACK_WRITEMASK, &stencil_back_mask);
    glStencilMask(0);

    // background
    renderBackground( actor, camera );

    // border
    renderBorder( actor, camera );

    // restores color writing
    glColorMask(color_mask[0],color_mask[1],color_mask[2],color_mask[3]);

    // restore the stencil masks
    glStencilMask(stencil_front_mask);
    if (Has_GL_Version_2_0)
      glStencilMaskSeparate(GL_BACK, stencil_back_mask);

    // Unbind buffer
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }
}
//-----------------------------------------------------------------------------
void CoreText::renderText(const Actor*, const Camera*, const fvec4& color, const fvec2& offset) const
{
  if(!mFont)
  {
    Log::error("CoreText::renderText() error: no Font assigned to the CoreText object.\n");
    VL_TRAP()
    return;
  }

  if (!font()->mFT_Face)
  {
    Log::error("CoreText::renderText() error: invalid FT_Face: probably you tried to load an unsupported font format.\n");
    VL_TRAP()
    return;
  }

  VL_glActiveTexture( GL_TEXTURE0 );
  glEnable(GL_TEXTURE_2D);

  for(int c=0; c<mText.length(); ++c)
  {
      //const ref<Glyph>& glyph = mFont->glyph(mText[c]);
      //if(glyph->textureHandle())
      {
          //glBindTexture(GL_TEXTURE_2D, glyph->textureHandle());
		  //glBindTexture(GL_TEXTURE_2D, mTextTexture->handle());
		  //VL_glUniform1uiv(
		  
		  glEnableVertexAttribArray(vl::VA_TexCoord0);
          glVertexAttribPointer(vl::VA_TexCoord0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), reinterpret_cast< GLvoid* >(c * 20 * sizeof(GL_FLOAT) + 3 * sizeof(GL_FLOAT)));

		  glEnableVertexAttribArray(vl::VA_Position);
          glVertexAttribPointer(vl::VA_Position, 3, GL_FLOAT, GL_TRUE, 5 * sizeof(GL_FLOAT), reinterpret_cast< GLvoid* >(c * 20 * sizeof(GL_FLOAT)));

		  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		  
      }
  }
	
  glBindTexture(GL_TEXTURE_2D, 0);
  glDisable(GL_TEXTURE_2D);
  
  glDisableVertexAttribArray(vl::VA_Position);
  glDisableVertexAttribArray(vl::VA_TexCoord0);

  VL_CHECK_OGL();

  /*glDisable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);*/
}
//-----------------------------------------------------------------------------
// returns the raw bounding box of the string, i.e. without alignment, margin and matrix transform.
AABB CoreText::computeRawBoundingRect(const String& text) const
{
  AABB aabb;

  if(!font())
  {
    Log::error("CoreText::rawboundingRect() error: no Font assigned to the CoreText object.\n");
    VL_TRAP()
    return aabb;
  }

  if (!font()->mFT_Face)
  {
    Log::error("CoreText::rawboundingRect() error: invalid FT_Face: probably you tried to load an unsupported font format.\n");
    VL_TRAP()
    return aabb;
  }

  fvec2 pen(0,0);
  fvec3 vect[4];

  FT_Long use_kerning = FT_HAS_KERNING( font()->mFT_Face );
  FT_UInt previous = 0;

  for(int c=0; c<(int)text.length(); c++)
  {
    if (text[c] == '\n')
    {
      pen.y() -= mFont->mHeight ? mFont->mHeight : mFont->mSize;
      pen.x()  = 0;
      continue;
    }

    const ref<CoreGlyph>& glyph = mFont->glyph(text[c]);

    // if glyph == NULL there was an error during its creation...
    if (glyph.get() == NULL)
      continue;

    if ( kerningEnabled() && use_kerning && previous && glyph->glyphIndex())
    {
      FT_Vector delta; delta.y = 0;
      if (layout() == LeftToRightText)
      {
        FT_Get_Kerning( font()->mFT_Face, previous, glyph->glyphIndex(), FT_KERNING_DEFAULT, &delta );
        pen.x() += delta.x / 64.0f;
      }
      else
      if (layout() == RightToLeftText)
      {
        FT_Get_Kerning( font()->mFT_Face, glyph->glyphIndex(), previous, FT_KERNING_DEFAULT, &delta );
        pen.x() -= delta.x / 64.0f;
      }
      pen.y() += delta.y / 64.0f;
    }
    previous = glyph->glyphIndex();

    //if ( glyph->textureHandle() )
    {

      int left = layout() == RightToLeftText ? -glyph->left() : +glyph->left();

      vect[0].x() = pen.x() + glyph->width()*0 + left -1;
      vect[0].y() = pen.y() + glyph->height()*0 + glyph->top() - glyph->height() -1;

      vect[1].x() = pen.x() + glyph->width()*1 + left +1;
      vect[1].y() = pen.y() + glyph->height()*0 + glyph->top() - glyph->height() -1;

      vect[2].x() = pen.x() + glyph->width()*1 + left +1;
      vect[2].y() = pen.y() + glyph->height()*1 + glyph->top() - glyph->height() +1;

      vect[3].x() = pen.x() + glyph->width()*0 + left -1;
      vect[3].y() = pen.y() + glyph->height()*1 + glyph->top() - glyph->height() +1;

      if (layout() == RightToLeftText)
      {
        vect[0].x() -= glyph->width()-1 +2;
        vect[1].x() -= glyph->width()-1 +2;
        vect[2].x() -= glyph->width()-1 +2;
        vect[3].x() -= glyph->width()-1 +2;
      }

      vect[0].y() -= mFont->mHeight;
      vect[1].y() -= mFont->mHeight;
      vect[2].y() -= mFont->mHeight;
      vect[3].y() -= mFont->mHeight;
    }

    aabb.addPoint( (vec3)vect[0] );
    aabb.addPoint( (vec3)vect[1] );
    aabb.addPoint( (vec3)vect[2] );
    aabb.addPoint( (vec3)vect[3] );

    if (layout() == LeftToRightText)
      pen += glyph->advance();
    else
    if (layout() == RightToLeftText)
      pen -= glyph->advance();
  }

  return aabb;
}
//-----------------------------------------------------------------------------
void CoreText::renderBackground(const Actor*, const Camera*) const
{
  // mic fixme:
  // rendering of border and background follows different rules in 3D compared from 2D: lines and polygons follow different rasterization rules!

  // Background color
  //glColor4fv(mBackgroundColor.ptr());

  // Constant normal
  //glNormal3fv( fvec3(0,0,1).ptr() );

  /*vec3 a,b,c,d;
  AABB bbox = boundingRect(); // mic fixme: this guy recomputes the bounds again instead of using the precomputed one!!
  a = bbox.minCorner();
  b.x() = (float)bbox.maxCorner().x();
  b.y() = (float)bbox.minCorner().y();
  c = bbox.maxCorner();
  d.x() = (float)bbox.minCorner().x();
  d.y() = (float)bbox.maxCorner().y();
  // set z to 0
  a.z() = b.z() = c.z() = d.z() = 0;

  fvec3 vect[] = { (fvec3)a, (fvec3)b, (fvec3)c, (fvec3)d };*/
  
  glEnableVertexAttribArray(vl::VA_Position);
  //glVertexAttribPointer(vl::VA_Position, 3, GL_FLOAT, GL_FALSE, 0, vect);

  glDrawArrays(GL_QUADS,mText.length() * 2,4);

  //glDisableClientState( GL_VERTEX_ARRAY );
  glDisableVertexAttribArray(vl::VA_Position);
}
//-----------------------------------------------------------------------------
void CoreText::renderBorder(const Actor*, const Camera*) const
{
  // mic fixme:
  // rendering of border and background follows different rules in 3D compared from 2D: lines and polygons follow different rasterization rules!

  // Border color
  //glColor4fv(mBorderColor.ptr());

  // Constant normal
  //glNormal3fv( fvec3(0,0,1).ptr() );

  /*vec3 a,b,c,d;
  AABB bbox = boundingRect(); // mic fixme: this guy recomputes the bounds again instead of using the precomputed one!!
  a = bbox.minCorner();
  b.x() = (float)bbox.maxCorner().x();
  b.y() = (float)bbox.minCorner().y();
  c = bbox.maxCorner();
  d.x() = (float)bbox.minCorner().x();
  d.y() = (float)bbox.maxCorner().y();
  // set z to 0
  a.z() = b.z() = c.z() = d.z() = 0;

  fvec3 vect[] = { (fvec3)a, (fvec3)b, (fvec3)c, (fvec3)d };*/
  glEnableVertexAttribArray(vl::VA_Position);
  //glVertexAttribPointer(vl::VA_Position, 3, GL_FLOAT, GL_FALSE, 0, vect);

  glDrawArrays(GL_LINE_LOOP,mText.length() * 2,4);

  //glDisableClientState( GL_VERTEX_ARRAY );
  glDisableVertexAttribArray(vl::VA_Position);
}
//-----------------------------------------------------------------------------
/*AABB CoreText::boundingRect() const
{
  return boundingRect(text());
}*/
//-----------------------------------------------------------------------------
AABB CoreText::boundingRect(/*const String& text*/) const
{
  int applied_margin = backgroundEnabled() || borderEnabled() ? margin() : 0;
  AABB bbox = mRawBoundingRect;	// Compute when text was setted or kerning was changed	//rawboundingRect( text );
  bbox.setMaxCorner( bbox.maxCorner() + vec3(2.0f*applied_margin, 2.0f*applied_margin, 0) );

  // normalize coordinate orgin to the bottom/left corner
  vec3 min = bbox.minCorner() - bbox.minCorner();
  vec3 max = bbox.maxCorner() - bbox.minCorner();

  // text pivot

  if (textOrigin() & AlignHCenter)
  {
    VL_CHECK( !(textOrigin() & AlignRight) )
    VL_CHECK( !(textOrigin() & AlignLeft) )
    min.x() -= int(bbox.width() / 2.0);
    max.x() -= int(bbox.width() / 2.0);
  }

  if (textOrigin() & AlignRight)
  {
    VL_CHECK( !(textOrigin() & AlignHCenter) )
    VL_CHECK( !(textOrigin() & AlignLeft) )
    min.x() -= (int)bbox.width();
    max.x() -= (int)bbox.width();
  }

  if (textOrigin() & AlignTop)
  {
    VL_CHECK( !(textOrigin() & AlignBottom) )
    VL_CHECK( !(textOrigin() & AlignVCenter) )
    min.y() -= (int)bbox.height();
    max.y() -= (int)bbox.height();
  }

  if (textOrigin() & AlignVCenter)
  {
    VL_CHECK( !(textOrigin() & AlignTop) )
    VL_CHECK( !(textOrigin() & AlignBottom) )
    min.y() -= int(bbox.height() / 2.0);
    max.y() -= int(bbox.height() / 2.0);
  }

  AABB aabb;
  aabb.setMinCorner(min);
  aabb.setMaxCorner(max);
  return aabb;
}//-----------------------------------------------------------------------------
int CoreText::getSampler() const
{
	// Artificial because they're probably more than one character in the text
	VL_CHECK(mText.length());
    VL_CHECK(mFont.get() != NULL);

    /*for(int c=0; c<mText.length(); ++c)
    {
        const ref<Glyph>& glyph = mFont->glyph(mText[c]);
        return glyph->textureHandle();
	}*/
	
	return -1;
}
//-----------------------------------------------------------------------------
vl::Image* CoreText::getImage()
{
    // Artificial because they're probably more than one character in the text
    VL_CHECK(mText.length());
    VL_CHECK(mFont.get() != NULL);

    /*for(int c=0; c<mText.length(); ++c)
    {
        const ref<CoreGlyph>& glyph = mFont->glyph(mText[c]);
        return const_cast< vl::Image* >(glyph->mGlyphImage.get());
    }*/
	
	return NULL;
}
//-----------------------------------------------------------------------------
Image* CoreText::textImage(const String &text)
{
	if(mTextImage)
		return mTextImage.get();

	// Get all the glyph of the text
    unsigned int width = 0;
    unsigned int maxHeight = 0;
    const unsigned int margin = 1;  // never change
	/*for(int c=0; c<(int)text.length(); c++)
	{
		const ref<Glyph>& glyph = mFont->glyph(text[c]);

        if(glyph->mGlyphImage.get() != NULL)
        {
            width += glyph->width();
        }
        else
        {
            width += glyph->advance().x();
        }

        if(glyph->height() > maxHeight)
            maxHeight = glyph->height();
	}

	// Generate the texture of all the text
	mTextImage = new Image();	
    mTextImage->allocate2D(width + 2 * margin, maxHeight + 2 * margin, 1, IF_RGBA, IT_UNSIGNED_BYTE);

    // Initialize the image
    std::vector<GLubyte> emptyData((width + 2 * margin) * (maxHeight + 2 * margin) * 4, 0);
    std::memcpy(mTextImage->imageBuffer()->ptr(), &emptyData[0], mTextImage->imageBuffer()->bytesUsed());
	
    fvec2 pen(0, 0);*/

	/*for(int c=0; c<(int)text.length(); c++)
    {
		ref<CoreGlyph> glyph = mFont->glyph(text[c]);

        if(glyph->mGlyphImage.get() != NULL)
        {
          mTextImage->copySubImage(glyph->mGlyphImage.get(), RectI(0, glyph->mGlyphImage->height() - (margin * 2 + glyph->height()), margin*2 + glyph->width(), margin * 2 + glyph->height()), ivec2(pen.x(), pen.y()));
          //pen += fvec2(glyph->width(), 0.0f);
        }
       /* else
        {*/
      /*      pen += glyph->advance()/* + fvec2(1.0f, 0.0f)*//*;
        //}


    }*/
	
	return mTextImage.get();
}
//-----------------------------------------------------------------------------
Texture* CoreText::textTexture(const String &text)
{
	if(mTextTexture)
		return mTextTexture.get();

	ref< Image > image = textImage(text);	
	
    mTextTexture = new Texture(image.get(), TF_RGBA, false);
	vl::TexParameter* parameters = mTextTexture->getTexParameter();
    parameters->setMagFilter(vl::TPF_NEAREST);
    parameters->setMinFilter(vl::TPF_NEAREST);
	parameters->setWrapS(vl::TPW_CLAMP);
	parameters->setWrapT(vl::TPW_CLAMP);
		
	return mTextTexture.get();
}
//-----------------------------------------------------------------------------
void CoreText::getCoordinates(float* texc)
{
    VL_CHECK(mText.length());
    VL_CHECK(mFont.get() != NULL);

	int index = 0;
    for(int c=0; c<mText.length(); ++c)
    {
        const ref<CoreGlyph>& glyph = mFont->glyph(mText[c]);
        /*if(glyph->textureHandle())
        {
            texc[index++] = glyph->s0();
            texc[index++] = glyph->t1();
            texc[index++] = glyph->s1();
            texc[index++] = glyph->t1();
            texc[index++] = glyph->s1();
            texc[index++] = glyph->t0();
            texc[index++] = glyph->s0();
            texc[index++] = glyph->t0();
        }*/
    }
}
