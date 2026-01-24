## 基本原理
Raft算法是分布式算法中的一种强一致性算法，其将分布式系统中所有节点分为Follower追随者、Candidate候选者、Leader领导者：

1. Follower：被动响应领导者的心跳和日志复制请求
2. Candidate：选举过程中的临时角色，由超时的Follower转换而来
3. Leader：处理所有客户端请求，负责日志复制。同一时刻有且仅有一个

   
## Leader选举：
#### term 任期：

一个逻辑时钟概念，**用连续递增的整数表示**。**每次选举都会开启一个新的任期**，**每一段任期从一次选举开始**，在某些情况下，一次选举无法选出Leader，这一任期会以没有Leader而立马结束，并开启下一次任期并再次选举。

任期用于**帮助节点识别过期的消息和领导者**。节点通信时会交换当前任期号，如果某个节点发现自己的任期号比收到的任期号小，则会更新自己的任期号为大值；如果一个节点收到一个包含过期任期号的请求，则会直接拒绝这个请求。

#### 心跳机制：

Raft实现了一种心跳机制，Leader通过发送心跳来告知所有Follower当前有Leader，如果Follower在一段时间内没有收到心跳，就会开始进行选举。

#### 选举：

1. 所有节点初始化都为Follower，通过心跳机制去判断有无Leader，若心跳机制超时，代表没有Leader，则该节点先增加自己的当前任期号，并将自己变为Candidate。
2. 成为Candidate的节点首先投票给自己，再向其他Follower发起投票请求（即后述RequestVote RPC），多个Candidate通过随机事件来避免同时选票，当某个Candidate获得的票数**超过节点数的一半**时成为Leader。
3. 成为Leader的节点开始日志同步、日志提交等操作，并发送心跳给所有Follower。
4. 收到新Leader心跳的Candidate会检查新Leader的任期号，如果新Leader的任期号不小于自己的任期号（正常情况下是相等），则拥护新Leader，将自己降为Follower。

  <img src="image.png" width="500" />

#### 随机选举超时时间：

如果在一个任期内没有任何一个Candidate得票数超过半数，则每个Candidate在等待一个在一定范围内随机的选举超时时间后，默认再次发起选举。

#### 投票：

每个Follower节点有一个选票，采用**先来先得**的策略投票（成为Candidate的节点不参与投票，因为他们已经把票投给自己了）。

投票条件：
1. 发起投票请求的Candidate的任期号是否大于自己的任期号
2. 


#### RPC通信：

Raft算法中，服务器节点之间通过RPC协议进行通信，有两种主要的RPC：
1. RequestVote RPC：请求投票，由Candidate在选举期间发起
2. AppendEntries RPC：追加条目，由Leader发起，用来复制日志和提供心跳。

```c++
// RequestVote RPC Request
struct RequestVoteRequest{
    int term;   // 自己当前的任期号
    int candidateId;    // 自己的ID
    int lastLogIndex;   // 自己最后一个日志号
    int lastLogTerm;    // 自己最后一个日志的任期
}

// AppendEntries RPC Response
struct RequestVoteResponse{
    int term;   // 自己当前的任期号
    bool voteGranted;   // 自己会不会投票给这个Candidate
}
```


#### 客户端如何得知哪个是Leader节点？
有三种情况：

1. 客户端找到的节点就是Leader：直接执行指令
2. 客户端找到的节点是Follower：通过心跳机制告知客户端去找Leader节点
3. 客户端找到的节点宕机了：客户端找另一个节点

## Raft日志
#### 日志：

日志在Raft中用于**保存客户端请求命令**，是实现一致性的核心载体。Raft让多个节点的上层状态机保持一致的关键时让各节点的日志保持一致。

#### 日志条目：

每个日志条目包含三个重要信息：
1. 索引：日志条目的唯一顺序编号，即日志号
2. 任期号：创建该条目时的任期信息
3. 命令：客户端请求的操作指令

- 注意：**索引和任期号两个因素才可唯一确定一个日志，缺一不可**

#### 日志复制和提交：

Leader收到客户端的指令后，会把指令作为一个新的日志条目追加到日志中去，接着**并行发送**AppendEntries RPC给所有Follower，让他们复制该条目。当**超过半数的Follower复制**后，Leader就在本地执行该指令并把结果返回客户端，本地执行指令并返回给客户端称作**日志提交**。

```c++
// AppendEntries RPC Request
struct AppendEntriesRequest{
    int term;   // 自己当前的任期号
    int leaderId;   // Leader（自己）的ID
    int prevLogIndex;    // 前一个日志条目的日志号
    int prevLogTerm;    // 前一个日志的任期号
    char[] entry;   // 当前日志体
    int leaderCommit;   // Leader已经提交的日志号
}

// AppendEntries RPC Response
struct AppendEntriesResponse{
    int term;   // 自己当前任期号
    bool success;   // 如果Request的term大于自己的term且Follower包括前一个日志，则返回true
}
```
int leaderCommit 的作用：Leader将自己已经提交的最后一个日志条目的日志号传递给Follower，然后Follower就可以将leaderCommit之前已经复制的日志条目进行提交



#### 一致性保证：

在日志复制过程中，Leader和Follower随时都有崩溃或缓慢的可能性，Raft必须保证在有宕机的情况下，保证每个副本日志顺序的一致。具体有三种可能：

1. 如果Follower由于某些原因（网络延迟等）没有给Leader响应，那么Leader会不断重发追加条目请求，无论Leader是否已经回复了客户端；
2. 如果Follower宕机后恢复，这时**一致性检查生效**，保证Follower能够按照顺序恢复宕机期间缺失的日志；
3. 如果Leader宕机，那么宕机的Leader节点可能已经复制了日志到部分Follower但**还没有提交**，而被选出的新Leader又可能不具备这些日志，这样就会导致部分Follower中的日志和新Leader中的日志不一致。在这种情况下，Leader通过一致性检查找到Follower中最后一个跟自己一致的条目，这个条目之后，Leader强制Follower复制它的日志来解决不一致问题（这意味着Follower中跟Leader冲突的日志条目会被舍弃）

#### 一致性检查：

Leader在每一个发往Follower的追加条目RPC中，会放入前一个日志条目的索引位置和任期号，如果Follower找不到前一个日志条目，那么它会拒接，Leader收到Follower的拒绝信息后，会发送前一个日志条目的索引，从而逐渐向前定位到Follower第一个缺失的日志。

优化：默认的一致性检查是线性查找，时间复杂度为O(n)，可以优化为让Follower返回自己的最后一个日志条目的索引和任期，或者优化为二分查找


#### 疑问

若Leader在过半数Follower复制日志后提交日志，接着在提交日志后宕机，那么那些未复制日志的Follower的日志（多）与新Leader的日志（少）不一致怎么办，如果按照一致性检查找到Follower中最后一个跟自己一致的条目，在这个条目之后，Leader强制Follower复制它的日志来解决不一致问题，那么Follower中那些已经提交的日志条目不久丢失了吗？这类宕机问题是如何处理的？

## 安全性
领导者选举和日志复制两个子问题实际上已经涵盖了共识算法的全程，但这两点还不能完全保证每一个状态机会按照相同的顺序执行相同的命令。所以Rft通过几个补充规则完善整个算法，使算法可以在各类宕机问题下都不出错。
这些规则包括（不讨论安全性条件的证明过程）：
1. Leader宕机处理：选举限制
2. Leader宕机处理：新leader是否提交之前任期内的日志条目
3. Follower和Candidate:宕机处理
4. 时间与可用性限制