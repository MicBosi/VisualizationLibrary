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

#include <vl/Array.hpp>
#include <vl/TriangleIterator.hpp>
#include <vl/IndexIterator.hpp>
#include <vl/PatchParameter.hpp>

namespace vl 
{
  //------------------------------------------------------------------------------
  // DrawCall
  //------------------------------------------------------------------------------
  /** The base class of DrawElements, MultiDrawElements, DrawRangeElements, DrawArrays 
   * which are used by Geometry to define a set of primitives to be rendered, see Geometry::drawCalls().
   *
   * \sa DrawElements, MultiDrawElements, DrawRangeElements, DrawArrays, Geometry, Actor */
  class DrawCall: public Object
  {
  public:
    virtual const char* className() { return "DrawCall"; }

    DrawCall(): mType(PT_TRIANGLES), mEnabled(true) {}

    DrawCall& operator=(const DrawCall& other)
    {
      mType      = other.mType;
      mEnabled   = other.mEnabled;
      return *this;
    }

    void setPrimitiveType(EPrimitiveType type) { mType = type; }
    EPrimitiveType primitiveType() const { return mType; }

    virtual void render(bool use_vbo = true) const = 0;
    virtual ref<DrawCall> clone() const = 0;

    virtual void deleteVBOs() = 0;
    virtual void updateVBOs(bool discard_local_data=false, bool force_update=false) = 0;
    virtual unsigned int handle() const = 0;

    void setEnabled(bool enable) { mEnabled = enable; }
    bool isEnabled() const { return mEnabled; }

    //! Returns a TriangleIterator used to iterate through the triangles of a DrawCall.
    //! Basically the iterator tesselates in triangles any DrawCall of type: PT_TRIANGLES, PT_TRIANGLE_STRIP
    //! PT_TRIANGLE_FAN, PT_POLYGON, PT_QUADS, PT_QUAD_STRIP.
    virtual TriangleIterator triangleIterator() const = 0;

    //! Returns a IndexIterator used to iterate through the virtual indices of a DrawCall.
    //! This \note The returned indices already take into account primitive restart and base vertex.
    virtual IndexIterator indexIterator() const = 0;

    /** Counts the number of virtual indices of a DrawCall. */
    int countIndices() const
    {
      int count = 0;
      for( IndexIterator it = indexIterator(); !it.isEnd(); it.next() )
        count++;
      return count;
    }

    /** Returns the number of instances for this set of primitives. */
    virtual int instances() const { return 1; }

    /** Attach a PatchParameter to a DrawCall to be used when using primitive-type PT_PATCHES */
    void setPatchParameter(PatchParameter* patch_param) { mPatchParameter = patch_param; }

    /** The PatchParameter attached to a DrawCall to be used when using primitive-type PT_PATCHES */
    PatchParameter* patchParameter() { return mPatchParameter.get(); }

    /** The PatchParameter attached to a DrawCall to be used when using primitive-type PT_PATCHES */
    const PatchParameter* patchParameter() const { return mPatchParameter.get(); }

  protected:
    void applyPatchParameters() const
    {
      if (mType == PT_PATCHES && mPatchParameter)
        mPatchParameter->apply();
    }

  protected:
      ref<PatchParameter> mPatchParameter;
      EPrimitiveType mType;
      bool mEnabled;
  };
}

#endif
