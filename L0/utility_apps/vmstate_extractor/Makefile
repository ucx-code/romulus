XML2_FLAGS=-I/usr/include/libxml2/
XML2_LIBS=-lxml2 -lz -lm -ldl
#LIBVMI_ROOT=/external_hdd/xen/libvmi-0.12.0/
LIBVMI_ROOT=/var/lib/nova/instances/vmidbg/libvmi-0.12.0/

all: main.c sw_vmi_helper.c
	gcc -g --std=gnu99 ${XML2_FLAGS}  -I ${LIBVMI_ROOT} main.c -o vmstate -lvmi -lglib-2.0 ${XML2_LIBS}
	gcc -g --std=gnu99 ${XML2_FLAGS}  -I ${LIBVMI_ROOT} sw_vmi_helper.c -o sw_vmi_helper -lvmi -lglib-2.0 -Wall
