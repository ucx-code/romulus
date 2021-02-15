import seaborn as sns

import matplotlib.pyplot as plt
import matplotlib as mpl
import matplotlib.patches as mpatches

import numpy as np
import pandas as pd
import sys
from math import *
#from statsmodels.stats import proportion

def binomial_prob(p, n ,k):
   # p - prob of single throw
   # n - total throws
   # k - how many "true" we looking for
   return ( (factorial(n) / (factorial(k) * factorial(n-k))) * pow(p, k) * pow((1-p), (n-k)) )
   
# Frequency side-by-side-bars
sns.set()
sns.set_style("whitegrid")

#sns.set_palette("RdYlGn")
sns.set_palette("YlOrRd")
#sns.set_context('paper')
#colors=plt.get_cmap("gray")(np.linspace(0,1,6))
#plt.rcParams['axes.prop_cycle'] = plt.cycler(color=colors)

filename = sys.argv[1]
x_axis = sys.argv[2]
group_col = sys.argv[3]

df = pd.read_csv(filename, header=0, na_filter=True, na_values=None, float_precision="round_trip")


df.dropna(subset=[x_axis], inplace=True)
#print df[x_axis]
uniqvals = df[x_axis].unique()
uniqvals.sort()
print uniqvals 

uniqgroups = df[group_col].unique()
uniqgroups.sort()

print uniqgroups

#mpl.rcParams['patch.force_edgecolor'] = True
fig, axs = plt.subplots(nrows=1, ncols=1, sharex=False, figsize=(12,6))
bars = []
legends = []
for group in uniqgroups:

   data = []
   filt_group = df[group_col] == group 
   total = len(df[filt_group].index) * 1.0
   recovered_vms = 0
   for val in uniqvals:
      filt_val = df[x_axis] == val
      freq = len(df[filt_group & filt_val].index)
      recovered_vms = recovered_vms + (freq * val)
      #error = proportion.proportion_confint(freq, total, 0.1, 'normal')
      #print error
      
      data.append((freq / total) * 100.0)
   legends.append("%d vCPUs" % group)
   b = axs.bar(np.arange(len(uniqvals)) + 0.17 * (list(uniqgroups).index(group) + 1), data, 0.17, edgecolor="b", linewidth=0.5)
   bars.append(b)
   print recovered_vms, total

# Binomial with p=0.5
#data = []
#for val in uniqvals:
#   data.append(binomial_prob(0.5, 4, val))
#   legends.append("Binom. p=0.5")
#b = axs.bar(np.arange(len(uniqvals)) + 0.10 * 5, data, 0.10)
#bars.append(b)

axs.legend(bars, legends, fontsize=19)
axs.set_xticks(np.arange(len(uniqvals)) + (0.17*6) / 2.0)

axs.xaxis.set_tick_params(labelsize=18)
axs.yaxis.set_tick_params(labelsize=18)
axs.set_ylabel('Percentage (%)', fontsize=20)
axs.set_xlabel('N. of recovered VMs per run', fontsize=20)
#axs.set_xticklabels(uniqvals)
axs.set_xticklabels([str(x) for x in xrange(len(uniqvals))])
#axs.set_ylim(ymin = 0, ymax=50)	
plt.yticks(np.arange(0, 50.000001, 5))
#axs[2] = do_registers_hcall(axs[2], "Hypercall")
#plt.tight_layout()
plt.savefig('group_fi.pdf', bbox_inches='tight')

plt.show()