######################################################################################
#                                                                                    #
#  Visualization Library                                                             #
#  http://www.visualizationlibrary.com                                               #
#                                                                                    #
#  Copyright (c) 2005-2010, Michele Bosi                                             #
#  All rights reserved.                                                              #
#                                                                                    #
#  Redistribution and use in source and binary forms, with or without modification,  #
#  are permitted provided that the following conditions are met:                     #
#                                                                                    #
#  - Redistributions of source code must retain the above copyright notice, this     #
#  list of conditions and the following disclaimer.                                  #
#                                                                                    #
#  - Redistributions in binary form must reproduce the above copyright notice, this  #
#  list of conditions and the following disclaimer in the documentation and/or       #
#  other materials provided with the distribution.                                   #
#                                                                                    #
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND   #
#  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED     #
#  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE            #
#  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR  #
#  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES    #
#  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      #
#  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON    #
#  ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT           #
#  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS     #
#  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                      #
#                                                                                    #
######################################################################################

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(${VLCORE_NAME})

SET(VL_SRC
  AABB.cpp
  Actor.cpp
  ActorTreeAbstract.cpp
  ActorTree.cpp
  BezierSurface.cpp
  Billboard.cpp
  Camera.cpp
  Clear.cpp
  ClipPlane.cpp
  DiskDirectory.cpp
  DiskFile.cpp
  DoubleVertexRemover.cpp
  DrawPixels.cpp
  EdgeExtractor.cpp
  EdgeRenderer.cpp
  Effect.cpp
  Extrusion.cpp
  Font.cpp
  FontManager.cpp
  FramebufferObject.cpp
  GLSL.cpp
  Geometry.cpp
  GhostCameraManipulator.cpp
  GZipCodec.cpp
  UUID.cpp
  Image.cpp
  Image_Tools.cpp
  ActorKdTree.cpp
  KeyValues.cpp
  Light.cpp
  LoadWriterManager.cpp
  Log.cpp
  MD5CheckSum.cpp
  MemoryDirectory.cpp
  MemoryFile.cpp
  MorphingCallback.cpp
  FileSystem.cpp
  OcclusionCullRenderer.cpp
  Object.cpp
  OpenGL.cpp
  OpenGLContext.cpp
  PixelLODEvaluator.cpp
  Plane.cpp
  PolygonSimplifier.cpp
  ProjViewTransfCallback.cpp
  SceneManagerPortals.cpp
  Random.cpp
  RayIntersector.cpp
  RenderTarget.cpp
  Renderer.cpp
  Rendering.cpp
  RenderingAbstract.cpp
  RenderingTree.cpp
  RenderStateSet.cpp
  UniformSet.cpp
  Say.cpp
  SceneManager.cpp
  Shader.cpp
  String.cpp
  TriangleStripGenerator.cpp
  Terrain.cpp
  Tessellator.cpp
  Text.cpp
  TextStream.cpp
  Texture.cpp
  Time.cpp
  TrackballManipulator.cpp
  Transform.cpp
  Viewport.cpp
  VirtualDirectory.cpp
  VirtualFile.cpp
  VisualizationLibrary.cpp
  ZippedDirectory.cpp
  ZippedFile.cpp
  math3D.cpp
)

set(VL_INC
  AABB.hpp
  Actor.hpp
  Array.hpp
  BezierSurface.hpp
  Billboard.hpp
  BlitFramebuffer.hpp
  ActorTreeAbstract.hpp
  ActorTree.hpp
  Buffer.hpp
  GLBufferObject.hpp
  BufferedStream.hpp
  CRC32CheckSum.hpp
  Camera.hpp
  CatmullRomInterpolator.hpp
  Clear.hpp
  ClipPlane.hpp
  CopyTexSubImage.hpp
  DepthSortCallback.hpp
  DiskDirectory.hpp
  DiskFile.hpp
  DistanceLODEvaluator.hpp
  DoubleVertexRemover.hpp
  DrawArrays.hpp
  DrawCall.hpp
  DrawElements.hpp
  DrawRangeElements.hpp
  MultiDrawElements.hpp
  DrawPixels.hpp
  EdgeExtractor.hpp
  EdgeRenderer.hpp
  EdgeUpdateCallback.hpp
  Effect.hpp
  Extrusion.hpp
  Font.hpp
  FontManager.hpp
  FramebufferObject.hpp
  Frustum.hpp
  GLSL.hpp
  GLSLmath.hpp
  Geometry.hpp
  GeometryLoadCallback.hpp
  GhostCameraManipulator.hpp
  GZipCodec.hpp
  UUID.hpp
  half.hpp
  KeyValues.hpp
  Image.hpp
  IndexIterator.hpp
  Interpolator.hpp
  IVertexAttribSet.hpp
  ActorKdTree.hpp
  LODEvaluator.hpp
  Light.hpp
  LinearInterpolator.hpp
  LoadWriterManager.hpp
  Log.hpp
  Matrix4.hpp
  Matrix3.hpp
  Matrix2.hpp
  MD5CheckSum.hpp
  MemoryDirectory.hpp
  MemoryFile.hpp
  MorphingCallback.hpp
  FileSystem.hpp
  Object.hpp
  Collection.hpp
  OcclusionCullRenderer.hpp
  OpenGL.hpp
  OpenGLContext.hpp
  PatchParameter.hpp
  PixelLODEvaluator.hpp
  Plane.hpp
  PolygonSimplifier.hpp
  ProjViewTransfCallback.hpp
  SceneManagerPortals.hpp
  Random.hpp
  Ray.hpp
  RayIntersector.hpp
  ReadPixels.hpp
  Rect.hpp
  RenderQueue.hpp
  RenderQueueSorter.hpp
  RenderState.hpp
  RenderTarget.hpp
  RenderToken.hpp
  Renderable.hpp
  Renderer.hpp
  RendererAbstract.hpp
  Rendering.hpp
  RenderingAbstract.hpp
  RenderingTree.hpp
  RenderEventCallback.hpp
  RenderStateSet.hpp
  EnableSet.hpp
  UniformSet.hpp
  ResourceDatabase.hpp
  ResourceLoadWriter.hpp
  Say.hpp
  SceneManager.hpp
  SceneManagerActorTree.hpp
  SceneManagerActorKdTree.hpp
  SceneManagerBVH.hpp
  Scissor.hpp
  Shader.hpp
  ShaderNode.hpp
  SignalSlot.hpp
  SignalSlotX.hpp
  Sphere.hpp
  String.hpp
  String_Tables.hpp
  TriangleStripGenerator.hpp
  Terrain.hpp
  Tessellator.hpp
  Text.hpp
  TextStream.hpp
  Texture.hpp
  Time.hpp
  TrackballManipulator.hpp
  Transform.hpp
  TriangleIterator.hpp
  UIEventListener.hpp
  Uniform.hpp
  Vector4.hpp
  Vector3.hpp
  Vector2.hpp
  VertexAttribInfo.hpp
  Viewport.hpp
  VirtualDirectory.hpp
  VirtualFile.hpp
  VisualizationLibrary.hpp
  ZippedDirectory.hpp
  ZippedFile.hpp
  checks.hpp
  config.hpp
  math3D.hpp
  Quaternion.hpp
  version.hpp
  vlnamespace.hpp
)

if(IO_MODULE_JPG)
  SET(VL_SRC ${VL_SRC} vlJPG.cpp)
  SET(VL_INC ${VL_INC} vlJPG.hpp)
endif(IO_MODULE_JPG)

if(IO_MODULE_PNG)
  SET(VL_SRC ${VL_SRC} vlPNG.cpp)
  SET(VL_INC ${VL_INC} vlPNG.hpp)
endif(IO_MODULE_PNG)

if(IO_MODULE_MD2)
  SET(VL_SRC ${VL_SRC} vlMD2.cpp)
  SET(VL_INC ${VL_INC} vlMD2.hpp)
endif(IO_MODULE_MD2)

if(IO_MODULE_PLY)
  SET(VL_SRC ${VL_SRC} vlPLY.cpp)
  SET(VL_INC ${VL_INC} vlPLY.hpp)
endif(IO_MODULE_PLY)

if(IO_MODULE_STL)
  SET(VL_SRC ${VL_SRC} vlSTL.cpp)
  SET(VL_INC ${VL_INC} vlSTL.hpp)
endif(IO_MODULE_STL)

if(IO_MODULE_AC3D)
  SET(VL_SRC ${VL_SRC} vlAC3D.cpp)
  SET(VL_INC ${VL_INC} vlAC3D.hpp)
endif(IO_MODULE_AC3D)

if(IO_MODULE_OBJ)
  SET(VL_SRC ${VL_SRC} vlOBJ.cpp)
  SET(VL_INC ${VL_INC} vlOBJ.hpp)
endif(IO_MODULE_OBJ)

if(IO_MODULE_3DS)
  SET(VL_SRC ${VL_SRC} vl3DS.cpp)
  SET(VL_INC ${VL_INC} vl3DS.hpp)
endif(IO_MODULE_3DS)

if(IO_MODULE_DAT)
  SET(VL_SRC ${VL_SRC} vlDAT.cpp)
  SET(VL_INC ${VL_INC} vlDAT.hpp)
endif(IO_MODULE_DAT)

if(IO_MODULE_BMP)
  SET(VL_SRC ${VL_SRC} vlBMP.cpp)
  SET(VL_INC ${VL_INC} vlBMP.hpp)
endif(IO_MODULE_BMP)

if(IO_MODULE_TGA)
  SET(VL_SRC ${VL_SRC} vlTGA.cpp)
  SET(VL_INC ${VL_INC} vlTGA.hpp)
endif(IO_MODULE_TGA)

if(IO_MODULE_DDS)
  SET(VL_SRC ${VL_SRC} vlDDS.cpp)
  SET(VL_INC ${VL_INC} vlDDS.hpp)
endif(IO_MODULE_DDS)

if(IO_MODULE_TIFF)
  SET(VL_SRC ${VL_SRC} vlTIFF.cpp)
  SET(VL_INC ${VL_INC} vlTIFF.hpp)
endif(IO_MODULE_TIFF)

if(IO_MODULE_DICOM)
  SET(VL_SRC ${VL_SRC} vlDICOM.cpp)
  SET(VL_INC ${VL_INC} vlDICOM.hpp)
endif(IO_MODULE_DICOM)

#source_group(vl FILES ${VL_INC} ${VL_SRC})

add_library(${VLCORE_NAME} STATIC ${VL_SRC} ${VL_INC} )

set_target_properties(${VLCORE_NAME} PROPERTIES DEBUG_POSTFIX "-d")
set_target_properties(${VLCORE_NAME} PROPERTIES SOVERSION "${VLVERSION_MAJOR}.{VLVERSION_MINOR}")
set_target_properties(${VLCORE_NAME} PROPERTIES VERSION "${VLVERSION_MAJOR}.{VLVERSION_MINOR}")

install(TARGETS ${VLCORE_NAME}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

install(FILES ${VL_INC}
  DESTINATION include/vl
)
