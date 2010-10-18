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

#include <vl/OpenGL.hpp>

//-----------------------------------------------------------------------------
void VL_glBindBuffer( GLenum target, GLuint buffer )
{
  if (GLEW_VERSION_1_5)
    glBindBuffer(target,buffer);
  else
  if (GLEW_ARB_vertex_buffer_object)
    glBindBufferARB(target,buffer);
  else
  {
    VL_CHECK( buffer == 0 );
  }
}

void VL_glGenBuffers( GLsizei n, GLuint * buffers)
{
  if (GLEW_VERSION_1_5)
    glGenBuffers( n, buffers);
  else
  if (GLEW_ARB_vertex_buffer_object)
    glGenBuffersARB( n, buffers);
  else
    VL_TRAP();
}

void VL_glDeleteBuffers( GLsizei n, const GLuint * buffers)
{
  if (GLEW_VERSION_1_5)
    glDeleteBuffers( n, buffers);
  else
  if (GLEW_ARB_vertex_buffer_object)
    glDeleteBuffersARB( n, buffers);
  else
    VL_TRAP();
}

void VL_glBufferData( GLenum target, GLsizeiptr size, const GLvoid * data, GLenum usage)
{
  if (GLEW_VERSION_1_5)
    glBufferData( target, size, data, usage);
  else
  if (GLEW_ARB_vertex_buffer_object)
    glBufferDataARB( target, size, data, usage);
  else
    VL_TRAP();
}

void VL_glBufferSubData( GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid * data)
{
  if (GLEW_VERSION_1_5)
    glBufferSubData( target, offset, size, data );
  else
  if (GLEW_ARB_vertex_buffer_object)
    glBufferSubDataARB( target, offset, size, data);
  else
    VL_TRAP();
}

void* VL_glMapBuffer( GLenum target, GLenum access)
{
  if (GLEW_VERSION_1_5)
    return glMapBuffer( target, access);
  else
  if (GLEW_ARB_vertex_buffer_object)
    return glMapBufferARB( target, access);
  else
    VL_TRAP();
  return NULL;
}

GLboolean VL_glUnmapBuffer(GLenum target)
{
  if (GLEW_VERSION_1_5)
    return glUnmapBuffer( target );
  else
  if (GLEW_ARB_vertex_buffer_object)
    return glUnmapBufferARB( target );
  else
    VL_TRAP();
  return GL_FALSE;
}
//-----------------------------------------------------------------------------
void VL_glSecondaryColor3f(float r, float g, float b)
{
  if(GLEW_VERSION_1_4)
    glSecondaryColor3f(r,g,b);
  else
  if(GLEW_EXT_secondary_color)
    glSecondaryColor3fEXT(r,g,b);
  else
    VL_TRAP();
}

void VL_glSecondaryColorPointer( GLint size, GLenum type, GLsizei stride, const GLvoid* pointer)
{
  if(GLEW_VERSION_1_4)
    glSecondaryColorPointer(size, type, stride, (GLvoid*)pointer);
  else
  if(GLEW_EXT_secondary_color)
    glSecondaryColorPointerEXT(size, type, stride, (GLvoid*)pointer);
  else
    VL_TRAP();
}
//-----------------------------------------------------------------------------
void VL_glFogCoordPointer( GLenum type, GLsizei stride, GLvoid* pointer )
{
  if (GLEW_VERSION_1_4)
    glFogCoordPointer(type,stride,pointer);
  else
  if (GLEW_EXT_fog_coord)
    glFogCoordPointerEXT(type,stride,pointer);
  else
    VL_TRAP();
}
//-----------------------------------------------------------------------------
void VL_glEnableVertexAttribArray( GLuint index )
{
  if (GLEW_VERSION_2_0)
    glEnableVertexAttribArray(index);
  else
  if (GLEW_ARB_vertex_program)
    glEnableVertexAttribArrayARB(index);
  else
    VL_TRAP();
}

void VL_glDisableVertexAttribArray( GLuint index )
{
  if (GLEW_VERSION_2_0)
    glDisableVertexAttribArray(index);
  else
  if (GLEW_ARB_vertex_program)
    glDisableVertexAttribArrayARB(index);
  else
    VL_TRAP();
}

//-----------------------------------------------------------------------------

void VL_glVertexAttribPointer( GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid * pointer)
{
  if (GLEW_VERSION_2_0)
    glVertexAttribPointer(index, size, type, normalized, stride, pointer);
  else
  if (GLEW_ARB_vertex_program)
    glVertexAttribPointerARB(index, size, type, normalized, stride, pointer);
  else
    VL_TRAP();
}

void VL_glVertexAttribIPointer(GLuint name, GLint size, GLenum type, GLsizei stride, const GLvoid* pointer)
{
  if(GLEW_VERSION_3_0)
    glVertexAttribIPointer(name, size, type, stride, pointer);
  else
  if (GLEW_EXT_gpu_shader4)
    glVertexAttribIPointerEXT(name, size, type, stride, pointer);
  else
    VL_TRAP();
}
//-----------------------------------------------------------------------------
void VL_glClientActiveTexture(GLenum texture)
{
  if(GLEW_VERSION_1_3)
    glClientActiveTexture(texture);
  else
  if (GLEW_ARB_multitexture)
    glClientActiveTextureARB(texture);
  else
  {
    VL_CHECK(texture == GL_TEXTURE0);
  }
}

void VL_glActiveTexture(GLenum texture)
{
  if(GLEW_VERSION_1_3)
    glActiveTexture(texture);
  else
  if (GLEW_ARB_multitexture)
    glActiveTextureARB(texture);
  else
  {
    VL_CHECK(texture == GL_TEXTURE0);
  }
}
//-----------------------------------------------------------------------------
void VL_glBlendFuncSeparate( GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha)
{
  if (GLEW_VERSION_1_4)
    glBlendFuncSeparate( srcRGB, dstRGB, srcAlpha, dstAlpha);
  else
  if (GLEW_EXT_blend_func_separate)
    glBlendFuncSeparateEXT( srcRGB, dstRGB, srcAlpha, dstAlpha);
  else
    VL_TRAP();
}

void VL_glBlendEquationSeparate( GLenum modeRGB, GLenum modeAlpha)
{
  if (GLEW_VERSION_2_0)
    glBlendEquationSeparate(modeRGB, modeAlpha);
  else
  if(GLEW_EXT_blend_equation_separate)
    glBlendEquationSeparateEXT(modeRGB, modeAlpha);
  else
    VL_TRAP();
}


void VL_glBlendEquation(GLenum mode)
{
  if (GLEW_VERSION_1_4)
    glBlendEquation(mode);
  else
  if (GLEW_EXT_blend_minmax)
    glBlendEquationEXT(mode);
  else
    VL_TRAP();
}

void VL_glBlendColor( GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
  if(GLEW_VERSION_1_4)
    glBlendColor(red,green,blue,alpha);
  else
  if (GLEW_EXT_blend_color)
    glBlendColorEXT(red,green,blue,alpha);
  else
    VL_TRAP();
}

//-----------------------------------------------------------------------------
void VL_glPointParameterfv( GLenum pname, const GLfloat* params)
{
  if (GLEW_VERSION_1_4)
    glPointParameterfv(pname,(GLfloat*)params);
  else
  if (GLEW_ARB_point_parameters)
    glPointParameterfvARB(pname,(GLfloat*)params);
  else
  if (GLEW_EXT_point_parameters)
    glPointParameterfvEXT(pname,(GLfloat*)params);
  else
    VL_TRAP();
}

void VL_glPointParameterf( GLenum pname, GLfloat param)
{
  if (GLEW_VERSION_1_4)
    glPointParameterf(pname,param);
  else
  if (GLEW_ARB_point_parameters)
    glPointParameterfARB(pname,param);
  else
  if (GLEW_EXT_point_parameters)
    glPointParameterfEXT(pname,param);
  else
    VL_TRAP();
}
//-----------------------------------------------------------------------------
void VL_glStencilFuncSeparate( GLenum face, GLenum func, GLint ref, GLuint mask)
{
  if ( GLEW_VERSION_2_0 )
    glStencilFuncSeparate( face, func, ref, mask );
  else
    VL_TRAP();
  // NOT SUPPORTED
  // see http://www.opengl.org/registry/specs/ATI/separate_stencil.txt
  /*else
  if ( GLEW_ATI_separate_stencil )
    glStencilFuncSeparateATI( GLenum frontfunc, GLenum backfunc, GLint ref, GLuint mask ) */
}

void VL_glStencilOpSeparate( GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass)
{
  if ( GLEW_VERSION_2_0 )
    glStencilOpSeparate( face, sfail, dpfail, dppass );
  else
    VL_TRAP();
  // NOT SUPPORTED
  // see http://www.opengl.org/registry/specs/ATI/separate_stencil.txt
  /*else
  if ( GLEW_ATI_separate_stencil )
    glStencilOpSeparateATI( GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass )*/
}
//-----------------------------------------------------------------------------
void VL_glSampleCoverage( GLclampf value, GLboolean invert)
{
  if (GLEW_VERSION_1_3)
    glSampleCoverage(value,invert);
  else
  if (GLEW_ARB_multisample)
    glSampleCoverageARB(value,invert);  
  else
    VL_TRAP();
}
//-----------------------------------------------------------------------------
void VL_glBindRenderbuffer(GLenum target, GLuint renderbuffer)
{
  if (GLEW_ARB_framebuffer_object||GLEW_VERSION_3_0)
    glBindRenderbuffer(target, renderbuffer);
  else
  if (GLEW_EXT_framebuffer_object)
    glBindRenderbufferEXT(target, renderbuffer);
  else
    VL_TRAP();
}
void VL_glDeleteRenderbuffers(GLsizei n, const GLuint *renderbuffers)
{
  if (GLEW_ARB_framebuffer_object||GLEW_VERSION_3_0)
    glDeleteRenderbuffers(n, renderbuffers);
  else
  if (GLEW_EXT_framebuffer_object)
    glDeleteRenderbuffersEXT(n, renderbuffers);
  else
    VL_TRAP();
}
void VL_glGenRenderbuffers(GLsizei n, GLuint *renderbuffers)
{
  if (GLEW_ARB_framebuffer_object||GLEW_VERSION_3_0)
    glGenRenderbuffers(n, renderbuffers);
  else
  if (GLEW_EXT_framebuffer_object)
    glGenRenderbuffersEXT(n, renderbuffers);
  else
    VL_TRAP();
}
void VL_glRenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height)
{
  if (GLEW_ARB_framebuffer_object||GLEW_VERSION_3_0)
    glRenderbufferStorage(target, internalformat, width, height);
  else
  if (GLEW_EXT_framebuffer_object)
    glRenderbufferStorageEXT(target, internalformat, width, height);
  else
    VL_TRAP();
}
void VL_glGetRenderbufferParameteriv(GLenum target, GLenum pname, GLint *params)
{
  if (GLEW_ARB_framebuffer_object||GLEW_VERSION_3_0)
    glGetRenderbufferParameteriv(target, pname, params);
  else
  if (GLEW_EXT_framebuffer_object)
    glGetRenderbufferParameterivEXT(target, pname, params);
  else
    VL_TRAP();
}
GLboolean VL_glIsFramebuffer(GLuint framebuffer)
{
  if (GLEW_ARB_framebuffer_object||GLEW_VERSION_3_0)
    return glIsFramebuffer(framebuffer);
  else
  if (GLEW_EXT_framebuffer_object)
    return glIsFramebufferEXT(framebuffer);
  else
    VL_TRAP();
  return GL_FALSE;
}
void VL_glBindFramebuffer(GLenum target, GLuint framebuffer)
{
  if (GLEW_ARB_framebuffer_object||GLEW_VERSION_3_0)
    glBindFramebuffer(target, framebuffer);
  else
  if (GLEW_EXT_framebuffer_object)
    glBindFramebufferEXT(target, framebuffer);
  else
  {
    VL_CHECK(framebuffer == 0);
  }
}
void VL_glDeleteFramebuffers(GLsizei n, const GLuint *framebuffers)
{
  if (GLEW_ARB_framebuffer_object||GLEW_VERSION_3_0)
    glDeleteFramebuffers(n, framebuffers);
  else
  if (GLEW_EXT_framebuffer_object)
    glDeleteFramebuffersEXT(n, framebuffers);
  else
    VL_TRAP();
}
void VL_glGenFramebuffers(GLsizei n, GLuint *framebuffers)
{
  if (GLEW_ARB_framebuffer_object||GLEW_VERSION_3_0)
    glGenFramebuffers(n, framebuffers);
  else
  if (GLEW_EXT_framebuffer_object)
    glGenFramebuffersEXT(n, framebuffers);
  else
    VL_TRAP();
}
GLenum VL_glCheckFramebufferStatus(GLenum target)
{
  if (GLEW_ARB_framebuffer_object||GLEW_VERSION_3_0)
    return glCheckFramebufferStatus(target);
  else
  if (GLEW_EXT_framebuffer_object)
    return glCheckFramebufferStatusEXT(target);
  else
    VL_TRAP();

  return GL_FRAMEBUFFER_UNSUPPORTED;
}
void VL_glFramebufferTexture1D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
{
  if (GLEW_ARB_framebuffer_object||GLEW_VERSION_3_0)
    glFramebufferTexture1D(target, attachment, textarget, texture, level);
  else
  if (GLEW_EXT_framebuffer_object)
    glFramebufferTexture1DEXT(target, attachment, textarget, texture, level);
  else
    VL_TRAP();
}
void VL_glFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
{
  if (GLEW_ARB_framebuffer_object||GLEW_VERSION_3_0)
    glFramebufferTexture2D(target, attachment, textarget, texture, level);
  else
  if (GLEW_EXT_framebuffer_object)
    glFramebufferTexture2DEXT(target, attachment, textarget, texture, level);
  else
    VL_TRAP();
}
void VL_glFramebufferTexture3D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint zoffset)
{
  if (GLEW_ARB_framebuffer_object||GLEW_VERSION_3_0)
    glFramebufferTexture3D(target, attachment, textarget, texture, level, zoffset);
  else
  if (GLEW_EXT_framebuffer_object)
    glFramebufferTexture3DEXT(target, attachment, textarget, texture, level, zoffset);
  else
    VL_TRAP();
}
void VL_glFramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer)
{
  if (GLEW_ARB_framebuffer_object||GLEW_VERSION_3_0)
    glFramebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer);
  else
  if (GLEW_EXT_framebuffer_object)
    glFramebufferRenderbufferEXT(target, attachment, renderbuffertarget, renderbuffer);
  else
    VL_TRAP();
}
void VL_glGetFramebufferAttachmentParameteriv(GLenum target, GLenum attachment, GLenum pname, GLint *params)
{
  if (GLEW_ARB_framebuffer_object||GLEW_VERSION_3_0)
    glGetFramebufferAttachmentParameteriv(target,attachment,pname,params);
  else
  if (GLEW_EXT_framebuffer_object)
    glGetFramebufferAttachmentParameterivEXT(target,attachment,pname,params);
  else
    VL_TRAP();
}
void VL_glGenerateMipmap(GLenum target)
{
  if (GLEW_ARB_framebuffer_object||GLEW_VERSION_3_0)
    glGenerateMipmap(target);
  else
  if (GLEW_EXT_framebuffer_object)
    glGenerateMipmapEXT(target);
  else
    VL_TRAP();
}
void VL_glFramebufferTexture(GLenum target, GLenum attachment, GLuint texture, GLint level)
{
  // even if this extension is signed ad ARB it does not appear in the GL 3.0 specs
  if (GLEW_ARB_geometry_shader4)
    glFramebufferTextureARB(target,attachment,texture,level);
  else
  if (GLEW_EXT_geometry_shader4)
    glFramebufferTextureEXT(target,attachment,texture,level);
  else
    VL_TRAP();
}
void VL_glFramebufferTextureLayer(GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer)
{
  // FIXME: missing function -> GLEW bug?
  //if (GLEW_VERSION_3_0)
  //  glFramebufferTextureLayer(target, attachment, texture, level, layer);
  //else
  if (GLEW_ARB_geometry_shader4)
    glFramebufferTextureLayerARB(target, attachment, texture, level, layer);
  else
  if (GLEW_EXT_geometry_shader4||GLEW_EXT_texture_array)
    glFramebufferTextureLayerEXT(target, attachment, texture, level, layer);
  else
    VL_TRAP();
}
void VL_glRenderbufferStorageMultisample( GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height )
{
  if (GLEW_VERSION_3_0||GLEW_ARB_framebuffer_object)
    glRenderbufferStorageMultisample(target, samples, internalformat, width, height);
  else
  if (GLEW_EXT_framebuffer_multisample)
    glRenderbufferStorageMultisampleEXT(target, samples, internalformat, width, height);
  else
    VL_TRAP();
}
void VL_glBlitFramebuffer(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter)
{
  if (GLEW_ARB_framebuffer_object)
    glBlitFramebuffer(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter);
  else
  if (GLEW_EXT_framebuffer_blit)
    glBlitFramebufferEXT(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter);
  else
    VL_TRAP();
}

//-----------------------------------------------------------------------------
void VL_glDrawElementsInstanced(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices, GLsizei primcount)
{
  // TODO: support OpenGL 3.1
  if (GLEW_ARB_draw_instanced)
    glDrawElementsInstancedARB(mode, count, type, indices, primcount);
  else
  if (GLEW_EXT_draw_instanced)
    glDrawElementsInstancedEXT(mode, count, type, indices, primcount);
  else
    VL_TRAP();
}

void VL_glDrawArraysInstanced(GLenum mode, GLint first, GLsizei count, GLsizei primcount)
{
  // TODO: support OpenGL 3.1
  if (GLEW_ARB_draw_instanced)
    glDrawArraysInstancedARB(mode, first, count, primcount);
  else
  if (GLEW_EXT_draw_instanced)
    glDrawArraysInstancedEXT(mode, first, count, primcount);
  else
    VL_TRAP();
}
//-----------------------------------------------------------------------------
void VL_glProgramParameteri(GLuint program, GLenum pname, GLint value)
{
  if (GLEW_ARB_geometry_shader4)
    glProgramParameteriARB(program, pname, value);
  else
  if (GLEW_EXT_geometry_shader4)
    glProgramParameteriEXT(program, pname, value);
  else
    VL_TRAP();
}

void VL_glBindFragDataLocation(GLuint program, GLuint colorNumber, const GLchar *name)
{
  if (GLEW_VERSION_3_0)
    glBindFragDataLocation(program, colorNumber, name);
  else
  if (GLEW_EXT_gpu_shader4)
    glBindFragDataLocationEXT(program, colorNumber, name);
  else
    VL_TRAP();
}

void VL_glUniform1uiv(GLint location, GLsizei count, const GLuint *value)
{
  if (GLEW_VERSION_3_0)
    glUniform1uiv(location, count, value);
  else
  if (GLEW_EXT_gpu_shader4)
    glUniform1uivEXT(location, count, value);
  else
    VL_TRAP();
}
void VL_glUniform2uiv(GLint location, GLsizei count, const GLuint *value)
{
  if (GLEW_VERSION_3_0)
    glUniform2uiv(location, count, value);
  else
  if (GLEW_EXT_gpu_shader4)
    glUniform2uivEXT(location, count, value);
  else
    VL_TRAP();
}
void VL_glUniform3uiv(GLint location, GLsizei count, const GLuint *value)
{
  if (GLEW_VERSION_3_0)
    glUniform3uiv(location, count, value);
  else
  if (GLEW_EXT_gpu_shader4)
    glUniform3uivEXT(location, count, value);
  else
    VL_TRAP();
}
void VL_glUniform4uiv(GLint location, GLsizei count, const GLuint *value)
{
  if (GLEW_VERSION_3_0)
    glUniform4uiv(location, count, value);
  else
  if (GLEW_EXT_gpu_shader4)
    glUniform4uivEXT(location, count, value);
  else
    VL_TRAP();
}
