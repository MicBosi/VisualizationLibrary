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
# FreeType Library
################################################################################

set(FT2_DIR "${3RDPARTY_DIR}/freetype")
set(FT2_SRC "${FT2_DIR}/src")

# Same list as upstream's BASE_SRCS, with the portable ANSI ftsystem.c/ftdebug.c.
# Modules are registered by the stock include/freetype/config/ftmodule.h.
# Optional dependencies (bzip2, libpng, HarfBuzz, Brotli) are disabled in
# the stock ftoption.h; gzip support uses FreeType's internal zlib copy.
add_library(FreeType STATIC
	"${FT2_SRC}/autofit/autofit.c"
	"${FT2_SRC}/base/ftbase.c"
	"${FT2_SRC}/base/ftbbox.c"
	"${FT2_SRC}/base/ftbdf.c"
	"${FT2_SRC}/base/ftbitmap.c"
	"${FT2_SRC}/base/ftcid.c"
	"${FT2_SRC}/base/ftfstype.c"
	"${FT2_SRC}/base/ftgasp.c"
	"${FT2_SRC}/base/ftglyph.c"
	"${FT2_SRC}/base/ftgxval.c"
	"${FT2_SRC}/base/ftinit.c"
	"${FT2_SRC}/base/ftmm.c"
	"${FT2_SRC}/base/ftotval.c"
	"${FT2_SRC}/base/ftpatent.c"
	"${FT2_SRC}/base/ftpfr.c"
	"${FT2_SRC}/base/ftstroke.c"
	"${FT2_SRC}/base/ftsynth.c"
	"${FT2_SRC}/base/fttype1.c"
	"${FT2_SRC}/base/ftwinfnt.c"
	"${FT2_SRC}/base/ftsystem.c"
	"${FT2_SRC}/base/ftdebug.c"
	"${FT2_SRC}/bdf/bdf.c"
	"${FT2_SRC}/bzip2/ftbzip2.c"
	"${FT2_SRC}/cache/ftcache.c"
	"${FT2_SRC}/cff/cff.c"
	"${FT2_SRC}/cid/type1cid.c"
	"${FT2_SRC}/gzip/ftgzip.c"
	"${FT2_SRC}/hvf/hvf.c"
	"${FT2_SRC}/lzw/ftlzw.c"
	"${FT2_SRC}/pcf/pcf.c"
	"${FT2_SRC}/pfr/pfr.c"
	"${FT2_SRC}/psaux/psaux.c"
	"${FT2_SRC}/pshinter/pshinter.c"
	"${FT2_SRC}/psnames/psnames.c"
	"${FT2_SRC}/raster/raster.c"
	"${FT2_SRC}/sdf/sdf.c"
	"${FT2_SRC}/sfnt/sfnt.c"
	"${FT2_SRC}/smooth/smooth.c"
	"${FT2_SRC}/svg/svg.c"
	"${FT2_SRC}/truetype/truetype.c"
	"${FT2_SRC}/type1/type1.c"
	"${FT2_SRC}/type42/type42.c"
	"${FT2_SRC}/winfonts/winfnt.c"
)

target_compile_definitions(FreeType PRIVATE FT2_BUILD_LIBRARY)
target_include_directories(FreeType PUBLIC "${FT2_DIR}/include")

VL_THIRDPARTY_TARGET(FreeType)

VL_PROJECT_ADD(VLGRAPHICS INCLUDE_DIRECTORIES "${FT2_DIR}/include" LINK_LIBRARIES FreeType)
