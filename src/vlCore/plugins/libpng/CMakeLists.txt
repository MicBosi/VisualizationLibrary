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
# PNG Library
################################################################################

set(PNG_DIR "${3RDPARTY_DIR}/libpng")

add_library(PNG STATIC
	"${PNG_DIR}/png.c"
	"${PNG_DIR}/pngerror.c"
	"${PNG_DIR}/pngget.c"
	"${PNG_DIR}/pngmem.c"
	"${PNG_DIR}/pngpread.c"
	"${PNG_DIR}/pngread.c"
	"${PNG_DIR}/pngrio.c"
	"${PNG_DIR}/pngrtran.c"
	"${PNG_DIR}/pngrutil.c"
	"${PNG_DIR}/pngset.c"
	"${PNG_DIR}/pngtrans.c"
	"${PNG_DIR}/pngwio.c"
	"${PNG_DIR}/pngwrite.c"
	"${PNG_DIR}/pngwtran.c"
	"${PNG_DIR}/pngwutil.c"
)

# Plain C code paths on every architecture: the SIMD filters need extra
# per-architecture sources (arm/, intel/, mips/, powerpc/, loongarch/, riscv/)
target_compile_definitions(PNG PRIVATE
	PNG_ARM_NEON_OPT=0
	PNG_INTEL_SSE_OPT=0
	PNG_MIPS_MSA_OPT=0
	PNG_MIPS_MMI_OPT=0
	PNG_POWERPC_VSX_OPT=0
	PNG_LOONGARCH_LSX_OPT=0
	PNG_RISCV_RVV_OPT=0
)
target_link_libraries(PNG PRIVATE ZLib)

VL_THIRDPARTY_TARGET(PNG)

VL_PROJECT_ADD(VLCORE INCLUDE_DIRECTORIES "${PNG_DIR}" LINK_LIBRARIES PNG)
