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

#ifndef ShaderNode_INCLUDE_ONCE
#define ShaderNode_INCLUDE_ONCE

#include <vlGraphics/Shader.hpp>
#include <vlCore/vlnamespace.hpp>
#include <vector>
#include <map>

namespace vl
{
  /**
   * The ShaderNode class is used to conveniently manage complex hierarchies of Shader[s].
   */
  class ShaderNode: public Object
  {
  private:
    class EnableInfo
    {
    public:
      EnableInfo(EEnable en=EN_UnknownEnable, bool on=false, EInheritance inheritance=IN_Propagate): mEnable(en), mOn(on), mInheritance(inheritance) {}
      EEnable mEnable;
      bool mOn;
      EInheritance mInheritance;
    };
    class RenderStateInfo
    {
    public:
      RenderStateInfo(RenderState* rs=NULL, EInheritance inheritance=IN_Propagate): mRenderState(rs), mInheritance(inheritance) {}
      ref<RenderState> mRenderState;
      EInheritance mInheritance;
    };
    class UniformInfo
    {
    public:
      UniformInfo(Uniform* unif=NULL, EInheritance inheritance=IN_Propagate): mUniform(unif), mInheritance(inheritance) {}
      ref<Uniform> mUniform;
      EInheritance mInheritance;
    };

  public:
    virtual const char* className() { return "ShaderNode"; }

    ShaderNode(): mParent(NULL) {}

    // tree-related functions

    void addChild(ShaderNode* node)
    { 
      VL_CHECK(node->parent() == NULL);
      mNodes.push_back(node);
      node->mParent = this;
    }

    void eraseChild(ShaderNode* node)
    {
      VL_CHECK(node->parent() == this);
      for(unsigned i=0; i<childrenCount(); ++i)
        if (child(i) == node)     
        {
          mNodes.erase(mNodes.begin()+i);
          node->mParent = NULL;
          return;
        }
    }

    void eraseChild(unsigned index)
    {
      VL_CHECK(index<childrenCount())
      mNodes[index]->mParent = NULL;
      mNodes.erase(mNodes.begin()+index);
    }

    void eraseChildren(unsigned start, unsigned count)
    {
      VL_CHECK(start < childrenCount())
      VL_CHECK(start+count-1 < childrenCount())
      for(unsigned i=start; i<start+count; ++i)
        mNodes[i]->mParent = NULL;
      mNodes.erase(mNodes.begin()+start, mNodes.begin()+start+count);
    }

    int findChild(const ShaderNode* node) const
    {
      for(unsigned i=0; i<childrenCount(); ++i)
        if(child(i) == node)
          return i;
      return -1;
    }

    size_t childrenCount() const { return mNodes.size(); }

    ShaderNode* child(unsigned i) { return mNodes[i].get(); }
    const ShaderNode* child(unsigned i) const { return mNodes[i].get(); }

    ShaderNode* parent() { return mParent; }
    const ShaderNode* parent() const { return mParent; }

    // shader-related functions

    void inherit(ShaderNode* parent)
    {
      // init
      mRenderStates_Final.clear();
      mEnables_Final.clear();
      mUniforms_Final.clear();

      // propagate
      for(std::map< ERenderState, RenderStateInfo >::const_iterator rs_it = mRenderStates.begin(); rs_it != mRenderStates.end(); ++rs_it)
          mRenderStates_Final[rs_it->first] = rs_it->second;

      for(std::map< EEnable, EnableInfo>::const_iterator en_it = mEnables.begin(); en_it != mEnables.end(); ++en_it)
        if(en_it->second.mOn)
          mEnables_Final[en_it->first] = en_it->second;

      for(std::map< std::string, UniformInfo >::const_iterator unif_it = mUniforms.begin(); unif_it != mUniforms.end(); ++unif_it)
          mUniforms_Final[unif_it->first] = unif_it->second;

      // iterate parent final values

      if (parent)
      {

        /* ALGORITHM:
         *
         * 1 - iterate over parent's values
         * 2 - if it doesn't propagate -> end
         * 3 - find current values
         *   3a - if there is not -> insert parent's -> end
         *   3b - if there is
         *      3ba - if parent's do not overrides -> end
         *      3bb - if child's do not overridden -> end
         *      3bc - else -> use parent's
         */

        // render states
        for(std::map< ERenderState, RenderStateInfo >::const_iterator par_it = parent->mRenderStates_Final.begin(); par_it != parent->mRenderStates_Final.end(); ++par_it)
        {
          if (!(par_it->second.mInheritance & IN_Propagate))
            continue;
          std::map< ERenderState, RenderStateInfo >::const_iterator cur_it = mRenderStates_Final.find(par_it->first);
          if (cur_it == mRenderStates_Final.end())
            mRenderStates_Final[par_it->first] = par_it->second;
          else
          if (!(par_it->second.mInheritance & 0x02/*IN_Overrides*/) || (cur_it->second.mInheritance & IN_Sticky))
            continue;
          else
            mRenderStates_Final[par_it->first] = par_it->second;
        }

        // enables
        for(std::map< EEnable, EnableInfo >::const_iterator par_it = parent->mEnables_Final.begin(); par_it != parent->mEnables_Final.end(); ++par_it)
        {
          if (!par_it->second.mOn)
            continue;
          if (!(par_it->second.mInheritance & IN_Propagate))
            continue;
          std::map< EEnable, EnableInfo >::const_iterator cur_it = mEnables_Final.find(par_it->first);
          if (cur_it == mEnables_Final.end())
            mEnables_Final[par_it->first] = par_it->second;
          else
          if (!(par_it->second.mInheritance & 0x02/*IN_Overrides*/) || (cur_it->second.mInheritance & IN_Sticky))
            continue;
          else
            mEnables_Final[par_it->first] = par_it->second;
        }

        // uniforms
        for(std::map< std::string, UniformInfo >::const_iterator par_it = parent->mUniforms_Final.begin(); par_it != parent->mUniforms_Final.end(); ++par_it)
        {
          if (!(par_it->second.mInheritance & IN_Propagate))
            continue;
          std::map< std::string, UniformInfo >::const_iterator cur_it = mUniforms_Final.find(par_it->first);
          if (cur_it == mUniforms_Final.end())
            mUniforms_Final[par_it->first] = par_it->second;
          else
          if (!(par_it->second.mInheritance & 0x02/*IN_Overrides*/) || (cur_it->second.mInheritance & IN_Sticky))
            continue;
          else
            mUniforms_Final[par_it->first] = par_it->second;
        }
      }

      // apply it to the Shader

      if (mShader)
      {
        mShader->eraseAllRenderStates();
        mShader->eraseAllUniforms();
        mShader->disableAll();

        // we can speed this up even more by removing the duplication check

        for(std::map< ERenderState, RenderStateInfo >::const_iterator rs_it = mRenderStates_Final.begin(); rs_it != mRenderStates_Final.end(); ++rs_it)
          mShader->setRenderState(rs_it->second.mRenderState.get());
        for(std::map< EEnable, EnableInfo >::const_iterator en_it = mEnables_Final.begin(); en_it != mEnables_Final.end(); ++en_it)
          mShader->enable(en_it->second.mEnable);
        for(std::map< std::string, UniformInfo >::const_iterator rs_it = mUniforms_Final.begin(); rs_it != mUniforms_Final.end(); ++rs_it)
          mShader->setUniform(rs_it->second.mUniform.get());
      }
    }

    void setRenderState(RenderState* rs, EInheritance inheritance=IN_Propagate)
    {
      RenderStateInfo info(rs, inheritance);
      mRenderStates[rs->type()] = info;
    }

    void eraseRenderState(RenderState* rs)
    {
      mRenderStates.erase(rs->type());
    }

    void setEnable(EEnable en, bool on, EInheritance inheritance=IN_Propagate) 
    {
      EnableInfo info(en,on,inheritance);
      mEnables[en] = info;
    }

    void eraseEnable(EEnable en)
    {
      mEnables.erase(en);
    }

    void setUniform(Uniform* unif, EInheritance inheritance=IN_Propagate)
    {
      UniformInfo info(unif, inheritance);
      mUniforms[unif->name()] = info;
    }

    void eraseUniform(Uniform* unif)
    {
      mUniforms.erase(unif->name());
    }

    void setShader(Shader* shader) { mShader = shader; }
    const Shader* shader() const { return mShader.get(); }
    Shader* shader() { return mShader.get(); }

    void updateHierachy()
    {
      inherit(parent());
      for(unsigned i=0;i <childrenCount(); ++i)
        child(i)->updateHierachy();
    }

  protected:
    std::vector< ref< ShaderNode > > mNodes;
    ShaderNode* mParent;

    std::map< ERenderState, RenderStateInfo > mRenderStates;
    std::map< EEnable, EnableInfo> mEnables;
    std::map< std::string, UniformInfo > mUniforms;

    std::map< ERenderState, RenderStateInfo > mRenderStates_Final;
    std::map< EEnable, EnableInfo> mEnables_Final;
    std::map< std::string, UniformInfo > mUniforms_Final;

    ref<Shader> mShader;
  };
}

#endif
