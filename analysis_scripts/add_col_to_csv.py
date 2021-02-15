
import pandas as pd
import sys 
import csv
import os

if (len(sys.argv) > 1):
   filename = sys.argv[1]
   col = sys.argv[2]
   val = sys.argv[3]

   df = pd.read_csv(filename, header=0, na_filter=False, float_precision="round_trip")
   df[col] = val

   df.to_csv(filename + "_modified.csv", index=False, quotechar='"', quoting=csv.QUOTE_NONNUMERIC)
