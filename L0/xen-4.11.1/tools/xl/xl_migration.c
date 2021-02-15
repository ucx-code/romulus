#include <fcntl.h>
#include <inttypes.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <time.h>
#include <unistd.h>

#include <libxl.h>
#include <libxl_utils.h>
#include <libxlutil.h>

#include "xl.h"
#include "xl_utils.h"
#include "xl_parse.h"

#include <libxml/parser.h>
#include <libxml/tree.h>

#define BUF_SIZE 21
// TODO: Avoid repeated struct
struct vmcs_save_state_xl {
                unsigned char ready; 
                unsigned long eptp, domid;
                unsigned long sysenter_cs, sysenter_esp, sysenter_eip;
                unsigned long es_sel, es_limit, es_base, es_arbytes;
                unsigned long cs_sel, cs_limit, cs_base, cs_arbytes;
                unsigned long ss_sel, ss_limit, ss_base, ss_arbytes;
                unsigned long ds_sel, ds_limit, ds_base, ds_arbytes;
                unsigned long fs_sel, fs_limit, fs_base, fs_arbytes;
                unsigned long gs_sel, gs_limit, gs_base, gs_arbytes;
                unsigned long tr_sel, tr_limit, tr_base, tr_arbytes;
                unsigned long ldtr_sel, ldtr_limit, ldtr_base, ldtr_arbytes;
                unsigned long idtr_limit, idtr_base;
                unsigned long  gdtr_limit, gdtr_base;
        };

int main_save_nvmcs(int argc, char **argv)
{
	struct vmcs_save_state_xl s;
	int step;
	unsigned int domid;
	xmlDocPtr doc = NULL;
	xmlNodePtr root = NULL;
	xmlChar buf[BUF_SIZE];
	LIBXML_TEST_VERSION;

	if (argc != 3)
	{
		fprintf(stderr, "Expecting two arguments (step, domid)\n");
		return -1;
	}

	sscanf(argv[1], "%d", &step);
	sscanf(argv[2], "%u", &domid);
	libxl_save_nvmcs_state(ctx, (void *) &s, step, domid);

	if (step == 2) /* Obtain saved state */
	{
		doc = xmlNewDoc(BAD_CAST "1.0");
		root = xmlNewNode(NULL, BAD_CAST "nvmcs");
		xmlDocSetRootElement(doc, root);

		xmlStrPrintf(buf, BUF_SIZE, "%lu", s.sysenter_cs);
		xmlNewChild(root, NULL, BAD_CAST "sysenter_cs", buf);

		xmlStrPrintf(buf, BUF_SIZE, "%lu", s.sysenter_esp);
		xmlNewChild(root, NULL, BAD_CAST "sysenter_esp", buf);

		xmlStrPrintf(buf, BUF_SIZE, "%lu", s.sysenter_eip);
		xmlNewChild(root, NULL, BAD_CAST "sysenter_eip", buf);

		xmlStrPrintf(buf, BUF_SIZE, "%lu", s.es_sel);
		xmlNewChild(root, NULL, BAD_CAST "es_sel", buf);

		xmlStrPrintf(buf, BUF_SIZE, "%lu", s.cs_sel);
		xmlNewChild(root, NULL, BAD_CAST "cs_sel", buf);

		xmlStrPrintf(buf, BUF_SIZE, "%lu", s.ss_sel);
		xmlNewChild(root, NULL, BAD_CAST "ss_sel", buf);

		xmlStrPrintf(buf, BUF_SIZE, "%lu", s.ds_sel);
		xmlNewChild(root, NULL, BAD_CAST "ds_sel", buf);

		xmlStrPrintf(buf, BUF_SIZE, "%lu", s.fs_sel);
		xmlNewChild(root, NULL, BAD_CAST "fs_sel", buf);

		xmlStrPrintf(buf, BUF_SIZE, "%lu", s.gs_sel);
		xmlNewChild(root, NULL, BAD_CAST "gs_sel", buf);

		xmlStrPrintf(buf, BUF_SIZE, "%lu", s.tr_sel);
		xmlNewChild(root, NULL, BAD_CAST "tr_sel", buf);

		xmlStrPrintf(buf, BUF_SIZE, "%lu", s.ldtr_sel);
		xmlNewChild(root, NULL, BAD_CAST "ldtr_sel", buf);

		xmlStrPrintf(buf, BUF_SIZE, "%lu", s.es_limit);
		xmlNewChild(root, NULL, BAD_CAST "es_limit", buf);

		xmlStrPrintf(buf, BUF_SIZE, "%lu", s.cs_limit);
		xmlNewChild(root, NULL, BAD_CAST "cs_limit", buf);

		xmlStrPrintf(buf, BUF_SIZE, "%lu", s.ss_limit);
		xmlNewChild(root, NULL, BAD_CAST "ss_limit", buf);

		xmlStrPrintf(buf, BUF_SIZE, "%lu", s.ds_limit);
		xmlNewChild(root, NULL, BAD_CAST "ds_limit", buf);

		xmlStrPrintf(buf, BUF_SIZE, "%lu", s.fs_limit);
		xmlNewChild(root, NULL, BAD_CAST "fs_limit", buf);

		xmlStrPrintf(buf, BUF_SIZE, "%lu", s.gs_limit);
		xmlNewChild(root, NULL, BAD_CAST "gs_limit", buf);

		xmlStrPrintf(buf, BUF_SIZE, "%lu", s.tr_limit);
		xmlNewChild(root, NULL, BAD_CAST "tr_limit", buf);

		xmlStrPrintf(buf, BUF_SIZE, "%lu", s.ldtr_limit);
		xmlNewChild(root, NULL, BAD_CAST "ldtr_limit", buf);

		xmlStrPrintf(buf, BUF_SIZE, "%lu", s.idtr_limit);
		xmlNewChild(root, NULL, BAD_CAST "idtr_limit", buf);

		xmlStrPrintf(buf, BUF_SIZE, "%lu", s.gdtr_limit);
		xmlNewChild(root, NULL, BAD_CAST "gdtr_limit", buf);

		xmlStrPrintf(buf, BUF_SIZE, "%lu", s.es_base);
		xmlNewChild(root, NULL, BAD_CAST "es_base", buf);

		xmlStrPrintf(buf, BUF_SIZE, "%lu", s.cs_base);
		xmlNewChild(root, NULL, BAD_CAST "cs_base", buf);

		xmlStrPrintf(buf, BUF_SIZE, "%lu", s.ss_base);
		xmlNewChild(root, NULL, BAD_CAST "ss_base", buf);

		xmlStrPrintf(buf, BUF_SIZE, "%lu", s.ds_base);
		xmlNewChild(root, NULL, BAD_CAST "ds_base", buf);

		xmlStrPrintf(buf, BUF_SIZE, "%lu", s.fs_base);
		xmlNewChild(root, NULL, BAD_CAST "fs_base", buf);

		xmlStrPrintf(buf, BUF_SIZE, "%lu", s.gs_base);
		xmlNewChild(root, NULL, BAD_CAST "gs_base", buf);

		xmlStrPrintf(buf, BUF_SIZE, "%lu", s.tr_base);
		xmlNewChild(root, NULL, BAD_CAST "tr_base", buf);

		xmlStrPrintf(buf, BUF_SIZE, "%lu", s.ldtr_base);
		xmlNewChild(root, NULL, BAD_CAST "ldtr_base", buf);

		xmlStrPrintf(buf, BUF_SIZE, "%lu", s.idtr_base);
		xmlNewChild(root, NULL, BAD_CAST "idtr_base", buf);

		xmlStrPrintf(buf, BUF_SIZE, "%lu", s.gdtr_base);
		xmlNewChild(root, NULL, BAD_CAST "gdtr_base", buf);

		xmlStrPrintf(buf, BUF_SIZE, "%lu", s.es_arbytes);
		xmlNewChild(root, NULL, BAD_CAST "es_arbytes", buf);

		xmlStrPrintf(buf, BUF_SIZE, "%lu", s.cs_arbytes);
		xmlNewChild(root, NULL, BAD_CAST "cs_arbytes", buf);

		xmlStrPrintf(buf, BUF_SIZE, "%lu", s.ss_arbytes);
		xmlNewChild(root, NULL, BAD_CAST "ss_arbytes", buf);

		xmlStrPrintf(buf, BUF_SIZE, "%lu", s.ds_arbytes);
		xmlNewChild(root, NULL, BAD_CAST "ds_arbytes", buf);

		xmlStrPrintf(buf, BUF_SIZE, "%lu", s.fs_arbytes);
		xmlNewChild(root, NULL, BAD_CAST "fs_arbytes", buf);

		xmlStrPrintf(buf, BUF_SIZE, "%lu", s.gs_arbytes);
		xmlNewChild(root, NULL, BAD_CAST "gs_arbytes", buf);

		xmlStrPrintf(buf, BUF_SIZE, "%lu", s.tr_arbytes);
		xmlNewChild(root, NULL, BAD_CAST "tr_arbytes", buf);

		xmlStrPrintf(buf, BUF_SIZE, "%lu", s.ldtr_arbytes);
		xmlNewChild(root, NULL, BAD_CAST "ldtr_arbytes", buf);



		xmlSaveFormatFileEnc("-", doc, "UTF-8", 1);
		xmlFreeDoc(doc);
		xmlCleanupParser();
		xmlMemoryDump();
	}
	return 0;	
}
