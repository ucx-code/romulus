
ssh root@192.168.66.101 "xl create /nfs/configs/acentosbig-2.cfg"
sleep 40
ssh root@192.168.66.101 "DEST=/nfs/p.save-1 bash create_savefile.sh 1 aa"
sleep 5
ssh root@192.168.66.3 "xl save-nvmcs 1 1"

ssh root@192.168.66.101 "xl create /nfs/configs/acentosbig-3.cfg"
sleep 40
ssh root@192.168.66.101 "DEST=/nfs/p.save-2 bash create_savefile.sh 2 aa"
sleep 5
ssh root@192.168.66.3 "xl save-nvmcs 1 2"

ssh root@192.168.66.101 "xl create /nfs/configs/acentosbig-4.cfg"
sleep 40
ssh root@192.168.66.101 "DEST=/nfs/p.save-3 bash create_savefile.sh 3 aa"
sleep 5
ssh root@192.168.66.3 "xl save-nvmcs 1 3"

ssh root@192.168.66.101 "xl create /nfs/configs/acentosbig-5.cfg"
sleep 40
ssh root@192.168.66.101 "DEST=/nfs/p.save-4 bash create_savefile.sh 4 aa"
sleep 5
ssh root@192.168.66.3 "xl save-nvmcs 1 4"

ssh root@192.168.66.101 "xl create /nfs/configs/acentosbig-6.cfg"
sleep 40
ssh root@192.168.66.101 "DEST=/nfs/p.save-5 bash create_savefile.sh 5 aa"
sleep 5
ssh root@192.168.66.3 "xl save-nvmcs 1 5"

ssh root@192.168.66.101 "xl create /nfs/configs/acentosbig-7.cfg"
sleep 40
ssh root@192.168.66.101 "DEST=/nfs/p.save-6 bash create_savefile.sh 6 aa"
sleep 5
ssh root@192.168.66.3 "xl save-nvmcs 1 6"

