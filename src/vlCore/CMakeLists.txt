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
# VLCore Library
################################################################################

project(VLCORE)

# Handle compile-time plugins for VLCore
add_subdirectory("plugins")

# Gather VLCore source files
file(GLOB VLCORE_SRC "*.cpp")
file(GLOB VLCORE_INC "*.hpp")

# Generate the 'vlCore/config.hpp' file
configure_file("config.hpp.in" "${CMAKE_CURRENT_BINARY_DIR}/config.hpp")
list(APPEND VLCORE_INC "${CMAKE_CURRENT_BINARY_DIR}/config.hpp")

# Handle extras added by plugins
VL_PROJECT_GET(VLCORE _SOURCES _DEFINITIONS _INCLUDE_DIRS _EXTRA_LIBS_D _EXTRA_LIBS_R)
add_definitions(${_DEFINITIONS})
include_directories(${_INCLUDE_DIRS})

add_library(VLCore ${VL_SHARED_OR_STATIC} ${VLCORE_SRC} ${VLCORE_INC} ${_SOURCES})
VL_DEFAULT_TARGET_PROPERTIES(VLCore)

# We need to link them one by one because the 'debug' and 'optimized' tags have to be specifed before every library name
foreach(libName ${_EXTRA_LIBS_D})
	target_link_libraries(VLCore debug ${libName})
endforeach()

foreach(libName ${_EXTRA_LIBS_R})
	target_link_libraries(VLCore optimized ${libName})
endforeach()

################################################################################
# Source Groups
################################################################################

source_group("plugins" FILES ${_SOURCES})

################################################################################
# Install Rules
################################################################################

VL_INSTALL_TARGET(VLCore)

# VLCore headers
install(FILES ${VLCORE_INC} DESTINATION "${VL_INCLUDE_INSTALL_DIR}/vlCore")
