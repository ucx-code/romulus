# TODO: Check if variables are defined
CSV_MASTER_FILE=$1
MIGRATION_SLEEP_TIME=$2
INJ_REG=$3
INJ_BIT=$4
INJ_SLEEP=$5

# Launch VMs
echo "Launching L1 VMs @ L0"
ssh root@kahvefali "xl destroy $L1A_DOMNAME; xl destroy $L1B_DOMNAME; mount -o hard,noacl,nocto,noatime,nodiratime  192.168.66.12:/root/nfs /nfs; xl create /var/lib/nova/instances/nestedvirt/new_image/new_debian2.cfg; xl create /var/lib/nova/instances/nestedvirt/new_image/new_debian.cfg;"
#ssh root@kahvefali "xl destroy $L1A_DOMNAME; xl destroy $L1B_DOMNAME; xl create /var/lib/nova/instances/nestedvirt/new_image/new_debian2.cfg; xl create /var/lib/nova/instances/nestedvirt/new_image/new_debian.cfg;"

RDTSC=`ssh root@kahvefali "/root/print_rdtsc"`
echo "Got RDTSC @ $RDTSC"
DOLMA_TSC=$(date +%s%3N)
echo "In dolma @ $DOLMA_TSC"

LAUNCH_L1_START=$(date +%s%3N)
# Wait for VMs to start
bash ssh-retry.sh root@$L1B_IP pwd &>/dev/null
bash ssh-retry.sh root@$L1A_IP pwd &>/dev/null
LAUNCH_L1_END=$(date +%s%3N)
LAUNCH_L1_DUR=$((LAUNCH_L1_END - LAUNCH_L1_START)) 
echo "Launching L1s took $LAUNCH_L1_DUR"

L1A_ID=`ssh root@kahvefali "xl domid $L1A_DOMNAME"`
L1B_ID=`ssh root@kahvefali "xl domid $L1B_DOMNAME"`

# Alloc memory space
echo "Allocating memory @ L1B and spawing L2 VM @ L1A"
#ssh root@$L1B_IP "mount -o noacl,nocto,noatime,nodiratime  192.168.66.3:/var/lib/nova/instances/nfs_server/ /nfs; bash alloc.sh"
ssh root@$L1B_IP "bash alloc.sh"

# Launch L2 VM
#ssh root@$L1A_IP "bash spawn.sh"
ssh root@$L1A_IP "mount -o noacl,nocto,noatime,nodiratime,nolock  192.168.66.12:/root/nfs /nfs; xl create /nfs/configs/acentosbig-2.cfg;"
# SW FAULTS ssh root@$L1A_IP "mount -o noacl,nocto,noatime,nodiratime  192.168.66.12:/root/nfs /nfs; xl create /nfs/configs/acentosbig-2.cfg;"
#ssh root@$L1A_IP "mount -o noacl,nocto,noatime,nodiratime  192.168.66.3:/var/lib/nova/instances/nfs_server/ /nfs; xl create /nfs/configs/acentosbig-2.cfg;"
sleep 3; ssh root@$L0_IP "xl save-nvmcs 1 1; bash /root/wait_for_nvmcs.sh 1;"
ssh root@$L1A_IP "xl create /nfs/configs/acentosbig-3.cfg;"
sleep 3; ssh root@$L0_IP "xl save-nvmcs 1 2; bash /root/wait_for_nvmcs.sh 2;"
ssh root@$L1A_IP "xl create /nfs/configs/acentosbig-4.cfg;"
sleep 3; ssh root@$L0_IP "xl save-nvmcs 1 3; bash /root/wait_for_nvmcs.sh 3;"
ssh root@$L1A_IP "xl create /nfs/configs/acentosbig-5.cfg;"
sleep 3; ssh root@$L0_IP "xl save-nvmcs 1 4; bash /root/wait_for_nvmcs.sh 4;"



#ssh root@$L1A_IP "xl create /nfs/configs/acentosbig-6.cfg;"
#sleep 40; ssh root@$L0_IP "xl save-nvmcs 1 5"
#ssh root@$L1A_IP "xl create /nfs/configs/acentosbig-7.cfg;"
#sleep 40; ssh root@$L0_IP "xl save-nvmcs 1 6"

echo "Waiting for SSH"
sleep 50
bash ssh-retry.sh root@192.168.66.104 "true"
ssh root@192.168.66.104 "service solr start; "  &

bash ssh-retry.sh root@192.168.66.105 "true"
ssh root@192.168.66.105 "service solr start; "  &

bash ssh-retry.sh root@192.168.66.106 "true"
ssh root@192.168.66.106 "service solr start; "  &

bash ssh-retry.sh root@192.168.66.107 "true"
ssh root@192.168.66.107 "service solr start; "  &

#bash ssh-retry.sh root@192.168.66.108 "true"
#ssh root@192.168.66.108 "service solr start; "  &

#bash ssh-retry.sh root@192.168.66.109 "true"
#ssh root@192.168.66.109 "service solr start; "  &

# Launch SAR probe on L2
echo "Launching SAR probe @ L2"
#ssh root@192.168.66.104 "service solr start; rm /root/sar_log.bin; sar -o /root/sar_log.bin 1 >/dev/null 2>&1 &" &> /dev/null
wait
 
#ssh root@$L2_IP "rm /root/sar_log.bin; sar -o /root/sar_log.bin 1 >/dev/null 2>&1 &"
# Wait for L2 to start and to stabilize
echo "RESET COV NOW"
sleep 100 # was 250

# Create base save file
CREATE_BASE_SAVE_START=$(date +%s%3N)
echo "Creating base save file started @ $CREATE_BASE_SAVE_START"
ssh root@$L1A_IP "DEST=/nfs/d.save-1 bash create_savefile.sh 1 /tmp/save.tmp; rm /tmp/save.tmp; sleep 1;"
ssh root@$L1A_IP "DEST=/nfs/d.save-2 bash create_savefile.sh 2 /tmp/save.tmp; rm /tmp/save.tmp; sleep 1;"
ssh root@$L1A_IP "DEST=/nfs/d.save-3 bash create_savefile.sh 3 /tmp/save.tmp; rm /tmp/save.tmp; sleep 1;"
ssh root@$L1A_IP "DEST=/nfs/d.save-4 bash create_savefile.sh 4 /tmp/save.tmp; rm /tmp/save.tmp; sleep 1;"
#ssh root@$L1A_IP "DEST=/nfs/d.save-5 bash create_savefile.sh 5 /tmp/save.tmp; rm /tmp/save.tmp; sleep 1;"
#ssh root@$L1A_IP "DEST=/nfs/d.save-6 bash create_savefile.sh 6 /tmp/save.tmp; rm /tmp/save.tmp; sleep 1;"
CREATE_BASE_SAVE_END=$(date +%s%3N)
CREATE_BASE_SAVE_DUR=$((CREATE_BASE_SAVE_END - CREATE_BASE_SAVE_START))
echo "Creating base save took $CREATE_BASE_SAVE_DUR"
echo "Creating base save file ended @ $CREATE_BASE_SAVE_END"

sleep 10 # was 70

echo "Launching monitors"
# Launching VM monitor for L1 and L0
python crash_detector.py $L1A_IP 0.4 800 ping_${L1A_IP}.csv &
PING_L1A_PID=$!
python crash_detector.py $L1B_IP 0.4 800 ping_${L1B_IP}.csv &
PING_L1B_PID=$!
python crash_detector.py $L0_IP 0.4 800 ping_${L0_IP}.csv &
PING_L0_PID=$!
python crash_detector.py 192.168.66.105 0.4 800 ping_192.168.66.105.csv &
PING_L2_2_PID=$!
python crash_detector.py 192.168.66.106 0.4 800 ping_192.168.66.106.csv &
PING_L2_3_PID=$!
python crash_detector.py 192.168.66.107 0.4 800 ping_192.168.66.107.csv &
PING_L2_4_PID=$!
#python crash_detector.py 192.168.66.108 0.4 800 ping_192.168.66.108.csv &
#PING_L2_5_PID=$!
#python crash_detector.py 192.168.66.109 0.4 800 ping_192.168.66.109.csv &
#PING_L2_6_PID=$!

#python crash_detector.py 192.168.66.104 0.4 800 ping_192.168.66.104.csv &
#PING_L2_1_PID=$!

# Launching VM monitor to detect crashes
bash launch_crash_detector_for_group.sh 192.168.66.104 $L0_FLOW_GROUP $L1A_ID $L1B_ID $L1B_IP &
CRSH_DET_PID=$!

# Wait to let workload run

WL_TSTAMP=$(date +%s%3N)
echo "Start workload @ $WL_TSTAMP"
#python ./client.py $L2_IP $NCLIENTS $CLIENT_WAITTIME $MIN_REQ $MAX_REQ &
python ./client_by_time.py 192.168.66.104 $NCLIENTS $CLIENT_WAITTIME $END_TIME dg1.csv &
WL_PID1=$!
python ./client_by_time.py 192.168.66.105 $NCLIENTS $CLIENT_WAITTIME $END_TIME dg2.csv &
WL_PID2=$!
python ./client_by_time.py 192.168.66.106 $NCLIENTS $CLIENT_WAITTIME $END_TIME dg3.csv &
WL_PID3=$!
python ./client_by_time.py 192.168.66.107 $NCLIENTS $CLIENT_WAITTIME $END_TIME dg4.csv &
WL_PID4=$!
#python ./client_by_time.py 192.168.66.108 $NCLIENTS $CLIENT_WAITTIME $END_TIME dg5.csv &
#WL_PID5=$!
#python ./client_by_time.py 192.168.66.109 $NCLIENTS $CLIENT_WAITTIME $END_TIME dg6.csv &
#WL_PID6=$!

# Injeting a fault
sleep $INJ_SLEEP
echo "Just slept $INJ_SLEEP"
FI_TSTAMP=$(date +%s%3N)
echo "Injecting a fault @ $FI_TSTAMP"
ssh root@kahvefali "xl inject-fault $L1A_ID $INJ_BIT $INJ_REG"


#===========================
#echo "Migrating L2 between L1s"
#FLOW_MIGRATION_START=$(date +%s%3N)
#ssh root@kahvefali "bash $L0_FLOW_GROUP $L1A_ID $L1B_ID /nfs/d.save"
#FLOW_MIGRATION_END=$(date +%s%3N)
#FLOW_MIGRATION_DUR=$((FLOW_MIGRATION_END - FLOW_MIGRATION_START))
#echo "Flow.sh migration took $FLOW_MIGRATION_DUR"

#RESTORE_START=$(date +%s%3N)
#ssh root@$L1B_IP "xl restore /nfs/d.save-1; xl restore /nfs/d.save-2; xl restore /nfs/d.save-3; xl restore /nfs/d.save-4; xl restore /nfs/d.save-5; xl restore /nfs/d.save-6;"
#RESTORE_END=$(date +%s%3N)
#RESTORE_DUR=$((RESTORE_END - RESTORE_START))
#echo "Restore took $RESTORE_DUR"
#======================

# Wait for wl to finish
wait $WL_PID1
wait $WL_PID2
wait $WL_PID3
wait $WL_PID4
#wait $WL_PID5
#wait $WL_PID6
echo "Workload has finished"

##### Verify VM state
python ./client_by_time.py 192.168.66.104 $NCLIENTS $CLIENT_WAITTIME 15 dgv1.csv &
wait $!

python ./client_by_time.py 192.168.66.105 $NCLIENTS $CLIENT_WAITTIME 15 dgv2.csv &
wait $!

python ./client_by_time.py 192.168.66.106 $NCLIENTS $CLIENT_WAITTIME 15 dgv3.csv &
wait $!

python ./client_by_time.py 192.168.66.107 $NCLIENTS $CLIENT_WAITTIME 15 dgv4.csv &
wait $!

#python ./client_by_time.py 192.168.66.108 $NCLIENTS $CLIENT_WAITTIME 15 dgv5.csv &
#wait $!

#python ./client_by_time.py 192.168.66.109 $NCLIENTS $CLIENT_WAITTIME 15 dgv6.csv &
#wait $!


### Correctness tests of VM
timeout 120 ssh -o ConnectTimeout=60 root@192.168.66.104 "pwd; dmesg; date +%s;" &> responsive1.txt &
SSH1=$!

timeout 120 ssh -o ConnectTimeout=60 root@192.168.66.105 "pwd; dmesg; date +%s;" &> responsive2.txt &
SSH2=$!

timeout 120 ssh -o ConnectTimeout=60 root@192.168.66.106 "pwd; dmesg; date +%s;" &> responsive3.txt &
SSH3=$!

timeout 120 ssh -o ConnectTimeout=60 root@192.168.66.107 "pwd; dmesg; date +%s;" &> responsive4.txt &
SSH4=$!

wait $SSH1
wait $SSH2
wait $SSH3
wait $SSH4
#####
pkill --signal SIGINT -P $CRSH_DET_PID
#kill -s 2 $PING_L2_1_PID

kill -s 2 $PING_L1A_PID
kill -s 2 $PING_L1B_PID
kill -s 2 $PING_L0_PID
kill -s 2 $PING_L2_2_PID
kill -s 2 $PING_L2_3_PID
kill -s 2 $PING_L2_4_PID
#kill -s 2 $PING_L2_5_PID
#kill -s 2 $PING_L2_6_PID

