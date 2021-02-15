# Sometimes runs may not follow the desired behaviour and have less-than-ideal data
# For example a VM may fail to start up, FI may not be effective, crash not detected, etc.
# This script filters out the bad runs and leaves the good ones

import sys
import pandas as pd
import csv
import os

path = sys.argv[1]
df = pd.read_csv(path, header=0, na_filter=True, float_precision="round_trip", quotechar='"', sep=",", quoting=csv.QUOTE_NONNUMERIC)
nvms = 1

# Ensure solr has started in all machines before recovery
print "Rows before", len(df.index)          
df = df[df["solr_started"] == True]
print "Rows after", len(df.index)  

# Ensure crash was detected
print "Rows before", len(df.index)
df = df[~df["crash_tstamp"].isna()]
print "Rows after", len(df.index)  
 
# We should not have any 500 code responses
print "Rows before", len(df.index)        
df = df[df["tot_500"] == 0]
print "Rows after", len(df.index) 

df.to_csv(os.path.splitext(path)[0] + "_filtered.csv", index=False)