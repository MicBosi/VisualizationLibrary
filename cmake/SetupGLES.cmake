# mic fixme
# search IMG and AMD library and include paths automatically

option(VL_GLES1_EXAMPLES "Build GLES 1.x examples" ON)

# set(VL_GLES_INCLUDE_DIR "NOTFOUND" CACHE STRING "Path to GLES include directories")
set(VL_GLES_INCLUDE_DIR "C:/Imagination Technologies/POWERVR SDK/OGLES/Builds/OGLES/Include" CACHE PATH "Path to GLES include directories")

# set(VL_GLES_LIB_DIR "NOTFOUND" CACHE STRING "Path to directory containing GLES and EGL libraries")
set(VL_GLES_LIB_DIR "C:/Imagination Technologies/POWERVR SDK/OGLES/Builds/OGLES/WindowsX86/Lib" CACHE PATH "Path to directory containing GLES and EGL libraries")

set(VL_GLES_LIBRARIES "libEGL.lib;libgles_cm.lib" CACHE STRING "GLES and EGL library names separated by ;")

if (VL_GLES_INCLUDE_DIR)
	include_directories(${VL_GLES_INCLUDE_DIR})
endif()

if (VL_GLES_LIB_DIR)
	link_directories(${VL_GLES_LIB_DIR})
endif()
