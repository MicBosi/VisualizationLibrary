/**************************************************************************************/
/*                                                                                    */
/*  Visualization Library                                                             */
/*  http://www.visualizationlibrary.org                                               */
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

#include <vlCore/ResourceDatabase.hpp>
#include <vlGraphics/Actor.hpp>
#include <vlGraphics/Camera.hpp>
#include <vlGraphics/Light.hpp>
#include <vlGraphics/ClipPlane.hpp>
#include <set>
#include <algorithm>

using namespace vl;

namespace
{
  struct expanded_res
  {
    std::set< ref<Renderable> > ren;
    std::set< ref<Actor> > act;
    std::set< ref<Transform> > tr;
    std::set< ref<Effect> > fx;
    std::set< ref<Shader> > sh;
    std::set< ref<RenderState> > rs;
    std::set< ref<Uniform> > un;
    std::set< ref<Viewport> > vp;
    std::set< ref<Camera> > cam;
    std::set< ref<Object> > misc;
  };

  std::vector< ref<RenderState> > sortRenderStates(const std::set< ref<RenderState> >& rs_set)
  {
    std::vector< ref<RenderState> > rs;
    rs.insert( rs.end(), rs_set.begin(), rs_set.end() );
    
    // for some reason GCC 3.4.5 does not allow operator() to sort
    struct rs_less
    {
      static bool compare(const ref<RenderState>& a, const ref<RenderState>& b)
      {
        return a->type() < b->type();
      }
    };

    std::sort( rs.begin(), rs.end(), rs_less::compare );
    return rs;
  }

  void expandShader(Shader* shader, expanded_res& set)
  {
    // Shader
    set.sh.insert( shader );
    // Uniforms
    if (shader->getUniformSet())
      for(size_t i=0; i<shader->uniforms().size(); ++i)
        set.un.insert(shader->uniforms()[i].get());
    // RenderStates
    if (shader->getRenderStateSet())
      for(size_t i=0; i<shader->renderStatesCount(); ++i)
        set.rs.insert( shader->renderStates()[i].mRS );
  }

  void expandEffect(Effect* fx, expanded_res& set)
  {
    // Effect
    set.fx.insert(fx);
    // Shaders
    for(int i=0; i<VL_MAX_EFFECT_LOD; ++i)
    {
      if( fx->lod(i) )
      {
        for(int j=0; j<fx->lod(i)->size(); ++j)
        {
          Shader* shader = fx->lod(i)->at(j);
          set.sh.insert( shader );
          expandShader(shader, set);
        }
      }
    }
  }

  void expandActor(Actor* actor, expanded_res& set)
  {
    // Actor
    set.act.insert( actor );
    // Transform
    if (actor->transform())
      set.tr.insert( actor->transform() );
    // Uniforms
    if (actor->getUniformSet())
      for(size_t i=0; i<actor->uniforms().size(); ++i)
        set.un.insert(actor->uniforms()[i].get());
    // Renderables
    for(int i=0; i<VL_MAX_ACTOR_LOD; ++i)
      if ( actor->lod(i) )
        set.ren.insert( actor->lod(i) );
    // Effects
    if (actor->effect())
      expandEffect(actor->effect(), set);
  }
}

void ResourceDatabase::expand()
{
  expanded_res set;

  for(size_t i=0; i<resources().size(); ++i)
  {
    Object* res = resources()[i].get();

    if( res->isOfType( Renderable::Type() ) )
      set.ren.insert( res->as<Renderable>() );
    else
    if( res->isOfType( Actor::Type() ) )
      expandActor(res->as<Actor>(), set);
    else
    if( res->isOfType( Transform::Type() ) )
      set.tr.insert( res->as<Transform>() );
    else
    if( res->isOfType( Effect::Type() ) )
      expandEffect( res->as<Effect>(), set );
    else
    if( res->isOfType( Shader::Type() ) )
      expandShader( res->as<Shader>(), set );
    else
    if( res->isOfType( RenderState::Type() ) )
      set.rs.insert( res->as<RenderState>() );
    else
    if( res->isOfType( Uniform::Type() ) )
      set.un.insert( res->as<Uniform>() );
    else
    if( res->isOfType( Viewport::Type() ) )
      set.vp.insert( res->as<Viewport>() );
    else
    if( res->isOfType( Camera::Type() ) )
    {
      Camera* camera = res->as<Camera>();
      set.cam.insert( camera );
      if ( camera->viewport() )
        set.vp.insert( camera->viewport() );
      if ( camera->boundTransform() )
        set.tr.insert( camera->boundTransform() );
    }
    else
      set.misc.insert( res );
  }

  // reconstruct the resources
  resources().clear();
  resources().insert( resources().end(), set.vp.begin(), set.vp.end() );
  resources().insert( resources().end(), set.cam.begin(), set.cam.end() );
  resources().insert( resources().end(), set.tr.begin(), set.tr.end() );
  std::vector< ref<RenderState> > sorted_rs = sortRenderStates( set.rs );
  resources().insert( resources().end(), sorted_rs.begin(), sorted_rs.end() );
  resources().insert( resources().end(), set.un.begin(), set.un.end() );
  resources().insert( resources().end(), set.sh.begin(), set.sh.end() );
  resources().insert( resources().end(), set.fx.begin(), set.fx.end() );
  resources().insert( resources().end(), set.ren.begin(), set.ren.end() );
  resources().insert( resources().end(), set.act.begin(), set.act.end() );
  resources().insert( resources().end(), set.misc.begin(), set.misc.end() );
}

