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
# 2D/3D IO Plugins (we need these to be visible from VLMain)
################################################################################

set(VLCORE_PLUGINS "BMP" "DAT" "MHD" "DDS" "DICOM" "JPG" "PNG" "TGA" "TIFF")
set(VLGRAPHICS_PLUGINS "3DS" "AC3D" "MD2" "OBJ" "PLY" "STL")

set(VL_IO_2D_DICOM OFF CACHE BOOL "Enable DICOM support (requires GDCM)")

foreach(pluginName ${VLCORE_PLUGINS})
  set(prefixedName VL_IO_2D_${pluginName})
  if(NOT DEFINED ${prefixedName})
    set(${prefixedName} ON CACHE BOOL "Enable ${pluginName} support")
  endif()
endforeach()

foreach(pluginName ${VLGRAPHICS_PLUGINS})
  set(prefixedName VL_IO_3D_${pluginName})
  if(NOT DEFINED ${prefixedName})
    set(${prefixedName} ON CACHE BOOL "Enable ${pluginName} support")
  endif()
endforeach()

################################################################################
# Modules
################################################################################

# VLCore
add_subdirectory("vlCore")

# VLGraphics
add_subdirectory("vlGraphics")

# VLX
add_subdirectory("vlX")

# VLVG
add_subdirectory("vlVG")

# VLMolecule
add_subdirectory("vlMolecule")

# VLVolume
add_subdirectory("vlVolume")

# tools
add_subdirectory("tools")

# VLMain
add_subdirectory("vlMain")

################################################################################
# GUI Modules (automatically enabled when their requirements are met)
################################################################################

# GLUT
option(VL_GUI_GLUT_SUPPORT "Build GLUT support" OFF)
if(VL_GUI_GLUT_SUPPORT)
  set(GLUT_ROOT_PATH "${3RDPARTY_DIR}")
  find_package(GLUT REQUIRED)
  add_subdirectory("gui/vlGLUT")
endif()

# GLFW
option(VL_GUI_GLFW_SUPPORT "Build GLFW support" OFF)
if(VL_GUI_GLFW_SUPPORT)
  # TODO: is there a better and portable method?
  set(GLFW_INCLUDE_DIR "" CACHE PATH "Path to directory containing 'GLFW' include dir")
  set(GLFW_glfw3dll_LIB "" CACHE FILEPATH "Path to GLFW library (must be compiled in DLL/PIC mode)")
  add_definitions(-DGLFW_DLL)
  add_subdirectory("gui/vlGLFW")
endif()

# Qt4
option(VL_GUI_QT4_SUPPORT "Build Qt4 support" OFF)
if(VL_GUI_QT4_SUPPORT)
	find_package(Qt4 COMPONENTS QtCore QtGui QtOpenGL QtMain REQUIRED)
	include("${QT_USE_FILE}")
	add_subdirectory("gui/vlQt4")
endif()

# Qt5
option(VL_GUI_QT5_SUPPORT "Build Qt5 support" OFF)
if(VL_GUI_QT5_SUPPORT)
	# Instruct CMake to run moc automatically when needed.
	set(CMAKE_AUTOMOC ON)
	find_package(Qt5Widgets)
	find_package(Qt5OpenGL)
	add_subdirectory("gui/vlQt5")
endif()

# Qt6
option(VL_GUI_QT6_SUPPORT "Build Qt6 support" OFF)
if(VL_GUI_QT6_SUPPORT)
	# Instruct CMake to run moc automatically when needed.
	set(CMAKE_AUTOMOC ON)
	find_package(Qt6 COMPONENTS OpenGLWidgets REQUIRED)
	add_subdirectory("gui/vlQt6")
endif()

if(VL_PLATFORM_WINDOWS)
	option(VL_GUI_WIN32_SUPPORT "Build Win32 support" OFF)
endif()

if(MSVC)
	option(VL_GUI_MFC_SUPPORT "Build MFC support" OFF)
endif()

# EGL bindings are available only under Windows right now
if(VL_PLATFORM_WINDOWS AND (VL_OPENGL_ES1 OR VL_OPENGL_ES2))
	option(VL_GUI_EGL_SUPPORT "Build EGL support" ON)
endif()

if(VL_GUI_MFC_SUPPORT)
  add_subdirectory("gui/vlMFC")
endif()

if(VL_GUI_WIN32_SUPPORT)
	add_subdirectory("gui/vlWin32")
endif()

if(VL_GUI_EGL_SUPPORT)
	add_subdirectory("gui/vlEGL")
endif()

# SDL
option(VL_GUI_SDL_SUPPORT "Build SDL support" OFF)

if(VL_GUI_SDL_SUPPORT)
	find_package(SDL REQUIRED)
	add_subdirectory("gui/vlSDL")
endif()

# SDL2
option(VL_GUI_SDL2_SUPPORT "Build SDL2 support" OFF)

if(VL_GUI_SDL2_SUPPORT)
	find_package(SDL2 REQUIRED)
	add_subdirectory("gui/vlSDL2")
endif()

# wxWidgets
option(VL_GUI_WXWIDGETS_SUPPORT "Build wxWidgets support" OFF)
if(VL_GUI_WXWIDGETS_SUPPORT)
	find_package(wxWidgets COMPONENTS gl core base REQUIRED)
	if (wxWidgets_FOUND)
		add_subdirectory("gui/vlWX")
	endif()
endif()

# Cocoa
#if(APPLE)
#	option(VL_GUI_COCOA_SUPPORT "Build Cocoa support" ON)
#endif()
#if(VL_GUI_COCOA_SUPPORT)
#	add_subdirectory("gui/vlCocoa")
#endif()

################################################################################
# Examples
################################################################################

cmake_dependent_option(VL_GUI_GLUT_EXAMPLES "Build GLUT examples" ON "VL_GUI_GLUT_SUPPORT" OFF)
cmake_dependent_option(VL_GUI_GLFW_EXAMPLES "Build GLFW examples" ON "VL_GUI_GLFW_SUPPORT" OFF)
cmake_dependent_option(VL_GUI_QT4_EXAMPLES "Build Qt4 examples" ON "VL_GUI_QT4_SUPPORT" OFF)
cmake_dependent_option(VL_GUI_QT5_EXAMPLES "Build Qt5 examples" ON "VL_GUI_QT5_SUPPORT" OFF)
cmake_dependent_option(VL_GUI_QT6_EXAMPLES "Build Qt6 examples" ON "VL_GUI_QT6_SUPPORT" OFF)
cmake_dependent_option(VL_GUI_MFC_EXAMPLES "Build MFC examples" ON "VL_GUI_MFC_SUPPORT" OFF)
cmake_dependent_option(VL_GUI_WIN32_EXAMPLES "Build win32 examples" ON "VL_GUI_WIN32_SUPPORT" OFF)
cmake_dependent_option(VL_GUI_SDL_EXAMPLES "Build SDL examples" ON "VL_GUI_SDL_SUPPORT" OFF)
cmake_dependent_option(VL_GUI_SDL2_EXAMPLES "Build SDL2 examples" ON "VL_GUI_SDL2_SUPPORT" OFF)
cmake_dependent_option(VL_GUI_WXWIDGETS_EXAMPLES "Build wxWidgets examples" ON "VL_GUI_WXWIDGETS_SUPPORT" OFF)
cmake_dependent_option(VL_GUI_GLES_EXAMPLES "Build OpenGL ES examples" ON "VL_GUI_EGL_SUPPORT" OFF)
#cmake_dependent_option(VL_GUI_COCOA_EXAMPLES "Build Cocoa examples" ON "VL_GUI_COCOA_SUPPORT" OFF)

add_subdirectory("examples")
