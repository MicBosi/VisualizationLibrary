/**************************************************************************************/
/*                                                                                    */
/*  Visualization Library                                                             */
/*  http://visualizationlibrary.org                                                   */
/*                                                                                    */
/*  Copyright (c) 2005-2020, Michele Bosi                                             */
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

#ifndef OpenGL_INCLUDE_ONCE
#define OpenGL_INCLUDE_ONCE

#include <vlGraphics/link_config.hpp>
#include <vlCore/OpenGLDefs.hpp>
#include <vlCore/Log.hpp>

namespace vl
{
  //! Set to \a true if the last call to vl::initializeOpenGL() was succesful.
  VLGRAPHICS_EXPORT extern bool Is_OpenGL_Initialized;

  //! OpenGL: true if the current context has been created with the WGL_CONTEXT_CORE_PROFILE_BIT_ARB or equivalent.
  //!         If true all removed functionalities are not present.
  //! OpenGL ES 1: always false
  //! OpenGL ES 2: always false
  VLGRAPHICS_EXPORT extern bool Is_OpenGL_Core_Profile;

  //! OpenGL: true if the current context has been created with the WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB or equivalent.
  //!         If true all removed AND deprecated (even if not yet removed) functionalities are not present.
  //! OpenGL ES 1: always false
  //! OpenGL ES 2: always false
  VLGRAPHICS_EXPORT extern bool Is_OpenGL_Forward_Compatible;

  //! OpenGL: true if !Is_OpenGL_Forward_Compatible && !Is_OpenGL_Core_Profile
  //! OpenGL ES 1: always true
  //! OpenGL ES 2: always false
  VLGRAPHICS_EXPORT extern bool Has_Fixed_Function_Pipeline;

  VLGRAPHICS_EXPORT extern const GLenum Translate_Enable[];
  VLGRAPHICS_EXPORT extern bool Is_Enable_Supported[];
  VLGRAPHICS_EXPORT extern const char* Translate_Enable_String[];

  VLGRAPHICS_EXPORT extern bool Has_GL_Version_1_1;
  VLGRAPHICS_EXPORT extern bool Has_GL_Version_1_2;
  VLGRAPHICS_EXPORT extern bool Has_GL_Version_1_3;
  VLGRAPHICS_EXPORT extern bool Has_GL_Version_1_4;
  VLGRAPHICS_EXPORT extern bool Has_GL_Version_1_5;
  VLGRAPHICS_EXPORT extern bool Has_GL_Version_2_0;
  VLGRAPHICS_EXPORT extern bool Has_GL_Version_2_1;
  VLGRAPHICS_EXPORT extern bool Has_GL_Version_3_0;
  VLGRAPHICS_EXPORT extern bool Has_GL_Version_3_1;
  VLGRAPHICS_EXPORT extern bool Has_GL_Version_3_2;
  VLGRAPHICS_EXPORT extern bool Has_GL_Version_3_3;
  VLGRAPHICS_EXPORT extern bool Has_GL_Version_4_0;
  VLGRAPHICS_EXPORT extern bool Has_GL_Version_4_1;
  VLGRAPHICS_EXPORT extern bool Has_GL_Version_4_2;
  VLGRAPHICS_EXPORT extern bool Has_GL_Version_4_3;
  VLGRAPHICS_EXPORT extern bool Has_GL_Version_4_4;
  VLGRAPHICS_EXPORT extern bool Has_GL_Version_4_5;
  VLGRAPHICS_EXPORT extern bool Has_GL_Version_4_6;

  // Helper variables

  VLGRAPHICS_EXPORT extern bool Has_GLSL;
  VLGRAPHICS_EXPORT extern bool Has_GLSL_120_Or_More;
  VLGRAPHICS_EXPORT extern bool Has_GLSL_130_Or_More;
  VLGRAPHICS_EXPORT extern bool Has_GLSL_140_Or_More;
  VLGRAPHICS_EXPORT extern bool Has_GLSL_150_Or_More;
  VLGRAPHICS_EXPORT extern bool Has_GLSL_330_Or_More;
  VLGRAPHICS_EXPORT extern bool Has_GLSL_400_Or_More;
  VLGRAPHICS_EXPORT extern bool Has_GLSL_410_Or_More;
  VLGRAPHICS_EXPORT extern bool Has_Geometry_Shader;
  VLGRAPHICS_EXPORT extern bool Has_BufferObject;
  VLGRAPHICS_EXPORT extern bool Has_FBO;
  VLGRAPHICS_EXPORT extern bool Has_PBO;
  VLGRAPHICS_EXPORT extern bool Has_FBO_Multisample;
  VLGRAPHICS_EXPORT extern bool Has_Cubemap_Textures;
  VLGRAPHICS_EXPORT extern bool Has_Texture_Rectangle;
  VLGRAPHICS_EXPORT extern bool Has_Texture_Array;
  VLGRAPHICS_EXPORT extern bool Has_Texture_Buffer;
  VLGRAPHICS_EXPORT extern bool Has_Texture_Multisample;
  VLGRAPHICS_EXPORT extern bool Has_Texture_3D;
  VLGRAPHICS_EXPORT extern bool Has_Multitexture;
  VLGRAPHICS_EXPORT extern bool Has_Primitive_Restart;
  VLGRAPHICS_EXPORT extern bool Has_Occlusion_Query;
  VLGRAPHICS_EXPORT extern bool Has_Transform_Feedback;
  VLGRAPHICS_EXPORT extern bool Has_glGenerateMipmaps;
  VLGRAPHICS_EXPORT extern bool Has_GL_GENERATE_MIPMAP;
  VLGRAPHICS_EXPORT extern bool Has_Point_Sprite;
  VLGRAPHICS_EXPORT extern bool Has_Base_Vertex;
  VLGRAPHICS_EXPORT extern bool Has_Primitive_Instancing;

  #define VL_EXTENSION(extension) VLGRAPHICS_EXPORT extern bool Has_##extension;
  #include <vlGraphics/GL/GLExtensionList.hpp>
  #undef VL_EXTENSION

  #if defined(VL_OPENGL)

  //-----------------------------------------------------------------------------
  // Globally accessible OpenGL functions
  //-----------------------------------------------------------------------------
  #define VL_GL_FUNCTION(TYPE, NAME) VLGRAPHICS_EXPORT extern TYPE NAME;
  #include <vlGraphics/GL/GLFunctionList.hpp>
  #undef VL_GL_FUNCTION

  //-----------------------------------------------------------------------------
  // OpenGLFunctions
  //-----------------------------------------------------------------------------
  class OpenGLFunctions {
  public:
    // OpenGL 1.1
    #define VL_GL_FUNCTION(NAME) decltype(NAME)* _##NAME = 0;
    #include <vlGraphics/GL/GLFunctionList_1_1.hpp>
    #undef VL_GL_FUNCTION

    // OpenGL 1.2 - 4.6
    #define VL_GL_FUNCTION(TYPE, NAME) TYPE _##NAME = 0;
    #include <vlGraphics/GL/GLFunctionList.hpp>
    #undef VL_GL_FUNCTION
    void initFunctions();
  };
#endif

  //-----------------------------------------------------------------------------
  // MISC
  //-----------------------------------------------------------------------------

  //! To test whether OpenGL has been initialized at least once check vl::Is_OpenGL_Initialized
  VLGRAPHICS_EXPORT bool initializeOpenGL(const OpenGLFunctions* gl = NULL);

  //! Returns the address of the specified OpenGL function if supported by the active OpenGL driver and profile.
  VLGRAPHICS_EXPORT void* getGLProcAddress(const char* name);

  //! Returns a readable string corresponding to the given OpenGL error code as returned by glGetError()
  VLGRAPHICS_EXPORT const char* getGLErrorString(int err);

  VLGRAPHICS_EXPORT std::string getOpenGLExtensions(const OpenGLFunctions* gl);
  
  //-----------------------------------------------------------------------------
  // VL_CHECK_OGL
  //-----------------------------------------------------------------------------

  VLGRAPHICS_EXPORT int glcheck( const char* file, int line );

  // Kind of brutal but all files including this one will have this warning disabled since VL_CHECK_OGL seem to trigger it everywhere
  #ifdef __GNUC__
  #pragma GCC diagnostic ignored "-Wmisleading-indentation"
  #endif
  #if defined( _DEBUG ) || !defined( NDEBUG ) || VL_FORCE_CHECKS == 1
    #define VL_CHECK_OGL( ) { if ( ::vl::glcheck( __FILE__, __LINE__ ) ) { VL_TRAP( ) } }
  #else
    #define VL_CHECK_OGL( );
  #endif

  //------------------------------------------------------------------------------

}
//-----------------------------------------------------------------------------
#endif
