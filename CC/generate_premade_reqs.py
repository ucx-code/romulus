# Receiving a list of words that can be queried, generates a list of possible queries along with expected output
import pandas as pd
import sys
import json
from geventhttpclient import HTTPClient
from geventhttpclient.url import URL
import hashlib
import base64

if len(sys.argv) == 1:
	dict_file = "words_alpha.txt"
else:
	dict_file = sys.argv[1]
base_url = "http://192.168.66.108:8983/solr/wikipediaCollection/query"
print "Opening", dict_file


with open(dict_file) as od:
	words = [w.strip() for w in od.readlines()]

dataset = []
p = [ (words[0], 'a'), (words[1], 'b') ]
for w in words:
	query = "?q=TITLE:%s" % w
	url = URL('%s%s' % (base_url, query))
	http = HTTPClient.from_url(url, concurrency=1)
	response = http.get(url.request_uri)
	body = response.read()
	status = response.status_code
	explen = response.content_length
	print body
	jsonified = json.loads(body)
	del jsonified["responseHeader"]["QTime"]
	body = json.dumps(jsonified)
	encodedBody = base64.b64encode(body)
	hash = hashlib.md5(encodedBody).hexdigest()
	dataset.append([w, query, status, explen, encodedBody, hash])
	http.close()

df = pd.DataFrame(dataset)
df.to_csv("baseline_answers.csv", header=["word", "query", "status_code", "length", "bodyb64", "md5hexhash"])
