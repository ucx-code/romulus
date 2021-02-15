# Script that receives raw output from other bash scripts, extracts the times and outputs a CSV-like row
import sys

def extract(s, ss):
	s = s.replace("\n", " ")
        ps =  s.find(ss)
	try:
	        return int(s[ps + len(ss):].split(" ")[0])
	except:
		return ""

msg = sys.argv[1]
l1_launch = extract(msg, "Launching L1s took ")
get_offsets = extract(msg, "Getting offsets from savefile Took ")
create_base =  extract(msg, "Creating base save took ")
save_nvmcs = extract(msg, "Save nVMCS took ")
inspect_l2 = extract(msg, "Inspecting L2 state took ")
recalc_offsets = extract(msg, "Recalculating the offsets took ")
replace_cpu = extract(msg, "Replacing CPU and nVMCS took ")
hypercall = extract(msg, "Performing hypercall took ")
update_eptp = extract(msg, "Updating EPTP in save took ")
flow_migration = extract(msg, "Flow.sh migration took ")
restore = extract(msg, "Restore took ")
wl_tstamp = extract(msg, "Start workload @ ")
fi_tstamp = extract(msg, "Injecting a fault @ ")
l0_rdtsc = extract(msg, "Got RDTSC @ ")
dolma_tsc = extract(msg, "In dolma @ ")
crash_tstamp = extract(msg, "Crash detected @ ")
base_save_start = extract(msg, "Creating base save file started @ ")
base_save_end = extract(msg, "Creating base save file ended @ ")

#print "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d" % (l1_launch, get_offsets, create_base, save_nvmcs, inspect_l2, recalc_offsets, replace_cpu, hypercall, update_eptp, flow_migration, restore, wl_tstamp, fi_tstamp, l0_rdtsc, dolma_tsc, crash_tstamp,base_save_start,base_save_end)
print "%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s" % (l1_launch, get_offsets, create_base, save_nvmcs, inspect_l2, recalc_offsets, replace_cpu, hypercall, update_eptp, flow_migration, restore, wl_tstamp, fi_tstamp, l0_rdtsc, dolma_tsc, crash_tstamp,base_save_start,base_save_end)



