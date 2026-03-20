# Paxos 算法详解

Paxos 算法是莱斯利·兰伯特（Leslie Lamport）于 1990 年提出的一种基于消息传递且具有高度容错特性的**共识（Consensus）算法**。它是分布式系统中最重要的算法之一，主要用于解决分布式环境下的数据一致性问题。

## 1. 核心角色

在 Paxos 算法中，主要有以下三种角色（一个节点可以同时充当多个角色）：
*   **Proposer（提议者）**：提出议案（Proposal）。包含提案编号（Proposal ID）和提议的值（Value）。
*   **Acceptor（接受者）**：参与决策，回应 Proposer 的提案。当收到提案后，可以接受（Accept）或拒绝（Reject）。只有半数以上的 Acceptor 接受了某个提案，该提案才算通过。
*   **Learner（学习者）**：不参与投票，只获取最终通过的提案结果。

## 2. 算法原理与执行流程

Paxos 的核心原理是通过两阶段提交（Two-Phase Commit, 2PC）的变种来达成共识。提案（Proposal）由两部分组成：`[N, V]`，其中 `N` 是全局唯一的递增提案编号，`V` 是提案的内容（值）。

标准的 Basic Paxos 将整个决议过程分为两个阶段：**Prepare 阶段** 和 **Accept 阶段**。

### 阶段一：Prepare 阶段（准备阶段）
1. **发出 Prepare 请求**：
   Proposer 选择一个递增的提案编号 `N`，向超过半数（多数派 `Quorum`）的 Acceptor 发送 `Prepare(N)` 请求。
2. **Acceptor 的承诺（Promise）**：
   当一个 Acceptor 收到 `Prepare(N)` 请求时，它会对比自己之前见过的最大提案编号 `MAX_N`：
   * 如果 `N > MAX_N`，Acceptor 承诺不再接受任何编号小于 `N` 的提案，并向 Proposer 回复自己过去曾经接受过的最大编号的提案 `[Accepted_N, Accepted_V]`（如果有的话）。随后更新自己的 `MAX_N = N`。
   * 如果 `N <= MAX_N`，说明已经有更新的提案在进行了，Acceptor 可以直接忽略该请求或回复拒绝（Reject）。

### 阶段二：Accept 阶段（接受阶段）
1. **发出 Accept 请求**：
   如果 Proposer 收到了多数派 Acceptor 对 `Prepare(N)` 的响应（Promise），它就会向这些 Acceptor 发送真正的**接受请求 `Accept(N, V)`**。
   * 关于 `V` 的抉择：如果 Acceptor 在 Prepare 阶段回复了之前接受过的决议 `[Accepted_N, Accepted_V]`，Proposer 会从中选出编号 `Accepted_N` 最大的那个 `Accepted_V` 作为本次要提交的值 `V`。如果所有 Acceptor 都没有回复之前的 `V`，则 Proposer 可以自己任意决定一个值作为 `V`。
2. **Acceptor 的接受（Accept）**：
   当 Acceptor 收到 `Accept(N, V)` 请求时，只要它在此期间**没有收到过任何编号大于 `N` 的 Prepare 请求**，它就会接受这个提案，记录下来，并向 Proposer 和 Learner 发送确认消息。

### 阶段三：Learn 阶段（学习阶段）
当多数派的 Acceptor 都接受了某个提案 `[N, V]`，该提案即告通过。Learner 获取该结果并同步给系统其他部分。

## 3. 常见疑问解析：关于编号与“舍弃自己的值”

**疑问一：提案编号（N）是如何设定的？**
为了保证全局唯一且单调递增，提案编号 `N` 通常由两部分组成：`[本地自增序列号, Proposer_ID]`。当 Proposer 的提议因为编号太小被拒绝时，它会提取 Acceptor 返回的最大编号，将自己的本地序号递增到比它更大，然后再发起新一轮的 Prepare 请求。这保证了即使多个节点同时发起请求，也能通过较量编号大小分出胜负。

**疑问二：如果 Proposer 提议时发现有旧值并舍弃了自己的值，那分布式系统的数据是不是就不会更新了？**
这是理解 Basic Paxos 和 Multi-Paxos 区别的**最大关键点**：
*   **Basic Paxos 的目标是一次性的**：它只负责为一个**单一的槽位（Slot）或单一的日志条目**，决定一个**一旦通过就永远不能改变的值**。
*   如果 Proposer 发现这个槽位已经被别人（历史前任）捷足先登了，为了不破坏系统一致性，它必须妥协，帮助系统把前任的值彻底确认下来。
*   **真正的数据更新机制是追加（Append）**：在实际系统中（即 Multi-Paxos），数据是以日志（LogArray）形式存在的。如果 Proposer A 发现“日志序号 1”已经被别的值占据，它妥协确认后，会立马发起一轮**全新的 Paxos 实例**，针对新的槽位“日志序号 2”提出自己原来想要的那个值。系统的状态就是通过这种不断追加新槽位的方式向前推进（更新）的。

## 4. 活锁问题（Livelock）
标准的 Basic Paxos 存在活锁（Livelock）问题：两个 Proposer 互相竞争，交替提出更大的提案编号，导致双方都只能完成 Prepare 阶段，而无法在 Accept 阶段达成多数派同意。
*   **解决方式**：通常通过选举一个唯一的**主提议者（Leader/Distinguished Proposer）**来解决。所有的提案都通过 Leader 提出，这就演变成了 Multi-Paxos 协议。

## 5. 总结
Paxos 的精髓在于**“少数服从多数”**，即 Quorum 机制。只要集群中有 `2F+1` 个节点，即使有 `F` 个节点宕机，网络中仍有 `F+1` 个节点存活，系统就可以正常选举并达成共识。这是绝大多数高可用分布式系统（如 ZooKeeper 的 ZAB、Raft 协议等）的容错基石。