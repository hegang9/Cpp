## MySQL 命令介绍

SQL（Structured Query Language，结构化查询语言）是操作关系型数据库的通用语言。在 MySQL 中，按照功能的不同，通常将 SQL 命令划分为以下五大类：

1. **DDL（Data Definition Language）数据定义语言**：用于定义或修改数据库的结构（库、表、视图、索引等）。
2. **DML（Data Manipulation Language）数据操作语言**：用于对表中的数据进行增、删、改。
3. **DQL（Data Query Language）数据查询语言**：用于查询表中的记录（日常开发使用最频繁）。
4. **DCL（Data Control Language）数据控制语言**：用于定义数据库的访问权限和安全级别。
5. **TCL（Transaction Control Language）事务控制语言**：用于管理数据库的事务。

---

### 一、 DDL (数据定义语言)
DDL 主要用于管理数据库和表的结构。**注意：DDL 操作会自动提交，且无法回滚。**

| 命令 | 功能说明 |
| :--- | :--- |
| `CREATE` | 创建数据库或表等对象 |
| `DROP` | 删除数据库或表等对象 |
| `ALTER` | 修改现有的数据库结构或表结构 |
| `TRUNCATE` | 清空表中的所有数据，但保留表结构，重置自增主键 |

#### 1. 数据库级别操作
```sql
-- 创建数据库
CREATE DATABASE [IF NOT EXISTS] db_name [DEFAULT CHARSET utf8mb4];
-- 示例：
CREATE DATABASE IF NOT EXISTS my_test_db DEFAULT CHARSET utf8mb4;

-- 删除数据库
DROP DATABASE [IF EXISTS] db_name;

-- 切换/进入数据库
USE db_name;

-- 查看所有数据库
SHOW DATABASES;
```

#### 2. 表级别操作
```sql
-- 创建表格式
CREATE TABLE [IF NOT EXISTS] table_name (
    column1 datatype constraints,
    column2 datatype constraints,
    ...
);
-- 示例：创造一个用户表
CREATE TABLE IF NOT EXISTS users (
    id INT AUTO_INCREMENT PRIMARY KEY COMMENT '主键',
    username VARCHAR(50) NOT NULL UNIQUE COMMENT '用户名',
    age INT DEFAULT 18 COMMENT '年龄',
    create_time DATETIME DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间'
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- 删除表
DROP TABLE [IF EXISTS] table_name;

-- 清空表数据（比 DELETE 跑得快，且重置自增 ID，不可回滚）
TRUNCATE TABLE table_name;

-- 查看表结构详情
DESC table_name;
SHOW CREATE TABLE table_name;

-- ================ 修改表结构 (ALTER) ================
-- 添加列
ALTER TABLE users ADD email VARCHAR(100);

-- 修改列的类型或属性
ALTER TABLE users MODIFY age TINYINT DEFAULT 20;

-- 改变列名和它的类型
ALTER TABLE users CHANGE username user_name VARCHAR(60);

-- 删除列
ALTER TABLE users DROP email;

-- 表重命名
ALTER TABLE users RENAME TO system_users;
```

---

### 二、 DML (数据操作语言)
DML 用于对表中的原始数据进行**增、删、改**操作。

| 命令 | 功能说明 |
| :--- | :--- |
| `INSERT` | 向表中插入一条或多条数据 |
| `UPDATE` | 更新表中的某条或全部数据 |
| `DELETE` | 删除表中的某条或全部数据 |

#### 1. INSERT (插入数据)
```sql
-- 全字段插入（插入的数据顺序需和表结构字段定义完全一致）
INSERT INTO table_name VALUES (value1, value2, value3, ...);
-- 示例：
INSERT INTO users VALUES (1, 'Alice', 25, '2023-01-01');

-- 指定字段插入（推荐做法，未指定的字段如果允许将使用默认值或 NULL）
INSERT INTO table_name (column1, column2) VALUES (value1, value2);
-- 示例：
INSERT INTO users (username, age) VALUES ('Bob', 22);

-- 批量插入多条数据
INSERT INTO users (username, age) VALUES 
('Charlie', 24),
('Dave', 28);
```

#### 2. UPDATE (修改数据)
> ⚠️ 切记：使用 UPDATE 时尽量加上 WHERE 条件进行限制，否则会更改全表的所有行数据！

```sql
-- 格式
UPDATE table_name SET column1 = value1, column2 = value2 [WHERE condition];

-- 示例：将 Bob 的年龄改为 23
UPDATE users SET age = 23 WHERE username = 'Bob';

-- 示例：全体用户年龄加 1（无 WHERE 条件）
UPDATE users SET age = age + 1; 
```

#### 3. DELETE (删除数据)
> ⚠️ 切记：使用 DELETE 时也要配合 WHERE 条件，否则会清空全表数据！

```sql
-- 格式
DELETE FROM table_name [WHERE condition];

-- 示例：删除名为 Alice 的用户
DELETE FROM users WHERE username = 'Alice';

-- 示例：删除全表数据（可被事务回滚，并且不重置自增列(AUTO_INCREMENT)）
DELETE FROM users; 
```

---

### 三、 DQL (数据查询语言)
DQL 是使用最多、最复杂的语言，用于从数据库中检索你想要的数据。

| 关键字 | 功能说明 |
| :--- | :--- |
| `SELECT` | 投影：指定要查询的列 |
| `FROM` | 指定要查询的数据表，并支持多表连接 (`JOIN`) |
| `WHERE` | 过滤：提取满足特定条件的记录 (分组前) |
| `GROUP BY` | 分组：将查询结果按某列或多列进行分组聚合 |
| `HAVING` | 分组过滤：针对分组后产生的结果集进行条件筛选 |
| `ORDER BY` | 排序：对结果排序（`ASC` 升序，`DESC` 降序） |
| `LIMIT` | 限制：限制返回集合的记录条数（常搭配用于分页） |

**💡 标准 DQL 代码的书写和执行顺序：** 
`FROM` -> `WHERE` -> `GROUP BY` -> `HAVING` -> `SELECT` -> `ORDER BY` -> `LIMIT`

#### 1. 基础查询与条件过滤
```sql
-- 查询全表
SELECT * FROM users;

-- 查询指定列，并给列起个别名 (AS 关键字通常可以省略)
SELECT username AS 姓名, age AS 年龄 FROM users;

-- 去重查询 (去除重复出现的 age 值)
SELECT DISTINCT age FROM users;

-- WHERE 条件查询 
-- (支持操作符: =, >, <, >=, <=, !=, <>, BETWEEN AND, IN, LIKE, IS NULL, AND, OR)
SELECT * FROM users WHERE age BETWEEN 20 AND 30;           -- 在20到30之间
SELECT * FROM users WHERE age IN (20, 25, 28);             -- 年龄是20,25或28的
SELECT * FROM users WHERE username LIKE 'A%';              -- 模糊查询，查出以A开头的
SELECT * FROM users WHERE username LIKE '_lice';           -- _代表匹配单一字符
SELECT * FROM users WHERE email IS NOT NULL AND age > 18;  -- 多条件联合
```

#### 2. 聚合查询与分组
常用的聚合函数包括：`COUNT()`记录数, `MAX()`最大值, `MIN()`最小值, `SUM()`求和, `AVG()`平均值。
```sql
-- 统计表中的总行数
SELECT COUNT(*) FROM users;

-- 分组查询：按年龄分组，分别统计每个年龄段有多少人
SELECT age, COUNT(*) AS numbers FROM users GROUP BY age;

-- HAVING 过滤：在上面分组的基础上，找出人数超过 2 人的年龄段
SELECT age, COUNT(*) AS numbers FROM users GROUP BY age HAVING numbers > 2;
```

#### 3. 排序与分页
```sql
-- 排序：按照年龄倒序(大到小)排列，若年龄相同，则再按 ID 升序排列
SELECT * FROM users ORDER BY age DESC, id ASC;

-- 分页：LIMIT offset, size (从第 offset 位置开始，向后取 size 行数据)
-- offset 从0开始计数，size 是每页显示的记录数，offset计算公式：offset = (page_number - 1) * page_size
-- 示例：获取第1页数据，每页显示10条 (offset从0开始)
SELECT * FROM users LIMIT 0, 10;
-- 示例：获取第2页数据
SELECT * FROM users LIMIT 10, 10;
```

#### 4. 多表联合查询 (Joins)
```sql
-- 内连接 (交集)：查找两张表中互相都能匹配上的记录
SELECT u.username, o.order_id 
FROM users u 
INNER JOIN orders o ON u.id = o.user_id;

-- 左外连接 (左偏)：以左表为基准，包含左表所有数据，如果右表没有匹配的值，则填充为空 (NULL)
SELECT u.username, o.order_id 
FROM users u 
LEFT JOIN orders o ON u.id = o.user_id;
```

---

### 四、 DCL (数据控制语言)
DCL 主要用于管理数据库的访问权限控制，通常由 DBA 管理员操作。

| 命令 | 功能说明 |
| :--- | :--- |
| `GRANT` | 赋予账号特定的权限 |
| `REVOKE` | 撤销账号特定的权限 |

#### 1. 管理账号权限示例
```sql
-- 创建新用户 (允许在本地 localhost 环境登录)
CREATE USER 'dev_user'@'localhost' IDENTIFIED BY 'password123';
-- 允许任意远程环境登录
CREATE USER 'dev_user'@'%' IDENTIFIED BY 'password123'; 

-- 授予权限：给 dev_user 分配 my_test_db 库下所有表增删改查权限
GRANT SELECT, INSERT, UPDATE, DELETE ON my_test_db.* TO 'dev_user'@'localhost';

-- 授予全部最高权限 (*.* 代表所有的数据库的所有表)
GRANT ALL PRIVILEGES ON *.* TO 'dev_user'@'%';

-- 撤销删除权限
REVOKE DELETE ON my_test_db.* FROM 'dev_user'@'localhost';

-- 刷新权限 (使得以上相关的权限改动操作立即刻在内存生效)
FLUSH PRIVILEGES;

-- 删除指定用户
DROP USER 'dev_user'@'localhost';
```

---

### 五、 TCL (事务控制语言)
用于确保一系列的 DML 操作（增删改）要么全部成功，要么全部失败，用来满足数据库事务的 ACID 特性（主要针对支持事务的 InnoDB 引擎）。

| 命令 | 功能说明 |
| :--- | :--- |
| `START TRANSACTION` / `BEGIN` | 开启一个新事务 |
| `COMMIT` | 提交事务，使得 DML 带来的更改永久生效并固化入盘 |
| `ROLLBACK` | 回滚事务，取消这期间事务所做的所有 DML 更改 |
| `SAVEPOINT` | 设置一个保存点，可以让事务有余地地回滚到特定的位置 |

#### 1. 事务控制示例
```sql
-- 查看当前是否支持自动提交 (1为开启，0为临时关闭)
SELECT @@autocommit;
SET autocommit = 0; 

-- =============== 经典转账场景 ===============
BEGIN; -- 或者 START TRANSACTION;开启事务

-- 步骤1：Alice的账户扣除 100
UPDATE accounts SET balance = balance - 100 WHERE name = 'Alice';
-- 步骤2：Bob的账户增加 100
UPDATE accounts SET balance = balance + 100 WHERE name = 'Bob';

-- 情况A：如果上述两条更新语句都没报错，那么提交
COMMIT; 

-- 情况B：如果步骤2系统异常，需要撤销步骤1的操作，那么回滚
ROLLBACK; 
```
