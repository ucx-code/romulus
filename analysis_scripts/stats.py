import pandas as pd
import sys
import os
import getopt
import csv
#from .. import ping_recovery_detector as prd

def extract_starttime(filename):
   filename = os.path.basename(filename)
   print filename, filename.count("_")
   if (filename.count("_") == 2): # Group data
      (_,_,start) = filename.split("_")
   elif (filename.count("_") == 1): # Single VM data
      (_,start) = filename.split("_")
   else:
      assert(1) # Unexpected
   return int(start.split(".")[0])
   
def prepend_to_key(d, prep):
   return {prep+"_"+k: v for k, v in d.items()}

# Sometimes solr may never start on a VM (too much overcommiting of VMs to vCPUs, slow disk I/O)
# we want to be able to filter these out
def check_solr_has_started(df):
   return {'solr_started' : str(bool(len(df[df["code"] == 200].index) > 0)) }

# 
#def call_ping_detection(df):
#   print prd.check_ping_detection(df[""])

def verify_recovery(df):
   # It should be enough to check whether the very last request had a response
      s = df.sort_values(by="sent_ts", ascending=False)
      if (s.iloc[0, :]["code"] == 200):
         print "Recovered"
         return True
      else:
         print "Did not recover"
         return False

def find_last_answered_request(df):
   answered = df[df["code"] == 200].sort_values(by="received_ts", ascending=False)
   #print answered
   last_ans_ts = answered.iloc[0,:]["received_ts"]
   all_reqs = df.sort_values(by="sent_ts", ascending=False)
   last_req_ts = all_reqs.iloc[0,:]["sent_ts"]
   print "No response received during", last_req_ts-last_ans_ts
   return { "noresp_interval" : last_req_ts-last_ans_ts }
   
def find_biggest_downtime(df):
   s = df.sort_values(by="received_ts")
   downtime = s["received_ts"].diff().max()
   print downtime
   return { "downtime" : downtime }
   
def calc_basic_stats(df):

   filter_code_200 = df["code"] == 200
   filter_code_500 = df["code"] == 500
   filter_code_nan = df["code"].isna()
   filter_code_else = ~filter_code_200 & ~filter_code_500 & ~filter_code_nan
   df["content_matches"] =   df["content_matches"].astype('string')

   filter_content_matches =  (df["content_matches"] == 'True') | (df["content_matches"] == '1') | (df["content_matches"] == '1.0') 
   filter_content_doesnt_match_on_200 = filter_code_200 & ~filter_content_matches
   

   non_200_responses = len(df[~filter_code_200].index)
   _200_responses = len(df[filter_code_200].index)
   _500_responses = len(df[filter_code_500].index)
   nan_responses = len(df[filter_code_nan].index)
   other_responses = len(df[filter_code_else].index)
   
   corrupted_responses = len(df[~filter_content_matches].index)
   corrupted_responses_200 = len(df[filter_content_doesnt_match_on_200].index)
   
   # Common exceptions: No route to host, timed out, Connection refused
   filter_noroutetohost = df["exception"] == "[Errno 113] No route to host"
   filter_timedout = df["exception"] == "timed out"
   filter_conrefused = df["exception"] == "[Errno 111] Connection refused"
   filter_otherexceptions = ~df["exception"].isna() & ~(filter_noroutetohost | filter_timedout | filter_conrefused)
   
   tot_excp = len(df[~df["exception"].isna()].index)
   tot_excp_noroutetohost = len(df[filter_timedout].index)
   tot_excp_timedout = len(df[filter_noroutetohost].index)
   tot_excp_conrefused = len(df[filter_conrefused].index)
   tot_excp_otherexceptions = len(df[filter_otherexceptions].index)
   
   print "Total sent requests %d" % df["sent_ts"].count()
   print "Total recv responses %d" % df["received_ts"].count()
   print "Total non-200 responses %d" % non_200_responses
   print "Total corrupted responses %d" % corrupted_responses
   return {
            "sent_reqs" : df["sent_ts"].count(),
            "recv_reqs" : df["received_ts"].count(),
            "tot_non200" : non_200_responses,
            "tot_200" : _200_responses,
            "tot_500" : _500_responses,
            "tot_nan" : nan_responses,
            "tot_othercodes" : other_responses,
            "tot_corrupted" : corrupted_responses,
            "tot_corrupted_on_200" : corrupted_responses_200,
            "unique_clients" : len(df["client_id"].unique()),
            "tot_exceptions": tot_excp,
            "tot_excp_noroute": tot_excp_noroutetohost, 
            "tot_excp_timedout": tot_excp_timedout, 
            "tot_excp_conrefused": tot_excp_conrefused, 
            "tot_excp_others": tot_excp_otherexceptions, 
          }
   
def calc_perf_stats(df):
   print "Duration"
   print "\tMin. %.3fs" % df["dur"].min()
   print "\tMax. %.3fs" % df["dur"].max()
   print "\tAvg. %.3fs" % df["dur"].mean()
   print "\tMed. %.3fs" % df["dur"].median()
   print "\tStd. %.3fs" % df["dur"].std()
   print "\tVar. %.3f" % df["dur"].var()
   print "\tQuartiles 1st %.3f 3rd %.3f" % (df["dur"].quantile(0.25), df["dur"].quantile(0.75))
   return {
            "dur_min": df["dur"].min(),
            "dur_max": df["dur"].max(),
            "dur_mean": df["dur"].mean(),
            "dur_median": df["dur"].median(),
            "dur_std": df["dur"].std(),
            "dur_var": df["dur"].var(),
            "dur_q1": df["dur"].quantile(0.25),
            "dur_q3": df["dur"].quantile(0.75)
          }

data_dict = {}
prep_key = None
master_file = None
run_filename = None
master = None
extend_columns = False
do_ping_detection = False
opts, args = getopt.getopt(sys.argv[1:], "d:m:r:k:ap")
for opt, arg in opts:
   if opt == "-d":
      data_file = arg #d
   elif opt == "-m":
      master_file = arg
   elif opt == "-p": # Multi-vm ping detection
      do_ping_detection = True
   elif opt == "-r":
      run_filename = arg
      filename = os.path.basename(run_filename)
   elif opt == "-k":
      prep_key = arg
   elif opt == "-a":
      extend_columns = True
   else:
      print "Unknown prop", opt, arg
      sys.exit(-1)

try:
   master = pd.read_csv(master_file, header=0, na_filter=True, float_precision="round_trip", quotechar='"', sep=",", quoting=csv.QUOTE_NONNUMERIC)
except Exception as e:
   print e, data_file, run_filename
   master = pd.DataFrame()
data_dict.update({"filename": filename})


print os.path.basename(run_filename)
df = pd.read_csv(data_file, header=0, na_filter=True, float_precision="round_trip", quotechar='"', sep=",", quoting=csv.QUOTE_NONNUMERIC)
print "Statistics for", data_file
data_dict.update(calc_basic_stats(df))
data_dict.update(calc_perf_stats(df))
data_dict.update(check_solr_has_started(df))
if do_ping_detection:
   data_dict.update(call_ping_detection(df))
   
if (verify_recovery(df)):
   data_dict.update({"recovered" : 1})
   data_dict.update(find_biggest_downtime(df[df["code"] == 200]))
else:
   data_dict.update({"recovered" : 0})
   try:
      data_dict.update(find_last_answered_request(df))
   except Exception as e:
      print e
      data_dict.update({ "noresp_interval" : -1 })

if prep_key != None:
   data_dict = prepend_to_key(data_dict, prep_key)

data_dict.update({"start_time": extract_starttime(filename)})

if master is not None:
   p = pd.Series(data_dict)

   if not extend_columns:
      master = master.append(p, ignore_index=True)
   else:
      #try:
      #master = pd.concat([master, pd.DataFrame([p])]) #, on='start_time')
      #print master["start_time"], data_dict["start_time"]
      #master = pd.concat([master, pd.DataFrame([p])], axis=1, join='outer')
      master = master.merge(right=pd.DataFrame([p]), how="left", on='start_time')
      #master[master["start_time"] == extract_starttime(filename)].join(pd.DataFrame([p]).set_index("start_time"), on="start_time") #, on='start_time')
      #except:
      #   master = master.append(p, ignore_index=True)
   #print master[master["start_time"] == extract_starttime(filename)].merge(pd.DataFrame([p]), on='start_time')
   #print master["start_time"]
   #print master.merge(pd.DataFrame([p]), on='start_time')
   
   master.to_csv(master_file, index=False)

"""crash_tstamp = int(sys.argv[2]) / 1000.0
print df["received_ts"].min()
print crash_tstamp
print df["received_ts"].max()
print "Before migration"
calc_basic_stats(df[df["received_ts"] <= crash_tstamp])
calc_perf_stats(df[df["received_ts"] <= crash_tstamp])
print "After migraton"
calc_basic_stats(df[df["sent_ts"] > crash_tstamp])
calc_perf_stats(df[df["sent_ts"] > crash_tstamp])"""