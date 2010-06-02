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

PROJECT(${VLFREETYPE_NAME})

add_definitions(-D_LIB -D_CRT_SECURE_NO_WARNINGS -DFT2_BUILD_LIBRARY)

add_library(${VLFREETYPE_NAME} STATIC 
	src/autofit/autofit.c
	src/bdf/bdf.c
	src/cff/cff.c
	src/base/ftbase.c
	src/base/ftbitmap.c
	src/cache/ftcache.c
	src/base/ftdebug.c
	src/base/ftfstype.c
	src/base/ftgasp.c
	src/base/ftglyph.c
	src/gzip/ftgzip.c
	src/lzw/ftlzw.c
	src/base/ftinit.c
	src/base/ftstroke.c
	src/base/ftsystem.c
	src/smooth/smooth.c
	# FT_MODULES
	src/base/ftbbox.c
	src/base/ftmm.c
	src/base/ftpfr.c
	src/base/ftsynth.c
	src/base/fttype1.c
	src/base/ftwinfnt.c
	src/pcf/pcf.c
	src/pfr/pfr.c
	src/psaux/psaux.c
	src/pshinter/pshinter.c
	src/psnames/psmodule.c
	src/raster/raster.c
	src/sfnt/sfnt.c
	src/truetype/truetype.c
	src/type1/type1.c
	src/cid/type1cid.c
	src/type42/type42.c
	src/winfonts/winfnt.c
)

set_target_properties(${VLFREETYPE_NAME} PROPERTIES DEBUG_POSTFIX "-d")

install(TARGETS ${VLFREETYPE_NAME}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
