# Uses values found in master.csv and ping's to infer and append other information
import pandas as pd
import sys
import os
import csv

def calculate_biggest_difference(r):
   #print r.sortby("timestamp")
   #rows = r.iterrows()
   #print "l", len(r.index)
   biggest_diff = 0
   for i in xrange(1, len(r.index)):
      #print "i", i
      d = r.iloc[i]["timestamp"] - r.iloc[i-1]["timestamp"] 
      if (d > biggest_diff):
         biggest_diff = d
   return None if biggest_diff == 0 else biggest_diff
   
master_csv = sys.argv[1]

if len(sys.argv) < 3:
   basepath = os.path.dirname(master_csv)
else:
   basepath = sys.argv[2]
   
dfM = pd.read_csv(master_csv, header=0, na_filter=True, float_precision="round_trip", quotechar='"', sep=",",
                    quoting=csv.QUOTE_NONNUMERIC)

data_list = []                 
for index, row in dfM.iterrows():
   sar_path=  os.path.splitext(row["SAR_LOG_PATH"])[0] + ".csv"
   print sar_path
   dfS = pd.read_csv(os.path.join(basepath, sar_path), header=0, na_filter=True, float_precision="round_trip", quotechar='"', sep=";", quoting=csv.QUOTE_NONNUMERIC)

   subsetDf = dfS[dfS["timestamp"] >= (row["crash_tstamp"]/1000.0)]
   sar_downtime = calculate_biggest_difference(subsetDf)
   last_entry_before_realfi = dfS[dfS["timestamp"] <= (row["real_fi_tstamp"]/1000.0)].max()
   last_entry_before_fi = dfS[dfS["timestamp"] <= (row["fi_tstamp"]/1000.0)].max()
   last_entry_before_crash = dfS[dfS["timestamp"] <= (row["crash_tstamp"]/1000.0)].max()
   
   lat_until_realfi = (row["real_fi_tstamp"]/1000.0) - last_entry_before_realfi["timestamp"]
   lat_until_fi = (row["fi_tstamp"]/1000.0) - last_entry_before_realfi["timestamp"]
   #print (row["real_fi_tstamp"]/1000.0), 100.0 - last_entry_before_realfi["%idle"], 100.0 - last_entry_before_fi["%idle"], 100.0-last_entry_before_crash["%idle"]
   
   smaller_filter = dfS["timestamp"] <= (row["real_fi_tstamp"]/1000.0)
   _1sec_filter = dfS["timestamp"] >= (row["real_fi_tstamp"]/1000.0 - 1)
   _2sec_filter = dfS["timestamp"] >= (row["real_fi_tstamp"]/1000.0 - 2)
   _5sec_filter = dfS["timestamp"] >= (row["real_fi_tstamp"]/1000.0 - 5)
   _10sec_filter = dfS["timestamp"] >= (row["real_fi_tstamp"]/1000.0 - 10)
   
   count_1sec = dfS[smaller_filter & _1sec_filter]["timestamp"].count()
   count_2sec = dfS[smaller_filter & _2sec_filter]["timestamp"].count() 
   count_5sec = dfS[smaller_filter & _5sec_filter]["timestamp"].count()
   count_10sec = dfS[smaller_filter & _10sec_filter]["timestamp"].count()
   
   avg_1sec = 100 -dfS[smaller_filter & _1sec_filter]["%idle"].mean()
   avg_2sec = 100 -dfS[smaller_filter & _2sec_filter]["%idle"].mean() 
   avg_5sec = 100 -dfS[smaller_filter & _5sec_filter]["%idle"].mean()
   avg_10sec = 100 -dfS[smaller_filter & _10sec_filter]["%idle"].mean()
   
   max_1sec = 100 - dfS[smaller_filter & _1sec_filter]["%idle"].min()
   max_2sec = 100 - dfS[smaller_filter & _2sec_filter]["%idle"].min()
   max_5sec = 100 - dfS[smaller_filter & _5sec_filter]["%idle"].min()
   max_10sec = 100 - dfS[smaller_filter & _10sec_filter]["%idle"].min()
   
   min_1sec = 100 - dfS[smaller_filter & _1sec_filter]["%idle"].max()
   min_2sec = 100 - dfS[smaller_filter & _2sec_filter]["%idle"].max()
   min_5sec = 100 - dfS[smaller_filter & _5sec_filter]["%idle"].max()
   min_10sec = 100 - dfS[smaller_filter & _10sec_filter]["%idle"].max()
   
   #print  (row["real_fi_tstamp"]/1000.0 - 10), dfS[smaller_filter & _10sec_filter].count()
   data_list.append({
                     "SAR_LOG_PATH" : row.loc["SAR_LOG_PATH"],
                     "sar_last_load_before_realfi": 100.0 - last_entry_before_realfi["%idle"],
                     "sar_last_load_before_fi": 100.0 - last_entry_before_fi["%idle"],
                     "sar_last_load_before_crash": 100.0 - last_entry_before_crash["%idle"],
                     "sar_lat_until_realfi" : lat_until_realfi,
                     "sar_lat_until_fi" : lat_until_fi,
                     "sar_count1sec" : count_1sec,
                     "sar_count2sec" : count_2sec,
                     "sar_count5sec" : count_5sec,
                     "sar_count10sec" : count_10sec,
                     "sar_load_max_1sec" : max_1sec,
                     "sar_load_max_2sec" : max_2sec,
                     "sar_load_max_5sec" : max_5sec,
                     "sar_load_max_10sec" : max_10sec,
                     "sar_load_min_1sec" : min_1sec,
                     "sar_load_min_2sec" : min_2sec,
                     "sar_load_min_5sec" : min_5sec,
                     "sar_load_min_10sec" : min_10sec,
                     "sar_load_min_1sec" : avg_1sec,
                     "sar_load_min_2sec" : avg_2sec,
                     "sar_load_min_5sec" : avg_5sec,
                     "sar_load_min_10sec" : avg_10sec,
                     "sar_downtime" : sar_downtime
                     })
   # Find last entry before fi
   
#real_fi_tstam
newDf = pd.DataFrame(data_list)
newDf = newDf.set_index("SAR_LOG_PATH")
mergedDf = pd.merge(dfM, newDf, on='SAR_LOG_PATH')
#print mergedDf
mergedDf.to_csv(master_csv, index=False)