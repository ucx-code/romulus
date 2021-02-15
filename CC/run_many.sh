#!/bin/bash
#set -euo pipefail
#IFS=$'\n\t'

CSV_MASTER_FILE="master.csv"
MIGRATION_SLEEP_TIME=50
COUNT=$1
mkdir /mnt/l2-disk
losetup -f -P /xen/centos-big-l2.raw 
for i in $(seq 1 $COUNT); 
do
	START=$(date +%s%3N)
	INJ_REG=$((RANDOM % 2))
	INJ_BIT=$((RANDOM % 64))
	#INJ_SLEEP=$((1 + RANDOM % 2))
	INJ_SLEEP=`bc <<< "scale=3; $(((RANDOM % (INJ_SLEEP_MAX - INJ_SLEEP_MIN)) + INJ_SLEEP_MIN))/1000"`
	output=$(. ./run_once.sh $CSV_MASTER_FILE $MIGRATION_SLEEP_TIME $INJ_REG $INJ_BIT $INJ_SLEEP)
	echo -e $output

	echo "Extracting dmesg"
	DMESG_LOG=`ssh root@kahvefali "xl dmesg | tail -n100"`
	FI_CSV=`python extract_fi_log.py "$DMESG_LOG"`
	MIGR_CSV=`python extract_migration_stats.py "$DMESG_LOG"`
	echo $DMESG_LOG
	ssh root@kahvefali "sleep 5; reboot"
	CSV_PATH=d${i}_${START}.csv
	mv -f data.csv results/${CSV_PATH}
	REBOOT_START=$(date +%s%3N)

	echo "Extracting SAR and ping log"
	mount /dev/loop0p1 /mnt/l2-disk
	SAR_LOG_PATH=sar_${i}_${START}.bin
	PING_PATH_L2=ping_${i}_${L2_IP}_${START}.csv
	PING_PATH_L1A=ping_${i}_${L1A_IP}_${START}.csv
	PING_PATH_L1B=ping_${i}_${L1B_IP}_${START}.csv
	PING_PATH_L0=ping_${i}_${L0_IP}_${START}.csv
        RESP1=responsive1_${i}_${START}.txt

        mv -f responsive1.txt results/${RESP1}
	cp -f /mnt/l2-disk/root/sar_log.bin results/${SAR_LOG_PATH} 
	umount /mnt/l2-disk
	mv -f ./ping_$L2_IP.csv results/${PING_PATH_L2}
	mv -f ./ping_$L1A_IP.csv results/${PING_PATH_L1A}
	mv -f ./ping_$L1B_IP.csv results/${PING_PATH_L1B}
	mv -f ./ping_$L0_IP.csv results/${PING_PATH_L0}
	sleep 60 # Avoid connecting right away whil reboot is still ongoing
	bash ssh-retry.sh root@kahvefali "true"
	REBOOT_END=$(date +%s%3N)
	REBOOT_DUR=$((REBOOT_END - REBOOT_START))
	echo "Reboot took $REBOOT_DUR"
	sleep 5
	PARSED=$(python parse_output_to_csv.py "$output")
	echo "$START,$REBOOT_DUR,$REBOOT_END,$PARSED,$INJ_REG,$INJ_BIT,$INJ_SLEEP,$FI_CSV,$MIGR_CSV,$PING_PATH_L2,$PING_PATH_L1A,$PING_PATH_L1B,$PING_PATH_L0,$SAR_LOG_PATH,$CSV_PATH,$RESP1" >> $CSV_MASTER_FILE
done
losetup -d /dev/loop0
