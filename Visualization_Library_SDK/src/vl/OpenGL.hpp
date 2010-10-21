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

#ifndef OpenGL_INCLUDE_ONCE
#define OpenGL_INCLUDE_ONCE

#include <vl/checks.hpp>

//-----------------------------------------------------------------------------

#define GLEW_STATIC
#include "GL/glew.h"
#ifdef _WIN32
  #ifndef NOMINMAX
    #define NOMINMAX
  #endif
  #include "GL/wglew.h"
#endif

namespace vl
{
  //-----------------------------------------------------------------------------
  int glcheck( const char* file, int line );

  #if defined( _DEBUG ) || !defined( NDEBUG ) || VL_FORCE_CHECKS == 1

    #define VL_CHECK_OGL( ) { if ( ::vl::glcheck( __FILE__, __LINE__ ) ) { VL_TRAP( ) } }

  #else

    #define VL_CHECK_OGL( );

  #endif
}

void VL_glBindBuffer( GLenum target, GLuint buffer );
void VL_glGenBuffers( GLsizei n, GLuint * buffers );
void VL_glDeleteBuffers( GLsizei n, const GLuint * buffers );
void VL_glBufferData( GLenum target, GLsizeiptr size, const GLvoid * data, GLenum usage );
void VL_glBufferSubData( GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid * data );
void* VL_glMapBuffer( GLenum target, GLenum access );
GLboolean VL_glUnmapBuffer( GLenum target );

void VL_glSecondaryColor3f( float, float, float );
void VL_glSecondaryColorPointer( GLint size, GLenum type, GLsizei stride, const GLvoid* pointer );
void VL_glFogCoordPointer( GLenum type,GLsizei stride,GLvoid* pointer );

void VL_glEnableVertexAttribArray( GLuint index );
void VL_glDisableVertexAttribArray( GLuint index );

void VL_glVertexAttribPointer( GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid * pointer );
void VL_glVertexAttribIPointer( GLuint name, GLint size, GLenum type, GLsizei stride, const GLvoid* pointer );

void VL_glClientActiveTexture( GLenum texture );
void VL_glActiveTexture( GLenum texture );

void VL_glBlendFuncSeparate( GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha );
void VL_glBlendEquationSeparate( GLenum modeRGB, GLenum modeAlpha );
void VL_glBlendEquation( GLenum mode );
void VL_glBlendColor( GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha );

void VL_glPointParameterf( GLenum pname, GLfloat param );
void VL_glPointParameterfv( GLenum pname, const GLfloat* params );

void VL_glStencilFuncSeparate( GLenum face, GLenum func, GLint ref, GLuint mask );
void VL_glStencilOpSeparate( GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass );

void VL_glSampleCoverage( GLclampf value, GLboolean invert );

void VL_glBindRenderbuffer( GLenum target, GLuint renderbuffer );
void VL_glDeleteRenderbuffers( GLsizei n, const GLuint *renderbuffers );
void VL_glGenRenderbuffers( GLsizei n, GLuint *renderbuffers );
void VL_glRenderbufferStorage( GLenum target, GLenum internalformat, GLsizei width, GLsizei height );
void VL_glGetRenderbufferParameteriv( GLenum target, GLenum pname, GLint *params );
GLboolean VL_glIsFramebuffer( GLuint framebuffer );
void VL_glBindFramebuffer( GLenum target, GLuint framebuffer );
void VL_glDeleteFramebuffers( GLsizei n, const GLuint *framebuffers );
void VL_glGenFramebuffers( GLsizei n, GLuint *framebuffers );
GLenum VL_glCheckFramebufferStatus( GLenum target );
void VL_glFramebufferTexture1D( GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level );
void VL_glFramebufferTexture2D( GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level );
void VL_glFramebufferTexture3D( GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint zoffset );
void VL_glFramebufferRenderbuffer( GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer );
void VL_glGetFramebufferAttachmentParameteriv( GLenum target, GLenum attachment, GLenum pname, GLint *params );
void VL_glGenerateMipmap( GLenum target );

void VL_glFramebufferTexture( GLenum target, GLenum attachment, GLuint texture, GLint level );
void VL_glFramebufferTextureLayer( GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer );
void VL_glRenderbufferStorageMultisample( GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height );
void VL_glBlitFramebuffer(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);

void VL_glDrawArraysInstanced(GLenum mode, GLint first, GLsizei count, GLsizei primcount);
void VL_glDrawElementsInstanced(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices, GLsizei primcount);

void VL_glProgramParameteri(GLuint program, GLenum pname, GLint value);
void VL_glBindFragDataLocation(GLuint program, GLuint colorNumber, const GLchar *name);

void VL_glUniform1uiv(GLint location, GLsizei count, const GLuint *value);
void VL_glUniform2uiv(GLint location, GLsizei count, const GLuint *value);
void VL_glUniform3uiv(GLint location, GLsizei count, const GLuint *value);
void VL_glUniform4uiv(GLint location, GLsizei count, const GLuint *value);


#endif
