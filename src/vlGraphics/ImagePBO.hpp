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

#ifndef ImagePBO_INCLUDE_ONCE
#define ImagePBO_INCLUDE_ONCE

#include <vlCore/Image.hpp>
#include <vlGraphics/GLBufferObject.hpp>

namespace vl
{
  //-----------------------------------------------------------------------------
  // ImagePBO
  //-----------------------------------------------------------------------------
  /** Represents a vl::Image with an associated Pixel Buffer Object. */
  class ImagePBO: public Image
  {
  public:
    virtual const char* className() const { return "vl::ImagePBO"; }
	
    //! Constructor.
    ImagePBO()
    {
      VL_DEBUG_SET_OBJECT_NAME()
      mPBO = new GLBufferObject;
    }
	
	  //! Constructors: loads an image from the specified path.
    ImagePBO(const String& path): Image(path) 
    {
      VL_DEBUG_SET_OBJECT_NAME()
      mPBO = new GLBufferObject;
    }
	
	  //! Constructor: initializes an image with the specified parameters.
    ImagePBO(int x, int y, int z, int bytealign, EImageFormat format, EImageType type): Image(x, y, z, bytealign, format, type) 
    {
      VL_DEBUG_SET_OBJECT_NAME()
      mPBO = new GLBufferObject;
    }

	  //! The associated OpenGL Buffer Object
    const GLBufferObject* pixelBufferObject() const { return mPBO.get(); }
    
	  //! The associated OpenGL Buffer Object
    GLBufferObject* pixelBufferObject() { return mPBO.get(); }
    
	  //! The associated OpenGL Buffer Object
    void setPixelBufferObject(GLBufferObject* pbo) { mPBO = pbo; }

  protected:
    ref<GLBufferObject> mPBO;
  };
  //-----------------------------------------------------------------------------
}

#endif
