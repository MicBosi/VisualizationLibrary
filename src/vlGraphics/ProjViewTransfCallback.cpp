/**************************************************************************************/
/*                                                                                    */
/*  Visualization Library                                                             */
/*  http://visualizationlibrary.org                                                   */
/*                                                                                    */
/*  Copyright (c) 2005-2011, Michele Bosi                                             */
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

#include <vlGraphics/ProjViewTransfCallback.hpp>
#include <vlCore/Transform.hpp>
#include <vlGraphics/Camera.hpp>
#include <vlGraphics/GLSL.hpp>

using namespace vl;

//------------------------------------------------------------------------------
// ProjViewTransfCallbackStandard
//------------------------------------------------------------------------------
void ProjViewTransfCallback::updateMatrices(bool cam_changed, bool transf_changed, const GLSLProgram* glsl_program, const Camera* camera, const Transform* transform)
{
  VL_CHECK_OGL();
  // Once you opt-in for using VL substitutes for matrix variables you should not use the GL fixed-function ones such as:
  // gl_ModelViewMatrix, gl_ProjectionMatrix, gl_ModelViewProjectionMatrix and gl_NormalMatrix
  // see http://www.opengl.org/registry/doc/GLSLangSpec.Full.1.10.59.pdf pag 45

  // projection matrix
  if ( cam_changed )
  {
    if ( glsl_program && glsl_program->vl_ProjectionMatrix() != -1 )
    {
#if VL_PIPELINE_PRECISION == 1
      glUniformMatrix4fv( glsl_program->vl_ProjectionMatrix(), 1, GL_FALSE, camera->projectionMatrix().ptr() ); VL_CHECK_OGL();
#elif VL_PIPELINE_PRECISION == 2
      glUniformMatrix4fv( glsl_program->vl_ProjectionMatrix(), 1, GL_FALSE, ((fmat4)camera->projectionMatrix()).ptr() ); VL_CHECK_OGL();
#endif
    }

    if ( Has_Fixed_Function_Pipeline )
    {
      // this updates 
      glMatrixMode( GL_PROJECTION ); VL_CHECK_OGL();
      VL_glLoadMatrix( camera->projectionMatrix().ptr() ); VL_CHECK_OGL();
    }
  }

  // model + view transform
  if ( cam_changed || transf_changed )
  {
    // compute the modelview matrix to send to GL
    mat4 modelview;
    if ( transform )
      modelview = camera->viewMatrix() * transform->worldMatrix();
    else
      modelview = camera->viewMatrix();

    if ( glsl_program )
    {
      // update vl_ModelViewMatrix if used
      if ( glsl_program->vl_ModelViewMatrix() != -1 )
      {
#if VL_PIPELINE_PRECISION == 1
        glUniformMatrix4fv( glsl_program->vl_ModelViewMatrix(), 1, GL_FALSE, modelview.ptr() ); VL_CHECK_OGL();
#elif VL_PIPELINE_PRECISION == 2
        glUniformMatrix4fv( glsl_program->vl_ModelViewMatrix(), 1, GL_FALSE, ((fmat4)modelview).ptr() ); VL_CHECK_OGL();
#endif
      }

      // update vl_ModelViewProjectionMatrix if used
      if ( glsl_program->vl_ModelViewProjectionMatrix() != -1 )
      {
#if VL_PIPELINE_PRECISION == 1
        glUniformMatrix4fv( glsl_program->vl_ModelViewProjectionMatrix(), 1, GL_FALSE, (camera->projectionMatrix() * modelview).ptr() ); VL_CHECK_OGL();
#elif VL_PIPELINE_PRECISION == 2
        glUniformMatrix4fv( glsl_program->vl_ModelViewProjectionMatrix(), 1, GL_FALSE, ((fmat4)(camera->projectionMatrix() * modelview)).ptr() ); VL_CHECK_OGL();
#endif
      }

      // update vl_NormalMatrix if used
      if ( glsl_program->vl_NormalMatrix() != -1 )
      {
        // transpose of the inverse of the upper leftmost 3x3 of vl_ModelViewMatrix
        mat3 normalmtx = modelview.get3x3().invert().transpose();
#if VL_PIPELINE_PRECISION == 1
        glUniformMatrix3fv( glsl_program->vl_NormalMatrix(), 1, GL_FALSE, normalmtx.ptr() ); VL_CHECK_OGL();
#elif VL_PIPELINE_PRECISION == 2
        glUniformMatrix3fv( glsl_program->vl_NormalMatrix(), 1, GL_FALSE, ((fmat3)normalmtx).ptr() ); VL_CHECK_OGL();
#endif
      }

      // update vl_WorldMatrix if used
      if ( transf_changed && glsl_program->vl_WorldMatrix() != - 1 ) {
        mat4 world_matrix = transform ? transform->worldMatrix() : mat4::getIdentity();
        #if VL_PIPELINE_PRECISION == 1
                glUniformMatrix4fv( glsl_program->vl_WorldMatrix(), 1, GL_FALSE, world_matrix.ptr() ); VL_CHECK_OGL();
        #elif VL_PIPELINE_PRECISION == 2
                glUniformMatrix4fv( glsl_program->vl_WorldMatrix(), 1, GL_FALSE, ((fmat4)world_matrix).ptr() ); VL_CHECK_OGL();
        #endif
      }
    }

    if( Has_Fixed_Function_Pipeline )
    {
      glMatrixMode( GL_MODELVIEW ); VL_CHECK_OGL();
      VL_glLoadMatrix( modelview.ptr() ); VL_CHECK_OGL();
    }
  }
}
//------------------------------------------------------------------------------
