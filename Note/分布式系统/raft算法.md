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

**Raft的心跳是一种特殊的 AppendEntries RPC，无日志体**

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
2. 自己的lastLogIndex和lastLogTerm比Candidate的新


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

// RequestVote RPC Response
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

#### 日志复制：

Leader收到客户端的指令后，会把指令作为一个新的日志条目追加到日志中去，接着**并行发送**AppendEntries RPC给所有Follower，让他们复制该条目。

#### 日志提交：

**Leader提交：**
当**超过半数的Follower复制**后，Leader就在本地执行该指令并把结果返回客户端，本地执行指令并返回给客户端称作**日志提交**。

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
**Follower提交：**
Leader通过心跳（特殊的AppendEntries RPC）或者AppendEntries RPC中的leaderCommit字段来通知Follower，int leaderCommit 的作用：Leader将自己已经提交的最后一个日志条目的日志号传递给Follower，然后Follower就可以将leaderCommit之前已经复制的日志条目进行提交。



#### 一致性保证：

在日志复制过程中，Leader和Follower随时都有崩溃或缓慢的可能性，Raft必须保证在有宕机的情况下，保证每个副本日志顺序的一致。具体有三种可能：

1. 如果Follower由于某些原因（网络延迟等）没有给Leader响应，那么Leader会不断重发追加条目请求，无论Leader是否已经回复了客户端；
2. 如果Follower宕机后恢复，这时**一致性检查生效**，保证Follower能够按照顺序恢复宕机期间缺失的日志；
3. 如果Leader宕机，那么宕机的Leader节点可能已经复制了日志到部分Follower（**不过半数，还没达到提交条件**）但**还没有提交**，而被选出的新Leader又可能不具备这些日志，这样就会导致部分Follower中的日志和新Leader中的日志不一致。在这种情况下，Leader通过一致性检查找到Follower中最后一个跟自己一致的条目，这个条目之后，Leader强制Follower复制它的日志来解决不一致问题（这意味着Follower中跟Leader冲突的日志条目会被舍弃）（注意：这与后述安全性第二点不同，这里是还未复制过半，还未达到提交条件）

#### 一致性检查：

Leader在每一个发往Follower的追加条目RPC中，会放入前一个日志条目的索引位置和任期号，如果Follower找不到前一个日志条目，那么它会拒接，Leader收到Follower的拒绝信息后，会发送前一个日志条目的索引，从而逐渐向前定位到Follower第一个缺失的日志。

优化：默认的一致性检查是线性查找，时间复杂度为O(n)，可以优化为让Follower返回自己的最后一个日志条目的索引和任期，或者优化为二分查找


#### 疑问

若Leader在过半数Follower复制日志后提交日志，接着在提交日志后宕机，那么那些未复制日志的Follower的日志（多）与新Leader的日志（少）不一致怎么办，如果按照一致性检查找到Follower中最后一个跟自己一致的条目，在这个条目之后，Leader强制Follower复制它的日志来解决不一致问题，那么Follower中那些已经提交的日志条目不久丢失了吗？这类宕机问题是如何处理的？

## 安全性
领导者选举和日志复制两个子问题实际上已经涵盖了共识算法的全程，但这两点还不能完全保证每一个状态机会按照相同的顺序执行相同的命令。所以Rft通过几个补充规则完善整个算法，使算法可以在各类宕机问题下都不出错。
这些规则包括（不讨论安全性条件的证明过程）：
1. Leader宕机处理：**选举限制**
    
    这个就是为了处理疑问中描述的场景，确保选出来的Leader包含了之前所有任期的所有被提交的条目。选举限制是通过RequestVote RPC Request中的lastLogIndex（Candidate的最后一个日志号）和lastLogTerm（Candidate最后一个日志的任期）来实现的：

    **投票者（即Follower）会检查这两个字段，若自己的日志比Candidate的日志还要新，就会拒绝该投票请求。先比较任期号，再比较日志号（比较顺序不能变）。**

2. Leader宕机处理：新leader是否提交之前任期内的日志条目

    如果某个Leader在提交之前（已经复制过半，达到了提交条件）宕机了，选举出的新Leader会试图完成未提交日志条目的复制，注意**只复制不提交，Raft永远不会通过计算副本数目的方式来提交之前任期内未提交的日志条目**。

    那么之前任期内的日志条目如何提交？通过提交当前任期的新日志来**间接提交**该旧日志。
3. Follower和Candidate:宕机处理

    如果Follower和Candidate宕机了，那么后续发给他们的RPC都会失败，Raft通过无限重试来处理这种失败，一旦宕机的机器重启了，那么这些RPC就会成功完成。
4. 时间与可用性限制

    **如果一次网络来回的时间大于选举超时时间，那么就永远选不出Leader**。

    **如果系统宕机特别频繁，每次宕机的间隔要短于选举超时时间，这时会出现永远也完成不了选举的情况**。

    即：广播时间(broadcastTime)<<选举超时时间(electionTimeout)<<平均故障时间(MTBF)

## 集群成员变更
集群成员变更指的是增删节点，替换宕机的机器或者改变复制的程度等会改变集群配置的操作。Raft可以进行**集群配置变更自动化**。

自动化变更配置机制的最大的难点是保证转换过程中不会出现同一任期的两个leader,因为转换期间整个集群可能划分为两个独立的大多数。这就是分布式系统中经典的**脑裂问题**。

#### 两阶段方法和联合一致状态：

1. 第一阶段：Leader发起 C<sub>old,new</sub>，使整个集群达到联合一致状态。这时，所有RPC都要在新旧两个配置中都达到大多数复制才算成功。
2. 第二阶段：Leader发起 C<sub>new</sub>，使整个集群进入新配置状态。这时，所有RPC只要在新配置下达到大多数复制就算成功。因为 C<sub>new</sub> 发起意味着 C<sub>old,new</sub> 已经复制到了大多数节点，不需要再管老配置了。
   
C<sub>old,new</sub>和 C<sub>new</sub> 将配置信息作为一个日志体，包装在一个AppendEntries RPC Request中，发送给所有的Follower。一旦某个节点将C<sub>old,new</sub> 或 C<sub>new</sub> 复制到自己的日志中，它就会利用该配置来做出未来所有的决策（**服务器总是使用它日志中最新的配置，无论该配置日志是否已经被提交**）。这意味着Leader不用等待C<sub>old,new</sub> 或 C<sub>new</sub> 返回，就会直接使用新规则来做出决策。

#### 状态变更时的宕机情况：
1. Leader在 C<sub>old,new</sub> 未提交时宕机
2. Leader在 C<sub>old,new</sub> 已提交，C<sub>new</sub> 未发起时宕机
3. Leader在 C<sub>new</sub> 已发起后宕机

上面三种宕机情况会因为安全性限制而保证安全，但是有一个例外：

![alt text](image-1.png)

#### 补充规则：
1. 新增节点时，需要等待新增的节点完成日志同步再开始集群成员变更
2. 缩减节点时，leader本身可能就是要缩减的节点，这时它会在完成 C<sub>new</sub>的提交后自动退位。在发起C<sub>new</sub>后，要退出集群的leader就会处在操纵一个不包含它本身的raft集群的状态下。这时它可以发送C<sub>new</sub>日志，但是日志计数时不计自身。
3. 为了避免下线的节点超时选举而影响集群运行，服务器会在它确信集群中有leader存在时拒绝Request Vote RPC.因为C<sub>new</sub>的新leader不会再发送心跳给要退出的节点，如果这些节点没有及时下线，它们会超时增加任期号后发送Request Vote RPC。虽然它们不可能当选leader,但会导致raft集群进入投票选举阶段，影响集群的正常运行。为了解决这个问题，Raft在Request Vote RPC上补充了一个规则：一个节点如果在最小超时时间之内收到了Request Vote RPC,那么它会拒绝此RPC。这样，只要follower:连续收到leader的心跳，那么退出集群节点的Request Vote RPC就不会影响到 raft集群的正常运行了。


## 深入理解
#### Raft共识算法的三个主要特性：
1. 共识算法可以保证在任何非拜占庭情况下的正确性。所谓拜占庭情况就是节点发送错误的命令或使坏。
2. 共识算法可以保证在大多数机器正常的情况下集群的高可用性，而少部分的机器缓慢不影响整个集群的性能。
3. 不依赖外部时间来保证日志的一致性。

#### Raft区别于其他共识算法的三个特征：
1. Strong Leader：