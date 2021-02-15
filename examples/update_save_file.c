/*
 * Receives an XML file with VM-specific data, a base xl save file and the offsets of where various structures
 * reside on that file. Updates original values of the save file with the new (XML-given) values.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <inttypes.h>

#include "struts.h"
#include <libxml/xmlreader.h>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#ifndef LIBXML_READER_ENABLED
	#error LibXML2 not supported
#endif

#ifndef LIBXML_XPATH_ENABLED
	#error LibXML2 XPath not supported
#endif

#define SWAP(X, Y, Z) if (X->Z != Y->Z) { printf("Swapping %s %lX with %lX\n", #Z, X->Z, Y->Z); X->Z = Y->Z; }
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat"

#define REC_TYPE_HVM_EPTP_INFO              0x00000010U

#define _1GB 0x40000000
#define _4GB 0x100000000


void swap_pmtimer(struct hvm_hw_pmtimer * source, struct hvm_hw_pmtimer * target)
{
	SWAP(source, target, tmr_val);
	SWAP(source, target, pm1a_sts);
	SWAP(source, target, pm1a_en);
}

void swap_rtc(struct hvm_hw_rtc * source, struct hvm_hw_rtc * target)
{
    int i;
    
	SWAP(source, target, cmos_index);
	SWAP(source, target, pad0);
    for (i = 0; i < RTC_CMOS_SIZE; i++)
    {
    	SWAP(source, target, cmos_data[i]);
    }
}

void swap_cpu(struct hvm_hw_cpu * source, struct hvm_hw_cpu * target)
{
    int i;
    for (i = 0; i < 512; i++)
    {
    	SWAP(source, target, fpu_regs[i]);
    }
	SWAP(source, target, rax);
	SWAP(source, target, rbx);
	SWAP(source, target, rcx);
	SWAP(source, target, rdx);
	SWAP(source, target, rbp);
	SWAP(source, target, rip);
	SWAP(source, target, rsi);
	SWAP(source, target, rdi);
	SWAP(source, target, rsp);
	SWAP(source, target, rflags);
	SWAP(source, target, r8);
	SWAP(source, target, r9);
	SWAP(source, target, r10);
	SWAP(source, target, r11);
	SWAP(source, target, r12);
	SWAP(source, target, r13);
	SWAP(source, target, r14);
	SWAP(source, target, r15);
	SWAP(source, target, dr0);
	SWAP(source, target, dr1);
	SWAP(source, target, dr2);
	SWAP(source, target, dr3);
	SWAP(source, target, dr7);
	SWAP(source, target, shadow_gs);
	SWAP(source, target, flags);
	SWAP(source, target, msr_lstar);
	SWAP(source, target, msr_star);
	SWAP(source, target, msr_cstar);
	SWAP(source, target, msr_syscall_mask);
	SWAP(source, target, cr0);
	SWAP(source, target, cr2);
	SWAP(source, target, cr3);
	SWAP(source, target, cr4);
	SWAP(source, target, msr_efer);
	SWAP(source, target, pending_event);
	SWAP(source, target, error_code);
	SWAP(source, target, msr_flags);

}

#pragma GCC diagnostic pop

uint64_t * extract_eptp(xmlNodePtr cur, void * base)
{
    uint64_t * tmp;
    xmlChar * val;
    
    if (cur == NULL)
        return NULL;
    tmp = malloc(sizeof(uint64_t));
    *tmp = 0;
    do {
    	val = xmlNodeGetContent(cur);
        if (xmlStrcmp(cur->name, (const xmlChar *) "eptp") == 0) 
        {
        	sscanf((const char *) val, "%lu", tmp);
        } 
		cur = cur->next;
	} while (cur != NULL);
	assert(*tmp != 0);
    return tmp;
}

struct hvm_hw_pmtimer * extract_pmtimer(xmlNodePtr cur, void * base)
{
    struct hvm_hw_pmtimer * tmp;
    xmlChar * val;
    
    if (cur == NULL)
        return NULL;
        
    tmp = malloc(sizeof(struct hvm_hw_pmtimer));
    memcpy(tmp, base, sizeof(struct hvm_hw_pmtimer));

	do {
    	val = xmlNodeGetContent(cur);
        if (xmlStrcmp(cur->name, (const xmlChar *) "tmr_val") == 0) 
        {
        	sscanf((const char *) val, "%u", &tmp->tmr_val);
        } else if (xmlStrcmp(cur->name, (const xmlChar *) "pm1a_sts") == 0) {
        	sscanf((const char *) val, "%hu", &tmp->pm1a_sts);
        } else if (xmlStrcmp(cur->name, (const xmlChar *) "pm1a_en") == 0) {
        	sscanf((const char *) val, "%hu", &tmp->pm1a_en);
        } 
		cur = cur->next;
	} while (cur != NULL);

    return tmp;
}

struct hvm_hw_rtc * extract_rtc(xmlNodePtr cur, void * base)
{
    struct hvm_hw_rtc * tmp;
    xmlChar * val;
    
    if (cur == NULL)
        return NULL;
        
    tmp = malloc(sizeof(struct hvm_hw_rtc));
    memcpy(tmp, base, sizeof(struct hvm_hw_rtc));

	do {
    	val = xmlNodeGetContent(cur);
        if (xmlStrncmp(cur->name, (const xmlChar *) "cmos_data", 9) == 0) 
        {
            int id;
            sscanf((const char *) cur->name, "cmos_data%d", &id);
        	sscanf((const char *) val, "%hhu", &tmp->cmos_data[id]);
        } else if (xmlStrcmp(cur->name, (const xmlChar *) "cmos_index") == 0) {
        	sscanf((const char *) val, "%hhu", &tmp->cmos_index);
        } else if (xmlStrcmp(cur->name, (const xmlChar *) "pad0") == 0) {
        	sscanf((const char *) val, "%hhu", &tmp->pad0);
        } 
		cur = cur->next;
	} while (cur != NULL);

    return tmp;
}

struct hvm_hw_cpu * extract_cpu(xmlNodePtr cur, void * base)
{
    struct hvm_hw_cpu * tmp;
    xmlChar * val;
    
    if (cur == NULL)
        return NULL;
        
    tmp = malloc(sizeof(struct hvm_hw_cpu));
    memcpy(tmp, base, sizeof(struct hvm_hw_cpu));
	do {
    	val = xmlNodeGetContent(cur);
        if (xmlStrcmp(cur->name, (const xmlChar *) "fpu_initialised") == 0) {
        	unsigned char initialised;
        	sscanf((const char *) val, "%hhu", &initialised);
        	if (initialised)
        	{
        	    tmp->flags = XEN_X86_FPU_INITIALISED;
        	} else {
                    tmp->flags = 0;
		}
        }  else if (xmlStrncmp(cur->name, (const xmlChar *) "fpu_", 4) == 0) 
        {
            int id;
            sscanf((const char *) cur->name, "fpu_%d", &id);
        	sscanf((const char *) val, "%hhu", &tmp->fpu_regs[id]); 
    	} else if (xmlStrcmp(cur->name, (const xmlChar *) "rax") == 0) {
        	sscanf((const char *) val, "%lu", &tmp->rax);
        } else if (xmlStrcmp(cur->name, (const xmlChar *) "rbx") == 0) {
        	sscanf((const char *) val, "%lu", &tmp->rbx);
        } else if (xmlStrcmp(cur->name, (const xmlChar *) "rcx") == 0) {
        	sscanf((const char *) val, "%lu", &tmp->rcx);
        } else if (xmlStrcmp(cur->name, (const xmlChar *) "rdx") == 0) {
        	sscanf((const char *) val, "%lu", &tmp->rdx);
        } else if (xmlStrcmp(cur->name, (const xmlChar *) "rbp") == 0) {
        	sscanf((const char *) val, "%lu", &tmp->rbp);
        } else if (xmlStrcmp(cur->name, (const xmlChar *) "rip") == 0) {
        	sscanf((const char *) val, "%lu", &tmp->rip);
        } else if (xmlStrcmp(cur->name, (const xmlChar *) "rsi") == 0) {
        	sscanf((const char *) val, "%lu", &tmp->rsi);
        } else if (xmlStrcmp(cur->name, (const xmlChar *) "rdi") == 0) {
        	sscanf((const char *) val, "%lu", &tmp->rdi);
        } else if (xmlStrcmp(cur->name, (const xmlChar *) "rsp") == 0) {
        	sscanf((const char *) val, "%lu", &tmp->rsp);
        } else if (xmlStrcmp(cur->name, (const xmlChar *) "rflags") == 0) {
        	sscanf((const char *) val, "%lu", &tmp->rflags);
        } else if (xmlStrcmp(cur->name, (const xmlChar *) "r8") == 0) {
        	sscanf((const char *) val, "%lu", &tmp->r8);
        } else if (xmlStrcmp(cur->name, (const xmlChar *) "r9") == 0) {
        	sscanf((const char *) val, "%lu", &tmp->r9);
        } else if (xmlStrcmp(cur->name, (const xmlChar *) "r10") == 0) {
        	sscanf((const char *) val, "%lu", &tmp->r10);
        } else if (xmlStrcmp(cur->name, (const xmlChar *) "r11") == 0) {
        	sscanf((const char *) val, "%lu", &tmp->r11);
        } else if (xmlStrcmp(cur->name, (const xmlChar *) "r12") == 0) {
        	sscanf((const char *) val, "%lu", &tmp->r12);
        } else if (xmlStrcmp(cur->name, (const xmlChar *) "r13") == 0) {
        	sscanf((const char *) val, "%lu", &tmp->r13);
        } else if (xmlStrcmp(cur->name, (const xmlChar *) "r14") == 0) {
        	sscanf((const char *) val, "%lu", &tmp->r14);
        } else if (xmlStrcmp(cur->name, (const xmlChar *) "r15") == 0) {
        	sscanf((const char *) val, "%lu", &tmp->r15);
        } else if (xmlStrcmp(cur->name, (const xmlChar *) "debugreg0") == 0) {
        	sscanf((const char *) val, "%lu", &tmp->dr0);
        } else if (xmlStrcmp(cur->name, (const xmlChar *) "debugreg1") == 0) {
        	sscanf((const char *) val, "%lu", &tmp->dr1);
        } else if (xmlStrcmp(cur->name, (const xmlChar *) "debugreg2") == 0) {
        	sscanf((const char *) val, "%lu", &tmp->dr2);
        } else if (xmlStrcmp(cur->name, (const xmlChar *) "debugreg3") == 0) {
        	sscanf((const char *) val, "%lu", &tmp->dr3);
        //} else if (xmlStrcmp(cur->name, (const xmlChar *) "debugreg4") == 0) {
        //} else if (xmlStrcmp(cur->name, (const xmlChar *) "debugreg5") == 0) {
        //} else if (xmlStrcmp(cur->name, (const xmlChar *) "debugreg6") == 0) {
        	sscanf((const char *) val, "%lu", &tmp->dr6);
        } else if (xmlStrcmp(cur->name, (const xmlChar *) "debugreg7") == 0) {
        	sscanf((const char *) val, "%lu", &tmp->dr7);
        } else if (xmlStrcmp(cur->name, (const xmlChar *) "shadow_gs") == 0) {
        	sscanf((const char *) val, "%lu", &tmp->shadow_gs);
        } else if (xmlStrcmp(cur->name, (const xmlChar *) "lstar") == 0) {
        	sscanf((const char *) val, "%lu", &tmp->msr_lstar);
        } else if (xmlStrcmp(cur->name, (const xmlChar *) "star") == 0) {
        	sscanf((const char *) val, "%lu", &tmp->msr_star);
        } else if (xmlStrcmp(cur->name, (const xmlChar *) "cstar") == 0) {
        	sscanf((const char *) val, "%lu", &tmp->msr_cstar);
        } else if (xmlStrcmp(cur->name, (const xmlChar *) "sfmask") == 0) {
        	sscanf((const char *) val, "%lu", &tmp->msr_syscall_mask);
        } else if (xmlStrcmp(cur->name, (const xmlChar *) "guest_cr0") == 0) {
        	sscanf((const char *) val, "%lu", &tmp->cr0);
        //} else if (xmlStrcmp(cur->name, (const xmlChar *) "guest_cr1") == 0) {
        } else if (xmlStrcmp(cur->name, (const xmlChar *) "guest_cr2") == 0) {
        	sscanf((const char *) val, "%lu", &tmp->cr2);
        } else if (xmlStrcmp(cur->name, (const xmlChar *) "guest_cr3") == 0) {
        	sscanf((const char *) val, "%lu", &tmp->cr3);
        } else if (xmlStrcmp(cur->name, (const xmlChar *) "guest_cr4") == 0) {
        	sscanf((const char *) val, "%lu", &tmp->cr4);
        } else if (xmlStrcmp(cur->name, (const xmlChar *) "guest_efer") == 0) {
        	sscanf((const char *) val, "%lu", &tmp->msr_efer);
        } else if (xmlStrcmp(cur->name, (const xmlChar *) "pending_event") == 0) {
        	sscanf((const char *) val, "%u", &tmp->pending_event);
        } else if (xmlStrcmp(cur->name, (const xmlChar *) "error_code") == 0) {
        	sscanf((const char *) val, "%u", &tmp->error_code);
        } else if (xmlStrcmp(cur->name, (const xmlChar *) "msr_flags") == 0) {
        	sscanf((const char *) val, "%lu", &tmp->msr_flags);
        }
		cur = cur->next;
	} while (cur != NULL);

    return tmp;
}

void * extract_from_xml( void * (*f)(xmlNodePtr, void *), xmlDocPtr doc, xmlChar * xpath, void * base)
{
	xmlXPathContextPtr context;
	xmlXPathObjectPtr result;
    xmlNodePtr cur;
    void * res = NULL;
    
    context = xmlXPathNewContext(doc);
    result = xmlXPathEvalExpression(xpath, context);
    if(xmlXPathNodeSetIsEmpty(result->nodesetval)){
	    printf("no result\n");
	} else {
	    for (int k = 0; k < result->nodesetval->nodeNr; k++)
	    {
	        if(result->nodesetval->nodeTab[k]->type == XML_ELEMENT_NODE)
	        {
	            cur = result->nodesetval->nodeTab[k];
	            res = f(cur->xmlChildrenNode, base);	        }
	    }
	}
	
    xmlXPathFreeObject(result);
    xmlXPathFreeContext(context); 
    return res;
}

xmlDocPtr read_xml_file(char * path)
{
    xmlDocPtr doc;
    xmlNodePtr cur;

    doc = xmlParseFile(path);
    if (doc == NULL)
        return NULL;
        
    cur = xmlDocGetRootElement(doc);
    if (cur == NULL) 
    {
        return NULL;
    }
    
    if (xmlStrcmp(cur->name, (const xmlChar *) "vms")) 
    {
        return NULL;
    }
    
    /*cur = cur->xmlChildrenNode;
    while (cur != NULL) {
        if (xmlStrcmp(cur->name, (const xmlChar *) "vm") == 0) {
            printf("%p\n", extract_pmtimer(cur->xmlChildrenNode));
        }
        cur = cur->next;
    }*/
   

    //xmlFreeDoc(doc);
    return doc;
    
}

int read_xml_file2(char * path)
{
    xmlTextReaderPtr reader;
    int ret;
    xmlChar * name;
    
    reader = xmlReaderForFile(path, NULL, 0);
    if (reader == NULL) {
		return -1;
    }
    do {
        ret = xmlTextReaderRead(reader);
        name = xmlTextReaderName(reader);
        printf("name %s\n", name);    
    } while (ret == 1);
    xmlFreeTextReader(reader);
    return 0;
}

int main (int argc, char ** argv)
{
 	char base_file_path[1024], base_xml_file[1024], new_file_path[1024];
 	unsigned int structure, offset;
 	int vm_id;
	struct hvm_hw_pmtimer tmp_pmtimer;
	struct hvm_hw_rtc tmp_rtc;
	struct hvm_hw_cpu tmp_cpu;
	struct hvm_hw_pmtimer * new_pmtimer;
	struct hvm_hw_rtc * new_rtc;
	struct hvm_hw_cpu * new_cpu;
	uint64_t * new_eptp;
    struct xc_sr_record ept_info = { REC_TYPE_HVM_EPTP_INFO, 0, NULL };
    struct xc_sr_rhdr hdr_rec;
    ssize_t read_amount;
    unsigned char * big_buffer;
    void * data_buffer;
    int nRead;

	xmlDocPtr xmlDoc;
	char xpath[50];
	
 	int base_fp, new_fp;
 	if (argc != 6)
 	{
 		printf("Usage: <base save file> <xml file> <structure to be updated> <offset> <vm id>\n");
 		return -1;
 	}
 	
 	memcpy(base_file_path, argv[1], 512);
 	memcpy(base_xml_file, argv[2], 512);
 	sscanf(argv[3], "%d", &structure);
 	sscanf(argv[4], "%d", &offset);
 	sscanf(argv[5], "%d", &vm_id);
 	
 	base_fp = open(base_file_path, O_RDWR);
 	assert(base_fp != -1);
	assert((xmlDoc = read_xml_file(base_xml_file)) != NULL);
	
 	switch (structure) {
 		case 0: /* PM Timer */
			lseek(base_fp, offset, SEEK_SET);
 			assert(read(base_fp, &tmp_pmtimer, sizeof(struct hvm_hw_pmtimer)) > 0);
 			sprintf(xpath, "/vms/vm[%d]/pmtimer", vm_id);
 			//printf("%u %u %u\n", tmp_pmtimer.tmr_val, tmp_pmtimer.pm1a_sts, tmp_pmtimer.pm1a_en);
			new_pmtimer =  extract_from_xml((void *) extract_pmtimer, xmlDoc, BAD_CAST xpath, (void *) &tmp_pmtimer);
			swap_pmtimer(&tmp_pmtimer, new_pmtimer);
			lseek(base_fp, offset, SEEK_SET);
			write(base_fp, &tmp_pmtimer, sizeof(tmp_pmtimer));
			free(new_pmtimer);
			break;
		case 1: /* RTC */
			lseek(base_fp, offset, SEEK_SET);
 			assert(read(base_fp, &tmp_rtc, sizeof(struct hvm_hw_rtc)) > 0);
			/*for (int p = 0; p < RTC_CMOS_SIZE; p++) {
				printf("%u\n", tmp_rtc.cmos_data[p]);
			}
			printf("%u %u\n", tmp_rtc.cmos_index, tmp_rtc.pad0);*/
			sprintf(xpath, "/vms/vm[%d]/rtc", vm_id);
			new_rtc = (struct hvm_hw_rtc *) extract_from_xml((void *) extract_rtc, xmlDoc, BAD_CAST xpath, (void *) &tmp_rtc);
			swap_rtc(&tmp_rtc, new_rtc);
            lseek(base_fp, offset, SEEK_SET);
			write(base_fp, &tmp_rtc, sizeof(tmp_rtc));
			free(new_rtc);
			break;
		case 2: /* CPU */
			lseek(base_fp, offset, SEEK_SET);
 			assert(read(base_fp, &tmp_cpu, sizeof(struct hvm_hw_cpu)) > 0);
 			//printf("%lX\n", tmp_cpu.rip);
			sprintf(xpath, "/vms/vm[%d]/vcpus/vcpu[1]", vm_id);
			new_cpu = (struct hvm_hw_cpu *) extract_from_xml((void *) extract_cpu, xmlDoc, BAD_CAST xpath, (void *) &tmp_cpu);
			swap_cpu(&tmp_cpu, new_cpu);
            lseek(base_fp, offset, SEEK_SET);
			write(base_fp, &tmp_cpu, sizeof(tmp_cpu));
			free(new_cpu);
			break;
		case 3: /* EPTP */
			lseek(base_fp, offset, SEEK_SET);
            big_buffer = malloc(_1GB);
            read_amount = read(base_fp, big_buffer, _1GB);
            printf("%s\n", strerror(errno));
            assert(read_amount > 0);
            
        	lseek(base_fp, offset, SEEK_SET);
        	
			sprintf(xpath, "/vms/vm[%d]", vm_id);
			new_eptp = (uint64_t *) extract_from_xml((void *) extract_eptp, xmlDoc, BAD_CAST xpath, NULL);
            ept_info.data = malloc(sizeof(uint64_t));
            memcpy(ept_info.data, new_eptp, sizeof(uint64_t));
            ept_info.length = sizeof(uint64_t);
			printf("New EPTP is %lX %lX\n", *new_eptp, *(uint64_t *) ept_info.data);

            write(base_fp, &ept_info.type, sizeof(ept_info.type));
            write(base_fp, &ept_info.length, sizeof(ept_info.length));
            write(base_fp, ept_info.data, sizeof(ept_info.data));
            //memset(big_buffer, 'A', read_amount);
            write(base_fp, big_buffer, read_amount);
            free(new_eptp);
            free(big_buffer);
		    break;
		case 4: /* Remove all page data entries from xc stream */
		    // Offset is the beginning of the xc stream.
		    sprintf(new_file_path, "%s-stripped.save", base_file_path);
		    printf("Creating stripped version in %s\n", new_file_path);
		    remove(new_file_path);
		    new_fp = open(new_file_path, O_RDWR | O_CREAT);
		    
		    data_buffer = malloc(offset);
			assert (read(base_fp, data_buffer, offset) == offset);
			write(new_fp, data_buffer, offset);
		    free(data_buffer);

    		while (1)
			{
				assert (read(base_fp, &hdr_rec, sizeof(struct xc_sr_rhdr)) == sizeof(struct xc_sr_rhdr));

		        data_buffer = malloc(hdr_rec.length);

		        assert(read(base_fp, data_buffer, hdr_rec.length) == hdr_rec.length);
		        
		        if (hdr_rec.type != REC_TYPE_PAGE_DATA) 
		        {
					write(new_fp, &hdr_rec, sizeof(struct xc_sr_rhdr));
		        	write(new_fp, data_buffer, hdr_rec.length);
		        }

		        free(data_buffer);

                if (hdr_rec.type == REC_TYPE_END)
    				break;
			}
			data_buffer = malloc(1024);
			while (nRead = read(base_fp, data_buffer, 1024), nRead != 0)
			{
				write(new_fp, data_buffer, nRead);
			}
			free(data_buffer);

			fdatasync(new_fp);
			close(new_fp);
			break;
		default:
 			printf("Unknown structure\n");
 			return -2;
 	} 	 
 	printf("%s %s %d %d\n", base_file_path, base_xml_file, structure, offset);
    fdatasync(base_fp);
	close(base_fp);
	return 0;
}
