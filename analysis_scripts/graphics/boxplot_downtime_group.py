import seaborn as sns

import matplotlib.pyplot as plt
import matplotlib as mpl
import matplotlib.patches as mpatches

import numpy as np
import pandas as pd
import sys
from math import *

#plt.style.use('seaborn-dark')
sns.set()
sns.set_style("whitegrid")

filename = sys.argv[1]
x_axis = sys.argv[2]
group_col = sys.argv[3]

df = pd.read_csv(filename, header=0, na_filter=True, float_precision="round_trip")

uniqvals = df[x_axis].unique()
uniqvals.sort()
print uniqvals

uniqgroups = df[group_col].unique()
uniqgroups.sort()


fig, ax = plt.subplots()
dfvm1 = df.copy()
dfvm2 = df.copy()
dfvm3 = df.copy()
dfvm4 = df.copy()
dfvm1.drop(columns=["vm2_downtime", "vm3_downtime", "vm4_downtime"], inplace=True)
dfvm2.drop(columns=["vm1_downtime", "vm3_downtime", "vm4_downtime"], inplace=True)
dfvm3.drop(columns=["vm1_downtime", "vm2_downtime", "vm4_downtime"], inplace=True)
dfvm4.drop(columns=["vm1_downtime", "vm2_downtime", "vm3_downtime"], inplace=True)
dfvm1["which_vm"] = "VM 1"
dfvm2["which_vm"] = "VM 2"
dfvm3["which_vm"] = "VM 3"
dfvm4["which_vm"] = "VM 4"
dfvm1.rename(columns={"vm1_downtime": "downtime"}, inplace=True)
dfvm2.rename(columns={"vm2_downtime": "downtime"}, inplace=True)
dfvm3.rename(columns={"vm3_downtime": "downtime"}, inplace=True)
dfvm4.rename(columns={"vm4_downtime": "downtime"}, inplace=True)
dfvm1["downtime"] = dfvm1["downtime"] - (dfvm1["detection_lat"] / 1000.0)
dfvm2["downtime"] = dfvm2["downtime"] - (dfvm2["detection_lat"] / 1000.0)
dfvm3["downtime"] = dfvm3["downtime"] - (dfvm3["detection_lat"] / 1000.0)
dfvm4["downtime"] = dfvm4["downtime"] - (dfvm4["detection_lat"] / 1000.0)
df = pd.concat([dfvm1, dfvm2, dfvm3, dfvm4])
sns.boxplot(x="vcpus", y="downtime", hue="which_vm", data=df, saturation=0.9)
#sns.swarmplot(x="vcpus", y="downtime", hue="which_vm",data=df, color=".25")
print "Min", dfvm1["downtime"].min(), dfvm2["downtime"].min(), dfvm3["downtime"].min(), dfvm4["downtime"].min()
print "Max", dfvm1["downtime"].max(), dfvm2["downtime"].max(), dfvm3["downtime"].max(), dfvm4["downtime"].max()
print "Avg", dfvm1["downtime"].mean(), dfvm2["downtime"].mean(), dfvm3["downtime"].mean(), dfvm4["downtime"].mean()

ax.xaxis.set_tick_params(labelsize=14)
ax.yaxis.set_tick_params(labelsize=14)
plt.xticks(np.arange(len(uniqvals)))
ax.set_xticklabels([int(x) for x in uniqgroups])
plt.xlabel("N. of vCPUs used by the hypervisor", fontsize=15)
plt.ylabel("Downtime of Solr sevice (s)", fontsize=15)
plt.ylim(ymin=0, ymax=450)
ax.legend(ncol=4, fontsize=12,frameon=True)
plt.savefig('boxplot_group_downtime.pdf', bbox_inches='tight')
plt.show()