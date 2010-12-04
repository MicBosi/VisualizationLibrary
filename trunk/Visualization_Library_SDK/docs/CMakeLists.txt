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

PROJECT(Documentation)

SET(VL_DOCUMENTATION
	Doxyfile
	dox/mainpage.dox
	dox/namespaces.dox
	dox/pagChangelog.dox
	dox/pagContacts.dox
	dox/pagContributing.dox
	dox/pagCredits.dox
	dox/pagDesign.dox
	dox/pagFAQ.dox
	dox/pagGuideAntialiasing.dox
	dox/pagGuideBezierSurfaces.dox
	dox/pagGuideBillboards.dox
	dox/pagGuideClipping.dox
	dox/pagGuideEdgeRendering.dox
	dox/pagGuideExtrusion.dox
	dox/pagGuideFramebufferObject.dox
	dox/pagGuideGLSL.dox
	dox/pagGuideInterpolators.dox
	dox/pagGuideIO.dox
	dox/pagGuideLights.dox
	dox/pagGuideMarchingCubes.dox
	dox/pagGuideMath.dox
	dox/pagGuideMemory.dox
	dox/pagGuideMolecule.dox
	dox/pagGuideOcclusionCulling.dox
	dox/pagGuidePicking.dox
	dox/pagGuidePolygonDepthSorting.dox
	dox/pagGuidePortals.dox
	dox/pagGuidePrimitives.dox
	dox/pagGuideRenderOrder.dox
	dox/pagGuideRotatingCube.dox
	dox/pagGuides.dox
	dox/pagGuideSlicedVolume.dox
	dox/pagGuideTerrain.dox
	dox/pagGuideTessellation.dox
	dox/pagGuideText.dox
	dox/pagGuideTextures.dox
	dox/pagGuideVectorGraphics.dox
	dox/pagInstall.dox
	dox/pagIntro.dox
	dox/pagKeyFeatures.dox
	dox/pagLearning.dox
	dox/pagLicensing.dox
	dox/pagMainClasses.dox
	dox/pagOpenGL.dox
)

source_group(Documentation FILES ${VL_DOCUMENTATION})

add_custom_target(Documentation SOURCES  ${VL_DOCUMENTATION})
