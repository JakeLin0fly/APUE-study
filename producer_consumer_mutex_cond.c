/**
****************** 生产者-消费者问题******************
*****************************************************
* 使用互斥锁和条件变量实现
* 生产者n个，消费者m个，缓冲区大小N
* 临界区设置互斥锁 mutex
* 条件变量not_full、not_empty标识缓存区状态
**/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>

#define MAX_BUF_NUM	10
#define MAX_THREAD_NUM	100
#define THREAD_RUN_TIME	100

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;		//临界区 互斥锁
pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;	//条件变量---未满(生产者可生产)
pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;	//条件变量---不空(消费者可消费)
int buf[MAX_BUF_NUM];	//临界区资源
int items = 0;	//资源数量(已使用缓冲区数)
int spaces = MAX_BUF_NUM;	//空闲缓冲区数
int in = 0;		//最后一个资源
int out = 0;	//第一个资源

bool thread_flag;	//线程运行状态标记

/** 生产者 **/
void *produce(void *arg) {
	int product;
	int pthread_no = *(int *)arg;
	//srand((unsigned)time(NULL));
	while(thread_flag) {
		// 先获取互斥锁
		pthread_mutex_lock(&mutex);
		//判断临界区是否有空闲区，以供资源存放
		while(0 == spaces) {
			//临界区没有空闲区，生产者wait空闲缓冲区，阻塞时释放已掌握的互斥锁
			//解除阻塞时重新申请获取互斥锁
			pthread_cond_wait(&not_full, &mutex);
		}
		// 生产资源 应先获取空闲块 再进行生产
		product = rand() % 1000;
		//存放资源到临界区
		buf[in] = product;
		in = (in + 1) % MAX_BUF_NUM;
		items++;
		spaces--;
		pthread_cond_signal(&not_empty);	//唤醒至少一个阻塞在条件变量not_empty上的消费者
		//pthread_cond_broadcast(&not_empty);	//唤醒全部阻塞在条件变量not_empty上的消费者

		//释放互斥锁
		//pthread_mutex_unlock(&mutex);
		//生成完成
		printf("producer No.%d thread id = %ld, product = %d\n", pthread_no, pthread_self(), product);
		pthread_mutex_unlock(&mutex);	//确保按顺序显示
		sleep(1);
	}
	pthread_exit(NULL);
}

/** 消费者 **/
void *consume(void *arg) {
	int product;
	int pthread_no = *(int *)arg;
	while(thread_flag) {
		//先获取锁
		pthread_mutex_lock(&mutex);
		//判断临界区是否有资源
		while(0 == items) {
			//临界区没有现存资源，消费者阻塞等待，并释放已掌握的互斥锁
			//解除阻塞时重新申请获取互斥锁
			pthread_cond_wait(&not_empty, &mutex);
		}
		//获取资源
		product = buf[out];
		out = (out + 1) % MAX_BUF_NUM;
		items--;
		spaces++;
		pthread_cond_signal(&not_full);	//唤醒至少一个阻塞在条件变量not_full上的生产者
		//pthread_cond_broadcast(&not_full);	//唤醒所有阻塞在条件变量not_full上的生产者

		//释放锁资源
		//pthread_mutex_unlock(&mutex);	
		//消费完成
		printf("-----consumer No.%d thread id = %ld, product = %d\n", pthread_no, pthread_self(), product);
		pthread_mutex_unlock(&mutex); //确保按顺序显示
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


	pthread_exit(NULL);
}