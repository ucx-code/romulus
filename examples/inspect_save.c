/* Reads a Xen save file from a HVM guest and prints some info about the various headers and records on it */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <inttypes.h>

#include <byteswap.h>

#include "struts.h"

// Hand made
const char *hvm_save_typecodes[] =
        {
       			[0]           = "END",
                [1]           = "HEADER",
                [2]           = "CPU",
                [3]                  = "PIC",
                [4]            = "IOAPIC",
                [5]            = "LAPIC",
                [6]            = "LAPIC_REGS",
                [7]            = "PCI_IRQ",
                [8]            = "ISA_IRQ",
                [9]            = "PCI_LINK",
                [10]            = "PIT",
                [11]            = "RTC",
                [12]            = "HPET",
                [13]            = "PMTIMER",
                [14]            = "MTTR",
                [15]            = "VIRIDIAN_DOMAIN",
                [16]            = "CPU_XSAVE",
                [17]            = "VIRIDIAN_VCPU",
                [18]            = "VMCE_VCPU",
                [19]            = "TSC_ADJUST"
        };
        
/* From xc_sr_common.h */
const char *mandatory_rec_types[] =
        {
                [REC_TYPE_END]                          = "End",
                [REC_TYPE_PAGE_DATA]                    = "Page data",
                [REC_TYPE_X86_PV_INFO]                  = "x86 PV info",
                [REC_TYPE_X86_PV_P2M_FRAMES]            = "x86 PV P2M frames",
                [REC_TYPE_X86_PV_VCPU_BASIC]            = "x86 PV vcpu basic",
                [REC_TYPE_X86_PV_VCPU_EXTENDED]         = "x86 PV vcpu extended",
                [REC_TYPE_X86_PV_VCPU_XSAVE]            = "x86 PV vcpu xsave",
                [REC_TYPE_SHARED_INFO]                  = "Shared info",
                [REC_TYPE_TSC_INFO]                     = "TSC info",
                [REC_TYPE_HVM_CONTEXT]                  = "HVM context",
                [REC_TYPE_HVM_PARAMS]                   = "HVM params",
                [REC_TYPE_TOOLSTACK]                    = "Toolstack",
                [REC_TYPE_X86_PV_VCPU_MSRS]             = "x86 PV vcpu msrs",
                [REC_TYPE_VERIFY]                       = "Verify",
                [REC_TYPE_CHECKPOINT]                   = "Checkpoint",
                [REC_TYPE_CHECKPOINT_DIRTY_PFN_LIST]    = "Checkpoint dirty pfn list",
                [16] = "EPTP Info"
        };


#define TAB "\t\t"
void dump_vcpu_state(struct hvm_hw_cpu * v)
{
	printf(TAB"rip %lX rbp %lX rsp %lX rax %lX rbx %lX rcx %lX rdx %lX\n",
		v->rip, v->rbp, v->rsp, v->rax, v->rbx, v->rcx, v->rdx);
	printf(TAB"rsi %lX rdi %lX r8 %lX r9 %lX r10 %lX r11 %lX r12 %lX r13 %lX r14 %lX r15 %lX\n",
		v->rsi, v->rdi, v->r8, v->r9, v->r10, v->r11, v->r12, v->r13, v->r14, v->r15);
	printf(TAB"rflags %lX cr0 %lX cr2 %lX cr3 %lX cr4 %lX\n",
		v->rflags, v->cr0, v->cr2, v->cr3, v->cr4);
	printf(TAB"tsc %ld\n", v->tsc);
	printf(TAB"msr -> flags %lX lstar %lX star %lX cstar %lX syscall_mask %lX efer %lX tsc_aux %lX\n",
		v->msr_flags, v->msr_lstar, v->msr_star, v->msr_cstar, v->msr_syscall_mask, v->msr_efer, v->msr_tsc_aux);
	printf(TAB"sel -> cs %X ds %X es %X fs %X gs %X ss %X tr %X ldtr %X\n",
		v->cs_sel, v->ds_sel, v->es_sel, v->fs_sel, v->gs_sel, v->ss_sel, v->tr_sel, v->ldtr_sel);
	printf(TAB"limit -> cs %X ds %X es %X fs %X gs %X ss %X tr %X ldtr %X idtr %X gdtr %X\n",
		v->cs_limit, v->ds_limit, v->es_limit, v->fs_limit, v->gs_limit, v->ss_limit, v->tr_limit, v->ldtr_limit, v->idtr_limit, v->gdtr_limit);
	printf(TAB"base -> cs %lX ds %lX es %lX fs %lX gs %lX ss %lX tr %lX ldtr %lX idtr %lX gdtr %lX\n",
		v->cs_base, v->ds_base, v->es_base, v->fs_base, v->gs_base, v->ss_base, v->tr_base, v->ldtr_base, v->idtr_base, v->gdtr_base);
	printf(TAB"arbytes -> cs %X ds %X es %X fs %X gs %X ss %X tr %X ldtr %X\n",
		v->cs_arbytes, v->ds_arbytes, v->es_arbytes, v->fs_arbytes, v->gs_arbytes, v->ss_arbytes, v->tr_arbytes, v->ldtr_arbytes);
	printf(TAB"sysenter cs %lX esp %lX eip %lX\n", v->sysenter_cs, v->sysenter_esp, v->sysenter_eip);
	printf(TAB"FPU:\n");
	for (int i = 0; i < 512; i+=32) {
		printf(TAB);
		for (int a = 0; a < 32; a++) {
			printf("%.2X ", v->fpu_regs[i+a]);
		}
		printf("\n");
	}
}

void dump_pmtimer(struct hvm_hw_pmtimer * pm) 
{
	printf(TAB"tmr_val %u pm1a_sts %u pm1a_en %u\n", pm->tmr_val, pm->pm1a_sts, pm->pm1a_en);
}

void dump_rtc(struct hvm_hw_rtc * r) {
	printf(TAB);
	for (int i = 0; i < RTC_CMOS_SIZE; i++) {
		printf("cmos_data[%d] %u ", i, r->cmos_data[i]);
	}
	printf("\n"TAB"cmos_index %d pad0 %d\n", r->cmos_index, r->pad0);
}
/* Converts xc record type to human readable format */
const char * xc_type_to_str(uint32_t type)
{
    return mandatory_rec_types[type];
}

unsigned long checksum(void * data, int size) {
	unsigned long total = 0;
	for (int i=0; i < size; i++) {
		total = (total +((unsigned char *) data)[i]) % 65535;
	}
	return total;
}
// For code clarity I split the xc stream handling part to another function
// main keeps the libxl stream part and calls this func when needed
void handle_xc_stream(int fp)
{
    // Structs to hold record header and all together
    struct xc_sr_rhdr hdr_rec;
    struct xc_sr_record cur_rec;

    struct xc_sr_ihdr image_hdr;    // Struct to hold image header
    struct xc_sr_dhdr domain_hdr;    // Struct to hold domain header


    // We have found a xc protocol, lets parse it
    // First record is a image header
    assert (read(fp, &image_hdr, sizeof(struct xc_sr_ihdr)) == sizeof(struct xc_sr_ihdr));

    /* From the doc: "Integer (numeric) fields in the image header are always in big-endian
        byte order." */
    image_hdr.marker = __bswap_64(image_hdr.marker);
    image_hdr.id = __bswap_32(image_hdr.id);
    image_hdr.version = __bswap_32(image_hdr.version);
    image_hdr.options = __bswap_16(image_hdr.options);
    image_hdr._res1 = __bswap_16(image_hdr._res1);
    image_hdr._res2 = __bswap_32(image_hdr._res2);

    printf("\t[xc stream] Read an Image Header with marker=%ld id=%d version=%d options=%d\n", image_hdr.marker, image_hdr.id,
           image_hdr.version, image_hdr.options);

    /* 0 = little-endian, 1 = big-endian */
    int endianess = image_hdr.options & 0x1;
    printf("\t[xc stream] Endianess is %s\n", endianess ? "big" : "little");

    assert(endianess == 0); // We just support x86

    // Then a domain header
    assert (read(fp, &domain_hdr, sizeof(struct xc_sr_dhdr)) == sizeof(struct xc_sr_dhdr));
    printf("\t[xc stream] Read a Domain Header with type=%d page_shift=%d xen_maj=%d and xen_min=%d\n", domain_hdr.type,
           domain_hdr.page_shift,
           domain_hdr.xen_major, domain_hdr.xen_minor);

    assert(domain_hdr.type == 0x0002); // We just support HVM which is type = 2

    printf("Beginning of at offset %ld\n", lseek(fp, 0, SEEK_CUR));
    /* From the docs: "Integer fields in the domain header and in the records are in the
        endianness described in the image header (which will typically be the
        native ordering)." */
    while (1) {
        // Now lets read the records. Reading first the record header and then its data
        assert (read(fp, &hdr_rec, sizeof(struct xc_sr_rhdr)) == sizeof(struct xc_sr_rhdr));
        printf("\t[xc stream] Read a record header with type=%d (%s) and length=%d ", hdr_rec.type,
                xc_type_to_str(hdr_rec.type), hdr_rec.length);

        // If length is not multiple of 8 we must make it, because it is aligned to 8 octets
        hdr_rec.length = ROUNDUP(hdr_rec.length, 3); // 2**3 = 8
        cur_rec.data = malloc(hdr_rec.length);

        assert(read(fp, cur_rec.data, hdr_rec.length) == hdr_rec.length);
	printf(" (checksum: %lX)\n", checksum(cur_rec.data, hdr_rec.length));
        // Lets dwelve deeper into HVM params
        if (hdr_rec.type == REC_TYPE_HVM_PARAMS)
        {
     		printf("\t\t[offsets] hvm_params at position %ld\n", lseek(fp, 0, SEEK_CUR) - hdr_rec.length);
            uint32_t count = * (uint32_t*) cur_rec.data;
            void * now  = cur_rec.data + 8;
            printf("\t\t[hvm params] count %d\n", count);
            for (int i = 0; i < count; i++, now += 16)
            {
                uint64_t index = * (uint64_t *) now;
                uint64_t val = * (uint64_t *) (now + 8);
                printf("\t\t[hvm params] ind %ld = %ld\n", index, val);

            }
        } else if (hdr_rec.type == REC_TYPE_TSC_INFO) 
        {
        	/*struct xc_sr_rec_tsc_info
				{
					uint32_t mode;
					uint32_t khz;
					uint64_t nsec;
					uint32_t incarnation;
					uint32_t _res1;
				};
			*/
			uint32_t mode, khz, incarnation, _res1;
			uint64_t nsec;
			mode = ((uint32_t *) cur_rec.data)[0];
			khz = ((uint32_t *) cur_rec.data)[1];
			nsec = ((uint64_t *) cur_rec.data)[1];
			incarnation = ((uint32_t *) cur_rec.data)[4];
			_res1 = ((uint32_t *) cur_rec.data)[5];
			printf("\t\t[offsets] tsc_info at position %ld\n", lseek(fp, 0, SEEK_CUR) - hdr_rec.length);
			printf("\t\t[tsc info] mode %d khz %d nsec %ld incarnation %d res1 %d\n", mode, khz, nsec, incarnation,
				_res1);
        } else if (hdr_rec.type == REC_TYPE_HVM_CONTEXT)
        {
        	/*
        	struct hvm_save_descriptor
			{
				uint16_t typecode;          // Used to demux the various types below 
				uint16_t instance;          // Further demux within a type 
				uint32_t length;            // In bytes, *not* including this descriptor 
			};
			*/
			uint16_t typecode, instance;
			uint32_t length;
			uint32_t cur = 0;
			void * cur_p = cur_rec.data;
			char data[9000];
			struct hvm_hw_cpu * vcpu_state;
			int k = 0;
			while (cur < hdr_rec.length) {
				cur_p = cur_rec.data + cur;
				typecode = *(uint16_t*) cur_p;
				instance = ((uint16_t*) cur_p)[1];
				length = ((uint32_t*) cur_p)[1];
				
						printf("\t\t[hvm context] typecode %d (%s) instance %d length %d\n", typecode, 					hvm_save_typecodes[typecode], instance, length);
					
				switch (typecode) {
					case 2: /* CPU */
						printf("\t\t[offsets] cpu at position %ld\n", lseek(fp, 0, SEEK_CUR) - hdr_rec.length + k + 8);
						vcpu_state = (struct hvm_hw_cpu *) (cur_p + 8);
						dump_vcpu_state(vcpu_state);
						break;
					case 11: /* RTC */
						printf("\t\t[offsets] rtc at position %ld\n", lseek(fp, 0, SEEK_CUR) - hdr_rec.length + k + 8);
						dump_rtc((struct hvm_hw_rtc *) (cur_p+8));
						break;
					case 13: /* PMTIMER */
						printf("\t\t[offsets] pmtimer at position %ld\n", lseek(fp, 0, SEEK_CUR) - hdr_rec.length + k + 8);
						dump_pmtimer((struct hvm_hw_pmtimer *) (cur_p+8)); 	
						break;
					default:
						memcpy(data, cur_p+8, length);
						/*for (int xy = 0; xy < length; xy++)
						{
							printf("%X", ((char *)data)[xy]);
						}*/
						printf("\n");
				}
				k += 8 + length;
				cur += 8 + length; // 8 from the descriptor
			}
        }

        free(cur_rec.data); // We discard this info

        // A valid save file terminates with a record of type REC_TYPE_END
        if (hdr_rec.type == REC_TYPE_END)
            return;

    }
}

/*
32 bit big endian: QEMU_VM_FILE_MAGIC
32 bit big endian: QEMU_VM_FILE_VERSION

for_each_device
{
    8 bit:              QEMU_VM_SECTION_FULL
    32 bit big endian:  section_id
    8 bit:              idstr (ID string) length
    string:             idstr (ID string)
    32 bit big endian:  instance_id
    32 bit big endian:  version_id
    buffer:             device specific data
}

8 bit: QEMU_VM_EOF
*/
void handle_emulator_context(int fp) 
{
    uint32_t vm_file_magic, vm_file_version;

    uint8_t vm_section_full, idstr_len;
    uint32_t section_id, instance_id, version_id;
    char idstr[250];
	// It seems that the first are not required, VM FILE MAGIC (0x5145564d) only appears at byte 8
	assert (read(fp, &idstr, 8) == 8);
	
    assert (read(fp, &vm_file_magic, sizeof(uint32_t)) == sizeof(uint32_t));
    assert (read(fp, &vm_file_version, sizeof(uint32_t)) == sizeof(uint32_t));

    printf("\t\t\tfile magic %X file version %d\n", __bswap_32(vm_file_magic),
    	__bswap_32(vm_file_version));


	  	assert (read(fp, &vm_section_full, sizeof(uint8_t)) == sizeof(uint8_t));
	do {
		assert (read(fp, &section_id, sizeof(uint32_t)) == sizeof(uint32_t));
		assert (read(fp, &idstr_len, sizeof(uint8_t)) == sizeof(uint8_t));
	   	assert (read(fp, &idstr, idstr_len) == idstr_len);
		assert (read(fp, &instance_id, sizeof(uint32_t)) == sizeof(uint32_t));
		assert (read(fp, &version_id, sizeof(uint32_t)) == sizeof(uint32_t));

		printf("\t\t\tsection full %X id %d idstr_len %d idstr %s instance_id %d version_id %d\n",
			vm_section_full, __bswap_32(section_id), idstr_len, idstr, 
			__bswap_32(instance_id), __bswap_32(version_id));
			
		uint8_t tmp;
		do {
			assert( read(fp, &tmp, 8) == 8);
			printf("%d ", tmp);

			if (tmp == 1) { printf("\n"); return; }
		} while (1);
    } while (1);
}

int main(int argc, char * argv[])
{
    int tmp = 0;

    // libxl stream structs
    libxl__sr_hdr libxl_header;
    libxl__sr_rec_hdr libxl_rec;

    assert(argc == 2);

    int fp = open(argv[1], O_RDONLY);

    assert(fp >= 0);

    // Hack: Turns out the saved image (not the stream) does not start right away (can be seen using xxd command)
    // and I cant be bothered to debug xl right now.
    // So we read 16 bits in 16 bits until we find a specific combination and then go from there
    unsigned long cur = 0;
    uint64_t block;
    int rc;
    while (1) {

        rc = read(fp, &block, sizeof(uint64_t));

        assert(rc == sizeof(uint64_t));

        if (block == __bswap_64(RESTORE_STREAM_IDENT))
        //if (block == 0x000a)
        {
            break;
        }
        ++cur;
        lseek(fp, cur, SEEK_SET);
    }
    lseek(fp, cur, SEEK_SET);
    // First we must parse the libxl v2 protocol
    assert (read(fp, &libxl_header, sizeof(libxl_header)) > 0);

    // The stream is byte big endian, but x86 uses little endian. we need to convert
    // the code is not portable or pretty but I dont care right now
    libxl_header.ident = __bswap_64(libxl_header.ident);
    libxl_header.version = __bswap_32(libxl_header.version);
    libxl_header.options = __bswap_32(libxl_header.options);

    printf("[libxl stream] Stream Header with ident=%lX version=%d options=%d\n", libxl_header.ident, libxl_header.version,
            libxl_header.options);

    // Check some assumptions
    assert(libxl_header.ident == RESTORE_STREAM_IDENT);
    assert(libxl_header.version == RESTORE_STREAM_VERSION);

    while (1) {
        assert (read(fp, &libxl_rec, sizeof(libxl_rec)) > 0);

        printf("[libxl stream] Record header with type=%d length=%d\n", libxl_rec.type, libxl_rec.length);

        switch (libxl_rec.type)
        {
            case REC_TYPE_END:
                goto stream_end;
            case REC_TYPE_LIBXC_CONTEXT:
                handle_xc_stream(fp);
                break;

            case REC_TYPE_EMULATOR_CONTEXT:
                printf("[libxl stream] Emulator context @ offset %ld size %d\n", lseek(fp, 0, SEEK_CUR), libxl_rec.length);
				//handle_emulator_context(fp);
                goto discard;
            case REC_TYPE_EMULATOR_XENSTORE_DATA:
                printf("[libxl stream] Emulator xenstore data @ offset %ld size %d\n", lseek(fp, 0, SEEK_CUR), libxl_rec.length);
                goto discard;
            default:
                printf("[libxl stream] Something else\n");
            discard:

                // We dont parse other record types
                libxl_rec.length = ROUNDUP(libxl_rec.length, 3);
                void * tmp_b = malloc(libxl_rec.length);
			
                assert(read(fp, tmp_b, libxl_rec.length) == libxl_rec.length);
                /*	for (int a = 0; a < libxl_rec.length; a++) {
					printf("%c", ((char *) tmp_b)[a]);
				}*/
		printf("Checksum: %lX\n", checksum(tmp_b, libxl_rec.length));
                free(tmp_b);
        }

    }

stream_end:
    // Make sure we have reached the end of the file
    assert(read(fp, &tmp, 1) <= 0);
    close(fp);

}