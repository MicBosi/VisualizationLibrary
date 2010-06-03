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

#ifndef Geometry_INCLUDE_ONCE
#define Geometry_INCLUDE_ONCE

#include <vl/GLBufferObject.hpp>
#include <vl/Vector2.hpp>
#include <vl/Vector4.hpp>
#include <vl/Renderable.hpp>
#include <vl/vlnamespace.hpp>
#include <vl/Log.hpp>
#include <vl/Say.hpp>
#include <vlut/Colors.hpp>
#include <vl/DrawElements.hpp>
#include <vl/DrawArrays.hpp>
#include <vl/Collection.hpp>
#include <vl/VertexAttributeArray.hpp>

namespace vl
{
  class OpenGLContext;
  //------------------------------------------------------------------------------
  // Geometry
  //------------------------------------------------------------------------------
  /**
   * The Geometry class is a Renderable that implements a polygonal mesh made of
   * polygons, lines and points.
   *
   * \sa 
   * - ArrayAbstract
   * - Renderable
   * - Actor
   * - Transform
   * - Effect
  */
  class Geometry: public Renderable
  {
    class TextureArray: public Object
    {
    public:
      TextureArray(int unit, ArrayAbstract* data): mTextureUnit(unit), mTexCoordArray(data) {}
      int mTextureUnit;
      ref<ArrayAbstract> mTexCoordArray;
      const GLBufferObject* gpuBuffer() const { return mTexCoordArray->gpuBuffer(); }
      GLBufferObject* gpuBuffer() { return mTexCoordArray->gpuBuffer(); }
    };
  public:
    virtual const char* className() { return "Geometry"; }
    /** Constructor. */
    Geometry();
    /** Destructor. */
    virtual ~Geometry();

    /**
     * Performs a shallow copy (as opposed to a deep copy) of the specified Geometry.
     * \sa deepCopy()
    */
    ref<Geometry> shallowCopy();

    /**
     * Performs a deep copy (as opposed to a shallow copy) of the specified Geometry.
     * \sa shallowCopy()
    */
    ref<Geometry> deepCopy() const;

    //! Performs a shallowCopy() of the Geometry
    Geometry& operator=(const Geometry& other);

    /*
     * Computes the bounding box and sphere from the local vertex attribute arrays (note: not from the VBO vertex array).
    */
    virtual void computeBounds_Implementation();

    size_t triangleCount() const;
    size_t lineCount() const;
    size_t pointCount() const;

    /**
     * Computes the normals in a "smooth" way, i.e. averaging the normals of those 
     * polygons that share one or more vertices.
     *
     * This function computes smoothed normals for triangle primitives and leaves 
     * unchanged the normals of line and point primitives when possible, i.e. when 
     * they don't share vertices with the polygonal primitives.
     *
     * \note 
     * This function modifies the local buffers. After calling this you might want 
     * to update the buffers allocated on the GPU.
    */
    void computeNormals();

    virtual void render(const Actor*, const OpenGLContext*, const Camera*) const;

    /**
     * Removes all the previously installed arrays.
    */
    virtual void clearArrays(bool clear_draw_calls=true);

    /**
     * Filles the color array with a single color.
     * If the color array has been already defined it must be of one of the following types:
     * ArrayFVec3, ArrayFVec4, ArrayUByte3, ArrayUBVec4
     * If no color array has been defined a new ArrayUBVec4 is installed and filled.
     * \remarks Since the color array element count will be set to the number elements in the 
     * position array this function must be called after the position array has been defined.
    */
    void setColorArray(const fvec4& col);

    void setVertexArray(ArrayAbstract* data);
    void setNormalArray(ArrayAbstract* data);
    void setColorArray(ArrayAbstract* data);
    void setSecondaryColorArray(ArrayAbstract* data);
    void setFogCoordArray(ArrayAbstract* data);
    void setTexCoordArray(int tex_unit, ArrayAbstract* data);

    const ArrayAbstract* vertexArray() const { return mVertexArray.get(); }
    ArrayAbstract* vertexArray() { return mVertexArray.get(); }
    const ArrayAbstract* normalArray() const { return mNormalArray.get(); }
    ArrayAbstract* normalArray() { return mNormalArray.get(); }
    const ArrayAbstract* colorArray() const { return mColorArray.get(); }
    ArrayAbstract* colorArray() { return mColorArray.get(); }
    const ArrayAbstract* secondaryColorArray() const { return mSecondaryColorArray.get(); }
    ArrayAbstract* secondaryColorArray() { return mSecondaryColorArray.get(); }
    const ArrayAbstract* fogCoordArray() const { return mFogCoordArray.get(); }
    ArrayAbstract* fogCoordArray() { return mFogCoordArray.get(); }
    const ArrayAbstract* texCoordArray(int tex_unit) const
    { 
      for(int i=0; i<mTexCoordArrays.size(); ++i)
        if (mTexCoordArrays.at(i)->mTextureUnit == tex_unit)
          return mTexCoordArrays.at(i)->mTexCoordArray.get();
      return NULL;
    }
    ArrayAbstract* texCoordArray(int tex_unit) 
    { 
      for(int i=0; i<mTexCoordArrays.size(); ++i)
        if (mTexCoordArrays.at(i)->mTextureUnit == tex_unit)
          return mTexCoordArrays.at(i)->mTexCoordArray.get();
      return NULL;
    }

    //! Returns the list of VertexAttributeArray objects bound to a Geometry
    Collection<VertexAttributeArray>* vertexAttributeArrays() { return &mVertexAttributeArrays; }
    //! Returns the list of VertexAttributeArray objects bound to a Geometry
    const Collection<VertexAttributeArray>* vertexAttributeArrays() const { return &mVertexAttributeArrays; }
    void setVertexAttributeArray(unsigned int name, bool normalize, bool pure_integer, ArrayAbstract* data) { setVertexAttributeArray(VertexAttributeArray(name, normalize, pure_integer, data)); }
    void setVertexAttributeArray(const VertexAttributeArray& info);
    const VertexAttributeArray* findVertexAttribute(unsigned int name) const;
    VertexAttributeArray* findVertexAttribute(unsigned int name);
    void eraseVertexAttributeByName(unsigned int name);

    //! Returns the list of Primitives objects bound to a Geometry
    Collection<Primitives>* primitives() { return &mPrimitives; }

    //! Returns the list of Primitives objects bound to a Geometry
    const Collection<Primitives>* primitives() const { return &mPrimitives; }

    void updateVBOs(bool discard_local_data=false);

    void deleteVBOs();

    /** 
    * Transforms vertices and normals belonging to this geometry.
    * If 'normalize' == true the normals are normalized after being transformed
    * \note This functions supports every possible vertex format, type and layout.
    * \sa
    *  - ArrayAbstract::transform()
    *  - ArrayAbstract::normalize()
    *  - ArrayAbstract::computeBoundingSphere()
    *  - ArrayAbstract::computeBoundingBox()
    */
    void transform(const mat4&matr, bool normalize = true);

    //! Tries to convert DrawElementsUInt in DrawElementsUShort/UByte and DrawElementsUShort in DrawElementsUByte in order to use less memory.
    void shrinkDrawElements();

    //! Merges the PT_TRIANGLE_STRIP Primitive objects into one single PT_TRIANGLE_STRIP DrawElements.
    void mergeTriangleStrips();

    //! Converts all the Primitives objects bound to a Geometry into DrawArrays.
    void convertPrimitivesToDrawArrays();

    //! Sorts the vertices of the geometry to maximize vertex-cache coherency.
    //! This function will work only if all the Primitives are DrawElements.
    //! \returns true if all the Primitives are DrawElements and the sorting took place.
    bool sortVertices();

    //! Calls DrawElements::sortTriangles().
    void sortTriangles();

    //! Assigns a random color to each vertex of each Primitives object. If a vertex is shared among more than one Primitives object its color is undefined.
    void colorizePrimitives();

    void regenerateVertices(const std::vector<size_t>& map_new_to_old);

    //! Updates the vertex buffer objects
    void updateVBOs();

  private:
    // standard arrays
    ref<ArrayAbstract> mVertexArray;
    ref<ArrayAbstract> mNormalArray;
    ref<ArrayAbstract> mColorArray;
    ref<ArrayAbstract> mSecondaryColorArray;
    ref<ArrayAbstract> mFogCoordArray;
    Collection<TextureArray> mTexCoordArrays;
    
    // mColorArray substitute
    fvec4 mColorArrayConstant;

    // custom arrays
    Collection<VertexAttributeArray> mVertexAttributeArrays;

    // render calls
    Collection<Primitives> mPrimitives;
  };
  //------------------------------------------------------------------------------
}

#endif
