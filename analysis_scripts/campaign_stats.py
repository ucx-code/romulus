import pandas as pd
import sys

def abstract_stats(df, field):
   print "Mean of", field, df[field].mean()
   print "Median of", field, df[field].median()
   print "Max of", field, df[field].max()
   print "Min of", field, df[field].min()
   print "Std. of", field, df[field].std()
   print "Var. of", field, df[field].var()
   print "Q1 of ", field, df[field].quantile(0.25)
   print "Q3 of ", field, df[field].quantile(0.75)
   print ""

data_file = sys.argv[1]
if len(sys.argv) == 2:
   f = sys.argv[1]
   df = pd.read_csv(f)
   abstract_stats(df, "downtime")
   abstract_stats(df, "dur_mean")
   abstract_stats(df, "tot_non200")
   #abstract_stats(df, "noresp_interval")