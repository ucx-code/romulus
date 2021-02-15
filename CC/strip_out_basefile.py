# The basefile is very big (11G) and cant find in memory. It has many fields that are not needed by the client.
# This script loads the big file, strips not required columns and creates a smaller file to be used on the client.

import pandas as pd
df = pd.read_csv("baseline_answers.csv.backup", index_col=0)
#,word,query,status_code,length,bodyb64,md5hexhash
df = df.drop(columns = ['status_code', 'length', 'bodyb64'])
df.to_csv("baseline_small.csv")
