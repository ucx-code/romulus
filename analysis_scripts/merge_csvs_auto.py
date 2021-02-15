# Merge all the CSVss
import pandas as pd
import sys 
import csv
import os

def iterate_csvs(rootdir):
	csv_list = []
	for subdir, dirs, files in os.walk(rootdir):
    		for f in files:
			if "csv" in f:
				csv_list.append(os.path.join(rootdir, f))
	return csv_list

if (len(sys.argv) > 1):
	path = sys.argv[1]

	# Load all the CSVs with panda
	data_list = []
	for filename in iterate_csvs(path):
		print filename
		try:
                	df = pd.read_csv(filename, header=0, na_filter=False, float_precision="round_trip")
	        except Exception as e:
        	        print "Failed to read CSV file with exception", e

	        #for k, v in schema.iteritems():
        	        #print k, v
			#print df[k]
                #	df[k] = df[k].astype(v)
		df["source_file"] = os.path.basename(filename)
    		data_list.append(df)
	
	all_together = pd.concat(data_list)
	all_together.to_csv("./all_together.csv", index=False, quotechar='"', quoting=csv.QUOTE_NONNUMERIC)
