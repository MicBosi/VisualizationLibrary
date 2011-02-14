mkdir Visualization_Library_Docs
mkdir Visualization_Library_Docs\gallery
mkdir Visualization_Library_Docs\gallery_small
mkdir Visualization_Library_Docs\highslide

xcopy highslide\* Visualization_Library_Docs\highslide /Y /S
xcopy gallery\* Visualization_Library_Docs\gallery /Y /S
xcopy gallery_small\* Visualization_Library_Docs\gallery_small /Y /S
doxygen.exe