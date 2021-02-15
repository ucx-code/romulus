# Uses values found in master.csv and ping's to infer and append other information
import pandas as pd
import sys
import os
import csv

master_csv = sys.argv[1]

dfM = pd.read_csv(master_csv, header=0, na_filter=True, float_precision="round_trip", quotechar='"', sep=",",
                    quoting=csv.QUOTE_NONNUMERIC)

#dfM["fi_tsc"] = dfM["fi_tsc"].fillna(0)
"""dfM["crash_tstamp"] = dfM["crash_tstamp"].replace("", None)
dfM["fi_tstamp"] = dfM["fi_tstamp"].replace("", None)
dfM["fi_tsc"] = dfM["fi_tsc"].replace("", None)
dfM["fi_tstamp"] = dfM["fi_tstamp"].astype('float64')
dfM["crash_tstamp"] = dfM["crash_tstamp"].astype('float64')
dfM["fi_tsc"] = dfM["fi_tsc"].astype('float64')  """   

# Latency between fi and "crash detection"
dfM["detection_lat"] = dfM["crash_tstamp"] - dfM["fi_tstamp"]

#print dfM["detection_lat"]

# TODO: Fault injection latency. difference between SSH command and real injection

dfM["real_fi_tstamp"] = dfM["dolma_tsc"] + ((dfM["fi_tsc"] - dfM["l0_rdtsc"]) / 2194865.0)
dfM["fi_lat"] = dfM["real_fi_tstamp"] - dfM["fi_tstamp"]
dfM["real_detection_lat"] = dfM["crash_tstamp"] - dfM["real_fi_tstamp"]

# TODO: Load avg in last 1, 2, 5, 10 seconds before FI

dfM.to_csv(master_csv, index=False)