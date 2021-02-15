#include <config.h>
#include <libvmi/libvmi.h>
#define _GNU_SOURCE
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

unsigned char memory[4096];
unsigned char memory2[4096];
unsigned char memory3[4096];
vmi_instance_t vmi = NULL;

/*
	EPTP
		Memory Type bit 0 - 2 (inclusive)
		Walk Length  3 - 5
		Enable EPT AD bits 6 
		Unused but reserved 7 - 11
		MFN 12 a 64 (total de 52 bits)

	eptp >> 12 da-nos o MFN	
*/
unsigned long virt_to_phys(unsigned long virt)
{
	if (virt == 0) return virt;
	return virt - 0xffff830000000000;
}

int get_basic_vminfo(xmlNodePtr *parent)
{
	unsigned short domid = *((unsigned short *) (memory + 0x0));
	unsigned int max_vcpus = *((unsigned int *) (memory + 0x4));
	unsigned int tot_pages = *((unsigned int *) (memory + 0x48));

	xmlChar buf[50];
	xmlNodePtr basic = xmlNewChild(*parent, NULL, BAD_CAST "basic", NULL);
	xmlStrPrintf(buf, 50, "%d", domid);
	xmlNewChild(basic, NULL, BAD_CAST "domid", buf);
	xmlStrPrintf(buf, 50, "%d", max_vcpus);
	xmlNewChild(basic, NULL, BAD_CAST "max_vcpus", buf);
	xmlStrPrintf(buf, 50, "%d", tot_pages);
	xmlNewChild(basic, NULL, BAD_CAST "total_pages", buf);

	// We dont want to know anything about dom0
	if (domid != 0)
		return max_vcpus;
	else
		return -1;
}

void pause_vm(unsigned int max_vcpus)
{
	unsigned long vcpu_list = *((unsigned long *) (memory + 0x8));

	int i;
	// Find more info about the vcpus
        vmi_read_pa(vmi, virt_to_phys(vcpu_list), 8 * max_vcpus, memory2, NULL);
        for (i = 0; i < max_vcpus; i++)
        {
                unsigned long vcpu_addr = ((unsigned long *) memory2)[i];
                unsigned long vcpu_p = vcpu_addr;
                // vcpu is runnable
                vmi_read_pa(vmi, virt_to_phys(vcpu_p) + 304, 4, memory3, NULL);
                int  pause_count = *((int *) memory3);
                //printf("PAAUSE %d\n", pause_count);
                *(int *) memory3 = 1;
                vmi_write_pa(vmi, virt_to_phys(vcpu_p) + 304,  4, memory3, NULL);
    		break; // TODO: Just supports 1 vcpu for now
	}
}

void get_percpu_info(unsigned int max_vcpus, xmlNodePtr *parent)
{
unsigned long vcpu_list = *((unsigned long *) (memory + 0x8));
	int i;
	//printf("\tDomain ID:%d Max vCPUs:%d Total pages: %d (%f Gb)\n", domid, max_vcpus, tot_pages, tot_pages*4/1024.0/1024.0);
	xmlChar buf[50], buf2[5];
	xmlNodePtr vcpus = xmlNewChild(*parent, NULL, BAD_CAST "vcpus", NULL);
	// Find more info about the vcpus
	vmi_read_pa(vmi, virt_to_phys(vcpu_list), 8 * max_vcpus, memory2, NULL);
	//unsigned long vcpu_array = *((unsigned long *) memory2);
	for (i = 0; i < max_vcpus; i++)
	{
		xmlNodePtr vcpu = xmlNewChild(vcpus, NULL, BAD_CAST "vcpu", NULL);
		unsigned long vcpu_addr = ((unsigned long *) memory2)[i];
		unsigned long vcpu_p = vcpu_addr;

		vmi_read_pa(vmi, virt_to_phys(vcpu_addr), 64, memory3, NULL);
		int vcpu_id = *((int *) memory3);
		int processor = *((int *) (memory3 + 0x4));
		xmlStrPrintf(buf, 50, "%d", vcpu_id);
		xmlNewChild(vcpu, NULL, BAD_CAST "vcpu_id", buf);
		xmlStrPrintf(buf, 50, "%d", processor);
		xmlNewChild(vcpu, NULL, BAD_CAST "processor", buf);

		vmi_read_pa(vmi, virt_to_phys(vcpu_p) + 736, 8, memory3, NULL);
		unsigned long rax = *((unsigned long *) memory3);
		xmlStrPrintf(buf, 50, "%lu", rax);
		xmlNewChild(vcpu, NULL, BAD_CAST "rax", buf);

		vmi_read_pa(vmi, virt_to_phys(vcpu_p) + 696, 8, memory3, NULL);
		unsigned long rbx = *((unsigned long *) memory3);
		xmlStrPrintf(buf, 50, "%lu", rbx);
		xmlNewChild(vcpu, NULL, BAD_CAST "rbx", buf);

		vmi_read_pa(vmi, virt_to_phys(vcpu_p) + 744, 8, memory3, NULL);
		unsigned long rcx = *((unsigned long *) memory3);
		xmlStrPrintf(buf, 50, "%lu", rcx);
		xmlNewChild(vcpu, NULL, BAD_CAST "rcx", buf);

		vmi_read_pa(vmi, virt_to_phys(vcpu_p) + 752, 8, memory3, NULL);
		unsigned long rdx = *((unsigned long *) memory3);
		xmlStrPrintf(buf, 50, "%lu", rdx);
		xmlNewChild(vcpu, NULL, BAD_CAST "rdx", buf);

		vmi_read_pa(vmi, virt_to_phys(vcpu_p) + 688, 8, memory3, NULL);
		unsigned long rbp = *((unsigned long *) memory3);
		xmlStrPrintf(buf, 50, "%lu", rbp);
		xmlNewChild(vcpu, NULL, BAD_CAST "rbp", buf);

		vmi_read_pa(vmi, virt_to_phys(vcpu_p) + 784, 8, memory3, NULL);
		unsigned long rip = *((unsigned long *) memory3);
		xmlStrPrintf(buf, 50, "%lu", rip);
		xmlNewChild(vcpu, NULL, BAD_CAST "rip", buf);

		vmi_read_pa(vmi, virt_to_phys(vcpu_p) + 760, 8, memory3, NULL);
		unsigned long rsi = *((unsigned long *) memory3);
		xmlStrPrintf(buf, 50, "%lu", rsi);
		xmlNewChild(vcpu, NULL, BAD_CAST "rsi", buf);

		vmi_read_pa(vmi, virt_to_phys(vcpu_p) + 768, 8, memory3, NULL);
		unsigned long rdi = *((unsigned long *) memory3);
		xmlStrPrintf(buf, 50, "%lu", rdi);
		xmlNewChild(vcpu, NULL, BAD_CAST "rdi", buf);

		vmi_read_pa(vmi, virt_to_phys(vcpu_p) + 808, 8, memory3, NULL);
		unsigned long rsp = *((unsigned long *) memory3);
		xmlStrPrintf(buf, 50, "%lu", rsp);
		xmlNewChild(vcpu, NULL, BAD_CAST "rsp", buf);

		vmi_read_pa(vmi, virt_to_phys(vcpu_p) + 800, 8, memory3, NULL);
		unsigned long rflags = *((unsigned long *) memory3);
		xmlStrPrintf(buf, 50, "%lu", rflags);
		xmlNewChild(vcpu, NULL, BAD_CAST "rflags", buf);

		vmi_read_pa(vmi, virt_to_phys(vcpu_p) + 728, 8, memory3, NULL);
		unsigned long r8 = *((unsigned long *) memory3);
		xmlStrPrintf(buf, 50, "%lu", r8);
		xmlNewChild(vcpu, NULL, BAD_CAST "r8", buf);

		vmi_read_pa(vmi, virt_to_phys(vcpu_p) + 720, 8, memory3, NULL);
		unsigned long r9 = *((unsigned long *) memory3);
		xmlStrPrintf(buf, 50, "%lu", r9);
		xmlNewChild(vcpu, NULL, BAD_CAST "r9", buf);

		vmi_read_pa(vmi, virt_to_phys(vcpu_p) + 712, 8, memory3, NULL);
		unsigned long r10 = *((unsigned long *) memory3);
		xmlStrPrintf(buf, 50, "%lu", r10);
		xmlNewChild(vcpu, NULL, BAD_CAST "r10", buf);

		vmi_read_pa(vmi, virt_to_phys(vcpu_p) + 704, 8, memory3, NULL);
		unsigned long r11 = *((unsigned long *) memory3);
		xmlStrPrintf(buf, 50, "%lu", r11);
		xmlNewChild(vcpu, NULL, BAD_CAST "r11", buf);

		vmi_read_pa(vmi, virt_to_phys(vcpu_p) + 680, 8, memory3, NULL);
		unsigned long r12 = *((unsigned long *) memory3);
		xmlStrPrintf(buf, 50, "%lu", r12);
		xmlNewChild(vcpu, NULL, BAD_CAST "r12", buf);

		vmi_read_pa(vmi, virt_to_phys(vcpu_p) + 672, 8, memory3, NULL);
		unsigned long r13 = *((unsigned long *) memory3);
		xmlStrPrintf(buf, 50, "%lu", r13);
		xmlNewChild(vcpu, NULL, BAD_CAST "r13", buf);

		vmi_read_pa(vmi, virt_to_phys(vcpu_p) + 664, 8, memory3, NULL);
		unsigned long r14 = *((unsigned long *) memory3);
		xmlStrPrintf(buf, 50, "%lu", r14);
		xmlNewChild(vcpu, NULL, BAD_CAST "r14", buf);

		vmi_read_pa(vmi, virt_to_phys(vcpu_p) + 656, 8, memory3, NULL);
		unsigned long r15 = *((unsigned long *) memory3);
		xmlStrPrintf(buf, 50, "%lu", r15);
		xmlNewChild(vcpu, NULL, BAD_CAST "r15", buf);

		vmi_read_pa(vmi, virt_to_phys(vcpu_p) + 856, 8, memory3, NULL);
		unsigned long debugreg0 = *((unsigned long *) memory3);
		xmlStrPrintf(buf, 50, "%lu", debugreg0);
		xmlNewChild(vcpu, NULL, BAD_CAST "debugreg0", buf);

		vmi_read_pa(vmi, virt_to_phys(vcpu_p) + 864, 8, memory3, NULL);
		unsigned long debugreg1 = *((unsigned long *) memory3);
		xmlStrPrintf(buf, 50, "%lu", debugreg1);
		xmlNewChild(vcpu, NULL, BAD_CAST "debugreg1", buf);

		vmi_read_pa(vmi, virt_to_phys(vcpu_p) + 872, 8, memory3, NULL);
		unsigned long debugreg2 = *((unsigned long *) memory3);
		xmlStrPrintf(buf, 50, "%lu", debugreg2);
		xmlNewChild(vcpu, NULL, BAD_CAST "debugreg2", buf);

		vmi_read_pa(vmi, virt_to_phys(vcpu_p) + 880, 8, memory3, NULL);
		unsigned long debugreg3 = *((unsigned long *) memory3);
		xmlStrPrintf(buf, 50, "%lu", debugreg3);
		xmlNewChild(vcpu, NULL, BAD_CAST "debugreg3", buf);

		vmi_read_pa(vmi, virt_to_phys(vcpu_p) + 888, 8, memory3, NULL);
		unsigned long debugreg4 = *((unsigned long *) memory3);
		xmlStrPrintf(buf, 50, "%lu", debugreg4);
		xmlNewChild(vcpu, NULL, BAD_CAST "debugreg4", buf);

		vmi_read_pa(vmi, virt_to_phys(vcpu_p) + 896, 8, memory3, NULL);
		unsigned long debugreg5 = *((unsigned long *) memory3);
		xmlStrPrintf(buf, 50, "%lu", debugreg5);
		xmlNewChild(vcpu, NULL, BAD_CAST "debugreg5", buf);

		vmi_read_pa(vmi, virt_to_phys(vcpu_p) + 904, 8, memory3, NULL);
		unsigned long debugreg6 = *((unsigned long *) memory3);
		xmlStrPrintf(buf, 50, "%lu", debugreg6);
		xmlNewChild(vcpu, NULL, BAD_CAST "debugreg6", buf);

		vmi_read_pa(vmi, virt_to_phys(vcpu_p) + 912, 8, memory3, NULL);
		unsigned long debugreg7 = *((unsigned long *) memory3);
		xmlStrPrintf(buf, 50, "%lu", debugreg7);
		xmlNewChild(vcpu, NULL, BAD_CAST "debugreg7", buf);

		vmi_read_pa(vmi, virt_to_phys(vcpu_p) + 264, 1, memory3, NULL);
		unsigned char fpu_initialised = *((unsigned char *) memory3);
		xmlStrPrintf(buf, 50, "%hhu", fpu_initialised);
		xmlNewChild(vcpu, NULL, BAD_CAST "fpu_initialised", buf);

		vmi_read_pa(vmi, virt_to_phys(vcpu_p) + 640, 8, memory3, NULL);
		unsigned char * fpu_ctx = ((unsigned char *) memory3);
		for (int j = 0; j < 512; j++) {
			xmlStrPrintf(buf2, 50, "fpu_%d", j);
			xmlStrPrintf(buf, 50, "%u", fpu_ctx[j]);
			xmlNewChild(vcpu, NULL, buf2, buf);
		}

		vmi_read_pa(vmi, virt_to_phys(vcpu_p) + 1536, 8, memory3, NULL);
		unsigned long shadow_gs = *((unsigned long *) memory3);
		xmlStrPrintf(buf, 50, "%lu", shadow_gs);
		xmlNewChild(vcpu, NULL, BAD_CAST "shadow_gs", buf);

		vmi_read_pa(vmi, virt_to_phys(vcpu_p) + 1552, 8, memory3, NULL);
		unsigned long lstar = *((unsigned long *) memory3);
		xmlStrPrintf(buf, 50, "%lu", lstar);
		xmlNewChild(vcpu, NULL, BAD_CAST "lstar", buf);

		vmi_read_pa(vmi, virt_to_phys(vcpu_p) + 1544, 8, memory3, NULL);
		unsigned long star = *((unsigned long *) memory3);
		xmlStrPrintf(buf, 50, "%lu", star);
		xmlNewChild(vcpu, NULL, BAD_CAST "star", buf);

		vmi_read_pa(vmi, virt_to_phys(vcpu_p) + 1560, 8, memory3, NULL);
		unsigned long cstar = *((unsigned long *) memory3);
		xmlStrPrintf(buf, 50, "%lu", cstar);
		xmlNewChild(vcpu, NULL, BAD_CAST "cstar", buf);

		vmi_read_pa(vmi, virt_to_phys(vcpu_p) + 1568, 8, memory3, NULL);
		unsigned long sfmask = *((unsigned long *) memory3);
		xmlStrPrintf(buf, 50, "%lu", sfmask);
		xmlNewChild(vcpu, NULL, BAD_CAST "sfmask", buf);

		vmi_read_pa(vmi, virt_to_phys(vcpu_p) + 1024, 8, memory3, NULL);
		unsigned long guest_cr0 = *((unsigned long *) memory3);
		xmlStrPrintf(buf, 50, "%lu", guest_cr0);
		xmlNewChild(vcpu, NULL, BAD_CAST "guest_cr0", buf);

		vmi_read_pa(vmi, virt_to_phys(vcpu_p) + 1032, 8, memory3, NULL);
		unsigned long guest_cr1 = *((unsigned long *) memory3);
		xmlStrPrintf(buf, 50, "%lu", guest_cr1);
		xmlNewChild(vcpu, NULL, BAD_CAST "guest_cr1", buf);

		vmi_read_pa(vmi, virt_to_phys(vcpu_p) + 1040, 8, memory3, NULL);
		unsigned long guest_cr2 = *((unsigned long *) memory3);
		xmlStrPrintf(buf, 50, "%lu", guest_cr2);
		xmlNewChild(vcpu, NULL, BAD_CAST "guest_cr2", buf);

		vmi_read_pa(vmi, virt_to_phys(vcpu_p) + 1048, 8, memory3, NULL);
		unsigned long guest_cr3 = *((unsigned long *) memory3);
		xmlStrPrintf(buf, 50, "%lu", guest_cr3);
		xmlNewChild(vcpu, NULL, BAD_CAST "guest_cr3", buf);

		vmi_read_pa(vmi, virt_to_phys(vcpu_p) + 1056, 8, memory3, NULL);
		unsigned long guest_cr4 = *((unsigned long *) memory3);
		xmlStrPrintf(buf, 50, "%lu", guest_cr4);
		xmlNewChild(vcpu, NULL, BAD_CAST "guest_cr4", buf);

		vmi_read_pa(vmi, virt_to_phys(vcpu_p) + 1064, 8, memory3, NULL);
		unsigned long guest_efer = *((unsigned long *) memory3);
		xmlStrPrintf(buf, 50, "%lu", guest_efer);
		xmlNewChild(vcpu, NULL, BAD_CAST "guest_efer", buf);

		xmlNewChild(vcpu, NULL, BAD_CAST "pending_event", BAD_CAST "0");
		xmlNewChild(vcpu, NULL, BAD_CAST "error_code", BAD_CAST "0");
		xmlNewChild(vcpu, NULL, BAD_CAST "msr_flags", BAD_CAST "0");

	}
}

void get_tsc_info(unsigned long address, xmlNodePtr * parent)
{
	xmlChar buf[50];
	xmlNodePtr tsc_info = xmlNewChild(*parent, NULL, BAD_CAST "tsc_info", NULL);

	vmi_read_pa(vmi, address + 0x9AC, 4, memory2, NULL);
	uint32_t incarnation = *((uint32_t *) memory2);
	xmlStrPrintf(buf, 50, "%u", incarnation);
	xmlNewChild(tsc_info, NULL, BAD_CAST "incarnation", buf);

	vmi_read_pa(vmi, address + 0x978, 4, memory2, NULL);
	int tsc_mode = *((int *) memory2);
	xmlStrPrintf(buf, 50, "%d", tsc_mode);
	xmlNewChild(tsc_info, NULL, BAD_CAST "tsc_mode", buf);

	vmi_read_pa(vmi, address + 0x97C, 1, memory2, NULL);
	unsigned char vtsc = *((unsigned char *) memory2);
	xmlStrPrintf(buf, 50, "%d", vtsc);
	xmlNewChild(tsc_info, NULL, BAD_CAST "vtsc", buf);

	vmi_read_pa(vmi, address + 0x998, 4, memory2, NULL);
	uint32_t tsc_khz = *((uint32_t *) memory2);
	xmlStrPrintf(buf, 50, "%u", tsc_khz);
	xmlNewChild(tsc_info, NULL, BAD_CAST "tsc_khz", buf);

	//vmi_read_pa(vmi, address + 0x99C, 8, memory2, NULL);
	//unsigned long vtsc_to_ns = *((unsigned long *) memory2);

	vmi_read_pa(vmi, address + 0x990, 8, memory2, NULL);
	unsigned long vtsc_offset = *((unsigned long *) memory2);
	xmlStrPrintf(buf, 50, "%lu", vtsc_offset);
	xmlNewChild(tsc_info, NULL, BAD_CAST "vtsc_offset", buf);
}

void get_pmtimer(unsigned long address, xmlNodePtr * parent)
{
	xmlChar buf[50];
	xmlNodePtr pmtimer = xmlNewChild(*parent, NULL, BAD_CAST "pmtimer", NULL);

	vmi_read_pa(vmi, address + 0x500, 4, memory2, NULL);
	uint32_t tmr_val = *((uint32_t *) memory2);
	xmlStrPrintf(buf, 50, "%u", tmr_val);
	xmlNewChild(pmtimer, NULL, BAD_CAST "tmr_val", buf);

	vmi_read_pa(vmi, address + 0x504, 2, memory2, NULL);
	uint16_t pm1a_sts = *((uint16_t *) memory2);
	xmlStrPrintf(buf, 50, "%u", pm1a_sts);
	xmlNewChild(pmtimer, NULL, BAD_CAST "pm1a_sts", buf);

	vmi_read_pa(vmi, address + 0x506, 2, memory2, NULL);
	uint16_t pm1a_en = *((uint16_t *) memory2);
	xmlStrPrintf(buf, 50, "%u", pm1a_en);
	xmlNewChild(pmtimer, NULL, BAD_CAST "pm1a_en", buf);
}

void get_rtc(unsigned long address, xmlNodePtr * parent)
{
	int xx;
	xmlChar buf[50], buf2[20];
	xmlNodePtr rtc = xmlNewChild(*parent, NULL, BAD_CAST "rtc", NULL);

	vmi_read_pa(vmi, address + 656, 8, memory2, NULL);
	unsigned long pl_time = *(unsigned long *) memory2;
	xmlStrPrintf(buf, 50, "%lu", pl_time);
	xmlNewChild(rtc, NULL, BAD_CAST "pl_time", buf);

	vmi_read_pa(vmi, address + 0x5B0, 8, memory2, NULL);
	unsigned long sync_tsc = *(unsigned long *) memory2;
	xmlStrPrintf(buf, 50, "%lu", sync_tsc);
	xmlNewChild(rtc, NULL, BAD_CAST "sync_tsc", buf);

	vmi_read_pa(vmi, virt_to_phys(pl_time), 20, memory2, NULL);
	uint8_t * cmos_data = (uint8_t *) memory2;
	for (xx = 0; xx < 14; xx++) {
		xmlStrPrintf(buf2, 20, "cmos_data%u", xx);
		xmlStrPrintf(buf, 50, "%u", cmos_data[xx]);
		xmlNewChild(rtc, NULL, buf2, buf);

	}
	vmi_read_pa(vmi, virt_to_phys(pl_time) + 14, 1, memory2, NULL);
	uint8_t cmos_index = *(uint8_t *) memory2;
	xmlStrPrintf(buf, 50, "%u", cmos_index);
	xmlNewChild(rtc, NULL, BAD_CAST "cmos_index", buf);

	vmi_read_pa(vmi, virt_to_phys(pl_time) + 15, 1, memory2,  NULL);
	uint8_t pad0 = *(uint8_t *) memory2;
	xmlStrPrintf(buf, 50, "%u", pad0);
	xmlNewChild(rtc, NULL, BAD_CAST "pad0", buf);
}

void get_eptp(unsigned long address, xmlNodePtr * parent)
{
	xmlChar buf[50];

	// 0x750 for xen 4.12.x, 0x758 for xen 4.11.1, 0x798 in xen 4.13.1, 0x750 for xen 4.12.3
	vmi_read_pa(vmi, address + 0x750, 8, memory2, NULL);	
	unsigned long p2m = *((unsigned long *) memory2);

	vmi_read_pa(vmi, virt_to_phys(p2m) + 0x40, 8, memory2, NULL);
	unsigned char p2m_class = *((unsigned char *) memory2);

	// used to be 0x280
	vmi_read_pa(vmi, virt_to_phys(p2m) + 0x280, 8, memory2, NULL);
	//vmi_read_pa(vmi, virt_to_phys(p2m) + 0x288, 8, memory2, NULL);
	unsigned long eptp = *((unsigned long *) memory2);
	xmlStrPrintf(buf, 50, "%lu", eptp);
	xmlNodePtr eptp_n = xmlNewChild(*parent, NULL, BAD_CAST "eptp", buf);

}

int
main(
    int argc,
    char **argv)
{
	int i, xx;
	int op_type=0;
	unsigned long addr_in;
    if ( argc < 3 )
        return -1;
	sscanf(argv[2], "%lu", &addr_in);
   if (argc == 4) {
		sscanf(argv[3], "%d", &op_type);
	}
    //addr_t address = 0x32EBA7000;
    addr_t address = (addr_t) addr_in;
    vmi_mode_t mode;

	xmlDocPtr doc = NULL;
	xmlNodePtr root = NULL, vm = NULL;
	LIBXML_TEST_VERSION;
    /* this is the VM or file that we are looking at */
    char *name = argv[1];

    if (VMI_FAILURE == vmi_get_access_mode(vmi, (void*)name, VMI_INIT_DOMAINNAME, NULL, &mode) ) {
	printf("Wut\n");
        goto error_exit;
    }

    /* initialize the libvmi library */
    if (VMI_FAILURE == vmi_init(&vmi, mode, (void*)name, VMI_INIT_DOMAINNAME, NULL, NULL)) {
        printf("Failed to init LibVMI library.\n");
        goto error_exit;
    }
	vmi_pause_vm(vmi);
	doc = xmlNewDoc(BAD_CAST "1.0");
	root = xmlNewNode(NULL, BAD_CAST "vms");
	xmlDocSetRootElement(doc, root);
    do {
	
	int SIZE = 0x800; // Read everything at once	
	if (VMI_SUCCESS == vmi_read_pa(vmi, address, SIZE, memory, NULL)) {
	vm = xmlNewChild(root, NULL, BAD_CAST "vm", NULL);


	// Xen 4.11.1  and Xen 4.12.3
	unsigned long next_domain = *((unsigned long *) (memory + 0x70));
	// Xen 4.13.1 
	//unsigned long next_domain = *((unsigned long *) (memory + 0x78));
	unsigned int max_vcpus = get_basic_vminfo(&vm);
	if (max_vcpus != -1)
	{
		if (op_type == 0)
		{
			get_percpu_info(max_vcpus, &vm);
			get_tsc_info(address, &vm);
			get_pmtimer(address, &vm);
			get_rtc(address, &vm);
			get_eptp(address, &vm);
		} else {
			pause_vm(max_vcpus);
		}
	}
	address = virt_to_phys(next_domain);
	} else {
	printf("unmapped %lX\n", address);
	address = 0;
	}

    } while (address != 0);
	vmi_resume_vm(vmi);
    //}

error_exit:

    /* cleanup any memory associated with the libvmi instance */
    vmi_destroy(vmi);
    if (op_type != 1) {
	xmlSaveFormatFileEnc("-", doc, "UTF-8", 1);
    }
	xmlFreeDoc(doc);
	xmlCleanupParser();
	xmlMemoryDump();
    
    return 0;
}
