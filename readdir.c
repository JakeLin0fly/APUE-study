/**
* 递归读取目录信息
**/
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

char filename[512];
//void is_file(const char *name);
void read_dir(const char *dir, void (*func)(const char *)) {
	DIR *dp;
	struct dirent *sdp;
	//打开目录
	dp = opendir(dir);
	if(NULL == dp) {
		perror("opendir error: ");
		return;
	}

	//读目录
	while(NULL != (sdp = readdir(dp))) {
		//对当前目录 上一级目录不进行操作
		if(strcmp(sdp->d_name, ".") == 0 || strcmp(sdp->d_name, "..") == 0) {
			continue;
		}
		//不改变工作目录，字符串拼接路径
		if(strlen(dir)+strlen(sdp->d_name)+2 > sizeof(filename)) {
			fprintf(stderr, "read_dir: name %s/%s is too long\n", dir, sdp->d_name);
		}
		sprintf(filename, "%s/%s", dir, sdp->d_name);
		//递归调用  回调
		func(filename);
	}

	//关闭目录
	closedir(dp);
}

//ls 处理文件、目录
// 文件： 输出信息
// 目录：调用目录遍历
void is_file(const char *name){ 
	struct stat sbuf;
	//先判断文件、目录名是否有效（存在）
	if(-1 == stat(name, &sbuf)) {
		perror("is_file error: ");
		exit(1);
	}
	//判断文件类型
	if(S_ISDIR(sbuf.st_mode)) {	//判断是否为目录
		read_dir(name, is_file);
	}

	//是文件 打印输出文件信息
	printf("%-12ld\t%s\n", sbuf.st_size, name);
}


int main(int argc, char const *argv[])
{
	printf("%s\t\t%s\n", "bytes", "name");
	if(1 == argc)	//没有参数传入，ls目录遍历当前目录
		is_file(".");
	else
		while(--argc > 0)		//一次查询多个
			is_file(*++argv);

	return 0;
}