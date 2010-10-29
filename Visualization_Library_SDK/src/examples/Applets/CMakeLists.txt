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

set(VL_APPLETS
  BaseDemo.hpp
  App_BezierSurfaces.hpp
  App_Billboards.hpp
  App_ClipPlanes.hpp
  App_CullingBenchmark.hpp
  App_Deformer.hpp
  App_DrawPixels.hpp
  App_EdgeRendering.hpp
  App_EffectOverride.hpp
  App_Extrusion.hpp
  App_FBO_MRT_GLSL.hpp
  App_Fractals.hpp
  App_GeometryInstancing.hpp
  App_GLSL.hpp
  App_GLSLImageProcessing.hpp
  App_GLSL_Bumpmapping.hpp
  App_ImageFunctions.hpp
  App_Interpolators.hpp
  App_KdTreeView.hpp
  App_Lights.hpp
  App_MarchingCubes.hpp
  App_ModelProfiler.hpp
  App_Molecules.hpp
  App_MorphAnimation.hpp
  App_MultipleCameras.hpp
  App_NearFarOptimization.hpp
  App_OcclusionCulling.hpp
  App_Picking.hpp
  App_PointSplatting.hpp
  App_PolyDepthSorting.hpp
  App_PolygonReduction.hpp
  App_PortalCulling.hpp
  App_RenderOrder.hpp
  App_RenderToTextureClassic.hpp
  App_RotatingCube.hpp
  App_ScatterPlot3D.hpp
  App_ShaderLOD.hpp
  App_ShaderOverride.hpp
  App_Terrain.hpp
  App_Tessellator.hpp
  App_TextRendering.hpp
  App_Texturing.hpp
  App_Transforms.hpp
  App_VectorGraphics.hpp
  App_VirtualFileSystemTest.hpp
  App_VolumeRendering.hpp
)

source_group(Applets FILES ${VL_APPLETS})

add_custom_target(Applets SOURCES  ${VL_APPLETS})
