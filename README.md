# apue-study-code

### 文件操作

---

* ls -a 命令实现：[ls-a.c](https://github.com/JakeLin0fly/apue-study-code/blob/master/ls-a.c)
* 递归遍历目录信息：[readdir.c](https://github.com/JakeLin0fly/apue-study-code/blob/master/readdir.c)

### 进程

---

* 管道原理：[pipe.c](https://github.com/JakeLin0fly/apue-study-code/blob/master/pipe.c)

* pause实现sleep(存在时序竞态问题)：[sig_sleep.c](https://github.com/JakeLin0fly/apue-study-code/blob/master/sig_sleep.c)
* sigsuspend实现sleep(解决时序竞态问题)：[sig_sleep_sigsuspend.c](https://github.com/JakeLin0fly/apue-study-code/blob/master/sig_sleep_sigsuspend.c)

* 管道、多进程实现shell命令解析执行：[simple_shell](https://github.com/JakeLin0fly/apue-study-code/tree/master/simple_shell)
* mmap存储映射实现多进程文件拷贝：[mult_process_cp](https://github.com/JakeLin0fly/apue-study-code/tree/master/mult_process_cp)

### 线程

---

* 互斥锁和条件变量解决生产者消费者问题模型：[producer_consumer_mutex_cond.c](https://github.com/JakeLin0fly/apue-study-code/blob/master/producer_consumer_mutex_cond.c)