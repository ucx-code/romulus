# Uses values found in master.csv and ping's to infer and append other information
import pandas as pd
import sys
import os
import csv

def calc_vals(basepath, ping_path, name):
   dfP = pd.read_csv(os.path.join(basepath, ping_path), header=0, na_filter=True, float_precision="round_trip", quotechar='"', sep=",", quoting=csv.QUOTE_NONNUMERIC)
   rowM = dfM[dfM[name] == ping_path]
   dfFailed = dfP[dfP["ret_code"] == 1.0]
   lat = float((dfFailed["ts"].min() * 1000.0) - rowM["fi_tstamp"])
   lat_realfi = float((dfFailed["ts"].min() * 1000.0) - rowM["real_fi_tstamp"])
   
   max_downtime = dfFailed["ts"].max() - dfFailed["ts"].min() # Assuming no intermittent failures
   failed_ping_count = len(dfFailed.index)
   
   return (lat, lat_realfi, float(dfFailed["ts"].min()), max_downtime, failed_ping_count)

master_csv = sys.argv[1]
basepath = os.path.dirname(master_csv)
typ = sys.argv[2]

dfM = pd.read_csv(master_csv, header=0, na_filter=True, float_precision="round_trip", quotechar='"', sep=",",
                    quoting=csv.QUOTE_NONNUMERIC)

data_list = []                 
if typ == "group":
   keysAndPrefixes = { 
      "PING_PATH_L1A" : "pingl1a_",
      "PING_PATH_L1B" : "pingl1b_",
      "PING_PATH_L0" : "pingl0_",
      "PING_PATH_L2_1" : "pingl2-1_",
      "PING_PATH_L2_2" : "pingl2-2_",
      "PING_PATH_L2_3" : "pingl2-3_",
      "PING_PATH_L2_4" : "pingl2-4_",
   }
else:
   keysAndPrefixes = { 
      "PING_PATH_L2": "pingl2_",
      "PING_PATH_L1A" : "pingl1a_",
      "PING_PATH_L1B" : "pingl1b_",
      "PING_PATH_L0" : "pingl0_",
   }


for index, row in dfM.iterrows():
   row_dict = {"PING_PATH_L0" : row.loc["PING_PATH_L0"]}
   for key, prefix in keysAndPrefixes.iteritems():
      t = calc_vals(basepath, row.loc[key], key)
      row_dict.update(
         {
            prefix + "ping2fi_lat": t[0],
            prefix + "ping2realfi_lat": t[1],
            prefix + "maxdowntime": t[3],
            prefix + "failedcount": t[4],
         }
      )
      
   data_list.append(row_dict)   
newDf = pd.DataFrame(data_list)
newDf = newDf.set_index("PING_PATH_L0")
mergedDf = pd.merge(dfM, newDf, on='PING_PATH_L0')
mergedDf.to_csv(master_csv, index=False)