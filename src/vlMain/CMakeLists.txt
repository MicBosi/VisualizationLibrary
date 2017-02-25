################################################################################
#                                                                              #
#  Copyright (c) 2005-2011, Michele Bosi, Thiago Bastos                        #
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
# VLMain Library
################################################################################

project(VLMAIN)

# Gather VLMain source files
file(GLOB VLMAIN_SRC "*.cpp")
file(GLOB VLMAIN_INC "*.hpp")

# Handle extras added by plugins
VL_PROJECT_GET(VLMAIN _SOURCES _DEFINITIONS _INCLUDE_DIRS _EXTRA_LIBS_D _EXTRA_LIBS_R)

add_definitions(${_DEFINITIONS})
include_directories(${_INCLUDE_DIRS})

add_library(VLMain ${VL_SHARED_OR_STATIC} ${VLMAIN_SRC} ${VLMAIN_INC} ${KHRONOS_INC} ${_SOURCES})
VL_DEFAULT_TARGET_PROPERTIES(VLMain)

target_link_libraries(VLMain VLCore VLGraphics VLX ${VL_OPENGL_LIBRARIES})

foreach(libName ${_EXTRA_LIBS_D})
	target_link_libraries(VLMain debug ${libName})
endforeach()

foreach(libName ${_EXTRA_LIBS_R})
	target_link_libraries(VLMain optimized ${libName})
endforeach()

################################################################################
# Install Rules
################################################################################

VL_INSTALL_TARGET(VLMain)

# VLMain headers
install(FILES ${VLMAIN_INC} DESTINATION "${VL_INCLUDE_INSTALL_DIR}/vlMain")

################################################################################
# Plug-in Defines
################################################################################

foreach(pluginName ${VLCORE_PLUGINS})
  set(prefixedName VL_IO_2D_${pluginName})
  if(${prefixedName})
    # MESSAGE(STATUS "${prefixedName} = ${${prefixedName}}")
    add_definitions("-D${prefixedName}")
  endif()
endforeach()

foreach(pluginName ${VLGRAPHICS_PLUGINS})
  set(prefixedName VL_IO_3D_${pluginName})
  if(${prefixedName})
    # MESSAGE(STATUS "${prefixedName} = ${${prefixedName}}")
    add_definitions("-D${prefixedName}")
  endif()
endforeach()

