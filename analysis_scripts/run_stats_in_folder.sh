#!/bin/sh

# Iterates over every d*.csv file in a certain folder, calling stats.py on it and storing the statistics to a file
DATA_PATH=$1
GLOBAL_FILE=stats_$(basename $1).csv
rm $GLOBAL_FILE
for f in $1/d*.csv; do 
   echo $f
   python stats.py -d $f -m $GLOBAL_FILE -r $f
done

python merge_master_with_stats.py $1/master.csv $GLOBAL_FILE &

for f in $1/sar*.bin; do 
   python convert_sar_to_csv.py $f &
done

wait

# Add some extra info to master.csv
python common/extra_info_to_master.py joined.csv

# Add ping info to master.csv
#python extra_ping_info.py joined.csv single

# When available, use SSH logs to verify VM state after a run
python common/ssh_recovery_detector.py $DATA_PATH/  joined.csv RESPONSIVE_PATH1 1

python extra_sar_info.py joined.csv $DATA_PATH

# Clean temporary run CSV files
PER_RUN_TMPFILE=stats_$(basename $DATA_PATH).csv
for f in $PER_RUN_TMPFILE; do 
   rm $f
done

mv joined.csv ${DATA_PATH}/../csvs/$(basename $DATA_PATH).csv