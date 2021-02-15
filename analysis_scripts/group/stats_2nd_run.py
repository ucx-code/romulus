import pandas as pd
import sys
import os
import getopt
import csv

def recovered(path):   
   df = pd.read_csv(path, header=0, na_filter=True, float_precision="round_trip", quotechar='"', sep=",",
                       quoting=csv.QUOTE_NONNUMERIC)
   return len(df[df["code"] == 200].index) > 0
   
"""def extract_starttime(filename):
   (_,_,start) = filename.split("_")
   return int(start.split(".")[0])

master = sys.argv[1]
csv_key = sys.argv[2]
key = sys.argv[3]
prefix = sys.argv[4]

dfM = pd.read_csv(master, header=0, na_filter=True, float_precision="round_trip", quotechar='"', sep=",",
              quoting=csv.QUOTE_NONNUMERIC)
              
df = pd.read_csv(dfM[, header=0, na_filter=True, float_precision="round_trip", quotechar='"', sep=",",
              quoting=csv.QUOTE_NONNUMERIC)
              
dfM[prefix + "_" + key] = recovered(df)

dfM.to_csv(master, index=False)"""

if __name__ == "__main__":
   if len(sys.argv) == 2:
      print recovered(sys.argv[1])
   elif len(sys.argv) == 5:
      data_path = sys.argv[1]
      csv_path = sys.argv[2]
      merge_key = sys.argv[3]
      vm_id = int(sys.argv[4])
      
      df = pd.read_csv(csv_path, header=0, na_filter=True, float_precision="round_trip", quotechar='"', sep=",",
                    quoting=csv.QUOTE_NONNUMERIC)
                    
      dict_list = []
      for v2_path in df[merge_key]:
         dict_list.append({
               merge_key : v2_path,   
               'vm%s_v2_recovered' % vm_id: recovered(os.path.join(data_path, v2_path))
            })
      dfP = pd.DataFrame(dict_list)
      df = pd.merge(dfP, df, left_on=merge_key, right_on=merge_key)
      
      df.to_csv(csv_path, index=False)
   else:
      print "Incorrect amount of params"
      
