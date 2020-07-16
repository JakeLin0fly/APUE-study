/**
* 进程同步 文件锁
* 借助fcntl 函数来实现锁机制

           struct flock {
               ...
               short l_type;    // Type of lock: F_RDLCK,
                                //   F_WRLCK, F_UNLCK 
               short l_whence;  // How to interpret l_start:
                                //   SEEK_SET, SEEK_CUR, SEEK_END 
               off_t l_start;   // Starting offset for lock 
               off_t l_len;     // Number of bytes to lock 
               pid_t l_pid;     // PID of process blocking our lock
                                //   (F_GETLK only) 
               ...
           };
**/

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#define HOLD_LOCK_TIME 10

int main(int argc, char const *argv[])
{
	int fd;
	int mode;
	struct flock f_lock;
	if(argc != 3){
		printf("Enter like this: ./a.out file_path mode(0 F_RDLCK; 1 F_WRLCK)\n");
		exit(1);
	}

	//打开文件
	if(-1 == (fd = open(argv[1], O_RDWR))) {
		perror("open error: ");
		exit(1);
	}

	mode = atoi(argv[2]);
	if(0 == mode)
		f_lock.l_type = F_RDLCK;	//读锁
	else if(1 == mode)
		f_lock.l_type = F_WRLCK;	//写锁
	else{
		printf("Enter like this: ./a.out file_path mode(0 F_RDLCK; 1 F_WRLCK)\n");
		exit(1);
	}

	f_lock.l_whence = SEEK_SET;	//从起始位置开始
	f_lock.l_start = 0;			//偏移 0
	f_lock.l_len = 0;			//0 表示对整个文件加锁

	//加锁 阻塞方式
	fcntl(fd, F_SETLKW, &f_lock);
	printf("fcntl locked \n");

	sleep(HOLD_LOCK_TIME);

	f_lock.l_type = F_UNLCK;
	printf("fcntl unlock ...\n");
	//解锁
	fcntl(fd, F_SETLKW, &f_lock);

	close(fd);

	return 0;
}