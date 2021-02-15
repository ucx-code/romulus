# TODO: Check if variables are defined
CSV_MASTER_FILE=$1
MIGRATION_SLEEP_TIME=$2
INJ_REG=$3
INJ_BIT=$4
INJ_SLEEP=$5

# Launch VMs
echo "Launching L1 VMs @ L0"
ssh root@kahvefali "xl destroy $L1A_DOMNAME; xl destroy $L1B_DOMNAME; mount -o noacl,nocto,noatime,nodiratime  192.168.66.5:/xen /nfs; xl create /var/lib/nova/instances/nestedvirt/new_image/new_debian2.cfg; xl create /var/lib/nova/instances/nestedvirt/new_image/new_debian.cfg; xl save-nvmcs 1 1;"

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
ssh root@$L1B_IP "bash alloc.sh"

# Launch L2 VM
ssh root@$L1A_IP "bash spawn.sh"
#sleep 80

sleep 20
bash ssh-retry.sh root@$L2_IP "true"

# Launch SAR probe on L2
echo "Launching SAR probe @ L2"
ssh root@$L2_IP "service solr start; rm /root/sar_log.bin; sar -o /root/sar_log.bin 1 >/dev/null 2>&1 &" &> /dev/null
#ssh root@$L2_IP "rm /root/sar_log.bin; sar -o /root/sar_log.bin 1 >/dev/null 2>&1 &"
# Wait for L2 to start and to stabilize
sleep 50


# Create base save file
CREATE_BASE_SAVE_START=$(date +%s%3N)
echo "Creating base save file started @ $CREATE_BASE_SAVE_START"
ssh root@$L1A_IP "DEST=/nfs/d.save bash create_savefile.sh 1 /tmp/save.tmp; rm /tmp/save.tmp"
CREATE_BASE_SAVE_END=$(date +%s%3N)
CREATE_BASE_SAVE_DUR=$((CREATE_BASE_SAVE_END - CREATE_BASE_SAVE_START))
echo "Creating base save took $CREATE_BASE_SAVE_DUR"
echo "Creating base save file ended @ $CREATE_BASE_SAVE_END"

sleep 2

echo "Launching monitors"
# Launching VM monitor for L1 and L0
python crash_detector.py $L1A_IP 0.4 200 ping_${L1A_IP}.csv &
PING_L1A_PID=$!
python crash_detector.py $L1B_IP 0.4 200 ping_${L1B_IP}.csv &
PING_L1B_PID=$!
python crash_detector.py $L0_IP 0.4 200 ping_${L0_IP}.csv &
PING_L0_PID=$!
python crash_detector.py $L2_IP 0.4 200 ping_${L2_IP}.csv &
PING_L2_PID=$!


# Wait to let workload run

WL_TSTAMP=$(date +%s%3N)
echo "Start workload @ $WL_TSTAMP"
#python ./client.py $L2_IP $NCLIENTS $CLIENT_WAITTIME $MIN_REQ $MAX_REQ &
python ./client_by_time.py $L2_IP $NCLIENTS $CLIENT_WAITTIME $END_TIME &
WL_PID=$!

# Injeting a fault
sleep $INJ_SLEEP

#sleep $MIGRATION_SLEEP_TIME

echo "Migrating L2 between L1s"
FLOW_MIGRATION_START=$(date +%s%3N)
ssh root@kahvefali "bash $L0_FLOW $L1A_ID $L1B_ID /nfs/d.save"
FLOW_MIGRATION_END=$(date +%s%3N)
FLOW_MIGRATION_DUR=$((FLOW_MIGRATION_END - FLOW_MIGRATION_START))
echo "Flow.sh migration took $FLOW_MIGRATION_DUR"

RESTORE_START=$(date +%s%3N)
ssh root@$L1B_IP "xl restore /nfs/d.save"
RESTORE_END=$(date +%s%3N)
RESTORE_DUR=$((RESTORE_END - RESTORE_START))
echo "Restore took $RESTORE_DUR"

# Wait for wl to finish
wait $WL_PID
echo "Workload has finished"
kill -s 2 $PING_L1A_PID
kill -s 2 $PING_L1B_PID
kill -s 2 $PING_L0_PID
kill -s 2 $PING_L2_PID

