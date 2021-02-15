XML2_FLAGS=-I/usr/include/libxml2/
XML2_LIBS=-lxml2 -lz -lm -ldl
LIBVMI_ROOT=/var/lib/nova/instances/vmidbg/libvmi-0.12.0/

all: main.c
	gcc --std=gnu99 ${XML2_FLAGS}  -I ${LIBVMI_ROOT} main.c -o vmstate -lvmi -lm -ljson-c -ljson-c -lm -ldl  -lglib-2.0   -ljson-c -ljson-c -lm -ldl  ${XML2_LIBS}

