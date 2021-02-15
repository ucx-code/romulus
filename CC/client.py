#!/usr/bin/env python
import sys
import numpy as np
import gevent.pool
import json
import pandas as pd
from threading import Lock
import time
from geventhttpclient import HTTPClient
from geventhttpclient.url import URL
import hashlib
import base64
import random
from httplib import HTTPException

global_seed = 123 # Fixed seed for RNG

# ,word,query,status_code,length,bodyb64,md5hexhash

query_dict = pd.read_csv("baseline_small.csv", index_col=0)
#iloc [line, column]
#print query_dict[query_dict.iloc[:, 0]]["query"]

#url = URL('https://www.google.com/?q=test')

# Global lock to serialize writes to the dataframe
m = Lock()

#response = http.get(url.request_uri)
#print response.status_code

df = pd.DataFrame()

# Removes the QTime field from the response message, to allow hash checking
def remove_time_from_response(r):
	try:
		jsonified = json.loads(r)
	        del jsonified["responseHeader"]["QTime"]
		return json.dumps(jsonified)
	except:
		print r
		return r

def do_request(ip, client_id, client_seed, req_id):
	global df
	response = body = status_code = content_length = ex_msg = hash = matches =  ""
	req_query = query_dict.iloc[req_id, :]["query"]
	expected_hash = query_dict.iloc[req_id, :]["md5hexhash"]
	url = URL('http://' + ip + ':8983/solr/wikipediaCollection/query%s' % (req_query))

	http = HTTPClient.from_url(url, concurrency=2, connection_timeout=30, network_timeout=30)

	start = time.time()
	try:
		response = http.get(url.request_uri)
		#print response.read()
		p = remove_time_from_response(response.read())
		body = base64.b64encode(p)
		hash = hashlib.md5(body).hexdigest()
		status_code = response.status_code
		content_length = response.content_length
		matches = bool(hash == expected_hash)
		#print hash, expected_hash
	except gevent.socket.timeout as e:
		print "timeout"
		ex_msg = str(e)
	except HTTPException as e:
		print "http exception"
		ex_msg = str(e)
	except gevent.socket.error as e:
		print "socket error"
		ex_msg = str(e)
	except Exception as e:
		print type(e)
		ex_msg = str(e)
	finally:
		end = time.time()
		# TODO move this to the end of the client, and avoid locking
		m.acquire()
		try:
			data = {
				'client_id': client_id,
				'client_seed': client_seed,
				'req_id' : req_id,
				'req_query' : req_query,
				'sent_ts' : start,
				'received_ts' : end,
				'dur': end-start,
				'code' : status_code,
				'length': content_length,
				'body' : body,
				'hash' : hash,
				'content_matches' : matches,
				'exception': ex_msg
				}
			df= df.append(data, ignore_index=True)
		finally:
			m.release()
			http.close()

def spawn_client(props):
	(ip, client_id, client_seed, total_req, max_wait) = props
	client_rng = np.random.RandomState(client_seed)
	
	for i in xrange(total_req):
		req_id = client_rng.randint(0, len(query_dict.index))
		do_request(ip, client_id, client_seed, req_id)
		time.sleep(client_rng.randint(0, max_wait) / 1000.0)

ip = str(sys.argv[1])
n_clients = int(sys.argv[2])
max_wait = int(sys.argv[3]) # ms
min_req = int(sys.argv[4])
max_req = int(sys.argv[5])
pool = gevent.pool.Pool(n_clients * 2)
global_rng = np.random.RandomState(global_seed)
for i in xrange(n_clients):
	pool.spawn(spawn_client, [ip, i, global_rng.randint(1, 5000), global_rng.randint(min_req, max_req), max_wait])

pool.join()

#print df
df.to_csv('data.csv')
