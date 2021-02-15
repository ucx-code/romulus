# Extracts information regarding FI from the output of 'xl dmesg' into csv format
import sys
import pandas as pd

cols = ["fi_tsc", "fi_rip", "fi_rbp", "fi_rsp", "fi_rax", "fi_rbx", "fi_rcx", "fi_rdx", "fi_r8", "fi_r9", "fi_r10", "fi_r11", "fi_r12", "fi_r13", "fi_r14", "fi_r15"]
if len(sys.argv) != 2:
	print "Unexpected amount of parameters has been passed to extract_fi_log"
else:
	rdtsc = ""
	rip = rbp = rsp = rax = rbx = rcx = rdx = r8 = r9 = r10 = r11 = r12 = r13 = r14 = r15 = ""
	txt = sys.argv[1]

	try:
		for l in txt.split("\n"):
			if ("injection" in l):
				if ("TDSC" in l):
					rdtsc = int(l.split(" ")[-1].rstrip(), 16) # Convert frm hex to int
				elif ("regs" in l):
					sp = l.rstrip().split(" ")
					rip = str(sp[4])
					rbp = str(sp[6])
					rsp = str(sp[8])
					rax = str(sp[10])
					rbx = str(sp[12])
					rcx = str(sp[14])
					rdx = str(sp[16])
					r8 = str(sp[18])
					r9 = str(sp[20])
					r10 = str(sp[22])
					r11 = str(sp[24])
					r12 = str(sp[26])
					r13 = str(sp[28])
					r14 = str(sp[30])
					r15 = str(sp[32])
				else:
					print "There was a problem in this line", l
	except:
		pass

	df = pd.DataFrame({
			"fi_tsc": [rdtsc,],
			"fi_rip": [rip,],
			"fi_rbp": [rbp,],
			"fi_rsp": [rsp,],
			"fi_rax": [rax,],
			"fi_rbx": [rbx,],
			"fi_rcx": [rcx,],
			"fi_rdx": [rdx,],
			"fi_r8":  [r8,],
			"fi_r9":  [r9,],
			"fi_r10": [r10,],
			"fi_r11": [r11,],
			"fi_r12": [r12,],
			"fi_r13": [r13,],
			"fi_r14": [r14,],
			"fi_r15": [r15,],
		})
	print df.to_csv(index=False, header=False, columns=cols)
