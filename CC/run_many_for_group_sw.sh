#!/bin/bash
#set -euo pipefail
#IFS=$'\n\t'

PATCHES_FOLDER="vmx_after_filtering"
LAST_PATCH="last_patch.info"
CSV_MASTER_FILE="master.csv"
MIGRATION_SLEEP_TIME=50
COUNT=$1

ALL_PATCHES=(${PATCHES_FOLDER}/*.patch)
if [ -f $LAST_PATCH ]; then
	PATCH_NR=`cat $LAST_PATCH`
else
	PATCH_NR=0
	echo $PATCH_NR > $LAST_PATCH 
fi

ssh root@kahvefali "cd /var/lib/nova/instances/nestedvirt/new_image; rm *-snap.qcow2; qemu-img create -f qcow2 -b debian-disk.qcow2 debian-disk-snap.qcow2; qemu-img create -f qcow2 -b debian-disk2.qcow2 debian-disk2-snap.qcow2;"

for i in $(seq 1 $COUNT); 
do
	START=$(date +%s%3N)
	START_L0_RDTSC=`ssh root@${L0_IP} "/root/print_rdtsc"`

	# Choose the next patch
	PATCH_FILE=${ALL_PATCHES[$PATCH_NR]}
	echo "Doing patch $PATCH_FILE"
	output=$(. ./run_once_for_group_sw.sh $CSV_MASTER_FILE $MIGRATION_SLEEP_TIME $PATCH_FILE)
	echo -e $output
	
	if [[ "$output" != *"Make failed"* ]]; then

		# Read VMI offsets from the output
		VMI_ENABLEFI_ADDR=`echo "$output" | tr -d -c '0-9 ' | awk '{print $3}'`
		VMI_INJTSC_ADDR=`echo "$output" | tr -d -c '0-9 ' | awk '{print $4}'`
		VMI_ITERSBEFORE_ADDR=`echo "$output" | tr -d -c '0-9 ' | awk '{print $5}'`
		VMI_ITERSAFTER_ADDR=`echo "$output" | tr -d -c '0-9 ' | awk '{print $6}'`
		VMI_DOMAINLIST_ADDR=`echo "$output" | tr -d -c '0-9 ' | awk '{print $7}'`

		echo "Extracting SW stats with VMI"
		SW_INJTSC=`ssh root@kahvefali "$VMI_SW_HELPER $L1A_DOMNAME read_injtsc $VMI_INJTSC_ADDR"`
		SW_ITERSAFTER=`ssh root@kahvefali "$VMI_SW_HELPER $L1A_DOMNAME read_itersafter $VMI_ITERSAFTER_ADDR"`
		SW_ITERSBEFORE=`ssh root@kahvefali "$VMI_SW_HELPER $L1A_DOMNAME read_itersbefore $VMI_ITERSBEFORE_ADDR"`

		echo "Extracting dmesg"
		DMESG_LOG=`ssh root@kahvefali "xl dmesg | tail -n100"`
		FI_CSV=`python extract_fi_log.py "$DMESG_LOG"`
		MIGR_CSV=`python extract_migration_stats.py "$DMESG_LOG"`
		echo $DMESG_LOG

		TRACE_PATH=trace_${i}_${START}.bin

		ssh root@$L0_IP "sleep 5; reboot"
		CSV_PATH1=dg1_${i}_${START}.csv
		CSV_PATH2=dg2_${i}_${START}.csv
		CSV_PATH3=dg3_${i}_${START}.csv
		CSV_PATH4=dg4_${i}_${START}.csv
		CSV_PATH1v=dgv1_${i}_${START}.csv
		CSV_PATH2v=dgv2_${i}_${START}.csv
		CSV_PATH3v=dgv3_${i}_${START}.csv
		CSV_PATH4v=dgv4_${i}_${START}.csv
		mv -f dg1.csv results/${CSV_PATH1}
		mv -f dg2.csv results/${CSV_PATH2}
		mv -f dg3.csv results/${CSV_PATH3}
		mv -f dg4.csv results/${CSV_PATH4}
		mv -f dgv1.csv results/${CSV_PATH1v}
		mv -f dgv2.csv results/${CSV_PATH2v}
		mv -f dgv3.csv results/${CSV_PATH3v}
		mv -f dgv4.csv results/${CSV_PATH4v}

		RESP1=responsive1_${i}_${START}.txt
		RESP2=responsive2_${i}_${START}.txt
		RESP3=responsive3_${i}_${START}.txt
		RESP4=responsive4_${i}_${START}.txt
		mv -f responsive1.txt results/${RESP1}
		mv -f responsive2.txt results/${RESP2}
		mv -f responsive3.txt results/${RESP3}
		mv -f responsive4.txt results/${RESP4}

		REBOOT_START=$(date +%s%3N)

		echo "Extracting ping log"
		PING_PATH_L2_1=ping_${i}_192.168.66.104_${START}.csv
		PING_PATH_L2_2=ping_${i}_192.168.66.105_${START}.csv
		PING_PATH_L2_3=ping_${i}_192.168.66.106_${START}.csv
		PING_PATH_L2_4=ping_${i}_192.168.66.107_${START}.csv
		PING_PATH_L1A=ping_${i}_${L1A_IP}_${START}.csv
		PING_PATH_L1B=ping_${i}_${L1B_IP}_${START}.csv
		PING_PATH_L0=ping_${i}_${L0_IP}_${START}.csv
		mv -f ./ping_192.168.66.104.csv results/${PING_PATH_L2_1}
		mv -f ./ping_192.168.66.105.csv results/${PING_PATH_L2_2}
		mv -f ./ping_192.168.66.106.csv results/${PING_PATH_L2_3}
		mv -f ./ping_192.168.66.107.csv results/${PING_PATH_L2_4}
		mv -f ./ping_$L1A_IP.csv results/${PING_PATH_L1A}
		mv -f ./ping_$L1B_IP.csv results/${PING_PATH_L1B}
		mv -f ./ping_$L0_IP.csv results/${PING_PATH_L0}
	
		echo "Restoring snapshots"
		ssh root@192.168.66.12 "rm /root/nfs/centos-big-l2-snapg*; cd /root/nfs;  qemu-img create -f qcow2 -b centos-big-l2-g2.qcow2 centos-big-l2-snapg2.qcow2; qemu-img create -f qcow2 -b centos-big-l2-g3.qcow2 centos-big-l2-snapg3.qcow2; qemu-img create -f qcow2 -b centos-big-l2-g4.qcow2 centos-big-l2-snapg4.qcow2; qemu-img create -f qcow2 -b centos-big-l2-g5.qcow2 centos-big-l2-snapg5.qcow2; chown nobody:nobody *;"


		bash ssh-retry.sh root@kahvefali "true"
		REBOOT_END=$(date +%s%3N)
		REBOOT_DUR=$((REBOOT_END - REBOOT_START))
		echo "Reboot took $REBOOT_DUR"
		sleep 5

		PARSED=$(python parse_output_to_csv.py "$output")
		echo "$output"
		echo "1,$START,$START_L0_RDTSC,$REBOOT_DUR,$REBOOT_END,$PARSED,$INJ_REG,$INJ_BIT,$INJ_SLEEP,$FI_CSV,$MIGR_CSV,$PING_PATH_L2_1,$PING_PATH_L2_2,$PING_PATH_L2_3,$PING_PATH_L2_4,$PING_PATH_L2_5,$PING_PATH_L2_6,$PING_PATH_L1A,$PING_PATH_L1B,$PING_PATH_L0,$CSV_PATH1,$CSV_PATH2,$CSV_PATH3,$CSV_PATH4,$CSV_PATH5,$CSV_PATH6,$CSV_PATH1v,$CSV_PATH2v,$CSV_PATH3v,$CSV_PATH4v,$CSV_PATH5v,$CSV_PATH6v,${TRACE_PATH}.pxz,$RESP1,$RESP2,$RESP3,$RESP4,$SW_INJTSC,$SW_ITERSAFTER,$SW_ITERSBEFORE,$VMI_ENABLEFI_ADDR,$VMI_INJTSC_ADDR,$VMI_ITERSBEFORE_ADDR,$VMI_ITERSAFTER_ADDR,$VMI_DOMAINLIST_ADDR,$PATCH_NR,$PATCH_FILE" >> $CSV_MASTER_FILE
	else
		echo "0,$START,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,$PATCH_NR,$PATCH_FILE" >> $CSV_MASTER_FILE
	fi

	# Move on to the next patch
	PATCH_NR=$((PATCH_NR+1))
        echo $PATCH_NR > $LAST_PATCH 

	echo "Restoring kahvefali's snapshots"
	ssh root@kahvefali "cd /var/lib/nova/instances/nestedvirt/new_image; rm *-snap.qcow2; qemu-img create -f qcow2 -b debian-disk.qcow2 debian-disk-snap.qcow2; qemu-img create -f qcow2 -b debian-disk2.qcow2 debian-disk2-snap.qcow2;"

done

