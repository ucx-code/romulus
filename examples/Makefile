XML2_FLAGS=-I/usr/include/libxml2/
XML2_LIBS=-lxml2 -lz -lm -ldl
CFLAGS+=-Wall -std=gnu99 -g -O0

all: inspect update update_qemu

inspect: inspect_save.c
	gcc ${CFLAGS} inspect_save.c -o inspect_save

update: update_save_file.c
	gcc  ${CFLAGS} ${XML2_FLAGS} update_save_file.c -o update_save_file  ${XML2_LIBS}  

update_qemu: update_qemu_ctx.c
	gcc ${CFLAGS} update_qemu_ctx.c -o update_qemu

clean:
	rm inspect_save update_save_file update_qemu
