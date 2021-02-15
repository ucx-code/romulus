import pandas as pd
import time
import sys
import pyping
import signal

def signal_handler(sig, frame):
	df.to_csv(out_path)
	sys.exit(0)

if len(sys.argv) != 5:
	print "Unexpected amount of args"
else:
	host = sys.argv[1]
	interval = float(sys.argv[2])
	retry_count = int(sys.argv[3])
	out_path = sys.argv[4]

	signal.signal(signal.SIGINT, signal_handler)
	df = pd.DataFrame()
	timeout_count = 0
	req_id = 0

	while (timeout_count < retry_count):
		resp = pyping.ping(hostname=host, timeout=500, count=1)
		req_ts = time.time()
		if (resp.ret_code == 0):
			timeout_count = 0
		elif (resp.ret_code == 1):
			timeout_count = timeout_count + 1
		else:
			print "Unexpected timeout count", timeout_count

		df = df.append({
				"ts" : req_ts,
				"req_id" : req_id,
				"interval" : interval,
				"retry_count" : retry_count,
				"ret_code" : resp.ret_code,
				"timeout_count" : timeout_count,
				#"hostname" : resp.hostname,
				
			}, ignore_index=True)
		time.sleep(interval)
		req_id = req_id + 1

	df.to_csv(out_path)
	sys.exit(255)
