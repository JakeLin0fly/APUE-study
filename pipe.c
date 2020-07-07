/**
* 管道原理理解，读写特性
①读管道: 
  1.管道中有数据，read 返回实际读到的字节数。。
  2.管道中无数据:。
    (1)管道写端被全部关闭，read 返回0 (好像读到文件结尾)。
    (2)写端没有全部被关闭，read 阻塞等待(不久的将来可能有数据递达，此时会让出cpu)
②写管道:
  1.管道读端全部被关闭， 进程异常终止(也可使用捕捉 SIGPIPE 信号，使进程不终止)。
  2.管道读端没有全部关闭:
    (1)管道已满，write 阻塞。
    (2)管道未满，write 将数据写入，并返回实际写入的字节数。
*/

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>


int main(int argc, char const *argv[]) {
  int fd[2];
  int ret;
  pid_t pid;
  char *str = "hello pipe\n";
  char buf[1024];

  //创建pipe
  ret = pipe(fd);
  if (-1 == ret) {
    perror("pipe create error: ");
    exit(1);
  }

  if (-1 == (pid = fork())) {
    perror("fork error: ");
  }
  else if (pid > 0) { //父进程写管道
    close(fd[0]); //关闭读端
    sleep(2); //父进程sleep，管道无数据且写端未全部关闭，子进程阻塞等待
    write(fd[1], str, strlen(str)); //写入数据
    wait(NULL);
    close(fd[1]); 
  }
  else if (0 == pid) { //子进程读管道
    close(fd[1]); //关闭写端
    ret =read(fd[0], buf, sizeof(buf));
    if (-1 == ret) {  //读数据
      perror("read error: ");
      exit(1);
    }
    write(STDOUT_FILENO, buf, ret); //输出到屏幕
    close(fd[0]);
  }


  return 0;
}
