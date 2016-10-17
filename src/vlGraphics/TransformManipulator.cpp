/**************************************************************************************/
/*                                                                                    */
/*  Visualization Library                                                             */
/*  http://www.visualizationlibrary.org                                               */
/*                                                                                    */
/*  Copyright (c) 2005-2011, Michele Bosi, Fabien Mathieu							  */
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

#include <vlGraphics/TransformManipulator.hpp>
#include <vlGraphics/RayIntersector.hpp>
#include <vlGraphics/OpenGLContext.hpp>
#include <vlGraphics/GLSL.hpp>
#include <iostream>

const float dRef = 50.0f;
const float angleRef = M_PI / 8.0f;
const float oppositeRef = std::tan(angleRef / 2.0f) * 2.0 * dRef;

using namespace vl;

//------------------------------------------------------------------------------
// TransformManipulator
//------------------------------------------------------------------------------

void TransformManipulator::setEditGeometry(Geometry* editGeometry, vl::Uniform* modelMatrixUniform)
{ 
	mEditGeometry = editGeometry;
	
	mModelMatrixUniform = modelMatrixUniform;
}

void TransformManipulator::mouseDownEvent(vl::EMouseButton button, int x, int y)
{
	if(mManipulatorActor.get() == NULL)
		return;
	
	vl::mat4 viewMatrix, projectionMatrix, manipulatorPositionMatrix;
    mViewMatrixUniform->getUniform(&viewMatrix);
    mProjectionMatrixUniform->getUniform(&projectionMatrix);
	
	ref< GLSLProgram > manipulatorProgram = mManipulatorActor->effect()->shader()->gocGLSLProgram();
		
	if(manipulatorProgram.get() == NULL)
		return;
	manipulatorProgram->gocUniform("ManipulatorPositionMatrix")->getUniform(&manipulatorPositionMatrix);
	
	
	int reversedY = openglContext()->height() - y;
	RayIntersector intersector;
		
	intersector.actors()->push_back(mManipulatorActor.get());
		
	/*for(int i = 0; i < 16; ++i)
	{
		if(i % 4 == 0)
			std::cout << std::endl;
		std::cout << viewMatrix.ptr()[i] << "\t";
	}
	
	for(int i = 0; i < 16; ++i)
	{
		if(i % 4 == 0)
			std::cout << std::endl;
		std::cout << projectionMatrix.ptr()[i] << "\t";
	}*/
		
			
	Ray ray = createRay(x, reversedY, viewMatrix * manipulatorPositionMatrix, projectionMatrix);
	/*ray.setOrigin(vl::vec3(x, reversedY, 0.0f));
	ray.setDirection(vl::vec3(0.0f,0.0, -1.0f));*/
	intersector.setRay(ray);
	intersector.intersect();
	
	std::cout << std::endl;
	
	std::cout << "ray.origin : (" << ray.origin().x() << ";" << ray.origin().y() << ";" << ray.origin().z() << ")" << std::endl;
	std::cout << "ray.direction : (" << ray.direction().x() << ";" << ray.direction().y() << ";" << ray.direction().z() << ")" << std::endl;
	
	std::cout << "down x: " << x << ";y: " << y << std::endl;
	
	if(intersector.intersections().size() != 0)	// We have an intersection
	{
		std::cout << "Manipulator Intersection" << std::endl;
		mActive = true;
		mStartPosition.x() = x;
		mStartPosition.y() = y;
		
		
		mEventListener->setEnabled(false);
		
		mFirstIntersection = intersector.intersections().at(0)->intersectionPoint();
	
	}
}

void TransformManipulator::mouseUpEvent(vl::EMouseButton button, int x, int y)
{
	if(mActive)
	{
		//vl::defLogger()->debug("mouse up event\n");
		std::cout << "up x: " << x << ";y: " << y << std::endl;
		mActive = false;
		
		mEventListener->setEnabled(true);
		openglContext()->setContinuousUpdate(true);
		
		
		if(mEditGeometry.get() != NULL)
		{
			vl::ref< vl::ArrayAbstract > vertexPositions = mEditGeometry->vertexArray() != NULL ? mEditGeometry->vertexArray() : mEditGeometry->vertexAttribArray(vl::VA_Position)->data();
			vertexPositions->transform(mModelMatrix);
			mEditGeometry->updateDirtyBufferObject(vl::BUM_KeepRamBufferAndForceUpdate);
		}
		
		ref< GLSLProgram > manipulatorProgram = mManipulatorActor->effect()->shader()->gocGLSLProgram();
		
		if(manipulatorProgram.get() == NULL)
			return;
		
		mEditGeometry->computeBounds();
		vl::AABB boundingBox = mEditGeometry->boundingBox();
		vl::vec3 center = boundingBox.center();
								
		manipulatorProgram->gocUniform("ManipulatorPositionMatrix")->setUniform(vl::mat4::getTranslation(center));
		
		mOldModelMatrix = mModelMatrix;
		mModelMatrix = vl::mat4::getIdentity();
		mModelMatrixUniform->setUniform(mModelMatrix);
	}
}


vl::Ray TransformManipulator::createRay(int x, int y, const vl::mat4& viewMatrix, const vl::mat4& projectionMatrix)
{
    vl::vec4 out;
    vl::Ray ray;
    if (!unprojectFromUniform( vl::vec3((vl::real)x,(vl::real)y,0), out, viewMatrix, projectionMatrix ))
      ray = vl::Ray();
    else
    {
      ray.setOrigin(out.xyz());
      ray.setDirection( (out.xyz() - viewMatrix.getInverse().getT()).normalize() );
    }
	
	return ray;
}

bool TransformManipulator::unprojectFromUniform(const vl::vec3& in, vl::vec4& out, const vl::mat4& viewMatrix, const vl::mat4& projectionMatrix)
{

    vl::vec4 v;
    v.x() = in.x();
    v.y() = in.y();
    v.z() = in.z();
    v.w() = 1.0;

    // map from viewport to 0-1
    v.x() = (v.x() - mCamera->viewport()->x()) / mCamera->viewport()->width();
    v.y() = (v.y() - mCamera->viewport()->y()) / mCamera->viewport()->height();

    // map to range -1 to 1
    v.x() = v.x() * 2.0f - 1.0f;
    v.y() = v.y() * 2.0f - 1.0f;
    v.z() = v.z() * 2.0f - 1.0f;

    vl::real det=0;
    vl::mat4 inverse = (projectionMatrix * viewMatrix).getInverse(&det);
    if (!det)
      return false;

    v = inverse * v;
    if (v.w() == 0.0)
      return false;

    out = v / v.w();
    return true;

}

//------------------------------------------------------------------------------
// TranslationManipulator
//------------------------------------------------------------------------------

void TranslationManipulator::mouseMoveEvent(int x, int y)
{
	if(mActive)
	{	  
	  vl::mat4 viewMatrix, projectionMatrix, manipulatorPositionMatrix;
      mViewMatrixUniform->getUniform(&viewMatrix);
      mProjectionMatrixUniform->getUniform(&projectionMatrix); 
	  
	  ref< GLSLProgram > manipulatorProgram = mManipulatorActor->effect()->shader()->gocGLSLProgram();
		
	  if(manipulatorProgram.get() == NULL)
		return;
	  manipulatorProgram->gocUniform("ManipulatorPositionMatrix")->getUniform(&manipulatorPositionMatrix);
	  
	  vl::Ray ray = createRay(x, openglContext()->height() - y, viewMatrix * manipulatorPositionMatrix, projectionMatrix);
	  
	  vec4 plane;
	  
	  if((mConstraint & CONSTRAINT_X) != 0)
	  {
		  if((mConstraint & CONSTRAINT_Z) != 0)
		  {
			  // Plan XZ
			  plane = vec4(0.0f, 1.0f, 0.0f, mFirstIntersection.y());
		  }
		  else
		  {
			  // Plan XY ou Axe X seul
			  plane = vec4(0.0f, 0.0f, 1.0f, mFirstIntersection.z());  
		  }
	  }
	  else if((mConstraint & CONSTRAINT_Y) != 0)
	  {
		 if((mConstraint & CONSTRAINT_Z) != 0)
		 {
			 // Plan YZ
			 plane = vec4(1.0f, 0.0f, 0.0f, mFirstIntersection.x());
		 }
		 else
		 {
			 // Axe Y seul car géré par le premier if, on se place sur le plan XY
			 plane = vec4(0.0f, 0.0f, 1.0f, mFirstIntersection.z());  
			 
		 }
	  }
	  else if((mConstraint & CONSTRAINT_Z) != 0)
	  {
		 // Plan YZ
		 plane = vec4(1.0f, 0.0f, 0.0f, mFirstIntersection.x());
	  }
	  
	  
	  float numerator = -((plane.x() * ray.origin().x()) + (plane.y() * ray.origin().y()) + (plane.z() * ray.origin().z()) - plane.w());
	  float denominator = ray.direction().x() * plane.x() + ray.direction().y() * plane.y() + ray.direction().z() * plane.z(); 
	  
	  float t0 = numerator / denominator;
	  
	  vec3 secondIntersection = ray.origin() + t0 * ray.direction();
	  
	  vec3 translation = secondIntersection - mFirstIntersection;
	  translation.x() = (mConstraint & CONSTRAINT_X) == 0 ? 0.0f : translation.x();
	  translation.y() = (mConstraint & CONSTRAINT_Y) == 0 ? 0.0f : translation.y();
	  translation.z() = (mConstraint & CONSTRAINT_Z) == 0 ? 0.0f : translation.z();
	  
	  mModelMatrix = mat4::getTranslation(translation);
	  mModelMatrixUniform->setUniform(mModelMatrix);
	  
      openglContext()->update();
	}
}

//------------------------------------------------------------------------------
// RotationManipulator
//------------------------------------------------------------------------------

void RotationManipulator::mouseMoveEvent(int x, int y)
{
	if(mActive)
	{
	  float yf = openglContext()->height() - y;
      float yf0 = openglContext()->height() - mStartPosition.y();

      float xf = x;
      float xf0 = mStartPosition.x();
	  
	  vl::mat4 viewMatrix, projectionMatrix, manipulatorPositionMatrix;
      mViewMatrixUniform->getUniform(&viewMatrix);
      mProjectionMatrixUniform->getUniform(&projectionMatrix); 
	  
	  ref< GLSLProgram > manipulatorProgram = mManipulatorActor->effect()->shader()->gocGLSLProgram();
		
	  vl::vec3 center;
	  if(manipulatorProgram.get() == NULL)
		return;
	  manipulatorProgram->gocUniform("ManipulatorPositionMatrix")->getUniform(&manipulatorPositionMatrix);
	  
	  vl::mat4 rot = viewMatrix.getInverse().as3x3();
	  vl::vec3 dir = rot * vl::vec3(0.0f,0.0f,-1.0f);
	  	  
	  vl::vec4 out = projectionMatrix * viewMatrix * vl::vec4(manipulatorPositionMatrix.getT(), 1.0f);
	  
	  if (out.w() == 0.0f)
		return;
	  
	  out.x() /= out.w();
	  out.y() /= out.w();
	  
	  // map to range 0-(width/height)
	  out.x() = (out.x() * 0.5f + 0.5f) * openglContext()->width();
	  out.y() = (out.y() * 0.5f + 0.5f) * openglContext()->height();
	  
	  vec2 u = vec2(xf0, yf0) - out.xy();
	  vec2 v = vec2(xf, yf) - out.xy();
	  u.normalize();
	  v.normalize();
	  
	  // alpha compris [-pi, pi]
	  float alpha = std::atan2(v.y(), v.x()) - std::atan2(u.y(), u.x());
	  		
	  if(mConstraint == CONSTRAINT_X)
	  {
		alpha = vl::dot(dir, vec3(1.0f, 0.0f, 0.0f)) > 0 ? -alpha : alpha;
		mModelMatrix = manipulatorPositionMatrix * vl::mat4::getRotation(alpha * 180 / M_PI, 1.0f, 0.0f, 0.0f) * manipulatorPositionMatrix.getInverse();
	  }
	  else if(mConstraint == CONSTRAINT_Y)
	  {
		alpha = vl::dot(dir, vec3(0.0f, 1.0f, 0.0f)) > 0 ? -alpha : alpha;
		mModelMatrix = manipulatorPositionMatrix * vl::mat4::getRotation(alpha * 180 / M_PI, 0.0f, 1.0f, 0.0f) * manipulatorPositionMatrix.getInverse();
	  }	  
	  else if(mConstraint == CONSTRAINT_Z)
	  {
		alpha = vl::dot(dir, vec3(0.0f, 0.0f, 1.0f)) > 0 ? -alpha : alpha;
		mModelMatrix = manipulatorPositionMatrix * vl::mat4::getRotation(alpha * 180 / M_PI, 0.0f, 0.0f, 1.0f) * manipulatorPositionMatrix.getInverse();
	  }
	  mModelMatrixUniform->setUniform(mModelMatrix);

          openglContext()->update();
	}
}

//------------------------------------------------------------------------------
// ScaleManipulator
//------------------------------------------------------------------------------

void ScaleManipulator::mouseMoveEvent(int x, int y)
{
	if(mActive)
	{
	  vl::mat4 viewMatrix, projectionMatrix, manipulatorPositionMatrix;
      mViewMatrixUniform->getUniform(&viewMatrix);
      mProjectionMatrixUniform->getUniform(&projectionMatrix); 
	  
	  ref< GLSLProgram > manipulatorProgram = mManipulatorActor->effect()->shader()->gocGLSLProgram();
		
	  if(manipulatorProgram.get() == NULL)
		return;
	  manipulatorProgram->gocUniform("ManipulatorPositionMatrix")->getUniform(&manipulatorPositionMatrix);
	  
	  vl::Ray ray = createRay(x, openglContext()->height() - y, viewMatrix * manipulatorPositionMatrix, projectionMatrix);
	  
	  vec4 plane;
	  
	  if((mConstraint & CONSTRAINT_X) != 0)
	  {
		  if((mConstraint & CONSTRAINT_Z) != 0)
		  {
			  // Plan XZ
			  plane = vec4(0.0f, 1.0f, 0.0f, mFirstIntersection.y());
		  }
		  else
		  {
			  // Plan XY ou Axe X seul
			  plane = vec4(0.0f, 0.0f, 1.0f, mFirstIntersection.z());  
		  }
	  }
	  else if((mConstraint & CONSTRAINT_Y) != 0)
	  {
		 if((mConstraint & CONSTRAINT_Z) != 0)
		 {
			 // Plan YZ
			 plane = vec4(1.0f, 0.0f, 0.0f, mFirstIntersection.x());
		 }
		 else
		 {
			 // Axe Y seul car géré par le premier if, on se place sur le plan XY
			 plane = vec4(0.0f, 0.0f, 1.0f, mFirstIntersection.z());  
			 
		 }
	  }
	  else if((mConstraint & CONSTRAINT_Z) != 0)
	  {
		 // Plan YZ
		 plane = vec4(1.0f, 0.0f, 0.0f, mFirstIntersection.x());
	  }
	  
	  
	  float numerator = -((plane.x() * ray.origin().x()) + (plane.y() * ray.origin().y()) + (plane.z() * ray.origin().z()) - plane.w());
	  float denominator = ray.direction().x() * plane.x() + ray.direction().y() * plane.y() + ray.direction().z() * plane.z(); 
	  
	  float t0 = numerator / denominator;
	  
	  vec3 secondIntersection = ray.origin() + t0 * ray.direction();
	  
	  vec3 scale = secondIntersection - mFirstIntersection;

	  //vec3 scale = -up * dy - right * dx;
	  scale.x() = (mConstraint & CONSTRAINT_X) == 0 ? 1.0f : 1.0f + scale.x();
	  scale.y() = (mConstraint & CONSTRAINT_Y) == 0 ? 1.0f : 1.0f + scale.y();
	  scale.z() = (mConstraint & CONSTRAINT_Z) == 0 ? 1.0f : 1.0f + scale.z();
	  
	  
	  mModelMatrix = manipulatorPositionMatrix * vl::mat4::getScaling(scale) * manipulatorPositionMatrix.getInverse();
	  //mModelMatrix = manipulatorPositionMatrix * vl::mat4::getTranslation(-1.0f, 0.0f, 0.0f) * vl::mat4::getScaling(scale) * vl::mat4::getTranslation(1.0f, 0.0f, 0.0f)  * manipulatorPositionMatrix.getInverse();
	  
	  mModelMatrixUniform->setUniform(mModelMatrix);

          openglContext()->update();
	}
}


