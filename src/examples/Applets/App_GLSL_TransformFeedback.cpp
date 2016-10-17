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
#include <vlGraphics/Geometry.hpp>
#include <vlGraphics/GLSL.hpp>
#include <vlGraphics/GeometryPrimitives.hpp>

#include <vlGraphics/OpenGLContext.hpp>
//#include <3rdparty/Khronos/GL/khronos_glext.h>

using namespace vl;

class App_GLSL_TransformFeedback: public BaseDemo
{
public:
  App_GLSL_TransformFeedback()
  {
  }

  void initEvent()
  {
    if (!Has_GLSL)
    {
      Log::error("OpenGL Shading Language not supported.\n");
      Time::sleep(2000);
      exit(1);
    }
	
	if(!Has_Transform_Feedback)
	{
	  Log::error("Transform Feedback not supported.\n");
	  Time::sleep(2000);
	  exit(1);
	}

    trackball()->setPivot(vl::vec3(0.0f, 0.0f, 0.0f));

    Log::notify(appletInfo());
  }

  void updateScene()
  {
	
  }
  
  void keyReleaseEvent(unsigned short, EKey key)
  {
	if(key == Key_T)
	{
		ref<GLSLProgram> glsl = new GLSLProgram;

		ref<GLSLVertexShader> transformFeedback_vs = new GLSLVertexShader("/glsl/transform_feedback.vs");
		ref<GLSLGeometryShader> transformFeedback_gs = new GLSLGeometryShader("/glsl/transform_feedback.gs");
		glsl->attachShader(transformFeedback_vs.get());
		glsl->attachShader(transformFeedback_gs.get());
		glsl->bindAttribLocation(VA_Position, "VertexPosition");
		
		ref<Effect> fx = new Effect;
		fx->shader()->enable(EN_DEPTH_TEST);
		fx->shader()->enable(EN_RASTERIZER_DISCARD);
		fx->shader()->setRenderState(glsl.get());
		ref< TransformFeedback > tf = fx->shader()->gocTransformFeedback();
		fx->shader()->setRenderState(tf.get());
		glsl->setTransformFeedback(tf.get());
		std::list< String > varyings;
		varyings.push_back("normale");
		tf->setTransformFeedbackVaryings(varyings);
		
		normales = new ArrayInt1;
		normales->resize(6);
		
		
		
		tf->addArray(normales.get());
				
		tf->set(PT_POINTS);
		
		ref<Geometry> geom = new Geometry;
		geom->setObjectName("Pyramid");
		
		ref<ArrayFloat3> vert3 = new ArrayFloat3;
		geom->setVertexAttribArray(VA_Position, vert3.get());
		
		real x = 5.0f   / 2.0f;
		real y = 10.0f;
		real z = 5.0f   / 2.0f;
		
		fvec3 a0( (fvec3)(vec3(+0,+y,+0) + vec3(0.0f, 0.0f, 0.0f)) );
		fvec3 a1( (fvec3)(vec3(-x,+0,-z) + vec3(0.0f, 0.0f, 0.0f)) );
		fvec3 a2( (fvec3)(vec3(-x,-0,+z) + vec3(0.0f, 0.0f, 0.0f)) );
		fvec3 a3( (fvec3)(vec3(+x,-0,+z) + vec3(0.0f, 0.0f, 0.0f)) );
		fvec3 a4( (fvec3)(vec3(+x,+0,-z) + vec3(0.0f, 0.0f, 0.0f)) );
		
		ref<DrawArrays> polys = new DrawArrays(PT_TRIANGLES, 0, 6*3);
		geom->drawCalls().push_back( polys.get() );
		
		vert3->resize(6*3);
		
		vert3->at(0)  = a4; vert3->at(1)  = a2; vert3->at(2)  = a1; 
		vert3->at(3)  = a2; vert3->at(4)  = a4; vert3->at(5)  = a3; 
		vert3->at(6)  = a4; vert3->at(7)  = a1; vert3->at(8)  = a0; 
		vert3->at(9)  = a1; vert3->at(10) = a2; vert3->at(11) = a0;
		vert3->at(12) = a2; vert3->at(13) = a3; vert3->at(14) = a0;
		vert3->at(15) = a3; vert3->at(16) = a4; vert3->at(17) = a0;
		
		ref< Actor > actor = sceneManager()->tree()->addActor( geom.get(), fx.get(), NULL);
				
		rendering()->render();
		
		sceneManager()->tree()->eraseActor(actor.get());
		
		tf->updateArray(normales.get());
		
		for(unsigned int i = 0; i < normales->size(); ++i)
		{
			defLogger()->error("primitive " + String::fromUInt(i) + " : " + String::fromDouble(normales->at(i)) + "\n");
		}
	}
  }

protected:
	ref< ArrayInt1 > normales;
};

// Have fun!

BaseDemo* Create_App_GLSL_TransformFeedback() { return new App_GLSL_TransformFeedback; }
