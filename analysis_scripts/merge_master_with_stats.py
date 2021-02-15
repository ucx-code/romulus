import pandas as pd
import sys
import os
import csv

master_csv = sys.argv[1]
stats_csv = sys.argv[2]
output_csv = "joined.csv"

dfM = pd.read_csv(master_csv, header=0, na_filter=False, float_precision="round_trip", quotechar='"', sep=",",
                    quoting=csv.QUOTE_NONNUMERIC)
dfS = pd.read_csv(stats_csv, header=0, na_filter=False, float_precision="round_trip", quotechar='"', sep=",",
                    quoting=csv.QUOTE_NONNUMERIC)
print dfS["filename"]                   
df = pd.merge(dfM, dfS, left_on='CSV_PATH', right_on='filename')

df.to_csv(output_csv, index=False, mode="w")