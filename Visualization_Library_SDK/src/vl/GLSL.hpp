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

#ifndef GLSL_INCLUDE_ONCE
#define GLSL_INCLUDE_ONCE

#include <vl/Uniform.hpp>
#include <vl/GLSLmath.hpp>
#include <vl/Object.hpp>
#include <vl/RenderState.hpp>
#include <vl/String.hpp>

namespace vl
{
  class Uniform;
  //------------------------------------------------------------------------------
  // GLSLShader
  //------------------------------------------------------------------------------
  /**
   * Base class for GLSLVertexShader, GLSLFragmentShader and GLSLGeometryShader
   *
   * \sa GLSLProgram, Shader, Effect, Actor
  */
  class GLSLShader: public Object
  {
  public:
    GLSLShader(EShaderType type, const String& source=String());

    ~GLSLShader();

    virtual const char* className() { return "GLSLShader"; }

    EShaderType type() const { return mType; }

    //! Sets the sources for this shader and schedules a recompilation for it
    void setSource( const std::string& source );
    //! Sets the sources for this shader and schedules a recompilation for it
    void setSource( const String& source );
    //! Returns the sources for this shader
    const std::string& source() const { return mSource; }

    //! Compiles the shader, see also http://www.opengl.org/sdk/docs/man/xhtml/glCompileShader.xml for more information.
    //! This function also create the shader if handle() == 0 using the OpenGL function glCreateShader(), see also http://www.opengl.org/sdk/docs/man/xhtml/glCreateShader.xml
    bool compile();

    //! Returns true if the shader has been succesfully compiled.
    //! The check is done using the OpenGL function glGetShaderiv(), see also http://www.opengl.org/sdk/docs/man/xhtml/glGetShader.xml for more information.
    bool compileStatus() const;

    //! Returns a String object containing this shader's info log as returned by glGetShaderInfoLog(), see also http://www.opengl.org/sdk/docs/man/xhtml/glGetShaderInfoLog.xml for more information.
    String infoLog() const;

    //! Creates the shader using the OpenGL function glCreateShader(), see also http://www.opengl.org/sdk/docs/man/xhtml/glCreateShader.xml for more information.
    void createShader();

    //! Deletes the shader using the OpenGL function glDeleteShader(), see also http://www.opengl.org/sdk/docs/man/xhtml/glDeleteShader.xml for more information.
    void deleteShader();

    //! The handle of this OpenGL shader object as returned by glCreateShader()
    unsigned int handle() const { return mHandle; }

  protected:
    EShaderType mType;
    std::string mSource;
    unsigned int mHandle;
    bool mCompiled;
  };
  //------------------------------------------------------------------------------
  /**
   * Wraps a GLSL vertex shader to be bound to a GLSL program.
   *
   * \sa GLSLProgram, GLSLFragmentShader, GLSLGeometryShader, Shader, Effect
  */
  class GLSLVertexShader: public GLSLShader
  {
  public:
    //! Constructor.
    //! \param source Vertex shader's source code or path to a text file containing the vertex shader's source code.
    GLSLVertexShader(const String& source=String()): GLSLShader(ST_VERTEX_SHADER, source)
    {
      #ifndef NDEBUG
        mName = "GLSLVertexShader";
      #endif
    }
    virtual const char* className() { return "GLSLVertexShader"; }
  };
  //------------------------------------------------------------------------------
  /**
   * Wraps a GLSL fragment shader to be bound to a GLSL program.
   *
   * \sa GLSLProgram, GLSLVertexShader, GLSLGeometryShader, Shader, Effect
  */
  class GLSLFragmentShader: public GLSLShader
  {
  public:
    //! \param source Fragment shader's source code or path to a text file containing the fragment shader's source code.
    GLSLFragmentShader(const String& source=String()): GLSLShader(ST_FRAGMENT_SHADER, source)
    {
      #ifndef NDEBUG
        mName = "GLSLFragmentShader";
      #endif
    }
    virtual const char* className() { return "GLSLFragmentShader"; }
  };
  //------------------------------------------------------------------------------
  /**
   * Wraps a GLSL geometry shader to be bound to a GLSL program.
   *
   * \sa GLSLProgram, GLSLFragmentShader, GLSLVertexShader, Shader, Effect
  */
  class GLSLGeometryShader: public GLSLShader
  {
  public:
    //! \param source Geometry shader's source code or path to a text file containing the geometry shader's source code.
    GLSLGeometryShader(const String& source=String()): GLSLShader(ST_GEOMETRY_SHADER, source)
    {
      #ifndef NDEBUG
        mName = "GLSLGeometryShader";
      #endif
    }
    virtual const char* className() { return "GLSLGeometryShader"; }
  };
  //------------------------------------------------------------------------------
  // GLSLProgram
  //------------------------------------------------------------------------------
  /**
   * Wraps a GLSL program to which you can bind vertex, fragment and geometry shaders.
   *
   * \par Uniforms
   * You have 3 ways to set the value of a uniform:
   * -# call useProgram() to activate the GLSLProgram and directly call glUniform* (see also getUniformLocation()).
   * -# add a Uniform to the Actor's UniformSet, see Actor::uniformSet()
   * -# add a Uniform to the Actor's Shader UniformSet, see Shader::uniformSet()
   *
   * \remarks
   * The Uniforms defined in the Actor and the ones defined in the Shader must not
   * overlap, that is, an Actor must  not define Uniforms present in the Shader and
   * vice versa.
   *
   * \sa
   * - GLSLVertexShader
   * - GLSLFragmentShader
   * - GLSLGeometryShader
   * - Shader
   * - Effect
   * - Actor::renderingCallbacks()
  */
  class GLSLProgram: public RenderState
  {
    // applyUniform
    friend class Renderer;
  public:
    //! Constructor.
    GLSLProgram();

    //! Destructor. Calls deleteProgram().
    ~GLSLProgram();

    virtual const char* className() { return "GLSLProgram"; }

    //! \internal
    virtual ERenderState type() const { return RS_GLSLProgram; }

    //! Calls glCreateProgram() in order to acquire a GLSL program handle, see also http://www.opengl.org/sdk/docs/man/xhtml/glCreateProgram.xml for more information.
    //! \note
    //! The program is created only if handle() == 0
    void createProgram();

    //! Deletes the GLSL program calling glDeleteProgram(handle()), see also http://www.opengl.org/sdk/docs/man/xhtml/glDeleteProgram.xml for more information.
    //! After this function handle() will return 0.
    void deleteProgram();

    //! The handle of the GLSL program as returned by glCreateProgram()
    //! \sa
    //! http://www.opengl.org/sdk/docs/man/xhtml/glCreateProgram.xml
    //! - createProgram()
    //! - deleteProgram()
    unsigned int handle() const { return mHandle; }

    //! Equivalent to glUseProgram(handle()), see also http://www.opengl.org/sdk/docs/man/xhtml/glUseProgram.xml for more information.
    bool useProgram() const;

    //! Calls useProgram()
    void apply(const Camera*) const;

    //! Calls linkProgram() and applyes this program's uniforms.
    //! \sa setUniform() setUniformSet() uniformSet()
    virtual void initResources();

    //! Links the GLSLProgram calling glLinkProgram(handle()) only if the program needs to be linked.
    //! \sa
    //! - http://www.opengl.org/sdk/docs/man/xhtml/glLinkProgram.xml
    //! - scheduleRelinking()
    //! - linked()
    bool linkProgram(bool force_relink = false);

    bool linkStatus() const;

    //! Returns true if the program has been succesfully linked.
    bool linked() const { return !mScheduleLink; }

    //! Schedules a relink of the GLSL program.
    void scheduleRelinking();

    /**
     * Attaches the GLSLShader to this GLSLProgram
     * \note
     * Attaching a shader triggers the compilation of the shader (if not already compiled) and relinking of the program.
    */
    bool attachShader(GLSLShader* shader);

    //! Removes a GLSLShader from the GLSLShader and schedules a relink of the program, see also http://www.opengl.org/sdk/docs/man/xhtml/glDetachShader.xml for more information.
    bool detachShader(GLSLShader* shader);

    //! Returns the info log of this GLSL program using the OpenGL function glGetProgramInfoLog(), see also http://www.opengl.org/sdk/docs/man/xhtml/glGetProgramInfoLog.xml for more information.
    String infoLog() const;

    //! Returns true if the validation of this GLSL program is succesful, see also http://www.opengl.org/sdk/docs/man/xhtml/glValidateProgram.xml for more information.
    bool validateProgram() const;

    //! Equivalent to glBindAttribLocation(handle(), index, name.c_str())
    void bindAttribLocation(unsigned int index, const std::string& name);

    //! Eqivalento to glGetAttribLocation(handle(), name).
    //! \note The program must be linked before calling this function.
    int getAttribLocation(const char* name) const
    {
      VL_WARN(GLEW_ARB_shading_language_100)
      if (!GLEW_ARB_shading_language_100)
        return -1;
      VL_CHECK(handle())
      VL_CHECK(linked())
      int location = glGetAttribLocation(handle(), name);
      return location;
    }

    //! Equivalent to glGetIntegerv( GL_MAX_VERTEX_ATTRIBS, &max )
    static int maxVertexAttributes();

    //! Returns the number of GLSLShader objects bound to this GLSLProgram
    int shaderCount() const { return (int)mShaders.size(); }
    //! Returns the i-th GLSLShader objects bound to this GLSLProgram
    const GLSLShader* shader(int i) const { return mShaders[i].get(); }
    //! Returns the i-th GLSLShader objects bound to this GLSLProgram
    GLSLShader* shader(int i) { return mShaders[i].get(); }
    //! Removes all the previously linked shaders and schedules a relinking
    void detachAllShaders();

    // --------------- bind frag data location ---------------

    void bindFragDataLocation(int color_number, const std::string& name);
    void unbindFragDataLocation(const std::string& name);
    int fragDataLocationBinding(const std::string& name) const;

    // --------------- geometry shader ---------------

    //! See GL_ARB_geometry_shader4's GL_GEOMETRY_VERTICES_OUT_EXT
    void setGeometryVerticesOut(int vertex_count) { scheduleRelinking(); mGeometryVerticesOut = vertex_count; }
    //! See GL_ARB_geometry_shader4's GL_GEOMETRY_VERTICES_OUT_EXT
    int geometryVerticesOut() const { return mGeometryVerticesOut; }

    //! See GL_ARB_geometry_shader4's GL_GEOMETRY_INPUT_TYPE_EXT
    void setGeometryInputType(EGeometryInputType type) { scheduleRelinking(); mGeometryInputType = type; }
    //! See GL_ARB_geometry_shader4's GL_GEOMETRY_INPUT_TYPE_EXT
    EGeometryInputType geometryInputType() const { return mGeometryInputType; }

    //! See GL_ARB_geometry_shader4's GL_GEOMETRY_OUTPUT_TYPE_EXT
    void setGeometryOutputType(EGeometryOutputType type) { scheduleRelinking(); mGeometryOutputType = type; }
    //! See GL_ARB_geometry_shader4's GL_GEOMETRY_OUTPUT_TYPE_EXT
    EGeometryOutputType geometryOutputType() const { return mGeometryOutputType; }

    // --------------- uniform variables ---------------

    /**
     * Applies a set of uniforms to the currently bound GLSL program.
     * This function expects the GLSLProgram to be already bound, see useProgram().
    */
    void applyUniformSet(const UniformSet* uniforms) const;

    /**
     * Returns the binding index of the uniform with the given name.
    */
    int getUniformLocation(const std::string& name) const
    {
      VL_WARN(GLEW_ARB_shading_language_100)
      if (!GLEW_ARB_shading_language_100)
        return -1;
      VL_CHECK(linked())
      VL_CHECK(handle())

      int location = glGetUniformLocation(handle(), name.c_str());
      VL_CHECK(location != -1)
      return location;
    }

    // --------------- uniform variables: getters ---------------

    // general uniform getters: use these to access to all the types supported by your GLSL implementation,
    // and not only the ordinary fvec2, fvec3, fvec4, ivec2, ivec3, ivec4, fmat2, fmat3, fmat4

    //! Equivalent to glGetUniformfv(handle(), location, params)
    void getUniformfv(int location, float* params) const
    {
      VL_WARN(GLEW_ARB_shading_language_100)
      if (!GLEW_ARB_shading_language_100)
        return;
      VL_CHECK(linked())
      VL_CHECK(handle())
      glGetUniformfv(handle(), location, params); VL_CHECK_OGL()
    }
    //! Equivalent to getUniformfv(getUniformLocation(name), params)
    void getUniformfv(const std::string& name, float* params) const { getUniformfv(getUniformLocation(name), params); }
    //! Equivalent to glGetUniformiv(handle(), location, params)
    void getUniformiv(int location, int* params) const
    {
      VL_WARN(GLEW_ARB_shading_language_100)
      if (!GLEW_ARB_shading_language_100)
        return;
      VL_CHECK(linked())
      VL_CHECK(handle())
      glGetUniformiv(handle(), location, params); VL_CHECK_OGL()
    }
    //! Equivalent to getUniformiv(getUniformLocation(name)
    void getUniformiv(const std::string& name, int* params) const { getUniformiv(getUniformLocation(name), params); }

    // utility functions for fvec2, fvec3, fvec4, ivec2, ivec3, ivec4, fmat2, fmat3, fmat4

    void getUniform(int location, fvec2& vec) const { getUniformfv(location, vec.ptr()); }
    void getUniform(int location, fvec3& vec) const { getUniformfv(location, vec.ptr()); }
    void getUniform(int location, fvec4& vec) const { getUniformfv(location, vec.ptr()); }
    void getUniform(int location, fmat2& mat) const { getUniformfv(location, mat.ptr()); }
    void getUniform(int location, fmat3& mat) const { getUniformfv(location, mat.ptr()); }
    void getUniform(int location, fmat4& mat) const { getUniformfv(location, mat.ptr()); }
    void getUniform(int location, ivec2& vec) const { getUniformiv(location, vec.ptr()); }
    void getUniform(int location, ivec3& vec) const { getUniformiv(location, vec.ptr()); }
    void getUniform(int location, ivec4& vec) const { getUniformiv(location, vec.ptr()); }
    void getUniform(const std::string& name, fvec2& vec) const { getUniform(getUniformLocation(name), vec); }
    void getUniform(const std::string& name, fvec3& vec) const { getUniform(getUniformLocation(name), vec); }
    void getUniform(const std::string& name, fvec4& vec) const { getUniform(getUniformLocation(name), vec); }
    void getUniform(const std::string& name, fmat2& mat) const { getUniform(getUniformLocation(name), mat); }
    void getUniform(const std::string& name, fmat3& mat) const { getUniform(getUniformLocation(name), mat); }
    void getUniform(const std::string& name, fmat4& mat) const { getUniform(getUniformLocation(name), mat); }
    void getUniform(const std::string& name, ivec2& vec) const { getUniform(getUniformLocation(name), vec); }
    void getUniform(const std::string& name, ivec3& vec) const { getUniform(getUniformLocation(name), vec); }
    void getUniform(const std::string& name, ivec4& vec) const { getUniform(getUniformLocation(name), vec); }

    //! Returns a GLSLProgram's 'static' UniformSet.
    //! \sa See setUniform() for more information about 'static' Uniforms and Uniform management.
    UniformSet* uniformSet() { return mUniformSet.get(); }
    //! Returns a GLSLProgram's 'static' UniformSet.
    //! \sa See setUniform() for more information about 'static' Uniforms and Uniform management.
    const UniformSet* uniformSet() const { return mUniformSet.get(); }
    //! Sets a GLSLProgram's 'static' UniformSet.
    //! \sa See setUniform() for more information about 'static' Uniforms and Uniform management.
    void setUniformSet(UniformSet* uniforms) { mUniformSet = uniforms; }
    //! Sets a GLSLProgram's 'static' Uniform.
    //! This is an utility function, it is equivalent to:
    //! \code 
    //! if (!uniformSet()) setUniformSet(new UniformSet); uniformSet()->setUniform(uniform);
    //! \endcode
    void setUniform(Uniform* uniform) { if (!uniformSet()) setUniformSet(new UniformSet); uniformSet()->setUniform(uniform); }
    //! Returns the specified Uniform. Returns NULL if there isn't such a Uniform
    Uniform* getUniform(const std::string& name) { if (!uniformSet()) return NULL; return uniformSet()->getUniform(name); }
    //! Gets or creates the specified Uniform.
    Uniform* gocUniform(const std::string& name) { if (!uniformSet()) setUniformSet(new UniformSet); return uniformSet()->gocUniform(name); }

  protected:
    std::vector< ref<GLSLShader> > mShaders;
    std::map<std::string, int> mFragDataLocation;
    std::map<std::string, int> mUniformLocation;
    ref<UniformSet> mUniformSet;
    unsigned int mHandle;
    bool mScheduleLink;
    int mGeometryVerticesOut;
    EGeometryInputType mGeometryInputType;
    EGeometryOutputType mGeometryOutputType;
  };
}

#endif
