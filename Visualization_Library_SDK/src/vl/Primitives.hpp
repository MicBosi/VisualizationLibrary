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
  /** The base class of DrawElements, MultiDrawElements, DrawRangeElements, DrawArrays 
   * which are used by Geometry to define a set of primitives to be rendered, see Geometry::primitives().
   *
   * \sa DrawElements, MultiDrawElements, DrawRangeElements, DrawArrays, Geometry, Actor */
  class Primitives: public Object
  {
  public:
    virtual const char* className() { return "Primitives"; }

    Primitives(): mType(PT_TRIANGLES), mEnabled(true) {}

    Primitives& operator=(const Primitives& other)
    {
      mType      = other.mType;
      mEnabled   = other.mEnabled;
      return *this;
    }

    void setPrimitiveType(EPrimitiveType type) { mType = type; }
    EPrimitiveType primitiveType() const { return mType; }

    virtual void render(bool use_vbo = true) const = 0;
    virtual ref<Primitives> clone() const = 0;

    virtual void deleteVBOs() = 0;
    virtual void updateVBOs(bool discard_local_data = false)  = 0;
    virtual unsigned int handle() const = 0;

    virtual size_t indexCount() const = 0;
    virtual size_t index(int i) const = 0;

    virtual int triangleCount() const = 0;
    virtual int lineCount() const = 0;
    virtual int pointCount() const = 0;

    //! Supported only by DrawElements and DrawRangeElements
    // this also does not work when primitive restart is enabled.
    virtual void sortTriangles() {}

    void setEnabled(bool enable) { mEnabled = enable; }
    bool isEnabled() const { return mEnabled; }

    //! Fills \p out_triangle with the indices of the index-th triangle.
    //! Returns \p true on success or \p false if the function fails. You should not call this function
    //! in conjunction with primitive restart or with MultiDrawElements*.
    //! This function is useful when you want to treat an arbitrary primitive type as if it was made of
    //! triangles, for example for tessellation purposes or geometry processing.
    //! The supported primitive types are: PT_TRIANGLES, PT_TRIANGLE_STRIP, PT_TRIANGLE_FAN, PT_POLYGON
    //! PT_QUADS and PT_QUAD_STRIP.
    //! @param tri_index The index of the triangle whose info we want to retrieve.
    //! @param out_triangle Filled with the 3 indices forming the requested triangle. Thus it must point to a buffer of at least 3 elements.
    virtual bool getTriangle( size_t tri_index, unsigned int* out_triangle ) const = 0;
  
  protected:
    bool getTriangle_internal( size_t tri_index, unsigned int* out_triangle ) const;

  protected:
      EPrimitiveType mType;
      bool mEnabled;
  };
}

#endif
