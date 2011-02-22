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

PROJECT(${VLVOLUME_NAME})

set(VLVOLUME_INC
  MarchingCubes.hpp
  SlicedVolume.hpp
  VolumePlot.hpp
  VolumeUtils.hpp
)

set(VLVOLUME_SRC
  MarchingCubes.cpp
  SlicedVolume.cpp
  VolumePlot.cpp
  VolumeUtils.cpp
)

#source_group(vlVolume FILES ${VLVOLUME_INC} ${VLVOLUME_SRC})

add_library(${VLVOLUME_NAME} STATIC ${VLVOLUME_INC} ${VLVOLUME_SRC} )
set_target_properties(${VLVOLUME_NAME} PROPERTIES DEBUG_POSTFIX "-d")
set_target_properties(${VLVOLUME_NAME} PROPERTIES SOVERSION "${VLVERSION_MAJOR}.{VLVERSION_MINOR}")
set_target_properties(${VLVOLUME_NAME} PROPERTIES VERSION "${VLVERSION_MAJOR}.{VLVERSION_MINOR}")

install(TARGETS ${VLVOLUME_NAME}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

install(FILES ${VLVOLUME_INC}
  DESTINATION include/vlVolume
)
