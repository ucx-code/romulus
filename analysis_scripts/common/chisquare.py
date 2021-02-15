import scipy.stats
import sys
import pandas as pd
filename = sys.argv[1]
df = pd.read_csv(filename, header=0, na_filter=True, na_values=None, float_precision="round_trip")
print df.groupby("vcpus")["count_recovered_vms_solr"].sum() /  df.groupby("vcpus")["count_recovered_vms_solr"].count()

observed = df.groupby("vcpus")["count_recovered_vms_solr"].sum()
total = df.groupby("vcpus")["count_recovered_vms_solr"].count()
expected_ratio = df["count_recovered_vms_solr"].sum() / df["count_recovered_vms_solr"].count()
print observed
print total
print expected_ratio
expected = df.groupby("vcpus")["count_recovered_vms"].count() * expected_ratio
print len(observed)
print scipy.stats.chisquare(f_obs=observed, f_exp=expected) # df = (r-1)*(c-1);  r=n. populations c=levels of categoric variable. r=5
print scipy.stats.chi2.ppf(0.995, len(expected)-1)

# Contigency table is like this
#                Recovered: True or False
# vcpus
# 2, 3, 4, 6, 12

# df = (2-1) * (5-1)