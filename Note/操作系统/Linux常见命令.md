## Linux常见命令
### 1. 文件和目录操作
- **`ls` (List)**：列出目录中的文件和子目录。
- **`cd` (Change Directory)**：切换当前工作目录。
- **`pwd` (Print Working Directory)**：打印当前所在的工作目录的绝对路径。
- **`mkdir` (Make Directory)**：创建一个新的空目录。
- **`rm` (Remove)**：删除文件或目录（删除目录常搭配 `-r` 参数）。
- **`cp` (Copy)**：复制文件或目录。
- **`mv` (Move)**：移动文件/目录，或者用于重命名文件/目录。
- **`touch`**：创建一个空文件，或者更新存量文件的时间戳（全称无特殊缩写，意为“触摸”文件以修改时间）。

### 2. 文件查看与处理
- **`cat` (Concatenate)**：连接文件并打印到标准输出设备上（常用于查看少量内容的文件）。
- **`more` / `less`**：分页查看文件内容（`less` 支持向前翻页，比 `more` 更强大）。
- **`head`**：查看文件的开头部分内容（默认前 10 行）。
- **`tail`**：查看文件的结尾部分内容（默认后 10 行，常搭配 `-f` 实时监控日志）。
- **`grep` (Global Regular Expression Print)**：使用正则表达式全局搜索文本，并把匹配的行打印出来。

### 3. 系统与进程管理
- **`ps` (Process Status)**：报告当前系统的进程状态（常搭配 `aux` 或 `-ef` 使用）。
- **`top`**：实时显示系统资源使用情况及各个进程的状态（类似于 Windows 的任务管理器）。
- **`kill`**：向进程发送信号，通常用于终止（杀死）指定 PID 的进程。
- **`df` (Disk Free)**：显示文件系统的磁盘空间占用情况。
- **`du` (Disk Usage)**：评估和显示文件或目录占用的磁盘空间。

### 4. 权限与用户管理
- **`chmod` (Change Mode)**：更改文件或目录的访问权限（如 `chmod 777 file`）。
- **`chown` (Change Owner)**：更改文件或目录的所有者和所属组。
- **`su` (Switch User)**：切换当前用户身份（`su -` 切换至 root 用户并应用其环境变量）。
- **`sudo` (Superuser DO)**：以超级用户（root）或其他特定用户的权限执行命令。

### 5. 网络操作
- **`ping` (Packet Internet Groper)**：向目标主机发送 ICMP 数据包，测试网络连通性。
- **`ifconfig` (Interface Configuration)**：查看或配置网络接口（在新版本 Linux 中常被 `ip` 命令替代）。
- **`netstat` (Network Statistics)**：打印网络连接、路由表、接口统计等信息（常搭配 `-tuln` 查看监听端口）。

### 6. 文件压缩与打包
- **`tar` (Tape Archive)**：最初用于磁带备份，现在常用于将多个文件或目录打包成一个文件，并可结合 gzip/bzip2 进行压缩（如 `tar -czvf` 打包压缩，`tar -xzvf` 解压）。
- **`zip` / `unzip`**：用于压缩 / 解压 `.zip` 格式的文件。
