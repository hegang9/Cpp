### C++ 网络编程进阶知识补充（排除系统接口差异）

### TCP 连接与状态机
#### TCP 状态机与异常状态
- 重点状态：LISTEN、SYN_SENT、SYN_RECV、ESTABLISHED、FIN_WAIT_1、FIN_WAIT_2、CLOSE_WAIT、LAST_ACK、TIME_WAIT
- 常见问题：
	- **CLOSE_WAIT**：对端已关闭写端，本端未及时关闭导致资源泄露
	- **TIME_WAIT**：主动关闭方进入，目的是确保对端收到最后ACK与防止旧包干扰

#### TIME_WAIT 处理要点
- 2MSL 等待是协议要求，主要用于：
	1. 可靠关闭（对端重传 FIN 时仍可回 ACK）
	2. 防止旧连接的延迟包污染新连接
- 常见缓解：启用端口复用（`SO_REUSEADDR`/`SO_REUSEPORT`）与短连接池

### TCP 可靠性机制与性能权衡
#### 流量控制 vs 拥塞控制
- **流量控制**：端到端，基于接收窗口（rwnd）防止接收端被写爆
- **拥塞控制**：网络层面，基于拥塞窗口（cwnd）防止网络拥塞
- 常见算法：慢启动、拥塞避免、快速重传、快速恢复

#### Nagle 与延迟确认（Delayed ACK）
- **Nagle**：合并小包减少包数，适合吞吐但会增加延迟
- **Delayed ACK**：延迟确认以合并 ACK，可能与 Nagle 叠加造成“延迟放大”
- 低延迟业务：考虑 `TCP_NODELAY` 关闭 Nagle
```c
// 关闭 Nagle（降低延迟）
int flag = 1;
setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (const char*)&flag, sizeof(flag));
```

#### TCP Keepalive
- 作用：探测半开连接（对端断电/网络断）
- 误区：Keepalive 不是心跳协议的替代，只是内核级探测
```c
// 开启 TCP Keepalive（参数值需按平台调整）
int on = 1;
setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, (const char*)&on, sizeof(on));
```

### I/O 模型与多路复用细节
#### select/poll/epoll 的关键差异
- **select**：fd 数量受限（FD_SETSIZE），每次拷贝与遍历开销大
- **poll**：无 fd 数量上限，但仍需遍历所有 fd
- **epoll**：事件驱动，O(1) 级别通知

#### 触发模式
- **LT（水平触发）**：只要缓冲区可读/可写就一直通知，简单但可能频繁唤醒
- **ET（边沿触发）**：状态变化才通知，效率高但必须循环读/写到 EAGAIN

#### 惊群问题
- 多线程/多进程同时等待同一 fd 事件会被全部唤醒
- 解决：减少监听者数量或使用事件分发机制

### 超时、重试与重连策略
#### 超时分类
- 连接超时、读写超时、整体请求超时（deadline）

#### 读写超时设置示例
```c
// 设置读写超时（不会改变阻塞/非阻塞属性）
struct timeval tv;
tv.tv_sec = 3;
tv.tv_usec = 0;
setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));
setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (const char*)&tv, sizeof(tv));
```

#### 重试策略
- 指数退避 + 抖动（jitter），避免集群雪崩
- 对非幂等请求谨慎重试

### 数据收发的工程细节
#### 读满 N 字节的范式（长度前缀协议）
```c
// 伪代码：读取固定长度
int readn(int fd, void *buf, size_t n) {
		size_t left = n;
		char *p = (char*)buf;
		while (left > 0) {
				ssize_t r = recv(fd, p, left, 0);
				if (r == 0) return 0;              // 对端关闭
				if (r < 0) {
						if (errno == EINTR) continue;
						if (errno == EAGAIN) continue; // 非阻塞需配合事件
						return -1;
				}
				left -= r;
				p += r;
		}
		return 1;
}
```

#### 发送端写满范式
- send 可能返回部分写入，必须循环发送
- 非阻塞下需处理 EAGAIN/EWOULDBLOCK

#### 零拷贝思想
- 减少用户态/内核态的拷贝次数，提高吞吐
- 常见方式：`sendfile`、`splice`、`mmap`（平台相关）

### 安全与加密通信（TLS）
#### TLS 关键概念
- 握手阶段：证书校验、密钥交换、协商加密套件
- 会话复用：Session ID / Session Ticket

#### TLS 基础使用示例（OpenSSL）
```c
// 伪代码：OpenSSL 客户端握手流程
SSL_library_init();
SSL_load_error_strings();

const SSL_METHOD *method = TLS_client_method();
SSL_CTX *ctx = SSL_CTX_new(method);

// 加载系统证书或自定义 CA
SSL_CTX_set_default_verify_paths(ctx);
SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);

SSL *ssl = SSL_new(ctx);
SSL_set_fd(ssl, sockfd);

if (SSL_connect(ssl) == 1) {
	// 发送与接收
	SSL_write(ssl, buf, len);
	SSL_read(ssl, buf, sizeof(buf));
}

SSL_shutdown(ssl);
SSL_free(ssl);
SSL_CTX_free(ctx);
```

#### 证书校验要点
- 必须验证：证书链、域名（SNI/Host）匹配、有效期
- 常见问题：忽略证书校验会导致中间人攻击

### 错误处理与健壮性
#### 常见错误码
- `EINTR`：系统调用被信号中断，应重试
- `EAGAIN`/`EWOULDBLOCK`：非阻塞下暂不可读/写
- `ECONNRESET`：连接被对端重置

#### SIGPIPE 问题
- 对已关闭连接写入会触发 SIGPIPE 导致进程退出
- 解决：忽略 SIGPIPE 或使用 `MSG_NOSIGNAL`

### 地址解析与 IPv6
#### 推荐使用 getaddrinfo
- 同时支持 IPv4/IPv6
- 可统一处理域名解析、端口与协议族
```c
// getaddrinfo 示例（支持 IPv4/IPv6）
struct addrinfo hints;
memset(&hints, 0, sizeof(hints));
hints.ai_family = AF_UNSPEC;
hints.ai_socktype = SOCK_STREAM;

struct addrinfo *res = NULL;
int rc = getaddrinfo("example.com", "80", &hints, &res);
if (rc == 0) {
	// 使用 res 链表创建 socket 并 connect
	freeaddrinfo(res);
}
```

#### IPv6 双栈注意点
- 监听 `::` 时是否同时接收 IPv4（依赖 `IPV6_V6ONLY`）
- 地址文本长度差异（`INET6_ADDRSTRLEN`）

### UDP 进阶与可靠传输
#### UDP 的特点
- 无连接、无可靠性保障、可能丢包/乱序

#### 可靠 UDP 的常见手段
- 序列号 + ACK/NACK
- 超时重传
- 滑动窗口
- FEC（前向纠错）

### Socket 选项与性能调优
#### 常见选项
- `SO_REUSEADDR` / `SO_REUSEPORT`：端口复用
- `SO_RCVBUF` / `SO_SNDBUF`：缓冲区大小
- `TCP_NODELAY`：关闭 Nagle
- `SO_LINGER`：控制 close 的行为

#### 常见选项设置示例
```c
// 端口复用（服务端重启友好）
int on = 1;
setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const char*)&on, sizeof(on));

// 设置收发缓冲区
int buf_size = 1 << 20; // 1MB
setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, (const char*)&buf_size, sizeof(buf_size));
setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, (const char*)&buf_size, sizeof(buf_size));

// 设置 close 行为（立即关闭或等待发送完成）
struct linger ling;
ling.l_onoff = 1;
ling.l_linger = 0; // 0 表示立即丢弃未发送数据
setsockopt(sockfd, SOL_SOCKET, SO_LINGER, (const char*)&ling, sizeof(ling));
```

#### 调优思路
- 高并发：适当增大缓冲区
- 低延迟：关闭 Nagle、减少应用层缓存

#### 性能调优组合示例
```c
// 关闭 Nagle，降低延迟
int on = 1;
setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (const char*)&on, sizeof(on));

// 调整缓冲区
int rcv = 1 << 20;
int snd = 1 << 20;
setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, (const char*)&rcv, sizeof(rcv));
setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, (const char*)&snd, sizeof(snd));

// 关闭 linger（避免 close 阻塞）
struct linger ling;
ling.l_onoff = 0;
ling.l_linger = 0;
setsockopt(sockfd, SOL_SOCKET, SO_LINGER, (const char*)&ling, sizeof(ling));
```

### 并发模型与架构
#### 常见模型
- 单 Reactor：简单，适合中小规模
- 主从 Reactor：主线程接收连接，子线程处理 I/O
- Proactor：完成事件通知，常见于 IOCP

#### 线程安全与队列
- 生产者/消费者队列
- 连接对象的生命周期管理（防止悬挂引用）

### 协议设计与消息结构
#### 消息头常见字段
- 魔数、版本号、长度、序列号、校验和、类型

#### 序列化方案
- JSON：可读性好，性能一般
- Protobuf/FlatBuffers：高性能、跨语言

### 抓包与诊断
#### 常用工具
- tcpdump/Wireshark：分析丢包、重传、窗口变化
- netstat/ss：查看连接状态与端口占用

#### 关键指标
- RTT、重传率、拥塞窗口变化
- QPS、连接数、平均延迟、P99
