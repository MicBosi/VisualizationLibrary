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

#include <vl/Object.hpp>
#include <vl/vlnamespace.hpp>

namespace vl
{
  class Camera;
  //------------------------------------------------------------------------------
  // RenderState
  //------------------------------------------------------------------------------
  /**
   * Base class for most of the OpenGL render state wrapper classes.
   *
   * \sa Shader, Effect, Actor
  */
  class RenderState: public Object
  {
  public:
    virtual const char* className() { return "RenderState"; }
    RenderState() 
    {
      #ifndef NDEBUG
        mName = "RenderState";
      #endif
      mTextureUnit = 0;
    }
    virtual ~RenderState() {}
    virtual ERenderState type() const { return RS_NONE; }
    virtual void apply(const Camera*) const = 0;
    virtual void disable() const {}
    virtual void enable()  const {}
    // used by GLSLProgram to link the program and by TextureUnit to create the texture
    virtual void initResources() {}
    // used by texture-unit-related render states
    int textureUnit() const { return mTextureUnit; }
    // used by texture-unit-related render states
    void setUnitIndex(int unit_index) { mTextureUnit = unit_index; }
  protected:
    int mTextureUnit;
  };
}

#endif
