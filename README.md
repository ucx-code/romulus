
## Login information

| VM | User | Password
| :---: | :---: | :---: |
| L1_A | root | faultinjection |
| L1_B | root | faultinjection |
| L2 | root | faultinjection |

## VM information
Below we present information about the VMs that have been used for testing.

### L0

### L1

| Name | Distro | Kernel version | Xen version | Disk size | Boot properties | Bridge name | MAC addr. | Def. IP |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| L1_A | Centos 7 (1908 minimal) | 4.14.89 (compiled) | 4.11.1 | 20G | xen: debugtrace=256 serial_tx_buffer=40960k hap_1gb=0 hap_2mb=0 loglvl=all guest_loglvl=all com1=115200,8n1,0x2f8,3 console=com1 linux: console=hvc0 earlyprintk=xen | 00:17:4e:7c:ac:a9 | 192.168.66.102 | 
| L1_B | Centos 7 (1908 minimal) | 4.14.89 (compiled) | 4.11.1 | 20G | xen: debugtrace=256 serial_tx_buffer=40960k hap_1gb=0 hap_2mb=0 loglvl=all guest_loglvl=all com1=115200,8n1,0x2f8,3 console=com1 linux: console=hvc0 earlyprintk=xen | 00:17:4e:7c:ac:e9 | 192.168.66.103 | 
 
### L2

| Name | Distro | Kernel version | Xen version | Disk size | Boot properties | Bridge name | MAC addr. | Def. IP |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| L2 | Debian | 4.19.0-8-amd64 | None | 6G |  linux: xen_nopv | None |  00:17:4e:7c:ac:01 | 192.168.66.99 |