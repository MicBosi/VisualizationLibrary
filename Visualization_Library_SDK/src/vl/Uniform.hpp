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

#ifndef Uniform_INCLUDE_ONCE
#define Uniform_INCLUDE_ONCE

#include <vl/vlnamespace.hpp>
#include <vl/Object.hpp>
#include <vl/Vector4.hpp>
#include <vl/Matrix4.hpp>
#include <cstring>
#include <map>
#include <vector>
#include <algorithm>

namespace vl
{
  //------------------------------------------------------------------------------
  // Uniform
  //------------------------------------------------------------------------------
  /**
   * Wraps an OpenGL Shading Language uniform to be associated to a GLSLProgram
   *
   * \sa
   * - GLSLProgram
   * - Shader
   * - Actor
   * - UniformSet
  */
  class Uniform: public Object
  {
    friend class GLSLProgram;
  public:
    typedef enum
    {
      NONE = 0x0,

      UT_Float,
      UT_Float2,
      UT_Float3,
      UT_Float4,

      UT_Int,
      UT_Int2,
      UT_Int3,
      UT_Int4,

      UT_UInt,
      UT_UInt2,
      UT_UInt3,
      UT_UInt4,

      UT_Mat2,
      UT_Mat3,
      UT_Mat4,

      UT_Mat2x3,
      UT_Mat3x2,
      UT_Mat2x4,
      UT_Mat4x2,
      UT_Mat3x4,
      UT_Mat4x3,
    } EUniformType;

  public:
    virtual const char* className() { return "Uniform"; }
    Uniform(): mType(NONE)
    {
      #ifndef NDEBUG
        mName = "Uniform";
      #endif
    }
    Uniform(const std::string& name): mType(NONE) 
    {
      mName = name;
    }

    // array setters

    void setUniform1i(int count, const int* value) { initInt(count*1); memcpy(&mIntData[0],value,sizeof(mIntData[0])*mIntData.size()); mType = UT_Int;  VL_WARN(GLEW_ARB_shading_language_100); }
    void setUniform2i(int count, const int* value) { initInt(count*2); memcpy(&mIntData[0],value,sizeof(mIntData[0])*mIntData.size()); mType = UT_Int2; VL_WARN(GLEW_ARB_shading_language_100); }
    void setUniform3i(int count, const int* value) { initInt(count*3); memcpy(&mIntData[0],value,sizeof(mIntData[0])*mIntData.size()); mType = UT_Int3; VL_WARN(GLEW_ARB_shading_language_100); }
    void setUniform4i(int count, const int* value) { initInt(count*4); memcpy(&mIntData[0],value,sizeof(mIntData[0])*mIntData.size()); mType = UT_Int4; VL_WARN(GLEW_ARB_shading_language_100); }

    void setUniform1ui(int count, const unsigned int* value) { initUInt(count*1); memcpy(&mUIntData[0],value,sizeof(mUIntData[0])*mUIntData.size()); mType = UT_UInt;  VL_WARN(GLEW_EXT_gpu_shader4); }
    void setUniform2ui(int count, const unsigned int* value) { initUInt(count*2); memcpy(&mUIntData[0],value,sizeof(mUIntData[0])*mUIntData.size()); mType = UT_UInt2; VL_WARN(GLEW_EXT_gpu_shader4); }
    void setUniform3ui(int count, const unsigned int* value) { initUInt(count*3); memcpy(&mUIntData[0],value,sizeof(mUIntData[0])*mUIntData.size()); mType = UT_UInt3; VL_WARN(GLEW_EXT_gpu_shader4); }
    void setUniform4ui(int count, const unsigned int* value) { initUInt(count*4); memcpy(&mUIntData[0],value,sizeof(mUIntData[0])*mUIntData.size()); mType = UT_UInt4; VL_WARN(GLEW_EXT_gpu_shader4); }

    void setUniform1f(int count, const float* value) { initFloat(count*1); memcpy(&mFloatData[0],value,sizeof(mFloatData[0])*mFloatData.size()); mType = UT_Float;  VL_WARN(GLEW_ARB_shading_language_100); }
    void setUniform2f(int count, const float* value) { initFloat(count*2); memcpy(&mFloatData[0],value,sizeof(mFloatData[0])*mFloatData.size()); mType = UT_Float2; VL_WARN(GLEW_ARB_shading_language_100); }
    void setUniform3f(int count, const float* value) { initFloat(count*3); memcpy(&mFloatData[0],value,sizeof(mFloatData[0])*mFloatData.size()); mType = UT_Float3; VL_WARN(GLEW_ARB_shading_language_100); }
    void setUniform4f(int count, const float* value) { initFloat(count*4); memcpy(&mFloatData[0],value,sizeof(mFloatData[0])*mFloatData.size()); mType = UT_Float4; VL_WARN(GLEW_ARB_shading_language_100); }

    // matrix array setters

    void setUniformMatrix2f(int count, const float* value) { initFloat(count*2*2); memcpy(&mFloatData[0],value,sizeof(mFloatData[0])*mFloatData.size()); mType = UT_Mat2; VL_WARN(GLEW_ARB_shading_language_100); }
    void setUniformMatrix3f(int count, const float* value) { initFloat(count*3*3); memcpy(&mFloatData[0],value,sizeof(mFloatData[0])*mFloatData.size()); mType = UT_Mat3; VL_WARN(GLEW_ARB_shading_language_100); }
    void setUniformMatrix4f(int count, const float* value) { initFloat(count*4*4); memcpy(&mFloatData[0],value,sizeof(mFloatData[0])*mFloatData.size()); mType = UT_Mat4; VL_WARN(GLEW_ARB_shading_language_100); }

    void setUniformMatrix2x3f(int count, const float* value) { initFloat(count*2*3); memcpy(&mFloatData[0],value,sizeof(mFloatData[0])*mFloatData.size()); mType = UT_Mat2x3; VL_WARN(GLEW_VERSION_2_1); }
    void setUniformMatrix3x2f(int count, const float* value) { initFloat(count*3*2); memcpy(&mFloatData[0],value,sizeof(mFloatData[0])*mFloatData.size()); mType = UT_Mat3x2; VL_WARN(GLEW_VERSION_2_1); }
    void setUniformMatrix2x4f(int count, const float* value) { initFloat(count*2*4); memcpy(&mFloatData[0],value,sizeof(mFloatData[0])*mFloatData.size()); mType = UT_Mat2x4; VL_WARN(GLEW_VERSION_2_1); }
    void setUniformMatrix4x2f(int count, const float* value) { initFloat(count*4*2); memcpy(&mFloatData[0],value,sizeof(mFloatData[0])*mFloatData.size()); mType = UT_Mat4x2; VL_WARN(GLEW_VERSION_2_1); }
    void setUniformMatrix3x4f(int count, const float* value) { initFloat(count*3*4); memcpy(&mFloatData[0],value,sizeof(mFloatData[0])*mFloatData.size()); mType = UT_Mat3x4; VL_WARN(GLEW_VERSION_2_1); }
    void setUniformMatrix4x3f(int count, const float* value) { initFloat(count*4*3); memcpy(&mFloatData[0],value,sizeof(mFloatData[0])*mFloatData.size()); mType = UT_Mat4x3; VL_WARN(GLEW_VERSION_2_1); }

    // vector/matrix array setters

    void setUniform(int count, const ivec2* value) { setUniform2i(count, value->ptr()); }
    void setUniform(int count, const ivec3* value) { setUniform3i(count, value->ptr()); }
    void setUniform(int count, const ivec4* value) { setUniform4i(count, value->ptr()); }

    void setUniform(int count, const uvec2* value) { setUniform2ui(count, value->ptr()); }
    void setUniform(int count, const uvec3* value) { setUniform3ui(count, value->ptr()); }
    void setUniform(int count, const uvec4* value) { setUniform4ui(count, value->ptr()); }

    void setUniform(int count, const fvec2* value)  { setUniform2f(count, value->ptr()); }
    void setUniform(int count, const fvec3* value)  { setUniform3f(count, value->ptr()); }
    void setUniform(int count, const fvec4* value)  { setUniform4f(count, value->ptr()); }

    void setUniform(int count, const fmat2* value)  { setUniformMatrix2f(count, value->ptr()); }
    void setUniform(int count, const fmat3* value)  { setUniformMatrix3f(count, value->ptr()); }
    void setUniform(int count, const fmat4* value)  { setUniformMatrix4f(count, value->ptr()); }

    // single value setters

    void setUniform(const int& value) { setUniform1i(1, &value); }
    void setUniform(const ivec2& value) { setUniform2i(1, value.ptr()); }
    void setUniform(const ivec3& value) { setUniform3i(1, value.ptr()); }
    void setUniform(const ivec4& value) { setUniform4i(1, value.ptr()); }

    void setUniform(const unsigned int& value){ setUniform1ui(1, &value); }
    void setUniform(const uvec2& value) { setUniform2ui(1, value.ptr()); }
    void setUniform(const uvec3& value) { setUniform3ui(1, value.ptr()); }
    void setUniform(const uvec4& value) { setUniform4ui(1, value.ptr()); }

    void setUniform(const float& value){ setUniform1f(1, &value); }
    void setUniform(const fvec2& value)  { setUniform2f(1, value.ptr()); }
    void setUniform(const fvec3& value)  { setUniform3f(1, value.ptr()); }
    void setUniform(const fvec4& value)  { setUniform4f(1, value.ptr()); }

    void setUniform(const fmat2& value)  { setUniformMatrix2f(1, value.ptr()); }
    void setUniform(const fmat3& value)  { setUniformMatrix3f(1, value.ptr()); }
    void setUniform(const fmat4& value)  { setUniformMatrix4f(1, value.ptr()); }

    // getters

    void getUniform(float* value)        { VL_CHECK(type() != NONE); VL_CHECK(mFloatData.size()); memcpy( value, &mFloatData[0], sizeof( mFloatData[0] ) * mFloatData.size() ); }
    void getUniform(int* value)          { VL_CHECK(type() != NONE); VL_CHECK(mIntData.size());   memcpy( value, &mIntData[0],   sizeof( mIntData[0]   ) * mIntData.size()   ); }
    void getUniform(unsigned int* value) { VL_CHECK(type() != NONE); VL_CHECK(mUIntData.size());  memcpy( value, &mUIntData[0],  sizeof( mUIntData[0]  ) * mUIntData.size()  ); }

    void getUniform(ivec2* value) { getUniform(value->ptr()); }
    void getUniform(ivec3* value) { getUniform(value->ptr()); }
    void getUniform(ivec4* value) { getUniform(value->ptr()); }

    void getUniform(uvec2* value) { getUniform(value->ptr()); }
    void getUniform(uvec3* value) { getUniform(value->ptr()); }
    void getUniform(uvec4* value) { getUniform(value->ptr()); }

    void getUniform(fvec2* value) { getUniform(value->ptr()); }
    void getUniform(fvec3* value) { getUniform(value->ptr()); }
    void getUniform(fvec4* value) { getUniform(value->ptr()); }

    void getUniform(fmat2* value) { getUniform(value->ptr()); }
    void getUniform(fmat3* value) { getUniform(value->ptr()); }
    void getUniform(fmat4* value) { getUniform(value->ptr()); }

    EUniformType type() const { return mType; }

    int count() const
    {
      switch(mType)
      {
        case Uniform::UT_Int:  return (int)mIntData.size();      
        case Uniform::UT_Int2: return (int)mIntData.size() / 2;      
        case Uniform::UT_Int3: return (int)mIntData.size() / 3;      
        case Uniform::UT_Int4: return (int)mIntData.size() / 4;

        case Uniform::UT_UInt:  return (int)mUIntData.size();      
        case Uniform::UT_UInt2: return (int)mUIntData.size() / 2;      
        case Uniform::UT_UInt3: return (int)mUIntData.size() / 3;      
        case Uniform::UT_UInt4: return (int)mUIntData.size() / 4;

        case Uniform::UT_Float:  return (int)mFloatData.size();
        case Uniform::UT_Float2: return (int)mFloatData.size() / 2;
        case Uniform::UT_Float3: return (int)mFloatData.size() / 3;
        case Uniform::UT_Float4: return (int)mFloatData.size() / 4;      

        case Uniform::UT_Mat2: return (int)mFloatData.size() / (2*2);
        case Uniform::UT_Mat3: return (int)mFloatData.size() / (3*3);      
        case Uniform::UT_Mat4: return (int)mFloatData.size() / (4*4);      

        case Uniform::UT_Mat2x3: return (int)mFloatData.size() / (2*3);
        case Uniform::UT_Mat3x2: return (int)mFloatData.size() / (3*2);
        case Uniform::UT_Mat2x4: return (int)mFloatData.size() / (2*4);
        case Uniform::UT_Mat4x2: return (int)mFloatData.size() / (4*2);
        case Uniform::UT_Mat3x4: return (int)mFloatData.size() / (3*4);
        case Uniform::UT_Mat4x3: return (int)mFloatData.size() / (4*3);

        default:
        // never reached
        VL_TRAP()
        return -1;
      }
    }

  protected:
    void initFloat(int count) { mFloatData.resize(count); mIntData.clear(); mUIntData.clear(); }
    void initInt(int count)   { mFloatData.clear(); mIntData.resize(count); mUIntData.clear(); }
    void initUInt(int count)  { mFloatData.clear(); mIntData.clear(); mUIntData.resize(count); }

    std::vector<float> mFloatData;
    std::vector<int>   mIntData;
    std::vector<unsigned int> mUIntData;
    EUniformType mType;
  };
  //------------------------------------------------------------------------------
  // UniformSet
  //------------------------------------------------------------------------------
  /**
   * A set of Uniform objects managed by a Shader.
   *
   * \sa 
   * Shader, Effect, Actor
  */
  class UniformSet: public Object
  {
  public:
    virtual const char* className() { return "UniformSet"; }
    UniformSet()
    {
      #ifndef NDEBUG
        mName = "UniformSet";
      #endif
    }

    // uniform getters and setters

    void setUniform(Uniform* uniform, bool check_for_doubles = true) 
    { 
      VL_CHECK(uniform)
      if (uniform == NULL)
        return;
      if ( check_for_doubles )
      {
        for(unsigned i=0; i<mUniforms.size(); ++i)
        {
          if (mUniforms[i]->name() == uniform->name())
          {
            mUniforms[i] = uniform;
            return;
          }
        }
      }
      mUniforms.push_back( uniform );
    }
    const std::vector< ref<Uniform> >& uniforms() const { return mUniforms; }
    void eraseUniform(const std::string& name) 
    { 
      for(unsigned i=0; i<mUniforms.size(); ++i)
        if (mUniforms[i]->name() == name)
        {
          mUniforms.erase( mUniforms.begin() + i );
          return;
        }
    }
    void eraseUniform(const Uniform* uniform) 
    { 
      for(unsigned i=0; i<mUniforms.size(); ++i)
        if (mUniforms[i] == uniform)
        {
          mUniforms.erase( mUniforms.begin() + i );
          return;
        }
    }
    void eraseAllUniforms() { mUniforms.clear(); }
    Uniform* gocUniform(const std::string& name)
    { 
      for(unsigned i=0; i<mUniforms.size(); ++i)
        if (mUniforms[i]->name() == name)
          return mUniforms[i].get();
      ref<Uniform> uniform = new Uniform;
      uniform->setName( name );
      mUniforms.push_back(uniform);
      return uniform.get();
    }
    Uniform* getUniform(const std::string& name)
    { 
      for(unsigned i=0; i<mUniforms.size(); ++i)
        if (mUniforms[i]->name() == name)
          return mUniforms[i].get();
      return NULL;
    }
    const Uniform* getUniform(const std::string& name) const
    { 
      for(unsigned i=0; i<mUniforms.size(); ++i)
        if (mUniforms[i]->name() == name)
          return mUniforms[i].get();
      return NULL;
    }
  protected:
    std::vector< ref<Uniform> > mUniforms;
  };
  //------------------------------------------------------------------------------
}

#endif
