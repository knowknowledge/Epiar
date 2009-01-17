CC=gcc
CFLAGS=`freetype-config --cflags` -Wall -g -O2
LIBS=`freetype-config --libs`

default: buildfont.c
	$(CC) $(CFLAGS) buildfont.c -o buildfont $(LIBS)

