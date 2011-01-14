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

PROJECT(${VLGRAPHICS_NAME})

SET(VLGRAPHICS_SRC
  Actor.cpp
  ActorTreeAbstract.cpp
  ActorTree.cpp
  Applet.cpp
  BezierSurface.cpp
  Billboard.cpp
  Camera.cpp
  Clear.cpp
  ClipPlane.cpp
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
  GeometryPrimitives.cpp
  GhostCameraManipulator.cpp
  ActorKdTree.cpp
  Light.cpp
  MorphingCallback.cpp
  OcclusionCullRenderer.cpp
  OpenGL.cpp
  OpenGLContext.cpp
  PixelLODEvaluator.cpp
  PolygonSimplifier.cpp
  ProjViewTransfCallback.cpp
  SceneManagerPortals.cpp
  RayIntersector.cpp
  RenderTarget.cpp
  Renderer.cpp
  Rendering.cpp
  RenderingAbstract.cpp
  RenderingTree.cpp
  RenderStateSet.cpp
  UniformSet.cpp
  SceneManager.cpp
  Shader.cpp
  TriangleStripGenerator.cpp
  Terrain.cpp
  Tessellator.cpp
  Text.cpp
  Texture.cpp
  TrackballManipulator.cpp
  Viewport.cpp
)

set(VLGRAPHICS_INC
  Actor.hpp
  Applet.hpp
  BezierSurface.hpp
  Billboard.hpp
  BlitFramebuffer.hpp
  ActorTreeAbstract.hpp
  ActorTree.hpp
  GLBufferObject.hpp
  Camera.hpp
  Clear.hpp
  ClipPlane.hpp
  CopyTexSubImage.hpp
  DepthSortCallback.hpp
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
  Geometry.hpp
  GeometryLoadCallback.hpp
  GeometryPrimitives.hpp
  GhostCameraManipulator.hpp
  ImagePBO.hpp
  IndexIterator.hpp
  IVertexAttribSet.hpp
  ActorKdTree.hpp
  LODEvaluator.hpp
  Light.hpp
  MorphingCallback.hpp
  OcclusionCullRenderer.hpp
  OpenGL.hpp
  OpenGLContext.hpp
  PatchParameter.hpp
  PixelLODEvaluator.hpp
  PolygonSimplifier.hpp
  ProjViewTransfCallback.hpp
  SceneManagerPortals.hpp
  RayIntersector.hpp
  ReadPixels.hpp
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
  SceneManager.hpp
  SceneManagerActorTree.hpp
  SceneManagerActorKdTree.hpp
  SceneManagerBVH.hpp
  Scissor.hpp
  Shader.hpp
  ShaderNode.hpp
  TriangleStripGenerator.hpp
  Terrain.hpp
  Tessellator.hpp
  Text.hpp
  Texture.hpp
  TrackballManipulator.hpp
  TriangleIterator.hpp
  UIEventListener.hpp
  Uniform.hpp
  VertexAttribInfo.hpp
  Viewport.hpp
)

if(IO_MODULE_JPG)
  SET(VLGRAPHICS_SRC ${VLGRAPHICS_SRC} vlJPG.cpp)
  SET(VLGRAPHICS_INC ${VLGRAPHICS_INC} vlJPG.hpp)
endif(IO_MODULE_JPG)

if(IO_MODULE_PNG)
  SET(VLGRAPHICS_SRC ${VLGRAPHICS_SRC} vlPNG.cpp)
  SET(VLGRAPHICS_INC ${VLGRAPHICS_INC} vlPNG.hpp)
endif(IO_MODULE_PNG)

if(IO_MODULE_MD2)
  SET(VLGRAPHICS_SRC ${VLGRAPHICS_SRC} vlMD2.cpp)
  SET(VLGRAPHICS_INC ${VLGRAPHICS_INC} vlMD2.hpp)
endif(IO_MODULE_MD2)

if(IO_MODULE_PLY)
  SET(VLGRAPHICS_SRC ${VLGRAPHICS_SRC} vlPLY.cpp)
  SET(VLGRAPHICS_INC ${VLGRAPHICS_INC} vlPLY.hpp)
endif(IO_MODULE_PLY)

if(IO_MODULE_STL)
  SET(VLGRAPHICS_SRC ${VLGRAPHICS_SRC} vlSTL.cpp)
  SET(VLGRAPHICS_INC ${VLGRAPHICS_INC} vlSTL.hpp)
endif(IO_MODULE_STL)

if(IO_MODULE_AC3D)
  SET(VLGRAPHICS_SRC ${VLGRAPHICS_SRC} vlAC3D.cpp)
  SET(VLGRAPHICS_INC ${VLGRAPHICS_INC} vlAC3D.hpp)
endif(IO_MODULE_AC3D)

if(IO_MODULE_OBJ)
  SET(VLGRAPHICS_SRC ${VLGRAPHICS_SRC} vlOBJ.cpp)
  SET(VLGRAPHICS_INC ${VLGRAPHICS_INC} vlOBJ.hpp)
endif(IO_MODULE_OBJ)

if(IO_MODULE_3DS)
  SET(VLGRAPHICS_SRC ${VLGRAPHICS_SRC} vl3DS.cpp)
  SET(VLGRAPHICS_INC ${VLGRAPHICS_INC} vl3DS.hpp)
endif(IO_MODULE_3DS)

if(IO_MODULE_DAT)
  SET(VLGRAPHICS_SRC ${VLGRAPHICS_SRC} vlDAT.cpp)
  SET(VLGRAPHICS_INC ${VLGRAPHICS_INC} vlDAT.hpp)
endif(IO_MODULE_DAT)

if(IO_MODULE_BMP)
  SET(VLGRAPHICS_SRC ${VLGRAPHICS_SRC} vlBMP.cpp)
  SET(VLGRAPHICS_INC ${VLGRAPHICS_INC} vlBMP.hpp)
endif(IO_MODULE_BMP)

if(IO_MODULE_TGA)
  SET(VLGRAPHICS_SRC ${VLGRAPHICS_SRC} vlTGA.cpp)
  SET(VLGRAPHICS_INC ${VLGRAPHICS_INC} vlTGA.hpp)
endif(IO_MODULE_TGA)

if(IO_MODULE_DDS)
  SET(VLGRAPHICS_SRC ${VLGRAPHICS_SRC} vlDDS.cpp)
  SET(VLGRAPHICS_INC ${VLGRAPHICS_INC} vlDDS.hpp)
endif(IO_MODULE_DDS)

if(IO_MODULE_TIFF)
  SET(VLGRAPHICS_SRC ${VLGRAPHICS_SRC} vlTIFF.cpp)
  SET(VLGRAPHICS_INC ${VLGRAPHICS_INC} vlTIFF.hpp)
endif(IO_MODULE_TIFF)

if(IO_MODULE_DICOM)
  SET(VLGRAPHICS_SRC ${VLGRAPHICS_SRC} vlDICOM.cpp)
  SET(VLGRAPHICS_INC ${VLGRAPHICS_INC} vlDICOM.hpp)
endif(IO_MODULE_DICOM)

#source_group(vl FILES ${VLGRAPHICS_INC} ${VLGRAPHICS_SRC})

add_library(${VLGRAPHICS_NAME} STATIC ${VLGRAPHICS_SRC} ${VLGRAPHICS_INC} )

set_target_properties(${VLGRAPHICS_NAME} PROPERTIES DEBUG_POSTFIX "-d")
set_target_properties(${VLGRAPHICS_NAME} PROPERTIES SOVERSION "${VLVERSION_MAJOR}.{VLVERSION_MINOR}")
set_target_properties(${VLGRAPHICS_NAME} PROPERTIES VERSION "${VLVERSION_MAJOR}.{VLVERSION_MINOR}")

install(TARGETS ${VLGRAPHICS_NAME}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

install(FILES ${VLGRAPHICS_INC}
  DESTINATION include/vlGraphics
)
