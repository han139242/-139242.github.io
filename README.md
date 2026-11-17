# 智慧社区桌面应用

基于 **Qt/C++ 和 SQLite** 的课程项目，按住户、物业工作人员和管理人员拆分桌面操作界面。

| 角色 | 有哪些功能 | 源码入口 |
| --- | --- | --- |
| 住户 | 看公告、提交报修、查看处理过程与评价、查费用和申请车位 | [住户界面](cell/resident.ui) · [住户模块](cell/residenttool) |
| 物业工作人员 | 处理报修、维护住户信息、发布公告、登记考勤、管理停车和收费记录 | [工作端界面](cell/propertyworker.ui) · [物业模块](cell/propertyworkertool) |
| 管理人员 | 人员与账号管理、请假审批、收费汇总及系统设置 | [管理端界面](cell/propertymanager.ui) · [管理模块](cell/propertymanagertool) |

## 从源码开始

1. 在 Qt Creator 打开 [SmartCommunity.pro](SmartCommunity.pro)，选择具备 Widgets、SQL（含 SQLite 驱动）和 Charts 的本机 Qt / C++ kit，再按 IDE 提示构建。
2. 应用按可执行文件目录下的 `db/smart_upc.db` 查找数据库。将仓库中的[空结构 SQLite 库](bin/db/smart_upc.db)复制到测试构建目录的对应位置。
3. 使用自己的测试数据和账号完成初始化，再分角色检查对应流程。结构可读[初始化说明](bin/db/init.txt)，数据库连接实现见 [sqlmanager.cpp](lib/sqlmanager.cpp)。

仓库内的 SQLite 文件只含空表结构。构建和角色登录需结合使用者的 Qt kit 和测试数据核验。QXlsx 的版权与许可按组件原有标注保留。
