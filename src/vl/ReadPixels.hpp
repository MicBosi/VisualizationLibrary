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

#ifndef CameraReadPixels_INCLUDE_ONCE
#define CameraReadPixels_INCLUDE_ONCE

#include <vl/Camera.hpp>
#include <vl/Say.hpp>
#include <vl/Log.hpp>
#include <vl/RenderEventCallback.hpp>
#include <vl/vlTGA.hpp>
#include <vl/vlTIFF.hpp>
#include <vl/vlPNG.hpp>
#include <vl/vlJPG.hpp>
#include <vl/Image.hpp>

namespace vl
{
  //-----------------------------------------------------------------------------
  // ReadPixels
  //-----------------------------------------------------------------------------
  /**
   * A RenderEventCallback that copyes a rectangular pixel area from a source 
   * buffer to an Image at the end of a rendering.
   *
   * The actual copy is performed using the function Image::readPixels(). 
   * Using the function setSavePath() the Image will be saved
   * on the specified location.
   *
   * \sa
   * - Image
   * - Image::readPixels()
   * - RenderEventCallback
  */
  class ReadPixels: public RenderEventCallback
  {
  public:
    virtual const char* className() { return "ReadPixels"; }
    ReadPixels():
      mX ( 0 ),
      mY ( 0 ),
      mWidth ( 0 ),
      mHeight ( 0 ),
      mReadBuffer ( RDB_BACK_LEFT ),
      mStoreInPixelBufferObject(false)
    {
      #ifndef NDEBUG
        mObjectName = className();
      #endif
    }

    ReadPixels(int x, int y, int width, int height, EReadDrawBuffer read_buffer, Image* image, bool store_in_pbo):
      mX ( x ),
      mY ( y ),
      mWidth  ( width ),
      mHeight ( height ),
      mReadBuffer ( read_buffer ),
      mImage ( image ),
      mStoreInPixelBufferObject( store_in_pbo )
    {
      #ifndef NDEBUG
        mObjectName = className();
      #endif
    }

    virtual bool onRenderingStarted(const RenderingAbstract*)
    {
      readPixels();
      return true;
    }

    virtual bool onRenderingFinished(const RenderingAbstract*)
    {
      readPixels();
      return true;
    }

    virtual bool onRendererStarted(const RendererAbstract*)
    {
      readPixels();
      return true;
    }

    virtual bool onRendererFinished(const RendererAbstract*)
    {
      readPixels();
      return true;
    }

    void setup(int x, int y, int width, int height, EReadDrawBuffer read_buffer, bool store_in_pbo)
    {
      mX = x;
      mY = y;
      mWidth  = width;
      mHeight = height;
      mReadBuffer = read_buffer;
      mStoreInPixelBufferObject = store_in_pbo;
    }

    void setX(int x) { mX = x; }
    void setY(int y) { mY = y; }
    void setWidth(int width) { mWidth = width; }
    void setHeight(int height) { mHeight = height; }
    void setReadBuffer(EReadDrawBuffer buffer) { mReadBuffer = buffer; }
    void setImage(Image* image) { mImage = image; }
    void setSavePath(const String& path) { mSavePath = path; }

    int x() const { return mX; }
    int y() const { return mY; }
    int width() const { return mWidth; }
    int height() const { return mHeight; }
    EReadDrawBuffer readBuffer() const { return mReadBuffer; }
    Image* image() { return mImage.get(); }
    const Image* image() const { return mImage.get(); }
    const String& savePath() const { return mSavePath; }

    void setStoreInPixelBufferObject( bool use_pbo ) { mStoreInPixelBufferObject = use_pbo; }
    bool storeInPixelBufferObject() const { return mStoreInPixelBufferObject; }

  protected:
    void readPixels()
    {
      if (mImage.get() == NULL)
        mImage = new Image;
      mImage->readPixels(mX, mY, mWidth, mHeight, mReadBuffer, storeInPixelBufferObject() );
      if ( savePath().length() )
      {
        if (!saveImage(mImage.get(), savePath()))
          Log::error( Say("ReadPixels: unknown format for file: '%s'\n") << savePath() );
      }
    }

  protected:
    int mX;
    int mY;
    int mWidth;
    int mHeight;
    EReadDrawBuffer mReadBuffer;
    ref<Image> mImage;
    String mSavePath;
    bool mStoreInPixelBufferObject;
  };
}

#endif
