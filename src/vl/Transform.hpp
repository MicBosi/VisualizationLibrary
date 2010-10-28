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

#ifndef Transform_INCLUDE_ONCE
#define Transform_INCLUDE_ONCE

#include <vl/vlnamespace.hpp>
#include <vl/Object.hpp>
#include <vl/Matrix4.hpp>
#include <vector>

namespace vl
{
  class Camera;

  //------------------------------------------------------------------------------
  // Transform
  //------------------------------------------------------------------------------
  /**
   * Implements a 4x4 matrix transform usually used to define the position and 
   * orientation of an Actor.
   *
   * Transforms can be linked together to create a tree-like hierarchy.
   *
   * \sa Actor, Effect, Renderable, Geometry
  */
  class Transform: public Object
  {
  public:
    virtual const char* className() { return "Transform"; }
    Transform(): mParent(NULL), mWorldMatrixUpdateTick(0), mAssumeIdentityWorldMatrix(false) 
    { 
      #ifndef NDEBUG
        mObjectName = className();
      #endif
    }
    Transform(const mat4& matrix): mParent(NULL), mAssumeIdentityWorldMatrix(false) 
    { 
      setLocalMatrix(matrix); 
      #ifndef NDEBUG
        mObjectName = className();
      #endif
    }
    virtual ~Transform();

    void addChild(Transform* child);
    void setChild(int index, Transform* child);
    int childCount() const;
    Transform* child(int i);
    Transform* lastChild();
    void eraseChild(Transform* child);
    void eraseChildren(int index, int count);
    void eraseAllChildren();

    void eraseAllChildrenRecursive();

    const Transform* parent() const { return mParent; }
    Transform* parent() { return mParent; }
    mat4 getComputedWorldMatrix();

    //! Sets the local matrix. After calling this you might want to call computeWorldMatrix() or computeWorldMatrixRecursive()
    void setLocalMatrix(const mat4& matrix);
    //! Returns the local matrix
    const mat4& localMatrix() const;

    //! Utility function equivalent to \p setLocalMatrix( mat4::translation(x,y,z)*localMatrix() )
    void translate(Real x, Real y, Real z);
    //! Utility function equivalent to \p setLocalMatrix( mat4::translation(t)*localMatrix() )
    void translate(const vec3& t);
    //! Utility function equivalent to \p setLocalMatrix( mat4::scaling(x,y,z)*localMatrix() )
    void scale(Real x, Real y, Real z);
    //! Utility function equivalent to \p setLocalMatrix( mat4::rotation(degrees,x,y,z)*localMatrix() )
    void rotate(Real degrees, Real x, Real y, Real z);
    //! Utility function equivalent to \p setLocalMatrix( mat4::rotation(from,to)*localMatrix() )
    void rotate(const vec3& from, const vec3& to);

    virtual void computeWorldMatrix(Camera* camera=NULL);
    void computeWorldMatrixRecursive(Camera* camera=NULL);
    const mat4& worldMatrix() const;
    long long worldMatrixUpdateTick() const { return mWorldMatrixUpdateTick; }

    /**
     * If set to true the world matrix of this transform will always be considered and identity.
     *
     * Is usually used to save calculations for top Transforms with many sub-Transforms.
    */
    void setAlwaysIdentityWorldMatrix(bool assume_I) { mAssumeIdentityWorldMatrix = assume_I; }

    /**
     * If set to true the world matrix of this transform will always be considered and identity.
     *
     * Is usually used to save calculations for top Transforms with many sub-Transforms.
    */
    bool alwaysIdentityWorldMatrix() { return mAssumeIdentityWorldMatrix; }

  protected:
    void setWorldMatrix(const mat4& matrix);
    mat4 mWorldMatrix; 
    mat4 mLocalMatrix;
    std::vector< ref<Transform> > mChildren;
    Transform* mParent;
    long long mWorldMatrixUpdateTick;
    bool mAssumeIdentityWorldMatrix;
  };
}

#endif
