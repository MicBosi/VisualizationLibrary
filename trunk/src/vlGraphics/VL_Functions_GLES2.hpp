
inline void VL_glBindBuffer( GLenum target, GLuint buffer )
{
  if (glBindBuffer)
    glBindBuffer(target,buffer);
  else
  if (glBindBufferARB)
    glBindBufferARB(target,buffer);
  else
  {
    VL_CHECK( buffer == 0 );
  }
}

inline void VL_glGenBuffers( GLsizei n, GLuint * buffers)
{
  if (glGenBuffers)
    glGenBuffers( n, buffers);
  else
  if (glGenBuffersARB)
    glGenBuffersARB( n, buffers);
  else
    VL_TRAP();
}

inline void VL_glDeleteBuffers( GLsizei n, const GLuint * buffers)
{
  if (glDeleteBuffers)
    glDeleteBuffers( n, buffers);
  else
  if (glDeleteBuffersARB)
    glDeleteBuffersARB( n, buffers);
  else
    VL_TRAP();
}

inline void VL_glBufferData( GLenum target, GLsizeiptr size, const GLvoid * data, GLenum usage)
{
  if (glBufferData)
    glBufferData( target, size, data, usage);
  else
  if (glBufferDataARB)
    glBufferDataARB( target, size, data, usage);
  else
    VL_TRAP();
}

inline void VL_glBufferSubData( GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid * data)
{
  if (glBufferSubData)
    glBufferSubData( target, offset, size, data );
  else
  if (glBufferSubDataARB)
    glBufferSubDataARB( target, offset, size, data);
  else
    VL_TRAP();
}

inline void* VL_glMapBuffer( GLenum target, GLenum access)
{
  if (glMapBuffer)
    return glMapBuffer( target, access);
  else
  if (glMapBufferARB)
    return glMapBufferARB( target, access);
  else
    VL_TRAP();
  return 0;
}

inline GLboolean VL_glUnmapBuffer(GLenum target)
{
  if (glUnmapBuffer)
    return glUnmapBuffer( target );
  else
  if (glUnmapBufferARB)
    return glUnmapBufferARB( target );
  else
    VL_TRAP();
  return GL_FALSE;
}
  
//-----------------------------------------------------------------------------
  
inline void VL_glSecondaryColor3f(float r, float g, float b)
{
  if(glSecondaryColor3f)
    glSecondaryColor3f(r,g,b);
  else
  if(glSecondaryColor3fEXT)
    glSecondaryColor3fEXT(r,g,b);
  else
    VL_TRAP();
}

inline void VL_glSecondaryColorPointer( GLint size, GLenum type, GLsizei stride, const GLvoid* pointer)
{
  if(glSecondaryColorPointer)
    glSecondaryColorPointer(size, type, stride, (GLvoid*)pointer);
  else
  if(glSecondaryColorPointerEXT)
    glSecondaryColorPointerEXT(size, type, stride, (GLvoid*)pointer);
  else
    VL_TRAP();
}
  
//-----------------------------------------------------------------------------
  
inline void VL_glFogCoordPointer( GLenum type, GLsizei stride, GLvoid* pointer )
{
  if (glFogCoordPointer)
    glFogCoordPointer(type,stride,pointer);
  else
  if (glFogCoordPointerEXT)
    glFogCoordPointerEXT(type,stride,pointer);
  else
    VL_TRAP();
}
  
//-----------------------------------------------------------------------------
  
inline void VL_glEnableVertexAttribArray( GLuint index )
{
  if (glEnableVertexAttribArray)
    glEnableVertexAttribArray(index);
  else
  if (glEnableVertexAttribArrayARB)
    glEnableVertexAttribArrayARB(index);
  else
    VL_TRAP();
}

inline void VL_glDisableVertexAttribArray( GLuint index )
{
  if (glDisableVertexAttribArray)
    glDisableVertexAttribArray(index);
  else
  if (glDisableVertexAttribArrayARB)
    glDisableVertexAttribArrayARB(index);
  else
    VL_TRAP();
}

//-----------------------------------------------------------------------------

inline void VL_glVertexAttribPointer( GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid * pointer)
{
  if (glVertexAttribPointer)
    glVertexAttribPointer(index, size, type, normalized, stride, pointer);
  else
  if (glVertexAttribPointerARB)
    glVertexAttribPointerARB(index, size, type, normalized, stride, pointer);
  else
    VL_TRAP();
}

inline void VL_glVertexAttribIPointer(GLuint name, GLint size, GLenum type, GLsizei stride, const GLvoid* pointer)
{
  if(glVertexAttribIPointer)
    glVertexAttribIPointer(name, size, type, stride, pointer);
  else
  if (glVertexAttribIPointerEXT)
    glVertexAttribIPointerEXT(name, size, type, stride, pointer);
  else
    VL_TRAP();
}

inline void VL_glVertexAttribLPointer(GLuint name, GLint size, GLenum type, GLsizei stride, const GLvoid* pointer)
{
  if(glVertexAttribLPointer)
    glVertexAttribLPointer(name, size, type, stride, pointer);
  else
  if(glVertexAttribLPointerEXT)
    glVertexAttribLPointerEXT(name, size, type, stride, pointer);
  else
    VL_TRAP();
}
  
//-----------------------------------------------------------------------------
  
inline void VL_glClientActiveTexture(GLenum texture)
{
  if(glClientActiveTexture)
    glClientActiveTexture(texture);
  else
  if (glClientActiveTextureARB)
    glClientActiveTextureARB(texture);
  else
  {
    VL_CHECK(texture == GL_TEXTURE0);
  }
}

inline void VL_glActiveTexture(GLenum texture)
{
  if(glActiveTexture)
    glActiveTexture(texture);
  else
  if (glActiveTextureARB)
    glActiveTextureARB(texture);
  else
  {
    VL_CHECK(texture == GL_TEXTURE0);
  }
}
  
//-----------------------------------------------------------------------------
  
inline void VL_glBlendFuncSeparate( GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha)
{
  if (glBlendFuncSeparate)
    glBlendFuncSeparate( srcRGB, dstRGB, srcAlpha, dstAlpha);
  else
  if (glBlendFuncSeparateEXT)
    glBlendFuncSeparateEXT( srcRGB, dstRGB, srcAlpha, dstAlpha);
  else
    VL_TRAP();
}

inline void VL_glBlendEquationSeparate( GLenum modeRGB, GLenum modeAlpha)
{
  if (glBlendEquationSeparate)
    glBlendEquationSeparate(modeRGB, modeAlpha);
  else
  if(glBlendEquationSeparateEXT)
    glBlendEquationSeparateEXT(modeRGB, modeAlpha);
  else
    VL_TRAP();
}

inline void VL_glBlendEquation(GLenum mode)
{
  if (glBlendEquation)
    glBlendEquation(mode);
  else
  if (glBlendEquationEXT)
    glBlendEquationEXT(mode);
  else
    VL_TRAP();
}

inline void VL_glBlendColor( GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
  if(glBlendColor)
    glBlendColor(red,green,blue,alpha);
  else
  if (glBlendColorEXT)
    glBlendColorEXT(red,green,blue,alpha);
  else
    VL_TRAP();
}

//-----------------------------------------------------------------------------
  
inline void VL_glPointParameterfv( GLenum pname, const GLfloat* params)
{
  if (glPointParameterfv)
    glPointParameterfv(pname,(GLfloat*)params);
  else
  if (glPointParameterfvARB)
    glPointParameterfvARB(pname,(GLfloat*)params);
  else
  if (glPointParameterfvEXT)
    glPointParameterfvEXT(pname,(GLfloat*)params);
  else
    VL_TRAP();
}

inline void VL_glPointParameterf( GLenum pname, GLfloat param)
{
  if (glPointParameterf)
    glPointParameterf(pname,param);
  else
  if (glPointParameterfARB)
    glPointParameterfARB(pname,param);
  else
  if (glPointParameterfEXT)
    glPointParameterfEXT(pname,param);
  else
    VL_TRAP();
}

inline void VL_glPointParameteri( GLenum pname, GLenum param)
{
  if (glPointParameteri)
    glPointParameteri(pname,param);
  else
  if (glPointParameteriNV)
    glPointParameteriNV(pname,param);
  else
    VL_TRAP();
}
  
//-----------------------------------------------------------------------------
  
inline void VL_glStencilFuncSeparate( GLenum face, GLenum func, GLint ref, GLuint mask)
{
  if (glStencilFuncSeparate)
    glStencilFuncSeparate( face, func, ref, mask );
  else
    VL_TRAP();
  // NOT SUPPORTED
  // see also http://www.opengl.org/registry/specs/ATI/separate_stencil.txt
  /*else
  if ( Has_GL_ATI_separate_stencil )
    glStencilFuncSeparateATI( GLenum frontfunc, GLenum backfunc, GLint ref, GLuint mask ) */
}

inline void VL_glStencilOpSeparate( GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass)
{
  if (glStencilOpSeparate)
    glStencilOpSeparate( face, sfail, dpfail, dppass );
  else
    VL_TRAP();
  // NOT SUPPORTED
  // see also http://www.opengl.org/registry/specs/ATI/separate_stencil.txt
  /*else
  if ( Has_GL_ATI_separate_stencil )
    glStencilOpSeparateATI( GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass )*/
}
  
//-----------------------------------------------------------------------------
  
inline void VL_glSampleCoverage( GLclampf value, GLboolean invert)
{
  if (glSampleCoverage)
    glSampleCoverage(value,invert);
  else
  if (glSampleCoverageARB)
    glSampleCoverageARB(value,invert);  
  else
    VL_TRAP();
}
  
//-----------------------------------------------------------------------------
  
inline void VL_glBindRenderbuffer(GLenum target, GLuint renderbuffer)
{
  if (glBindRenderbuffer)
    glBindRenderbuffer(target, renderbuffer);
  else
  if (glBindRenderbufferEXT)
    glBindRenderbufferEXT(target, renderbuffer);
  else
    VL_TRAP();
}
  
inline void VL_glDeleteRenderbuffers(GLsizei n, const GLuint *renderbuffers)
{
  if (glDeleteRenderbuffers)
    glDeleteRenderbuffers(n, renderbuffers);
  else
  if (glDeleteRenderbuffersEXT)
    glDeleteRenderbuffersEXT(n, renderbuffers);
  else
    VL_TRAP();
}
  
inline void VL_glGenRenderbuffers(GLsizei n, GLuint *renderbuffers)
{
  if (glGenRenderbuffers)
    glGenRenderbuffers(n, renderbuffers);
  else
  if (glGenRenderbuffersEXT)
    glGenRenderbuffersEXT(n, renderbuffers);
  else
    VL_TRAP();
}
  
inline void VL_glRenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height)
{
  if (glRenderbufferStorage)
    glRenderbufferStorage(target, internalformat, width, height);
  else
  if (glRenderbufferStorageEXT)
    glRenderbufferStorageEXT(target, internalformat, width, height);
  else
    VL_TRAP();
}
  
inline void VL_glGetRenderbufferParameteriv(GLenum target, GLenum pname, GLint *params)
{
  if (glGetRenderbufferParameteriv)
    glGetRenderbufferParameteriv(target, pname, params);
  else
  if (glGetRenderbufferParameterivEXT)
    glGetRenderbufferParameterivEXT(target, pname, params);
  else
    VL_TRAP();
}
  
inline GLboolean VL_glIsFramebuffer(GLuint framebuffer)
{
  if (glIsFramebuffer)
    return glIsFramebuffer(framebuffer);
  else
  if (glIsFramebufferEXT)
    return glIsFramebufferEXT(framebuffer);
  else
    VL_TRAP();
  return GL_FALSE;
}
  
inline void VL_glBindFramebuffer(GLenum target, GLuint framebuffer)
{
  if (glBindFramebuffer)
    glBindFramebuffer(target, framebuffer);
  else
  if (glBindFramebufferEXT)
    glBindFramebufferEXT(target, framebuffer);
  else
  {
    VL_CHECK(framebuffer == 0);
  }
}
  
inline void VL_glDeleteFramebuffers(GLsizei n, const GLuint *framebuffers)
{
  if (glDeleteFramebuffers)
    glDeleteFramebuffers(n, framebuffers);
  else
  if (glDeleteFramebuffersEXT)
    glDeleteFramebuffersEXT(n, framebuffers);
  else
    VL_TRAP();
}
  
inline void VL_glGenFramebuffers(GLsizei n, GLuint *framebuffers)
{
  if (glGenFramebuffers)
    glGenFramebuffers(n, framebuffers);
  else
  if (glGenFramebuffersEXT)
    glGenFramebuffersEXT(n, framebuffers);
  else
    VL_TRAP();
}
  
inline GLenum VL_glCheckFramebufferStatus(GLenum target)
{
  if (glCheckFramebufferStatus)
    return glCheckFramebufferStatus(target);
  else
  if (glCheckFramebufferStatusEXT)
    return glCheckFramebufferStatusEXT(target);
  else
    VL_TRAP();

  return GL_FRAMEBUFFER_UNSUPPORTED;
}
  
inline void VL_glFramebufferTexture1D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
{
  if (glFramebufferTexture1D)
    glFramebufferTexture1D(target, attachment, textarget, texture, level);
  else
  if (glFramebufferTexture1DEXT)
    glFramebufferTexture1DEXT(target, attachment, textarget, texture, level);
  else
    VL_TRAP();
}
  
inline void VL_glFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
{
  if (glFramebufferTexture2D)
    glFramebufferTexture2D(target, attachment, textarget, texture, level);
  else
  if (glFramebufferTexture2DEXT)
    glFramebufferTexture2DEXT(target, attachment, textarget, texture, level);
  else
    VL_TRAP();
}
  
inline void VL_glFramebufferTexture3D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint zoffset)
{
  if (glFramebufferTexture3D)
    glFramebufferTexture3D(target, attachment, textarget, texture, level, zoffset);
  else
  if (glFramebufferTexture3DEXT)
    glFramebufferTexture3DEXT(target, attachment, textarget, texture, level, zoffset);
  else
    VL_TRAP();
}
  
inline void VL_glFramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer)
{
  if (glFramebufferRenderbuffer)
    glFramebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer);
  else
  if (glFramebufferRenderbufferEXT)
    glFramebufferRenderbufferEXT(target, attachment, renderbuffertarget, renderbuffer);
  else
    VL_TRAP();
}
  
inline void VL_glGetFramebufferAttachmentParameteriv(GLenum target, GLenum attachment, GLenum pname, GLint *params)
{
  if (glGetFramebufferAttachmentParameteriv)
    glGetFramebufferAttachmentParameteriv(target,attachment,pname,params);
  else
  if (glGetFramebufferAttachmentParameterivEXT)
    glGetFramebufferAttachmentParameterivEXT(target,attachment,pname,params);
  else
    VL_TRAP();
}
  
inline void VL_glGenerateMipmap(GLenum target)
{
  if (glGenerateMipmap)
    glGenerateMipmap(target);
  else
  if (glGenerateMipmapEXT)
    glGenerateMipmapEXT(target);
  else
    VL_TRAP();
}
  
inline void VL_glFramebufferTexture(GLenum target, GLenum attachment, GLuint texture, GLint level)
{
  // even if this extension is signed ad ARB it does not appear in the GL 3.0 specs
  if (glFramebufferTextureARB)
    glFramebufferTextureARB(target,attachment,texture,level);
  else
  if (glFramebufferTextureEXT)
    glFramebufferTextureEXT(target,attachment,texture,level);
  else
    VL_TRAP();
}
  
inline void VL_glFramebufferTextureLayer(GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer)
{
  if (glFramebufferTextureLayer)
    glFramebufferTextureLayer(target, attachment, texture, level, layer);
  else
  if (glFramebufferTextureLayerARB)
    glFramebufferTextureLayerARB(target, attachment, texture, level, layer);
  else
  if (glFramebufferTextureLayerEXT)
    glFramebufferTextureLayerEXT(target, attachment, texture, level, layer);
  else
    VL_TRAP();
}
  
inline void VL_glRenderbufferStorageMultisample( GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height )
{
  if (glRenderbufferStorageMultisample)
    glRenderbufferStorageMultisample(target, samples, internalformat, width, height);
  else
  if (glRenderbufferStorageMultisampleEXT)
    glRenderbufferStorageMultisampleEXT(target, samples, internalformat, width, height);
  else
    VL_TRAP();
}
  
inline void VL_glBlitFramebuffer(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter)
{
  if (glBlitFramebuffer)
    glBlitFramebuffer(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter);
  else
  if (glBlitFramebufferEXT)
    glBlitFramebufferEXT(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter);
  else
    VL_TRAP();
}

//-----------------------------------------------------------------------------

inline void VL_glDrawElementsInstanced(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices, GLsizei primcount)
{
  if (glDrawElementsInstanced)
    glDrawElementsInstanced(mode, count, type, indices, primcount);
  else
  if (glDrawElementsInstancedARB)
    glDrawElementsInstancedARB(mode, count, type, indices, primcount);
  else
  if (glDrawElementsInstancedEXT)
    glDrawElementsInstancedEXT(mode, count, type, indices, primcount);
  else
    VL_TRAP();
}

inline void VL_glDrawElementsInstancedBaseVertex(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices, GLsizei primcount, int basevertex)
{
  if (glDrawElementsInstancedBaseVertex)
    glDrawElementsInstancedBaseVertex(mode, count, type, indices, primcount, basevertex);
  else
    VL_TRAP();
}

inline void VL_glDrawElementsBaseVertex(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices, int basevertex)
{
  if (glDrawElementsBaseVertex)
    glDrawElementsBaseVertex(mode, count, type, (void*)indices, basevertex);
  else
    VL_TRAP();
}

inline void VL_glDrawRangeElementsBaseVertex(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid *indices, int basevertex)
{
  if (glDrawRangeElementsBaseVertex)
    glDrawRangeElementsBaseVertex(mode, start, end, count, type, (void*)indices, basevertex);
  else
    VL_TRAP();
}

inline void VL_glDrawArraysInstanced(GLenum mode, GLint first, GLsizei count, GLsizei primcount)
{
  if (glDrawArraysInstanced)
    glDrawArraysInstanced(mode, first, count, primcount);
  else
  if (glDrawArraysInstancedARB)
    glDrawArraysInstancedARB(mode, first, count, primcount);
  else
  if (glDrawArraysInstancedEXT)
    glDrawArraysInstancedEXT(mode, first, count, primcount);
  else
    VL_TRAP();
}
  
//-----------------------------------------------------------------------------
  
inline void VL_glProgramParameteri(GLuint program, GLenum pname, GLint value)
{
  if (glProgramParameteriARB)
    glProgramParameteriARB(program, pname, value);
  else
  if (glProgramParameteriEXT)
    glProgramParameteriEXT(program, pname, value);
  else
    VL_TRAP();
}

inline void VL_glBindFragDataLocation(GLuint program, GLuint colorNumber, const GLchar *name)
{
  if (glBindFragDataLocation)
    glBindFragDataLocation(program, colorNumber, name);
  else
  if (glBindFragDataLocationEXT)
    glBindFragDataLocationEXT(program, colorNumber, name);
  else
    VL_TRAP();
}

inline void VL_glUniform1uiv(GLint location, GLsizei count, const GLuint *value)
{
  if (glUniform1uiv)
    glUniform1uiv(location, count, value);
  else
  if (glUniform1uivEXT)
    glUniform1uivEXT(location, count, value);
  else
    VL_TRAP();
}
  
inline void VL_glUniform2uiv(GLint location, GLsizei count, const GLuint *value)
{
  if (glUniform2uiv)
    glUniform2uiv(location, count, value);
  else
  if (glUniform2uivEXT)
    glUniform2uivEXT(location, count, value);
  else
    VL_TRAP();
}
  
inline void VL_glUniform3uiv(GLint location, GLsizei count, const GLuint *value)
{
  if (glUniform3uiv)
    glUniform3uiv(location, count, value);
  else
  if (glUniform3uivEXT)
    glUniform3uivEXT(location, count, value);
  else
    VL_TRAP();
}
  
inline void VL_glUniform4uiv(GLint location, GLsizei count, const GLuint *value)
{
  if (glUniform4uiv)
    glUniform4uiv(location, count, value);
  else
  if (glUniform4uivEXT)
    glUniform4uivEXT(location, count, value);
  else
    VL_TRAP();
}
