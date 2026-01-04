### CPU占用率较高，如何排查？
首先明确：进程是CPU资源分配的基本单位，线程是CPU调度的基本单位
1. 先找到进程，通过进程找到线程，在线程中定位代码位置
2. 分析原因，主要原因可能有以下几点：
   1. 代码逻辑出错：死循环/算法复杂度过高
   2. 多线程并发中，自旋锁忙等导致CPU空转
   3. 频繁的内存分配释放，程序设计局部性差，缓存命中率低
   4. 频繁的系统调用导致频繁进行用户态与系统态之间的切换，上下文切换占用CPU资源

- 流程命令参考如下：
    1. 编译运行
    2. 使用top -c命令找到当前运行的进程，保存该进程的pid
    3. 使用top -H -p <pid>显示该进程的所有线程信息，定位到线程
    4. gdb -p <pid>-ex "thread apply all bt"-ex "detach"-ex "quit">stack_dump.log打印耗时线程堆栈信息，定位代码位置
    5. 查看日志

- 其他更为现代的方法：
    1. **使用 perf 工具 (Linux)**
       `perf` 是 Linux 内核自带的性能分析工具，功能强大。
       - **实时查看热点函数**：
         ```bash
         perf top -p <pid>
         ```
       - **记录并分析**：
         ```bash
         # 记录进程的性能数据，-g 表示记录调用栈
         perf record -g -p <pid> -- sleep 10
         # 解析 perf.data 文件并展示
         perf report
         ```

    2. **生成火焰图 (Flame Graph)**
       火焰图可以直观地展示 CPU 在各个函数上的消耗比例。
       需要配合 `perf` 和 Brendan Gregg 的 FlameGraph 脚本。
       ```bash
       # 1. 采集数据
       perf record -F 99 -p <pid> -g -- sleep 30
       
       # 2. 解析数据并折叠堆栈
       perf script | ./stackcollapse-perf.pl > out.perf-folded
       
       # 3. 生成 SVG 火焰图
       ./flamegraph.pl out.perf-folded > perf.svg
       ```
       生成的 `perf.svg` 可以用浏览器打开，横轴表示 CPU 占用时间，纵轴表示调用栈深度。

    3. **eBPF 与 BCC 工具包**
       eBPF 技术允许在内核中运行沙盒程序，BCC 提供了大量开箱即用的性能分析工具，比传统工具开销更低，适合生产环境。
       - **runqlat**: 测量 CPU 调度延迟，查看线程是否因为 CPU 资源紧张而在等待调度。
       - **offcputime**: 分析线程被阻塞、休眠或等待 I/O 的时间（即“非 CPU 时间”去哪了）。
       - **syscount**: 统计系统调用次数和延迟。

    4. **内存性能分析 (Valgrind / gperftools)**
       频繁的内存分配或缓存未命中（Cache Miss）往往是 CPU 瓶颈的隐形杀手。
       - **Valgrind (Callgrind)**:
         提供指令级的调用图分析，能精确统计函数调用次数和 CPU 周期（注意：运行速度极慢）。
         ```bash
         valgrind --tool=callgrind ./your_program
         # 使用 KCachegrind 可视化查看 callgrind.out.<pid>
         ```
       - **Google Perftools (gperftools)**:
         包含极其高效的 CPU profiler 和 Heap profiler，适合在生产环境挂载使用。

    5. **系统调用分析 (strace)**
       如果 CPU 占用主要集中在内核态 (sys)，可能是系统调用滥用。
       ```bash
       # 统计进程的系统调用耗时和次数
       strace -c -p <pid>
       ``` 