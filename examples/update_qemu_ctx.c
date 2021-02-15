#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <inttypes.h>

#include "struts.h"


int main (int argc, char ** argv)
{
 	char base_file_path[512], target_file_path[512];
    unsigned int offset_base, offset_target;
    unsigned int length;
    libxl__sr_rec_hdr libxl_rec;
    
 	int base_fp, target_fp;
 	if (argc < 6)
 	{
 		printf("Usage: <base save file> <target save file> <base offset> <target offset> <length>\n");
 		return -1;
 	}
 	
 	memcpy(base_file_path, argv[1], 512);
 	memcpy(target_file_path, argv[2], 512);
 	sscanf(argv[3], "%d", &offset_base);
 	sscanf(argv[4], "%d", &offset_target);
 	sscanf(argv[5], "%d", &length);

 	base_fp = open(base_file_path, O_RDONLY);
 	target_fp = open(target_file_path, O_RDWR);
 	
 	assert(base_fp != -1);
 	assert(target_fp != -1);
	
	
	assert(lseek(target_fp, offset_target, SEEK_SET) != -1);
	assert(lseek(base_fp, offset_base, SEEK_SET) != -1);
	
	unsigned char * p = malloc(length);
	
	assert(read(base_fp, p, length) == length);
	assert(write(target_fp, p, length) == length);
	
	// Finish save file
	libxl_rec.type = REC_TYPE_END;
	libxl_rec.length = 0;
	assert(write(target_fp, &libxl_rec, sizeof(libxl_rec)) == sizeof(libxl_rec));
	
    // Update length @ target
    assert(lseek(target_fp, offset_target - sizeof(libxl_rec), SEEK_SET) != -1);	
	assert(read(target_fp, &libxl_rec, sizeof(libxl_rec)) == sizeof(libxl_rec));
	assert(libxl_rec.type == REC_TYPE_EMULATOR_CONTEXT);
	libxl_rec.length = length;
    assert(lseek(target_fp, offset_target - sizeof(libxl_rec), SEEK_SET) != -1);	
    assert(write(target_fp, &libxl_rec, sizeof(libxl_rec)) == sizeof(libxl_rec));
    
    fdatasync(target_fp);
	free(p);
 
	close(base_fp);
	close(target_fp);
	return 0;
}
