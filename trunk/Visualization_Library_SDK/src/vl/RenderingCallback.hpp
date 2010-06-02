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

#ifndef RenderingCallback_INCLUDE_ONCE
#define RenderingCallback_INCLUDE_ONCE

#include <vl/Object.hpp>
#include <vl/vlnamespace.hpp>

namespace vl
{
  //-----------------------------------------------------------------------------
  // RenderingCallback
  //-----------------------------------------------------------------------------
  class RenderingAbstract;
  /**
   * An abstract class used to perform operations at the end or at the beginning of a rendering.
   * Install a callback to react to the following two events: vl::RC_PreRendering and vl::RC_PostRendering.
   * The callbacks are executed in the same order in which they appear in the vl::Rendering::renderingCallbacks() collection.
   * For more information see:
   * - vl::Rendering::renderingCallbacks()
   * - vl::Rendering
  */
  class RenderingCallback: public Object
  {
  public:
    virtual const char* className() { return "RenderingCallback"; }
    RenderingCallback(): mRank(0.0f), mRemoveAfterCall(false)
    {
      #ifndef NDEBUG
        mName = className();
      #endif
    }

    //! Implements the actual code to be executed upon callback activation.
    //! \return \p true if the callback reacted to the given reason
    virtual bool renderingCallback(const RenderingAbstract* rendering, ERenderingCallback reason) = 0;

    //! Defines if the callback shall be removed after being executed.
    //! Note that the callback is removed only if the renderingCallback() method returns \p true.
    void setRemoveAfterCall(bool remove) { mRemoveAfterCall = remove; }
    //! Defines if the callback shall be removed after being executed.
    //! Note that the callback is removed only if the renderingCallback() method returns \p true.
    bool removeAfterCall() const { return mRemoveAfterCall; }

  protected:
    float mRank;
    bool mRemoveAfterCall;
  };
}

#endif
