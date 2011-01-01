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

#include <vlCore/DrawPixels.hpp>
#include <vlCore/Actor.hpp>
#include <vlCore/Camera.hpp>
#include <map>

using namespace vl;

//-----------------------------------------------------------------------------
// DrawPixels::Pixels
//-----------------------------------------------------------------------------
DrawPixels::Pixels::Pixels()
{
  #ifndef NDEBUG
    mObjectName = className();
  #endif
  mAlign = AlignBottom | AlignLeft;
}
//-----------------------------------------------------------------------------
DrawPixels::Pixels::Pixels(Image* img, int scrx, int scry, int startx, int starty, int width, int height, int alignment)
{
  #ifndef NDEBUG
    mObjectName = className();
  #endif

  mAlign = alignment;

  if (width < 0)
    width = img->width()  - startx;

  if (height < 0)
    height = img->height() - starty;

  mImage     = img;
  mPosition  = ivec2(scrx, scry);
  mStart     = ivec2(startx, starty);
  mSize      = ivec2(width, height);
}
//-----------------------------------------------------------------------------
DrawPixels::Pixels::Pixels(const Pixels& other): Object(other)
{
  #ifndef NDEBUG
    mObjectName = className();
  #endif

  *this = other;
}
//-----------------------------------------------------------------------------
DrawPixels::Pixels& DrawPixels::Pixels::operator=(const Pixels& other)
{
  mImage = other.mImage;
  mPosition = other.mPosition;
  mStart = other.mStart;
  mSize  = other.mSize;
  mAlign = other.mAlign;
  return *this;
}
//-----------------------------------------------------------------------------
DrawPixels::Pixels::~Pixels()
{
}
//-----------------------------------------------------------------------------
void DrawPixels::Pixels::deletePixelBufferObject()
{
  image()->imageBuffer()->deleteGLBufferObject();
}
//-----------------------------------------------------------------------------
bool DrawPixels::Pixels::generatePixelBufferObject(EGLBufferUsage usage, bool discard_local_storage)
{
  VL_CHECK(image())
  if (!image())
    return false;
  image()->imageBuffer()->setBufferData(usage, discard_local_storage);
  return true;
}
//-----------------------------------------------------------------------------
bool DrawPixels::Pixels::hasPixelBufferObject() const
{
  return image()->imageBuffer()->handle() != 0;
}
//-----------------------------------------------------------------------------
// DrawPixels
//-----------------------------------------------------------------------------
DrawPixels::DrawPixels()
{
  mUsePixelBufferObject = false;
  mDraws.setAutomaticDelete(false);
}
//-----------------------------------------------------------------------------
void DrawPixels::render_Implementation(const Actor* actor, const Shader*, const Camera* camera, OpenGLContext*) const
{
  VL_CHECK_OGL()

  int viewport[] = {0,0,0,0};
  glGetIntegerv(GL_VIEWPORT, viewport);
  VL_CHECK_OGL()

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  VL_CHECK_OGL();

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho( -0.5, viewport[2]-0.5, -0.5, viewport[3]-0.5, -1, +1 ); VL_CHECK_OGL();

  glPushClientAttrib(GL_CLIENT_PIXEL_STORE_BIT); VL_CHECK_OGL();

  for(int i=0; i<(int)mDraws.size(); ++i)
  {
    const Pixels* cmd = draws()->at(i);

    if (cmd->image() == 0)
      continue;

    VL_CHECK( cmd->image() )
    VL_CHECK( cmd->image()->imageBuffer() )
    VL_CHECK( cmd->image()->imageBuffer()->handle() || cmd->image()->pixels() )
    VL_CHECK( cmd->image()->isValid() )
    VL_CHECK( cmd->mStart.x() >= 0 )
    VL_CHECK( cmd->mStart.y() >= 0 )

    //VL_CHECK( cmd->mStart.x()+cmd->mSize.x() -1 < cmd->image()->width() )
    //VL_CHECK( cmd->mStart.y()+cmd->mSize.y() -1 < cmd->image()->height() )

    int pos_x = cmd->mPosition.x();
    int pos_y = cmd->mPosition.y();

    // alignment

    if (cmd->align() & AlignRight)
      pos_x -= cmd->mSize.x() -1;
    if (cmd->align() & AlignHCenter)
      pos_x -= cmd->mSize.x() / 2;
    if (cmd->align() & AlignTop)
      pos_y -= cmd->mSize.y() -1;
    if (cmd->align() & AlignVCenter)
      pos_y -= cmd->mSize.y() / 2;

    // transform following

    if ( camera && actor && actor->transform() )
    {
      vec4 v(0,0,0,1);
      v = actor->transform()->worldMatrix() * v;

      camera->project(v,v);

      // from screen space to viewport space
      v.x() -= viewport[0];
      v.y() -= viewport[1];

      pos_x += int(v.x() + 0.5);
      pos_y += int(v.y() + 0.5);
    }

    // culling

    if ( pos_x + cmd->mSize.x() -1 < 0 )
      continue;

    if ( pos_y + cmd->mSize.y() -1 < 0 )
      continue;

    if (pos_x > viewport[2] - 1)
      continue;

    if (pos_y > viewport[3] - 1)
      continue;

    // clipping

    int clip_left   = pos_x < 0 ? -pos_x : 0;
    int clip_bottom = pos_y < 0 ? -pos_y : 0;
    int clip_right  = (pos_x+cmd->mSize.x()-1)-( viewport[2]-1 );
    int clip_top    = (pos_y+cmd->mSize.y()-1)-( viewport[3]-1 );

    if (clip_right < 0)
      clip_right = 0;

    if (clip_top < 0)
      clip_top = 0;

    glRasterPos2f( /*0.5f +*/ (float)pos_x + clip_left, /*0.5f +*/ (float)pos_y + clip_bottom );

    // clear the current color, texture, normal
    glColor4f(1.0f,1.0f,1.0f,1.0f);
    glNormal3f(0,0,1.0f);
    glTexCoord3f(0,0,0);
    VL_CHECK_OGL()

    glPixelStorei( GL_UNPACK_ALIGNMENT, cmd->image()->byteAlignment() );
    glPixelStorei( GL_UNPACK_ROW_LENGTH, cmd->image()->width() );
    glPixelStorei( GL_UNPACK_SKIP_PIXELS, cmd->mStart.x() + clip_left );
    glPixelStorei( GL_UNPACK_SKIP_ROWS, cmd->mStart.y() + clip_bottom );
    VL_CHECK_OGL()

    if ( cmd->image()->imageBuffer()->handle() )
    {
      VL_glBindBuffer( GL_PIXEL_UNPACK_BUFFER, cmd->image()->imageBuffer()->handle() ); VL_CHECK_OGL()
      glDrawPixels( cmd->mSize.x() -clip_left -clip_right, cmd->mSize.y() -clip_bottom -clip_top, cmd->image()->format(), cmd->image()->type(), 0 );
      VL_CHECK_OGL();
    }
    else
    {
      VL_glBindBuffer( GL_PIXEL_UNPACK_BUFFER, 0 );
      glDrawPixels( cmd->mSize.x() -clip_left -clip_right, cmd->mSize.y() -clip_bottom -clip_top, cmd->image()->format(), cmd->image()->type(), cmd->image()->pixels() );
      VL_CHECK_OGL();
    }
  }

  VL_CHECK_OGL();

  VL_glBindBuffer( GL_PIXEL_UNPACK_BUFFER, 0 );

  VL_CHECK_OGL()

  // restores the default values
  glPopClientAttrib();
  VL_CHECK_OGL();

  glMatrixMode(GL_MODELVIEW);
  glPopMatrix(); VL_CHECK_OGL()

  glMatrixMode(GL_PROJECTION);
  glPopMatrix(); VL_CHECK_OGL()
}
//-----------------------------------------------------------------------------
//! deallocate PBOs
void DrawPixels::deletePixelBufferObjects()
{
  VL_CHECK_OGL()
  for(int i=0; i<(int)mDraws.size(); ++i)
  {
    mDraws[i]->image()->imageBuffer()->deleteGLBufferObject();
  }
  VL_CHECK_OGL()
}
//-----------------------------------------------------------------------------
void DrawPixels::releaseImages()
{
  for(int i=0; i<(int)mDraws.size(); ++i)
    mDraws[i]->mImage = NULL;
}
//-----------------------------------------------------------------------------
//! generates PBOs only for Pixels objects without a PBO handle
bool DrawPixels::generatePixelBufferObjects(EGLBufferUsage usage, bool discard_local_storage)
{
  if ( !( GLEW_ARB_pixel_buffer_object||GLEW_EXT_pixel_buffer_object ) )
    return false;

  // generates PBOs if they have an attached Image

  // avoids to PBO duplicates for the same images
  std::map< ref<Image>, unsigned int> pbomap;

  for(int i=0; i<(int)mDraws.size(); ++i)
  {
    if ( mDraws[i]->image()->imageBuffer()->handle() )
      continue;

    if ( mDraws[i]->mImage.get() == NULL )
      continue;

    mDraws[i]->generatePixelBufferObject(usage, discard_local_storage);
  }
  return true;
}
//-----------------------------------------------------------------------------
void DrawPixels::setUsePixelBufferObject(bool use_pbo)
{
  if ( (GLEW_ARB_pixel_buffer_object||GLEW_EXT_pixel_buffer_object) )
    mUsePixelBufferObject = use_pbo;
  else
    mUsePixelBufferObject = false;
}
//-----------------------------------------------------------------------------
