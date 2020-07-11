/**
***********************实现shell命令解析执行********************
**/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAX_CMD_LEN 	4096	//最大输入命令字节数
#define MAX_PIPE_NUM 	16	//最大管道数
#define MAX_ARGC_NUM 	10	//单一命令，最大参数数量

typedef struct CMD_STRUCT
{
	char *argv[MAX_ARGC_NUM];	//命令及参数数组
	char *in;	//输入文件
	char *out;	//输出文件
}CMD_STRUCT;
CMD_STRUCT cmd[MAX_PIPE_NUM+1];	//shell命令结构体数组
int pfd[MAX_PIPE_NUM][2];	//管道数组

int parse(char *buf, int cmdNum) {
	char *p = buf;
	int index = 0;
	cmd[cmdNum].in = cmd[cmdNum].out = NULL;

	while('\0' != *p) {
		//空格全部替换为字符串结束标志 '\0'
		if(*p == ' ') {
			*p++ = '\0';
			continue;
		}
		//用户输入重定向
		if('<' == *p) {
			*p = '\0';
			while(' ' == *(++p))
				*p = '\0';	//处理多个空格分隔情况
			if('\0' == *p++) //有输入重定向符(<)但没有输入文件
				return -1;	
			cmd[cmdNum].in = p;
			continue;
		}
		//保存命令 eg:ls -l -R 分别保存为'ls'、'-l'、'-R'
		if(p == buf || *(p-1) == '\0') {
			if(index < MAX_ARGC_NUM-1){
				cmd[cmdNum].argv[index++] = p++;
				continue;
			}
			else {	//单一命令过长
				fprintf(stderr, "this cmd(index is %d) is too long\n", cmdNum);
				return -1;	
			}
		}
		++p;	//!!!
	}

	if(index < MAX_ARGC_NUM)
		cmd[cmdNum].argv[index] = NULL;	//截断：确保不因短命令覆盖长命令出错

	return 0;
}

int main(int argc, char const *argv[])
{
	char buf[MAX_CMD_LEN];
	pid_t pid;
	int cmd_num, pipe_num;
	char *cur_cmd, *next_cmd;
	int i_fork;
	int fd;	//用户输入重定向 eg: file.txt

	while(true) {	//一直循环等待接收命令
		printf("Enter shell cmd: ");
		if(NULL == fgets(buf, MAX_CMD_LEN, stdin)) {
			perror("fgets error: ");
			exit(1);
		}
		
		//!!!fgets获取的字符串末尾存在换行符
		if('\n' == buf[strlen(buf)-1]) {
			buf[strlen(buf)-1] = '\0';
		}

		cmd_num = 0;	//记录单一命令条数
		next_cmd = buf;
		//拆分由管道(|)连接的命令字符串
		cur_cmd = strtok(next_cmd, "|");
		while(NULL != cur_cmd) {
			if(parse(cur_cmd, cmd_num) < 0) //解析命令字符串
				break;
			++cmd_num;
			if(MAX_PIPE_NUM+1 == cmd_num) {
				printf("shell cmd is too long!%d\n",cmd_num);
				break;
			}
			cur_cmd = strtok(NULL, "|");
		}
		if(0 == cmd_num)	//仅回车换行 不进行解释执行
			continue;

		pipe_num = cmd_num - 1;	//根据命令数确定管道数目
		//创建管道
		for (int i = 0; i < pipe_num; ++i) {
			if(pipe(pfd[i])) {
				perror("pipe error: ");
				exit(1);
			}
		}

		//创建子进程，一个子进程负责一条命令执行，通过管道进行通信
		for (i_fork = 0; i_fork < cmd_num; ++i_fork) {
			//父进程创建子进程
			if(0 == (pid = fork())) 
				break;	//子进程跳出
		}

		if(0 == pid) {	//子进程执行单一命令
			if(0 != pipe_num) {	//用户输入命令存在管道
				//第一个子进程
				if(0 == i_fork) {	
					//仅 输出重定向
					dup2(pfd[0][1], STDOUT_FILENO);
					close(pfd[0][0]);	//关闭管道读端
					//关闭当前进程中其余管道
					for(int i = 1; i < pipe_num; ++i) {
						close(pfd[i][0]);
						close(pfd[i][1]);
					}	
				}
				//最后一个子进程
				else if(pipe_num == i_fork) {	
					//仅 输入重定向
					dup2(pfd[i_fork-1][0], STDIN_FILENO);
					close(pfd[i_fork-1][1]);	//关闭管道写端
					//关闭当前进程中其余管道
					for(int i = 0; i < pipe_num-1; ++i) {
						close(pfd[i][0]);
						close(pfd[i][1]);
					}
				}
				//其余子进程 输入输出均通过管道
				else {
					//输入重定向
					dup2(pfd[i_fork-1][0], STDIN_FILENO);
					close(pfd[i_fork-1][1]);
					//输出重定向
					dup2(pfd[i_fork][1], STDOUT_FILENO);
					close(pfd[i_fork][0]);
					//关闭当前进程中其余管道
					for(int i = 0; i < pipe_num; ++i) {
						if(i != i_fork || i != i_fork-1) {
							close(pfd[i][0]);
							close(pfd[i][1]);
						}
					}
				}
			} //end if(0 != pipe_num)

			//用户输入重定向
			if(cmd[i_fork].in) {
				fd = open(cmd[i_fork].in, O_RDONLY);
				if(-1 != fd)
					dup2(fd, STDIN_FILENO);
			}
			//用户输出重定向
			if(cmd[i_fork].out) {
				fd = open(cmd[i_fork].out, O_WRONLY|O_CREAT|O_TRUNC, 0644);
				if(-1 != fd) 
					dup2(fd, STDOUT_FILENO);
			}

			//执行用户输入命令
			execvp(cmd[i_fork].argv[0], cmd[i_fork].argv);
			fprintf(stderr, "executing %s error: ", cmd[i_fork].argv[0]);
			perror("");
			exit(127);
		}//end 子进程
		else {
			/* 父进程 不参与命令执行、循环等待子进程结束 */
			for(int i = 0; i < pipe_num; ++i){
				close(pfd[i][0]);
				close(pfd[i][1]);
			}
			for (int i = 0; i < cmd_num; ++i) {
				wait(NULL);
			}
		}	

	}


	return 0;
}