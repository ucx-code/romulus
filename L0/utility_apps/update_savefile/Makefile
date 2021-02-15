XML2_FLAGS=-I/usr/include/libxml2/
XML2_LIBS=-lxml2 -lz -lm -ldl
CFLAGS+=-Wall -std=gnu99 -O3

update_savefile: update_savefile.c
	gcc  ${CFLAGS} ${XML2_FLAGS} update_savefile.c -o update_savefile  ${XML2_LIBS}  
