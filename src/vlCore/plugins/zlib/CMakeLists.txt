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
# ZLib Library - required by VLCore and libtiff
################################################################################

set(ZLIB_DIR "${3RDPARTY_DIR}/zlib")

add_library(ZLib STATIC
    "${ZLIB_DIR}/adler32.c"
    "${ZLIB_DIR}/compress.c"
    "${ZLIB_DIR}/crc32.c"
    "${ZLIB_DIR}/deflate.c"
    "${ZLIB_DIR}/gzclose.c"
    "${ZLIB_DIR}/gzlib.c"
    "${ZLIB_DIR}/gzread.c"
    "${ZLIB_DIR}/gzwrite.c"
    "${ZLIB_DIR}/inflate.c"
    "${ZLIB_DIR}/infback.c"
    "${ZLIB_DIR}/inftrees.c"
    "${ZLIB_DIR}/inffast.c"
    "${ZLIB_DIR}/trees.c"
    "${ZLIB_DIR}/uncompr.c"
    "${ZLIB_DIR}/zutil.c"
)

# These change zconf.h (e.g. z_off_t) so every user of zlib.h must see them
set(ZLIB_DEFINITIONS "")
set(ZLIB_DEFINITIONS_FLAGS "")
foreach(_def UNISTD_H STDARG_H)
	if(VL_HAVE_${_def})
		list(APPEND ZLIB_DEFINITIONS HAVE_${_def})
		list(APPEND ZLIB_DEFINITIONS_FLAGS -DHAVE_${_def})
	endif()
endforeach()
target_compile_definitions(ZLib PUBLIC ${ZLIB_DEFINITIONS})
target_include_directories(ZLib PUBLIC "${ZLIB_DIR}")

VL_THIRDPARTY_TARGET(ZLib)

VL_PROJECT_ADD(VLCORE DEFINITIONS ${ZLIB_DEFINITIONS_FLAGS} INCLUDE_DIRECTORIES "${ZLIB_DIR}" LINK_LIBRARIES ZLib)
