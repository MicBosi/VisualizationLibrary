######################################################################################
#                                                                                    #
#  Visualization Library                                                             #
#  http://www.visualizationlibrary.com                                               #
#                                                                                    #
#  Copyright (c) 2005-2010, Michele Bosi                                             #
#  All rights reserved.                                                              #
#                                                                                    #
#  Redistribution and use in source and binary forms, with or without modification,  #
#  are permitted provided that the following conditions are met:                     #
#                                                                                    #
#  - Redistributions of source code must retain the above copyright notice, this     #
#  list of conditions and the following disclaimer.                                  #
#                                                                                    #
#  - Redistributions in binary form must reproduce the above copyright notice, this  #
#  list of conditions and the following disclaimer in the documentation and/or       #
#  other materials provided with the distribution.                                   #
#                                                                                    #
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND   #
#  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED     #
#  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE            #
#  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR  #
#  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES    #
#  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      #
#  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON    #
#  ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT           #
#  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS     #
#  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                      #
#                                                                                    #
######################################################################################

cmake_minimum_required(VERSION 2.6)

set(VLVERSION_MAJOR 2010)
set(VLVERSION_MINOR 12)
set(VLVERSION_PATCH 1129)
set(VLVERSION ${VLVERSION_MAJOR}.${VLVERSION_MINOR})

project(Visualization_Library_SDK-${VLVERSION_MAJOR}.${VLVERSION_MINOR})

# high warning level
if(MSVC)
	set(CMAKE_CXX_FLAGS "/W4 /EHsc")
else()
	set(CMAKE_CXX_FLAGS "-W -Wall") # see also: -W -Wall -Wwrite-strings -Wcast-qual -Wconversion -Wshadow
endif()
# high verbosity
set(CMAKE_VERBOSE_MAKEFILE ON)

if(WIN32)
  add_definitions(-DUNICODE)
endif()

include_directories("src")
include_directories("3rdparty")
include_directories("3rdparty/glew/include")
include_directories("3rdparty/zlib")
include_directories("3rdparty/freetype/include")

set(VLCORE_NAME "VLCore")
set(VLGRAPHICS_NAME "VLGraphics")
#set(VLGEOMETRY_NAME "VLGeometry")

set(VLMOLECULE_NAME "VLMolecule")
set(VLVOLUME_NAME "VLVolume")
set(VLVG_NAME "VLVG")

set(VLAPPLETS_NAME "Applets")
set(VLWIN32_NAME "VLWin32")
set(VLGLUT_NAME "VLGLUT")
set(VLSDL_NAME "VLSDL")
set(VLQT4_NAME "VLQt4")
#set(VLCOCOA_NAME "VLCocoa")
set(VLMFC_NAME "VLMFC")
set(VLWX_NAME "VLWX")
set(VLZLIB_NAME "ZLib")
set(VLFREETYPE_NAME "FreeType")
set(VLJPG_NAME "JPG")
set(VLPNG_NAME "PNG")
set(VLTIFF_NAME "TIFF")

if(MSVC)
  option(GUI_WIN32_SUPPORT "Build Win32 support" ON)
  option(GUI_MFC_SUPPORT "Build MFC support" OFF)
endif()
if(MINGW)
  option(GUI_WIN32_SUPPORT "Build Win32 support" ON)
  option(GUI_MFC_SUPPORT "Build MFC support" OFF)
endif()
option(GUI_SDL_SUPPORT "Build SDL support" OFF)
option(GUI_GLUT_SUPPORT "Build GLUT support" OFF)
option(GUI_QT4_SUPPORT "Build QT4 support" OFF)
option(GUI_WXWIDGETS_SUPPORT "Build wxWidgets support" OFF)
#if(${CMAKE_SYSTEM} MATCHES "Darwin")
#  option(GUI_COCOA_SUPPORT "Build Cocoa support" ON)
#endif()

if(MSVC)
	set(WINVER "0x0600" CACHE STRING "WINVER version (see MSDN documentation)")
	add_definitions(-DWINVER=${WINVER})
	add_definitions(-D_WIN32_WINNT=${WINVER})
endif()

find_package(OpenGL REQUIRED)

option(IO_MODULE_JPG "Enable JPEG support" ON)
if(IO_MODULE_JPG)
	add_definitions(-DIO_MODULE_JPG)
	set(VL_MODULE_LIBRARIES ${VL_MODULE_LIBRARIES} ${VLJPG_NAME})
	include_directories("3rdparty/jpeg-6b")
	add_subdirectory("3rdparty/jpeg-6b")
endif()

option(IO_MODULE_PNG "Enable PNG support" ON)
if(IO_MODULE_PNG)
	add_definitions(-DIO_MODULE_PNG)
	set(VL_MODULE_LIBRARIES ${VL_MODULE_LIBRARIES} ${VLPNG_NAME})
	include_directories("3rdparty/libpng")
	add_subdirectory("3rdparty/libpng")
endif()

option(IO_MODULE_TIFF "Enable TIFF support" ON)
if(IO_MODULE_TIFF)
	add_definitions(-DIO_MODULE_TIFF)
	set(VL_MODULE_LIBRARIES ${VL_MODULE_LIBRARIES} ${VLTIFF_NAME})
	include_directories("3rdparty/libtiff")
	add_subdirectory("3rdparty/libtiff")
endif()

option(IO_MODULE_DICOM "Enable DICOM support (requires dcmtk)" OFF)
if(IO_MODULE_DICOM)
	add_definitions(-DIO_MODULE_DICOM)
  set(IO_MODULE_GDCM_LIB_DIR "FILLME" CACHE PATH "GDCM library directory")
	link_directories(${IO_MODULE_GDCM_LIB_DIR})
  set(IO_MODULE_GDCM_LIBRARIES "FILLME" CACHE STRING "GDCM libraries (space separated, needed only when compiling the examples)")
	set(VL_MODULE_LIBRARIES ${VL_MODULE_LIBRARIES} ${IO_MODULE_GDCM_LIBRARIES})
  set(IO_MODULE_GDCM_INC_DIR "FILLME" CACHE PATH "GDCM include directory")
	include_directories(${IO_MODULE_GDCM_INC_DIR})
endif()

option(IO_MODULE_MD2 "Enable MD2 support" ON)
if(IO_MODULE_MD2)
  add_definitions(-DIO_MODULE_MD2)
endif()

option(IO_MODULE_PLY "Enable PLY support" ON)
if(IO_MODULE_PLY)
  add_definitions(-DIO_MODULE_PLY)
endif()

option(IO_MODULE_STL "Enable STL support" ON)
if(IO_MODULE_STL)
  add_definitions(-DIO_MODULE_STL)
endif()

option(IO_MODULE_AC3D "Enable AC3D support" ON)
if(IO_MODULE_AC3D)
  add_definitions(-DIO_MODULE_AC3D)
endif()

option(IO_MODULE_OBJ "Enable OBJ support" ON)
if(IO_MODULE_OBJ)
  add_definitions(-DIO_MODULE_OBJ)
endif()

option(IO_MODULE_3DS "Enable 3DS support" ON)
if(IO_MODULE_3DS)
  add_definitions(-DIO_MODULE_3DS)
endif()

option(IO_MODULE_DAT "Enable DAT support" ON)
if(IO_MODULE_DAT)
  add_definitions(-DIO_MODULE_DAT)
endif()

option(IO_MODULE_BMP "Enable BMP support" ON)
if(IO_MODULE_BMP)
  add_definitions(-DIO_MODULE_BMP)
endif()

option(IO_MODULE_TGA "Enable TGA support" ON)
if(IO_MODULE_TGA)
  add_definitions(-DIO_MODULE_TGA)
endif()

option(IO_MODULE_DDS "Enable DDS support" ON)
if(IO_MODULE_DDS)
  add_definitions(-DIO_MODULE_DDS)
endif()

option(IO_MODULE_TIFF "Enable TIFF support" ON)
if(IO_MODULE_TIFF)
  add_definitions(-DIO_MODULE_TIFF)
endif()

#VLCore
add_subdirectory("src/vlCore")

#VLGraphics
add_subdirectory("src/vlGraphics")

#VLVG
option(VL_MODULE_VLVG "Enable VL Vector Graphics Library" ON)
if(VL_MODULE_VLVG)
	add_subdirectory("src/vlVG")
endif()

#VLMOLECULE
option(VL_MODULE_MOLECULE "Enable VL Molecule Library" ON)
if(VL_MODULE_MOLECULE)
	add_subdirectory("src/vlMolecule")
endif()

#VLVOLUME
option(VL_MODULE_VOLUME "Enable VL Volume Library" ON)
if(VL_MODULE_VOLUME)
	add_subdirectory("src/vlVolume")
endif()

add_subdirectory("3rdparty/zlib")
add_subdirectory("3rdparty/freetype")

if(GUI_MFC_SUPPORT)
  option(GUI_MFC_EXAMPLES "Build MFC examples" ON)
  add_subdirectory("src/vlMFC")
endif()

if(GUI_WIN32_SUPPORT)
  option(GUI_WIN32_EXAMPLES "Build win32 examples" ON)
  add_subdirectory("src/vlWin32")
endif()

if(GUI_SDL_SUPPORT)
  option(GUI_SDL_EXAMPLES "Build SDL examples" ON)
  if(MSVC)
    # Visual Studio
    set(SDL_INCLUDE_DIR  "${VisualizationLibrarySDK_SOURCE_DIR}/3rdparty/include/SDL"    CACHE PATH "Location of the SDL.h include file")
    set(SDLMAIN_LIBRARY "${VisualizationLibrarySDK_SOURCE_DIR}/3rdparty/lib/SDLmain.lib" CACHE FILEPATH "SDLmain library path")
    set(SDL_LIBRARY      "${VisualizationLibrarySDK_SOURCE_DIR}/3rdparty/lib/SDL.lib"    CACHE FILEPATH "SDL library path")
  else()
    if(MINGW)
      # MinGW
      set(SDL_INCLUDE_DIR  "C:/msys/1.0/local/include/SDL"  CACHE PATH "Location of the SDL.h include file")
      set(SDLMAIN_LIBRARY  "C:/msys/1.0/local/lib/libSDLmain.a" CACHE FILEPATH "SDL library path")
      set(SDL_LIBRARY      "C:/msys/1.0/local/lib/libSDL.a" CACHE FILEPATH "SDL library path")
    else()
      # Linux / Mac
      find_package(SDL REQUIRED)
    endif()
  endif()
  add_subdirectory("src/vlSDL")
endif()

if(GUI_GLUT_SUPPORT)
  option(GUI_GLUT_EXAMPLES "Build GLUT examples" ON)
  if(WIN32)
    set(GLUT_INCLUDE_DIR  "${VisualizationLibrarySDK_SOURCE_DIR}/3rdparty/include" CACHE PATH "Location of the GLUT include directory")
    set(GLUT_glut_LIBRARY "${VisualizationLibrarySDK_SOURCE_DIR}/3rdparty/lib/freeglut.lib" CACHE FILEPATH "GLUT library path")
  else(WIN32)
    find_package(GLUT REQUIRED)
  endif()
  add_subdirectory("src/vlGLUT")
endif()

if(GUI_WXWIDGETS_SUPPORT)
  option(GUI_WXWIDGETS_EXAMPLES "Build wxWidgets examples" OFF)
  set(WXWIDGETS_INCLUDE_DIR "FILLME" CACHE PATH "wxWidgets include directories")
  add_subdirectory("src/vlWX")
endif()

if(GUI_WXWIDGETS_EXAMPLES)
  set(WXWIDGETS_LIBRARY_DIR "FILLME" CACHE PATH "wxWidgets libraries path")
  set(WXWIDGETS_LIBRARIES   "FILLME" CACHE STRING "wxWidgets core and base libraries (space separated)")
endif()

if(GUI_QT4_SUPPORT)
  option(GUI_QT4_EXAMPLES "Build Qt4 examples" ON)
  find_package(Qt4 REQUIRED)
  SET(QT_USE_QTCORE 1)
  SET(QT_USE_QTGUI 1)
  #SET(QT_USE_QT3SUPPORT 1)
  #SET(QT_USE_QTASSISTANT 1)
  #SET(QT_USE_QTDESIGNER 1)
  #SET(QT_USE_QTMOTIF 1)
  SET(QT_USE_QTMAIN 1)
  #SET(QT_USE_QTNETWORK 1)
  #SET(QT_USE_QTNSPLUGIN 1)
  SET(QT_USE_QTOPENGL 1)
  #SET(QT_USE_QTSQL 1)
  #SET(QT_USE_QTXML 1)
  #SET(QT_USE_QTSVG 1)
  #SET(QT_USE_QTTEST 1)
  #SET(QT_USE_QTUITOOLS 1)
  #SET(QT_USE_QTDBUS 1)
  #SET(QT_USE_QTSCRIPT 1)
  #SET(QT_USE_QTASSISTANTCLIENT 1)
  #SET(QT_USE_QTHELP 1)
  #SET(QT_USE_QTWEBKIT 1)
  #SET(QT_USE_QTXMLPATTERNS 1)
  #SET(QT_USE_PHONON 1)
  include(${QT_USE_FILE})
  add_subdirectory("src/vlQt4")
endif()

#if(GUI_COCOA_SUPPORT)
#  option(GUI_COCOA_EXAMPLES "Build Cocoa examples" ON)
#  add_subdirectory("src/vlCocoa")
#endif()

add_subdirectory("src/examples")

add_subdirectory("docs")
add_subdirectory("src/examples/Applets")

#install(FILES "bin/Win32_Tests.bat" DESTINATION bin)
#install(DIRECTORY "docs" DESTINATION ".")
install(FILES "LICENSE.TXT" "README.TXT" DESTINATION ".")
install(FILES "3rdparty/glew/include/GL/glew.h" "3rdparty/glew/include/GL/glxew.h" "3rdparty/glew/include/GL/wglew.h" DESTINATION "include/GL/" )

