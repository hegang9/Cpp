## MySQL 约束介绍

约束（Constraint）用于限制字段取值和表之间关系，核心作用是保证数据的**正确性**、**完整性**和**一致性**。

---

### 一、常见约束总览

| 约束名 | 作用 | 常见场景 | 关键特点 |
| :--- | :--- | :--- | :--- |
| `NOT NULL` | 字段不能为空 | 用户名、密码 | 不允许 `NULL` |
| `DEFAULT` | 设置默认值 | 状态、创建时间 | 插入未赋值时生效 |
| `UNIQUE` | 保证值唯一 | 手机号、邮箱 | 可建立唯一索引 |
| `PRIMARY KEY` | 唯一标识一行 | 用户 ID、订单 ID | 唯一且非空 |
| `AUTO_INCREMENT` | 自动递增 | 自增主键 | 常与整数主键配合 |
| `CHECK` | 限制取值范围 | 年龄、工资 | MySQL 8.0.16+ 才真正生效 |
| `FOREIGN KEY` | 维护表关联关系 | 订单-用户 | 防止孤儿数据 |

---

### 二、约束定义方式

| 方式 | 写法位置 | 适用场景 | 示例 |
| :--- | :--- | :--- | :--- |
| 列级约束 | 写在字段后面 | 单字段简单约束 | `username VARCHAR(50) NOT NULL` |
| 表级约束 | 写在字段定义后 | 联合约束、显式命名 | `CONSTRAINT uq_email UNIQUE(email)` |

**示例：**

```sql
CREATE TABLE users (
    id INT PRIMARY KEY AUTO_INCREMENT,
    username VARCHAR(50) NOT NULL,
    email VARCHAR(100),
    CONSTRAINT uq_users_email UNIQUE (email)
);
```

---

### 三、各类约束详解

| 约束 | 作用 | 示例写法 | 插入/使用示例 | 注意事项 |
| :--- | :--- | :--- | :--- | :--- |
| `NOT NULL` | 该字段不能为空 | `username VARCHAR(50) NOT NULL` | `INSERT INTO users(username) VALUES('Tom');` | `NULL` 与空字符串 `''` 不同 |
| `DEFAULT` | 未赋值时使用默认值 | `status INT DEFAULT 1` | `INSERT INTO users(username) VALUES('Tom');` | 仅在未显式赋值时生效 |
| `UNIQUE` | 字段值不能重复 | `phone VARCHAR(20) UNIQUE` | 重复手机号插入会失败 | 多个 `NULL` 在 MySQL 中通常允许 |
| `PRIMARY KEY` | 唯一标识一行记录 | `id INT PRIMARY KEY` | 适合作为行唯一编号 | 一张表只能有一个主键 |
| `AUTO_INCREMENT` | 自动生成递增值 | `id INT PRIMARY KEY AUTO_INCREMENT` | 插入时可不写 `id` | 一般只能有一个，且需为索引列 |
| `CHECK` | 限制字段值范围 | `age INT CHECK(age >= 18)` | 小于 18 的年龄插入失败 | 老版本 MySQL 可能不生效 |
| `FOREIGN KEY` | 保证引用记录存在 | `FOREIGN KEY(user_id) REFERENCES users(id)` | 子表中的 `user_id` 必须存在于父表 | 父表字段通常必须是主键或唯一键 |

---

### 四、重点约束示例

#### 1. `UNIQUE` 联合唯一约束

| 项目 | 说明 |
| :--- | :--- |
| 定义 | 多个字段组合起来后，整体不能重复 |
| 约束目标 | 不是限制单个字段唯一，而是限制“字段组合”唯一 |
| 典型场景 | 一个学生不能重复选同一门课；一个用户每天只能签到一次 |
| 常见写法 | `UNIQUE (col1, col2)` |
| 是否允许单列重复 | 允许，只要组合后整体不重复即可 |

| 场景 | SQL |
| :--- | :--- |
| 同一个学生不能重复选同一门课 | `CONSTRAINT uq_student_course UNIQUE (student_id, course_id)` |

```sql
CREATE TABLE student_course (
    id INT PRIMARY KEY AUTO_INCREMENT,
    student_id INT,
    course_id INT,
    CONSTRAINT uq_student_course UNIQUE (student_id, course_id)
);
```

**理解示例：**

| `student_id` | `course_id` | 是否允许插入 | 原因 |
| :--- | :--- | :--- | :--- |
| `1` | `101` | 是 | 首次出现 |
| `1` | `102` | 是 | 课程不同，组合不同 |
| `2` | `101` | 是 | 学生不同，组合不同 |
| `1` | `101` | 否 | 组合重复 |

**注意事项：**

| 要点 | 说明 |
| :--- | :--- |
| 单列不要求唯一 | `student_id` 可以重复，`course_id` 也可以重复 |
| 关注的是组合值 | 数据库判断的是 `(student_id, course_id)` 整体 |
| 常用于防重复业务 | 如“用户-角色”“学生-课程”“用户-日期” |

#### 2. `PRIMARY KEY` 联合主键

| 项目 | 说明 |
| :--- | :--- |
| 定义 | 使用多个字段共同作为主键 |
| 约束目标 | 多个字段组合后唯一标识一条记录 |
| 和联合唯一的区别 | 联合主键不仅唯一，而且所有字段都不能为 `NULL` |
| 典型场景 | 成绩表、关系表、中间映射表 |

| 场景 | SQL |
| :--- | :--- |
| 一条成绩由“学生 + 课程”唯一确定 | `PRIMARY KEY (student_id, course_id)` |

```sql
CREATE TABLE score (
    student_id INT,
    course_id INT,
    score INT,
    PRIMARY KEY (student_id, course_id)
);
```

**理解示例：**

| `student_id` | `course_id` | `score` | 是否允许插入 | 原因 |
| :--- | :--- | :--- | :--- | :--- |
| `1` | `101` | `90` | 是 | 首次出现 |
| `1` | `102` | `88` | 是 | 组合不同 |
| `1` | `101` | `95` | 否 | 主键组合重复 |
| `NULL` | `101` | `80` | 否 | 主键字段不能为 `NULL` |

**联合唯一 vs 联合主键：**

| 对比项 | 联合唯一约束 | 联合主键 |
| :--- | :--- | :--- |
| 是否唯一 | 是 | 是 |
| 是否允许 `NULL` | 通常允许 | 不允许 |
| 一张表能有几个 | 可以有多个 | 只能有一个 |
| 是否适合作为行标识 | 一般不作为主标识 | 适合作为主标识 |

#### 3. `FOREIGN KEY` 外键约束

| 项目 | 说明 |
| :--- | :--- |
| 定义 | 子表中的某个字段引用父表中的主键或唯一键 |
| 作用 | 保证引用关系有效，防止子表指向不存在的数据 |
| 父表 | 被引用的表 |
| 子表 | 保存外键字段、引用父表的表 |
| 典型场景 | 订单属于用户、评论属于文章、成绩属于学生 |

**父表 / 子表关系示意：**

| 表 | 角色 | 关键字段 |
| :--- | :--- | :--- |
| `users` | 父表 | `id` |
| `orders` | 子表 | `user_id` |

含义：`orders.user_id` 的值必须在 `users.id` 中真实存在。

```sql
CREATE TABLE users (
    id INT PRIMARY KEY AUTO_INCREMENT,
    username VARCHAR(50) NOT NULL
);

CREATE TABLE orders (
    id INT PRIMARY KEY AUTO_INCREMENT,
    order_no VARCHAR(50) NOT NULL,
    user_id INT,
    CONSTRAINT fk_orders_user FOREIGN KEY (user_id) REFERENCES users(id)
);
```

**外键生效后，数据库会帮你检查：**

| 操作 | 是否允许 | 原因 |
| :--- | :--- | :--- |
| 插入 `user_id = 1`，且 `users.id=1` 存在 | 允许 | 引用合法 |
| 插入 `user_id = 999`，但父表无该用户 | 不允许 | 引用了不存在的数据 |
| 删除父表用户，但子表仍有订单引用它 | 默认不允许 | 会破坏引用完整性 |

**外键使用前提：**

| 条件 | 说明 |
| :--- | :--- |
| 父表字段必须可被引用 | 通常是 `PRIMARY KEY` 或 `UNIQUE` 字段 |
| 父子字段类型要兼容 | 类型、长度、符号属性最好一致 |
| 存储引擎要支持外键 | 通常使用 `InnoDB` |

**外键适用场景：**

| 场景 | 是否适合 |
| :--- | :--- |
| 教学 / 小型系统 / 强一致业务 | 很适合 |
| 高并发核心链路 / 分库分表系统 | 通常谨慎使用 |
| 需要数据库强约束保证关系 | 很适合 |

**注意事项：**

| 要点 | 说明 |
| :--- | :--- |
| 优点 | 数据一致性强，数据库自动兜底 |
| 缺点 | 可能增加写操作复杂度，删除更新要更谨慎 |
| 生产实践 | 有些团队不用物理外键，而采用逻辑外键（业务代码保证） |

---

### 五、外键级联操作

| 选项 | 含义 | 典型效果 |
| :--- | :--- | :--- |
| `CASCADE` | 父表删除/更新时，子表同步删除/更新 | 删除用户时，订单也被删除 |
| `SET NULL` | 父表删除/更新时，子表外键设为 `NULL` | 删除用户后，订单 `user_id = NULL` |
| `RESTRICT` | 存在子表记录时，禁止父表删除/更新 | 有订单的用户不能删 |
| `NO ACTION` | 通常与 `RESTRICT` 类似 | 效果接近限制操作 |

**示例：**

```sql
CREATE TABLE orders (
    id INT PRIMARY KEY AUTO_INCREMENT,
    order_no VARCHAR(50) NOT NULL,
    user_id INT,
    CONSTRAINT fk_orders_user FOREIGN KEY (user_id)
        REFERENCES users(id)
        ON DELETE CASCADE
        ON UPDATE CASCADE
);
```

---

### 六、综合建表示例

```sql
CREATE TABLE users (
    id INT PRIMARY KEY AUTO_INCREMENT COMMENT '用户ID',
    username VARCHAR(50) NOT NULL UNIQUE COMMENT '用户名',
    email VARCHAR(100) NOT NULL UNIQUE COMMENT '邮箱',
    age INT DEFAULT 18 CHECK (age >= 0),
    status TINYINT NOT NULL DEFAULT 1 COMMENT '状态：1正常 0禁用',
    create_time DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间'
);
```

| 字段 | 使用到的约束 |
| :--- | :--- |
| `id` | `PRIMARY KEY` + `AUTO_INCREMENT` |
| `username` | `NOT NULL` + `UNIQUE` |
| `email` | `NOT NULL` + `UNIQUE` |
| `age` | `DEFAULT` + `CHECK` |
| `status` | `NOT NULL` + `DEFAULT` |
| `create_time` | `NOT NULL` + `DEFAULT` |

---

### 七、修改约束常用语句

| 操作 | SQL |
| :--- | :--- |
| 添加 `NOT NULL` | `ALTER TABLE users MODIFY username VARCHAR(50) NOT NULL;` |
| 添加默认值 | `ALTER TABLE users MODIFY status INT DEFAULT 1;` |
| 添加唯一约束 | `ALTER TABLE users ADD CONSTRAINT uq_users_email UNIQUE (email);` |
| 添加主键 | `ALTER TABLE users ADD CONSTRAINT pk_users PRIMARY KEY (id);` |
| 添加外键 | `ALTER TABLE orders ADD CONSTRAINT fk_orders_user FOREIGN KEY (user_id) REFERENCES users(id);` |
| 删除唯一约束 | `ALTER TABLE users DROP INDEX uq_users_email;` |
| 删除主键 | `ALTER TABLE users DROP PRIMARY KEY;` |
| 删除外键 | `ALTER TABLE orders DROP FOREIGN KEY fk_orders_user;` |

---

### 八、约束与索引的区别

| 对比项 | 约束 | 索引 |
| :--- | :--- | :--- |
| 主要目标 | 保证数据合法性 | 提高查询效率 |
| 是否强制规则 | 是 | 否 |
| 是否限制重复值 | 某些约束会 | 普通索引不会 |
| 与索引关系 | `PRIMARY KEY`、`UNIQUE` 常自动建索引 | 索引本身不等于约束 |

---

### 九、实战建议

| 建议 | 说明 |
| :--- | :--- |
| 关键字段加 `NOT NULL` | 如用户名、订单号、状态、创建时间 |
| 主键尽量简单稳定 | 常用整型自增主键 |
| 唯一字段提前规划 | 如账号、邮箱、手机号 |
| 谨慎使用级联删除 | 防止误删大量关联数据 |
| 老版本谨慎依赖 `CHECK` | 可能写了也不生效 |
| 数据库约束 + 业务校验并存 | 约束是底线，不替代业务判断 |

---

### 十、总结速记

| 约束 | 一句话理解 |
| :--- | :--- |
| `NOT NULL` | 不能为空 |
| `DEFAULT` | 不写就给默认值 |
| `UNIQUE` | 不能重复 |
| `PRIMARY KEY` | 唯一且非空的主标识 |
| `AUTO_INCREMENT` | 自动递增编号 |
| `CHECK` | 必须满足条件 |
| `FOREIGN KEY` | 必须引用已存在的数据 |
