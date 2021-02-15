# Some more recent runs perform a ssh at the end to obtain the state of the VM. This script checks if this ssh worked or not
import pandas as pd
import sys
import os
import csv



def check_ssh_detection(data_path, ssh_path):
   path = os.path.join(data_path, ssh_path)
   with open(path, "r") as f:
      if (f.readline() == "/root\n"):
         return True
      else:
         return False
   return False
   

if __name__ == "__main__":
   if len(sys.argv) == 2:
      print check_ssh_detection(sys.argv[1])
   elif len(sys.argv) == 5:
      data_path = sys.argv[1]
      csv_path = sys.argv[2]
      merge_key = sys.argv[3]
      vm_id = int(sys.argv[4])
      
      df = pd.read_csv(csv_path, header=0, na_filter=False, float_precision="round_trip", quotechar='"', sep=",",
                    quoting=csv.QUOTE_NONNUMERIC)
                    
      dict_list = []
      for ssh_path in df[merge_key]:
         if ssh_path != "":
            dict_list.append({
                  merge_key : ssh_path,   
                  'vm%s_recovered_ssh' % vm_id: check_ssh_detection(data_path, ssh_path)
               })
      if len(dict_list) > 0:
         dfP = pd.DataFrame(dict_list)
         df = pd.merge(dfP, df, left_on=merge_key, right_on=merge_key)
      
      df.to_csv(csv_path, index=False)
   else:
      print "Incorrect amount of params"
      