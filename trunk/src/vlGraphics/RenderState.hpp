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

#ifndef RenderState_INCLUDE_ONCE
#define RenderState_INCLUDE_ONCE

#include <vlCore/Object.hpp>
#include <vlCore/vlnamespace.hpp>

namespace vl
{
  class Camera;
  class OpenGLContext;
  //------------------------------------------------------------------------------
  // RenderState
  //------------------------------------------------------------------------------
  /**
   * Base class for most of the OpenGL render state wrapper classes.
   *
   * \sa Shader, Effect, Actor
  */
  class VLGRAPHICS_EXPORT RenderState: public Object
  {
  public:
    virtual const char* className() { return "vl::RenderState"; }
    RenderState() 
    {
      VL_DEBUG_SET_OBJECT_NAME()
    }
    virtual ERenderState type() const { return RS_NONE; }

    /** The parameter cameara is NULL if we are disabling the state, non-NULL if we are enabling it. */
    virtual void apply(const Camera* camera, OpenGLContext* ctx) const = 0;
  };
  //------------------------------------------------------------------------------
  // TextureState
  //------------------------------------------------------------------------------
  /**
   * Base class for the OpenGL texture state wrapper classes.
   *
   * \sa Shader, Effect, Actor, RenderState
  */
  class VLGRAPHICS_EXPORT TextureState: public RenderState
  {
  public:
    virtual const char* className() { return "vl::TextureState"; }
    TextureState(): mTextureUnit(0)
    {
      VL_DEBUG_SET_OBJECT_NAME()
    }
    int textureUnit() const { return mTextureUnit; }
    void setUnitIndex(int unit_index) { mTextureUnit = unit_index; }
  protected:
    int mTextureUnit;
  };

}

#endif
