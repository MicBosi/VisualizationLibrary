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
    virtual const char* className() { return "Uniform"; }
    Uniform(): mType(NONE)
    {
      #ifndef NDEBUG
        mObjectName = className();
      #endif
    }
    Uniform(const std::string& name): mType(NONE) 
    {
      #ifndef NDEBUG
        mObjectName = className();
      #endif
      mName = name;
    }

    //! Returns the name of the uniform variable
    const std::string& name() const { return mName; }
    //! Returns the name of the uniform variable
    std::string& name() { return mName; }
    //! Sets the name of the uniform variable
    void setName(const std::string& name) { mName = name; }

    // array setters

    void setUniform1i(int count, const int* value) { initData(count*1); memcpy(&mData[0],value,sizeof(mData[0])*mData.size()); mType = UT_Int;  VL_CHECK( GLEW_VERSION_2_0||GLEW_VERSION_3_0 ); }
    void setUniform2i(int count, const int* value) { initData(count*2); memcpy(&mData[0],value,sizeof(mData[0])*mData.size()); mType = UT_Int2; VL_CHECK( GLEW_VERSION_2_0||GLEW_VERSION_3_0 ); }
    void setUniform3i(int count, const int* value) { initData(count*3); memcpy(&mData[0],value,sizeof(mData[0])*mData.size()); mType = UT_Int3; VL_CHECK( GLEW_VERSION_2_0||GLEW_VERSION_3_0 ); }
    void setUniform4i(int count, const int* value) { initData(count*4); memcpy(&mData[0],value,sizeof(mData[0])*mData.size()); mType = UT_Int4; VL_CHECK( GLEW_VERSION_2_0||GLEW_VERSION_3_0 ); }

    void setUniform1ui(int count, const unsigned int* value) { initData(count*1); memcpy(&mData[0],value,sizeof(mData[0])*mData.size()); mType = UT_UInt;  VL_CHECK(GLEW_EXT_gpu_shader4||GLEW_VERSION_3_0); }
    void setUniform2ui(int count, const unsigned int* value) { initData(count*2); memcpy(&mData[0],value,sizeof(mData[0])*mData.size()); mType = UT_UInt2; VL_CHECK(GLEW_EXT_gpu_shader4||GLEW_VERSION_3_0); }
    void setUniform3ui(int count, const unsigned int* value) { initData(count*3); memcpy(&mData[0],value,sizeof(mData[0])*mData.size()); mType = UT_UInt3; VL_CHECK(GLEW_EXT_gpu_shader4||GLEW_VERSION_3_0); }
    void setUniform4ui(int count, const unsigned int* value) { initData(count*4); memcpy(&mData[0],value,sizeof(mData[0])*mData.size()); mType = UT_UInt4; VL_CHECK(GLEW_EXT_gpu_shader4||GLEW_VERSION_3_0); }

    void setUniform1f(int count, const float* value) { initData(count*1); memcpy(&mData[0],value,sizeof(mData[0])*mData.size()); mType = UT_Float;  VL_CHECK( GLEW_VERSION_2_0||GLEW_VERSION_3_0 ); }
    void setUniform2f(int count, const float* value) { initData(count*2); memcpy(&mData[0],value,sizeof(mData[0])*mData.size()); mType = UT_Float2; VL_CHECK( GLEW_VERSION_2_0||GLEW_VERSION_3_0 ); }
    void setUniform3f(int count, const float* value) { initData(count*3); memcpy(&mData[0],value,sizeof(mData[0])*mData.size()); mType = UT_Float3; VL_CHECK( GLEW_VERSION_2_0||GLEW_VERSION_3_0 ); }
    void setUniform4f(int count, const float* value) { initData(count*4); memcpy(&mData[0],value,sizeof(mData[0])*mData.size()); mType = UT_Float4; VL_CHECK( GLEW_VERSION_2_0||GLEW_VERSION_3_0 ); }

    // matrix array setters

    void setUniformMatrix2f(int count, const float* value) { initData(count*2*2); memcpy(&mData[0],value,sizeof(mData[0])*mData.size()); mType = UT_Mat2F; VL_CHECK( GLEW_VERSION_2_0||GLEW_VERSION_3_0 ); }
    void setUniformMatrix3f(int count, const float* value) { initData(count*3*3); memcpy(&mData[0],value,sizeof(mData[0])*mData.size()); mType = UT_Mat3F; VL_CHECK( GLEW_VERSION_2_0||GLEW_VERSION_3_0 ); }
    void setUniformMatrix4f(int count, const float* value) { initData(count*4*4); memcpy(&mData[0],value,sizeof(mData[0])*mData.size()); mType = UT_Mat4F; VL_CHECK( GLEW_VERSION_2_0||GLEW_VERSION_3_0 ); }

    void setUniformMatrix2x3f(int count, const float* value) { initData(count*2*3); memcpy(&mData[0],value,sizeof(mData[0])*mData.size()); mType = UT_Mat2x3F; VL_CHECK(GLEW_VERSION_2_1||GLEW_VERSION_3_0); }
    void setUniformMatrix3x2f(int count, const float* value) { initData(count*3*2); memcpy(&mData[0],value,sizeof(mData[0])*mData.size()); mType = UT_Mat3x2F; VL_CHECK(GLEW_VERSION_2_1||GLEW_VERSION_3_0); }
    void setUniformMatrix2x4f(int count, const float* value) { initData(count*2*4); memcpy(&mData[0],value,sizeof(mData[0])*mData.size()); mType = UT_Mat2x4F; VL_CHECK(GLEW_VERSION_2_1||GLEW_VERSION_3_0); }
    void setUniformMatrix4x2f(int count, const float* value) { initData(count*4*2); memcpy(&mData[0],value,sizeof(mData[0])*mData.size()); mType = UT_Mat4x2F; VL_CHECK(GLEW_VERSION_2_1||GLEW_VERSION_3_0); }
    void setUniformMatrix3x4f(int count, const float* value) { initData(count*3*4); memcpy(&mData[0],value,sizeof(mData[0])*mData.size()); mType = UT_Mat3x4F; VL_CHECK(GLEW_VERSION_2_1||GLEW_VERSION_3_0); }
    void setUniformMatrix4x3f(int count, const float* value) { initData(count*4*3); memcpy(&mData[0],value,sizeof(mData[0])*mData.size()); mType = UT_Mat4x3F; VL_CHECK(GLEW_VERSION_2_1||GLEW_VERSION_3_0); }

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

    void getUniform(float* value)        { VL_CHECK(type() != NONE); VL_CHECK(mData.size()); memcpy( value, &mData[0], sizeof( mData[0] ) * mData.size() ); }
    void getUniform(int* value)          { VL_CHECK(type() != NONE); VL_CHECK(mData.size());   memcpy( value, &mData[0],   sizeof( mData[0]   ) * mData.size()   ); }
    void getUniform(unsigned int* value) { VL_CHECK(type() != NONE); VL_CHECK(mData.size());  memcpy( value, &mData[0],  sizeof( mData[0]  ) * mData.size()  ); }

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
        case UT_Int:    return (int)mData.size();      
        case UT_Int2:   return (int)mData.size() / 2;      
        case UT_Int3:   return (int)mData.size() / 3;      
        case UT_Int4:   return (int)mData.size() / 4;

        case UT_UInt:   return (int)mData.size();      
        case UT_UInt2:  return (int)mData.size() / 2;      
        case UT_UInt3:  return (int)mData.size() / 3;      
        case UT_UInt4:  return (int)mData.size() / 4;

        case UT_Float:  return (int)mData.size();
        case UT_Float2: return (int)mData.size() / 2;
        case UT_Float3: return (int)mData.size() / 3;
        case UT_Float4: return (int)mData.size() / 4;      

        case UT_Mat2F:   return (int)mData.size() / (2*2);
        case UT_Mat3F:   return (int)mData.size() / (3*3);      
        case UT_Mat4F:   return (int)mData.size() / (4*4);      

        case UT_Mat2x3F: return (int)mData.size() / (2*3);
        case UT_Mat3x2F: return (int)mData.size() / (3*2);
        case UT_Mat2x4F: return (int)mData.size() / (2*4);
        case UT_Mat4x2F: return (int)mData.size() / (4*2);
        case UT_Mat3x4F: return (int)mData.size() / (3*4);
        case UT_Mat4x3F: return (int)mData.size() / (4*3);

        default:
        // never reached
        VL_TRAP()
        return -1;
      }
    }

  protected:
    VL_COMPILE_TIME_CHECK( sizeof(int) == sizeof(float) );
    void initData(int count) { mData.resize(count); }
    float* floatData() { VL_CHECK(!mData.empty()); return (float*)&mData[0]; }
    int* intData() { VL_CHECK(!mData.empty()); return (int*)&mData[0]; }
    unsigned int* uintData() { VL_CHECK(!mData.empty()); return (unsigned int*)&mData[0]; }

    EUniformType mType;
    std::vector<int> mData;
    std::string mName;
  };
}

#endif
