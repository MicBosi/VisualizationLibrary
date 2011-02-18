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

#include <vlGraphics/RenderStateSet.hpp>
#include <vlGraphics/RenderState.hpp>
#include <vlGraphics/GLSL.hpp>

using namespace vl;

//------------------------------------------------------------------------------
void RenderStateSet::setRenderState(RenderState* renderstate)
{
  if (renderstate)
  {
    if (renderstate->type() == RS_GLSLProgram)
      mGLSLProgram = dynamic_cast<GLSLProgram*>(renderstate);
    for(unsigned i=0; i<mRenderStates.size(); ++i)
    {
      if (mRenderStates[i]->type() == renderstate->type())
      {
        mRenderStates[i] = renderstate;
        return;
      }
    }
    mRenderStates.push_back( renderstate );
  }
}
//------------------------------------------------------------------------------
RenderState* RenderStateSet::renderState( ERenderState type )
{
  for(unsigned i=0; i<mRenderStates.size(); ++i)
    if ( mRenderStates[i]->type() == type )
      return mRenderStates[i].get();
  return NULL;
}
//------------------------------------------------------------------------------
const RenderState* RenderStateSet::renderState( ERenderState type ) const
{
  for(unsigned i=0; i<mRenderStates.size(); ++i)
    if ( mRenderStates[i]->type() == type )
      return mRenderStates[i].get();
  return NULL;
}
//------------------------------------------------------------------------------
void RenderStateSet::eraseRenderState(ERenderState type)
{
  if (type == RS_GLSLProgram)
    mGLSLProgram = NULL;
  for(unsigned i=0; i<mRenderStates.size(); ++i)
    if (mRenderStates[i]->type() == type)
    {
      mRenderStates.erase(mRenderStates.begin() + i);
      return;
    }
}
//------------------------------------------------------------------------------
