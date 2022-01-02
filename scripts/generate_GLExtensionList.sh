#!/bin/bash

# usage: ./generate_GLExtensionList.sh > ../src/vlGraphics/GL/GLExtensionList.hpp

GLDIR=../src/external/Khronos/GL

echo '// File automatically generated on '$(date '+%Y-%m-%d %H:%M:%S')

echo
echo '// GL Extensions'
echo
sed -n "s/#ifndef\s*\(GL_.*_[_a-zA-Z0-9]*\)/\1/p" $GLDIR/khronos_glext.h | sort | grep -v GL_VERSION | xargs -L 1 -I {} echo 'VL_EXTENSION('{}')'
echo
echo '// GLX Extensions'
echo
sed -n "s/#ifndef\s*\(GLX_.*_[_a-zA-Z0-9]*\)/\1/p" $GLDIR/khronos_glxext.h | sort | xargs -L 1 -I {} echo 'VL_EXTENSION('{}')'
echo
echo '// WGL Extensions'
echo
sed -n "s/#ifndef\s*\(WGL_.*_[_a-zA-Z0-9]*\)/\1/p" $GLDIR/khronos_wglext.h | sort | xargs -L 1 -I {} echo 'VL_EXTENSION('{}')'
