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

df = pd.read_csv(filename, header=0, na_filter=True, float_precision="round_trip")

# We just want to look at those that saw recovery of Solr
df = df[df["recovered"] == 1]


uniqvals = df[x_axis].unique()
uniqvals.sort()
print uniqvals

# Divide downtimes
df_solr = df.copy()
df_sar = df.copy()
df_solr.drop(columns=["sar_downtime"], inplace=True)
df_sar.drop(columns=["downtime"], inplace=True)
df_sar.rename(columns={"sar_downtime": "downtime"}, inplace=True)
df_sar["type"] = "Operating System"
df_solr["type"] = "Solr"
df = pd.concat([df_sar, df_solr])
print df_sar["downtime"].max(), df_solr["downtime"].mean()
fig, ax = plt.subplots()
#df["downtime"] = df["downtime"] - (df["detection_lat"] / 1000.0)
sns.boxplot(x="type", y="downtime", hue="profile", data=df, saturation=0.9)
print "Min", df_sar["downtime"].min(), df_solr["downtime"].min() 
print "Max", df_sar["downtime"].max(), df_solr["downtime"].max()
print "Avg", df_sar["downtime"].mean(), df_solr["downtime"].mean()

ax.xaxis.set_tick_params(labelsize=14)
ax.yaxis.set_tick_params(labelsize=14)
plt.xticks(np.arange(len(uniqvals)))
#ax.set_xticklabels([int(x) for x in uniqgroups])
plt.xlabel("Recovered component", fontsize=15)
plt.ylabel("Downtime (s)", fontsize=15)
plt.ylim(0)
ax.legend(ncol=4, fontsize=14,frameon=True)
plt.savefig('boxplot_singleVM_downtime.pdf', bbox_inches='tight')
plt.show()