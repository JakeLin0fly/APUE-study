/**
*************************** 生产者-消费者问题**************************
**********************************************************************
* 使用 信号量 实现
* 生产者n个，消费者m个，缓冲区大小N
* items_num 临界区资源池 资源数信号量
* spaces_num 临界区资源池 空闲区块数信号量
**********************************************************************
*** 注：因CPU轮询 打印的信息可能不按顺序输出,故调整sprintf位置,并加大锁粒度
#### 实际编码应如下：		
		互斥信号量mutex应该仅锁资源位置
		即mutex_in锁第一个空闲区块位置 in
		mutex_out锁第一个资源位置 out
####
**/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <semaphore.h>

#define MAX_BUF_NUM	10
#define MAX_THREAD_NUM	100
#define THREAD_RUN_TIME	100


int buf[MAX_BUF_NUM];	//临界区资源池
struct {
	sem_t items_num;	//产品信号量		初值：0
	sem_t spaces_num;	//空闲区块信号量 初值：MAX_BUF_NUM
	sem_t mutex;		//临界区锁信号 	初值：1
} semaphores;

int in = 0;		//第一个空闲区块位置
int out = 0;	//第一个资源位置

bool thread_flag;	//线程运行状态标记

/** 生产者 **/
void *produce(void *arg) {
	int product;
	int pthread_no = *(int *)arg;
	//srand((unsigned)time(NULL));
	while(thread_flag) {
		// 先获取空闲块
		sem_wait(&semaphores.spaces_num);
		//获取锁信号
		sem_wait(&semaphores.mutex);

		// 生产资源
		product = rand() % 1000;
		//存放资源到临界区
		buf[in] = product;
		in = (in + 1) % MAX_BUF_NUM;
		
		//因CPU轮询 在此处打印输出，不会出现顺序错乱
		printf("producer No.%d thread id = %ld, product = %d\n", pthread_no, pthread_self(), product);
		
		//释放锁
		sem_post(&semaphores.mutex);
		//临界区资源数 +1
		sem_post(&semaphores.items_num);	//唤醒阻塞在items_num信号上的消费者
		//生成完成
		//printf("producer No.%d thread id = %ld, product = %d\n", pthread_no, pthread_self(), product);
		
		sleep(1);
	}
	pthread_exit(NULL);
}

/** 消费者 **/
void *consume(void *arg) {
	int product;
	int pthread_no = *(int *)arg;
	while(thread_flag) {
		//先获取资源信号
		sem_wait(&semaphores.items_num);
		//获取临界区锁
		sem_wait(&semaphores.mutex);
		
		//获取资源
		product = buf[out];

		out = (out + 1) % MAX_BUF_NUM;
		//因CPU轮询 在此处打印输出，不会出现顺序错乱
		printf("-----consumer No.%d thread id = %ld, product = %d\n", pthread_no, pthread_self(), product);
		
		//释放锁资源
		sem_post(&semaphores.mutex);
		//临界区资源数 +1
		sem_post(&semaphores.spaces_num);	//唤醒阻塞在spaces_num信号上的生产者
		//消费
		//printf("-----consumer No.%d thread id = %ld, product = %d\n", pthread_no, pthread_self(), product);
		
		sleep(1);
	}
	pthread_exit(NULL);
}

int main(int argc, char const *argv[])
{
	int producer_num, consumer_num;
	pthread_t producer_tid[MAX_THREAD_NUM];
	pthread_t consumer_tid[MAX_THREAD_NUM];

	if (3 != argc) {
		printf("Enter like this: ./out <producer_num> <consumer_num>\n");
		exit(1);
	}
	producer_num = atoi(argv[1]);
	consumer_num = atoi(argv[2]);
	if(producer_num > MAX_THREAD_NUM || consumer_num > MAX_THREAD_NUM) {
		printf("max thread number is %d!\n", MAX_THREAD_NUM);
		exit(1);
	}

	//初始化
	sem_init(&semaphores.mutex, 0, 1);
	sem_init(&semaphores.items_num, 0, 0);
	sem_init(&semaphores.spaces_num, 0, MAX_BUF_NUM);
	thread_flag = true;	//线程运行状态
	srand((unsigned)time(NULL));
	//创建生产者线程
	for(int i = 0; i < producer_num; ++i) {
		pthread_create(&producer_tid[i], NULL, produce, (void *)&i);
	}

	//创建消费者线程
	for (int i = 0; i < consumer_num; ++i) {
		pthread_create(&consumer_tid[i], NULL, consume, (void *)&i);
	}

	//定时终止所有线程 并回收
	sleep(THREAD_RUN_TIME);
	thread_flag = false;
	for (int i = 0; i < producer_num; ++i) {
		pthread_join(producer_tid[i], NULL);
	}
	for (int i = 0; i < consumer_num; ++i) {
		pthread_join(consumer_tid[i], NULL);
	}

	//destroy sem_t
	sem_destroy(&semaphores.mutex);
	sem_destroy(&semaphores.items_num);
	sem_destroy(&semaphores.spaces_num);

	pthread_exit(NULL);
}