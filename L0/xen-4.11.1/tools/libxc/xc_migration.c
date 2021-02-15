#include "xc_private.h"
#include "xc_core.h"
#include "xg_private.h"
#include "xg_save_restore.h"
#include <xen/memory.h>
#include <xen/hvm/hvm_op.h>

int xc_save_nvmcs(xc_interface *xch, vmcs_save_state_t * state, int step, unsigned long domid)
{
   int ret = -1;
   DECLARE_HYPERCALL_BUFFER(vmcs_save_state_t, local);
   local = xc_hypercall_buffer_alloc(xch, local, sizeof(*local));
   if (state == NULL) {
      printf("Buffer alloc failed\n");
      return -1;
   }
   ret = xencall3(xch->xcall, __HYPERVISOR_save_nvmcs, HYPERCALL_BUFFER_AS_ARG(local), step, domid);

   if (step == 2)
      memcpy(state, local, sizeof(*local));
   xc_hypercall_buffer_free(xch, local);
   return ret;
}

