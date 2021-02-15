#!/bin/sh
if [ -z "$GIT_PATH" ]; then
	GIT_PATH="/root/xen-hackers"
fi
INSPECT_SAVE="$GIT_PATH/L1_A/utility_apps/inspect_savefile"
UPDATE_SAVE="$GIT_PATH/L1_A/utility_apps/update_savefile"
VM_ID=$1
SAVE_FILE=$2

echo "<xml></xml>" > /tmp/dummy.xml

echo "Saving VM to file"
SAVE_STARTTIME=$(date +%s%3N)
# Create base save file but let VM continue
xl save -c $VM_ID $SAVE_FILE
SAVE_ENDTIME=$(date +%s%3N)
echo "Took $((SAVE_ENDTIME - SAVE_STARTTIME)) ms"

# Get needed offsets
echo "Getting offsets from savefile"
OFF_STARTTIME=$(date +%s%3N)
XC_STREAM_OFFSET=`$INSPECT_SAVE $SAVE_FILE |  grep "Beginning of at offset" | tr -dc '0-9'`
OFF_ENDTIME=$(date +%s%3N)
echo "Took $((OFF_ENDTIME - OFF_STARTTIME)) ms"

# Remove page tables from save file
echo "Removing page tables from savefile"
REMOVE_STARTTIME=$(date +%s%3N)
$UPDATE_SAVE $SAVE_FILE /tmp/dummy.xml 4 $XC_STREAM_OFFSET 2
REMOVE_ENDTIME=$(date +%s%3N)
echo "Took $((REMOVE_ENDTIME - REMOVE_STARTTIME)) ms"


NEW_SAVE_FILE="${SAVE_FILE}-stripped.save"
rm /tmp/dummy.xml
if [ ! -z "$DEST" ]; then
	mv $NEW_SAVE_FILE $DEST
fi
