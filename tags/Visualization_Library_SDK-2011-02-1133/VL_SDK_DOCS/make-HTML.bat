mkdir VL_SDK_DOCS
mkdir VL_SDK_DOCS\gallery
mkdir VL_SDK_DOCS\gallery_small
mkdir VL_SDK_DOCS\highslide

xcopy highslide\* VL_SDK_DOCS\highslide /Y /S
xcopy gallery\* VL_SDK_DOCS\gallery /Y /S
xcopy gallery_small\* VL_SDK_DOCS\gallery_small /Y /S
doxygen.exe