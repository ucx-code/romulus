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
df = pd.read_csv(filename, header=0, na_filter=True, float_precision="round_trip")



p_list = []
#y_var = sys.argv[3]
x_var = sys.argv[2]
y_vars = ["flow_migration","hypercall", "restore", ]
labels = {"flow_migration":"VM state migration", "restore": "Restore VM in L1 B", "hypercall":"Memory migration"}

def estimate_time_at_pages(target_pages):
   return target_pages * 0.14550132304909 / 10000.0 # Obtained from the average of the average hypercall time / pages 
   
def forward(x):
   return int(df[df[x_var] == x]["migr_totalPages"].head(n=1))
   
def inverse(x):
   return int(df[df["migr_totalPages"] == x][x_var].head(n=1))
   
print forward(2000), inverse(1026092)

fig, axs = plt.subplots(constrained_layout=True)

# Convert from ms to s to make reading easier
for y_var in y_vars:
   df[y_var] = df[y_var] /1000.0
   means = df.groupby(x_var)[y_var].mean() 
   print means
   #p_list.append(ax.plot(means, '-o', label=labels[y_var]))
   # Lineplot does confidence interval right away...
   sns.lineplot(x=x_var, y=y_var, data=df, err_style="band", marker="o", ci=99.99,  label=labels[y_var])
hypothetic_superpages = pd.DataFrame([[1000, 3150], [2000, 3651], [3000, 3640], [4000, 3628],  [5000, 3617],  [6000, 3606],  [7000, 3595],  [8000, 3584]], columns=["x", "pages"])
hypothetic_superpages["y"] = estimate_time_at_pages(hypothetic_superpages["pages"])
print hypothetic_superpages
plt.plot(hypothetic_superpages["x"], hypothetic_superpages["y"], linestyle='--',marker=".", label="", color='darkorange')
print (df["flow_migration"] - df["hypercall"] + df["restore"]).min(), (df["flow_migration"] - df["hypercall"]+ df["restore"]).mean(), (df["flow_migration"] - df["hypercall"]+ df["restore"]).max()

# Needs Matplot 3.1 or later
#secax = ax.secondary_xaxis('top', functions=(forward, inverse))
#secax.set_xlabel('4K pages')
axs.xaxis.set_tick_params(labelsize=16)
axs.yaxis.set_tick_params(labelsize=16)
plt.xlabel('VM Memory Size (Mb)', fontsize=20)
plt.ylabel('Time (s)', fontsize=20)
plt.legend(fontsize=18)
plt.savefig('impl_performance.pdf', bbox_inches='tight')
plt.show()