## MySQL 常用函数介绍

MySQL 提供了大量内置函数，可以直接在 SQL 中完成字符串处理、数值计算、日期运算、空值处理、聚合统计、窗口分析以及 JSON 数据操作。熟练掌握这些函数，可以显著减少业务层处理逻辑。

按照功能划分，常用函数通常分为以下几类：**字符串函数**、**数值函数**、**日期时间函数**、**流程控制函数**、**聚合函数**、**窗口函数**、**JSON 函数**、**系统信息函数**。

---

### 一、字符串函数

字符串函数主要用于拼接、截取、替换、查找和格式化字符串。

| 函数名 | 功能说明 | 示例代码 |
| :--- | :--- | :--- |
| `CONCAT(S1, S2, ...)` | 拼接字符串，若任一参数为 `NULL`，结果通常为 `NULL` | `SELECT CONCAT('Hello', ' ', 'MySQL');` -> `'Hello MySQL'` |
| `CONCAT_WS(sep, S1, S2, ...)` | 使用指定分隔符拼接字符串，会忽略 `NULL` 值 | `SELECT CONCAT_WS('-', '2026', '03', '10');` -> `'2026-03-10'` |
| `UPPER(str)` / `LOWER(str)` | 转为大写 / 小写 | `SELECT UPPER('mysql');` -> `'MYSQL'` |
| `LENGTH(str)` | 返回字符串的字节长度 | `SELECT LENGTH('数据库');` -> `9` |
| `CHAR_LENGTH(str)` | 返回字符串的字符长度 | `SELECT CHAR_LENGTH('数据库');` -> `3` |
| `SUBSTRING(str, pos, len)` | 从指定位置开始截取子串，位置从 `1` 开始 | `SELECT SUBSTRING('Hello MySQL', 7, 5);` -> `'MySQL'` |
| `LEFT(str, len)` / `RIGHT(str, len)` | 从左侧 / 右侧截取指定长度的子串 | `SELECT LEFT('MySQL', 2);` -> `'My'` |
| `REPLACE(str, from_str, to_str)` | 替换子串 | `SELECT REPLACE('www.mysql.com', 'mysql', 'mariadb');` |
| `TRIM(str)` | 去除首尾空格 | `SELECT TRIM('  hello  ');` -> `'hello'` |
| `LPAD(str, len, padstr)` / `RPAD(str, len, padstr)` | 左填充 / 右填充到指定长度 | `SELECT LPAD('123', 5, '0');` -> `'00123'` |
| `INSTR(str, substr)` | 返回子串首次出现的位置，不存在返回 `0` | `SELECT INSTR('Hello', 'l');` -> `3` |
| `LOCATE(substr, str)` | 查找子串位置，作用类似 `INSTR` | `SELECT LOCATE('sql', 'mysqlsql');` -> `6` |
| `FIND_IN_SET(str, strlist)` | 查找某值在逗号分隔字符串中的位置 | `SELECT FIND_IN_SET('b', 'a,b,c');` -> `2` |
| `REVERSE(str)` | 反转字符串 | `SELECT REVERSE('abc');` -> `'cba'` |
| `ASCII(str)` | 返回字符串首字符的 ASCII 码 | `SELECT ASCII('A');` -> `65` |

**⚠️ 注意事项：**
* MySQL 中很多字符串函数的位置索引从 **1** 开始，不是从 `0` 开始。
* `LENGTH()` 返回的是字节数，中文在 `utf8mb4` 下通常占多个字节；统计字符个数建议使用 `CHAR_LENGTH()`。
* `FIND_IN_SET()` 适合处理逗号分隔字段的精确匹配，但从设计上看，更推荐使用规范的关联表而不是逗号拼接字段。

---

### 二、数值函数

数值函数主要用于四舍五入、取整、绝对值、随机数、幂运算等场景。

| 函数名 | 功能说明 | 示例代码 |
| :--- | :--- | :--- |
| `ROUND(x, d)` | 四舍五入保留 `d` 位小数 | `SELECT ROUND(3.1415, 2);` -> `3.14` |
| `CEIL(x)` / `CEILING(x)` | 向上取整 | `SELECT CEIL(3.14);` -> `4` |
| `FLOOR(x)` | 向下取整 | `SELECT FLOOR(3.99);` -> `3` |
| `TRUNCATE(x, d)` | 直接截断到 `d` 位小数，不四舍五入 | `SELECT TRUNCATE(3.1415, 2);` -> `3.14` |
| `MOD(n, m)` | 求余数 | `SELECT MOD(10, 3);` -> `1` |
| `ABS(x)` | 求绝对值 | `SELECT ABS(-10.5);` -> `10.5` |
| `SIGN(x)` | 返回数值符号：正数 `1`，负数 `-1`，零为 `0` | `SELECT SIGN(-15);` -> `-1` |
| `RAND()` | 返回 `0` 到 `1` 之间的随机浮点数 | `SELECT RAND();` |
| `POW(x, y)` / `POWER(x, y)` | 返回 `x` 的 `y` 次方 | `SELECT POW(2, 3);` -> `8` |
| `SQRT(x)` | 返回平方根 | `SELECT SQRT(16);` -> `4` |
| `GREATEST(v1, v2, ...)` | 返回多个值中的最大值 | `SELECT GREATEST(3, 9, 5);` -> `9` |
| `LEAST(v1, v2, ...)` | 返回多个值中的最小值 | `SELECT LEAST(3, 9, 5);` -> `3` |

**💡 实用技巧：生成随机整数**
如果想生成一个 `[a, b]` 范围内的随机整数，可以使用公式：`FLOOR(RAND() * (b - a + 1) + a)`。

例如生成 `[10, 100]` 范围内的随机整数：
```sql
SELECT FLOOR(RAND() * (100 - 10 + 1) + 10);
```

**⚠️ 注意事项：**
* `ROUND()` 是四舍五入，`TRUNCATE()` 是直接截断，两者含义不同。
* `RAND()` 每执行一次通常都会重新生成随机值，因此在 `ORDER BY RAND()` 大表场景下性能较差。

---

### 三、日期和时间函数

日期时间函数在报表统计、日志分析、定时任务和业务有效期计算中使用非常频繁。

| 函数名 | 功能说明 | 示例代码 |
| :--- | :--- | :--- |
| `NOW()` | 返回当前日期和时间 | `SELECT NOW();` |
| `CURDATE()` / `CURRENT_DATE()` | 返回当前日期 | `SELECT CURDATE();` |
| `CURTIME()` / `CURRENT_TIME()` | 返回当前时间 | `SELECT CURTIME();` |
| `YEAR(date)` / `MONTH(date)` / `DAY(date)` | 提取年 / 月 / 日 | `SELECT YEAR('2023-10-01');` -> `2023` |
| `HOUR(time)` / `MINUTE(time)` / `SECOND(time)` | 提取时 / 分 / 秒 | `SELECT HOUR('12:30:45');` -> `12` |
| `DATE(date_time)` | 提取日期部分 | `SELECT DATE(NOW());` |
| `TIME(date_time)` | 提取时间部分 | `SELECT TIME(NOW());` |
| `DATE_ADD(date, INTERVAL expr unit)` | 日期加上时间间隔 | `SELECT DATE_ADD(NOW(), INTERVAL 1 DAY);` |
| `DATE_SUB(date, INTERVAL expr unit)` | 日期减去时间间隔 | `SELECT DATE_SUB(NOW(), INTERVAL 1 MONTH);` |
| `DATEDIFF(date1, date2)` | 返回两个日期相差的天数 | `SELECT DATEDIFF('2023-10-10', '2023-10-01');` -> `9` |
| `TIMESTAMPADD(unit, interval, datetime_expr)` | 按指定单位增加时间 | `SELECT TIMESTAMPADD(MINUTE, 30, NOW());` |
| `TIMESTAMPDIFF(unit, datetime_expr1, datetime_expr2)` | 按指定单位计算时间差 | `SELECT TIMESTAMPDIFF(HOUR, '2023-01-01 10:00:00', '2023-01-01 12:30:00');` -> `2` |
| `DATE_FORMAT(date, format)` | 按指定格式格式化日期 | `SELECT DATE_FORMAT(NOW(), '%Y-%m-%d %H:%i:%s');` |
| `STR_TO_DATE(str, format)` | 按格式将字符串转为日期时间 | `SELECT STR_TO_DATE('2023-10-01', '%Y-%m-%d');` |
| `UNIX_TIMESTAMP()` | 返回当前 Unix 时间戳 | `SELECT UNIX_TIMESTAMP();` |
| `FROM_UNIXTIME(ts)` | 将 Unix 时间戳转成日期时间 | `SELECT FROM_UNIXTIME(1696118400);` |
| `LAST_DAY(date)` | 返回指定日期所在月的最后一天 | `SELECT LAST_DAY('2023-02-05');` -> `2023-02-28` |
| `EXTRACT(unit FROM date)` | 提取指定日期部分 | `SELECT EXTRACT(YEAR FROM NOW());` |
| `WEEK(date)` | 返回该日期是当年的第几周 | `SELECT WEEK(NOW());` |
| `DAYOFWEEK(date)` | 返回星期几，星期日通常为 `1` | `SELECT DAYOFWEEK(NOW());` |

**⚠️ 注意事项：**
* `DATEDIFF()` 只比较日期部分，不比较时分秒。
* `TIMESTAMPDIFF()` 更适合计算小时、分钟、秒等精细时间差。
* `DATE_FORMAT()` 中常见格式符：`%Y` 年、`%m` 月、`%d` 日、`%H` 时、`%i` 分、`%s` 秒。

---

### 四、流程控制函数

流程控制函数用于在 SQL 中实现条件判断、空值兜底和多分支逻辑。

| 函数名 | 功能说明 | 示例代码 |
| :--- | :--- | :--- |
| `IF(expr, v1, v2)` | 条件成立返回 `v1`，否则返回 `v2` | `SELECT IF(80 >= 60, '及格', '不及格');` |
| `IFNULL(v1, v2)` | `v1` 不为 `NULL` 返回 `v1`，否则返回 `v2` | `SELECT IFNULL(NULL, 0);` -> `0` |
| `NULLIF(expr1, expr2)` | 若两值相等返回 `NULL`，否则返回 `expr1` | `SELECT NULLIF(1, 1);` -> `NULL` |
| `COALESCE(v1, v2, ...)` | 返回第一个非 `NULL` 的值 | `SELECT COALESCE(NULL, NULL, 'default');` |
| `CASE WHEN ... THEN ... ELSE ... END` | 多条件分支判断 | 见下方示例 |

#### 1. 流程控制示例
```sql
-- IF()：判断成绩是否及格
SELECT user_name, score, IF(score >= 60, '及格', '不及格') AS result
FROM exam_results;

-- IFNULL()：将空补助按 0 处理
SELECT user_name, IFNULL(bonus, 0) AS real_bonus
FROM employees;

-- CASE WHEN：按工资区间分类
SELECT employee_name, salary,
       CASE
           WHEN salary < 5000 THEN '低收入'
           WHEN salary >= 5000 AND salary < 10000 THEN '中收入'
           ELSE '高收入'
       END AS salary_level
FROM employees;
```

**⚠️ 注意事项：**
* `IFNULL()` 只处理两个参数，多个候选值应优先使用 `COALESCE()`。
* `CASE WHEN` 可读性通常优于多层嵌套 `IF()`。

---

### 五、聚合函数

聚合函数通常配合 `GROUP BY` 使用，用于将多行数据计算成一行结果。

| 函数名 | 功能说明 | 示例代码 |
| :--- | :--- | :--- |
| `COUNT(*)` / `COUNT(col)` | 统计记录数 | `SELECT COUNT(*) FROM users;` |
| `SUM(col)` | 求和 | `SELECT SUM(salary) FROM employees;` |
| `AVG(col)` | 求平均值 | `SELECT AVG(score) FROM exam_results;` |
| `MAX(col)` | 求最大值 | `SELECT MAX(age) FROM users;` |
| `MIN(col)` | 求最小值 | `SELECT MIN(price) FROM products;` |
| `GROUP_CONCAT(col SEPARATOR ',')` | 将多行值拼接成一个字符串 | `SELECT dept_id, GROUP_CONCAT(name SEPARATOR ',') FROM employees GROUP BY dept_id;` |

**⚠️ 注意事项：**
* 大多数聚合函数都会忽略 `NULL` 值，`COUNT(*)` 统计的是整行记录。
* `GROUP_CONCAT()` 结果长度受系统变量限制，必要时可调整 `group_concat_max_len`。
* 聚合函数通常不能直接嵌套，例如 `MAX(AVG(score))` 需要借助子查询实现。

---

### 六、窗口函数（MySQL 8.0+）

窗口函数可以在不合并结果集的前提下，对多行数据进行排名、累计和前后行对比分析。

| 函数名 | 功能说明 | 示例 |
| :--- | :--- | :--- |
| `ROW_NUMBER() OVER(...)` | 为每一行分配唯一连续编号 | `1, 2, 3` |
| `RANK() OVER(...)` | 相同值并列排名，后续名次跳号 | `1, 1, 3` |
| `DENSE_RANK() OVER(...)` | 相同值并列排名，后续名次不跳号 | `1, 1, 2` |
| `LEAD(col, n) OVER(...)` | 取当前行后第 `n` 行的值 | 对比后一天数据 |
| `LAG(col, n) OVER(...)` | 取当前行前第 `n` 行的值 | 对比前一天数据 |
| `SUM(col) OVER(...)` | 做累计和、分区和 | 计算累计销售额 |

#### 1. 窗口函数示例
```sql
-- 查询每个班级成绩前 3 名
SELECT *
FROM (
    SELECT student_name,
           class_id,
           score,
           DENSE_RANK() OVER (PARTITION BY class_id ORDER BY score DESC) AS ranking
    FROM scores
) AS temp_t
WHERE ranking <= 3;
```

**⚠️ 注意事项：**
* 窗口函数需要 MySQL `8.0+` 支持。
* `PARTITION BY` 表示分组范围，`ORDER BY` 表示窗口内排序规则。

---

### 七、JSON 处理函数（MySQL 5.7+）

MySQL 从 `5.7` 开始支持 JSON 类型，可用于保存结构灵活的扩展信息。

| 函数名 | 功能说明 | 示例代码 |
| :--- | :--- | :--- |
| `JSON_EXTRACT(json_doc, path)` | 从 JSON 中提取字段 | `SELECT JSON_EXTRACT('{"id":1,"name":"Tom"}', '$.name');` |
| `->` / `->>` | JSON 提取简写，`->>` 常返回去引号后的值 | `SELECT json_col->>'$.name' FROM user_profile;` |
| `JSON_SET(json_doc, path, val)` | 新增或更新 JSON 字段 | `SELECT JSON_SET('{"a":1}', '$.b', 2);` |
| `JSON_REPLACE(json_doc, path, val)` | 仅更新已存在字段 | `SELECT JSON_REPLACE('{"a":1}', '$.a', 10);` |
| `JSON_ARRAY(...)` | 构造 JSON 数组 | `SELECT JSON_ARRAY(1, 'abc', NULL);` |
| `JSON_OBJECT(...)` | 构造 JSON 对象 | `SELECT JSON_OBJECT('id', 1, 'name', 'Tom');` |
| `JSON_CONTAINS(target, candidate)` | 判断目标 JSON 是否包含指定内容 | `SELECT JSON_CONTAINS('[1,2,3]', '2');` |
| `JSON_LENGTH(json_doc)` | 返回 JSON 元素个数 | `SELECT JSON_LENGTH('[1,2,3]');` -> `3` |

**⚠️ 注意事项：**
* JSON 字段适合存储结构变化频繁的扩展信息，但不适合滥用替代结构化表设计。
* 高频筛选的 JSON 字段建议配合生成列和索引一起使用，否则查询性能可能较差。

---

### 八、系统信息与其他常用函数

这类函数常用于调试、数据校验、环境确认和简单加密场景。

| 函数名 | 功能说明 | 示例代码 |
| :--- | :--- | :--- |
| `VERSION()` | 获取 MySQL 版本 | `SELECT VERSION();` |
| `USER()` | 获取当前连接用户 | `SELECT USER();` |
| `CURRENT_USER()` | 获取当前认证用户 | `SELECT CURRENT_USER();` |
| `DATABASE()` | 获取当前数据库名称 | `SELECT DATABASE();` |
| `CONNECTION_ID()` | 获取当前连接 ID | `SELECT CONNECTION_ID();` |
| `MD5(str)` | 计算 MD5 哈希 | `SELECT MD5('password');` |
| `SHA1(str)` | 计算 SHA1 哈希 | `SELECT SHA1('password');` |
| `SHA2(str, len)` | 计算指定长度的 SHA2 哈希 | `SELECT SHA2('password', 256);` |

**⚠️ 注意事项：**
* `USER()` 与 `CURRENT_USER()` 的含义不完全相同，前者偏向连接身份，后者偏向认证身份。
* `MD5()`、`SHA1()` 适合做简单摘要演示，不建议直接用于高安全性的密码存储。

---

### 九、学习建议

在日常开发中，最常用、最建议优先掌握的是以下函数：

1. **字符串函数**：`CONCAT`、`SUBSTRING`、`REPLACE`、`TRIM`
2. **数值函数**：`ROUND`、`CEIL`、`FLOOR`、`ABS`
3. **日期函数**：`NOW`、`DATE_FORMAT`、`DATE_ADD`、`TIMESTAMPDIFF`
4. **空值处理**：`IFNULL`、`COALESCE`
5. **聚合函数**：`COUNT`、`SUM`、`AVG`、`GROUP_CONCAT`
6. **进阶分析**：`ROW_NUMBER`、`RANK`、`DENSE_RANK`

如果只是做常规业务开发，先熟练掌握前五类函数就已经能覆盖大部分场景；如果涉及报表、BI、复杂查询，再进一步掌握窗口函数和 JSON 函数。