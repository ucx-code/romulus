# Uses values found in master.csv and ping's to infer and append other information
import pandas as pd
import numpy as np
import sys
import os
import csv

master_csv = sys.argv[1]
nVMs = int(sys.argv[2])

dfM = pd.read_csv(master_csv, header=0, na_filter=True, float_precision="round_trip", quotechar='"', sep=",",
                    quoting=csv.QUOTE_NONNUMERIC)
# Count recovered vms
recovered_lst1 = []
recovered_lst2 = []
recovered_lst3 = []
recovered_lst4 = []
recovered_lst = []
have_ssh_lst = []

for index, row in dfM.iterrows():
   # Count recovered vms
   count_recovered = 0
   for i in xrange(1, nVMs + 1):
      if row["vm" + str(i) + "_recovered"] == 1.0:
         count_recovered = count_recovered + 1
   recovered_lst1.append(count_recovered)
   
   # Count recovered vms using v2 data only
   count_recovered = 0
   for i in xrange(1, nVMs + 1):
      if row["vm" + str(i) + "_v2_recovered"] == 1.0:
         count_recovered = count_recovered + 1
   recovered_lst2.append(count_recovered)
   
   # Count recovered vms (solr v1 + v2)
   count_recovered = 0
   for i in xrange(1, nVMs + 1):
      if row["vm" + str(i) + "_recovered"] == 1.0 or row["vm" + str(i) + "_v2_recovered"] == 1.0:
         count_recovered = count_recovered + 1
   recovered_lst3.append(count_recovered)
   
   # Count recovered vms by ping
   count_recovered = 0
   for i in xrange(2, nVMs + 1): # VM1 has no ping check
      if row["vm" + str(i) + "_recovered_ping"] == 1.0:
         count_recovered = count_recovered + 1
   recovered_lst4.append(count_recovered) 
   
   # Count recovered vms by ssh
   tot_have_ssh = 0
   count_recovered = 0
   if "vm1_recovered_ssh" in row:
      for i in xrange(1, nVMs + 1):
         if row["vm" + str(i) + "_recovered_ssh"] == 1.0:
            count_recovered = count_recovered + 1
         tot_have_ssh = tot_have_ssh + 1
   else:
      count_recovered = None
      
   recovered_lst.append(count_recovered)
   have_ssh_lst.append(tot_have_ssh)
   
dfM["count_recovered_vms"] = np.asarray(recovered_lst1)
dfM["count_recovered_vms_v2"] = np.asarray(recovered_lst2)
dfM["count_recovered_vms_solr"] = np.asarray(recovered_lst3)
dfM["count_recovered_vms_ping"] = np.asarray(recovered_lst4)
dfM["count_recovered_vms_ssh"] = np.asarray(recovered_lst)
dfM["count_have_ssh_check"] = np.asarray(have_ssh_lst)

dfM.to_csv(master_csv, index=False)