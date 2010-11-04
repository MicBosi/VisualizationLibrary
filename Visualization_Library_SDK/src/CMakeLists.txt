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

PROJECT(${VISUALIZATIONLIBRARY_NAME})

SET(VL_SRC
  vl/AABB.cpp
  vl/Actor.cpp
  vl/ActorTreeAbstract.cpp
  vl/ActorTree.cpp
  vl/BezierSurface.cpp
  vl/Billboard.cpp
  vl/Camera.cpp
  vl/ClipPlane.cpp
  vl/DiskDirectory.cpp
  vl/DiskFile.cpp
  vl/DoubleVertexRemover.cpp
  vl/DrawElements.cpp
  vl/DrawRangeElements.cpp
  vl/MultiDrawElements.cpp
  vl/DrawPixels.cpp
  vl/EdgeExtractor.cpp
  vl/EdgeRenderer.cpp
  vl/Effect.cpp
  vl/Extrusion.cpp
  vl/Font.cpp
  vl/FontManager.cpp
  vl/FramebufferObject.cpp
  vl/GLSL.cpp
  vl/Geometry.cpp
  vl/GhostCameraManipulator.cpp
  vl/GZipCodec.cpp
  vl/Image.cpp
  vl/Image_Tools.cpp
  vl/ActorKdTree.cpp
  vl/KeyValues.cpp
  vl/Light.cpp
  vl/LoadWriterManager.cpp
  vl/Log.cpp
  vl/MD5CheckSum.cpp
  vl/MemoryDirectory.cpp
  vl/MemoryFile.cpp
  vl/MorphingActor.cpp
  vl/FileSystem.cpp
  vl/OcclusionCullRenderer.cpp
  vl/Object.cpp
  vl/OpenGL.cpp
  vl/OpenGLContext.cpp
  vl/PixelLODEvaluator.cpp
  vl/Plane.cpp
  vl/PolygonSimplifier.cpp
  vl/DrawCall.cpp
  vl/SceneManagerPortals.cpp
  vl/RayIntersector.cpp
  vl/RenderTarget.cpp
  vl/Renderer.cpp
  vl/Rendering.cpp
  vl/RenderingAbstract.cpp
  vl/RenderingTree.cpp
  vl/RenderStateSet.cpp
  vl/UniformSet.cpp
  vl/Say.cpp
  vl/SceneManager.cpp
  vl/Shader.cpp
  vl/String.cpp
  vl/TriangleStripGenerator.cpp
  vl/Terrain.cpp
  vl/Tessellator.cpp
  vl/Text.cpp
  vl/TextStream.cpp
  vl/Texture.cpp
  vl/Time.cpp
  vl/TrackballManipulator.cpp
  vl/Transform.cpp
  vl/Viewport.cpp
  vl/VirtualDirectory.cpp
  vl/VirtualFile.cpp
  vl/VisualizationLibrary.cpp
  vl/ZippedDirectory.cpp
  vl/ZippedFile.cpp
  vl/math3D.cpp
  vl/quat.cpp
)

set(VL_INC
  vl/AABB.hpp
  vl/Actor.hpp
  vl/Array.hpp
  vl/BezierSurface.hpp
  vl/Billboard.hpp
  vl/BlitFramebuffer.hpp
  vl/ActorTreeAbstract.hpp
  vl/ActorTree.hpp
  vl/Buffer.hpp
  vl/GLBufferObject.hpp
  vl/BufferedStream.hpp
  vl/CRC32CheckSum.hpp
  vl/Camera.hpp
  vl/CatmullRomInterpolator.hpp
  vl/Clear.hpp
  vl/ClipPlane.hpp
  vl/CopyTexSubImage.hpp
  vl/DepthSortCallback.hpp
  vl/DiskDirectory.hpp
  vl/DiskFile.hpp
  vl/DistanceLODEvaluator.hpp
  vl/DoubleVertexRemover.hpp
  vl/DrawArrays.hpp
  vl/DrawCall.hpp
  vl/DrawElements.hpp
  vl/DrawPixels.hpp
  vl/EdgeExtractor.hpp
  vl/EdgeRenderer.hpp
  vl/EdgeUpdateCallback.hpp
  vl/Effect.hpp
  vl/Extrusion.hpp
  vl/Font.hpp
  vl/FontManager.hpp
  vl/FramebufferObject.hpp
  vl/Frustum.hpp
  vl/GLSL.hpp
  vl/GLSLmath.hpp
  vl/Geometry.hpp
  vl/GeometryLoadCallback.hpp
  vl/GhostCameraManipulator.hpp
  vl/GZipCodec.hpp
  vl/KeyValues.hpp
  vl/Image.hpp
  vl/Interpolator.hpp
  vl/ActorKdTree.hpp
  vl/LODEvaluator.hpp
  vl/Light.hpp
  vl/LinearInterpolator.hpp
  vl/LoadWriterManager.hpp
  vl/Log.hpp
  vl/Matrix4.hpp
  vl/Matrix3.hpp
  vl/Matrix2.hpp
  vl/MD5CheckSum.hpp
  vl/MemoryDirectory.hpp
  vl/MemoryFile.hpp
  vl/MorphingActor.hpp
  vl/FileSystem.hpp
  vl/Object.hpp
  vl/Collection.hpp
  vl/OcclusionCullRenderer.hpp
  vl/OpenGL.hpp
  vl/OpenGLContext.hpp
  vl/PixelLODEvaluator.hpp
  vl/Plane.hpp
  vl/PolygonSimplifier.hpp
  vl/SceneManagerPortals.hpp
  vl/Ray.hpp
  vl/RayIntersector.hpp
  vl/ReadPixels.hpp
  vl/Rect.hpp
  vl/RenderQueue.hpp
  vl/RenderQueueSorter.hpp
  vl/RenderState.hpp
  vl/RenderTarget.hpp
  vl/RenderToken.hpp
  vl/Renderable.hpp
  vl/Renderer.hpp
  vl/Rendering.hpp
  vl/RenderingAbstract.hpp
  vl/RenderingTree.hpp
  vl/RenderingCallback.hpp
  vl/RenderStateSet.hpp
  vl/EnableSet.hpp
  vl/UniformSet.hpp
  vl/ResourceDatabase.hpp
  vl/ResourceLoadWriter.hpp
  vl/Say.hpp
  vl/SceneManager.hpp
  vl/SceneManagerActorTree.hpp
  vl/SceneManagerActorKdTree.hpp
  vl/SceneManagerBVH.hpp
  vl/Scissor.hpp
  vl/Shader.hpp
  vl/ShaderNode.hpp
  vl/Sphere.hpp
  vl/String.hpp
  vl/String_Tables.hpp
  vl/TriangleStripGenerator.hpp
  vl/Terrain.hpp
  vl/Tessellator.hpp
  vl/Text.hpp
  vl/TextStream.hpp
  vl/Texture.hpp
  vl/Time.hpp
  vl/TrackballManipulator.hpp
  vl/Transform.hpp
  vl/TriangleIterator.hpp
  vl/UIEventListener.hpp
  vl/Uniform.hpp
  vl/Vector4.hpp
  vl/Vector3.hpp
  vl/Vector2.hpp
  vl/VertexAttributeArray.hpp
  vl/Viewport.hpp
  vl/VirtualDirectory.hpp
  vl/VirtualFile.hpp
  vl/VisualizationLibrary.hpp
  vl/ZippedDirectory.hpp
  vl/ZippedFile.hpp
  vl/checks.hpp
  vl/config.hpp
  vl/math3D.hpp
  vl/quat.hpp
  vl/version.hpp
  vl/vlnamespace.hpp
)

if(IO_MODULE_JPG)
  SET(VL_SRC ${VL_SRC} vl/vlJPG.cpp)
  SET(VL_INC ${VL_INC} vl/vlJPG.hpp)
endif(IO_MODULE_JPG)

if(IO_MODULE_PNG)
  SET(VL_SRC ${VL_SRC} vl/vlPNG.cpp)
  SET(VL_INC ${VL_INC} vl/vlPNG.hpp)
endif(IO_MODULE_PNG)

if(IO_MODULE_MD2)
  SET(VL_SRC ${VL_SRC} vl/vlMD2.cpp)
  SET(VL_INC ${VL_INC} vl/vlMD2.hpp)
endif(IO_MODULE_MD2)

if(IO_MODULE_PLY)
  SET(VL_SRC ${VL_SRC} vl/vlPLY.cpp)
  SET(VL_INC ${VL_INC} vl/vlPLY.hpp)
endif(IO_MODULE_PLY)

if(IO_MODULE_STL)
  SET(VL_SRC ${VL_SRC} vl/vlSTL.cpp)
  SET(VL_INC ${VL_INC} vl/vlSTL.hpp)
endif(IO_MODULE_STL)

if(IO_MODULE_AC3D)
  SET(VL_SRC ${VL_SRC} vl/vlAC3D.cpp)
  SET(VL_INC ${VL_INC} vl/vlAC3D.hpp)
endif(IO_MODULE_AC3D)

if(IO_MODULE_OBJ)
  SET(VL_SRC ${VL_SRC} vl/vlOBJ.cpp)
  SET(VL_INC ${VL_INC} vl/vlOBJ.hpp)
endif(IO_MODULE_OBJ)

if(IO_MODULE_3DS)
  SET(VL_SRC ${VL_SRC} vl/vl3DS.cpp)
  SET(VL_INC ${VL_INC} vl/vl3DS.hpp)
endif(IO_MODULE_3DS)

if(IO_MODULE_DAT)
  SET(VL_SRC ${VL_SRC} vl/vlDAT.cpp)
  SET(VL_INC ${VL_INC} vl/vlDAT.hpp)
endif(IO_MODULE_DAT)

if(IO_MODULE_BMP)
  SET(VL_SRC ${VL_SRC} vl/vlBMP.cpp)
  SET(VL_INC ${VL_INC} vl/vlBMP.hpp)
endif(IO_MODULE_BMP)

if(IO_MODULE_TGA)
  SET(VL_SRC ${VL_SRC} vl/vlTGA.cpp)
  SET(VL_INC ${VL_INC} vl/vlTGA.hpp)
endif(IO_MODULE_TGA)

if(IO_MODULE_DDS)
  SET(VL_SRC ${VL_SRC} vl/vlDDS.cpp)
  SET(VL_INC ${VL_INC} vl/vlDDS.hpp)
endif(IO_MODULE_DDS)

if(IO_MODULE_TIFF)
  SET(VL_SRC ${VL_SRC} vl/vlTIFF.cpp)
  SET(VL_INC ${VL_INC} vl/vlTIFF.hpp)
endif(IO_MODULE_TIFF)

if(IO_MODULE_DICOM)
  SET(VL_SRC ${VL_SRC} vl/vlDICOM.cpp)
  SET(VL_INC ${VL_INC} vl/vlDICOM.hpp)
endif(IO_MODULE_DICOM)

set(VLUT_SRC
  vlut/GeometryPrimitives.cpp
  vlut/Applet.cpp
)

set(VLUT_INC
  vlut/GeometryPrimitives.hpp
  vlut/Applet.hpp
  vlut/Colors.hpp
)

set(VLVG_INC
  vlVG/SceneManagerVectorGraphics.hpp
  vlVG/VectorGraphics.hpp
)

set(VLVOLUME_INC
  vlVolume/MarchingCubes.hpp
  vlVolume/SlicedVolume.hpp
  vlVolume/VolumePlot.hpp
)

set(VLVOLUME_SRC
  vlVolume/MarchingCubes.cpp
  vlVolume/SlicedVolume.cpp
  vlVolume/VolumePlot.cpp
)

set(VLMOLECULE_INC
  vlMolecule/Molecule.hpp
  vlMolecule/Atom.hpp
  vlMolecule/Bond.hpp
  vlMolecule/RingExtractor.hpp
  vlMolecule/chem_database.hpp
)

set(VLMOLECULE_SRC
  vlMolecule/Molecule.cpp
  vlMolecule/Molecule_rendering.cpp
  vlMolecule/vlMOL2.cpp
  vlMolecule/chem_database.cpp
)

source_group(vl FILES ${VL_INC} ${VL_SRC})
source_group(vlut FILES ${VLUT_INC} ${VLUT_SRC})
source_group(vlVG FILES ${VLVG_INC})
source_group(vlVolume FILES ${VLVOLUME_INC} ${VLVOLUME_SRC})
source_group(vlMolecule FILES ${VLMOLECULE_INC} ${VLMOLECULE_SRC})

add_library(${VISUALIZATIONLIBRARY_NAME} STATIC 
  ${VL_APPLETS_INC}
  ${VL_SRC} ${VL_INC} 
  ${VLUT_SRC} ${VLUT_INC} 
  ${VLVG_INC} 
  ${VLVOLUME_INC} ${VLVOLUME_SRC}
  ${VLMOLECULE_INC} ${VLMOLECULE_SRC}
  )
set_target_properties(${VISUALIZATIONLIBRARY_NAME} PROPERTIES DEBUG_POSTFIX "-d")
set_target_properties(${VISUALIZATIONLIBRARY_NAME} PROPERTIES SOVERSION "${VLVERSION_MAJOR}.{VLVERSION_MINOR}")
set_target_properties(${VISUALIZATIONLIBRARY_NAME} PROPERTIES VERSION "${VLVERSION_MAJOR}.{VLVERSION_MINOR}")

install(TARGETS ${VISUALIZATIONLIBRARY_NAME}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

install(FILES ${VL_INC}
  DESTINATION include/vl
)

install(FILES ${VLUT_INC}
  DESTINATION include/vlut
)

install(FILES ${VLVG_INC}
  DESTINATION include/vlVG
)

install(FILES ${VLVOLUME_INC}
  DESTINATION include/vlVolume
)

install(FILES ${VLMOLECULE_INC}
  DESTINATION include/vlMolecule
)

