type Doxyfile > Doxyfile.tmp
echo. >> Doxyfile.tmp
echo #---------------------- >> Doxyfile.tmp
echo. >> Doxyfile.tmp
echo PROJECT_NUMBER=2011.07.1151 >> Doxyfile.tmp
echo OUTPUT_DIRECTORY=D:/VL/Install/docs >> Doxyfile.tmp
echo HTML_OUTPUT=html >> Doxyfile.tmp
doxygen.exe Doxyfile.tmp