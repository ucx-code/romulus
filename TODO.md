TODO list:
* Support for superpages (2Mb or 1Gb pages)
* ~~Add options to enable/disable performance monitoring (times, count number of pages)~~
* Migration performance improvements
* Fix "Netdev watchdog: transmit queue 0 timed out" error message which appears at L2 Linux VM after a migration
* Support for multi-CPU L1 VMs
* Support for multi-CPU L2 VMs
* Improve TSC, HPET, APIC support
* Support for migrating more than 1 L2 VM
* Fix unaccounted pages which confuse L0 Xen (page count_info is not 0)
* Avoid the need for forcefully pausing L1 A prior to a live migration
* Add support for pausing L1 B until it is required
* Add support to expand vCPU and memory of L1 B when required
* Add support for PVHVM (or PV on HVM drivers), avoiding the need for xen_nopv boot flag for Linux
