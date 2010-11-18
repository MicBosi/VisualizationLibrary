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

#include <vl/RenderingAbstract.hpp>
#include <vl/RenderingCallback.hpp>

using namespace vl;

//------------------------------------------------------------------------------
RenderingAbstract::RenderingAbstract()
{
  mUpdateTime = 0.0f;
  mEnableMask = 0xFFFFFFFF;
  mRenderingCallbacks = new Collection<RenderingCallback>;
}
//------------------------------------------------------------------------------
RenderingAbstract& RenderingAbstract::operator=(const RenderingAbstract& other)
{
  Object::operator=(other);

  mUpdateTime          = other.mUpdateTime;
  mEnableMask          = other.mEnableMask;
  *mRenderingCallbacks = *other.mRenderingCallbacks;

  return *this;
}

//------------------------------------------------------------------------------
void RenderingAbstract::dispatchRenderingCallbacks(ERenderingCallback reason)
{
  // iterate on a copy so that we can perform callback removal if requested
  const Collection<RenderingCallback>& cb = *mRenderingCallbacks;
  for(int i=0; i<cb.size(); ++i)
  {
    if ( cb[i]->renderingCallback(this, reason) && cb[i]->removeAfterCall() )
    {
      renderingCallbacks()->eraseAt( i );
      --i;
    }
  }
}
//------------------------------------------------------------------------------
