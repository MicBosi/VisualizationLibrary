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

#include <vl/Renderer.hpp>
#include <vl/RenderQueue.hpp>
#include <vl/Effect.hpp>
#include <vl/Transform.hpp>
#include <vl/checks.hpp>
#include <vl/GLSL.hpp>
#include <vl/Light.hpp>
#include <vl/ClipPlane.hpp>
#include <vl/Time.hpp>
#include <vl/Log.hpp>
#include <vl/Say.hpp>

using namespace vl;

namespace
{
  unsigned g_OcclusionQueryMasterTick = 0;
}

//------------------------------------------------------------------------------
// Renderer
//------------------------------------------------------------------------------
Renderer::Renderer()
{
  #ifndef NDEBUG
    mObjectName = className();
  #endif

  mOcclusionQueryTickPrev  = 0;
  mOcclusionQueryTick      = 0;
  mOcclusionThreshold      = 0;
  mOcclusionCullingEnabled = false;
  mCollectStatistics       = false;
  mRenderedRenderableCount = 0;
  mRenderedTriangleCount   = 0;
  mRenderedLineCount       = 0;
  mRenderedPointCount      = 0;

  setupDefaultRenderStates();

  mProjViewTranfCallback = new ProjViewTranfCallbackStandard;

  // just for debugging purposes
  memset( mCurrentRenderState, 0xFF, sizeof(mCurrentRenderState[0]) * RS_COUNT );
  memset( mRenderStateTable,   0xFF, sizeof(int)                    * RS_COUNT );
  memset( mCurrentEnable,      0xFF, sizeof(mCurrentEnable[0])      * EN_EnableCount );
  memset( mEnableTable,        0xFF, sizeof(int)                    * EN_EnableCount );

  // occlusion culling shader
  mOcclusionShader = new Shader;
  mOcclusionShader->gocDepthMask()->set(false);
  mOcclusionShader->gocColorMask()->set(false,false,false,false);
  mOcclusionShader->enable(vl::EN_CULL_FACE);
  mOcclusionShader->enable(vl::EN_DEPTH_TEST);
}
//------------------------------------------------------------------------------
void Renderer::render(const RenderQueue* render_queue, Camera* camera)
{
  struct ShaderInfo
  {
  public:
    ShaderInfo(): mTransform(NULL), mShaderUniformSet(NULL), mActorUniformSet(NULL) {}
    bool operator<(const ShaderInfo& other) const
    {
      if (mTransform != other.mTransform)
        return mTransform < other.mTransform;
      else
      if ( mShaderUniformSet != other.mShaderUniformSet ) 
        return mShaderUniformSet < other.mShaderUniformSet;
      else
        return mActorUniformSet < other.mActorUniformSet;
    }

    const Transform* mTransform;
    const UniformSet* mShaderUniformSet;
    const UniformSet* mActorUniformSet;
  };

  std::map<const GLSLProgram*, ShaderInfo> glslprogram_map;

  VL_CHECK_OGL()

  mRenderedRenderableCount = 0;
  mRenderedTriangleCount  = 0;
  mRenderedLineCount = 0;
  mRenderedPointCount = 0;

  // --------------- rendering ---------------

  // non GLSLProgram state sets
  const RenderStateSet* cur_render_state_set = NULL;
  const EnableSet* cur_enable_set = NULL;
  const Scissor* cur_scissor = NULL;

  // scissor the viewport by default: needed for points and lines sice they are not clipped against the viewport
  #if 1
    glEnable(GL_SCISSOR_TEST);
    glScissor(camera->viewport()->x(), camera->viewport()->y(), camera->viewport()->width(), camera->viewport()->height());
  #else
    glDisable(GL_SCISSOR_TEST);
  #endif

  /* camera/eye position (mic fixme) */
  // vec3 eye = camera->inverseViewMatrix().getT();

  /* current occlusion query tick (mic fixme) */
  // mOcclusionQueryTickPrev = mOcclusionQueryTick;
  // mOcclusionQueryTick     = g_OcclusionQueryMasterTick++;

  for(int itok=0; itok < render_queue->size(); ++itok)
  {
    const RenderToken* tok = render_queue->at(itok);

    VL_CHECK(tok);

    // --------------- Actor's scissor ---------------

    const Scissor* scissor = tok->mActor->scissor() ? tok->mActor->scissor() : tok->mShader->scissor();
    if (cur_scissor != scissor)
    {
      cur_scissor = scissor;
      if (cur_scissor)
      {
        cur_scissor->enable(camera->viewport());
      }
      else
      {
        #if 1
          // scissor the viewport by default: needed for points and lines with size > 1.0 as they are not clipped against the viewport.
          VL_CHECK(glIsEnabled(GL_SCISSOR_TEST))
          glScissor(camera->viewport()->x(), camera->viewport()->y(), camera->viewport()->width(), camera->viewport()->height());
        #else
          glDisable(GL_SCISSOR_TEST);
        #endif
      }
    }

    // --------------- occlusion culling ---------------

    // fixme - occlusion culling disabled for now until the transition to the new architecture is finalized.
    /*
    bool occluded = false;
    if ( occlusionCullingEnabled() && !tok->mActor->boundingBox().isInside(eye) )
    {
      VL_WARN(GLEW_ARB_occlusion_query || GLEW_VERSION_1_5)

      if ( tok->mActor->occlusionQuery() && tok->mActor->occlusionQueryTick() == mOcclusionQueryTickPrev )
      {
        #if 0
          GLint ready = GL_FALSE;
          glGetQueryObjectiv(tok->mActor->occlusionQuery(),GL_QUERY_RESULT_AVAILABLE,&ready); VL_CHECK_OGL();
          if (ready == GL_FALSE)
            vl::Log::error("Occlusion culling query not yet available.\n");
        #endif
        // a few benchmarks say that even if it is not ready it is convenient to flush the OpenGL pipeline at this point
        GLint pixels = 0;
        glGetQueryObjectiv(tok->mActor->occlusionQuery(),GL_QUERY_RESULT,&pixels); VL_CHECK_OGL();
        // object is occluded
        if (pixels <= occlusionThreshold())
          occluded = true;
      }

      // if occludee -> perform occlusion test to be used for the next frame
      if (tok->mActor->isOccludee())
      {
        // register occlusion query tick

        tok->mActor->setOcclusionQueryTick(mOcclusionQueryTick);

        // activate occlusion culling shader

        applyRenderStates(cur_render_state_set, mOcclusionShader->getRenderStateSet(), camera );
        cur_render_state_set = mOcclusionShader->getRenderStateSet();

        VL_CHECK_OGL()

        applyEnables(cur_enable_set, mOcclusionShader->getEnableSet() );
        cur_enable_set = mOcclusionShader->getEnableSet();

        VL_CHECK_OGL()

        // compute Renderable AABB geometry (we are using the currently active Transform)
        const AABB& aabb = tok->mRenderable->boundingBox();
        const float verts[] = 
        {
          (float)aabb.minCorner().x(), (float)aabb.minCorner().y(), (float)aabb.minCorner().z(),
          (float)aabb.maxCorner().x(), (float)aabb.minCorner().y(), (float)aabb.minCorner().z(),
          (float)aabb.maxCorner().x(), (float)aabb.maxCorner().y(), (float)aabb.minCorner().z(),
          (float)aabb.minCorner().x(), (float)aabb.maxCorner().y(), (float)aabb.minCorner().z(),
          (float)aabb.minCorner().x(), (float)aabb.minCorner().y(), (float)aabb.maxCorner().z(),
          (float)aabb.maxCorner().x(), (float)aabb.minCorner().y(), (float)aabb.maxCorner().z(),
          (float)aabb.maxCorner().x(), (float)aabb.maxCorner().y(), (float)aabb.maxCorner().z(),
          (float)aabb.minCorner().x(), (float)aabb.maxCorner().y(), (float)aabb.maxCorner().z()
        };
        const unsigned quads[] = { 3,2,1,0, 2,6,5,1, 3,7,6,2, 7,3,0,4, 4,0,1,5, 6,7,4,5 };
        // glColor3f(1.0f,1.0f,1.0f);
        glEnableClientState(GL_VERTEX_ARRAY); VL_CHECK_OGL();
        glVertexPointer(3, GL_FLOAT, 0, verts); VL_CHECK_OGL();
          tok->mActor->createOcclusionQuery(); VL_CHECK_OGL();
          glBeginQuery(GL_SAMPLES_PASSED, tok->mActor->occlusionQuery()); VL_CHECK_OGL();
          glDrawElements(GL_QUADS, 6*4, GL_UNSIGNED_INT, quads); VL_CHECK_OGL();
          glEndQuery(GL_SAMPLES_PASSED); VL_CHECK_OGL();
        glDisableClientState(GL_VERTEX_ARRAY); VL_CHECK_OGL();
        glVertexPointer(3, GL_FLOAT, 0, NULL); VL_CHECK_OGL();
      }
    }

    // skip occluded object
    if (occluded)
      continue;
    */

    VL_CHECK_OGL()

    // multipassing
    for( int ipass=0; tok != NULL; tok = tok->mNextPass, ++ipass )
    {

      // --------------- shader setup ---------------

      VL_CHECK_OGL()

      // shader's render states

      if ( cur_render_state_set != tok->mShader->getRenderStateSet() )
      {
        applyRenderStates(cur_render_state_set, tok->mShader->getRenderStateSet(), camera );
        cur_render_state_set = tok->mShader->getRenderStateSet();
      }

      VL_CHECK_OGL()

      // shader's enables

      if ( cur_enable_set != tok->mShader->getEnableSet() )
      {
        applyEnables(cur_enable_set, tok->mShader->getEnableSet() );
        cur_enable_set = tok->mShader->getEnableSet();
      }

      #ifndef NDEBUG
        if (glGetError() != GL_NO_ERROR)
        {
          Log::error("An unsupported OpenGL glEnable/glDisable mode has been enabled!\n");
          VL_TRAP()
        }
      #endif


      // --------------- GLSLProgram setup ---------------

      VL_CHECK_OGL()

      // current transform
      const Transform*   cur_transform          = tok->mActor->transform(); 
      const GLSLProgram* cur_glsl_program       = NULL; // NULL == fixed function pipeline
      const UniformSet*  cur_shader_uniform_set = NULL;
      const UniformSet*  cur_actor_uniform_set  = NULL;
      // make sure we update these things only if there is a valid GLSLProgram
      if (tok->mShader->glslProgram() && tok->mShader->glslProgram()->handle())
      {
        cur_glsl_program       = tok->mShader->glslProgram();
        cur_shader_uniform_set = tok->mShader->getUniformSet();
        cur_actor_uniform_set  = tok->mActor->uniformSet();
        // consider them NULL if they are empty
        if (cur_shader_uniform_set && cur_shader_uniform_set->uniforms().empty())
          cur_shader_uniform_set = NULL;
        if (cur_actor_uniform_set  && cur_actor_uniform_set ->uniforms().empty())
          cur_actor_uniform_set = NULL;
      } 

      // is it the first update we do overall? (ie this is the first object rendered)
      bool is_first_overall = glslprogram_map.empty();
      bool update_su = false; // update shader uniforms
      bool update_au = false; // update actor uniforms
      bool update_tr = false; // update transform
      ShaderInfo* shader_info = NULL;
      // retrieve the state of this GLSLProgram (including the NULL one)
      std::map<const GLSLProgram*, ShaderInfo>::iterator shader_info_it = glslprogram_map.find(cur_glsl_program);
      bool is_first_use = false;
      if ( shader_info_it == glslprogram_map.end() )
      {
        // create a new shader-info entry
        shader_info = &glslprogram_map[cur_glsl_program];
        // update_tr = true; not needed since is_first_use overrides it.
        update_su = cur_shader_uniform_set != NULL ? true : false;
        update_au = cur_actor_uniform_set  != NULL ? true : false;
        // is it the first update to this GLSLProgram? Yes.
        is_first_use = true;
      }
      else
      {
        shader_info = &shader_info_it->second;      
        // check for differences
        update_tr = shader_info->mTransform        != cur_transform;
        update_su = shader_info->mShaderUniformSet != cur_shader_uniform_set && cur_shader_uniform_set;
        update_au = shader_info->mActorUniformSet  != cur_actor_uniform_set  && cur_actor_uniform_set;
      }

      // update shader-info structure
      shader_info->mTransform        = cur_transform;
      shader_info->mShaderUniformSet = cur_shader_uniform_set;
      shader_info->mActorUniformSet  = cur_actor_uniform_set;

      // fixme?
      // theoretically we can optimize further caching the last GLSLProgram used and it's shader-info and if 
      // the new one is the same as the old one we can avoid looking up in the map. The gain should be minimal
      // and the code would get further clutterd.

      // --- update proj, view and transform matrices ---

      if (is_first_use) // note: 'is_first_overall' implies 'is_first_use'
        projViewTransfCallback()->programFirstUse(this, cur_glsl_program, cur_transform, camera, is_first_overall);
      else
      if (update_tr)
        projViewTransfCallback()->programTransfChange(this, cur_glsl_program, cur_transform, camera);

      // --- uniforms ---

      // note: the user must not make the shader's and actor's uniforms collide!

      // shader uniform set
      if ( update_su )
      {
        VL_CHECK( cur_shader_uniform_set && cur_shader_uniform_set->uniforms().size() );
        VL_CHECK( tok->mShader->getRenderStateSet()->glslProgram() && tok->mShader->getRenderStateSet()->glslProgram()->handle() )
        tok->mShader->getRenderStateSet()->glslProgram()->applyUniformSet( cur_shader_uniform_set );
      }

      // actor uniform set
      if ( update_au )
      {
        VL_CHECK( cur_actor_uniform_set && cur_actor_uniform_set->uniforms().size() );
        VL_CHECK( tok->mShader->getRenderStateSet()->glslProgram() && tok->mShader->getRenderStateSet()->glslProgram()->handle() )
        tok->mShader->getRenderStateSet()->glslProgram()->applyUniformSet( cur_actor_uniform_set );
      }

      VL_CHECK_OGL()

      // --- Actor's prerender callback --- done after GLSLProgam has been bound and setup
      tok->mActor->executeRenderingCallbacks( camera, tok->mRenderable, tok->mShader, ipass );

      VL_CHECK_OGL()

      // --------------- Actor rendering ---------------

      // statistics

      if (collectStatistics())
      {
        mRenderedRenderableCount++;
        mRenderedTriangleCount += tok->mRenderable->triangleCount();
        mRenderedLineCount     += tok->mRenderable->lineCount();
        mRenderedPointCount    += tok->mRenderable->pointCount();
      }

      // contract (fixme: to be changed for vertex-array and elem-array lazy bind):
      // 1 - all vertex arrays and VBOs are disabled before calling render()
      // 2 - all vertex arrays and VBOs are disabled after  calling render()

      VL_CHECK( !tok->mRenderable->displayListEnabled() || (tok->mRenderable->displayListEnabled() && tok->mRenderable->displayList()) )

      if (tok->mRenderable->displayListEnabled())
        glCallList( tok->mRenderable->displayList() );
      else
        tok->mRenderable->render( tok->mActor, openglContext(), camera );

      VL_CHECK_OGL()
    }
  }

  glDisable(GL_SCISSOR_TEST);
}
//------------------------------------------------------------------------------
namespace
{
  const GLenum TranslateEnable[] =
  {
    GL_ALPHA_TEST,
    GL_BLEND,
    GL_COLOR_LOGIC_OP,
    GL_LIGHTING,
    GL_COLOR_SUM,
    GL_CULL_FACE,
    GL_DEPTH_TEST,
    GL_FOG,
    GL_LINE_SMOOTH,
    GL_LINE_STIPPLE,
    GL_POLYGON_STIPPLE,
    GL_NORMALIZE,
    GL_POINT_SMOOTH,
    GL_POINT_SPRITE,
    GL_POLYGON_SMOOTH,
    GL_POLYGON_OFFSET_FILL,
    GL_POLYGON_OFFSET_LINE,
    GL_POLYGON_OFFSET_POINT,
    GL_RESCALE_NORMAL,
    GL_STENCIL_TEST,
    GL_VERTEX_PROGRAM_POINT_SIZE,
    GL_VERTEX_PROGRAM_TWO_SIDE,

    // multisampling
    GL_MULTISAMPLE,
    GL_SAMPLE_ALPHA_TO_COVERAGE,
    GL_SAMPLE_ALPHA_TO_ONE,
    GL_SAMPLE_COVERAGE
  };

  #ifndef NDEBUG
  const char* TranslateEnableString[] =
  {
    "EN_ALPHA_TEST",
    "EN_BLEND",
    "EN_COLOR_LOGIC_OP",
    "EN_LIGHTING",
    "EN_COLOR_SUM",
    "EN_CULL_FACE",
    "EN_DEPTH_TEST",
    "EN_FOG ",
    "EN_LINE_SMOOTH",
    "EN_LINE_STIPPLE",
    "EN_POLYGON_STIPPLE",
    "EN_NORMALIZE",
    "EN_POINT_SMOOTH",
    "EN_POINT_SPRITE",
    "EN_POLYGON_SMOOTH",
    "EN_POLYGON_OFFSET_FILL",
    "EN_POLYGON_OFFSET_LINE",
    "EN_POLYGON_OFFSET_POINT",
    "EN_RESCALE_NORMAL",
    "EN_STENCIL_TEST",
    "EN_VERTEX_PROGRAM_POINT_SIZE",
    "EN_VERTEX_PROGRAM_TWO_SIDE",

    // multisampling
    "EN_MULTISAMPLE",
    "EN_SAMPLE_ALPHA_TO_COVERAGE",
    "EN_SAMPLE_ALPHA_TO_ONE",
    "EN_SAMPLE_COVERAGE"
  };
  #endif
}
//------------------------------------------------------------------------------
void Renderer::applyEnables( const EnableSet* prev, const EnableSet* cur )
{
  VL_CHECK_OGL()
  VL_CHECK(cur)
  if (prev == NULL)
    memset( mEnableTable, 0, sizeof(int) * EN_EnableCount );

  // iterate current: increment

  for( unsigned i=0; i<cur->enables().size(); ++i )
    mEnableTable[cur->enables()[i]] |= 2;

  // iterate on prev: reset to default only if it won't be overwritten by cur

  if (prev)
  {
    for( unsigned i=0; i<prev->enables().size(); ++i )
    {
      if ( mEnableTable[prev->enables()[i]] == 1 )
      {
        mEnableTable[prev->enables()[i]] = 0;
        mCurrentEnable[prev->enables()[i]] = false;
        glDisable( TranslateEnable[prev->enables()[i]] );
        #ifndef NDEBUG
          if (glGetError() != GL_NO_ERROR)
          {
            Log::error( Say("An unsupported enum has been disabled: %s.\n") << TranslateEnableString[prev->enables()[i]]);
            VL_TRAP()
          }
        #endif
      }
    }
  }
  else
  {
    memset(mCurrentEnable, 0, sizeof(mCurrentEnable[0])*EN_EnableCount);
  }

  // iterate current: apply only if needed

  for( unsigned i=0; i<cur->enables().size(); ++i )
  {
    mEnableTable[cur->enables()[i]] = 1;
    if ( !mCurrentEnable[cur->enables()[i]] )
    {
      glEnable( TranslateEnable[cur->enables()[i]] );
      mCurrentEnable[ cur->enables()[i] ] = true;
      #ifndef NDEBUG
        if (glGetError() != GL_NO_ERROR)
        {
          Log::error( Say("An unsupported enum has been enabled: %s.\n") << TranslateEnableString[cur->enables()[i]]);
          VL_TRAP()
        }
      #endif
    }
  }
}
//------------------------------------------------------------------------------
void Renderer::applyRenderStates( const RenderStateSet* prev, const RenderStateSet* cur, const Camera* camera )
{
  VL_CHECK(cur)
  if (prev == NULL)
    memset( mRenderStateTable, 0, sizeof(int) * RS_COUNT );

  // iterate current: increment

  for( unsigned i=0; i<cur->renderStates().size(); ++i )
    mRenderStateTable[cur->renderStates()[i]->type()] |= 2;

  // iterate on prev: reset to default only if it won't be overwritten by cur

  if (prev)
  {
    for( unsigned i=0; i<prev->renderStates().size(); ++i )
    {
      if ( mRenderStateTable[prev->renderStates()[i]->type()] == 1 )
      {
        mRenderStateTable[prev->renderStates()[i]->type()] = 0;
        mCurrentRenderState[prev->renderStates()[i]->type()] = mDefaultRenderStates[prev->renderStates()[i]->type()].get();
        mDefaultRenderStates[prev->renderStates()[i]->type()]->disable();
        mDefaultRenderStates[prev->renderStates()[i]->type()]->apply(NULL);
      }
    }
  }
  else
  {
    memset(mCurrentRenderState, 0, sizeof(mCurrentRenderState[0])*RS_COUNT);
  }

  // iterate current: apply only if needed

  for( unsigned i=0; i<cur->renderStates().size(); ++i )
  {
    mRenderStateTable[cur->renderStates()[i]->type()] = 1;
    if ( mCurrentRenderState[cur->renderStates()[i]->type()] != cur->renderStates()[i] )
    {
      mCurrentRenderState[cur->renderStates()[i]->type()] = cur->renderStates()[i].get();
      if (cur->renderStates()[i]->textureUnit() < openglContext()->textureUnitCount())
      {
        cur->renderStates()[i]->enable();
        cur->renderStates()[i]->apply(camera);
      }
      /*else
        Log::error( Say("Render state error: texture unit index #%n not supported by this OpenGL implementation. Max texture unit index is %n.\n") << cur->renderStates()[i]->textureUnit() << openglContext()->textureUnitCount()-1);*/
    }
  }
}
//------------------------------------------------------------------------------
void Renderer::setupDefaultRenderStates()
{
  mDefaultRenderStates[RS_AlphaFunc] = new AlphaFunc;
  mDefaultRenderStates[RS_BlendColor] = new BlendColor;
  mDefaultRenderStates[RS_BlendEquation] = new BlendEquation;
  mDefaultRenderStates[RS_BlendFunc] = new BlendFunc;
  mDefaultRenderStates[RS_ColorMask] = new ColorMask;
  mDefaultRenderStates[RS_CullFace] = new CullFace;
  mDefaultRenderStates[RS_DepthFunc] = new DepthFunc;
  mDefaultRenderStates[RS_DepthMask] = new DepthMask;
  mDefaultRenderStates[RS_DepthRange] = new DepthRange;
  mDefaultRenderStates[RS_Fog] = new Fog;
  mDefaultRenderStates[RS_FrontFace] = new FrontFace;
  mDefaultRenderStates[RS_PolygonMode] = new PolygonMode;
  mDefaultRenderStates[RS_Hint] = new Hint;
  mDefaultRenderStates[RS_LightModel] = new LightModel;
  mDefaultRenderStates[RS_LineStipple] = new LineStipple;
  mDefaultRenderStates[RS_LineWidth] = new LineWidth;
  mDefaultRenderStates[RS_LogicOp] = new LogicOp;
  mDefaultRenderStates[RS_Material] = new Material;
  mDefaultRenderStates[RS_PixelTransfer] = new PixelTransfer;
  mDefaultRenderStates[RS_PointParameter] = new PointParameter;
  mDefaultRenderStates[RS_PointSize] = new PointSize;
  mDefaultRenderStates[RS_PolygonOffset] = new PolygonOffset;
  mDefaultRenderStates[RS_PolygonStipple] = new PolygonStipple;
  mDefaultRenderStates[RS_SampleCoverage] = new SampleCoverage;
  mDefaultRenderStates[RS_ShadeModel] = new ShadeModel;
  mDefaultRenderStates[RS_StencilFunc] = new StencilFunc;
  mDefaultRenderStates[RS_StencilMask] = new StencilMask;
  mDefaultRenderStates[RS_StencilOp] = new StencilOp;
  mDefaultRenderStates[RS_GLSLProgram] = new GLSLProgram;

  mDefaultRenderStates[RS_Light0] = new Light(0);
  mDefaultRenderStates[RS_Light1] = new Light(1);
  mDefaultRenderStates[RS_Light2] = new Light(2);
  mDefaultRenderStates[RS_Light3] = new Light(3);
  mDefaultRenderStates[RS_Light4] = new Light(4);
  mDefaultRenderStates[RS_Light5] = new Light(5);
  mDefaultRenderStates[RS_Light6] = new Light(6);
  mDefaultRenderStates[RS_Light7] = new Light(7);

  mDefaultRenderStates[RS_ClipPlane0] = new ClipPlane(0);
  mDefaultRenderStates[RS_ClipPlane1] = new ClipPlane(1);
  mDefaultRenderStates[RS_ClipPlane2] = new ClipPlane(2);
  mDefaultRenderStates[RS_ClipPlane3] = new ClipPlane(3);
  mDefaultRenderStates[RS_ClipPlane4] = new ClipPlane(4);
  mDefaultRenderStates[RS_ClipPlane5] = new ClipPlane(5);

  for(unsigned int i=0; i<VL_MAX_TEXTURE_UNIT_COUNT; ++i)
  {
    mDefaultRenderStates[RS_TextureUnit0   + i] = new TextureUnit(i);
    mDefaultRenderStates[RS_TexGen0        + i] = new TexGen(i);
    mDefaultRenderStates[RS_TexEnv0        + i] = new TexEnv(i);
    mDefaultRenderStates[RS_TextureMatrix0 + i] = new TextureMatrix(i);
  }
}
//-----------------------------------------------------------------------------
void Renderer::resetRenderStates()
{
  memset( mCurrentRenderState, 0, sizeof(mCurrentRenderState[0]) * RS_COUNT );
  memset( mRenderStateTable,   0, sizeof(int)                    * RS_COUNT );
  // render states
  for( unsigned i=0; i<RS_COUNT; ++i )
  {
    if ( mDefaultRenderStates[i]->textureUnit() < openglContext()->textureUnitCount() )
    {
       mDefaultRenderStates[i]->disable();
       mDefaultRenderStates[i]->apply(NULL);
    }
    /*else
      don't issue any error message here*/
  }
}
//-----------------------------------------------------------------------------
void Renderer::resetEnables()
{
  memset( mCurrentEnable, 0, sizeof(mCurrentEnable[0]) * EN_EnableCount );
  memset( mEnableTable,   0, sizeof(int)               * EN_EnableCount );
  for( unsigned i=0; i<EN_EnableCount; ++i )
    glDisable( TranslateEnable[i] );
  // clears errors due to unsupported enable flags
  while( glGetError() ) {}
}
//-----------------------------------------------------------------------------
void ProjViewTranfCallbackStandard::programFirstUse(const Renderer*, const GLSLProgram*, const Transform* transform, const Camera* camera, bool first_overall)
{
  if (mLastTransform != transform || first_overall)
  {
    if ( transform )
    {
      glMatrixMode(GL_MODELVIEW);
      VL_glLoadMatrix( (camera->viewMatrix() * transform->worldMatrix() ).ptr() );
    }
    else
    {
      glMatrixMode(GL_MODELVIEW);
      VL_glLoadMatrix( camera->viewMatrix().ptr() );
    }
  }

  // set the projection matrix only once per rendering
  if (first_overall)
  {
    glMatrixMode(GL_PROJECTION);
    VL_glLoadMatrix( (camera->projectionMatrix() ).ptr() );
  }

  // update last transform
  mLastTransform = transform;
}
//-----------------------------------------------------------------------------
void ProjViewTranfCallbackStandard::programTransfChange(const Renderer*, const GLSLProgram*, const Transform* transform, const Camera* camera)
{
  if (mLastTransform != transform)
  {
    if ( transform )
    {
      glMatrixMode(GL_MODELVIEW);
      VL_glLoadMatrix( (camera->viewMatrix() * transform->worldMatrix() ).ptr() );
    }
    else
    {
      glMatrixMode(GL_MODELVIEW);
      VL_glLoadMatrix( camera->viewMatrix().ptr() );
    }
  }

  // update last transform
  mLastTransform = transform;
}
//-----------------------------------------------------------------------------
