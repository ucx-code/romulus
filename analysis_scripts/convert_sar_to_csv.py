# Receives a sar binary file and sar flags and converts it to CSV format
import sys
import pandas as pd
from StringIO import StringIO
import subprocess
import numpy as np
import os
from pandas.core.common import flatten

def relative_index(df, col_name):
   minval = df[col_name].min()
   return df["timestamp"] - minval

def fill_missing_tstamps(df):
   max_ts = df["timestamp"].max()

   for i in xrange(max_ts):
      if i not in df["timestamp"].values:
         print "missing", i
         df1 = pd.DataFrame(np.repeat(0, len(df.columns))[None, :], 
                  columns=df.columns)
         df1["timestamp"] = i
         #print df1
         df = pd.concat([df, df1])
   return df
     
filename = sys.argv[1]
if len(sys.argv) > 2:
   sar_flag = sys.argv[2].split("_") # To receive multiple values we replace comma with _
else:
   sar_flag = '-p'
if len(sys.argv) == 4:
   ending = sys.argv[3]
else:
   ending = ""
   

print flatten(['sadf', '-U', '-d',  filename, '--', sar_flag])
try:
   out = subprocess.check_output(flatten(['sadf', '-U', '-d',  filename, '--', sar_flag]))
except Exception as e:
   out = e.output # Ignore the error from sadf
#print out

# Divide the output into several parts, according to headers
parts = out.split("#")
dfs = []
for part in parts[1:]:
    tmp = pd.read_csv(StringIO(part.lstrip()), sep=";", header=0, decimal=',')#, index_col='timestamp')
    # Because of join() we need to delete the repeated columns, which are also useless for us...
    del tmp["interval"]
    del tmp["hostname"]
    if "CPU" in tmp:
      del tmp["CPU"]
    dfs.append(tmp)
#print dfs, parts
if (len(dfs) > 1):
   final = dfs[0].join(dfs[1])
else:
   final = dfs[0]
print final
#final = final.astype({'timestamp': 'int64'})
final["relts"] = relative_index(final, "timestamp") # start timestamp from 0
#final = fill_missing_tstamps(final)
final.set_index("timestamp", inplace = True)
new_filename = os.path.splitext(filename)[0] + ending + ".csv"
final.to_csv(new_filename, sep=";")
