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

#ifndef ProjViewTransfCallback_INCLUDE_ONCE
#define ProjViewTransfCallback_INCLUDE_ONCE

#include <vlCore/Object.hpp>
#include <vlGraphics/link_config.hpp>

namespace vl
{
  class Renderer;
  class GLSLProgram;
  class Transform;
  class Camera;
  //-----------------------------------------------------------------------------
  // ProjViewTransfCallback
  //-----------------------------------------------------------------------------
  /** Vitual class used as a callback to update the state of the \p projection, \p view and \p transform matrices of a GLSLProgram or fixed function pipeline. */
  class VLGRAPHICS_EXPORT ProjViewTransfCallback: public Object
  {
  public:
    virtual const char* className() { return "vl::ProjViewTransfCallback"; }

    /** This function is called whenever a new GLSLProgram (or the NULL one, i.e. the fixed function pipeline) is being activated for the first time in the current rendering.
     * This callback is most useful to initialize the GLSLProgram with the current projection and view matrices, besides the current Actor's transform.
     * \param caller The Renderer object calling this function.
     * \param glsl The GLSLProgram being activated. If NULL the fixed function pipeline is being activated.
     * \param transform The transform of the current Actor being rendered.
     * \param camera The Camera used for the rendering from which you can retrieve the projection and view matrices.
     * \param first_overall If \p true it means that the rendering has just started. Useful if you want to initialized your callback object.
     */
    virtual void programFirstUse(const Renderer* caller, const GLSLProgram* glsl, const Transform* transform, const Camera* camera, bool first_overall) = 0;
    /** This function is called whenever the Transform changes with respect to the current GLSLProgram (including the NULL one, i.e. the fixed function pipeline).
     * This callback is most useful to update the GLSLProgram with the current Actor's transform matrix.
     * \param caller The Renderer object calling this function.
     * \param glsl The GLSLProgram being activated. If NULL the fixed function pipeline is being activated.
     * \param transform The transform of the current Actor being rendered.
     * \param camera The Camera used for the rendering from which you can retrieve the projection and view matrices.
     */
    virtual void programTransfChange(const Renderer* caller, const GLSLProgram* glsl, const Transform* transform, const Camera* camera) = 0;
  };

  //-----------------------------------------------------------------------------
  // ProjViewTransfCallbackStandard
  //-----------------------------------------------------------------------------
  //! Updates the GL_MODELVIEW and GL_PROJECTION matrices of the fixed function pipeline in an optimized manner.
  //! You usually want to install this callback if the fixed fuction pipeline is available, even when using GLSL shaders.
  //! In fact the GL_MODELVIEW and GL_PROJECTION matrices are visible from all the GLSL shaders, thus requiring fewer matrix updates
  //! compared to being forced to send projection, view and transform matrix to every single GLSLProgram at least once during the rendering!
  class VLGRAPHICS_EXPORT ProjViewTransfCallbackStandard: public ProjViewTransfCallback
  {
  public:
    virtual const char* className() { return "vl::ProjViewTransfCallbackStandard"; }
    
    ProjViewTransfCallbackStandard(): mLastTransform(NULL) 
    {
      VL_DEBUG_SET_OBJECT_NAME()
    }
    
    virtual void programFirstUse(const Renderer*, const GLSLProgram* glsl, const Transform*, const Camera*, bool first_overall );
    
    virtual void programTransfChange(const Renderer*, const GLSLProgram* glsl, const Transform*, const Camera* );
  
  private:
    const Transform* mLastTransform;
  };
  //------------------------------------------------------------------------------
}

#endif
