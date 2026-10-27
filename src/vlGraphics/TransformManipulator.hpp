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
        VL_INSTRUMENT_ABSTRACT_CLASS(vl::TransformManipulator, UIEventListener)
		
		public:
			/** Axe constraint manipulation */
            enum TransformManipulatorConstraint
			{
				NO_CONSTRAINT = 0,
				CONSTRAINT_X = 1,
				CONSTRAINT_Y = 2,
				CONSTRAINT_Z = 4
			};

            /** Position constraint */
            enum TransformManipulatorPosition
            {
                CENTER =0,
                CORNER_xyz,
                CORNER_xYz,
                CORNER_xYZ,
                CORNER_xyZ,
                CORNER_Xyz,
                CORNER_XYz,
                CORNER_XYZ,
                CORNER_XyZ,
                CENTER_x,
                CENTER_y,
                CENTER_z,
                CENTER_X,
                CENTER_Y,
                CENTER_Z,
            };
		
		public:
			/** Constructor */
            TransformManipulator() :vl::UIEventListener(),
				mActive(false),
                mConstraint(NO_CONSTRAINT),
                mPosition(CENTER),
                mWorkingButton (LeftButton)
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
			
            //vl::mat4 oldModelMatrix() const { return mOldModelMatrix; }
            vl::mat4 ModelMatrix() const { return mModelMatrix; }
			
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
            /**
             * @brief active
             * @return true if the transformmanipulator is active ie if the mouse move the corresponding geometry is modified
             */
            bool active() const;
            /**
             * @brief workingButton
             * @return the mouse button used to manipulate the geometry left, right or middle
             */
            vl::EMouseButton workingButton() const;
            /**
             * @brief setWorkingButton
             * @param workingButton define which mouse button is used to manipulate the geometry
             */
            void setWorkingButton(const vl::EMouseButton &workingButton);

            void reInit();
            short position() const;

            void setPosition(short position);

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
            short int mPosition;
			ref< Geometry > mEditGeometry;	// To update real object
			vec3 mFirstIntersection;
            /*!
             * \brief mWorkingButton define the button used to select and move manipulator
             */
            vl::EMouseButton mWorkingButton;
            /**
             * \brief mOneTransformManipulatorIsActive indicate if there is already a transformmanipulator that has been activated
             * to avoid activate several at the same time (necessary to be able to move seperately two geometry that have the same bounding box)
             */
            static bool mOneTransformManipulatorIsActive;
            //mat4 mOldModelMatrix;
    private:
            void vecteurPosition(vl::vec3 &aVecPos);
    };
	
	//------------------------------------------------------------------------------
    // TranslationManipulator
    //------------------------------------------------------------------------------
	/**
	 * Class that represent a Translation manipulator
	 */
	class VLGRAPHICS_EXPORT TranslationManipulator: public TransformManipulator
	{
        VL_INSTRUMENT_ABSTRACT_CLASS(vl::TranslationManipulator, TransformManipulator)
		
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
        VL_INSTRUMENT_ABSTRACT_CLASS(vl::RotationManipulator, TransformManipulator)
		
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
        VL_INSTRUMENT_ABSTRACT_CLASS(vl::ScaleManipulator, TransformManipulator)
		
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
