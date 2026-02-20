# mmap详解

## 1. mmap是什么？
mmap（memory map）是Linux/Unix系统提供的一种内存映射机制，可以将一个文件或其他对象映射到进程的虚拟地址空间，实现文件内容与内存的直接映射。

## 2. mmap的底层原理
- **虚拟内存映射**：mmap通过**内核页表**机制，将文件内容与进程虚拟地址空间建立映射关系。访问映射区域时，实际上是直接操作文件内容。
- **页缺失处理**：当访问的虚拟地址尚未分配物理页时，内核会触发缺页异常（page fault），自动将文件内容加载到物理内存。
- **脏页回写**：对映射区域的写操作会标记为脏页，内核会在适当时机（如msync、munmap或文件关闭时）将修改同步回文件。
- **零拷贝**：mmap避免了read/write的用户态与内核态缓冲区拷贝，提升了I/O效率。
- **共享/私有映射**：可选择MAP_SHARED（多进程共享，修改会同步到文件）或MAP_PRIVATE（写时复制，修改不影响原文件）。

## 3. mmap的函数原型
```c
#include <sys/mman.h>
void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
```
- **addr**：建议的映射起始地址，通常为NULL，由内核自动分配。
- **length**：映射的字节数，建议为页大小的整数倍。
- **prot**：访问权限，如PROT_READ、PROT_WRITE、PROT_EXEC。
- **flags**：映射类型和属性，如MAP_SHARED、MAP_PRIVATE、MAP_ANONYMOUS。
- **fd**：要映射的文件描述符。
- **offset**：文件映射的起始偏移，必须为页大小的整数倍。

## 4. mmap的典型用法
### 文件映射
```c
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

int main() {
	int fd = open("test.txt", O_RDWR | O_CREAT, 0666);
	ftruncate(fd, 4096); // 设置文件大小
	char* addr = (char*)mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	strcpy(addr, "Hello mmap!"); // 直接写入映射内存，相当于写文件
	msync(addr, 4096, MS_SYNC); // 手动同步到磁盘（可选）
	munmap(addr, 4096); // 解除映射
	close(fd);
	return 0;
}
```

### 匿名映射（进程间共享内存）
```c
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

int main() {
	char* addr = (char*)mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	pid_t pid = fork();
	if (pid == 0) {
		sleep(1);
		printf("Child read: %s\n", addr);
	} else {
		strcpy(addr, "Hello shared memory!");
	}
	munmap(addr, 4096);
	return 0;
}
```

## 5. mmap的应用场景
- 大文件高效读写（如数据库、日志分析、内存映射文件I/O）
- 多进程间共享内存（匿名映射或映射同一文件）
- 设备驱动用户态访问（如显卡帧缓冲区）
- 代码段/动态库的加载

## 6. mmap的优缺点
### 优点
- 零拷贝，I/O效率高，适合大数据量场景
- 支持进程间共享内存
- 代码简洁，直接操作内存即可读写文件
### 缺点
- 需要注意同步（多进程/多线程并发写）
- 文件大小和映射长度需对齐页大小
- 错误处理复杂，容易出现段错误
- 不是所有文件都支持mmap（如管道、字符设备等）

## 7. mmap常见陷阱与注意事项
- **同步问题**：MAP_SHARED时多进程/线程写入需加锁，否则数据竞争。
- **脏页未及时回写**：建议用msync或munmap确保数据落盘。
- **映射长度和偏移需页对齐**：否则mmap调用会失败。
- **文件截断/扩展需谨慎**：映射期间改变文件大小可能导致SIGBUS。
- **资源泄漏**：忘记munmap会导致虚拟内存泄漏。
- **匿名映射不可与文件fd混用**：MAP_ANONYMOUS时fd必须为-1。

## 8. 相关系统调用
- munmap：解除映射
- msync：同步内存到磁盘
- mprotect：更改映射区域权限
- madvise：内存访问优化建议

## 9. 参考资料
- man 2 mmap
- 《Linux高性能服务器编程》
- 《深入理解Linux内核》
