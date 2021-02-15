PATCH_FOLDER=$1

L1A_XEN_PATH="/root/xen-4.11.1/"
L1A_CODE_PATH="/root/xen-4.11.1/xen/arch/x86/"
L1A_IP="192.168.66.101"

for patch_file in  $PATCH_FOLDER/*.patch
do
	patch_file_name=`basename $patch_file`
	patch_name="${patch_file_name%.*}"
	echo "Doing $patch_name"

	cp $patch_file /tmp/$patch_file_name
	bash postprocess_patch.sh /tmp/$patch_file_name
	scp /tmp/$patch_file_name  root@$L1A_IP:${L1A_CODE_PATH}/${patch_file_name}
	ssh root@$L1A_IP "cd ${L1A_CODE_PATH}; patch --dry-run msr.c $patch_file_name; " 
	#ssh root@$L1A_IP "cd ${L1A_CODE_PATH}; patch  msr.c $patch_file_name; " 
	if [ $? -eq 0 ]
	then
		ssh root@$L1A_IP "cd ${L1A_XEN_PATH}; make -j4 dist-xen; make -j4 install-xen";
		echo $?
	#else
		# Faile patch
	fi
	ssh root@$L1A_IP "rm ${L1A_CODE_PATH}/${patch_file_name}"

done
