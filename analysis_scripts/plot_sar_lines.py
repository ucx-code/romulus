import sys
import csv
import matplotlib.pyplot as plt
import pandas as pd
import os
#import numpy as np

path = sys.argv[1]
x_var = sys.argv[2]
master_file = sys.argv[3]
sar_path = sys.argv[4]
ping_path = sys.argv[5]

df = pd.read_csv(path, header=0, na_filter=True, float_precision="round_trip", quotechar='"', sep=";",
                    quoting=csv.QUOTE_NONNUMERIC)
dfM = pd.read_csv(master_file, header=0, na_filter=True, float_precision="round_trip", quotechar='"', sep=",",
                    quoting=csv.QUOTE_NONNUMERIC)
dfP = pd.read_csv(ping_path, header=0, na_filter=True, float_precision="round_trip", quotechar='"', sep=",",
                    quoting=csv.QUOTE_NONNUMERIC)                    
# Filter dfM by sar name
dfM = dfM[dfM["SAR_LOG_PATH"] == os.path.basename(sar_path)]
#print  master_file,dfM["wl_tstamp"]
wl_tstamp = float(dfM["wl_tstamp"])/1000.0
fi_tstamp = float(dfM["fi_tstamp"])/1000.0
realfi_tstamp = float(dfM["real_fi_tstamp"])/1000.0
#print float(dfM["base_save_end"])
crash_tstamp = float(dfM["crash_tstamp"])/1000.0
basesave_start_tstamp = float(dfM["base_save_start"])/1000.0
basesave_end_tstamp = float(dfM["base_save_end"])/1000.0
reboot_end = float(dfM["REBOOT_END"] - dfM["REBOOT_DUR"])/1000.0

#print df.columns
fig, axs = plt.subplots(nrows=1, ncols=1, sharex=True)
plt.axvline(x=wl_tstamp, label='wl start', c='g', linestyle='dashed')
plt.axvline(x=fi_tstamp, label="fi", c="r", linestyle='dashed')
plt.axvline(x=realfi_tstamp, label="fi", c="r", linestyle='dotted')
plt.axvline(x=crash_tstamp)
plt.axvline(x=basesave_start_tstamp, c="y", linestyle='dashed')
plt.axvline(x=basesave_end_tstamp, c="y", linestyle='dashed')
#plt.axvline(x=reboot_end, c="orange", linestyle='dashed')

#plt.axvline(x=fi_tstamp+140, c="y", linestyle='solid')

for ts in dfP[dfP["ret_code"] == 1.0]["ts"]:
   plt.axvline(x=ts, c="gray", linestyle='dotted')
   
df = df.drop(["relts"], axis=1)


# Fill empty timestamps with empty values
ndf = df
for i in xrange(int(df[x_var].min()),  int(df[x_var].max()) +1):
   if df[df[x_var] == i].empty:
      ndf = ndf.append({x_var: i}, ignore_index=True)
df = ndf.sort_values(by=x_var)
#print ndf
      
for i in df.columns:
    if i != x_var:# and i[0] != "%":
        #df[i] = df[i].astype('float64')
        #df[i] = df[i].replace(0.0, None)
        #print len(df[df[i] == None]), len(df[x_var])
        #x2 = np.ma.masked_where(df[i] == None, df[x_var])
        #y2 = np.ma.masked_where(df[i] == None, df[i])
        axs.plot(df[x_var], df[i], '-o')
        #axs.plot(x2, y2)#, 'o')
plt.legend()
plt.show()
