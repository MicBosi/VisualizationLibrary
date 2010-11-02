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

#include <vl/GLSL.hpp>
#include <vl/VisualizationLibrary.hpp>
#include <vl/VirtualFile.hpp>
#include <vl/Log.hpp>
#include <vl/Say.hpp>

using namespace vl;

//-----------------------------------------------------------------------------
// GLSLShader
//-----------------------------------------------------------------------------
GLSLShader::GLSLShader(EShaderType type, const String& source)
{
  #ifndef NDEBUG
    mObjectName = className();
  #endif
  mType = type;
  mHandle = 0;
  mCompiled = false;
  setSource(source);
}
//-----------------------------------------------------------------------------
GLSLShader::~GLSLShader()
{
  deleteShader();
}
//-----------------------------------------------------------------------------
void GLSLShader::setSource( const std::string& source )
{
  mCompiled = false;
  mSource = source;
}
//-----------------------------------------------------------------------------
void GLSLShader::setSource( const String& source )
{
  if (vl::locateFile(source))
  {
    setSource(vl::String::loadText(source).toStdString());
    setObjectName( source.toStdString() );
  }
  else
    setSource( source.toStdString() );
}
//-----------------------------------------------------------------------------
bool GLSLShader::compile()
{
  VL_CHECK( GLEW_VERSION_2_0||GLEW_VERSION_3_0 )
  if( !(GLEW_VERSION_2_0||GLEW_VERSION_3_0) )
    return false;

  if (!mCompiled)
  {
    // compile the shader

    if (!handle())
    {
      // createShader();
      mHandle = glCreateShader(mType);
    }

    // assign sources

    const GLchar* source[] = { mSource.c_str() };
    glShaderSource(handle(), 1, source, NULL);

    // compile the shader

    glCompileShader(handle());

    if ( compileStatus() )
    {
      mCompiled = true;
      #ifndef NDEBUG
      if (!infoLog().empty())
        Log::info( Say("%s\n%s\n\n") << objectName().c_str() << infoLog() );
      #endif
    }
    else
    {
      Log::error( Say("\nShader compilation error: '%s':\n\n") << objectName().c_str() );
      Log::print( Say("SOURCE:\n%s\n\n") << mSource.c_str() );
      Log::print( Say("LOG:\n%s\n\n") << infoLog() );
      VL_TRAP()
    }
  }

  return mCompiled;
}
//-----------------------------------------------------------------------------
bool GLSLShader::compileStatus() const
{
  VL_CHECK( GLEW_VERSION_2_0||GLEW_VERSION_3_0 )
  if( !(GLEW_VERSION_2_0||GLEW_VERSION_3_0) )
    return false;
  VL_CHECK(handle())

  int status = 0;
  glGetShaderiv(handle(), GL_COMPILE_STATUS, &status);
  return status == GL_TRUE;
}
//-----------------------------------------------------------------------------
String GLSLShader::infoLog() const
{
  VL_CHECK( GLEW_VERSION_2_0||GLEW_VERSION_3_0 )
  if( !(GLEW_VERSION_2_0||GLEW_VERSION_3_0) )
    return "OpenGL Shading Language not supported.\n";
  VL_CHECK(handle())

  int max_length = 0;
  glGetShaderiv(handle(), GL_INFO_LOG_LENGTH, &max_length);
  std::vector<GLchar> log_buffer;
  log_buffer.resize(max_length+1);
  glGetShaderInfoLog(handle(), max_length, NULL, &log_buffer[0]);
  String log_string(&log_buffer[0]);
  return log_string;
}
//-----------------------------------------------------------------------------
void GLSLShader::createShader()
{
  VL_CHECK( GLEW_VERSION_2_0||GLEW_VERSION_3_0 )
  if( !(GLEW_VERSION_2_0||GLEW_VERSION_3_0) )
    return;
  if (!handle())
  {
    mHandle = glCreateShader(mType);
    mCompiled = false;
  }
  VL_CHECK(handle());
}
//------------------------------------------------------------------------------
void GLSLShader::deleteShader()
{
  VL_CHECK( GLEW_VERSION_2_0||GLEW_VERSION_3_0 )
  if( !(GLEW_VERSION_2_0||GLEW_VERSION_3_0) )
    return;
  if (handle())
  {
    glDeleteShader(handle());
    mHandle = 0;
    mCompiled = false;
  }
}
//------------------------------------------------------------------------------
// GLSLProgram
//------------------------------------------------------------------------------
GLSLProgram::GLSLProgram()
{
  #ifndef NDEBUG
    mObjectName = className();
  #endif
  mScheduleLink = true;
  mHandle = 0;
  mGeometryVerticesOut = 0;
  mGeometryInputType   = GIT_TRIANGLES;
  mGeometryOutputType  = GOT_TRIANGLE_STRIP;
}
//-----------------------------------------------------------------------------
GLSLProgram::~GLSLProgram()
{
  if (handle())
    deleteProgram();
}
//-----------------------------------------------------------------------------
void GLSLProgram::createProgram()
{
  VL_CHECK( GLEW_VERSION_2_0||GLEW_VERSION_3_0 )
  if( !(GLEW_VERSION_2_0||GLEW_VERSION_3_0) )
    return;
  if (handle() == 0)
  {
    scheduleRelinking();
    mHandle = glCreateProgram(); VL_CHECK_OGL();
    VL_CHECK(handle())
    VL_CHECK_OGL()
  }
}
//-----------------------------------------------------------------------------
void GLSLProgram::deleteProgram()
{
  VL_CHECK( GLEW_VERSION_2_0||GLEW_VERSION_3_0 )
  if( !(GLEW_VERSION_2_0||GLEW_VERSION_3_0) )
    return;
  if(handle())
  {
    glDeleteProgram(handle()); // VL_CHECK_OGL();
    mHandle = 0;
  }
  scheduleRelinking();
}
//-----------------------------------------------------------------------------
bool GLSLProgram::attachShader(GLSLShader* shader)
{
  VL_CHECK( GLEW_VERSION_2_0||GLEW_VERSION_3_0 )
  if( !(GLEW_VERSION_2_0||GLEW_VERSION_3_0) )
    return false;
  scheduleRelinking();

  #if 0
    if(std::find(mShaders.begin(), mShaders.end(), shader) != mShaders.end())
    {
      if ( shader->handle() )
        glDetachShader( handle(), shader->handle() ); VL_CHECK_OGL();
    }
    else
      mShaders.push_back(shader);
  #else
    detachShader(shader);
    mShaders.push_back(shader);
  #endif

  if ( shader->compile() )
  {
    createProgram();
    glAttachShader( handle(), shader->handle() ); VL_CHECK_OGL();
    return true;
  }

  VL_CHECK_OGL()
  VL_TRAP()
  return false;

}
//-----------------------------------------------------------------------------
void GLSLProgram::detachAllShaders()
{
  for(size_t i=mShaders.size(); i--;)
    detachShader(mShaders[i].get());
}
//-----------------------------------------------------------------------------
// detaching a shader that has not been attached is allowed, and is a No-Op
bool GLSLProgram::detachShader(GLSLShader* shader)
{
  VL_CHECK( GLEW_VERSION_2_0||GLEW_VERSION_3_0 )
  if( !(GLEW_VERSION_2_0||GLEW_VERSION_3_0) )
    return false;
  if (!handle() || !shader->handle())
    return false;

  // if it fails the shader has never been attached to any GLSL program
  for(int i=0; i<(int)mShaders.size(); ++i)
  {
    if (mShaders[i] == shader)
    {
      scheduleRelinking();
      if ( shader->handle() )
        glDetachShader( handle(), shader->handle() ); VL_CHECK_OGL();
      mShaders.erase(mShaders.begin() + i);
      break;
    }
  }

  if ( shader->handle() )
    return true;
  else
    return false;
}
//-----------------------------------------------------------------------------
bool GLSLProgram::linkProgram(bool force_relink)
{
  VL_CHECK( GLEW_VERSION_2_0||GLEW_VERSION_3_0 )
  if( !(GLEW_VERSION_2_0||GLEW_VERSION_3_0) )
    return false;
  VL_CHECK_OGL();

  if (!linked() || force_relink)
  {
    if (shaderCount() == 0)
    {
      Log::error("GLSLProgram::linkProgram() called on a GLSLProgram with no shaders!\n");
      VL_TRAP()
      return false;
    }

    createProgram();

    // fragment shader color number binding

    if (GLEW_EXT_gpu_shader4||GLEW_VERSION_3_0)
    {
      std::map<std::string, int>::iterator it = mFragDataLocation.begin();
      while(it != mFragDataLocation.end())
      {
        VL_glBindFragDataLocation( handle(), it->second, it->first.c_str() );
        ++it;
      }
    }

    // geometry shader

    if (GLEW_EXT_geometry_shader4||GLEW_ARB_geometry_shader4)
    {
      // if there is at least one geometry shader applies the geometry shader parameters
      for(unsigned i=0; i<mShaders.size(); ++i)
      {
        if (mShaders[i]->type() == ST_GEOMETRY_SHADER)
        {
          VL_glProgramParameteri(handle(), GL_GEOMETRY_VERTICES_OUT_EXT, geometryVerticesOut());
          VL_glProgramParameteri(handle(), GL_GEOMETRY_INPUT_TYPE_EXT,   geometryInputType());
          VL_glProgramParameteri(handle(), GL_GEOMETRY_OUTPUT_TYPE_EXT,  geometryOutputType());
          break;
        }
      }
    }

    // automatically binds the specified attributes to the desired values

    for( std::map<std::string, int>::iterator it = mAttribLocation.begin(); it != mAttribLocation.end(); ++it)
    {
      glBindAttribLocation(handle(),it->second,it->first.c_str());
    }

    // link the program

    glLinkProgram(handle()); VL_CHECK_OGL();
    mScheduleLink = !linkStatus();

    // check link error
    if(!linked())
    {
      Log::error("GLSLProgram::linkProgram() failed:\n");
      Log::print("Name: '"+String(objectName().c_str()) + "'\n");
      Log::print( Say("Log:\n%s\n") << infoLog() );
      VL_TRAP()
      return false;
    }

    // populate uniform binding map

    mUniformLocation.clear();

    int uniform_count = 0;
    glGetProgramiv(handle(), GL_ACTIVE_UNIFORMS, &uniform_count);
    int uniform_len = 0;
    glGetProgramiv(handle(), GL_ACTIVE_UNIFORM_MAX_LENGTH, &uniform_len);

    std::vector<char> name;
    name.resize(uniform_len);
    if (name.size())
    {
      for(int i=0; i<uniform_count; ++i)
      {
        GLenum type;
        int size;
        glGetActiveUniform(handle(), i, uniform_len, NULL, &size, &type, &name[0]);
        mUniformLocation[&name[0]] = glGetUniformLocation(handle(), &name[0]);
      }
    }
  }

  return true;
}
//-----------------------------------------------------------------------------
bool GLSLProgram::linkStatus() const
{
  VL_CHECK( GLEW_VERSION_2_0||GLEW_VERSION_3_0 )
  if( !(GLEW_VERSION_2_0||GLEW_VERSION_3_0) )
    return false;
  VL_CHECK(handle()) // no shaders attached

  if (handle() == 0)
    return false;
  int status = 0;
  glGetProgramiv(handle(), GL_LINK_STATUS, &status); VL_CHECK_OGL();
  return status == GL_TRUE;
}
//-----------------------------------------------------------------------------
String GLSLProgram::infoLog() const
{
  VL_CHECK( GLEW_VERSION_2_0||GLEW_VERSION_3_0 )
  if( !(GLEW_VERSION_2_0||GLEW_VERSION_3_0) )
    return "OpenGL Shading Language not supported.\n";
  VL_CHECK(handle()) // no shaders attached

  if (handle() == 0)
    return "GLSLProgram::infoLog(): error! No shaders bound.";
  int max_length = 0;
  glGetProgramiv(handle(), GL_INFO_LOG_LENGTH, &max_length); VL_CHECK_OGL();
  std::vector<GLchar> log_buffer;
  log_buffer.resize(max_length+1);
  glGetProgramInfoLog(handle(), max_length, NULL, &log_buffer[0]); VL_CHECK_OGL();
  String info_log(&log_buffer[0]);
  return info_log;
}
//-----------------------------------------------------------------------------
bool GLSLProgram::validateProgram() const
{
  VL_CHECK( GLEW_VERSION_2_0||GLEW_VERSION_3_0 )
  if( !(GLEW_VERSION_2_0||GLEW_VERSION_3_0) )
    return false;
  VL_CHECK(handle()) // no shaders attached

  if (handle() == 0)
    return false;

  glValidateProgram(handle());
  int status = 0;
  glGetProgramiv(handle(), GL_VALIDATE_STATUS, &status); VL_CHECK_OGL();
  return status == GL_TRUE;
}
//-----------------------------------------------------------------------------
void GLSLProgram::bindAttribLocation(unsigned int index, const std::string& name)
{
  VL_CHECK( GLEW_VERSION_2_0||GLEW_VERSION_3_0 )

  createProgram();
  scheduleRelinking();
  glBindAttribLocation(handle(), index, name.c_str()); VL_CHECK_OGL()
}
//-----------------------------------------------------------------------------
int GLSLProgram::maxVertexAttributes()
{
  VL_CHECK( GLEW_VERSION_2_0||GLEW_VERSION_3_0 )
  if( !(GLEW_VERSION_2_0||GLEW_VERSION_3_0) )
    return 0;

  int max = 0;
  glGetIntegerv( GL_MAX_VERTEX_ATTRIBS, &max ); VL_CHECK_OGL()
  return max;
}
//-----------------------------------------------------------------------------
bool GLSLProgram::useProgram() const
{
  VL_CHECK( GLEW_VERSION_2_0||GLEW_VERSION_3_0 )
  if( !(GLEW_VERSION_2_0||GLEW_VERSION_3_0) )
    return false;
  VL_CHECK(handle())
  VL_CHECK(linked())

  if (!linked())
    return false;

  VL_CHECK(validateProgram())

  // bind the GLSL program
  glUseProgram(handle()); VL_CHECK_OGL()

  return true;
}
//-----------------------------------------------------------------------------
void GLSLProgram::apply(const Camera*, OpenGLContext*) const
{
  if(GLEW_VERSION_2_0||GLEW_VERSION_3_0)
  {
    if ( handle() )
      useProgram();
    else
      glUseProgram(0);
  }
}
//-----------------------------------------------------------------------------
bool GLSLProgram::applyUniformSet(const UniformSet* uniforms) const
{
  VL_CHECK_OGL();
  VL_CHECK( GLEW_VERSION_2_0||GLEW_VERSION_3_0 )
  if( !(GLEW_VERSION_2_0||GLEW_VERSION_3_0) )
    return false;
  if(!uniforms)
    return false;
  if (!linked())
    return false;
  if (!handle())
    return false;

  #ifndef NDEBUG
    int current_glsl_program = -1;
    glGetIntegerv(GL_CURRENT_PROGRAM, &current_glsl_program); VL_CHECK_OGL();
    VL_CHECK(current_glsl_program == (int)handle())
  #endif

  for(size_t i=0, count=uniforms->uniforms().size(); i<count; ++i)
  {
    Uniform* uniform = uniforms->uniforms()[i].get();

    #if 1
      std::map<std::string, int>::const_iterator it = mUniformLocation.find(uniform->name());
      int location = it == mUniformLocation.end() ? -1 : it->second;
    #else
      // for benchmarking purposes
      int location = glGetUniformLocation(handle(), uniform->name().c_str());
    #endif

    if (location == -1)
    {
      // Check the following:
      // (1) Is the uniform variable declared but not used in your GLSL program?
      // (2) Double-check the spelling of the uniform variable name.
      vl::Log::error( vl::Say("GLSLProgram::applyUniformSet(): uniform '%s' not found!\n"
                              "Is the uniform variable declared but not used in your GLSL program?\n"
                              "Also double-check the spelling of the uniform variable name.\n") << uniform->name() );
      VL_TRAP();
      return false;
    }

    // finally transmits the uniform
    // note: we don't perform delta binding per-uniform variable at the moment!

    VL_CHECK_OGL();
    switch(uniform->mType)
    {
      case UT_Int:    glUniform1iv(location, uniform->count(), uniform->intData()); VL_CHECK_OGL(); break;
      case UT_Int2:   glUniform2iv(location, uniform->count(), uniform->intData()); VL_CHECK_OGL(); break;
      case UT_Int3:   glUniform3iv(location, uniform->count(), uniform->intData()); VL_CHECK_OGL(); break;
      case UT_Int4:   glUniform4iv(location, uniform->count(), uniform->intData()); VL_CHECK_OGL(); break;

      case UT_UInt:   VL_glUniform1uiv(location, uniform->count(), uniform->uintData()); VL_CHECK_OGL(); break;
      case UT_UInt2:  VL_glUniform2uiv(location, uniform->count(), uniform->uintData()); VL_CHECK_OGL(); break;
      case UT_UInt3:  VL_glUniform3uiv(location, uniform->count(), uniform->uintData()); VL_CHECK_OGL(); break;
      case UT_UInt4:  VL_glUniform4uiv(location, uniform->count(), uniform->uintData()); VL_CHECK_OGL(); break;

      case UT_Float:  glUniform1fv(location, uniform->count(), uniform->floatData()); VL_CHECK_OGL(); break;
      case UT_Float2: glUniform2fv(location, uniform->count(), uniform->floatData()); VL_CHECK_OGL(); break;
      case UT_Float3: glUniform3fv(location, uniform->count(), uniform->floatData()); VL_CHECK_OGL(); break;
      case UT_Float4: glUniform4fv(location, uniform->count(), uniform->floatData()); VL_CHECK_OGL(); break;

      case UT_Mat2F:   glUniformMatrix2fv(location, uniform->count(), GL_FALSE, uniform->floatData()); VL_CHECK_OGL(); break;
      case UT_Mat3F:   glUniformMatrix3fv(location, uniform->count(), GL_FALSE, uniform->floatData()); VL_CHECK_OGL(); break;
      case UT_Mat4F:   glUniformMatrix4fv(location, uniform->count(), GL_FALSE, uniform->floatData()); VL_CHECK_OGL(); break;

      case UT_Mat2x3F: glUniformMatrix2x3fv(location, uniform->count(), GL_FALSE, uniform->floatData()); VL_CHECK_OGL(); break;
      case UT_Mat3x2F: glUniformMatrix3x2fv(location, uniform->count(), GL_FALSE, uniform->floatData()); VL_CHECK_OGL(); break;
      case UT_Mat2x4F: glUniformMatrix2x4fv(location, uniform->count(), GL_FALSE, uniform->floatData()); VL_CHECK_OGL(); break;
      case UT_Mat4x2F: glUniformMatrix4x2fv(location, uniform->count(), GL_FALSE, uniform->floatData()); VL_CHECK_OGL(); break;
      case UT_Mat3x4F: glUniformMatrix3x4fv(location, uniform->count(), GL_FALSE, uniform->floatData()); VL_CHECK_OGL(); break;
      case UT_Mat4x3F: glUniformMatrix4x3fv(location, uniform->count(), GL_FALSE, uniform->floatData()); VL_CHECK_OGL(); break;

      default:
        // Probably you added a uniform to a Shader or Actor but you forgot to specify it's data!
        vl::Log::error( vl::Say("GLSLProgram::applyUniformSet(): uniform '%s' does not contain any data!\n") << uniform->name() );
        VL_TRAP();
        break;
    }
  }
  VL_CHECK_OGL();
  return true;
}
//-----------------------------------------------------------------------------
void GLSLProgram::bindFragDataLocation(int color_number, const std::string& name)
{
  scheduleRelinking();
  mFragDataLocation[name] = color_number;
}
//-----------------------------------------------------------------------------
void GLSLProgram::unbindFragDataLocation(const std::string& name)
{
  scheduleRelinking();
  mFragDataLocation.erase(name);
}
//-----------------------------------------------------------------------------
int GLSLProgram::fragDataLocationBinding(const std::string& name) const
{
  std::map<std::string, int>::const_iterator it = mFragDataLocation.find(name);
  if (it != mFragDataLocation.end())
    return it->second;
  else
    return -1;
}
//-----------------------------------------------------------------------------
