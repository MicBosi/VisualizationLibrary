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
set(VLVERSION_MINOR 11)
set(VLVERSION_PATCH 1123)
set(VLVERSION ${VLVERSION_MAJOR}.${VLVERSION_MINOR})

project(Visualization_Library_SDK-${VLVERSION_MAJOR}.${VLVERSION_MINOR})

# high warning level
if(MSVC)
	set(CMAKE_CXX_FLAGS "/W4 /EHsc")
else(MSVC)
	set(CMAKE_CXX_FLAGS "-W -Wall") # see also: -W -Wall -Wwrite-strings -Wcast-qual -Wconversion -Wshadow
endif(MSVC)
# high verbosity
set(CMAKE_VERBOSE_MAKEFILE ON)

if(WIN32)
  add_definitions(-DUNICODE)
endif(WIN32)

include_directories("src")
include_directories("3rdparty")
include_directories("3rdparty/glew/include")
include_directories("3rdparty/zlib")
include_directories("3rdparty/freetype/include")
if(APPLE)
include_directories("/Developer/Headers/FlatCarbon")
endif(APPLE)

set(VISUALIZATIONLIBRARY_NAME "VisualizationLibrary")
set(VLWIN32_NAME "VLWin32")
set(VLGLUT_NAME "VLGLUT")
set(VLSDL_NAME "VLSDL")
set(VLQT4_NAME "VLQt4")
set(VLCOCOA_NAME "VLCocoa")
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
endif(MSVC)
if(MINGW)
  option(GUI_WIN32_SUPPORT "Build Win32 support" ON)
  option(GUI_MFC_SUPPORT "Build MFC support" OFF)
endif(MINGW)
option(GUI_SDL_SUPPORT "Build SDL support" OFF)
option(GUI_GLUT_SUPPORT "Build GLUT support" OFF)
option(GUI_QT4_SUPPORT "Build QT4 support" OFF)
option(GUI_WXWIDGETS_SUPPORT "Build wxWidgets support" OFF)
if(APPLE)
  option(GUI_COCOA_SUPPORT "Build Cocoa support" ON)
endif(APPLE)

if(MSVC)
	set(WINVER "0x0600" CACHE STRING "WINVER version (see MSDN documentation)")
	add_definitions(-DWINVER=${WINVER})
	add_definitions(-D_WIN32_WINNT=${WINVER})
endif(MSVC)

find_package(OpenGL REQUIRED)

option(IO_MODULE_JPG "Enable JPEG support" ON)
if(IO_MODULE_JPG)
	add_definitions(-DIO_MODULE_JPG)
	set(VL_MODULE_LIBRARIES ${VL_MODULE_LIBRARIES} ${VLJPG_NAME})
	include_directories("3rdparty/jpeg-6b")
	add_subdirectory("3rdparty/jpeg-6b")
endif(IO_MODULE_JPG)

option(IO_MODULE_PNG "Enable PNG support" ON)
if(IO_MODULE_PNG)
	add_definitions(-DIO_MODULE_PNG)
	set(VL_MODULE_LIBRARIES ${VL_MODULE_LIBRARIES} ${VLPNG_NAME})
	include_directories("3rdparty/libpng")
	add_subdirectory("3rdparty/libpng")
endif(IO_MODULE_PNG)

option(IO_MODULE_TIFF "Enable TIFF support" ON)
if(IO_MODULE_TIFF)
	add_definitions(-DIO_MODULE_TIFF)
	set(VL_MODULE_LIBRARIES ${VL_MODULE_LIBRARIES} ${VLTIFF_NAME})
	include_directories("3rdparty/libtiff")
	add_subdirectory("3rdparty/libtiff")
endif(IO_MODULE_TIFF)

option(IO_MODULE_DICOM "Enable DICOM support (requires dcmtk)" OFF)
if(IO_MODULE_DICOM)
	add_definitions(-DIO_MODULE_DICOM)
  set(IO_MODULE_GDCM_LIB_DIR "FILLME" CACHE PATH "GDCM library directory")
	link_directories(${IO_MODULE_GDCM_LIB_DIR})
  set(IO_MODULE_GDCM_LIBRARIES "FILLME" CACHE STRING "GDCM libraries (space separated, needed only when compiling the examples)")
	set(VL_MODULE_LIBRARIES ${VL_MODULE_LIBRARIES} ${IO_MODULE_GDCM_LIBRARIES})
  set(IO_MODULE_GDCM_INC_DIR "FILLME" CACHE PATH "GDCM include directory")
	include_directories(${IO_MODULE_GDCM_INC_DIR})
endif(IO_MODULE_DICOM)

option(IO_MODULE_MD2 "Enable MD2 support" ON)
IF(IO_MODULE_MD2)
  add_definitions(-DIO_MODULE_MD2)
endif(IO_MODULE_MD2)

option(IO_MODULE_PLY "Enable PLY support" ON)
IF(IO_MODULE_PLY)
  add_definitions(-DIO_MODULE_PLY)
endif(IO_MODULE_PLY)

option(IO_MODULE_STL "Enable STL support" ON)
IF(IO_MODULE_STL)
  add_definitions(-DIO_MODULE_STL)
endif(IO_MODULE_STL)

option(IO_MODULE_AC3D "Enable AC3D support" ON)
IF(IO_MODULE_AC3D)
  add_definitions(-DIO_MODULE_AC3D)
endif(IO_MODULE_AC3D)

option(IO_MODULE_OBJ "Enable OBJ support" ON)
IF(IO_MODULE_OBJ)
  add_definitions(-DIO_MODULE_OBJ)
endif(IO_MODULE_OBJ)

option(IO_MODULE_3DS "Enable 3DS support" ON)
IF(IO_MODULE_3DS)
  add_definitions(-DIO_MODULE_3DS)
endif(IO_MODULE_3DS)

option(IO_MODULE_DAT "Enable DAT support" ON)
IF(IO_MODULE_DAT)
  add_definitions(-DIO_MODULE_DAT)
endif(IO_MODULE_DAT)

option(IO_MODULE_BMP "Enable BMP support" ON)
IF(IO_MODULE_BMP)
  add_definitions(-DIO_MODULE_BMP)
endif(IO_MODULE_BMP)

option(IO_MODULE_TGA "Enable TGA support" ON)
IF(IO_MODULE_TGA)
  add_definitions(-DIO_MODULE_TGA)
endif(IO_MODULE_TGA)

option(IO_MODULE_DDS "Enable DDS support" ON)
IF(IO_MODULE_DDS)
  add_definitions(-DIO_MODULE_DDS)
endif(IO_MODULE_DDS)

option(IO_MODULE_TIFF "Enable TIFF support" ON)
IF(IO_MODULE_TIFF)
  add_definitions(-DIO_MODULE_TIFF)
endif(IO_MODULE_TIFF)

add_subdirectory("src")
add_subdirectory("3rdparty/zlib")
add_subdirectory("3rdparty/freetype")

if(GUI_MFC_SUPPORT)
  option(GUI_MFC_EXAMPLES "Build MFC examples" ON)
  add_subdirectory("src/vlMFC")
endif(GUI_MFC_SUPPORT)

if(GUI_WIN32_SUPPORT)
  option(GUI_WIN32_EXAMPLES "Build win32 examples" ON)
  add_subdirectory("src/vlWin32")
endif(GUI_WIN32_SUPPORT)

if(GUI_SDL_SUPPORT)
  option(GUI_SDL_EXAMPLES "Build SDL examples" ON)
  if(MSVC)
    # Visual Studio
    set(SDL_INCLUDE_DIR  "${VisualizationLibrarySDK_SOURCE_DIR}/3rdparty/include/SDL"    CACHE PATH "Location of the SDL.h include file")
    set(SDLMAIN_LIBRARY "${VisualizationLibrarySDK_SOURCE_DIR}/3rdparty/lib/SDLmain.lib" CACHE FILEPATH "SDLmain library path")
    set(SDL_LIBRARY      "${VisualizationLibrarySDK_SOURCE_DIR}/3rdparty/lib/SDL.lib"    CACHE FILEPATH "SDL library path")
  else(MSVC)
    if(MINGW)
      # MinGW
      set(SDL_INCLUDE_DIR  "C:/msys/1.0/local/include/SDL"  CACHE PATH "Location of the SDL.h include file")
      set(SDLMAIN_LIBRARY  "C:/msys/1.0/local/lib/libSDLmain.a" CACHE FILEPATH "SDL library path")
      set(SDL_LIBRARY      "C:/msys/1.0/local/lib/libSDL.a" CACHE FILEPATH "SDL library path")
    else(MINGW)
      # Linux / Mac
      find_package(SDL REQUIRED)
    endif(MINGW)
  endif(MSVC)
  add_subdirectory("src/vlSDL")
endif(GUI_SDL_SUPPORT)

if(GUI_GLUT_SUPPORT)
  option(GUI_GLUT_EXAMPLES "Build GLUT examples" ON)
  if(WIN32)
    set(GLUT_INCLUDE_DIR  "${VisualizationLibrarySDK_SOURCE_DIR}/3rdparty/include" CACHE PATH "Location of the GLUT include directory")
    set(GLUT_glut_LIBRARY "${VisualizationLibrarySDK_SOURCE_DIR}/3rdparty/lib/freeglut.lib" CACHE FILEPATH "GLUT library path")
  else(WIN32)
    find_package(GLUT REQUIRED)
  endif(WIN32)
  add_subdirectory("src/vlGLUT")
endif(GUI_GLUT_SUPPORT)

if(GUI_WXWIDGETS_SUPPORT)
  option(GUI_WXWIDGETS_EXAMPLES "Build wxWidgets examples" OFF)
  set(WXWIDGETS_INCLUDE_DIR "FILLME" CACHE PATH "wxWidgets include directories")
  add_subdirectory("src/vlWX")
endif(GUI_WXWIDGETS_SUPPORT)

if(GUI_WXWIDGETS_EXAMPLES)
  set(WXWIDGETS_LIBRARY_DIR "FILLME" CACHE PATH "wxWidgets libraries path")
  set(WXWIDGETS_LIBRARIES   "FILLME" CACHE STRING "wxWidgets core and base libraries (space separated)")
endif(GUI_WXWIDGETS_EXAMPLES)

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
endif(GUI_QT4_SUPPORT)

if(GUI_COCOA_SUPPORT)
  option(GUI_COCOA_EXAMPLES "Build Cocoa examples" ON)
  add_subdirectory("src/vlCocoa")
endif(GUI_COCOA_SUPPORT)

add_subdirectory("src/examples")

add_subdirectory("docs")
add_subdirectory("src/examples/Applets")

#install(FILES "bin/Win32_Tests.bat" DESTINATION bin)
#install(DIRECTORY "docs" DESTINATION ".")
install(FILES "LICENSE.TXT" "README.TXT" DESTINATION ".")
install(FILES "3rdparty/glew/include/GL/glew.h" "3rdparty/glew/include/GL/glxew.h" "3rdparty/glew/include/GL/wglew.h" DESTINATION "include/GL/" )

