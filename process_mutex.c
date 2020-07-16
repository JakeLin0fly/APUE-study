/**
* 进程间通过 mutex 同步
**/
#include <unistd.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>

struct memory_mutex_struct {
	int number;
	pthread_mutex_t mutex;
	pthread_mutexattr_t mutex_attr;
};

int main(int argc, char const *argv[])
{
	int fd;
	struct memory_mutex_struct *mm;
	pid_t pid;

	//创建映射
	fd =  open("mt_test", O_CREAT | O_RDWR, 0777);
	ftruncate(fd, sizeof(*mm));
	mm = (struct memory_mutex_struct *)mmap(NULL, sizeof(*mm), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	close(fd);
	unlink("mt_test");
	//可直接创建匿名映射 MAP_ANON(MAP_ANONYMOUS)
	//mm = (struct memory_mutex_struct *)mmap(NULL, sizeof(*mm), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANON, 0);

	memset(mm, 0, sizeof(*mm));

	//初始化mutex属性
	pthread_mutexattr_init(&mm->mutex_attr);
	//设置mutex属性 进程锁：PTHREAD_PROCESS_SHARED
	pthread_mutexattr_setpshared(&mm->mutex_attr, PTHREAD_PROCESS_SHARED);
	//初始化mutex锁
	pthread_mutex_init(&mm->mutex, &mm->mutex_attr);

	pid = fork();
	if(-1 == pid) {
		perror("fork error: ");
		exit(1);
	}
	else if (0 == pid) {	//子进程数数每次 +1
		for(int i = 0; i < 10; ++i){
			//获取锁
			pthread_mutex_lock(&mm->mutex);
			mm->number += 1;
			printf("child process number+1    %d\n", mm->number);
			//释放锁
			pthread_mutex_unlock(&mm->mutex);
			sleep(1);			
		}
	}
	else if (0 < pid) { //子进程数数每次 +2
		for(int i = 0; i < 10; ++i){
			//获取锁
			pthread_mutex_lock(&mm->mutex);
			mm->number += 2;
			printf("-----parent process number+2    %d\n", mm->number);
			//释放锁
			pthread_mutex_unlock(&mm->mutex);
			sleep(1);			
		}
		wait(NULL);	//回收子进程
	}

	pthread_mutexattr_destroy(&mm->mutex_attr);
	pthread_mutex_destroy(&mm->mutex);
	munmap(mm, sizeof(*mm));

	return 0;
}