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

#ifndef UniformSet_INCLUDE_ONCE
#define UniformSet_INCLUDE_ONCE

#include <vl/Object.hpp>
#include <vl/Uniform.hpp>

namespace vl
{
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
        mObjectName = className();
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
}

#endif
