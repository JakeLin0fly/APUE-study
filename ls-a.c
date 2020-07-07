#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <dirent.h>

int main(int argc, char const *argv[])
{
	DIR *dp;
	struct dirent *sdp;

	dp = open(argv[1]);
	if(NULL == dp){
		perror("opendir error: ");
		exit(1);
	}

	while(NULL != (sdp = readdir(dp))){
		printf("%s\t", sdp->d_name);
	}
	printf("\n");
	return 0;
}