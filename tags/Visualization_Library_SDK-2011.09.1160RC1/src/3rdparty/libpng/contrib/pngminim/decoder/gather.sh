cp ../../pngminus/png2pnm.c pngm2pnm.c
cp ../../../*.h .
cp ../../../*.c .
rm example.c pnggccrd.c pngvcrd.c pngtest.c pngpread.c
# change the following 2 lines if zlib is somewhere else
cp ../../../../zlib/*.h .
cp ../../../../zlib/*.c .
rm minigzip.c example.c compress.c deflate.c 
