#############################################################################
#                                                                           #
#  Visualization Library                                                    #
#  http://www.visualizationlibrary.com                                      #
#                                                                           #
#  Copyright 2005-2009 Michele Bosi, all rights are reserved                #
#                                                                           #
#  This file is part of Visualization Library.                              #
#  It may only be used, modified, and distributed under the terms of the    #
#  license specified in the LICENSE.TXT file. By continuing to use,         #
#  modify or distribute this file you indicate that you have read the       #
#  license and understand and accept it fully.                              #
#                                                                           #
#  Visualization Library is distributed in the hope that it will be useful, #
#  but WITHOUT ANY WARRANTY; without even the implied warranty of           #
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            #
#  LICENSE.TXT file for more details.                                       #
#                                                                           #
#############################################################################

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(${VLPNG_NAME})

include_directories("../zlib")
add_definitions(-DPNG_NO_MMX_CODE -D_CRT_SECURE_NO_WARNINGS)

add_library(${VLPNG_NAME} STATIC 
	png.c
	pngerror.c
	pngget.c
	pngmem.c
	pngpread.c
	pngread.c
	pngrio.c
	pngrtran.c
	pngrutil.c
	pngset.c
	pngtrans.c
	pngwio.c
	pngwrite.c
	pngwtran.c
	pngwutil.c
)
set_target_properties(${VLPNG_NAME} PROPERTIES DEBUG_POSTFIX "-d")

install(TARGETS ${VLPNG_NAME}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
