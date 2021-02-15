#include <stdio.h>
#include <stdlib.h>
#include <xenctrl.h>
#include <xen/sys/privcmd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>

int main(int argc, char *argv[])
{
int fd;
int id, tid;
unsigned long ret, addr, base_pfn;
	if (argc != 5) {
		printf("Hypercaller2 received incorrect amount of params\n");
		return -1;
	}
	sscanf(argv[1], "%d", &id);
	sscanf(argv[2], "%lu", &addr);
	sscanf(argv[3], "%d", &tid);
	sscanf(argv[4], "%lu", &base_pfn);
 privcmd_hypercall_t mycall = {
            __HYPERVISOR_testing,
            { id, addr, tid, base_pfn, 0}
    };
    fd = open("/proc/xen/privcmd", O_RDWR);
    if (fd < 0) {
        printf("Couldnt happen privcmd\n");
        return -1;
    }
    ret = ioctl(fd, IOCTL_PRIVCMD_HYPERCALL, &mycall);
    printf("%lu\n", ret);
    return 0;
}


