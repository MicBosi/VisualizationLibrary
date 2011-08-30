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

#include <vlCore/VisualizationLibrary.hpp>
#include <vlCore/LoadWriterManager.hpp>
#include <vlGraphics/Rendering.hpp>
#include <vlGraphics/BezierSurface.hpp>
#include <vlGraphics/FontManager.hpp>

using namespace vl;

#if defined(VL_IO_3D_VLX)
  #include "plugins/vlVLX.hpp"
#endif
#if defined(VL_IO_3D_3DS)
  #include "plugins/vl3DS.hpp"
#endif
#if defined(VL_IO_3D_OBJ)
  #include "plugins/vlOBJ.hpp"
#endif
#if defined(VL_IO_3D_AC3D)
  #include "plugins/vlAC3D.hpp"
#endif
#if defined(VL_IO_3D_PLY)
  #include "plugins/vlPLY.hpp"
#endif
#if defined(VL_IO_3D_STL)
  #include "plugins/vlSTL.hpp"
#endif
#if defined(VL_IO_3D_MD2)
  #include "plugins/vlMD2.hpp"
#endif
#if defined(VL_IO_3D_COLLADA)
  #include "plugins/COLLADA/vlDae.hpp"
#endif

//------------------------------------------------------------------------------
// VL misc
//------------------------------------------------------------------------------
namespace
{
  bool gInitializedGraphics = false;
};
//-----------------------------------------------------------------------------
// Default FontManager
//-----------------------------------------------------------------------------
namespace
{
  ref<FontManager> gDefaultFontManager = NULL;
}
FontManager* vl::defFontManager()
{
  return gDefaultFontManager.get();
}
void vl::setDefFontManager(FontManager* fm)
{
  gDefaultFontManager = fm;
}
//-----------------------------------------------------------------------------
namespace
{
  void registerVLXSerializers()
  {
    // Geometry serializer
    defVLXRegistry()->addSerializer( Geometry::Type(), new VLXIO_Geometry );

    // VertexAttribInfo
    defVLXRegistry()->addSerializer( VertexAttribInfo::Type(), new VLXIO_VertexAttribInfo );

    // BezierSurface
    defVLXRegistry()->addSerializer( BezierSurface::Type(), new VLXIO_Geometry );

    // PatchParameter
    defVLXRegistry()->addSerializer( PatchParameter::Type(), new VLXIO_PatchParameter );

    // DrawCall
    ref<VLXIO_DrawCall> drawcall_serializer = new VLXIO_DrawCall;
    defVLXRegistry()->addSerializer( DrawArrays::Type(), drawcall_serializer.get() );
    defVLXRegistry()->addSerializer( DrawElementsUInt::Type(), drawcall_serializer.get() );
    defVLXRegistry()->addSerializer( DrawElementsUShort::Type(), drawcall_serializer.get() );
    defVLXRegistry()->addSerializer( DrawElementsUByte::Type(), drawcall_serializer.get() );
    defVLXRegistry()->addSerializer( MultiDrawElementsUInt::Type(), drawcall_serializer.get() );
    defVLXRegistry()->addSerializer( MultiDrawElementsUShort::Type(), drawcall_serializer.get() );
    defVLXRegistry()->addSerializer( MultiDrawElementsUByte::Type(), drawcall_serializer.get() );

    // ResourceDatabase
    defVLXRegistry()->addSerializer( ResourceDatabase::Type(), new VLXIO_ResourceDatabase );

    // Uniform
    defVLXRegistry()->addSerializer( Uniform::Type(), new VLXIO_Uniform );

    // LODEvaluator
    defVLXRegistry()->addSerializer( LODEvaluator::Type(), new VLXIO_LODEvaluator );

    // Transform
    defVLXRegistry()->addSerializer( Transform::Type(), new VLXIO_Transform );

    // Material
    defVLXRegistry()->addSerializer( Material::Type(), new VLXIO_Material );

    // Texture
    defVLXRegistry()->addSerializer( Texture::Type(), new VLXIO_Texture );

    // TextureSampler
    defVLXRegistry()->addSerializer( TextureSampler::Type(), new VLXIO_TextureSampler );

    // TexParameter
    defVLXRegistry()->addSerializer( TexParameter::Type(), new VLXIO_TexParameter );

    // ActorEventCallback
    defVLXRegistry()->addSerializer( DepthSortCallback::Type(), new VLXIO_ActorEventCallback );

    // LODEvaluator
    ref<VLXIO_LODEvaluator> lod_evaluator = new VLXIO_LODEvaluator;
    defVLXRegistry()->addSerializer( PixelLODEvaluator::Type(), lod_evaluator.get() );
    defVLXRegistry()->addSerializer( DistanceLODEvaluator::Type(), lod_evaluator.get() );

    // Actor
    defVLXRegistry()->addSerializer( Actor::Type(), new VLXIO_Actor );

    // Effect
    defVLXRegistry()->addSerializer( Effect::Type(), new VLXIO_Effect );

    // Shader
    defVLXRegistry()->addSerializer( Shader::Type(), new VLXIO_Shader );

    // Camera
    defVLXRegistry()->addSerializer( Camera::Type(), new VLXIO_Camera );

    // Light
    defVLXRegistry()->addSerializer( Light::Type(), new VLXIO_Light );

    // ClipPlane
    defVLXRegistry()->addSerializer( ClipPlane::Type(), new VLXIO_ClipPlane );

    // Color
    defVLXRegistry()->addSerializer( Color::Type(), new VLXIO_Color );

    // SecondaryColor
    defVLXRegistry()->addSerializer( SecondaryColor::Type(), new VLXIO_SecondaryColor );

    // Normal
    defVLXRegistry()->addSerializer( Normal::Type(), new VLXIO_Normal );

    // VertexAttrib
    defVLXRegistry()->addSerializer( VertexAttrib::Type(), new VLXIO_VertexAttrib );

    // Viewport
    defVLXRegistry()->addSerializer( Viewport::Type(), new VLXIO_Viewport );

    // GLSL
    defVLXRegistry()->addSerializer( GLSLProgram::Type(), new VLXIO_GLSLProgram );
    ref<VLXIO_GLSLShader> sh_serializer = new VLXIO_GLSLShader;
    defVLXRegistry()->addSerializer( GLSLVertexShader::Type(), sh_serializer.get() );
    defVLXRegistry()->addSerializer( GLSLFragmentShader::Type(), sh_serializer.get() );
    defVLXRegistry()->addSerializer( GLSLGeometryShader::Type(), sh_serializer.get() );
    defVLXRegistry()->addSerializer( GLSLTessControlShader::Type(), sh_serializer.get() );
    defVLXRegistry()->addSerializer( GLSLTessEvaluationShader::Type(), sh_serializer.get() );

    // GLSLShader
    defVLXRegistry()->addSerializer( GLSLShader::Type(), new VLXIO_GLSLShader );

    // Array serializer
    ref<VLXIO_Array> array_serializer = new VLXIO_Array;

    defVLXRegistry()->addSerializer( ArrayFloat1::Type(), array_serializer.get() );
    defVLXRegistry()->addSerializer( ArrayFloat2::Type(), array_serializer.get() );
    defVLXRegistry()->addSerializer( ArrayFloat3::Type(), array_serializer.get() );
    defVLXRegistry()->addSerializer( ArrayFloat4::Type(), array_serializer.get() );

    defVLXRegistry()->addSerializer( ArrayDouble1::Type(), array_serializer.get() );
    defVLXRegistry()->addSerializer( ArrayDouble2::Type(), array_serializer.get() );
    defVLXRegistry()->addSerializer( ArrayDouble3::Type(), array_serializer.get() );
    defVLXRegistry()->addSerializer( ArrayDouble4::Type(), array_serializer.get() );

    defVLXRegistry()->addSerializer( ArrayInt1::Type(), array_serializer.get() );
    defVLXRegistry()->addSerializer( ArrayInt2::Type(), array_serializer.get() );
    defVLXRegistry()->addSerializer( ArrayInt3::Type(), array_serializer.get() );
    defVLXRegistry()->addSerializer( ArrayInt4::Type(), array_serializer.get() );

    defVLXRegistry()->addSerializer( ArrayUInt1::Type(), array_serializer.get() );
    defVLXRegistry()->addSerializer( ArrayUInt2::Type(), array_serializer.get() );
    defVLXRegistry()->addSerializer( ArrayUInt3::Type(), array_serializer.get() );
    defVLXRegistry()->addSerializer( ArrayUInt4::Type(), array_serializer.get() );

    defVLXRegistry()->addSerializer( ArrayShort1::Type(), array_serializer.get() );
    defVLXRegistry()->addSerializer( ArrayShort2::Type(), array_serializer.get() );
    defVLXRegistry()->addSerializer( ArrayShort3::Type(), array_serializer.get() );
    defVLXRegistry()->addSerializer( ArrayShort4::Type(), array_serializer.get() );

    defVLXRegistry()->addSerializer( ArrayUShort1::Type(), array_serializer.get() );
    defVLXRegistry()->addSerializer( ArrayUShort2::Type(), array_serializer.get() );
    defVLXRegistry()->addSerializer( ArrayUShort3::Type(), array_serializer.get() );
    defVLXRegistry()->addSerializer( ArrayUShort4::Type(), array_serializer.get() );

    defVLXRegistry()->addSerializer( ArrayByte1::Type(), array_serializer.get() );
    defVLXRegistry()->addSerializer( ArrayByte2::Type(), array_serializer.get() );
    defVLXRegistry()->addSerializer( ArrayByte3::Type(), array_serializer.get() );
    defVLXRegistry()->addSerializer( ArrayByte4::Type(), array_serializer.get() );

    defVLXRegistry()->addSerializer( ArrayUByte1::Type(), array_serializer.get() );
    defVLXRegistry()->addSerializer( ArrayUByte2::Type(), array_serializer.get() );
    defVLXRegistry()->addSerializer( ArrayUByte3::Type(), array_serializer.get() );
    defVLXRegistry()->addSerializer( ArrayUByte4::Type(), array_serializer.get() );
  }
}
//------------------------------------------------------------------------------
void VisualizationLibrary::initGraphics()
{
  VL_CHECK(!gInitializedGraphics);
  if (gInitializedGraphics)
  {
    Log::bug("VisualizationLibrary::initGraphics(): Visualization Library Graphics is already initialized!\n");
    return;
  }

  // --- Init Graphics ---

  // Install default FontManager
  gDefaultFontManager = new FontManager;

  // Register VLX serializers
  registerVLXSerializers();

  // Register 3D modules
  #if defined(VL_IO_3D_VLX)
    registerLoadWriter(new LoadWriterVLX);
  #endif
  #if defined(VL_IO_3D_OBJ)
    registerLoadWriter(new LoadWriterOBJ);
  #endif
  #if defined(VL_IO_3D_3DS)
    registerLoadWriter(new LoadWriter3DS);
  #endif
  #if defined(VL_IO_3D_AC3D)
    registerLoadWriter(new LoadWriterAC3D);
  #endif
  #if defined(VL_IO_3D_PLY)
    registerLoadWriter(new LoadWriterPLY);
  #endif
  #if defined(VL_IO_3D_STL)
    registerLoadWriter(new LoadWriterSTL);
  #endif
  #if defined(VL_IO_3D_MD2)
    registerLoadWriter(new LoadWriterMD2);
  #endif
  #if defined(VL_IO_3D_COLLADA)
    registerLoadWriter(new LoadWriterDae);
  #endif

  // ---

  // Initialized = on
  gInitializedGraphics = true;
}
//------------------------------------------------------------------------------
void VisualizationLibrary::shutdownGraphics()
{
  if (gInitializedGraphics)
  {
    gInitializedGraphics = false;

    // --- Dispose Graphics ---

    // Dispose default FontManager
    gDefaultFontManager->releaseAllFonts();
    gDefaultFontManager = NULL;
  }
}
//------------------------------------------------------------------------------
void VisualizationLibrary::init(bool log_info)
{
  initCore(log_info);
  initGraphics();
}
//------------------------------------------------------------------------------
void VisualizationLibrary::shutdown()
{
  shutdownGraphics();
  shutdownCore();
}
//------------------------------------------------------------------------------
bool VisualizationLibrary::isGraphicsInitialized() { return gInitializedGraphics; }
//------------------------------------------------------------------------------
