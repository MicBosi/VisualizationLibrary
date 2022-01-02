#!/bin/bash

# usage: ./generate_GLFunctionList.sh > ../src/vlGraphics/GL/GLFunctionList.hpp

GLDIR=../src/external/Khronos/GL

echo '// File automatically generated on '$(date '+%Y-%m-%d %H:%M:%S')

echo
echo '// GL Extensions'
echo
sed -n -E "s/.*\s*APIENTRY\s*(gl[^(]*).*/VL_GL_FUNCTION( PFN\\U\\1PROC, \\E\\1 )/p" $GLDIR/khronos_glext.h | sort
echo
echo '// WGL Extensions'
echo
echo '#if defined(VL_PLATFORM_WINDOWS)'
sed -n -E "s/.*\s*WINAPI\s*(wgl[^(]*).*/VL_GL_FUNCTION( PFN\\U\\1PROC, \\E\\1 )/p" $GLDIR/khronos_wglext.h | sort
echo '#endif'
echo
echo '// GLX Extensions'
echo
echo '#if defined(VL_PLATFORM_LINUX)'
sed -n -E "s/.*\s*(glX[^(]*).*/VL_GL_FUNCTION( PFN\\U\\1PROC, \\E\\1 )/p" $GLDIR/khronos_glxext.h | sort
echo '#endif'
echo
echo '#if defined(VL_PLATFORM_MACOSX)'
echo '  Nothing to do here?'
echo '#endif'

