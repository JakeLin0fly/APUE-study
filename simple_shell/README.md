# 简易shell

* 拆分组合shell命令
* 一个子进程负责一条单一命令执行
* 进程间通过管道进行通信，dup2重定向

![simple_shell](simple_shell.png)