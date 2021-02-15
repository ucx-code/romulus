Being a PoC this code has various limitations that make it not advisable to be used in production.
Below we present a list and explanation for the (known) limitations.

#### L2 VM can only have 1 vCPU
This limitation is solely due to the code of inspect_savefile, vmstate_extractor and update_savefile not having been coded with support for more than 1 vCPU. It should be possible to somewhat easily had this support.

### L1 VM can only have 1 vCPU
Although it is perfectly possible to perform migration when the L1's have > 1 vCPU (we tested up to 12 vCPUs) a problem arises in a specific scenario. When a L1 VMs has many vCPUs and the L2 VM is under intense CPU load there is a high (the more cores the higher) probability that the L2 VM will not be sucessfully migrated to L1 B. The migration process itself finishes correctly but the L2 VM in L1 B normally enters an infinite loop/hang without any error message. The suspected reason for this behaviour has to do with how vCPUs are scheduled and how the register context is stored to memory.

Our migration process reads the register values from a memory structure (struct cpu_user_regs of struct arch_vcpu, which is kept for each vCPU) kept by Xen. This structure is updated (overwritten) whenever there is a context switch between domains (in function __context_switch). Therefore if a migration takes places while the vCPU of the L2 is under full load, it is highly likely that there will be a significant discrepancy between the real register state (wrt to L2) and the state kept in memory. Having a L1 with just 1vCPU forces the L1 and L2 to share it and increases the amount of times that a domain is switched. For this reason, we can theorize (have not tested yet) that smaller scheduler timeslices will help to increase the migration sucess.

During the migration process we are already using an approach to pause the L2 VM from running in L1 A (assuming that the L1 A has not crashed completely), however it seems to not be 100% effective, as it might lead to no context_switch being called and hence it wont update the register state to memory.

