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

#include <vl/Transform.hpp>
#include <vl/Camera.hpp>
#include <algorithm>

using namespace vl;

//-----------------------------------------------------------------------------
// Transform
//-----------------------------------------------------------------------------
Transform::~Transform()
{
}
//-----------------------------------------------------------------------------
int Transform::childCount() const 
{ 
  return (int)mChildren.size(); 
}
//-----------------------------------------------------------------------------
Transform* Transform::child(int i) 
{ 
  VL_CHECK(i>=0 && i<(int)mChildren.size())
  return mChildren[i].get(); 
}
//-----------------------------------------------------------------------------
Transform* Transform::lastChild()
{
  return mChildren.back().get();
}
//-----------------------------------------------------------------------------
void Transform::setChild(int index, Transform* child)
{
  VL_CHECK( index < (int)mChildren.size() )
  mChildren[index]->mParent = NULL;
  mChildren[index] = child;
  mChildren[index]->mParent = this;
}
//-----------------------------------------------------------------------------
void Transform::addChild(Transform* child)
{
  VL_CHECK(child != NULL)
  VL_CHECK(child->mParent == NULL)
  VL_CHECK(std::find(mChildren.begin(), mChildren.end(), child) == mChildren.end());
  mChildren.push_back(child);
  child->mParent = this;
}
//-----------------------------------------------------------------------------
void Transform::eraseChild(Transform* child)
{
  VL_CHECK(child != NULL)
  std::vector< ref<Transform> >::iterator it;
  it = std::find(mChildren.begin(), mChildren.end(), child);
  VL_CHECK(it != mChildren.end())
  if (it != mChildren.end())
  {
    (*it)->mParent = NULL;
    mChildren.erase(it);
  }
}
//-----------------------------------------------------------------------------
void Transform::eraseChildren(int index, int count)
{
  VL_CHECK( index + count <= (int)mChildren.size() );

  for(int j=index; j<index+count; ++j)
    mChildren[j]->mParent = NULL;

  for(int i=index+count, j=index; i<(int)mChildren.size(); ++i, ++j)
    mChildren[j] = mChildren[i];

  mChildren.resize( mChildren.size() - count );
}
//-----------------------------------------------------------------------------
void Transform::eraseAllChildren()
{
  for(int i=0; i<(int)mChildren.size(); ++i)
    mChildren[i]->mParent = NULL;
  mChildren.clear();
}
//-----------------------------------------------------------------------------
void Transform::eraseAllChildrenRecursive()
{
  for(int i=0; i<(int)mChildren.size(); ++i)
  {
    mChildren[i]->eraseAllChildrenRecursive();
    mChildren[i]->mParent = NULL;
  }
  mChildren.clear();
}
//-----------------------------------------------------------------------------
void Transform::computeWorldMatrix(Camera*)
{
  if( alwaysIdentityWorldMatrix() )
  {
    setWorldMatrix(mat4()); 
  }
  else
  /* top Transforms are usually alwaysIdentityWorldMatrix() == true for performance reasons */
  if( parent() && !parent()->alwaysIdentityWorldMatrix() )
  {
    setWorldMatrix( parent()->worldMatrix() * localMatrix() );
  }
  else
    setWorldMatrix(localMatrix());
}
//-----------------------------------------------------------------------------
void Transform::setWorldMatrix(const mat4& matrix) 
{ 
  //if (!mWorldMatrix) 
  //  mWorldMatrix = new MatrixObject; 
  //mWorldMatrix->setMatrix(matrix); 
  mWorldMatrix = matrix; 
  ++mWorldMatrixUpdateTick;
}
//-----------------------------------------------------------------------------
const mat4& Transform::worldMatrix() const 
{ 
  //if (!mWorldMatrix) 
  //  mWorldMatrix = new MatrixObject;
  //return mWorldMatrix->matrix(); 
  return mWorldMatrix;
}
//-----------------------------------------------------------------------------
void Transform::setLocalMatrix(const mat4& matrix) 
{ 
  //if (!mLocalMatrix) 
  //  mLocalMatrix = new MatrixObject; 
  //mLocalMatrix->setMatrix(matrix); 
  mLocalMatrix = matrix;
}
//-----------------------------------------------------------------------------
const mat4& Transform::localMatrix() const
{ 
  //if (!mLocalMatrix) 
  //  mLocalMatrix = new MatrixObject;
  //return mLocalMatrix->matrix(); 
  return mLocalMatrix;
}
//-----------------------------------------------------------------------------
void Transform::translate(Real x, Real y, Real z)
{
  setLocalMatrix( mat4::translation(x,y,z)*localMatrix() );
}
//-----------------------------------------------------------------------------
void Transform::translate(const vec3& t)
{
  setLocalMatrix( mat4::translation(t)*localMatrix() );
}
//-----------------------------------------------------------------------------
void Transform::scale(Real x, Real y, Real z)
{
  setLocalMatrix( mat4::scaling(x,y,z)*localMatrix() );
}
//-----------------------------------------------------------------------------
void Transform::rotate(Real degrees, Real x, Real y, Real z)
{
  setLocalMatrix( mat4::rotation(degrees,x,y,z)*localMatrix() );
}
//-----------------------------------------------------------------------------
void Transform::rotate(const vec3& from, const vec3& to)
{
  setLocalMatrix( mat4::rotation(from,to)*localMatrix() );
}
//-----------------------------------------------------------------------------
//! Returns the matrix computed concatenating this transform's matrix with its parents'.
mat4 Transform::getComputedWorldMatrix()
{
  mat4 world = localMatrix();
  ref<Transform> par = parent();
  while(par)
  {
    world = par->localMatrix() * world;
    par = par->parent();
  }
  return world;
}
//-----------------------------------------------------------------------------
void Transform::computeWorldMatrixRecursive(Camera* camera)
{
  computeWorldMatrix(camera);
  for(int i=0; i<(int)mChildren.size(); ++i)
    mChildren[i]->computeWorldMatrixRecursive(camera);
}
//-----------------------------------------------------------------------------


