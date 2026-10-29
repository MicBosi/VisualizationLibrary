################################################################################
# Helpers shared by the bundled 3rd-party libraries (src/external)
################################################################################

include(CheckIncludeFile)
include(CheckTypeSize)
include(TestBigEndian)

check_include_file("unistd.h"    VL_HAVE_UNISTD_H)
check_include_file("io.h"        VL_HAVE_IO_H)
check_include_file("search.h"    VL_HAVE_SEARCH_H)
check_include_file("fcntl.h"     VL_HAVE_FCNTL_H)
check_include_file("sys/types.h" VL_HAVE_SYS_TYPES_H)
check_include_file("stdint.h"    VL_HAVE_STDINT_H)
check_include_file("stdarg.h"    VL_HAVE_STDARG_H)
check_include_file("strings.h"   VL_HAVE_STRINGS_H)
check_include_file("assert.h"    VL_HAVE_ASSERT_H)

check_type_size("size_t"        VL_SIZEOF_SIZE_T)
check_type_size("unsigned long" VL_SIZEOF_UNSIGNED_LONG)

test_big_endian(VL_BIG_ENDIAN)

#-------------------------------------------------------------------------------
# VL_THIRDPARTY_TARGET(target)
# Applies the common settings to a bundled 3rd-party static library:
# PIC when building shared VL libraries, CRT warnings off on MSVC and
# compiler warnings silenced (we don't maintain this code).
#-------------------------------------------------------------------------------
function(VL_THIRDPARTY_TARGET target)
	if(VL_DYNAMIC_LINKING)
		set_target_properties(${target} PROPERTIES POSITION_INDEPENDENT_CODE ON)
	endif()
	if(MSVC)
		target_compile_definitions(${target} PRIVATE _CRT_SECURE_NO_WARNINGS _CRT_NONSTDC_NO_DEPRECATE)
		target_compile_options(${target} PRIVATE /W0)
	else()
		target_compile_options(${target} PRIVATE -w)
	endif()
	if(NOT VL_DYNAMIC_LINKING)
		VL_INSTALL_TARGET(${target})
	endif()
endfunction()
