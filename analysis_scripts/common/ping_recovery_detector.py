# Uses ping log to detect if there was a recovery of the VM
import pandas as pd
import sys
import os
import csv



def check_ping_detection(path):
   df = pd.read_csv(path, header=0, na_filter=True, float_precision="round_trip", quotechar='"', sep=",",
                       quoting=csv.QUOTE_NONNUMERIC)

   return (df.tail(1)["ret_code"] == 0.0).bool()
   

if __name__ == "__main__":
   if len(sys.argv) == 2:
      print check_ping_detection(sys.argv[1])
   elif len(sys.argv) == 5:
      data_path = sys.argv[1]
      csv_path = sys.argv[2]
      merge_key = sys.argv[3]
      vm_id = int(sys.argv[4])
      
      df = pd.read_csv(csv_path, header=0, na_filter=False, float_precision="round_trip", quotechar='"', sep=",",
                    quoting=csv.QUOTE_NONNUMERIC)
                    
      dict_list = []
      for ping_path in df[merge_key]:
         dict_list.append({
               merge_key : ping_path,   
               'vm%s_recovered_ping' % vm_id: check_ping_detection(os.path.join(data_path, ping_path))
            })
      dfP = pd.DataFrame(dict_list)
      df = pd.merge(dfP, df, left_on=merge_key, right_on=merge_key)
      
      df.to_csv(csv_path, index=False)
   else:
      print "Incorrect amount of params"
      