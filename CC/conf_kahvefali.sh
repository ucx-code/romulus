# Defines configuration variables for experiments running in 'kahvefali'
L1A_IP=192.168.66.101
L1B_IP=192.168.66.102
L2_IP=192.168.66.103
L0_IP=192.168.66.3
L1A_DOMNAME=L1A
L1B_DOMNAME=L1B
L0_FLOW=/var/lib/nova/instances/gitvmm/L0/scripts/flow.sh
L0_FLOW_GROUP=/var/lib/nova/instances/gitvmm/L0/scripts/flow_group.sh
VMI_SW_HELPER=/var/lib/nova/instances/gitvmm/L0/utility_apps/vmstate_extractor/sw_vmi_helper

XEN_FOLDER="/root/xen-4.12.3/"
VMI_TARGET_FILE="/root/xen-4.12.3/xen/arch/x86/hvm/vmx/vmx.c"
VMI_TARGET_FOLDER=$(dirname "${VMI_TARGET_FILE}")

MIN_REQ=0
MAX_REQ=0
# Workload configuration

# ====== Single VM Full Load Profile =======
#NCLIENTS=25
#CLIENT_WAITTIME=50 # ms
#END_TIME=200
#INJ_SLEEP_MIN=15000 #ms
#INJ_SLEEP_MAX=40000 # ms

# ====== Single VM Light Load Profile =======
#NCLIENTS=1
#CLIENT_WAITTIME=1000 # ms
#END_TIME=200
#INJ_SLEEP_MIN=15000 #ms
#INJ_SLEEP_MAX=40000 # ms

# ====== Group VM Light Load Profile =======
NCLIENTS=1
CLIENT_WAITTIME=1000 # ms
END_TIME=600
INJ_SLEEP_MIN=200000 #ms
INJ_SLEEP_MAX=210000 # ms

# ====== Single VM Performance (nofi) Light Load Profile =======
#NCLIENTS=1
#CLIENT_WAITTIME=1000 # ms
#END_TIME=175
#INJ_SLEEP_MIN=25000 #ms
#INJ_SLEEP_MAX=25001 # ms

# ===== Single VM Performance with and without nested + Light Load Profile
#NCLIENTS=1
#CLIENT_WAITTIME=1000 # ms
#END_TIME=60
#INJ_SLEEP_MIN=0 #ms
#INJ_SLEEP_MAX=1 # ms

# ===== Single VM Performance with and without nested + Full Load Profile
#NCLIENTS=25
#CLIENT_WAITTIME=50 # ms
#END_TIME=60
#INJ_SLEEP_MIN=0 #ms
#INJ_SLEEP_MAX=1 # ms





