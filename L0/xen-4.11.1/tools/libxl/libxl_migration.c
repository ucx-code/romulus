#include "libxl_osdeps.h"

#include "libxl_internal.h"

int libxl_save_nvmcs_state(libxl_ctx * ctx, void * state, int step, unsigned int domid)
{
    int  rc = xc_save_nvmcs(ctx->xch, state, step, domid);
    return rc;
}
