import sys
import pandas as pd
import scipy.stats

filename = sys.argv[1]
df = pd.read_csv(filename, header=0, na_filter=True, na_values=None, float_precision="round_trip")

df["downtime"] = (df["vm1_downtime"] + df["vm2_downtime"] +df["vm3_downtime"] +df["vm4_downtime"]) / 4.0
df = df[~df["downtime"].isna()]

df_v2 = df[df["vcpus"] == 2]
df_v3 = df[df["vcpus"] == 3]
df_v4 = df[df["vcpus"] == 4]
df_v6 = df[df["vcpus"] == 6]
df_v12 = df[df["vcpus"] == 12]
print df_v2["downtime"].mean(), df_v3["downtime"].mean(), df_v4["downtime"].mean(), df_v6["downtime"].mean(), df_v12["downtime"].mean()
print df_v2["downtime"].min(), df_v3["downtime"].min(), df_v4["downtime"].min(), df_v6["downtime"].min(), df_v12["downtime"].min()
print df_v2["downtime"].max(), df_v3["downtime"].max(), df_v4["downtime"].max(), df_v6["downtime"].max(), df_v12["downtime"].max()
print df_v2["downtime"].var(), df_v3["downtime"].var(), df_v4["downtime"].var(), df_v6["downtime"].var(), df_v12["downtime"].var()

print scipy.stats.f_oneway(df_v2["downtime"], df_v3["downtime"], df_v4["downtime"], df_v6["downtime"], df_v12["downtime"])