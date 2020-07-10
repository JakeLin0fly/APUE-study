/**
* ########## mmap多进程拷贝 ###########
*********************************************
* 通过存储映射I/O，mmap方法，实现文件的存储映射
* 多个子进程，对映射到内存的文件进行拷贝
*********************************************
**/
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/mman.h>

int main(int argc, char const *argv[])
{
	int fd_src, fd_dst;					//原文件fd、目标文件fd
	int ret, file_len, i_process, n_process;	//操作返回值、文件长度、第i个进程、进程总数
	char *mp_src, *mp_dst;				//映射地址
	char *tmp_srcp, *tmp_dstp;
	pid_t pid;
	struct  stat sbuf;
	long byte_size, mod_size;


	//输入命令参数检测
	if (argc < 2 || argc > 4) {
		printf("Enter like this please: ./a.out file_src file_dst fork_num[auto 5] \n");
		exit(1);
	}
	else if (3 == argc) {
		n_process = 5;		//默认进程数 5
	}
	else if (4 == argc) {
		n_process = atoi(argv[4]);	//用户指定进程数
	}

	//打开原文件
	fd_src = open(argv[1], O_RDONLY);
	if (-1 == fd_src) {
		perror("open src file error: ");
		exit(1);
	}

	//创建(打开)目标文件		截断
	fd_dst = open(argv[2], O_RDWR|O_CREAT|O_TRUNC, 0664);
	if (-1 == fd_dst) {
		perror("open dest file error: ");
	}

	//获取文件大小
	ret = fstat(fd_src, &sbuf);
	if (-1 == ret) {
		perror("fstat error: ");
		exit(1);
	}
	file_len = sbuf.st_size;
	if (file_len < n_process) {	
		n_process = file_len;	//文件长度小于进程数，重新设置进程数
	}

	//目标文件:文件扩展
	ret = ftruncate(fd_dst, file_len);
	if (-1 == ret) {
		perror("ftruncate fd_dst error: ");
		exit(1);
	}

	//原文件存储映射
	mp_src = (char *)mmap(NULL, file_len, PROT_READ, MAP_SHARED, fd_src, 0);
	if (MAP_FAILED == mp_src) {
		perror("mmap src error: ");
		exit(1);
	}
	//目标文件存储映射 MAP_SHARED才会反映到磁盘上
	mp_dst = (char *)mmap(NULL, file_len, PROT_READ|PROT_WRITE, MAP_SHARED, fd_dst, 0);
	if (MAP_FAILED == mp_dst) {
		perror("mmap dest error: ");
		exit(1);
	}

	tmp_srcp = mp_src;
	tmp_dstp = mp_dst;

	//计算各进程拷贝字节数
	byte_size = file_len / n_process;	//每个子进程平均拷贝字节数
	mod_size = file_len % byte_size;	//不能整除的余下字节数，有最后一个进程负责

	//创建子进程
	for (i_process = 0; i_process < n_process; ++i_process) {
		if(0 == (pid = fork()))
			break;	//子进程执行 跳出
	}

	if(i_process == n_process) {
		//父进程 等待所有子进程
		for (int i = 0; i < n_process; ++i)
			wait(NULL);	
	}
	else if (i_process == (n_process-1)) {
		//最后一个子进程 处理字节数可能不一样
		memcpy(tmp_dstp+byte_size*i_process, tmp_srcp+byte_size*i_process, byte_size+mod_size);
	}
	else {
		memcpy(tmp_dstp+byte_size*i_process, tmp_srcp+byte_size*i_process, byte_size);
	}

	munmap(mp_src, file_len);
	munmap(mp_dst, file_len);

	return 0;
}