#!/bin/bash
if [ -z "$GIT_PATH" ]; then
        #GIT_PATH="/external_hdd/xen/xen-hackers"
        GIT_PATH="/var/lib/nova/instances/gitvmm"
fi

set -euo pipefail
IFS=$'\n\t'


#OFFSET_ADDR=13711671296 # 6vcpus,13G
OFFSET_ADDR=13712289792 # 1vcpu,13G
#OFFSET_ADDR=13668872192 ##0x32EBA7000
#OFFSET_ADDR=4348772352
VM_L1_A=$1
VM_L1_B=$2
SAVE_FILE=$3
L1_A_NAME="L1A"
STATE_FILE="/tmp/state"
INSPECT_PATH="$GIT_PATH/L0/utility_apps/vmstate_extractor/vmstate"
INSPECT_SAVE="$GIT_PATH/L0/utility_apps/inspect_savefile/inspect_savefile"
UPDATE_SAVE="$GIT_PATH/L0/utility_apps/update_savefile/update_savefile"
OFFSETS_FILE="/tmp/offsets"
#HYPERCALLER="/external_hdd/xen/xen-4.11.1/tools/xcutils/hypercaller2"
HYPERCALLER="/var/lib/nova/instances/xen4/xen-4.11.1/tools/xcutils/hypercaller2"

TOTAL_L2_VMS=4

##======
echo "Doing save NVMCS"
NVMCS_STARTTIME=$(date +%s%3N)
NVMCS_PATH="/tmp/nvmcs.xml"
#xl save-nvmcs 1
#sleep 1
$INSPECT_PATH $L1_A_NAME $OFFSET_ADDR 1
sleep 3 # A small sleep is used to avoid entering an infinite loop on the hypervisor because it tries to get a nVMCs before any exist has taken place
for (( VM_ID=1; VM_ID<=TOTAL_L2_VMS; VM_ID++ ))
do
	xl save-nvmcs 2 $VM_ID > ${NVMCS_PATH}-${VM_ID}
done
NVMCS_ENDTIME=$(date +%s%3N)
#====
echo "Save nVMCS took $((NVMCS_ENDTIME - NVMCS_STARTTIME)) ms"

# Step 5 - Inspect L2 VM state
echo "Inspecting L2 VM state"
INSPECT_STATE_STARTTIME=$(date +%s%3N)
$INSPECT_PATH $L1_A_NAME $OFFSET_ADDR> $STATE_FILE
xl pause $VM_L1_A
INSPECT_STATE_ENDTIME=$(date +%s%3N)
echo "Inspecting L2 state took $((INSPECT_STATE_ENDTIME - INSPECT_STATE_STARTTIME)) ms"

# Step 8 - Recalculate the offsets
echo "Recalculating the offsets"
#RECALC_OFFSETS_STARTTIME=$(date +%s%3N)
for (( VM_ID=1; VM_ID<=TOTAL_L2_VMS; VM_ID++ ))
do
	$INSPECT_SAVE ${SAVE_FILE}-${VM_ID}  2 > ${OFFSETS_FILE}-${VM_ID}
done

#echo "Recalculating the offsets took $((RECALC_OFFSETS_ENDTIME - RECALC_OFFSETS_STARTTIME)) ms"

# Step 9 - Replace CPU and nVMCS values in save file with new ones
echo "Replacing CPU and nVMCS values in save file"
REPLACE_SAVE_VALUES_STARTTIME=$(date +%s%3N)
for (( VM_ID=1; VM_ID<=TOTAL_L2_VMS; VM_ID++ ))
do
	CPU_OFFSET=`cat ${OFFSETS_FILE}-${VM_ID} | grep "\[offsets\] cpu at position" | tr -dc '0-9'`

	$UPDATE_SAVE ${SAVE_FILE}-${VM_ID} $STATE_FILE 2 $CPU_OFFSET $((VM_ID + 1))
	$UPDATE_SAVE ${SAVE_FILE}-${VM_ID} ${NVMCS_PATH}-${VM_ID} 5 $CPU_OFFSET $((VM_ID + 1))
done
REPLACE_SAVE_VALUES_ENDTIME=$(date +%s%3N)
echo "Replacing CPU and nVMCS took $((REPLACE_SAVE_VALUES_ENDTIME - REPLACE_SAVE_VALUES_STARTTIME)) ms"

for (( VM_ID=1; VM_ID<=TOTAL_L2_VMS; VM_ID++ ))
do
	# Step 10 - Obtain original EPTP
	OLD_EPTP=`xmllint --xpath "/vms/vm[$((VM_ID+1))]/eptp" $STATE_FILE  | tr -dc '0-9'`

	# Step 11 - Perform hypercall
	echo "Performing hypercall"
	HCALL_STARTTIME=$(date +%s%3N)
	#BASE_PFN=$((282624 + (393216 * (VM_ID - 1))))
#	BASE_PFN=$((524288 + (458752 * (VM_ID - 1))))
	#BASE_PFN=$((1048576 + (458752 * (VM_ID - 1))))
	BASE_PFN=$((1048576 + (196608 * (VM_ID - 1))))

	#BASE_PFN=393216
	echo "$HYPERCALLER $VM_L1_A $OLD_EPTP $VM_L1_B $BASE_PFN"
	NEW_EPTP=`$HYPERCALLER $VM_L1_A $OLD_EPTP $VM_L1_B $BASE_PFN`
	HCALL_ENDTIME=$(date +%s%3N)
	echo "Performing hypercall took $((HCALL_ENDTIME - HCALL_STARTTIME)) ms"

	# Hypercaller returns a signed 64 bit value which contains only the MFN of the EPTP. We append 0x01E which represent EPT flags.
	NEW_EPTP=$(((NEW_EPTP << 12) + 30))
	echo $NEW_EPTP
	#NEW_EPTP=$((NEW_EPTP>>12))
	#NEW_EPTP=$OLD_EPTP

	echo "Updating EPTP in save file"
	UPDATE_EPTP_STARTTIME=$(date +%s%3N)
	# Step 12- Replace old EPTP with the new EPTP on the state file
	NEW_STATE_FILE="${STATE_FILE}-new.csv"
	xmlstarlet ed -u "/vms/vm[$((VM_ID+1))]/eptp" -v "$NEW_EPTP" $STATE_FILE > $NEW_STATE_FILE

	XC_STREAM_OFFSET=`cat ${OFFSETS_FILE}-${VM_ID} |  grep "Beginning of at offset" | tr -dc '0-9'`
	# Step 13 - Add new EPTP to save file
	$UPDATE_SAVE ${SAVE_FILE}-${VM_ID} $NEW_STATE_FILE 3 $XC_STREAM_OFFSET $((VM_ID + 1))
	UPDATE_EPTP_ENDTIME=$(date +%s%3N)
	echo "Updating EPTP in save took $((UPDATE_EPTP_ENDTIME - UPDATE_EPTP_STARTTIME)) ms"
done

echo "Done"
