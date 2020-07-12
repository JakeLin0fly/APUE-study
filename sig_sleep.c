/**
**自定义实现sleep(存在时序竞态问题)
**/
#include <unistd.h>
#include <stdio.h>
#include <signal.h>

/* 信号处理函数原型，无返回值，只有一个参数，表示信号编号 */
void sig_alrm(int signo) {
	/**
	 * 用来站位，可以不做任何事，但是函数得存在
	**/
	printf("sig_alrm do something...\n");
}

unsigned int sig_sleep(unsigned int sec){
	struct sigaction new_act, old_act;
	unsigned int unsleep_time;	//保存未休眠的时间

	new_act.sa_handler = sig_alrm;	//注册信号处理函数sig_alrm
	sigemptyset(&new_act.sa_mask);	//清空屏蔽位
	new_act.sa_flags = 0;
	//设置信号处理动作、保存原信号处理方式
	sigaction(SIGALRM, &new_act, &old_act);

	//设置定时器
	alarm(sec);
/**
 **  alarm开始计时，此时进程失去CPU 
 **  定时器时间到了，发出SIGALRM信号(此时仍还未获得CPU)
 **  当获得CPU时，首先执行信号处理
 **  再返回程序执行 pause()
 **  程序僵死
 **/
	//进程阻塞，收到一个信号后，pause返回-1，解除阻塞
	pause();

	unsleep_time = alarm(0);	//获取定时器剩余时间

	sigaction(SIGALRM, &old_act, NULL);	//恢复默认信号处理动作

	return unsleep_time;
}

int main(int argc, char const *argv[])
{
	while(true){
		sig_sleep(5);
		printf("sleep 5 secsnds\n");
	}
	return 0;
}