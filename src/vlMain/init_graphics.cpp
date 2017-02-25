/**************************************************************************************/
/*                                                                                    */
/*  Visualization Library                                                             */
/*  http://visualizationlibrary.org                                                   */
/*                                                                                    */
/*  Copyright (c) 2005-2017, Michele Bosi                                             */
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

#include <vlX/WrappersGraphics.hpp>

#if defined(VL_IO_3D_3DS)
  #include <vlGraphics/plugins/io3DS.hpp>
#endif
#if defined(VL_IO_3D_OBJ)
  #include <vlGraphics/plugins/ioOBJ.hpp>
#endif
#if defined(VL_IO_3D_AC3D)
  #include <vlGraphics/plugins/ioAC3D.hpp>
#endif
#if defined(VL_IO_3D_PLY)
  #include <vlGraphics/plugins/ioPLY.hpp>
#endif
#if defined(VL_IO_3D_STL)
  #include <vlGraphics/plugins/ioSTL.hpp>
#endif
#if defined(VL_IO_3D_MD2)
  #include <vlGraphics/plugins/ioMD2.hpp>
#endif
#if defined(VL_IO_3D_COLLADA)
  #include <vlGraphics/plugins/COLLADA/ioDae.hpp>
#endif

static void registerVLXGraphicsWrappers();

using namespace vl;

//------------------------------------------------------------------------------
namespace
{
  bool gInitializedGraphics = false;
};
//------------------------------------------------------------------------------
void VisualizationLibrary::initGraphics()
{
  VL_CHECK( ! gInitializedGraphics );
  if (gInitializedGraphics) {
    Log::bug("VisualizationLibrary::initGraphics(): Visualization Library Graphics is already initialized!\n");
    return;
  }

  // Install default FontManager
  setDefFontManager( new FontManager );

  // Register VLGraphics classes to VLX
  registerVLXGraphicsWrappers();

  // Register VLGraphics modules

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

  gInitializedGraphics = true;
}
//------------------------------------------------------------------------------
void VisualizationLibrary::shutdownGraphics()
{
  if ( ! gInitializedGraphics ) {
    Log::debug("VisualizationLibrary::shutdownGraphics(): VL Graphics not initialized.\n");
    return;
  }

  gInitializedGraphics = false;

  // Dispose default FontManager
  defFontManager()->releaseAllFonts();
  setDefFontManager( NULL );

  Log::debug("VisualizationLibrary::shutdownGraphics()\n");
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
void registerVLXGraphicsWrappers()
{
  // Geometry serializer
  vlX::defVLXRegistry()->registerClassWrapper( Geometry::Type(), new vlX::VLXClassWrapper_Geometry );

  // BezierSurface
  vlX::defVLXRegistry()->registerClassWrapper( BezierSurface::Type(), new vlX::VLXClassWrapper_Geometry );

  // PatchParameter
  vlX::defVLXRegistry()->registerClassWrapper( PatchParameter::Type(), new vlX::VLXClassWrapper_PatchParameter );

  // DrawCall
  ref<vlX::VLXClassWrapper_DrawCall> drawcall_serializer = new vlX::VLXClassWrapper_DrawCall;
  vlX::defVLXRegistry()->registerClassWrapper( DrawArrays::Type(), drawcall_serializer.get() );
  vlX::defVLXRegistry()->registerClassWrapper( DrawElementsUInt::Type(), drawcall_serializer.get() );
  vlX::defVLXRegistry()->registerClassWrapper( DrawElementsUShort::Type(), drawcall_serializer.get() );
  vlX::defVLXRegistry()->registerClassWrapper( DrawElementsUByte::Type(), drawcall_serializer.get() );
  vlX::defVLXRegistry()->registerClassWrapper( MultiDrawElementsUInt::Type(), drawcall_serializer.get() );
  vlX::defVLXRegistry()->registerClassWrapper( MultiDrawElementsUShort::Type(), drawcall_serializer.get() );
  vlX::defVLXRegistry()->registerClassWrapper( MultiDrawElementsUByte::Type(), drawcall_serializer.get() );

  // ResourceDatabase
  vlX::defVLXRegistry()->registerClassWrapper( ResourceDatabase::Type(), new vlX::VLXClassWrapper_ResourceDatabase );

  // Uniform
  vlX::defVLXRegistry()->registerClassWrapper( Uniform::Type(), new vlX::VLXClassWrapper_Uniform );

  // LODEvaluator
  vlX::defVLXRegistry()->registerClassWrapper( LODEvaluator::Type(), new vlX::VLXClassWrapper_LODEvaluator );

  // Transform
  vlX::defVLXRegistry()->registerClassWrapper( Transform::Type(), new vlX::VLXClassWrapper_Transform );

  // Material
  vlX::defVLXRegistry()->registerClassWrapper( Material::Type(), new vlX::VLXClassWrapper_Material );

  // Texture
  vlX::defVLXRegistry()->registerClassWrapper( Texture::Type(), new vlX::VLXClassWrapper_Texture );

  // TextureSampler
  vlX::defVLXRegistry()->registerClassWrapper( TextureSampler::Type(), new vlX::VLXClassWrapper_TextureSampler );

  // TexParameter
  vlX::defVLXRegistry()->registerClassWrapper( TexParameter::Type(), new vlX::VLXClassWrapper_TexParameter );

  // ActorEventCallback
  vlX::defVLXRegistry()->registerClassWrapper( DepthSortCallback::Type(), new vlX::VLXClassWrapper_ActorEventCallback );

  // LODEvaluator
  ref<vlX::VLXClassWrapper_LODEvaluator> lod_evaluator = new vlX::VLXClassWrapper_LODEvaluator;
  vlX::defVLXRegistry()->registerClassWrapper( PixelLODEvaluator::Type(), lod_evaluator.get() );
  vlX::defVLXRegistry()->registerClassWrapper( DistanceLODEvaluator::Type(), lod_evaluator.get() );

  // Actor
  vlX::defVLXRegistry()->registerClassWrapper( Actor::Type(), new vlX::VLXClassWrapper_Actor );

  // Effect
  vlX::defVLXRegistry()->registerClassWrapper( Effect::Type(), new vlX::VLXClassWrapper_Effect );

  // Shader
  vlX::defVLXRegistry()->registerClassWrapper( Shader::Type(), new vlX::VLXClassWrapper_Shader );

  // Camera
  vlX::defVLXRegistry()->registerClassWrapper( Camera::Type(), new vlX::VLXClassWrapper_Camera );

  // Light
  vlX::defVLXRegistry()->registerClassWrapper( Light::Type(), new vlX::VLXClassWrapper_Light );

  // ClipPlane
  vlX::defVLXRegistry()->registerClassWrapper( ClipPlane::Type(), new vlX::VLXClassWrapper_ClipPlane );

  // Color
  vlX::defVLXRegistry()->registerClassWrapper( Color::Type(), new vlX::VLXClassWrapper_Color );

  // SecondaryColor
  vlX::defVLXRegistry()->registerClassWrapper( SecondaryColor::Type(), new vlX::VLXClassWrapper_SecondaryColor );

  // Normal
  vlX::defVLXRegistry()->registerClassWrapper( Normal::Type(), new vlX::VLXClassWrapper_Normal );

  // VertexAttrib
  vlX::defVLXRegistry()->registerClassWrapper( VertexAttrib::Type(), new vlX::VLXClassWrapper_VertexAttrib );

  // Viewport
  vlX::defVLXRegistry()->registerClassWrapper( Viewport::Type(), new vlX::VLXClassWrapper_Viewport );

  // GLSL
  vlX::defVLXRegistry()->registerClassWrapper( GLSLProgram::Type(), new vlX::VLXClassWrapper_GLSLProgram );
  ref<vlX::VLXClassWrapper_GLSLShader> sh_serializer = new vlX::VLXClassWrapper_GLSLShader;
  vlX::defVLXRegistry()->registerClassWrapper( GLSLVertexShader::Type(), sh_serializer.get() );
  vlX::defVLXRegistry()->registerClassWrapper( GLSLFragmentShader::Type(), sh_serializer.get() );
  vlX::defVLXRegistry()->registerClassWrapper( GLSLGeometryShader::Type(), sh_serializer.get() );
  vlX::defVLXRegistry()->registerClassWrapper( GLSLTessControlShader::Type(), sh_serializer.get() );
  vlX::defVLXRegistry()->registerClassWrapper( GLSLTessEvaluationShader::Type(), sh_serializer.get() );

  // GLSLShader
  vlX::defVLXRegistry()->registerClassWrapper( GLSLShader::Type(), new vlX::VLXClassWrapper_GLSLShader );

  // Array serializer
  ref<vlX::VLXClassWrapper_Array> array_serializer = new vlX::VLXClassWrapper_Array;

  vlX::defVLXRegistry()->registerClassWrapper( ArrayFloat1::Type(), array_serializer.get() );
  vlX::defVLXRegistry()->registerClassWrapper( ArrayFloat2::Type(), array_serializer.get() );
  vlX::defVLXRegistry()->registerClassWrapper( ArrayFloat3::Type(), array_serializer.get() );
  vlX::defVLXRegistry()->registerClassWrapper( ArrayFloat4::Type(), array_serializer.get() );

  vlX::defVLXRegistry()->registerClassWrapper( ArrayDouble1::Type(), array_serializer.get() );
  vlX::defVLXRegistry()->registerClassWrapper( ArrayDouble2::Type(), array_serializer.get() );
  vlX::defVLXRegistry()->registerClassWrapper( ArrayDouble3::Type(), array_serializer.get() );
  vlX::defVLXRegistry()->registerClassWrapper( ArrayDouble4::Type(), array_serializer.get() );

  vlX::defVLXRegistry()->registerClassWrapper( ArrayInt1::Type(), array_serializer.get() );
  vlX::defVLXRegistry()->registerClassWrapper( ArrayInt2::Type(), array_serializer.get() );
  vlX::defVLXRegistry()->registerClassWrapper( ArrayInt3::Type(), array_serializer.get() );
  vlX::defVLXRegistry()->registerClassWrapper( ArrayInt4::Type(), array_serializer.get() );

  vlX::defVLXRegistry()->registerClassWrapper( ArrayUInt1::Type(), array_serializer.get() );
  vlX::defVLXRegistry()->registerClassWrapper( ArrayUInt2::Type(), array_serializer.get() );
  vlX::defVLXRegistry()->registerClassWrapper( ArrayUInt3::Type(), array_serializer.get() );
  vlX::defVLXRegistry()->registerClassWrapper( ArrayUInt4::Type(), array_serializer.get() );

  vlX::defVLXRegistry()->registerClassWrapper( ArrayShort1::Type(), array_serializer.get() );
  vlX::defVLXRegistry()->registerClassWrapper( ArrayShort2::Type(), array_serializer.get() );
  vlX::defVLXRegistry()->registerClassWrapper( ArrayShort3::Type(), array_serializer.get() );
  vlX::defVLXRegistry()->registerClassWrapper( ArrayShort4::Type(), array_serializer.get() );

  vlX::defVLXRegistry()->registerClassWrapper( ArrayUShort1::Type(), array_serializer.get() );
  vlX::defVLXRegistry()->registerClassWrapper( ArrayUShort2::Type(), array_serializer.get() );
  vlX::defVLXRegistry()->registerClassWrapper( ArrayUShort3::Type(), array_serializer.get() );
  vlX::defVLXRegistry()->registerClassWrapper( ArrayUShort4::Type(), array_serializer.get() );

  vlX::defVLXRegistry()->registerClassWrapper( ArrayByte1::Type(), array_serializer.get() );
  vlX::defVLXRegistry()->registerClassWrapper( ArrayByte2::Type(), array_serializer.get() );
  vlX::defVLXRegistry()->registerClassWrapper( ArrayByte3::Type(), array_serializer.get() );
  vlX::defVLXRegistry()->registerClassWrapper( ArrayByte4::Type(), array_serializer.get() );

  vlX::defVLXRegistry()->registerClassWrapper( ArrayUByte1::Type(), array_serializer.get() );
  vlX::defVLXRegistry()->registerClassWrapper( ArrayUByte2::Type(), array_serializer.get() );
  vlX::defVLXRegistry()->registerClassWrapper( ArrayUByte3::Type(), array_serializer.get() );
  vlX::defVLXRegistry()->registerClassWrapper( ArrayUByte4::Type(), array_serializer.get() );
}
//------------------------------------------------------------------------------
