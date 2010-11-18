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

#ifndef RenderingAbstract_INCLUDE_ONCE
#define RenderingAbstract_INCLUDE_ONCE

#include <vl/Collection.hpp>
#include <vl/vlnamespace.hpp>

namespace vl
{
  class RenderingCallback;

  //! The RenderingAbstract class implements the abstract rendering interface.
  class RenderingAbstract: public Object
  {
  public:
    //! Constructor.
    RenderingAbstract();

    //! Executes the rendering.
    //! - RC_PreRendering RenderingCallback is issued right before the rendering takes place.
    //! - RC_PostRendering RenderingCallback is issued right after the rendering is completed.
    virtual void render() = 0;

    RenderingAbstract& operator=(const RenderingAbstract& other);

    //! Triggers the execution of the rendering callbacks with the given reason.
    void dispatchRenderingCallbacks(ERenderingCallback reason);
    //! Returns the list of RenderingCallback objects bound to a Rendering
    Collection<RenderingCallback>* renderingCallbacks() { return mRenderingCallbacks.get(); }
    //! Returns the list of RenderingCallback objects bound to a Rendering
    const Collection<RenderingCallback>* renderingCallbacks() const { return mRenderingCallbacks.get(); }

    //! The enable mask of the Rendering, used to define wheter the rendering is enabled or not, and which objects should be rendered.
    //! @sa
    //! vl::Actor::setEnableMask()
    void setEnableMask(unsigned int mask) { mEnableMask = mask; }
    //! The enable mask of the Rendering, used to define wheter the rendering is enabled or not, and which objects should be rendered.
    unsigned int enableMask() const { return mEnableMask; }
    //! Utility function equivalent to \p "(mask & mEnableMask) != 0".
    bool isEnabled(unsigned int mask) { return (mask & mEnableMask) != 0; }

    //! The update time of the current rendering frame.
    void setUpdateTime(Real cur_time) { mUpdateTime = cur_time; }
    //! The update time of the current rendering frame.
    Real updateTime() const { return mUpdateTime; }

  protected:
    ref< Collection<RenderingCallback> > mRenderingCallbacks;
    Real mUpdateTime;
    unsigned int mEnableMask;
  };
}

#endif
