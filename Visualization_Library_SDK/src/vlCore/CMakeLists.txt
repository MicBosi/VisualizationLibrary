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

SET(VLCORE_SRC
  AABB.cpp
  DiskDirectory.cpp
  DiskFile.cpp
  GZipCodec.cpp
  UUID.cpp
  Image.cpp
  KeyValues.cpp
  LoadWriterManager.cpp
  Log.cpp
  MD5CheckSum.cpp
  MemoryDirectory.cpp
  MemoryFile.cpp
  FileSystem.cpp
  Object.cpp
  Plane.cpp
  Random.cpp
  Say.cpp
  VLSettings.cpp
  String.cpp
  TextStream.cpp
  Time.cpp
  Transform.cpp
  VirtualDirectory.cpp
  VirtualFile.cpp
  VisualizationLibrary.cpp
  ZippedDirectory.cpp
  ZippedFile.cpp
  math3D.cpp
)

set(VLCORE_INC
  AABB.hpp
  Array.hpp
  Buffer.hpp
  BufferedStream.hpp
  CatmullRomInterpolator.hpp
  Collection.hpp
  Colors.hpp
  CRC32CheckSum.hpp
  DiskDirectory.hpp
  DiskFile.hpp
  GZipCodec.hpp
  UUID.hpp
  half.hpp
  KeyValues.hpp
  GLSLmath.hpp
  Image.hpp
  ImageTools.hpp
  Interpolator.hpp
  LinearInterpolator.hpp
  LoadWriterManager.hpp
  Log.hpp
  Matrix4.hpp
  Matrix3.hpp
  Matrix2.hpp
  MD5CheckSum.hpp
  MemoryDirectory.hpp
  MemoryFile.hpp
  FileSystem.hpp
  Object.hpp
  OpenGLDefs.hpp
  Plane.hpp
  Random.hpp
  Ray.hpp
  Rect.hpp
  ResourceDatabase.hpp
  ResourceLoadWriter.hpp
  Say.hpp
  VLSettings.hpp
  SignalSlot.hpp
  SignalSlotX.hpp
  Sphere.hpp
  String.hpp
  String_Tables.hpp
  TextStream.hpp
  Time.hpp
  Transform.hpp
  Vector4.hpp
  Vector3.hpp
  Vector2.hpp
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

if(VL_IO_2D_BMP)
  SET(VLCORE_SRC ${VLCORE_SRC} vlBMP.cpp)
  SET(VLCORE_INC ${VLCORE_INC} vlBMP.hpp)
endif(VL_IO_2D_BMP)

if(VL_IO_2D_DAT)
  SET(VLCORE_SRC ${VLCORE_SRC} vlDAT.cpp)
  SET(VLCORE_INC ${VLCORE_INC} vlDAT.hpp)
endif(VL_IO_2D_DAT)

if(VL_IO_2D_DDS)
  SET(VLCORE_SRC ${VLCORE_SRC} vlDDS.cpp)
  SET(VLCORE_INC ${VLCORE_INC} vlDDS.hpp)
endif(VL_IO_2D_DDS)

if(VL_IO_2D_DICOM)
  SET(VLCORE_SRC ${VLCORE_SRC} vlDICOM.cpp)
  SET(VLCORE_INC ${VLCORE_INC} vlDICOM.hpp)
endif(VL_IO_2D_DICOM)

if(VL_IO_2D_JPG)
  SET(VLCORE_SRC ${VLCORE_SRC} vlJPG.cpp)
  SET(VLCORE_INC ${VLCORE_INC} vlJPG.hpp)
endif(VL_IO_2D_JPG)

if(VL_IO_2D_PNG)
  SET(VLCORE_SRC ${VLCORE_SRC} vlPNG.cpp)
  SET(VLCORE_INC ${VLCORE_INC} vlPNG.hpp)
endif(VL_IO_2D_PNG)

if(VL_IO_2D_TGA)
  SET(VLCORE_SRC ${VLCORE_SRC} vlTGA.cpp)
  SET(VLCORE_INC ${VLCORE_INC} vlTGA.hpp)
endif(VL_IO_2D_TGA)

if(VL_IO_2D_TIFF)
  SET(VLCORE_SRC ${VLCORE_SRC} vlTIFF.cpp)
  SET(VLCORE_INC ${VLCORE_INC} vlTIFF.hpp)
endif(VL_IO_2D_TIFF)

#source_group(vl FILES ${VLCORE_INC} ${VLCORE_SRC})

add_library(${VLCORE_NAME} STATIC ${VLCORE_SRC} ${VLCORE_INC} )

set_target_properties(${VLCORE_NAME} PROPERTIES DEBUG_POSTFIX "-d")
set_target_properties(${VLCORE_NAME} PROPERTIES SOVERSION "${VLVERSION_MAJOR}.{VLVERSION_MINOR}")
set_target_properties(${VLCORE_NAME} PROPERTIES VERSION "${VLVERSION_MAJOR}.{VLVERSION_MINOR}")

install(TARGETS ${VLCORE_NAME}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

install(FILES ${VLCORE_INC}
  DESTINATION include/vlCore
)
