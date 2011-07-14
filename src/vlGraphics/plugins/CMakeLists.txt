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

# List of "3D IO" plugins
set(VLGRAPHICS_PLUGINS "3DS" "AC3D" "MD2" "OBJ" "PLY" "STL")
VL_PROCESS_PROJECT_PLUGINS(VLGRAPHICS VL_IO_3D_ ${VLGRAPHICS_PLUGINS})
