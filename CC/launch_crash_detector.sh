#!/bin/sh
# Launching VM monitor to detect crashes
L2_IP=$1
L0_FLOW=$2
L1A_ID=$3
L1B_ID=$4
L1B_IP=$5

echo "LOADING $L2_IP"

python crash_detector.py $L2_IP 0.4 8 ping_${L2_IP}.csv
RET=$?

if [ "$RET" -eq "255" ]; then
	CRASH_TSTAMP=$(date +%s%3N)
	echo "Crash detected @ $CRASH_TSTAMP"
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

fi


