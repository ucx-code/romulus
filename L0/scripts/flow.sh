#!/bin/bash
if [ -z "$GIT_PATH" ]; then
        #GIT_PATH="/root/xen-hackers"
        GIT_PATH="/var/lib/nova/instances/gitvmm"
fi

set -euo pipefail
IFS=$'\n\t'

OFFSET_ADDR=13712289792 #0x33150F000
#OFFSET_ADDR=13668872192 ##0x32EBA7000
#OFFSET_ADDR=4348772352
VM_L1_A=$1
VM_L1_B=$2
SAVE_FILE=$3
L1_A_NAME="L1A" # TODO
STATE_FILE="/tmp/state"
INSPECT_PATH="$GIT_PATH/L0/utility_apps/vmstate_extractor/vmstate"
INSPECT_SAVE="$GIT_PATH/L0/utility_apps/inspect_savefile/inspect_savefile"
UPDATE_SAVE="$GIT_PATH/L0/utility_apps/update_savefile/update_savefile"
OFFSETS_FILE="/tmp/offsets"
#HYPERCALLER="/external_hdd/xen/xen-4.11.1/tools/xcutils/hypercaller2"
HYPERCALLER="/var/lib/nova/instances/xen4/xen-4.11.1/tools/xcutils/hypercaller2"

##======
echo "Doing save NVMCS"
NVMCS_STARTTIME=$(date +%s%3N)
NVMCS_PATH="/tmp/nvmcs.xml"
#xl save-nvmcs 1
#sleep 1
$INSPECT_PATH $L1_A_NAME $OFFSET_ADDR 1
#sleep 3 # A small sleep is used to avoid entering an infinite loop on the hypervisor because it tries to get a nVMCs before any exist has taken place
xl save-nvmcs 2 1 > $NVMCS_PATH
NVMCS_ENDTIME=$(date +%s%3N)
#====
echo "Save nVMCS took $((NVMCS_ENDTIME - NVMCS_STARTTIME)) ms"

# Step 5 - Inspect L2 VM state
echo "Inspecting L2 VM state"
INSPECT_STATE_STARTTIME=$(date +%s%3N)
$INSPECT_PATH $L1_A_NAME $OFFSET_ADDR> $STATE_FILE
# pause was here
INSPECT_STATE_ENDTIME=$(date +%s%3N)
echo "Inspecting L2 state took $((INSPECT_STATE_ENDTIME - INSPECT_STATE_STARTTIME)) ms"

# Step 8 - Recalculate the offsets
echo "Recalculating the offsets"
RECALC_OFFSETS_STARTTIME=$(date +%s%3N)
echo "$INSPECT_SAVE $SAVE_FILE > $OFFSETS_FILE"
$INSPECT_SAVE $SAVE_FILE 2 > $OFFSETS_FILE
CPU_OFFSET=`cat $OFFSETS_FILE | grep "\[offsets\] cpu at position" | tr -dc '0-9'`
XC_STREAM_OFFSET=`cat $OFFSETS_FILE |  grep "Beginning of at offset" | tr -dc '0-9'`
RECALC_OFFSETS_ENDTIME=$(date +%s%3N)
echo "Recalculating the offsets took $((RECALC_OFFSETS_ENDTIME - RECALC_OFFSETS_STARTTIME)) ms"

# Step 9 - Replace CPU and nVMCS values in save file with new ones
echo "Replacing CPU and nVMCS values in save file"
REPLACE_SAVE_VALUES_STARTTIME=$(date +%s%3N)
$UPDATE_SAVE $SAVE_FILE $STATE_FILE 2 $CPU_OFFSET 2
$UPDATE_SAVE $SAVE_FILE $NVMCS_PATH 5 $CPU_OFFSET 2
REPLACE_SAVE_VALUES_ENDTIME=$(date +%s%3N)
echo "Replacing CPU and nVMCS took $((REPLACE_SAVE_VALUES_ENDTIME - REPLACE_SAVE_VALUES_STARTTIME)) ms"

# Step 10 - Obtain original EPTP
OLD_EPTP=`xmllint --xpath '/vms/vm[2]/eptp' $STATE_FILE  | tr -dc '0-9'`

# Step 11 - Perform hypercall
echo "Performing hypercall"
HCALL_STARTTIME=$(date +%s%3N)
BASE_PFN=1048576
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
xmlstarlet ed -u '/vms/vm[2]/eptp' -v "$NEW_EPTP" $STATE_FILE > $NEW_STATE_FILE

# Step 13 - Add new EPTP to save file
$UPDATE_SAVE $SAVE_FILE $NEW_STATE_FILE 3 $XC_STREAM_OFFSET 2
UPDATE_EPTP_ENDTIME=$(date +%s%3N)
echo "Updating EPTP in save took $((UPDATE_EPTP_ENDTIME - UPDATE_EPTP_STARTTIME)) ms"

echo "Done"
xl pause $VM_L1_A

