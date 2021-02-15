#brctl addbr br0;
#brctl addif br0 ens4;
#ifconfig ens4 0.0.0.0;
#ifconfig br0 up;
#dhclient br0;

mount -o noacl,nocto,noatime,nodiratime 192.168.66.5:/xen /nfs
#xl create smallvm4.cfg
xl create /nfs/configs/a.cfg
#xl create /nfs/configs/l2_debian_2.cfg
#xl create /nfs/configs/l2_freebsd.cfg
#xl create /nfs/configs/l2_openbsd.cfg
