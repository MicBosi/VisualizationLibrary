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

#include <vlGraphics/TransformFeedback.hpp>
#include <vlGraphics/OpenGLContext.hpp>

using namespace vl;

void TransformFeedback::createTransformFeedbacks()
{
	glGenTransformFeedbacks(1, &mHandle);
}

void TransformFeedback::bindTransformFeedback() const
{
	VL_CHECK_OGL();
	
	if ( !Has_Transform_Feedback )
	{
		Log::error( "TransformFeedback::bindTransformFeedback(): transform feedback not supported.\n" );
		return;
	}
	if(handle() != 0)
	{
		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, TransformFeedback::handle());	VL_CHECK_OGL();
	}
}

void TransformFeedback::beginTransformFeedback( ) const
{
	
	if(mHandleQuery != 0)
		glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, mHandleQuery);
			
	glBeginTransformFeedback(mType);	VL_CHECK_OGL();
}

void TransformFeedback::endTransformFeedback() const
{
	glEndTransformFeedback();	
	
	if(mHandleQuery != 0)
		glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);
		
	VL_CHECK_OGL();
}
	
void TransformFeedback::pauseTransformFeedback() const
{
	glPauseTransformFeedback();	VL_CHECK_OGL();
}
	
void TransformFeedback::resumeTransformFeedback() const
{
	glResumeTransformFeedback();	VL_CHECK_OGL();
}

void TransformFeedback::setTransformFeedbackVaryings(std::list< String >& varyings)
{
	mVaryingsCount = varyings.size();
	mVaryings = new char*[varyings.size()];
	std::list< String >::iterator it;
	int i = 0;
	for(it = varyings.begin(); it != varyings.end(); ++it)
	{
		mVaryings[i] = new char[it->length()];
		std::memcpy(mVaryings[i++], it->toStdString().c_str(), it->length() + 1);
	}
}

char** TransformFeedback::transformFeedbackVaryings() const
{
	return mVaryings;
}
