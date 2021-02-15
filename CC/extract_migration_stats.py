# Extracts information regarding the migration from the output of 'xl dmesg' into csv format
import sys
import pandas as pd

cols = ["migr_totalPages", "migr_super1Gb", "migr_super2Mb", "migr_p2m", "migr_l0", "migr_l1", "migr_l2", "migr_l3"]
if len(sys.argv) != 2:
	print "Unexpected amount of parameters has been passed to extract_fi_log"
else:
	txt = sys.argv[1]
	total_pages = super1Gb = super2Mb = p2m_pages = pages_l0 = pages_l1 = pages_l2 = pages_l3 = ""
	try:
		for l in txt.split("\n"):
			if ("[stats]" in l):
				if ("Total Pages" in l):
					total_pages = int(l.split(":")[-1].strip())
				elif ("Superpages (1 Gb):" in l):
					super1Gb = int(l.split(":")[-1].strip())
				elif ("Superpages (2 Mb):" in l):
					super2Mb = int(l.split(":")[-1].strip())
				elif ("P2M pages" in l):
					p2m_pages = int(l.split(":")[-1].strip())
				elif ("Pages @ L0" in l):
					sp = l.rstrip().split(" ")
					pages_l0 = int(sp[5])
					pages_l1 = int(sp[7])
					pages_l2 = int(sp[9])
					pages_l3 = int(sp[11])
				else:
					print "Unexpected line", l
	except:
		pass
	df = pd.DataFrame({
                         	"migr_totalPages": [total_pages,],
        	                "migr_super1Gb": [super1Gb,],
                                "migr_super2Mb": [super2Mb,],
                                "migr_p2m": [p2m_pages,],
                                "migr_l0": [pages_l0,],
                                "migr_l1": [pages_l1,],
                                "migr_l2": [pages_l2,],
                                "migr_l3": [pages_l3,],
                        })
        print df.to_csv(index=False, header=False, columns=cols)

