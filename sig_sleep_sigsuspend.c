/**
*************** 自定义实现 sleep **************************
** ############ sigsuspend 解决时序竞态问题 ###############
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
	sigset_t new_mask, old_mask, suspend_mask;
	unsigned int unsleep_time;	//保存未休眠的时间

	new_act.sa_handler = sig_alrm;	//注册信号处理函数sig_alrm
	sigemptyset(&new_act.sa_mask);	//清空屏蔽位
	new_act.sa_flags = 0;
	//设置信号处理动作、保存原信号处理方式
	sigaction(SIGALRM, &new_act, &old_act);

	//屏蔽(阻塞) SIGALRM
	sigemptyset(&new_mask);
	sigaddset(&new_mask, SIGALRM);
	sigprocmask(SIG_BLOCK, &new_mask, &old_mask);

	//设置定时器
	alarm(sec);
/*	
	//时序竞态
 *1*  alarm开始计时，此时进程失去CPU 
 *2*  定时器时间到了，发出SIGALRM信号(此时仍还未获得CPU)
 *3*  当获得CPU时，首先执行信号处理
 *4*  再返回程序执行 pause()
 *5*  程序僵死
	//进程阻塞，收到一个信号后，pause返回-1，解除阻塞
	pause();
*/
	//解除屏蔽(阻塞) SIGALRM  挂起等待
	suspend_mask = old_mask;
	sigdelset(&suspend_mask, SIGALRM);
	sigsuspend(&suspend_mask); //一个原子操作:先恢复信号屏蔽字，然后使进程休眠

	unsleep_time = alarm(0);	//获取定时器剩余时间
	sigaction(SIGALRM, &old_act, NULL);	//恢复默认信号处理动作
	sigprocmask(SIG_SETMASK, &old_mask, NULL);	//恢复屏蔽字

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