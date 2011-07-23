################################################################################
#                                                                              #
#  Copyright (c) 2005-2011, Michele Bosi, Thiago Bastos                        #
#  All rights reserved.                                                        #
#                                                                              #
#  This file is part of Visualization Library                                  #
#  http://www.visualizationlibrary.org                                         #
#                                                                              #
#  Released under the OSI approved Simplified BSD License                      #
#  http://www.opensource.org/licenses/bsd-license.php                          #
#                                                                              #
################################################################################

################################################################################
# Compile-time Plugins for VLGraphics
################################################################################

# VLGraphics requires FreeType (not really a plugin, it's here for convenience)
add_subdirectory("freetype")

# All plugins are enabled by default, except for COLLADA
set(VL_IO_3D_COLLADA_OPTION "Enable COLLADA support (requires COLLADA DOM)" OFF)

# List of "3D IO" plugins
set(VLGRAPHICS_PLUGINS "3DS" "AC3D" "MD2" "OBJ" "PLY" "STL" "COLLADA")
VL_PROCESS_PROJECT_PLUGINS(VLGRAPHICS VL_IO_3D_ ${VLGRAPHICS_PLUGINS})

################################################################################
# Add external/3rd-party dependencies as necessary
################################################################################

if(VL_IO_3D_COLLADA)
	add_subdirectory("collada_dom")
endif()
