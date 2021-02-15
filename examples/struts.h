#ifndef _STRUTS_H_
#define _STRUTS_H_

#define ROUNDUP(_x,_w) (((unsigned long)(_x)+(1UL<<(_w))-1) & ~((1UL<<(_w))-1))

/* ================ libxl migrate stream v2 =============== */
#define RESTORE_STREAM_IDENT         0x4c6962786c466d74UL
#define RESTORE_STREAM_VERSION       0x00000002U

/* Stream header - from libxl/libxl_sr_stream_format.h */
typedef struct libxl__sr_hdr
{
    uint64_t ident;
    uint32_t version;
    uint32_t options;
} libxl__sr_hdr;

/* Stream record header - from libxl/libxl_sr_stream_format.h */
typedef struct libxl__sr_rec_hdr
{
    uint32_t type;
    uint32_t length;
} libxl__sr_rec_hdr;


/* libxl types */
#define REC_TYPE_END                    0x00000000U
#define REC_TYPE_LIBXC_CONTEXT          0x00000001U
#define REC_TYPE_EMULATOR_XENSTORE_DATA 0x00000002U
#define REC_TYPE_EMULATOR_CONTEXT       0x00000003U
#define REC_TYPE_CHECKPOINT_END         0x00000004U
#define REC_TYPE_CHECKPOINT_STATE       0x00000005U

/* libxc types */
#define REC_TYPE_PAGE_DATA                  0x00000001U
#define REC_TYPE_X86_PV_INFO                0x00000002U
#define REC_TYPE_X86_PV_P2M_FRAMES          0x00000003U
#define REC_TYPE_X86_PV_VCPU_BASIC          0x00000004U
#define REC_TYPE_X86_PV_VCPU_EXTENDED       0x00000005U
#define REC_TYPE_X86_PV_VCPU_XSAVE          0x00000006U
#define REC_TYPE_SHARED_INFO                0x00000007U
#define REC_TYPE_TSC_INFO                   0x00000008U
#define REC_TYPE_HVM_CONTEXT                0x00000009U
#define REC_TYPE_HVM_PARAMS                 0x0000000aU
#define REC_TYPE_TOOLSTACK                  0x0000000bU
#define REC_TYPE_X86_PV_VCPU_MSRS           0x0000000cU
#define REC_TYPE_VERIFY                     0x0000000dU
#define REC_TYPE_CHECKPOINT                 0x0000000eU
#define REC_TYPE_CHECKPOINT_DIRTY_PFN_LIST  0x0000000fU
#define REC_TYPE_HVM_EPTP_INFO              0x00000010U


/* ================ libxc migrate stream v2 =============== */
/* Record Header - from xc/xc_sr_stream_format.h */
struct xc_sr_rhdr
{
    uint32_t type;
    uint32_t length;
};

/* Domain Header - from xc/xc_sr_stream_format.h */
struct xc_sr_dhdr
{
    uint32_t type;
    uint16_t page_shift;
    uint16_t _res1;
    uint32_t xen_major;
    uint32_t xen_minor;
};

/* Image Header - from xc/xc_sr_stream_format.h */
struct xc_sr_ihdr
{
    uint64_t marker;
    uint32_t id;
    uint32_t version;
    uint16_t options;
    uint16_t _res1;
    uint32_t _res2;
};

/* From xc/xc_sr_common.h */
struct xc_sr_record
{
    uint32_t type;
    uint32_t length;
    void *data;
};
/* ============================================================== */

struct hvm_hw_cpu {
    uint8_t  fpu_regs[512];

    uint64_t rax;
    uint64_t rbx;
    uint64_t rcx;
    uint64_t rdx;
    uint64_t rbp;
    uint64_t rsi;
    uint64_t rdi;
    uint64_t rsp;
    uint64_t r8;
    uint64_t r9;
    uint64_t r10;
    uint64_t r11;
    uint64_t r12;
    uint64_t r13;
    uint64_t r14;
    uint64_t r15;

    uint64_t rip;
    uint64_t rflags;

    uint64_t cr0;
    uint64_t cr2;
    uint64_t cr3;
    uint64_t cr4;

    uint64_t dr0;
    uint64_t dr1;
    uint64_t dr2;
    uint64_t dr3;
    uint64_t dr6;
    uint64_t dr7;    

    uint32_t cs_sel;
    uint32_t ds_sel;
    uint32_t es_sel;
    uint32_t fs_sel;
    uint32_t gs_sel;
    uint32_t ss_sel;
    uint32_t tr_sel;
    uint32_t ldtr_sel;

    uint32_t cs_limit;
    uint32_t ds_limit;
    uint32_t es_limit;
    uint32_t fs_limit;
    uint32_t gs_limit;
    uint32_t ss_limit;
    uint32_t tr_limit;
    uint32_t ldtr_limit;
    uint32_t idtr_limit;
    uint32_t gdtr_limit;

    uint64_t cs_base;
    uint64_t ds_base;
    uint64_t es_base;
    uint64_t fs_base;
    uint64_t gs_base;
    uint64_t ss_base;
    uint64_t tr_base;
    uint64_t ldtr_base;
    uint64_t idtr_base;
    uint64_t gdtr_base;

    uint32_t cs_arbytes;
    uint32_t ds_arbytes;
    uint32_t es_arbytes;
    uint32_t fs_arbytes;
    uint32_t gs_arbytes;
    uint32_t ss_arbytes;
    uint32_t tr_arbytes;
    uint32_t ldtr_arbytes;

    uint64_t sysenter_cs;
    uint64_t sysenter_esp;
    uint64_t sysenter_eip;

    /* msr for em64t */
    uint64_t shadow_gs;

    /* msr content saved/restored. */
    uint64_t msr_flags; /* Obsolete, ignored. */
    uint64_t msr_lstar;
    uint64_t msr_star;
    uint64_t msr_cstar;
    uint64_t msr_syscall_mask;
    uint64_t msr_efer;
    uint64_t msr_tsc_aux;

    /* guest's idea of what rdtsc() would return */
    uint64_t tsc;

    /* pending event, if any */
    union {
        uint32_t pending_event;
        struct {
            uint8_t  pending_vector:8;
            uint8_t  pending_type:3;
            uint8_t  pending_error_valid:1;
            uint32_t pending_reserved:19;
            uint8_t  pending_valid:1;
        };
    };
    /* error code for pending event */
    uint32_t error_code;

#define _XEN_X86_FPU_INITIALISED        0
#define XEN_X86_FPU_INITIALISED         (1U<<_XEN_X86_FPU_INITIALISED)
    uint32_t flags;
    uint32_t pad0;
};

struct hvm_hw_pmtimer {
    uint32_t tmr_val;   /* PM_TMR_BLK.TMR_VAL: 32bit free-running counter */
    uint16_t pm1a_sts;  /* PM1a_EVT_BLK.PM1a_STS: status register */
    uint16_t pm1a_en;   /* PM1a_EVT_BLK.PM1a_EN: enable register */
};

#define RTC_CMOS_SIZE 14
struct hvm_hw_rtc {
    /* CMOS bytes */
    uint8_t cmos_data[RTC_CMOS_SIZE];
    /* Index register for 2-part operations */
    uint8_t cmos_index;
    uint8_t pad0;
};

#endif