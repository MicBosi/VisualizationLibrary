################################################################################
#                                                                              #
#  Copyright (c) 2005-2020, Michele Bosi, Thiago Bastos                        #
#  All rights reserved.                                                        #
#                                                                              #
#  This file is part of Visualization Library                                  #
#  http://visualizationlibrary.org                                             #
#                                                                              #
#  Released under the OSI approved Simplified BSD License                      #
#  http://www.opensource.org/licenses/bsd-license.php                          #
#                                                                              #
################################################################################

################################################################################
# VLX Library
################################################################################

project(VLX)

# Gather VLX source files
file(GLOB VLX_SRC "*.cpp")
file(GLOB VLX_INC "*.hpp")

# Handle extras added by plugins
VL_PROJECT_GET(VLX _SOURCES _DEFINITIONS _INCLUDE_DIRS _EXTRA_LIBS_D _EXTRA_LIBS_R)
add_definitions(${_DEFINITIONS})
include_directories(${_INCLUDE_DIRS})

add_library(VLX ${VL_SHARED_OR_STATIC} ${VLX_SRC} ${VLX_INC} ${_SOURCES})
VL_DEFAULT_TARGET_PROPERTIES(VLX)

target_link_libraries(VLX VLCore VLGraphics ${VL_OPENGL_LIBRARIES})

# We need to link them one by one because the 'debug' and 'optimized' tags have to be specifed before every library name
foreach(libName ${_EXTRA_LIBS_D})
	target_link_libraries(VLX debug ${libName})
endforeach()

foreach(libName ${_EXTRA_LIBS_R})
	target_link_libraries(VLX optimized ${libName})
endforeach()

################################################################################
# Install Rules
################################################################################

VL_INSTALL_TARGET(VLX)

# VLX headers
install(FILES ${VLX_INC} DESTINATION "${VL_INCLUDE_INSTALL_DIR}/vlX")
