import numpy as np
import pandas as pd
import sys
import getopt
import csv

cpu_csv = sys.argv[1]
mem_csv = sys.argv[2]
io_csv = sys.argv[3]
net_csv = sys.argv[4]

df_cpu = pd.read_csv(cpu_csv, header=0, na_filter=True, float_precision="round_trip", quotechar='"', sep=";", quoting=csv.QUOTE_NONNUMERIC)
df_mem = pd.read_csv(mem_csv, header=0, na_filter=True, float_precision="round_trip", quotechar='"', sep=";", quoting=csv.QUOTE_NONNUMERIC)
df_io = pd.read_csv(io_csv, header=0, na_filter=True, float_precision="round_trip", quotechar='"', sep=";", quoting=csv.QUOTE_NONNUMERIC)
df_net = pd.read_csv(net_csv, header=0, na_filter=True, float_precision="round_trip", quotechar='"', sep=";", quoting=csv.QUOTE_NONNUMERIC)

# Start from 75
df_cpu = df_cpu[df_cpu["relts"] >= 75]

print "CPU (min, max, avg, median, 99th percentile"
print "total", (100 - df_cpu["%idle"]).min(), (100 - df_cpu["%idle"]).max(), (100 - df_cpu["%idle"]).mean(), (100 - df_cpu["%idle"]).median(), (100 - df_cpu["%idle"]).quantile(.99)
print "iowait", df_cpu["%iowait"].min(), df_cpu["%iowait"].max(), df_cpu["%iowait"].mean(), df_cpu["%iowait"].median(), df_cpu["%iowait"].quantile(.99)
print "system", df_cpu["%system"].min(), df_cpu["%system"].max(), df_cpu["%system"].mean(), df_cpu["%system"].median(), df_cpu["%system"].quantile(.99)
print "user", df_cpu["%user"].min(), df_cpu["%user"].max(), df_cpu["%user"].mean(), df_cpu["%user"].median(), df_cpu["%user"].quantile(.99)

print "IO (min, max, avg, median, 99th percentile"
print "tps", df_io["tps"].min(), df_io["tps"].max(), df_io["tps"].mean(), df_io["tps"].median(), df_io["tps"].quantile(.99)
print "rtps", df_io["rtps"].min(), df_io["rtps"].max(), df_io["rtps"].mean(), df_io["rtps"].median(), df_io["rtps"].quantile(.99)
print "wtps", df_io["wtps"].min(), df_io["wtps"].max(), df_io["wtps"].mean(), df_io["wtps"].median(), df_io["wtps"].quantile(.99)
# Multiplied by 512 to get raw throughput (a block is 512 bytes)
print "bread/s", df_io["bread/s"].min()*512, df_io["bread/s"].max()*512, df_io["bread/s"].mean()*512, df_io["bread/s"].median()*512, df_io["bread/s"].quantile(.99)*512
print "bwrtn/s", df_io["bwrtn/s"].min()*512, df_io["bwrtn/s"].max()*512, df_io["bwrtn/s"].mean()*512, df_io["bwrtn/s"].median()*512, df_io["bwrtn/s"].quantile(.99)*512

print "Net (min, max, avg, median, 99th percentile"
print "oseg/s", df_net["oseg/s"].min(), df_net["oseg/s"].max(), df_net["oseg/s"].mean(), df_net["oseg/s"].median(), df_net["oseg/s"].quantile(.99)
print "iseg/s", df_net["iseg/s"].min(), df_net["iseg/s"].max(), df_net["iseg/s"].mean(), df_net["iseg/s"].median(), df_net["iseg/s"].quantile(.99)