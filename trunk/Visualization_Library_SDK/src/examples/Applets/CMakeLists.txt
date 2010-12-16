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

PROJECT(Applets)

set(VL_APPLETS_INC
  BaseDemo.hpp
  App_RotatingCube.hpp
)

set(VL_APPLETS_SRC
  App_BezierSurfaces.cpp
  App_Billboards.cpp
  App_ClipPlanes.cpp
  App_CullingBenchmark.cpp
  App_Deformer.cpp
  App_DrawPixels.cpp
  App_DrawCalls.cpp
  App_EdgeRendering.cpp
  App_EffectOverride.cpp
  App_Extrusion.cpp
  App_Framebuffer_Object.cpp
  App_Fractals.cpp
  App_GeometryInstancing.cpp
  App_GLSL.cpp
  App_GLSLImageProcessing.cpp
  App_GLSL_Bumpmapping.cpp
  App_ImageFunctions.cpp
  App_Interpolators.cpp
  App_KdTreeView.cpp
  App_Lights.cpp
  App_MarchingCubes.cpp
  App_ModelProfiler.cpp
  App_Molecules.cpp
  App_MorphAnimation.cpp
  App_MultipleCameras.cpp
  App_NearFarOptimization.cpp
  App_OcclusionCulling.cpp
  App_Picking.cpp
  App_PointSplatting.cpp
  App_PolyDepthSorting.cpp
  App_PolygonReduction.cpp
  App_PortalCulling.cpp
  App_Primitives.cpp
  App_RenderOrder.cpp
  App_ScatterPlot3D.cpp
  App_ShaderMultiPassLODAnim.cpp
  App_ShaderMultiPassAlpha.cpp
  App_GeomLODAnim.cpp
  App_ShaderOverride.cpp
  App_Terrain.cpp
  App_Tessellator.cpp
  App_TextRendering.cpp
  App_Texturing.cpp
  App_Transforms.cpp
  App_VectorGraphics.cpp
  App_VirtualFileSystemTest.cpp
  App_VolumeSliced.cpp
  App_VolumeRaycast.cpp
  
  #blind tests
  App_BlindTests/App_BlindTests.cpp
  App_BlindTests/test_signal_slot.cpp
)

add_library(${VLAPPLETS_NAME} STATIC ${VL_APPLETS_INC} ${VL_APPLETS_SRC})
set_target_properties(${VLAPPLETS_NAME} PROPERTIES DEBUG_POSTFIX "-d")
set_target_properties(${VLAPPLETS_NAME} PROPERTIES SOVERSION "${VLVERSION_MAJOR}.{VLVERSION_MINOR}")
set_target_properties(${VLAPPLETS_NAME} PROPERTIES VERSION "${VLVERSION_MAJOR}.{VLVERSION_MINOR}")
