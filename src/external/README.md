
 The content of this directory (except the CMakeLists.txt files) is not 
 part of Visualization Library and the copyright of each library belongs 
 to its respective author(s).
 
 +------------------+------------+---------+---------------------------------+
 | LIBRARY          | DIRECTORY  | VERSION | WEBSITE                         |
 +------------------+------------+---------+---------------------------------+
 | FreeType library | freetype   | 2.15.0  | https://freetype.org            |
 | zlib library     | zlib       | 1.3.2   | https://zlib.net                |
 | IJG's libjpeg    | libjpeg    | 9f      | https://www.ijg.org             |
 | libpng library   | libpng     | 1.6.59  | http://www.libpng.org           |
 | libtiff library  | libtiff    | 4.7.2   | https://libtiff.gitlab.io       |
 +------------------+------------+---------+---------------------------------+

 The libraries are unmodified upstream release files; only what is needed
 to build them is kept (sources, public headers and licence files). Build
 configuration lives in the CMakeLists.txt files under src/vlCore/plugins
 and src/vlGraphics/plugins:
 
 - libjpeg: jconfig.h is provided by src/vlCore/plugins/jpeg/config.
 - libtiff: tif_config.h and tiffconf.h are generated at configure time
   from the upstream *.cmake.in templates.
 - libpng: pnglibconf.h is upstream's scripts/pnglibconf.h.prebuilt.

 To upgrade a library, replace its directory with the same subset of files
 from the new release and update the source list in its CMakeLists.txt.

Other credits:

- Colin Plumb: MD5 message-digest algorithm; see "md5" directory.

- Tanguy Fautré: Triangle stripping algorithm; see "tristripper" directory.
