Qt图书管理系统

1.介绍：基于Qt + MySQL的C/S架构图书管理系统，包含服务器端和客户端。

2.环境要求：Qt6.5+，MySQL 8.0+，MinGW/GCC/MSVC（64位）

3.快速运行

3.1. 数据库准备

安装MySQL并启动服务

执行sql/init_db.sql脚本初始化数据库：

mysql -u root -p < …/sql/init.sql（仅导入结构，使用librarymanagersystem.sql可导入结构和数据）

3.2. 编译运行服务器

用 Qt Creator 打开librarySystemServer/pro/librarySystemServer.pro


编译并运行服务器端

3.3编译运行客户端

用 Qt Creator 打开libraryManagermentClient/pro/libraryManagermentClient.pro

编译并运行客户端

连接服务器（端口1925）

4.项目结构

LibrarySystemServer：服务器端代码（处理数据库交互、客户端连接）

LibraryManagermentClient：客户端代码（用户界面、与服务器通信）

sql/：数据库初始化 / 更新脚本

功能说明:

图书增删改查

用户登录

借书 / 还书操作

数据统计

个人资料修改
