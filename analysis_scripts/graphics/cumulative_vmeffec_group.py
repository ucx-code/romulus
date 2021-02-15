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

#sns.set_palette("RdYlGn")
#sns.set_palette("YlOrRd")

filename = sys.argv[1]
x_axis = sys.argv[2]
group_col = sys.argv[3]

df = pd.read_csv(filename, header=0, na_filter=True,na_values=None, float_precision="round_trip")

df.dropna(subset=[x_axis], inplace=True)
uniqvals = df[x_axis].unique()
uniqvals.sort()
print uniqvals

uniqgroups = df[group_col].unique()
uniqgroups.sort()

print uniqgroups

fig, ax = plt.subplots()
for group in uniqgroups:

   data = []
   filt_group = df[group_col] == group 
   n, bins, patches = ax.hist(df[filt_group][x_axis],len(uniqvals), cumulative=-1, density=True, label="%d vCPUs" % group, linewidth=1.8, histtype='step' ) #  
   
ax.xaxis.set_tick_params(labelsize=13)
ax.yaxis.set_tick_params(labelsize=13)
plt.xticks(np.arange(len(uniqvals)))
plt.xlabel("Cumulative sum of recovered VMs", fontsize=14)
plt.ylabel("Probability in [0; 1]", fontsize=14)
plt.legend(fontsize=12, loc="upper center", bbox_to_anchor=(0.5, 1.19), ncol=3
)
plt.savefig('cumulative_hist.pdf', bbox_inches='tight')
plt.show()