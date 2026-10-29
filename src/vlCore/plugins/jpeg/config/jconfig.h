/* jconfig.h for IJG libjpeg 9 as bundled with Visualization Library.
 *
 * Based on the upstream jconfig.vc, which is valid for every compiler VL
 * supports (MSVC, MinGW, GCC, Clang). It is used on all platforms so that
 * libjpeg, libtiff (tif_jpeg.c) and ioJPG.cpp agree on the definition of
 * 'boolean': a mismatch changes the size of jpeg_decompress_struct and makes
 * libjpeg fail at runtime with "JPEG parameter struct mismatch".
 * See jconfig.txt in the libjpeg distribution for explanations.
 */

#define HAVE_PROTOTYPES
#define HAVE_UNSIGNED_CHAR
#define HAVE_UNSIGNED_SHORT
#undef CHAR_IS_UNSIGNED
#define HAVE_STDDEF_H
#define HAVE_STDLIB_H
#undef NEED_BSD_STRINGS
#undef NEED_SYS_TYPES_H
#undef NEED_FAR_POINTERS
#undef NEED_SHORT_EXTERNAL_NAMES
#undef INCOMPLETE_TYPES_BROKEN

/* Define "boolean" as unsigned char on every platform (Windows custom) */
#ifndef __RPCNDR_H__		/* don't conflict if rpcndr.h already read */
typedef unsigned char boolean;
#endif
#ifndef FALSE			/* in case these macros already exist */
#define FALSE	0		/* values of boolean */
#endif
#ifndef TRUE
#define TRUE	1
#endif
#define HAVE_BOOLEAN		/* prevent jmorecfg.h from redefining it */

#undef JPEG_HAVE_RGB_CUSTOM
#undef JPEG_USE_RGB_CUSTOM
#undef JPEG_HAVE_FILE_IO_CUSTOM
#undef JPEG_USE_FILE_IO_CUSTOM

#ifdef JPEG_INTERNALS

#undef RIGHT_SHIFT_IS_UNSIGNED

#endif /* JPEG_INTERNALS */
