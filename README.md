## apue-study-code

---

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
* 进程同步
  * 使用**信号量**实现进程间同步(mmap映射)
  * 使用**互斥量mutex**实现进程同步(mmap映射)：[process_mutex.c](https://github.com/JakeLin0fly/apue-study-code/blob/master/process_mutex.c)
  * 使用**文件锁fcntl**实现进程同步：[process_fcntl.c](https://github.com/JakeLin0fly/apue-study-code/blob/master/process_fcntl.c)

### 线程

---

* **互斥锁和条件变量**解决生产者-消费者问题模型：[producer_consumer_mutex_cond.c](https://github.com/JakeLin0fly/apue-study-code/blob/master/producer_consumer_mutex_cond.c)

* **信号量**解决生产者-消费者问题模型：[producer_consumer_sem.c](https://github.com/JakeLin0fly/apue-study-code/blob/master/producer_consumer_sem.c)

  ***注：此代码仅做演示使用，实际使用信号量同步时，互斥信号量mutex应该仅锁资源位置（即mutex_in锁第一个空闲区块位置in、mutex_out锁第一个资源位置out）***