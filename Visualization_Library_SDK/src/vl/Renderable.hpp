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

#ifndef Renderable_INCLUDE_ONCE
#define Renderable_INCLUDE_ONCE

#include <vl/Object.hpp>
#include <vl/Transform.hpp>
#include <vl/AABB.hpp>
#include <vl/Sphere.hpp>

namespace vl
{
  //------------------------------------------------------------------------------
  // Renderable
  //------------------------------------------------------------------------------
  class Actor;
  class Shader;
  class Transform;
  class Camera;
  class OpenGLContext;
  /**
   * An abstract class that represents all the objects that can be rendered.
   * In order to render a Renderable you have to bind it to an Actor.
   * An Actor glues together a Renderable, an Effect and eventually a Transform.
   * Note that the same Renderable can be associated to more than one Actor.
   * 
   * \sa Actor, Effect, Transform, Geometry
  */
  class Renderable: public Object
  {
    Renderable(const Renderable& other): Object(other)
    {
      #ifndef NDEBUG
        mObjectName = className();
      #endif
    }

  public:
    virtual const char* className() { return "Renderable"; }
    
    /** Constructor. */
    Renderable(): mBoundsUpdateTick(0), mDisplayList(0), mBoundsDirty(true), 
    mDisplayListEnabled(false), mDisplayListDirty(true), mVBOEnabled(true), mVBODirty(true) {}
    
    /** Destructor. */
    virtual ~Renderable() { deleteDisplayList(); }

    /** Renders the Renderable. */
    virtual void render(const Actor* actor, const Shader* shader, const Camera* camera, OpenGLContext* gl_context) const = 0;

    long long boundsUpdateTick() const { return mBoundsUpdateTick; }
    void computeBounds() { computeBounds_Implementation(); setBoundsDirty(false); }
    void setBoundsDirty(bool dirty) { ++mBoundsUpdateTick; mBoundsDirty = dirty; }
    bool boundsDirty() const { return mBoundsDirty; }
    void setBoundingBox( const AABB& aabb ) { mAABB = aabb; setBoundsDirty(false); }
    const AABB& boundingBox() const { return mAABB; }
    void setBoundingSphere( const Sphere& sphere) { mSphere = sphere; setBoundsDirty(false); }
    const Sphere& boundingSphere() const { return mSphere; }

    unsigned int displayList() const { return mDisplayList; }
    void setDisplayList(unsigned int disp_list) { mDisplayList = disp_list; }

    bool displayListEnabled() const { return mDisplayListEnabled; }
    void setDisplayListEnabled(bool enabled) { mDisplayListEnabled = enabled; }

    bool displayListDirty() const { return mDisplayListDirty; }
    void setDisplayListDirty(bool dirty) { mDisplayListDirty = dirty; }

    bool vboEnabled() const { return mVBOEnabled; }
    void setVBOEnabled(bool enabled) { mVBOEnabled = enabled; }

    bool isVBODirty() const { return mVBODirty; }
    void setVBODirty(bool dirty) { mVBODirty = dirty; }

    //! Uploads the data stored in the local buffers on the GPU memory.
    //! If 'discard_local_data' is set to \p true the memory used by the local buffers is released.
    virtual void updateVBOs(bool discard_local_data, bool force_update) = 0;

    //! Destroyes the VBO (VertexBufferObjects) associated to this Geometry attributes.
    //! If 'clear_primitives' all the VBOs associated the DrawCall objects are also deleted.
    //! \note This function does not touch the local (non GPU) data stored in the buffers associated to the vertex attributes and DrawCall.
    virtual void deleteVBOs() {}

    void deleteDisplayList() 
    {
      if (displayList())
        glDeleteLists(displayList(), 1);
      mDisplayList = 0;
    }

    void compileDisplayList(const Actor* actor, const Shader* shader, const Camera* camera, OpenGLContext* gl_context)
    {
      if (!displayList())
        setDisplayList( glGenLists(1) );
      glNewList( displayList(), GL_COMPILE );
        render( actor, shader, camera, gl_context );
      glEndList();
      VL_CHECK_OGL();
      setDisplayListDirty(false);
    }

  protected:
    virtual void computeBounds_Implementation() = 0;

  private:
    long long mBoundsUpdateTick;
    unsigned int mDisplayList;
    bool mBoundsDirty;
    bool mDisplayListEnabled;
    bool mDisplayListDirty;
    bool mVBOEnabled;
    bool mVBODirty;
    AABB mAABB;
    Sphere mSphere;
  };
}

#endif
