## 进程
资源分配的基本单位

## fork系统调用原理

### fork的基本概念
- `fork()` 是 Linux 系统中创建新进程的系统调用
- 调用一次,返回两次:父进程返回子进程的 PID,子进程返回 0
- 如果创建失败,父进程返回 -1

### fork的返回值
```cpp
pid_t pid = fork();
if (pid < 0) {
    // fork失败
    perror("fork failed");
} else if (pid == 0) {
    // 子进程
    printf("This is child process\n");
} else {
    // 父进程,pid是子进程的ID
    printf("This is parent process, child pid = %d\n", pid);
}
```

## 写时复制(Copy-On-Write, COW)

### COW机制
- fork 后,子进程不会立即复制父进程的所有内存页
- 父子进程共享相同的物理内存页,但都标记为只读
- 当任一进程尝试修改某个页面时,才会触发页错误,内核复制该页面
- 大大提高了 fork 的效率,避免不必要的内存复制

### COW的优势
1. **减少内存消耗**: 只在需要时复制
2. **提高创建速度**: fork 调用几乎立即返回
3. **节省系统资源**: 对于 exec 场景特别有效(fork后立即exec,避免了无意义的复制)

## 多进程fork后不同进程的共享与独立

### 共享的资源
1. **代码段(Text Segment)**: 只读,可以共享
2. **打开的文件描述符**: 
   - 文件描述符表被复制,但指向相同的文件表项
   - 共享文件偏移量(offset)
   - 一个进程改变偏移量会影响另一个进程
3. **信号处理设置**: 继承父进程的信号处理函数
4. **当前工作目录**: 继承父进程的工作目录
5. **用户ID和组ID**: uid, gid, euid, egid 等
6. **环境变量**: 复制父进程的环境变量
7. **共享内存段**: 通过 shmget 等创建的共享内存
8. **内存映射(mmap)**: MAP_SHARED 方式映射的内存

### 独立的资源(子进程独有)
1. **进程ID(PID)**: 子进程有新的 PID
2. **父进程ID(PPID)**: 子进程的 PPID 是父进程的 PID
3. **数据段、堆、栈**: 通过 COW 机制逻辑上独立
4. **文件锁**: 不继承父进程的文件锁
5. **待处理信号集**: 子进程清空待处理信号
6. **进程资源使用统计**: CPU时间等重新计时
7. **定时器**: alarm、setitimer 设置的定时器不继承
8. **异步I/O操作**: 不继承
9. **信号量调整值(semadj)**: 不继承

## fork的使用场景

### 1. 服务器并发处理
```cpp
// 传统的并发服务器模型
while (1) {
    int client_fd = accept(server_fd, ...);
    pid_t pid = fork();
    if (pid == 0) {
        // 子进程处理客户端请求
        close(server_fd);  // 子进程不需要监听套接字
        handle_client(client_fd);
        close(client_fd);
        exit(0);
    } else {
        // 父进程继续接受新连接
        close(client_fd);  // 父进程不需要客户端套接字
    }
}
```

### 2. 执行外部程序
```cpp
pid_t pid = fork();
if (pid == 0) {
    // 子进程执行新程序
    execl("/bin/ls", "ls", "-l", NULL);
    perror("execl failed");
    exit(1);
} else {
    // 父进程等待子进程完成
    wait(NULL);
}
```

### 3. 并行任务处理
```cpp
// 创建多个子进程并行处理任务
for (int i = 0; i < num_workers; i++) {
    pid_t pid = fork();
    if (pid == 0) {
        // 子进程处理任务
        process_task(i);
        exit(0);
    }
}
// 父进程等待所有子进程
while (wait(NULL) > 0);
```

## fork的性能考虑

### 优点
- 进程间完全隔离,稳定性高
- 一个进程崩溃不影响其他进程
- 利用多核CPU并行处理

### 缺点
- 创建进程开销较大(相比线程)
- 进程间通信(IPC)较复杂
- 内存占用较多(即使有COW)

### 优化建议
1. 对于大量并发,考虑使用线程池或进程池
2. 使用 `vfork()` (但要小心使用,现代系统中已较少使用)
3. 考虑使用 `clone()` 系统调用实现更精细的控制
4. 对于I/O密集型任务,考虑使用异步I/O或多路复用

## 孤儿进程与僵尸进程

### 孤儿进程(Orphan Process)
- 父进程先于子进程结束
- 子进程会被 init 进程(PID=1)收养
- 不会造成资源泄漏

### 僵尸进程(Zombie Process)
- 子进程已终止,但父进程未调用 wait/waitpid 回收
- 保留进程表项,占用 PID 资源
- 大量僵尸进程会耗尽系统 PID

### 避免僵尸进程
```cpp
// 方法1: 父进程及时调用wait
pid_t pid = fork();
if (pid > 0) {
    wait(NULL);  // 等待子进程结束
}

// 方法2: 信号处理
signal(SIGCHLD, SIG_IGN);  // 忽略SIGCHLD信号,自动回收子进程

// 方法3: 信号处理函数
void sigchld_handler(int sig) {
    while (waitpid(-1, NULL, WNOHANG) > 0);  // 非阻塞回收所有子进程
}
signal(SIGCHLD, sigchld_handler);

// 方法4: 双重fork
pid_t pid = fork();
if (pid == 0) {
    // 第一个子进程
    if (fork() == 0) {
        // 第二个子进程,真正执行任务
        do_work();
        exit(0);
    }
    exit(0);  // 第一个子进程立即退出
}
wait(NULL);  // 回收第一个子进程
// 第二个子进程成为孤儿进程,由init收养
```

## fork与多线程

### 注意事项
- 在多线程程序中调用 fork,只有调用线程被复制到子进程
- 其他线程在子进程中消失,但其占用的资源(如锁)可能仍然存在
- 可能导致死锁等问题

### 安全实践
```cpp
// fork后应该立即exec,避免在多线程环境下的复杂问题
pid_t pid = fork();
if (pid == 0) {
    execl(...);  // 立即执行新程序
    _exit(1);
}
```

### pthread_atfork
```cpp
// 注册fork处理函数
void prepare(void) {
    // fork前在父进程中调用,通常用于获取锁
}

void parent(void) {
    // fork后在父进程中调用,释放锁
}

void child(void) {
    // fork后在子进程中调用,重新初始化锁
}

pthread_atfork(prepare, parent, child);
```
