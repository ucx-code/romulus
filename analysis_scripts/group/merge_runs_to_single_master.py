import pandas as pd
import sys
import os
import csv

output_csv = "joined.csv"
master_csv = sys.argv[1]
dfM = pd.read_csv(master_csv, header=0, na_filter=False, float_precision="round_trip", quotechar='"', sep=",",
                       quoting=csv.QUOTE_NONNUMERIC)
df_list = []
for i in xrange(2, len(sys.argv)):
   dfS = pd.read_csv(sys.argv[i], header=0, na_filter=False, float_precision="round_trip", quotechar='"', sep=",",
                       quoting=csv.QUOTE_NONNUMERIC)
   df_list.append(dfS)

df = pd.concat(df_list, sort=False)
df = pd.merge(dfM, df, left_on='CSV_PATH1', right_on='vm1_filename')

df.to_csv(output_csv, index=False, mode="w")