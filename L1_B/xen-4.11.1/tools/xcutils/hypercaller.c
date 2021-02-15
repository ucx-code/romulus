#include <stdio.h>
#include <stdlib.h>
#include <xenctrl.h>
#include <xen/sys/privcmd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>

int main(int argc, char *argv[])
{
	unsigned long addr; int a;
	if (argc != 3) return -1;
	sscanf(argv[1], "%lu", &addr);
	sscanf(argv[2], "%d", &a);
	int fd, ret;
    privcmd_hypercall_t mycall = {
            __HYPERVISOR_checkmem,
            { addr, 0, 0, a, 0}
    };
    fd = open("/proc/xen/privcmd", O_RDWR);
    if (fd < 0) {
        printf("Couldnt happen privcmd\n");
        return -1;
    }
    ret = ioctl(fd, IOCTL_PRIVCMD_HYPERCALL, &mycall);
    printf("%d\n", ret);
    return 0;
}


