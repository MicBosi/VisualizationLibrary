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

#ifndef DrawCommand_INCLUDE_ONCE
#define DrawCommand_INCLUDE_ONCE

#include <vl/Object.hpp>
#include <vl/Array.hpp>

namespace vl 
{
  //------------------------------------------------------------------------------
  // Primitives
  //------------------------------------------------------------------------------
  /**
   * This is the base class of DrawElements and DrawArrays which respectively wrap
   * the OpenGL functions glDrawElements() and glDrawArrays().
  */
  class Primitives: public Object
  {
  public:
    Primitives(): mType(PT_TRIANGLES), mInstances(1), mEnabled(true) {}

    virtual const char* className() { return "Primitives"; }

    virtual ~Primitives() {}
    void setPrimitiveType(EPrimitiveType type) { mType = type; }
    EPrimitiveType primitiveType() const { return mType; }

    virtual void render(bool use_vbo = true) const = 0;
    virtual ref<Primitives> clone() const = 0;

    virtual void deleteVBOs() = 0;
    virtual void updateVBOs(bool discard_local_data = false)  = 0;
    virtual unsigned int handle() const = 0;

    virtual size_t indexCount() const = 0;
    virtual size_t index(int i) const = 0;

    virtual size_t triangleCount() const = 0;
    virtual size_t lineCount() const = 0;
    virtual size_t pointCount() const = 0;
    //! Supported only by DrawElements
    virtual void sortTriangles() {}

    size_t instances() const { return mInstances; }
    void setInstances(size_t instances) { mInstances = instances; }

    void setEnabled(bool enable) { mEnabled = enable; }
    bool isEnabled() const { return mEnabled; }

    //! Returns in 'out_triangle' the indices of the index-th triangle or '(unsigned int)-1' if
    //! an error occurred (unsupported primitive type, indices out of bounds, NULL pointers).
    //! This function is useful when you want to treat an arbitrary primitive type as if it was made of
    //! triangles, for example for tessellation purposes or geometry processing.
    //! The supported primitive types are: PT_TRIANGLES, PT_TRIANGLE_STRIP, PT_TRIANGLE_FAN, PT_POLYGON
    //! PT_QUADS and PT_QUAD_STRIP.
    //! @param tri_index The index of the triangle whose info we want to retrieve.
    //! @return out_triangle The 3 indices forming the requested triangle.
    void getTriangle( size_t tri_index, unsigned int* out_triangle ) const;

  protected:
      EPrimitiveType mType;
      size_t mInstances;
      bool mEnabled;
  };
}

#endif
