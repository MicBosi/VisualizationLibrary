#!/bin/bash

# Usage: ./generate_GLFunctionList_1_1.sh > ../src/vlGraphics/GL/GLFunctionList_1_1.hpp
# Note: to generate gl_1_1.h take Mesa's or Win's gl.h and remove all functions above 1.1

GLDIR=../src/external/Khronos/GL

echo '// File automatically generated on '$(date '+%Y-%m-%d %H:%M:%S')

echo
echo '// GL 1.1 Functions'
echo
sed -n -E "s/.*\s*APIENTRY\s*(gl[^(]*).*/VL_GL_FUNCTION( \\1 )/p" gl_1_1.h | sort 
