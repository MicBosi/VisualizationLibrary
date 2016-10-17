/**************************************************************************************/
/*                                                                                    */
/*  Visualization Library                                                             */
/*  http://www.visualizationlibrary.org                                               */
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

#include "BaseDemo.hpp"
#include <vlCore/Colors.hpp>
#include <vlGraphics/GeometryPrimitives.hpp>
#include <vlGraphics/TransformManipulator.hpp>
#include <vlGraphics/Uniform.hpp>
#include <vlGraphics/Rendering.hpp>
#include <vlGraphics/GLSL.hpp>
#include <vlGraphics/DrawArrays.hpp>

#include <iostream>

const float screenHeight = 5.0f;

/*
This applet tests transform manipulator
*/
class App_TransformManipulator: public BaseDemo
{
public:
  void initEvent()
  {
    vl::Log::notify(appletInfo());
	
	vl::vec4 colorX = vl::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	vl::vec4 colorY = vl::vec4(0.0f, 1.0f, 0.0f, 1.0f);
	vl::vec4 colorZ = vl::vec4(0.0f, 0.0f, 1.0f, 1.0f);

    /* effect to be applied to the manipulator */
    mEffectX = new vl::Effect;
	mEffectY = new vl::Effect;
	mEffectZ = new vl::Effect;

	/* uniform used to transform actor */
	mCubeModelMatrixUniform = new vl::Uniform("ModelMatrix");
	mCubeModelMatrixUniform->setUniform(vl::mat4::getIdentity());
	
	mPyramidModelMatrixUniform = new vl::Uniform("ModelMatrix");
	mPyramidModelMatrixUniform->setUniform(vl::mat4::getIdentity());
	
	mCurrentModelMatrixUniform = mCubeModelMatrixUniform;
	
	// use context to determine fix size of gizmo
	float alpha = std::atan((static_cast< float >(openglContext()->height()) / 2.0f) / 50.0f) * 2.0f;
	
	//mProjectionGizmoMatrix = new vl::Uniform("ProjectionMatrix");	
	
	/*mProjectionGizmoMatrix->setUniform(vl::mat4::getPerspective((alpha * 180) / M_PI, static_cast< float >(openglContext()->width()) / static_cast< float >(openglContext()->height()),
																0.001f, 100000.0f));*/
	
	vl::ref< vl::Rendering > appletRendering = rendering()->as< vl::Rendering >();
	//appletRendering->camera()->setProjectionPerspective(60.0f, 0.001f, 10000.0f);
	appletRendering->camera()->setViewMatrixLookAt(vl::vec3(0.0, 0.0, 50.0f), vl::vec3(0.0, 0.0, 0.0f), vl::vec3(0.0, 1.0, 0.0f));
	
	mViewMatrixUniform = new vl::Uniform("ViewMatrix");
	mViewMatrixUniform->setUniform(appletRendering->camera()->viewMatrix());
	
	mManipulatorPositionMatrixUniform = new vl::Uniform("ManipulatorPositionMatrix");
	mManipulatorPositionMatrixUniform->setUniform(vl::mat4::getIdentity());
	
	mProjectionMatrixUniform = new vl::Uniform("ProjectionMatrix");
	mProjectionMatrixUniform->setUniform(appletRendering->camera()->projectionMatrix());
	
	/*mViewMatrixGizmoMatrix = new vl::Uniform("ViewMatrix");
	mViewMatrixGizmoMatrix->setUniform(vl::mat4::getLookAt(vl::vec3(0.0f, 0.0f, 50.0f), vl::vec3(0.0f, 0.0f, 0.0f), vl::vec3(0.0f, 1.0f, 0.0f)));*/
	//mViewMatrixGizmoMatrix->setUniform(appletRendering->camera()->viewMatrix());
		
	vl::ref< vl::GLSLVertexShader > vertexShader = new vl::GLSLVertexShader("glsl/transformManipulator.vs");
	vl::ref< vl::GLSLFragmentShader > fragmentShader = new vl::GLSLFragmentShader("glsl/transformManipulator.fs");
	
	mEffectX->shader()->enable(vl::EN_DEPTH_TEST);
	vl::ref< vl::GLSLProgram > programX = mEffectX->shader()->gocGLSLProgram();
	programX->attachShader(vertexShader.get());
	programX->attachShader(fragmentShader.get());
	programX->bindAttribLocation(vl::VA_Position, "VertexPosition");
	//programX->setUniform(mModelMatrixUniform.get());
	programX->setUniform(mViewMatrixUniform.get());
	//programX->setUniform(mProjectionGizmoMatrix.get());
	programX->setUniform(mProjectionMatrixUniform.get());
	programX->gocUniform("Color")->setUniform(colorX);
	programX->setUniform(mManipulatorPositionMatrixUniform.get());
	//programX->gocUniform("ManipulatorPositionMatrix")->setUniform(vl::mat4::getIdentity());
	
	
	mEffectY->shader()->enable(vl::EN_DEPTH_TEST);
	vl::ref< vl::GLSLProgram > programY = mEffectY->shader()->gocGLSLProgram();
	programY->attachShader(vertexShader.get());
	programY->attachShader(fragmentShader.get());
	programY->bindAttribLocation(vl::VA_Position, "VertexPosition");
	//programY->setUniform(mModelMatrixUniform.get());
	programY->setUniform(mViewMatrixUniform.get());
	//programY->setUniform(mProjectionGizmoMatrix.get());
	programY->setUniform(mProjectionMatrixUniform.get());
	programY->gocUniform("Color")->setUniform(colorY);
	programY->setUniform(mManipulatorPositionMatrixUniform.get());
	//programY->gocUniform("ManipulatorPositionMatrix")->setUniform(vl::mat4::getIdentity());
	
	mEffectZ->shader()->enable(vl::EN_DEPTH_TEST);
	vl::ref< vl::GLSLProgram > programZ = mEffectZ->shader()->gocGLSLProgram();
	programZ->attachShader(vertexShader.get());
	programZ->attachShader(fragmentShader.get());
	programZ->bindAttribLocation(vl::VA_Position, "VertexPosition");
	//programZ->setUniform(mModelMatrixUniform.get());
	programZ->setUniform(mViewMatrixUniform.get());
	//programZ->setUniform(mProjectionGizmoMatrix.get());
	programZ->setUniform(mProjectionMatrixUniform.get());
	programZ->gocUniform("Color")->setUniform(colorZ);
	programZ->setUniform(mManipulatorPositionMatrixUniform.get());
	//programZ->gocUniform("ManipulatorPositionMatrix")->setUniform(vl::mat4::getIdentity());
	
	/*vl::ref< vl::Actor > manipulatorActorX = sceneManager()->tree()->addActor( mTransformManipulatorGeometryX.get(), effectManipulatorX.get(), NULL);
	vl::ref< vl::Actor > manipulatorActorY = sceneManager()->tree()->addActor( mTransformManipulatorGeometryY.get(), effectManipulatorY.get(), NULL);
	vl::ref< vl::Actor > manipulatorActorZ = sceneManager()->tree()->addActor( mTransformManipulatorGeometryZ.get(), effectManipulatorZ.get(), NULL);*/
	
	/*mTransformManipulatorX->setManipulatorActor(manipulatorActorX.get());
	mTransformManipulatorX->setViewMatrixUniform(mViewMatrixUniform.get());
	mTransformManipulatorX->setProjectionMatrixUniform(mProjectionMatrixUniform.get());
	mTransformManipulatorX->setCamera(appletRendering->camera());
	mTransformManipulatorX->setConstraint(vl::TransformManipulator::CONSTRAINT_X);
	
	mTransformManipulatorY->setManipulatorActor(manipulatorActorY.get());
	mTransformManipulatorY->setViewMatrixUniform(mViewMatrixUniform.get());
	mTransformManipulatorY->setProjectionMatrixUniform(mProjectionMatrixUniform.get());
	mTransformManipulatorY->setCamera(appletRendering->camera());
	mTransformManipulatorY->setConstraint(vl::TransformManipulator::CONSTRAINT_Y);
	
	mTransformManipulatorZ->setManipulatorActor(manipulatorActorZ.get());
	mTransformManipulatorZ->setViewMatrixUniform(mViewMatrixUniform.get());
	mTransformManipulatorZ->setProjectionMatrixUniform(mProjectionMatrixUniform.get());
	mTransformManipulatorZ->setCamera(appletRendering->camera());
	mTransformManipulatorZ->setConstraint(vl::TransformManipulator::CONSTRAINT_Z);*/
	
	
	
	/* landmark */
	vl::ref< vl::Geometry > landMark = new vl::Geometry;
	vl::ref< vl::ArrayFloat3 > arrayPosition = new vl::ArrayFloat3;
	arrayPosition->resize(6);
	arrayPosition->at(0) = vl::vec3(0.0f, 0.0f, 0.0f);	arrayPosition->at(1) = vl::vec3(10.0f, 0.0f, 0.0f);
	arrayPosition->at(2) = vl::vec3(0.0f, 0.0f, 0.0f);	arrayPosition->at(3) = vl::vec3(0.0f, 10.0f, 0.0f);
	arrayPosition->at(4) = vl::vec3(0.0f, 0.0f, 0.0f);	arrayPosition->at(5) = vl::vec3(0.0f, 0.0f, 10.0f);
	vl::ref< vl::ArrayFloat3 > arrayColor = new vl::ArrayFloat3;
	arrayColor->resize(6);
	arrayColor->at(0) = vl::vec3(1.0f, 0.0f, 0.0f);	arrayColor->at(1) = vl::vec3(1.0f, 0.0f, 0.0f);
	arrayColor->at(2) = vl::vec3(0.0f, 1.0f, 0.0f);	arrayColor->at(3) = vl::vec3(0.0f, 1.0f, 0.0f);
	arrayColor->at(4) = vl::vec3(0.0f, 0.0f, 1.0f);	arrayColor->at(5) = vl::vec3(0.0f, 0.0f, 1.0f);
	vl::ref< vl::DrawCall > drawArrays = new vl::DrawArrays(vl::PT_LINES, 0, 6);
	landMark->setVertexAttribArray(vl::VA_Position, arrayPosition.get());
	landMark->setVertexAttribArray(vl::VA_Color, arrayColor.get());
	landMark->drawCalls().push_back(drawArrays.get());
	
	vl::ref<vl::Effect> effect_landmark = new vl::Effect;
	effect_landmark->shader()->enable(vl::EN_DEPTH_TEST);
	
	
	
	vl::ref< vl::GLSLProgram > program_landmark = effect_landmark->shader()->gocGLSLProgram();
	program_landmark->attachShader(new vl::GLSLVertexShader("glsl/simpleColored.vs"));
	program_landmark->attachShader(new vl::GLSLFragmentShader("glsl/simpleColored.fs"));
	program_landmark->bindAttribLocation(vl::VA_Position, "VertexPosition");
	program_landmark->bindAttribLocation(vl::VA_Color, "VertexColor");
	//program_landmark->gocUniform("ProjectionMatrix")->setUniform(appletRendering->camera()->projectionMatrix());
	program_landmark->setUniform(mViewMatrixUniform.get());
	program_landmark->setUniform(mProjectionMatrixUniform.get());
	program_landmark->gocUniform("ModelMatrix")->setUniform(vl::mat4::getIdentity());
	
	sceneManager()->tree()->addActor( landMark.get(), effect_landmark.get(), NULL);
	
	mCubeGeometry = vl::makeBox(vl::vec3(10.0f, 0.0f, 0.0f), 2.0f, 2.0f, 2.0f, false);
	mCubeGeometry->convertToVertexAttribs();
	
	mPyramidGeometry = vl::makePyramid(vl::vec3(-4.0f, 10.0f, -7.0f), 5.0f, 5.0f);
	mPyramidGeometry->convertToVertexAttribs();
	
	mEffectBox = new vl::Effect;
	mEffectBox->shader()->enable(vl::EN_DEPTH_TEST);
	mEffectBox->shader()->gocPolygonMode()->set(vl::PM_LINE, vl::PM_LINE);
	vl::ref< vl::GLSLProgram > programBox = mEffectBox->shader()->gocGLSLProgram();
	programBox->attachShader(new vl::GLSLVertexShader("glsl/simple.vs"));
	programBox->attachShader(new vl::GLSLFragmentShader("glsl/simple.fs"));
	programBox->bindAttribLocation(vl::VA_Position, "VertexPosition");
	programBox->setUniform(mViewMatrixUniform.get());
	programBox->setUniform(mProjectionMatrixUniform.get());
	programBox->setUniform(mCubeModelMatrixUniform.get());
	programBox->gocUniform("Color")->setUniform(vl::vec4(0.0f, 1.0f, 1.0f, 1.0f));
	
	mEffectPyramid = new vl::Effect;
	mEffectPyramid->shader()->enable(vl::EN_DEPTH_TEST);
	mEffectPyramid->shader()->gocPolygonMode()->set(vl::PM_LINE, vl::PM_LINE);
	vl::ref< vl::GLSLProgram > programPyramid = mEffectPyramid->shader()->gocGLSLProgram();
	programPyramid->attachShader(new vl::GLSLVertexShader("glsl/simple.vs"));
	programPyramid->attachShader(new vl::GLSLFragmentShader("glsl/simple.fs"));
	programPyramid->bindAttribLocation(vl::VA_Position, "VertexPosition");
	programPyramid->setUniform(mViewMatrixUniform.get());
	programPyramid->setUniform(mProjectionMatrixUniform.get());
	programPyramid->setUniform(mPyramidModelMatrixUniform.get());
	programPyramid->gocUniform("Color")->setUniform(vl::vec4(0.0f, 1.0f, 1.0f, 1.0f));
	
	sceneManager()->tree()->addActor( mCubeGeometry.get(), mEffectBox.get(), NULL);
	sceneManager()->tree()->addActor( mPyramidGeometry.get(), mEffectPyramid.get(), NULL);
	
	mCubeGeometry->computeBounds();
	vl::AABB boundingBox = mCubeGeometry->boundingBox();
	vl::vec3 center = boundingBox.center();

	mCurrentGeometry = mCubeGeometry;
		
	mManipulatorPositionMatrixUniform->setUniform(vl::mat4::getTranslation(center));
	
	/*vl::ref< vl::Uniform > scaleMatrixUniform = new vl::Uniform("ScaleMatrix");
	scaleMatrixUniform->setUniform(scaleMatrix);
	
	programX->setUniform(scaleMatrixUniform.get());
	programY->setUniform(scaleMatrixUniform.get());
	programZ->setUniform(scaleMatrixUniform.get());*/
	
	createTranslationManipulators();
  }
  
  void createTranslationManipulators()
  {
	/* remove the manipulator to the applet */
	if(mTransformManipulatorX.get() != NULL)	openglContext()->removeEventListener(mTransformManipulatorX.get());
	if(mTransformManipulatorY.get() != NULL)	openglContext()->removeEventListener(mTransformManipulatorY.get());
	if(mTransformManipulatorZ.get() != NULL)	openglContext()->removeEventListener(mTransformManipulatorZ.get());
	
	/* manipulators */
	mTransformManipulatorX = new vl::TranslationManipulator;
	mTransformManipulatorY = new vl::TranslationManipulator;
	mTransformManipulatorZ = new vl::TranslationManipulator;
	
	/* add the manipulator to the applet */
	openglContext()->addEventListener(mTransformManipulatorX.get());
	openglContext()->addEventListener(mTransformManipulatorY.get());
	openglContext()->addEventListener(mTransformManipulatorZ.get());
	
	mTransformManipulatorX->setEventListener(trackball());
	mTransformManipulatorY->setEventListener(trackball());
	mTransformManipulatorZ->setEventListener(trackball());
	
	createTranslationManipulatorsGeometry();
	
	updateManipulators();
  }
  
  void createTranslationManipulatorsGeometry()
  {
	/* Gyzmo translation geometry */
	vl::AABB aabb = mCurrentGeometry->boundingBox();
	float size = aabb.longestSideLength() * 1.2f;
	
	mTransformManipulatorGeometryX = vl::makeCylinder(vl::vec3(0.0f, size / 2.0f, 0.0f), 0.1f * size, size);	// Replace with an arrow
	mTransformManipulatorGeometryX->convertToVertexAttribs();
	vl::ref< vl::ArrayAbstract > vertexPositionsX = mTransformManipulatorGeometryX->vertexArray() != NULL ? mTransformManipulatorGeometryX->vertexArray() : mTransformManipulatorGeometryX->vertexAttribArray(vl::VA_Position)->data();
	vertexPositionsX->transform(vl::mat4::getRotation(vl::vec3(0.0f, 1.0f, 0.0f), vl::vec3(1.0f, 0.0f, 0.0f)));
	mTransformManipulatorGeometryX->updateDirtyBufferObject(vl::BUM_KeepRamBufferAndForceUpdate);
	
	mTransformManipulatorGeometryY = vl::makeCylinder(vl::vec3(0.0f, size / 2.0f, 0.0f), 0.1f * size, size);	// Replace with an arrow
	mTransformManipulatorGeometryY->convertToVertexAttribs();
	
	mTransformManipulatorGeometryZ = vl::makeCylinder(vl::vec3(0.0f, size / 2.0f, 0.0f), 0.1f * size, size);	// Replace with an arrow
	mTransformManipulatorGeometryZ->convertToVertexAttribs();
	vl::ref< vl::ArrayAbstract > vertexPositionsZ = mTransformManipulatorGeometryZ->vertexArray() != NULL ? mTransformManipulatorGeometryZ->vertexArray() : mTransformManipulatorGeometryZ->vertexAttribArray(vl::VA_Position)->data();
	vertexPositionsZ->transform(vl::mat4::getRotation(vl::vec3(0.0f, 1.0f, 0.0f), vl::vec3(0.0f, 0.0f, 1.0f)));
	mTransformManipulatorGeometryZ->updateDirtyBufferObject(vl::BUM_KeepRamBufferAndForceUpdate);
  }
  
  void createRotationManipulators()
  {
	/* remove the manipulator to the applet */
	if(mTransformManipulatorX.get() != NULL)	openglContext()->removeEventListener(mTransformManipulatorX.get());
	if(mTransformManipulatorY.get() != NULL)	openglContext()->removeEventListener(mTransformManipulatorY.get());
	if(mTransformManipulatorZ.get() != NULL)	openglContext()->removeEventListener(mTransformManipulatorZ.get());
	
	/* manipulators */
	mTransformManipulatorX = new vl::RotationManipulator;
	mTransformManipulatorY = new vl::RotationManipulator;
	mTransformManipulatorZ = new vl::RotationManipulator;
	
	/* add the manipulator to the applet */
	openglContext()->addEventListener(mTransformManipulatorX.get());
	openglContext()->addEventListener(mTransformManipulatorY.get());
	openglContext()->addEventListener(mTransformManipulatorZ.get());
	
	mTransformManipulatorX->setEventListener(trackball());
	mTransformManipulatorY->setEventListener(trackball());
	mTransformManipulatorZ->setEventListener(trackball());
	
	createRotationManipulatorsGeometry();
	
	updateManipulators();
  }
  
  void createRotationManipulatorsGeometry()
  {
	/* Gyzmo translation geometry */
	vl::AABB aabb = mCurrentGeometry->boundingBox();
	float size = aabb.longestSideLength() * 1.2f;
	
	mTransformManipulatorGeometryX = vl::makeTorus(vl::vec3(0.0f, 0.0f, 0.0f), size, 0.02f * size);
	mTransformManipulatorGeometryX->convertToVertexAttribs();
	vl::ref< vl::ArrayAbstract > vertexPositionsX = mTransformManipulatorGeometryX->vertexArray() != NULL ? mTransformManipulatorGeometryX->vertexArray() : mTransformManipulatorGeometryX->vertexAttribArray(vl::VA_Position)->data();
	vertexPositionsX->transform(vl::mat4::getRotation(vl::vec3(0.0f, 0.0f, 1.0f), vl::vec3(1.0f, 0.0f, 0.0f)));
	mTransformManipulatorGeometryX->updateDirtyBufferObject(vl::BUM_KeepRamBufferAndForceUpdate);
	
	mTransformManipulatorGeometryY = vl::makeTorus(vl::vec3(0.0f, 0.0f, 0.0f), size, 0.02f * size);	// Replace with an arrow
	mTransformManipulatorGeometryY->convertToVertexAttribs();
	vl::ref< vl::ArrayAbstract > vertexPositionsY = mTransformManipulatorGeometryY->vertexArray() != NULL ? mTransformManipulatorGeometryY->vertexArray() : mTransformManipulatorGeometryY->vertexAttribArray(vl::VA_Position)->data();
	vertexPositionsY->transform(vl::mat4::getRotation(vl::vec3(0.0f, 0.0f, 1.0f), vl::vec3(0.0f, 1.0f, 0.0f)));
	mTransformManipulatorGeometryY->updateDirtyBufferObject(vl::BUM_KeepRamBufferAndForceUpdate);
	
	mTransformManipulatorGeometryZ = vl::makeTorus(vl::vec3(0.0f, 0.0f, 0.0f), size, 0.02f * size);	// Replace with an arrow
	mTransformManipulatorGeometryZ->convertToVertexAttribs();
	
  }
  
  void createScaleManipulators()
  {
	/* remove the manipulator to the applet */
	if(mTransformManipulatorX.get() != NULL)	openglContext()->removeEventListener(mTransformManipulatorX.get());
	if(mTransformManipulatorY.get() != NULL)	openglContext()->removeEventListener(mTransformManipulatorY.get());
	if(mTransformManipulatorZ.get() != NULL)	openglContext()->removeEventListener(mTransformManipulatorZ.get());
	  
	/* manipulators */
	mTransformManipulatorX = new vl::ScaleManipulator;
	mTransformManipulatorY = new vl::ScaleManipulator;
	mTransformManipulatorZ = new vl::ScaleManipulator;
	
	/* add the manipulator to the applet */
	openglContext()->addEventListener(mTransformManipulatorX.get());
	openglContext()->addEventListener(mTransformManipulatorY.get());
	openglContext()->addEventListener(mTransformManipulatorZ.get());
	
	mTransformManipulatorX->setEventListener(trackball());
	mTransformManipulatorY->setEventListener(trackball());
	mTransformManipulatorZ->setEventListener(trackball());
	
	createTranslationManipulatorsGeometry();
	
	updateManipulators();
  }
  
  void createScaleManipulatorsGeometry()
  {
	/* Gyzmo translation geometry */
	vl::AABB aabb = mCurrentGeometry->boundingBox();
	float size = aabb.longestSideLength() * 1.2f;
	
	mTransformManipulatorGeometryX = vl::makeCylinder(vl::vec3(0.0f, size / 2.0f, 0.0f), 0.1f * size, size);	// Replace with an arrow
	mTransformManipulatorGeometryX->convertToVertexAttribs();
	vl::ref< vl::ArrayAbstract > vertexPositionsX = mTransformManipulatorGeometryX->vertexArray() != NULL ? mTransformManipulatorGeometryX->vertexArray() : mTransformManipulatorGeometryX->vertexAttribArray(vl::VA_Position)->data();
	vertexPositionsX->transform(vl::mat4::getRotation(vl::vec3(0.0f, 1.0f, 0.0f), vl::vec3(1.0f, 0.0f, 0.0f)));
	mTransformManipulatorGeometryX->updateDirtyBufferObject(vl::BUM_KeepRamBufferAndForceUpdate);
	
	mTransformManipulatorGeometryY = vl::makeCylinder(vl::vec3(0.0f, size / 2.0f, 0.0f), 0.1f * size, size);	// Replace with an arrow
	mTransformManipulatorGeometryY->convertToVertexAttribs();
	
	mTransformManipulatorGeometryZ = vl::makeCylinder(vl::vec3(0.0f, size / 2.0f, 0.0f), 0.1f * size, size);	// Replace with an arrow
	mTransformManipulatorGeometryZ->convertToVertexAttribs();
	vl::ref< vl::ArrayAbstract > vertexPositionsZ = mTransformManipulatorGeometryZ->vertexArray() != NULL ? mTransformManipulatorGeometryZ->vertexArray() : mTransformManipulatorGeometryZ->vertexAttribArray(vl::VA_Position)->data();
	vertexPositionsZ->transform(vl::mat4::getRotation(vl::vec3(0.0f, 1.0f, 0.0f), vl::vec3(0.0f, 0.0f, 1.0f)));
	mTransformManipulatorGeometryZ->updateDirtyBufferObject(vl::BUM_KeepRamBufferAndForceUpdate);
  }
  
  void updateManipulators()
  {
		sceneManager()->tree()->eraseActor(mTransformManipulatorActorX.get());
		sceneManager()->tree()->eraseActor(mTransformManipulatorActorY.get());
		sceneManager()->tree()->eraseActor(mTransformManipulatorActorZ.get());
		
		mTransformManipulatorActorX = sceneManager()->tree()->addActor( mTransformManipulatorGeometryX.get(), mEffectX.get(), NULL);
		mTransformManipulatorActorY = sceneManager()->tree()->addActor( mTransformManipulatorGeometryY.get(), mEffectY.get(), NULL);
		mTransformManipulatorActorZ = sceneManager()->tree()->addActor( mTransformManipulatorGeometryZ.get(), mEffectZ.get(), NULL);
		
		mTransformManipulatorX->setManipulatorActor(mTransformManipulatorActorX.get());
		mTransformManipulatorX->setViewMatrixUniform(mViewMatrixUniform.get());
		//mTransformManipulatorX->setProjectionMatrixUniform(mProjectionGizmoMatrix.get());
		mTransformManipulatorX->setProjectionMatrixUniform(mProjectionMatrixUniform.get());
		mTransformManipulatorX->setCamera(rendering()->as< vl::Rendering >()->camera());
		mTransformManipulatorX->setConstraint(vl::TransformManipulator::CONSTRAINT_X);
		
		mTransformManipulatorY->setManipulatorActor(mTransformManipulatorActorY.get());
		mTransformManipulatorY->setViewMatrixUniform(mViewMatrixUniform.get());
		//mTransformManipulatorY->setProjectionMatrixUniform(mProjectionGizmoMatrix.get());
		mTransformManipulatorY->setProjectionMatrixUniform(mProjectionMatrixUniform.get());
		mTransformManipulatorY->setCamera(rendering()->as< vl::Rendering >()->camera());
		mTransformManipulatorY->setConstraint(vl::TransformManipulator::CONSTRAINT_Y);
		
		mTransformManipulatorZ->setManipulatorActor(mTransformManipulatorActorZ.get());
		mTransformManipulatorZ->setViewMatrixUniform(mViewMatrixUniform.get());
		//mTransformManipulatorZ->setProjectionMatrixUniform(mProjectionGizmoMatrix.get());
		mTransformManipulatorZ->setProjectionMatrixUniform(mProjectionMatrixUniform.get());
		mTransformManipulatorZ->setCamera(rendering()->as< vl::Rendering >()->camera());
		mTransformManipulatorZ->setConstraint(vl::TransformManipulator::CONSTRAINT_Z);
		
		if(dynamic_cast< vl::ScaleManipulator* >(mTransformManipulatorX.get()) == NULL)	// Do not scale manipulator to scale object
		{
			mEffectX->shader()->glslProgram()->setUniform(mCurrentModelMatrixUniform.get());
			mEffectY->shader()->glslProgram()->setUniform(mCurrentModelMatrixUniform.get());
			mEffectZ->shader()->glslProgram()->setUniform(mCurrentModelMatrixUniform.get());
		}
		else
		{
			vl::ref< vl::Uniform > modelMatrixUniformX, modelMatrixUniformY, modelMatrixUniformZ;
			modelMatrixUniformX = new vl::Uniform("ModelMatrix");
			modelMatrixUniformX->setUniform(vl::mat4::getIdentity());
			modelMatrixUniformY = new vl::Uniform("ModelMatrix");
			modelMatrixUniformY->setUniform(vl::mat4::getIdentity());
			modelMatrixUniformZ = new vl::Uniform("ModelMatrix");
			modelMatrixUniformZ->setUniform(vl::mat4::getIdentity());
			mEffectX->shader()->glslProgram()->setUniform(modelMatrixUniformX.get());
			mEffectY->shader()->glslProgram()->setUniform(modelMatrixUniformY.get());
			mEffectZ->shader()->glslProgram()->setUniform(modelMatrixUniformZ.get());
		}
		
		//mEffectBox->shader()->glslProgram()->setUniform(mCurrentModelMatrixUniform.get());
			
		mTransformManipulatorX->setEditGeometry(mCurrentGeometry.get(), mCurrentModelMatrixUniform.get());
		mTransformManipulatorY->setEditGeometry(mCurrentGeometry.get(), mCurrentModelMatrixUniform.get());
		mTransformManipulatorZ->setEditGeometry(mCurrentGeometry.get(), mCurrentModelMatrixUniform.get());
  }

  /* rotate the camera around the sphere */
  void updateScene()
  {
	vl::ref< vl::Rendering > appletRendering = rendering()->as< vl::Rendering >();
	mViewMatrixUniform->setUniform(appletRendering->camera()->viewMatrix());
  }
  
  void keyReleaseEvent(unsigned short, vl::EKey key)
  {
	  if(key == vl::Key_T)
	  {
		if(dynamic_cast< vl::TranslationManipulator* >(mTransformManipulatorX.get()) == NULL)
			createTranslationManipulators();
	  }
	  else if(key == vl::Key_R)
	  {
		if(dynamic_cast< vl::RotationManipulator* >(mTransformManipulatorX.get()) == NULL)
			createRotationManipulators();
	  }
	  else if(key == vl::Key_S)
	  {
		if(dynamic_cast< vl::ScaleManipulator* >(mTransformManipulatorX.get()) == NULL)
			createScaleManipulators();
	  }
	  else if(key == vl::Key_Left || key == vl::Key_Right)
	  {
		if(mCurrentGeometry.get() == mCubeGeometry.get())
		{
			mCurrentGeometry = mPyramidGeometry;
			mCurrentModelMatrixUniform = mPyramidModelMatrixUniform;
			//mCurrentEffect = mEffectPyramid;
		}
		else if(mCurrentGeometry.get() == mPyramidGeometry.get())
		{
			mCurrentGeometry = mCubeGeometry;
			mCurrentModelMatrixUniform = mCubeModelMatrixUniform;
			//mCurrentEffect = mEffectBox;
		}
		
		mCurrentGeometry->computeBounds();
		vl::AABB boundingBox = mCurrentGeometry->boundingBox();
		vl::vec3 center = boundingBox.center();

		mManipulatorPositionMatrixUniform->setUniform(vl::mat4::getTranslation(center));
		
		if(dynamic_cast< vl::TranslationManipulator* >(mTransformManipulatorX.get()) != NULL)
			createTranslationManipulators();
		else if(dynamic_cast< vl::RotationManipulator* >(mTransformManipulatorX.get()) != NULL)
			createRotationManipulators();
		else if(dynamic_cast< vl::ScaleManipulator* >(mTransformManipulatorX.get()) != NULL)
			createScaleManipulators();
	  }
  }
  
  vl::ref< vl::Uniform > mViewMatrixUniform;
  vl::ref< vl::Uniform > mProjectionMatrixUniform;
  vl::ref< vl::Uniform > mCubeModelMatrixUniform;
  vl::ref< vl::Uniform > mPyramidModelMatrixUniform;
  vl::ref< vl::Uniform > mCurrentModelMatrixUniform;
  vl::ref< vl::TransformManipulator > mTransformManipulatorX;
  vl::ref< vl::TransformManipulator > mTransformManipulatorY;
  vl::ref< vl::TransformManipulator > mTransformManipulatorZ;
  vl::ref< vl::Geometry > mTransformManipulatorGeometryX;
  vl::ref< vl::Geometry > mTransformManipulatorGeometryY;
  vl::ref< vl::Geometry > mTransformManipulatorGeometryZ;
  vl::ref< vl::Effect > mEffectX;
  vl::ref< vl::Effect > mEffectY;
  vl::ref< vl::Effect > mEffectZ;
  vl::ref< vl::Effect > mEffectBox;
  vl::ref< vl::Effect > mEffectPyramid;
  vl::ref< vl::Effect > mCurrentEffect;
  vl::ref< vl::Actor > mTransformManipulatorActorX;
  vl::ref< vl::Actor > mTransformManipulatorActorY;
  vl::ref< vl::Actor > mTransformManipulatorActorZ;
  vl::ref< vl::Geometry > mCubeGeometry;
  vl::ref< vl::Geometry > mPyramidGeometry;
  vl::ref< vl::Geometry > mCurrentGeometry;
  vl::ref< vl::Uniform > mProjectionGizmoMatrix;
  vl::ref< vl::Uniform > mViewMatrixGizmoMatrix;
  vl::ref< vl::Uniform > mManipulatorPositionMatrixUniform;
};

// Have fun!

BaseDemo* Create_App_TransformManipulator() { return new App_TransformManipulator(); }
