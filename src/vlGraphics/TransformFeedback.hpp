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

#ifndef TransformFeedback_INCLUDE_ONCE
#define TransformFeedback_INCLUDE_ONCE

#include <vlCore/String.hpp>
#include <vlCore/Collection.hpp>
#include <vlGraphics/Array.hpp>
#include <vlGraphics/link_config.hpp>
#include <vlGraphics/RenderState.hpp>
#include <list>

namespace vl 
{
	class OpenGLContext;
	class Camera;
	//------------------------------------------------------------------------------
    // TransformFeedback
    //------------------------------------------------------------------------------
	/**
     * Class that represents a Transform Feedback associated to a shader
     */
	class VLGRAPHICS_EXPORT TransformFeedback: public RenderStateNonIndexed
	{
		VL_INSTRUMENT_ABSTRACT_CLASS(vl::TransformFeedback, RenderStateNonIndexed);
		
		public:
			/** Constructor */
			TransformFeedback(EPrimitiveType type = PT_TRIANGLES) : mHandle(0), mHandleQuery(0), mType(type), mBufferMode(BM_SeparateAttribs), mReallocateTransformFeedback( true )
			{
				VL_DEBUG_SET_OBJECT_NAME()
			}
			
			/** Destructor */
			~TransformFeedback() 
			{
				if(mHandleQuery != 0)	// We have created a query
				{
					glDeleteQueries(1, &mHandleQuery);
				}
			}
			
			/**
			 * Creates a renderbuffer object calling glGenTransformFeedbacks(). 
			 * The identifier returned by glGenTransformFeedbacks() can be queried calling the handle() method.
			 */
			void createTransformFeedbacks();
			
			/** 
			 * Sets the handle for this transform feedback, the handle must have been created by glGenTransformFeedbacks().
			 * Normally you don't need to call this. See also: createTransformFeedbacks(), handle().
			 */
			void setHandle( GLuint  handle ) { if ( mHandle != handle ) { mHandle = handle; mReallocateTransformFeedback = false; } }
    
			/** Returns the handle obtained by createTransformFeedbacks() using glGenTransformFeedbacks() */
			GLuint handle() const { return mHandle; }
			
			/**
			 * Makes the transform feedback bound calling glBindTransformFeedback( GL_TRANSFORM_FEEDBACK, TransformFeedback::handle() )
			 * \sa http://www.opengl.org/sdk/docs/man4/xhtml/glBindTransformFeedback.xml
			 */
			void bindTransformFeedback() const;
			
			void beginTransformFeedback( ) const;
			
			void endTransformFeedback() const;
			
			void pauseTransformFeedback() const;
			
			void resumeTransformFeedback() const;
			
			void setTransformFeedbackVaryings(std::list< String >& varyings);
			
			unsigned int transformFeedbackVaryingsCount() const { return mVaryingsCount; }
			
			void setTransformFeedbackBufferMode(EBufferMode bufferMode) { mBufferMode = bufferMode; }

			EBufferMode bufferMode() const { return mBufferMode; }
			
			char** transformFeedbackVaryings() const;
						
			virtual ERenderState type() const { return RS_TransformFeedback; }
    
			virtual void apply(int, const Camera*, OpenGLContext*) const 
			{
				if(mVBOsOut.empty())
				{
					VL_glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);
					glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, 0);
				}
				else
				{
					for(int i = 0; i < mVBOsOut.size(); ++i)
					{
						if(mVBOsOut[i]->bufferObject()->handle())
						{
							VL_glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, mVBOsOut[i]->bufferObject()->handle());
							glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, mVBOsOut[i]->bufferObject()->handle());
						}
					}
				}
			}
			
			/*template <typename T_VectorType, typename T_Scalar, size_t T_GL_Size, GLenum T_GL_Type>
			void addArray(Array< T_VectorType, T_Scalar, T_GL_Size, T_GL_Type >* array, int size) */
			void addArray(ArrayAbstract* array)
			{ 
				array->bufferObject()->createBufferObject();	// Necessary to create buffer to send to transform feedback
				glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, array->bufferObject()->handle());
				glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, array->bytesUsed(), NULL, GL_DYNAMIC_COPY);	// Have to put size of the data
				glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);
				mVBOsOut.push_back(array); 
			}
			
			/*template <typename T_VectorType, typename T_Scalar, size_t T_GL_Size, GLenum T_GL_Type>
			void updateArray(Array< T_VectorType, T_Scalar, T_GL_Size, T_GL_Type >* array, int size) */
			void updateArray(ArrayAbstract* array)
			{
				glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, array->bufferObject()->handle());
				glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, array->bytesUsed(), array->ptr());
				glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);
			}
			
			void updateArrays()
			{
				for(int i = 0; i < mVBOsOut.size(); ++i)
				{
					updateArray(mVBOsOut[i].get());
				}
			}
			
			void queryWrittenPrimitive()
			{
				glGenQueries(1, &mHandleQuery);
			}
			
			unsigned int getWrittenPrimitives()
			{
				unsigned int writtenPrimitives;
				glGetQueryObjectuiv(mHandleQuery, GL_QUERY_RESULT, &writtenPrimitives);
				return writtenPrimitives;
			}
			
			void set(EPrimitiveType type) { mType = type; }
		   
			virtual ref<RenderState> clone() const
			{
			  ref<TransformFeedback> rs = new TransformFeedback(mType);
			  *rs = *this;
			  return rs;
			}
		
		private:
			GLuint mHandle;
			GLuint mHandleQuery;
			EPrimitiveType mType;
			EBufferMode mBufferMode;
			bool mReallocateTransformFeedback;
			char** mVaryings;
			unsigned int mVaryingsCount;
			Collection< ArrayAbstract > mVBOsOut;
	};

}

#endif