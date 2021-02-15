import seaborn as sns

import matplotlib.pyplot as plt
import matplotlib as mpl
import matplotlib.patches as mpatches
import numpy as np
import pandas as pd
import sys
import getopt
import csv

def add_line(df, x_var, y_var, axs, linestyle='-', lab='', c=None):
   return axs.plot(df[x_var], df[y_var], linestyle, label=lab, color=c, linewidth=1)
   
def fill_na(df, x_var):
   # Fill empty timestamps with empty values
   ndf = df
   for i in xrange(int(df[x_var].min()),  int(df[x_var].max()) +1):
      if df[df[x_var] == i].empty:
         ndf = ndf.append({x_var: i}, ignore_index=True)
   return ndf.sort_values(by=x_var)
   
#plt.style.use('seaborn-dark')
sns.set()
sns.set_style("dark")

cpu_csv = []
mem_csv = []
io_csv = []
net_csv = []
display = "together"
mylabels = []
startX = 0

opts, args = getopt.getopt(sys.argv[1:], "c:m:i:n:d:l:x:")
for opt, arg in opts:
   if opt == "-c": # CPU file
      print arg
      cpu_csv.append(arg)
   elif opt == "-m": # MEM file
      mem_csv.append(arg)
   elif opt == "-i": # IO file
      io_csv.append(arg)
   elif opt == "-n": # net file
      net_csv.append(arg)
   elif opt == "-d": # display type
      display = arg
   elif opt == "-l": # labels for legends
      mylabels.append(arg)
   elif opt == '-x': # X axis starting point
      startX = int(arg)
   else:
      print "Unknown prop", opt, arg
      sys.exit(-1)
      
if cpu_csv != []:
   df_cpu = []
   for i in cpu_csv:
      df_cpu.append(pd.read_csv(i, header=0, na_filter=True, float_precision="round_trip", quotechar='"', sep=";", quoting=csv.QUOTE_NONNUMERIC))
      
if mem_csv != []:
   df_mem = []
   for i in mem_csv:
      df_mem.append(pd.read_csv(i, header=0, na_filter=True, float_precision="round_trip", quotechar='"', sep=";", quoting=csv.QUOTE_NONNUMERIC))
if io_csv != []:
   df_io = []
   for i in io_csv:
      df_io.append(pd.read_csv(i, header=0, na_filter=True, float_precision="round_trip", quotechar='"', sep=";", quoting=csv.QUOTE_NONNUMERIC))
      
if net_csv != []:
   df_net = []
   for i in net_csv:
      df_net.append(pd.read_csv(i, header=0, na_filter=True, float_precision="round_trip", quotechar='"', sep=";", quoting=csv.QUOTE_NONNUMERIC))           
                                   
#print df.columns
if display == "together":
   fig, axs = plt.subplots(nrows=1, ncols=1, sharex=False)
   axs_cpu = axs_mem = axs_io = axs_net = axs
elif display == "in4":
   #fig, axs = plt.subplots(nrows=2, ncols=2, sharex='col', sharey='row', gridspec_kw={'hspace':0, 'wspace':0})
   fig, axs = plt.subplots(nrows=2, ncols=2)
   axs_cpu = axs[0][0]
   axs_mem = axs[0][1]
   axs_io = axs[1][0]
   axs_net = axs[1][1]
   """if startX != None:
      axs_cpu.set_xlim(xmin=startX)
      axs_mem.set_xlim(xmin=startX)
      axs_io.set_xlim(xmin=startX)
      axs_net.set_xlim(xmin=startX)"""
   

def_lines = ('-', '--')
def_colors = (sns.xkcd_rgb["tangerine"], sns.xkcd_rgb["denim blue"], sns.xkcd_rgb["medium green"] )
#df = df.drop(["relts"], axis=1)
if cpu_csv != []:
   print df_cpu
   for pos in xrange(len(df_cpu)):
      df = df_cpu[pos]
      df["%usage"] = 100 - df["%idle"]
      #add_line(df, "relts", "%user", axs_cpu, def_lines[1], lab="user (%s)" % mylabels[pos], c=def_colors[pos])
      add_line(df, "relts", "%usage", axs_cpu, def_lines[0], lab="total (%s)" % mylabels[pos], c=def_colors[pos])
      #add_line(df, "relts", "%system", axs_cpu, def_lines[pos], lab="system (%s)" % mylabels[pos], c=def_colors[2])

      
   #axs_cpu.legend(handles=bars, labels=legends)
   handles, labels = axs_cpu.get_legend_handles_labels()
   axs_cpu.legend(handles, labels, fontsize="small", frameon=True, loc=2)
   axs_cpu.set_title("CPU")
   axs_cpu.set_ylim(ymin=-1, ymax=105)
   axs_cpu.set_xlim(xmin=startX, xmax=df_cpu[0]["relts"].max()+1)
   axs_cpu.set_xlabel("Time (s)")
   axs_cpu.set_ylabel("Usage (%)")
  
if mem_csv != []:
   for pos in xrange(len(df_mem)):
      df = df_mem[pos]
      add_line(df, "relts", "%memused", axs_mem, def_lines[0], lab="total (%s)" % mylabels[pos], c=def_colors[pos])
   handles, labels = axs_mem.get_legend_handles_labels()
   axs_mem.legend(handles, labels, fontsize="small", frameon=True)
   axs_mem.set_title("Memory")
   axs_mem.set_ylim(ymin=-1, ymax=105)
   axs_mem.set_xlim(xmin=startX, xmax=df_mem[0]["relts"].max()+1)
   axs_mem.set_xlabel("Time (s)")
   axs_mem.set_ylabel("Usage (%)")
   
if io_csv != []:
   for pos in xrange(len(df_io)):
      df = df_io[pos]
      b1, = add_line(df, "relts", "rtps", axs_io, def_lines[0], lab="reads (%s)" % mylabels[pos], c=def_colors[pos])
      #b2, = add_line(df, "relts", "wtps", axs_io, def_lines[1], lab="writes (%s)" % mylabels[pos], c=def_colors[pos])
   handles, labels = axs_io.get_legend_handles_labels()
   axs_io.legend(handles, labels, fontsize="small", frameon=True)
   axs_io.set_title("Disk I/O")
   axs_io.set_ylim(ymin=-1, ymax=105)
   axs_io.set_xlim(xmin=startX, xmax=df_io[0]["relts"].max()+1)
   axs_io.set_xlabel("Time (s)")
   axs_io.set_ylabel("Transactions per second")
   
if net_csv != []:
   for pos in xrange(len(df_net)):
      df = df_net[pos]
      #b1, = add_line(df, "relts", "iseg/s", axs_net, def_lines[0], lab="received (%s)" % mylabels[pos], c=def_colors[pos])
      b2, = add_line(df, "relts", "oseg/s", axs_net, def_lines[0], lab="sent (%s)" % mylabels[pos], c=def_colors[pos])
   handles, labels = axs_net.get_legend_handles_labels()
   axs_net.legend(handles, labels, fontsize="small", frameon=True)
   axs_net.set_title("Network")
   axs_net.set_xlim(xmin=startX, xmax=df_net[0]["relts"].max()+1)
   axs_net.set_xlabel("Time (s)")
   axs_net.set_ylabel("Segments per second")
      
#plt.legend()
plt.tight_layout(pad=0.1)
plt.savefig('sar_plot.pdf', bbox_inches='tight')
plt.show()
