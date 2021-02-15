#!/bin/sh

STAGING_PATH=/home/frederico/xen/results/staging/
RUNS_PATH=$1
rm  -v -I --preserve-root $STAGING_PATH/*
for f in $RUNS_PATH/*/dg[0-9]_*_*.csv; do 
   base=$(basename $f)
   cp $f ${STAGING_PATH}/$base
done

bash run_stats_in_folder.sh $STAGING_PATH 4