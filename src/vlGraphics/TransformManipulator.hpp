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

#ifndef TransformManipulator_INCLUDE_ONCE
#define TransformManipulator_INCLUDE_ONCE

#include <vlGraphics/UIEventListener.hpp>
#include <vlGraphics/Uniform.hpp>
#include <vlGraphics/Camera.hpp>
#include <vlGraphics/Actor.hpp>
#include <vlGraphics/Geometry.hpp>

namespace vl
{
	//------------------------------------------------------------------------------
    // TransformManipulator
    //------------------------------------------------------------------------------
	/**
     * Class that represents a Transform Manipulator associated to a visual gyzmo
     */
	class VLGRAPHICS_EXPORT TransformManipulator: public UIEventListener
	{
		VL_INSTRUMENT_ABSTRACT_CLASS(vl::TransformManipulator, UIEventListener);
		
		public:
			/** Axe constraint manipulation */
			enum Constraint
			{
				NO_CONSTRAINT = 0,
				CONSTRAINT_X = 1,
				CONSTRAINT_Y = 2,
				CONSTRAINT_Z = 4
			};
		
		public:
			/** Constructor */
			TransformManipulator() :
				mActive(false),
				mConstraint(NO_CONSTRAINT)
			{
				VL_DEBUG_SET_OBJECT_NAME()
			}
			
			/** Destructor */
			~TransformManipulator()
			{
			}
			
			void setModelMatrixUniform(Uniform* modelMatrixUniform) { mModelMatrixUniform = modelMatrixUniform; }
			
			void setViewMatrixUniform(Uniform* viewMatrixUniform) { mViewMatrixUniform = viewMatrixUniform; }
			
			void setProjectionMatrixUniform(Uniform* projectionMatrixUniform) { mProjectionMatrixUniform = projectionMatrixUniform; }
			
			void setManipulatorActor(Actor* manipulatorActor) { mManipulatorActor = manipulatorActor; }
			
			void setCamera(Camera* camera) { mCamera = camera; }
						
			void setModelMatrix(mat4 modelMatrix) { mModelMatrix = modelMatrix; }
			
			void setEventListener( UIEventListener* eventListener) { mEventListener = eventListener; }
			
			void setConstraint(short int constraint) { mConstraint = constraint; }
			
			void setEditGeometry(Geometry* editGeometry, vl::Uniform* modelMatrixUniform);
			
			vl::mat4 oldModelMatrix() const { return mOldModelMatrix; }
			
			Ray createRay(int x, int y, const vl::mat4& viewMatrix, const vl::mat4& projectionMatrix);

			bool unprojectFromUniform(const vl::vec3& in, vl::vec4& out, const vl::mat4& viewMatrix, const vl::mat4& projectionMatrix);
			
			// --- UIEventListener ---
			
			virtual void mouseDownEvent(vl::EMouseButton button, int x, int y);

			virtual void mouseUpEvent(vl::EMouseButton button, int x, int y);

                        virtual void enableEvent(bool enabled) {}

			virtual void initEvent() {}

			virtual void destroyEvent() {}

                        virtual void updateEvent() {}

			virtual void addedListenerEvent(vl::OpenGLContext*) {}

			virtual void removedListenerEvent(vl::OpenGLContext*) {}

			virtual void mouseWheelEvent(int) {}

			virtual void keyPressEvent(unsigned short, vl::EKey) {}

			virtual void keyReleaseEvent(unsigned short, vl::EKey) {}

			virtual void resizeEvent(int, int) {}

			virtual void fileDroppedEvent(const std::vector< vl::String>& ) {}

			virtual void visibilityEvent(bool) {}
			
		protected:
			bool mActive;
			ref< Uniform > mModelMatrixUniform;
			ref< Uniform > mViewMatrixUniform;
			ref< Uniform > mProjectionMatrixUniform;
			vec2 mStartPosition;
			ref< Actor > mManipulatorActor;
			ref< Camera > mCamera;
			mat4 mModelMatrix;
			ref< UIEventListener > mEventListener;
			short int mConstraint;
			ref< Geometry > mEditGeometry;	// To update real object
			vec3 mFirstIntersection;
			mat4 mOldModelMatrix;
	};
	
	//------------------------------------------------------------------------------
    // TranslationManipulator
    //------------------------------------------------------------------------------
	/**
	 * Class that represent a Translation manipulator
	 */
	class VLGRAPHICS_EXPORT TranslationManipulator: public TransformManipulator
	{
		VL_INSTRUMENT_ABSTRACT_CLASS(vl::TranslationManipulator, TransformManipulator);
		
		public:
		 /** Constructor */
		 TranslationManipulator() :
			TransformManipulator()
		{}
		 
		 /** Destructor*/
		 ~TranslationManipulator() {}
		 
		 // --- UIEventListener ---
		 virtual void mouseMoveEvent(int x, int y);
	};
	
	//------------------------------------------------------------------------------
    // RotationManipulator
    //------------------------------------------------------------------------------
	/**
	 * Class that represet a Rotation Manipulator
	 */
	class VLGRAPHICS_EXPORT RotationManipulator: public TransformManipulator
	{
		VL_INSTRUMENT_ABSTRACT_CLASS(vl::RotationManipulator, TransformManipulator);
		
		public:
		 /** Constructor */
		 RotationManipulator() :
			TransformManipulator()
		{}
		 
		 /** Destructor*/
		 ~RotationManipulator() {}
		 
		 // --- UIEventListener ---
		 virtual void mouseMoveEvent(int x, int y);
	};
	
	//------------------------------------------------------------------------------
    // ScaleManipulator
    //------------------------------------------------------------------------------
	/**
	 * Class that represent a Scale Manipulator
	 */
	class VLGRAPHICS_EXPORT ScaleManipulator: public TransformManipulator
	{
		VL_INSTRUMENT_ABSTRACT_CLASS(vl::ScaleManipulator, TransformManipulator);
		
		public:
		 /** Constructor */
		 ScaleManipulator() :
			TransformManipulator()
		{}
		 
		 /** Destructor*/
		 ~ScaleManipulator() {}
		 
		 // --- UIEventListener ---
		 virtual void mouseMoveEvent(int x, int y);
	};
}

#endif
