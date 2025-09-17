# Visualization Library 2.2

[Gallery](http://VisualizationLibrary.org/gallery)

## About

Visualization Library is a C++ middleware for high-performance 2D and 3D graphics applications based on the industry standard OpenGL 1.x-4.x, designed to develop portable applications for the Windows, Linux and Mac OS X operating systems.

## Compilation and Installation

### Windows via CLI

- Install CMake 3.x: https://cmake.org/download/
- Install Doxygen 1.8.x: http://www.doxygen.nl/download.html (optional to build the docs, if not interested remove `-DVL_BUILD_DOCS=ON`)
- Install Qt5 (optional, to run the Qt5 GUI bindings, if not interested remove `-DVL_GUI_QT5_SUPPORT=ON` `-DVL_GUI_QT5_EXAMPLES=ON`)

Example to build and install a `Debug` build of VL and run the `vlQt5_tests.exe glsl` test:

```
cd C:\
git clone git@github.com:MicBosi/VisualizationLibrary.git
cd VisualizationLibrary
mkdir _BUILD
mkdir _INSTALL
cd _BUILD
cmake .. -G "Visual Studio 16 2019" -DVL_GUI_WIN32_SUPPORT=ON -DVL_GUI_WIN32_EXAMPLES=ON -DVL_GUI_QT5_SUPPORT=ON -DVL_GUI_QT5_EXAMPLES=ON -DVL_INSTALL_DATA=ON -DVL_BUILD_DOCS=ON -DCMAKE_INSTALL_PREFIX='C:\VisualizationLibrary\_INSTALL\'
cmake --build . --config Debug --target INSTALL
cmake --build . --config Debug --target Docs
set PATH=C:\VisualizationLibrary\_INSTALL\bin;%PATH%
set VL_DATA_PATH=C:\VisualizationLibrary\_INSTALL\data
vlQt5_tests.exe glsl
cat log.txt
```

Use `make --build . --config Release --target INSTALL` to build and install in `Release` mode.

### Linux via CLI

Something similar to this should work (uses QT5 gui bindings as an example, requires Qt5 dev to be installed):

```
cd ~
git clone git@github.com:MicBosi/VisualizationLibrary.git
cd VisualizationLibrary
mkdir _BUILD/Debug _BUILD/Release -p
mkdir _INSTALL

# Debug build
cd _BUILD/Debug
cmake ../.. -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug -DVL_GUI_QT5_SUPPORT=ON -DVL_GUI_QT5_EXAMPLES=ON -DVL_INSTALL_DATA=ON -DVL_BUILD_DOCS=ON -DCMAKE_INSTALL_PREFIX=~/VisualizationLibrary/_INSTALL
make -j 4
make install
make Docs
vlQt5_tests glsl
cat log.txt

# Release build
cd ../..
cd _BUILD/Release
cmake ../.. -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DVL_GUI_QT5_SUPPORT=ON -DVL_GUI_QT5_EXAMPLES=ON -DVL_INSTALL_DATA=ON -DVL_BUILD_DOCS=ON -DCMAKE_INSTALL_PREFIX=~/VisualizationLibrary/_INSTALL
make -j 4
make install
export PATH=~/VisualizationLibrary/_INSTALL/bin:$PATH
export VL_DATA_PATH=~/VisualizationLibrary/_INSTALL/data
vlQt5_tests glsl
cat log.txt
```

### Windows, Linux, Mac via GUI

Follow the instructions here: http://visualizationlibrary.org/docs/2.0/html/pag_install.html

## Licensing

Visualization Library is released under the OSI approved Simplified BSD License ([LICENSE.md](LICENSE.md)).

## Resources

* Official website and documentation: [VisualizationLibrary.org](http://VisualizationLibrary.org)
  
* Support and contribution:
    * [Visualization Library Google group](https://groups.google.com/forum/#!forum/visualization-library).
    * [Issue tracker on GitHub](https://github.com/MicBosi/VisualizationLibrary/issues).
    * Send your patches via [GitHub pull request](https://help.github.com/articles/using-pull-requests/).

* Releases: [github.com/MicBosi/VisualizationLibrary/releases](https://github.com/MicBosi/VisualizationLibrary/releases)

* Sources: [github.com/MicBosi/VisualizationLibrary](https://github.com/MicBosi/VisualizationLibrary)

* Download page: [VisualizationLibrary.org/download](http://VisualizationLibrary.org/download)

* Contacts: 
    * michele *at* visualizationlibrary *dot* org
    * [twitter.com/VizLibrary](https://twitter.com/VizLibrary)
    * [michelebosi.com](https://michelebosi.com)

Happy coding :)

# Star History

[![Star History Chart](https://api.star-history.com/svg?repos=MicBosi/VisualizationLibrary&type=Date)](https://www.star-history.com/#MicBosi/VisualizationLibrary&Date)
