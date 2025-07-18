#include "sqlmanager.h"
#include <QDebug>
#include <QCoreApplication>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QDate>
#include <QSqlRecord>
#include <QSqlDatabase>


SqlManager::SqlManager()
{
    // 初始化数据库连接
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(QCoreApplication::applicationDirPath() + "/db/smart_upc.db");
}

SqlManager::~SqlManager()
{
    if (db.isOpen()) {
        db.close();
    }
    qDebug() << "数据库连接已关闭";
}

SqlManager& SqlManager::GetInstance()
{
    static SqlManager sqlManager;
    return sqlManager;
}

void SqlManager::Print()
{
    qDebug() << "SqlManager 实例地址:" << this;
}

bool SqlManager::InitDatabase()
{
    if (db.open()) {
        qDebug() << "数据库打开成功! 路径:" << db.databaseName();
        return true;
    } else {
        qDebug() << "无法打开数据库:" << db.lastError().text();
        return false;
    }
}

// ================== 用户认证相关 ==================
bool SqlManager::Login(const QString &id, const QString &password, const QString &identity)
{
    QSqlQuery query;
    query.prepare("SELECT * FROM user WHERE id = :id AND password = :password AND identity = :identity");
    query.bindValue(":id", id);
    query.bindValue(":password", password);
    query.bindValue(":identity", identity);

    if (query.exec() && query.next()) {
        qDebug() << "登录成功! 用户ID:" << id;
        return true;
    } else {
        qDebug() << "登录失败! 错误:" << query.lastError().text();
        return false;
    }
}

// ================== 业主信息管理 ==================
bool SqlManager::AddOwnerInfo(const QString &id, const QString &ownerName,
                              const QString &propertyInformation, const QString &parkingInformation,
                              const QString &contact, const QString &licensePlateNumber)
{
    QSqlQuery query;
    query.prepare("INSERT INTO ownerTable (id, ownerName, propertyInformation, parkingInformation, contact, licensePlateNumber) "
                  "VALUES (:id, :ownerName, :propertyInformation, :parkingInformation, :contact, :licensePlateNumber)");
    query.bindValue(":id", id);
    query.bindValue(":ownerName", ownerName);
    query.bindValue(":propertyInformation", propertyInformation);
    query.bindValue(":parkingInformation", parkingInformation);
    query.bindValue(":contact", contact);
    query.bindValue(":licensePlateNumber", licensePlateNumber);

    if (query.exec()) {
        qDebug() << "业主信息添加成功! ID:" << id;
        return true;
    } else {
        qDebug() << "业主信息添加失败:" << query.lastError().text();
        return false;
    }
}

QVector<QVector<QString>> SqlManager::GetOwnerInfo(const QString &id)
{
    QSqlQuery query;
    query.prepare("SELECT * FROM ownerTable WHERE id = :id");
    query.bindValue(":id", id);

    QVector<QVector<QString>> ownerInfo;

    if (query.exec()) {
        while (query.next()) {
            QVector<QString> row;
            for (int i = 0; i < query.record().count(); i++) {
                row.append(query.value(i).toString());
            }
            ownerInfo.append(row);
        }
    } else {
        qDebug() << "查询业主信息失败:" << query.lastError().text();
    }
    return ownerInfo;
}

// ================== 考勤管理 ==================
QVector<QVector<QString>> SqlManager::GetAttendanceData(const QString &id)
{
    QSqlQuery query;
    query.prepare("SELECT date, state, clockinTime, clockoutTime FROM propertyTable WHERE id = :id");
    query.bindValue(":id", id);

    QVector<QVector<QString>> attendanceData;

    if (query.exec()) {
        while (query.next()) {
            QVector<QString> row;
            row.append(query.value("date").toString());
            row.append(query.value("state").toString());
            row.append(query.value("clockinTime").toString());
            row.append(query.value("clockoutTime").toString());
            attendanceData.append(row);
        }
    } else {
        qDebug() << "查询考勤数据失败:" << query.lastError().text();
    }
    return attendanceData;
}

QString SqlManager::GetAttendanceState(const QString &id)
{
    QSqlQuery query;
    query.prepare("SELECT state FROM propertyTable WHERE id = :id");
    query.bindValue(":id", id);

    if (query.exec() && query.next()) {
        return query.value("state").toString();
    } else {
        qDebug() << "查询出勤状态失败:" << query.lastError().text();
        return QString();
    }
}

QString SqlManager::GetClockInTime(const QString &id)
{
    QSqlQuery query;
    query.prepare("SELECT clockinTime FROM propertyTable WHERE id = :id");
    query.bindValue(":id", id);

    if (query.exec() && query.next()) {
        return query.value("clockinTime").toString();
    } else {
        qDebug() << "查询上班打卡时间失败:" << query.lastError().text();
        return QString();
    }
}

QString SqlManager::GetClockOutTime(const QString &id)
{
    QSqlQuery query;
    query.prepare("SELECT clockoutTime FROM propertyTable WHERE id = :id");
    query.bindValue(":id", id);

    if (query.exec() && query.next()) {
        return query.value("clockoutTime").toString();
    } else {
        qDebug() << "查询下班打卡时间失败:" << query.lastError().text();
        return QString();
    }
}

// ================== 打卡功能 ==================
// 修改后的打卡功能实现
bool SqlManager::InsertClockInRecord(const QString &id, const QDateTime &clockInTime)
{
    QSqlQuery query;
    query.prepare("INSERT INTO propertyTable (id, date, clockinTime,state) "
                  "VALUES (:id, :date, :clockinTime,'出勤')");
    query.bindValue(":id", id);
    query.bindValue(":date", clockInTime.date());  // 存储日期
    query.bindValue(":clockinTime", clockInTime.time().toString("hh:mm"));  // 只存储时间（精确到分钟）

    if (query.exec()) {
        qDebug() << "上班打卡成功! ID:" << id << "日期:" << clockInTime.date().toString("yyyy-MM-dd")
            << "时间:" << clockInTime.time().toString("hh:mm");
        return true;
    } else {
        qDebug() << "上班打卡失败:" << query.lastError().text();
        return false;
    }
}

bool SqlManager::UpdateClockOutRecord(const QString &id, const QDateTime &clockOutTime)
{
    QSqlQuery query;
    query.prepare("UPDATE propertyTable SET clockoutTime = :clockoutTime "
                  "WHERE id = :id AND date = :date");
    query.bindValue(":clockoutTime", clockOutTime.time().toString("hh:mm"));  // 只存储时间（精确到分钟）
    query.bindValue(":id", id);
    query.bindValue(":date", clockOutTime.date());  // 使用日期作为查询条件

    if (query.exec()) {
        qDebug() << "下班打卡成功! ID:" << id << "时间:" << clockOutTime.time().toString("hh:mm");
        return true;
    } else {
        qDebug() << "下班打卡失败:" << query.lastError().text();
        return false;
    }
}

bool SqlManager::GetTodayAttendanceStatus(const QString &id, QDateTime &clockInTime, QDateTime &clockOutTime)
{
    QSqlQuery query;
    QDate today = QDate::currentDate();

    query.prepare("SELECT date, clockinTime, clockoutTime FROM propertyTable "
                  "WHERE id = :id AND date = :date");
    query.bindValue(":id", id);
    query.bindValue(":date", today);

    if (query.exec() && query.next()) {
        QDate date = query.value("date").toDate();
        QString inTimeStr = query.value("clockinTime").toString();
        QString outTimeStr = query.value("clockoutTime").toString();

        // 创建包含日期和时间的完整对象
        clockInTime = QDateTime(date, QTime::fromString(inTimeStr, "hh:mm"));

        // 如果下班时间不为空
        if (!outTimeStr.isNull() && !outTimeStr.isEmpty()) {
            clockOutTime = QDateTime(date, QTime::fromString(outTimeStr, "hh:mm"));
        } else {
            clockOutTime = QDateTime(); // 设置为无效时间
        }
        return true;
    }
    return false;
}

QVector<QVector<QString> > SqlManager::GetOwnerData()
{
    QSqlQuery query;
    query.prepare("SELECT id,ownerName,propertyInformation,parkingInformation,contact,licensePlateNumber FROM ownerTable");

    QVector<QVector<QString>> ownerData;

    if (query.exec()) {
        while (query.next()) {
            QVector<QString> row;
            for (int i = 0; i < query.record().count(); i++) {
                row.append(query.value(i).toString());
            }
            ownerData.append(row);
        }
    } else {
        qDebug() << "查询业主数据失败:" << query.lastError().text();
    }
    return ownerData;


    if (query.exec()) {
        while (query.next()) {
            QVector<QString> row;
            for (int i = 0; i < query.record().count(); i++) {
                row.append(query.value(i).toString());
            }
            ownerData.append(row);
        }
    } else {
        qDebug() << "查询业主数据失败:" << query.lastError().text();
    }
    return ownerData;

}

bool SqlManager::AddOwnerData(const QVector<QString> &ownerData)
{
    if (ownerData.size() != 6) {
        qDebug() << "业主数据格式错误，必须包含6个字段";
        return false;
    }

    QSqlQuery query;
    query.prepare("INSERT INTO ownerTable (id, ownerName, propertyInformation, parkingInformation, contact, licensePlateNumber) "
                  "VALUES (:id, :ownerName, :propertyInformation, :parkingInformation, :contact, :licensePlateNumber)");
    query.bindValue(":id", ownerData[0]);
    query.bindValue(":ownerName", ownerData[1]);
    query.bindValue(":propertyInformation", ownerData[2]);
    query.bindValue(":parkingInformation", ownerData[3]);
    query.bindValue(":contact", ownerData[4]);
    query.bindValue(":licensePlateNumber", ownerData[5]);

    if (query.exec()) {
        qDebug() << "业主数据添加成功! ID:" << ownerData[0];
        return true;
    } else {
        qDebug() << "业主数据添加失败:" << query.lastError().text();
        return false;
    }

}

bool SqlManager::DeleteOwnerData(const QString &id)
{
    QSqlQuery query;
    query.prepare("DELETE FROM ownerTable WHERE id = :id");
    query.bindValue(":id", id);

    if (query.exec()) {
        qDebug() << "业主数据删除成功! ID:" << id;
        return true;
    } else {
        qDebug() << "业主数据删除失败:" << query.lastError().text();
        return false;
    }
}

bool SqlManager::UpdateOwnerData(const QString &id, const QVector<QString> &ownerData)
{
    if (ownerData.size() != 6) {
        qDebug() << "业主数据格式错误，必须包含6个字段";
        return false;
    }

    QSqlQuery query;
    query.prepare("UPDATE ownerTable SET id = :newid,"
                  "ownerName = :ownerName, "
                  "propertyInformation = :propertyInformation, "
                  "parkingInformation = :parkingInformation, "
                  "contact = :contact, "
                  "licensePlateNumber = :licensePlateNumber "
                  "WHERE id = :id");
    query.bindValue(":id", id);
    query.bindValue(":newid", ownerData[0]); // 更新ID
    query.bindValue(":ownerName", ownerData[1]);
    query.bindValue(":propertyInformation", ownerData[2]);
    query.bindValue(":parkingInformation", ownerData[3]);
    query.bindValue(":contact", ownerData[4]);
    query.bindValue(":licensePlateNumber", ownerData[5]);

    if (query.exec()) {
        qDebug() << "业主数据更新成功! ID:" << id;
        return true;
    } else {
        qDebug() << "业主数据更新失败:" << query.lastError().text();
        return false;
    }

}

QVector<QVector<QString> > SqlManager::GetParkingLeaseData()
{

    QSqlQuery query;
    query.prepare("SELECT id, carId,userName, position, carType, rentalStatus, rent FROM parkingSpaceMeter;");

    QVector<QVector<QString>> leaseData;

    if (query.exec()) {
        while (query.next()) {
            QVector<QString> row;
            for (int i = 0; i < query.record().count(); i++) {
                row.append(query.value(i).toString());
            }
            leaseData.append(row);
        }
    } else {
        qDebug() << "查询停车位租赁数据失败:" << query.lastError().text();
    }
    return leaseData;
}

bool SqlManager::AddParkingLeaseData(const QVector<QString> &leaseData)
{
    if (leaseData.size() != 7) {
        qDebug() << "停车位租赁数据格式错误，必须包含7个字段";
        return false;
    }

    QSqlQuery query;
    query.prepare("INSERT INTO parkingSpaceMeter (id, carId,userName, position, carType, rentalStatus, rent) "
                  "VALUES (:id, :carId, :userName, :position, :carType, :rentalStatus, :rent);");
    query.bindValue(":id", leaseData[0]);
    query.bindValue(":carId", leaseData[1]);
    query.bindValue(":userName", leaseData[2]);
    query.bindValue(":position", leaseData[3]);
    query.bindValue(":carType", leaseData[4]);
    query.bindValue(":rentalStatus", leaseData[5]);
    query.bindValue(":rent", leaseData[6]);


    if (query.exec()) {
        qDebug() << "停车位租赁数据添加成功! ID:" << leaseData[0];
        return true;
    } else {
        qDebug() << "停车位租赁数据添加失败:" << query.lastError().text();
        return false;
    }
}

bool SqlManager::UpdateParkingLeaseData(const QString &id)
{
    QSqlQuery query;
    query.prepare("UPDATE parkingSpaceMeter SET rentalStatus = '已出租' WHERE id = :id");
    query.bindValue(":id", id);

    if (query.exec()) {
        qDebug() << "停车位租赁数据更新成功! ID:" << id;
        return true;
    } else {
        qDebug() << "停车位租赁数据更新失败:" << query.lastError().text();
        return false;
    }
}

bool SqlManager::insertLeaveData( const QString &id, const QString &reason, const QString &startTime, const QString &finshTime, const QString &leaveOfAbsenceStatus)
{
    QSqlQuery query;
    query.prepare("INSERT INTO leaveOfAbsence (id, reason, startTime, finshTime,leaveOfAbsenceStatus) "
                  "VALUES (:id, :reason, :startTime, :finshTime,:leaveOfAbsenceStatus)");
    query.bindValue(":id", id);
    query.bindValue(":reason", reason);
    query.bindValue(":startTime", startTime);
    query.bindValue(":finshTime", finshTime);
    query.bindValue(":leaveOfAbsenceStatus", leaveOfAbsenceStatus);


    if (query.exec()) {
        qDebug() << "请假数据插入成功! ID:" << id;
        return true;
    } else {
        qDebug() << "请假数据插入失败:" << query.lastError().text();
        return false;
    }
}

bool SqlManager::InsertLeaveDataToAttendance(const QString &id, const QDate &startDate, const QDate &endDate)
{
    //在考勤表中 从startDate到endDate之间的每一天插入一条记录
    QSqlQuery query;
    query.prepare("INSERT INTO propertyTable (id, date, state) "
                  "VALUES (:id, :date, '请假')"); // 0表示未打卡状态
    query.bindValue(":id", id);
    QDate currentDate = startDate;
    while (currentDate <= endDate) {
        query.bindValue(":date", currentDate);
        if (!query.exec()) {
            qDebug() << "插入请假数据到考勤表失败:" << query.lastError().text();
            return false;
        }
        currentDate = currentDate.addDays(1);
    }
    qDebug() << "请假数据成功插入到考勤表! ID:" << id << "从" << startDate.toString("yyyy-MM-dd") << "到" << endDate.toString("yyyy-MM-dd");
    return true;
}

QVector<QVector<QString> > SqlManager::GetLeaveData(const QString &id,const QString&leaveOfAbsenceStatus)
{
    QSqlQuery query;//id,leaveOfAbsenceStatus,startTime,finshTime，reason
    query.prepare("SELECT id,leaveOfAbsenceStatus,startTime,finshTime,reason,handlers FROM leaveOfAbsence WHERE id = :id AND leaveOfAbsenceStatus = :leaveOfAbsenceStatus");
    query.bindValue(":id", id);
    query.bindValue(":leaveOfAbsenceStatus", leaveOfAbsenceStatus);

    QVector<QVector<QString>> leaveData;

    if (query.exec()) {
        while (query.next()) {
            QVector<QString> row;
            for (int i = 0; i < query.record().count(); i++) {
                row.append(query.value(i).toString());
            }
            leaveData.append(row);
        }

    } else {
        qDebug() << "查询请假数据失败:" << query.lastError().text();
    }
    return leaveData;
}

QVector<QVector<QString> > SqlManager::GetLeaveData(const QString &id)
{
    QSqlQuery query;
    query.prepare("SELECT * FROM leaveOfAbsence WHERE id = :id");
    query.bindValue(":id", id);

    QVector<QVector<QString>> leaveData;

    if (query.exec()) {
        while (query.next()) {
            QVector<QString> row;
            for (int i = 0; i < query.record().count(); i++) {
                row.append(query.value(i).toString());
            }
            leaveData.append(row);
        }
    } else {
        qDebug() << "查询请假数据失败:" << query.lastError().text();
    }
    return leaveData;
}
//插入数据库中leaveOfAbsence表中的leaveOfAbsanceStatu中状态未销假的数据
bool SqlManager::InsertLeaveData(const QString &id, const QString &reason, const QString &startTime, const QString &finshTime)
{
    QSqlQuery query;
    query.prepare("INSERT INTO leaveOfAbsence (id, reason, startTime, finshTime) "
                  "VALUES (:id, :reason, :startTime, :finshTime)");
    query.bindValue(":id", id);
    query.bindValue(":reason", reason);
    query.bindValue(":startTime", startTime);
    query.bindValue(":finshTime", finshTime);

    if (query.exec()) {
        qDebug() << "请假数据插入成功! ID:" << id;
        return true;
    } else {
        qDebug() << "请假数据插入失败:" << query.lastError().text();
        return false;
    }
}

QVector<QVector<QString> > SqlManager::GetAllLeaveData()
{
    QSqlQuery query;
    //"ID", "姓名","请假状态", "开始时间", "结束时间", "请假理由", "处理人","处理意见"
    query.prepare("SELECT id, leaveOfAbsenceStatus, startTime, finshTime, reason,handlers,handingOfComments FROM leaveOfAbsence");
    QVector<QVector<QString>> leaveData;
    if (query.exec()) {
        while (query.next()) {
            QVector<QString> row;
            row.append(query.value("id").toString());
            row.append(query.value("leaveOfAbsenceStatus").toString());
            row.append(query.value("startTime").toString());
            row.append(query.value("finshTime").toString());
            row.append(query.value("reason").toString());
            row.append(query.value("handlers").toString());
            row.append(query.value("handingOfComments").toString());
            leaveData.append(row);
        }
    } else {
        qDebug() << "查询所有请假数据失败:" << query.lastError().text();
    }
    //查询user表 在第二位加上姓名
    for (auto &row : leaveData) {
        QSqlQuery userQuery;
        userQuery.prepare("SELECT name FROM user WHERE id = :id");
        userQuery.bindValue(":id", row[0]);
        if (userQuery.exec() && userQuery.next()) {
            QString name = userQuery.value("name").toString();
            row.insert(1, name); // 在第二位插入姓名
        } else {
            row.insert(1, "未知"); // 如果查询失败，插入"未知"
        }
    }
    return leaveData;

}

bool SqlManager::UpdateLeaveData(const QString &id, const QString &pass, const QString &handingOfComments)
{
    QSqlQuery query;
    query.prepare("UPDATE leaveOfAbsence SET cancellationOfFakes = :cancellationOfFakes, "
                  "handingOfComments = :handingOfComments WHERE id = :id");
    query.bindValue(":id", id);
    query.bindValue(":pass", pass);
    query.bindValue(":handingOfComments", handingOfComments);

    if (query.exec()) {
        qDebug() << "请假数据更新成功! ID:" << id;
        return true;
    } else {
        qDebug() << "请假数据更新失败:" << query.lastError().text();
        return false;
    }
}

bool SqlManager::ApproveLeave(const QString &id, const QString &startTime, const QString &handingOfComments,const QString &handlers)//灏
{
    //把id和startTime对应的leaveOfAbsenceStatus改成为“未销假” 然后在加上处理意见handingOfComments
    QSqlQuery query;
    query.prepare("UPDATE leaveOfAbsence SET leaveOfAbsenceStatus = '未销假', handlers= :handlers ,handingOfComments = :handingOfComments "
                  "WHERE id = :id AND startTime = :startTime AND leaveOfAbsenceStatus = '未审批'");
    query.bindValue(":id", id);
    query.bindValue(":startTime", startTime);
    query.bindValue(":handingOfComments", handingOfComments);
    query.bindValue(":handlers", handlers);
    if (query.exec()) {
        qDebug() << "请假批准成功! ID:" << id << "开始时间:" << startTime;
        return true;
    } else {
        qDebug() << "请假批准失败:" << query.lastError().text();
        return false;
    }
}

bool SqlManager::RejectLeave(const QString &id, const QString &startTime)
{
    //把id和startTime对应的leaveOfAbsenceStatus改成为“已拒绝”
    QSqlQuery query;
    query.prepare("UPDATE leaveOfAbsence SET leaveOfAbsenceStatus = '已拒绝' WHERE id = :id AND startTime = :startTime AND leaveOfAbsenceStatus = '未审批'");
    query.bindValue(":id", id);
    query.bindValue(":startTime", startTime);

    if (query.exec()) {
        qDebug() << "请假拒绝成功! ID:" << id << "开始时间:" << startTime;
        return true;
    } else {
        qDebug() << "请假拒绝失败:" << query.lastError().text();
        return false;
    }
}

QVector<QVector<QString> > SqlManager::GetStaffData()
{
    QSqlQuery query;
    query.prepare("SELECT id, staffName, job, telephoneNumber, dateOfEmployment FROM staffManagerTable");

    QVector<QVector<QString>> staffData;

    if (query.exec()) {
        while (query.next()) {
            QVector<QString> row;
            for (int i = 0; i < query.record().count(); i++) {
                row.append(query.value(i).toString());
            }
            staffData.append(row);
        }
    } else {
        qDebug() << "查询员工数据失败:" << query.lastError().text();
    }
    return staffData;
}

bool SqlManager::AddStaffData(const QVector<QString> &staffData)
{
    if (staffData.size() != 5) {
        qDebug() << "员工数据格式错误，必须包含5个字段";
        return false;
    }

    QSqlQuery query;
    query.prepare("INSERT INTO staffManagerTable (id, staffName, job,telephoneNumber , dateOfEmployment) "
                  "VALUES (:id, :staffName, :job, :telephoneNumber, :dateOfEmployment)");
    query.bindValue(":id", staffData[0]);
    query.bindValue(":staffName", staffData[1]);
    query.bindValue(":job", staffData[2]);
    query.bindValue(":telephoneNumber", staffData[3]);
    query.bindValue(":dateOfEmployment", staffData[4]);

    if (query.exec()) {
        qDebug() << "员工数据添加成功! ID:" << staffData[0];
        return true;
    } else {
        qDebug() << "员工数据添加失败:" << query.lastError().text();
        return false;
    }
}

bool SqlManager::DeleteStaffData(const QString &id)
{
    QSqlQuery query;
    query.prepare("DELETE FROM staffManagerTable WHERE id = :id");
    query.bindValue(":id", id);

    if (query.exec()) {
        qDebug() << "员工数据删除成功! ID:" << id;
        return true;
    } else {
        qDebug() << "员工数据删除失败:" << query.lastError().text();
        return false;
    }
}

bool SqlManager::UpdateStaffData(const QString &id, const QVector<QString> &staffData)
{
    if (staffData.size() != 5) {
        qDebug() << "员工数据格式错误，必须包含5个字段";
        return false;
    }

    QSqlQuery query;
    query.prepare("UPDATE staffManagerTable SET id = :newid, "
                  "staffName = :staffName, "
                  "job = :job, "
                  "telephoneNumber = :telephoneNumber, "
                  "dateOfEmployment = :dateOfEmployment "
                  "WHERE id = :id");
    query.bindValue(":id", id);
    query.bindValue(":newid", staffData[0]); // 更新ID
    query.bindValue(":staffName", staffData[1]);
    query.bindValue(":job", staffData[2]);
    query.bindValue(":telephoneNumber", staffData[3]);
    query.bindValue(":dateOfEmployment", staffData[4]);

    if (query.exec()) {
        qDebug() << "员工数据更新成功! ID:" << id;
        return true;
    } else {
        qDebug() << "员工数据更新失败:" << query.lastError().text();
        return false;
    }
}

QVector<QVector<QString> > SqlManager::GetStaffNamesAndIds() //从User表里查出所有物业工作人员的name和id
{
    QSqlQuery query;
    query.prepare("SELECT id, name FROM User WHERE identity = '物业工作人员'");

    QVector<QVector<QString>> staffNamesAndIds;

    if (query.exec()) {
        while (query.next()) {
            QVector<QString> row;
            row.append(query.value("id").toString());
            row.append(query.value("name").toString());
            staffNamesAndIds.append(row);
        }
    } else {
        qDebug() << "查询员工姓名和ID失败:" << query.lastError().text();
    }
    return staffNamesAndIds;
}

bool SqlManager::InsertFaultRepairData(const QString &id, const QString &repairman, const QString &maintenanceLocation, const QString &submissionReason, const QString &repairedItems, const QString &repairProgress)
{
    QSqlQuery query;
    query.prepare("INSERT INTO repairTable (id, repairman, maintenanceLocation, submissionReason, repairedItems, repairProgress) "
                  "VALUES (:id, :repairman, :maintenanceLocation, :submissionReason, :repairedItems, :repairProgress)");
    query.bindValue(":id", id);
    query.bindValue(":repairman", repairman);
    query.bindValue(":maintenanceLocation", maintenanceLocation);
    query.bindValue(":submissionReason", submissionReason);
    query.bindValue(":repairedItems", repairedItems);
    query.bindValue(":repairProgress", repairProgress);

    if (query.exec()) {
        qDebug() << "故障报修数据插入成功! ID:" << id;
        return true;
    } else {
        qDebug() << "故障报修数据插入失败:" << query.lastError().text();
        return false;
    }
}









bool SqlManager::DeleteLeaveData(const QString &id) //销假
{
    QSqlQuery query;
    query.prepare("UPDATE leaveOfAbsence SET leaveOfAbsenceStatus = '已销假' WHERE id = :id");
    query.bindValue(":id", id);

    if (query.exec()) {
        qDebug() << "销假成功! ID:" << id;
        return true;
    } else {
        qDebug() << "销假失败:" << query.lastError().text();
        return false;
    }
}



QVector<QVector<QString> > SqlManager::GetRepairEvaluationData()
{
    QSqlQuery query;
    query.prepare("SELECT id, repairman, maintenanceLocation, submissionReason, repairedItems,score FROM repairTable");

    QVector<QVector<QString>> repairData;

    if (query.exec()) {
        while (query.next()) {
            QVector<QString> row;
            for (int i = 0; i < query.record().count(); i++) {
                row.append(query.value(i).toString());
            }
            repairData.append(row);
        }
    } else {
        qDebug() << "查询维修数据失败:" << query.lastError().text();
    }
    return repairData;
}

bool SqlManager::UpdateRepairEvaluationData(const QString &id, const QString &score, const QString &ownerEvaluation, const QString &repairProgress)
{
    QSqlQuery query;
    query.prepare("UPDATE repairTable SET score = :score, ownerEvaluation = :ownerEvaluation, repairProgress = :repairProgress WHERE newId = :id");
    query.bindValue(":id", id);
    query.bindValue(":score", score);
    query.bindValue(":ownerEvaluation", ownerEvaluation);
    query.bindValue(":repairProgress", repairProgress);

    if (query.exec()) {
        qDebug() << "维修评价数据更新成功! ID:" << id;
        return true;
    } else {
        qDebug() << "维修评价数据更新失败:" << query.lastError().text();
        return false;
    }
}

QVector<QVector<QString> > SqlManager::GetRepairProcessData()
{
    QSqlQuery query;
    query.prepare("SELECT id, repairman, maintenanceLocation, submissionReason, repairedItems, repairProgress FROM repairTable");

    QVector<QVector<QString>> repairProcessData;

    if (query.exec()) {
        while (query.next()) {
            QVector<QString> row;
            for (int i = 0; i < query.record().count(); i++) {
                row.append(query.value(i).toString());
            }
            repairProcessData.append(row);
        }
    } else {
        qDebug() << "查询维修进程数据失败:" << query.lastError().text();
    }
    return repairProcessData;
}
/*
QVector<QVector<QString> > SqlManager::GetWarrantyData()
{
    QSqlQuery query;
    query.prepare("SELECT newId, objectName, warrantyEndDate FROM objectTable");

    QVector<QVector<QString>> warrantyData;

    if (query.exec()) {
        while (query.next()) {
            QVector<QString> row;
            for (int i = 0; i < query.record().count(); i++) {
                row.append(query.value(i).toString());
            }
            warrantyData.append(row);
        }
    } else {
        qDebug() << "查询保修物品数据失败:" << query.lastError().text();
    }
    return warrantyData;
}
*/
bool SqlManager::UpdateRepairProcessStatus(const QString &newId, const QString &repairProgress)
{
    QSqlQuery query;
    query.prepare("UPDATE repairTable SET repairProgress = :repairProgress WHERE newId = :newId");
    query.bindValue(":newId", newId);
    query.bindValue(":repairProgress", repairProgress);

    if (query.exec()) {
        qDebug() << "维修进程更新成功! ID:" << newId;
        return true;
    } else {
        qDebug() << "维修进程更新失败:" << query.lastError().text();
        return false;
    }
}

bool SqlManager::UpdateRepairProcessStatusNo(const QString &id, const QString &repairProgress)
{
    QSqlQuery query;
    query.prepare("UPDATE repairTable SET repairProgress = :repairProgress WHERE id = :id");
    query.bindValue(":id", id);
    query.bindValue(":repairProgress", repairProgress);

    if (query.exec()) {
        qDebug() << "维修进程更新成功! ID:" << id;
        return true;
    } else {
        qDebug() << "维修进程更新失败:" << query.lastError().text();
        return false;
    }
}

QString SqlManager::GetNewIdFromRepairProcess(const QString &id, const QString &repairman, const QString &maintenanceLocation, const QString &repairedItems, const QString &submissionReason)
{
    QSqlQuery query;
    query.prepare("SELECT newId FROM repairTable WHERE id = :id AND repairman = :repairman "
                  "AND maintenanceLocation = :maintenanceLocation AND repairedItems = :repairedItems "
                  "AND submissionReason = :submissionReason");
    query.bindValue(":id", id);
    query.bindValue(":repairman", repairman);
    query.bindValue(":maintenanceLocation", maintenanceLocation);
    query.bindValue(":repairedItems", repairedItems);
    query.bindValue(":submissionReason", submissionReason);
    if (query.exec() && query.next()) {
        return query.value("newId").toString();
    } else {
        qDebug() << "查询维修进程新ID失败:" << query.lastError().text();
        qDebug() << "查询条件:" << id << repairman << maintenanceLocation << repairedItems << submissionReason;
        return QString();
    }
}
QString SqlManager::GetNameById(const QString &id)
{
    QSqlQuery query;
    query.prepare("SELECT name FROM user WHERE id = :id");
    query.bindValue(":id", id);

    if (query.exec() && query.next()) {
        return query.value("name").toString();
    } else {
        qDebug() << "查询姓名失败:" << query.lastError().text();
        return QString();
    }
}

bool SqlManager::HasPendingLeaveRequests()
{
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM leaveOfAbsence WHERE leaveOfAbsenceStatus = '未审批'");

    if (query.exec() && query.next()) {
        int count = query.value(0).toInt();
        return count > 0; // 如果有未审批的请假请求，返回true
    } else {
        qDebug() << "查询未审批请假请求失败:" << query.lastError().text();
        return false; // 查询失败时返回false
    }
}

bool SqlManager::HasUnfinishedLeaveRequests(const QString &id)
{
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM leaveOfAbsence WHERE id = :id AND leaveOfAbsenceStatus = '未销假'");
    query.bindValue(":id", id);

    if (query.exec() && query.next()) {
        int count = query.value(0).toInt();
        return count > 0; // 如果有未销假的请假请求，返回true
    } else {
        qDebug() << "查询未销假请假请求失败:" << query.lastError().text();
        return false; // 查询失败时返回false
    }
}

bool SqlManager::HasUnfinishedRepairRequests(const QString &id)
{
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM repairTable WHERE id = :id AND repairProgress = '正在进行中'");
    query.bindValue(":id", id);

    if (query.exec() && query.next()) {
        int count = query.value(0).toInt();
        return count > 0; // 如果有未完成的维修请求，返回true
    } else {
        qDebug() << "查询未完成维修请求失败:" << query.lastError().text();
        return false; // 查询失败时返回false
    }
}

QString SqlManager::GetPasswordById(const QString &id)
{
    QSqlQuery query;
    query.prepare("SELECT password FROM User WHERE id = :id");
    query.bindValue(":id", id);

    if (query.exec() && query.next()) {
        return query.value("password").toString();
    } else {
        qDebug() << "查询密码失败:" << query.lastError().text();
        return QString();
    }
}

bool SqlManager::ChangePassword(const QString &id, const QString &newPassword)
{
    QSqlQuery query;
    query.prepare("UPDATE User SET password = :newPassword WHERE id = :id");
    query.bindValue(":id", id);
    query.bindValue(":newPassword", newPassword);

    if (query.exec()) {
        qDebug() << "密码修改成功! 用户ID:" << id;
        return true;
    } else {
        qDebug() << "密码修改失败:" << query.lastError().text();
        return false;
    }
}

QVector<QVector<QString> > SqlManager::GetParkingData()
{
    QSqlQuery query;
    query.prepare("SELECT carId,position, carType,rentalStatus, userid, rent FROM parkingSpaceMeter");

    QVector<QVector<QString>> parkingData;

    if (query.exec()) {
        while (query.next()) {
            QVector<QString> row;
            for (int i = 0; i < query.record().count(); i++) {
                row.append(query.value(i).toString());
            }
            parkingData.append(row);
        }
    } else {
        qDebug() << "查询停车数据失败:" << query.lastError().text();
    }
    return parkingData;
}

bool SqlManager::AddParkingData(const QVector<QString> &parkingData)
{
    if (parkingData.size() != 6) {
        qDebug() << "停车数据格式错误，必须包含6个字段";
        return false;
    }

    QSqlQuery query;
    query.prepare("INSERT INTO parkingSpaceMeter (carId, position, carType, rentalStatus, userName, rent) "
                  "VALUES (:carId, :position, :carType, :rentalStatus, :userName, :rent)");
    query.bindValue(":carId", parkingData[0]);
    query.bindValue(":position", parkingData[1]);
    query.bindValue(":carType", parkingData[2]);
    query.bindValue(":rentalStatus", parkingData[3]);
    query.bindValue(":userName", parkingData[4]);
    query.bindValue(":rent", parkingData[5]);

    if (query.exec()) {
        qDebug() << "停车数据添加成功! 车牌号:" << parkingData[0];
        return true;
    } else {
        qDebug() << "停车数据添加失败:" << query.lastError().text();
        return false;
    }
}

bool SqlManager::DeleteParkingData(const QString &id)
{
    QSqlQuery query;
    query.prepare("DELETE FROM parkingSpaceMeter WHERE carId = :id");
    query.bindValue(":id", id);

    if (query.exec()) {
        qDebug() << "停车数据删除成功! 车牌号:" << id;
        return true;
    } else {
        qDebug() << "停车数据删除失败:" << query.lastError().text();
        return false;
    }
}

bool SqlManager::UpdateParkingData(const QString &id, const QVector<QString> &parkingData)
{
    if (parkingData.size() != 6) {
        qDebug() << "停车数据格式错误，必须包含6个字段";
        return false;
    }

    QSqlQuery query;
    query.prepare("UPDATE parkingSpaceMeter SET carId = :newCarId, position = :position, "
                  "carType = :carType, rentalStatus = :rentalStatus, userName = :userName, rent = :rent "
                  "WHERE carId = :id");
    query.bindValue(":id", id);
    query.bindValue(":newCarId", parkingData[0]); // 更新车牌号
    query.bindValue(":position", parkingData[1]);
    query.bindValue(":carType", parkingData[2]);
    query.bindValue(":rentalStatus", parkingData[3]);
    query.bindValue(":userName", parkingData[4]);
    query.bindValue(":rent", parkingData[5]);

    if (query.exec()) {
        qDebug() << "停车数据更新成功! 车牌号:" << id;
        return true;
    } else {
        qDebug() << "停车数据更新失败:" << query.lastError().text();
        return false;
    }
}

QVector<QVector<QString> > SqlManager::QueryParkingData(const QString &id)
{
    QSqlQuery query;
    query.prepare("SELECT carId, position, carType, rentalStatus, userName, rent FROM parkingSpaceMeter WHERE carId = :id");
    query.bindValue(":id", id);

    QVector<QVector<QString>> parkingData;

    if (query.exec()) {
        while (query.next()) {
            QVector<QString> row;
            for (int i = 0; i < query.record().count(); i++) {
                row.append(query.value(i).toString());
            }
            parkingData.append(row);
        }
    } else {
        qDebug() << "查询停车数据失败:" << query.lastError().text();
    }
    return parkingData;
}

bool SqlManager::InsertParkingApplication(const QString &userid,const QString &userName, const QString &carPlateNumber,const QString &time)
{
    QSqlQuery query;
    query.prepare("INSERT INTO vehicleApplicationForm (userid,userName, carPlateNumber,time) "
                  "VALUES (:userid,:userName, :carPlateNumber,:time)");
    query.bindValue(":userid", userid);
    query.bindValue(":userName", userName);
    query.bindValue(":carPlateNumber", carPlateNumber);
    query.bindValue(":time", time);

    if (query.exec()) {
        qDebug() << "停车申请插入成功! 用户名:" << userName << "停车位置:" << carPlateNumber;
        return true;
    } else {
        qDebug() << "停车申请插入失败:" << query.lastError().text();
        return false;
    }
}

bool SqlManager::InsertVehicleEntryDate(const QString &plateNumber, const QString &inOrOut, const QString &time)
{
    QSqlQuery query;
    query.prepare("INSERT INTO vehicleEntryAndExitMeters (licensePlateNumber, timeOfEntry,departureTime) "
                  "VALUES (:licensePlateNumber, :timeOfEntry,:departureTime)");
    query.bindValue(":licensePlateNumber", plateNumber);
    query.bindValue(":timeOfEntry", inOrOut);
    query.bindValue(":departureTime", time);
    if (query.exec()) {
        qDebug() << "车辆进入时间插入成功! 车牌号:" << plateNumber << "进入时间:" << inOrOut;
        return true;
    } else {
        qDebug() << "车辆进入时间插入失败:" << query.lastError().text();
        return false;
    }
}
QVector<QVector<QString>> SqlManager::InitVehicleEntryDate()
{
    QSqlQuery query;
    query.prepare("SELECT licensePlateNumber, timeOfEntry,departureTime FROM vehicleEntryAndExitMeters");
    QVector<QVector<QString>> vehicleEntryDate;
    if (query.exec()) {
        while (query.next()) {
            QVector<QString> row;
            for (int i = 0; i < query.record().count(); i++) {
                row.append(query.value(i).toString());
            }
            vehicleEntryDate.append(row);
        }
    } else {
        qDebug() << "查询车辆进入时间失败:" << query.lastError().text();
    }
    return vehicleEntryDate;
}
bool SqlManager::DeleteAllVehicleEntryDate()
{
    QSqlQuery query;
    query.prepare("DELETE FROM vehicleEntryAndExitMeters");
    if (query.exec()) {
        qDebug() << "所有车辆进入时间数据删除成功!";
        return true;
    } else {
        qDebug() << "删除所有车辆进入时间数据失败:" << query.lastError().text();
        return false;
    }
}
QString SqlManager::GetEarliestTimeUserId()
{
    QSqlQuery query;
    query.prepare("SELECT userid FROM vehicleApplicationForm WHERE time = (SELECT MIN(time) FROM vehicleApplicationForm WHERE parkingPosition IS NULL)");
    if (query.exec()) {
        if (query.next()) {
            qDebug()<<query.value(0).toString();
            return query.value(0).toString();
        }
    } else {
        qDebug() << "查询最早时间用户ID失败:" << query.lastError().text();
    }
    return QString();
}
bool SqlManager::AssignParkingToUser(const QString &userId, const QString &parkingId)
{
    QSqlQuery query;
    query.prepare("UPDATE vehicleApplicationForm SET parkingPosition = :parkingId WHERE userid = :userId");
    query.bindValue(":userId", userId);
    query.bindValue(":parkingId", parkingId);

    if (query.exec()) {
        qDebug() << "用户分配车位成功! 用户ID:" << userId << "车位ID:" << parkingId;
        return true;
    } else {
        qDebug() << "用户分配车位失败:" << query.lastError().text();
        return false;
    }
    //更新车位表
    QSqlQuery query2;
    query2.prepare("UPDATE parkingSpaceMeter SET rentalStatus = :rentalStatus WHERE carId = :carId");
    query2.bindValue(":rentalStatus", "已出租");
    query2.bindValue(":carId", parkingId);
    if (query2.exec()) {
        qDebug() << "车位状态更新成功! 车位ID:" << parkingId;
        return true;
    } else {
        qDebug() << "车位状态更新失败:" << query2.lastError().text();
        return false;
    }
}
QVector<QVector<QString> > SqlManager::GetSystemSettingsData()  //刘涵 systemSetting
{
    QSqlQuery query;
    query.prepare("SELECT * FROM User");

    QVector<QVector<QString>> settingsData;

    if (query.exec()) {
        while (query.next()) {
            QVector<QString> row;
            for (int i = 0; i < query.record().count(); i++) {
                row.append(query.value(i).toString());
            }
            settingsData.append(row);
        }
    } else {
        qDebug() << "查询系统设置数据失败:" << query.lastError().text();
    }
    return settingsData;
}

bool SqlManager::AddSystemSettingsData(const QVector<QString> &systemSettingsData)  //刘涵 systemSetting
{
    if (systemSettingsData.size() != 4) {
        qDebug() << "系统设置数据格式错误，必须包含4个字段";
        return false;
    }

    QSqlQuery query;
    query.prepare("INSERT INTO User (id, name, password, identity) "
                  "VALUES (:id, :name, :password, :identity)");
    query.bindValue(":id", systemSettingsData[0]);
    query.bindValue(":name", systemSettingsData[1]);
    query.bindValue(":password", systemSettingsData[2]);
    query.bindValue(":identity", systemSettingsData[3]);

    if (query.exec()) {
        qDebug() << "系统设置数据添加成功! ID:" << systemSettingsData[0];
        return true;
    } else {
        qDebug() << "系统设置数据添加失败:" << query.lastError().text();
        return false;
    }
}

bool SqlManager::DeleteSystemSettingsData(const QString &id) //刘涵 systemSetting
{
    QSqlQuery query;
    query.prepare("DELETE FROM User WHERE id = :id");
    query.bindValue(":id", id);

    if (query.exec()) {
        qDebug() << "系统设置数据删除成功! ID:" << id;
        return true;
    } else {
        qDebug() << "系统设置数据删除失败:" << query.lastError().text();
        return false;
    }
}

bool SqlManager::UpdateSystemSettingsData(const QString &id, const QVector<QString> &systemSettingsData) //刘涵 systemSetting
{
    if (systemSettingsData.size() != 4) {
        qDebug() << "系统设置数据格式错误，必须包含4个字段";
        return false;
    }

    QSqlQuery query;
    query.prepare("UPDATE User SET id = :newid, name = :name, password = :password, identity = :identity WHERE id = :id");
    query.bindValue(":id", id);
    query.bindValue(":newid", systemSettingsData[0]); // 更新ID
    query.bindValue(":name", systemSettingsData[1]);
    query.bindValue(":password", systemSettingsData[2]);
    query.bindValue(":identity", systemSettingsData[3]);

    if (query.exec()) {
        qDebug() << "系统设置数据更新成功! ID:" << id;
        return true;
    } else {
        qDebug() << "系统设置数据更新失败:" << query.lastError().text();
        return false;
    }
}

QVector<QVector<QString> > SqlManager::GetUserData()  //刘涵 usermanagementtab
{
    QSqlQuery query;
    query.prepare("SELECT id,  state, clockinTime,clockoutTime FROM propertyTable");

    QVector<QVector<QString>> userData;

    if (query.exec()) {
        while (query.next()) {
            QVector<QString> row;
            for (int i = 0; i < query.record().count(); i++) {
                row.append(query.value(i).toString());
            }
            userData.append(row);
        }
    } else {
        qDebug() << "查询用户数据失败:" << query.lastError().text();
    }
    //查询user表 在第二位加上姓名
    for (auto &row : userData) {
        QSqlQuery userQuery;
        userQuery.prepare("SELECT name FROM User WHERE id = :id");
        userQuery.bindValue(":id", row[0]);
        if (userQuery.exec() && userQuery.next()) {
            QString name = userQuery.value("name").toString();
            row.insert(1, name); // 在第二位插入姓名
        } else {
            row.insert(1, "未知"); // 如果查询失败，插入"未知"
        }
    }
    return userData;
}

bool SqlManager::ClearUserData()
{
    QSqlQuery query;
    query.prepare("DELETE FROM propertyTable");

    if (query.exec()) {
        qDebug() << "用户数据清除成功!";
        return true;
    } else {
        qDebug() << "用户数据清除失败:" << query.lastError().text();
        return false;
    }
}
QVector<QVector<QString> > SqlManager::GetWarrantyData() //范家权
{
    QSqlQuery query;
    query.prepare("SELECT newId, objectName, warrantyEndDate FROM objectTable");

    QVector<QVector<QString>> warrantyData;

    if (query.exec()) {
        while (query.next()) {
            QVector<QString> row;
            for (int i = 0; i < query.record().count(); i++) {
                row.append(query.value(i).toString());
            }
            warrantyData.append(row);
        }
    } else {
        qDebug() << "查询保修物品数据失败:" << query.lastError().text();
    }
    return warrantyData;
}

bool SqlManager::AddWarrantyData(const QVector<QString> &warrantyData)
{
    if (warrantyData.size() != 3) {
        qDebug() << "保修物品数据格式错误，必须包含3个字段";
        return false;
    }

    QSqlQuery query;
    query.prepare("INSERT INTO objectTable (newId, objectName, warrantyEndDate) "
                  "VALUES (:newId, :objectName, :warrantyEndDate)");
    query.bindValue(":newId", warrantyData[0]);
    query.bindValue(":objectName", warrantyData[1]);
    query.bindValue(":warrantyEndDate", warrantyData[2]);

    if (query.exec()) {
        qDebug() << "保修物品数据添加成功! 新ID:" << warrantyData[0];
        return true;
    } else {
        qDebug() << "保修物品数据添加失败:" << query.lastError().text();
        return false;
    }
}

bool SqlManager::DeleteWarrantyData(const QString &newId)
{
    QSqlQuery query;
    query.prepare("DELETE FROM objectTable WHERE newId = :newId");
    query.bindValue(":newId", newId);

    if (query.exec()) {
        qDebug() << "保修物品数据删除成功! 新ID:" << newId;
        return true;
    } else {
        qDebug() << "保修物品数据删除失败:" << query.lastError().text();
        return false;
    }
}

bool SqlManager::UpdateWarrantyData(const QString &newId, const QVector<QString> &warrantyData)
{
    if (warrantyData.size() != 3) {
        qDebug() << "保修物品数据格式错误，必须包含3个字段";
        return false;
    }

    QSqlQuery query;
    query.prepare("UPDATE objectTable SET newId = :newId, objectName = :objectName, warrantyEndDate = :warrantyEndDate WHERE newId = :oldNewId");
    query.bindValue(":oldNewId", newId);
    query.bindValue(":newId", warrantyData[0]); // 更新新ID
    query.bindValue(":objectName", warrantyData[1]);
    query.bindValue(":warrantyEndDate", warrantyData[2]);
    qDebug()<< "更新保修物品数据:" << warrantyData[0] << warrantyData[1] << warrantyData[2];

    if (query.exec()) {
        qDebug() << "保修物品数据更新成功! 新ID:" << newId;
        return true;
    } else {
        qDebug() << "保修物品数据更新失败:" << query.lastError().text();
        return false;
    }
}
QVector<QVector<QString> > SqlManager::GetmyParking(const QString &userid)
{
    QSqlQuery query;
    //"车位位置","当前使用人", "是否可充电",  "租金"
    query.prepare("SELECT position,userName, carType, rent FROM parkingSpaceMeter WHERE userid=:userid");
    query.bindValue(":userid", userid);

    QVector<QVector<QString>> parkingData;

    if (query.exec()) {
        while (query.next()) {
            QVector<QString> row;
            for (int i = 0; i < query.record().count(); i++) {
                row.append(query.value(i).toString());
            }
            parkingData.append(row);
        }
    } else {
        qDebug() << "查询停车数据失败:" << query.lastError().text();
    }
    return parkingData;
}

QVector<QVector<QString> > SqlManager::GetPaymentRecordData()
{
    QSqlQuery query;
    query.prepare("SELECT userid, payType, theAmountOfTheContribution, paymentTime FROM paymentForm");

    QVector<QVector<QString>> paymentData;

    if (query.exec()) {
        while (query.next()) {
            QVector<QString> row;
            for (int i = 0; i < query.record().count(); i++) {
                row.append(query.value(i).toString());
            }
            paymentData.append(row);
        }
    } else {
        qDebug() << "查询缴费记录数据失败:" << query.lastError().text();
    }
    return paymentData;
}

bool SqlManager::AddPaymentRecordData(const QVector<QString> &paymentData)
{
    QSqlQuery query;
    query.prepare("INSERT INTO paymentForm (userid, payType, theAmountOfTheContribution, payState,paymentTime) "
                  "VALUES (:userid, :payType, :theAmountOfTheContribution, :payState,:peymentTime)");
    query.bindValue(":userid", paymentData[0]);
    query.bindValue(":payType", paymentData[1]);
    query.bindValue(":theAmountOfTheContribution", paymentData[2]);
    query.bindValue(":payState", "未缴费");
    query.bindValue(":peymentTime", "无"); // 添加缴费时间字段


    if (query.exec()) {
        qDebug() << "缴费记录数据添加成功! ID:" << paymentData[0];
        return true;
    } else {
        qDebug() << "缴费记录数据添加失败:" << query.lastError().text();
        return false;
    }
}

QVector<QVector<QString> > SqlManager::GetPaymentRecordData_1()
{
    QSqlQuery query;
    query.prepare("SELECT userid, payType, theAmountOfTheContribution, payState,paymentTime FROM paymentForm");

    QVector<QVector<QString>> paymentData;

    if (query.exec()) {
        while (query.next()) {
            QVector<QString> row;
            for (int i = 0; i < query.record().count(); i++) {
                row.append(query.value(i).toString());
            }
            paymentData.append(row);
        }
    } else {
        qDebug() << "查询缴费记录数据失败:" << query.lastError().text();
    }
    return paymentData;
}

bool SqlManager::DeletePaymentRecordData(const QString &id)
{
    QSqlQuery query;
    query.prepare("DELETE FROM paymentForm WHERE id = :id");
    query.bindValue(":id", id);

    if (query.exec()) {
        qDebug() << "缴费记录数据删除成功! ID:" << id;
        return true;
    } else {
        qDebug() << "缴费记录数据删除失败:" << query.lastError().text();
        return false;
    }
}

bool SqlManager::UpdatePaymentFormData(const QString &id, const QVector<QString> &paymentData)
{
    if (paymentData.size() != 5) {
        qDebug() << "缴费记录数据格式错误，必须包含5个字段";
        return false;
    }

    QSqlQuery query;
    query.prepare("UPDATE paymentForm SET userid = :userid, payType = :payType, theAmountOfTheContribution = :theAmountOfTheContribution, payState = :payState,paymentTime = :paymentTime WHERE id = :id");
    query.bindValue(":id", id);
    query.bindValue(":userid", paymentData[0]); // 更新ID
    query.bindValue(":payType", paymentData[1]);
    query.bindValue(":theAmountOfTheContribution", paymentData[2]);
    query.bindValue(":payState", paymentData[3]);
    query.bindValue(":paymentTime", paymentData[4]); // 更新缴费时间
    qDebug() << "更新缴费记录数据:" << paymentData[0] << paymentData[1] << paymentData[2] << paymentData[3];

    if (query.exec()) {
        qDebug() << "缴费记录数据更新成功! ID:" << id;
        return true;
    } else {
        qDebug() << "缴费记录数据更新失败:" << query.lastError().text();
        return false;
    }
}

QVector<QVector<QString> > SqlManager::GetPaymentData(const QString &userid)
{
    QSqlQuery query;
    query.prepare("SELECT userid,payType, paymentTime, theAmountOfTheContribution FROM paymentForm WHERE userid = :userid");
    query.bindValue(":userid", userid);

    QVector<QVector<QString>> paymentData;

    if (query.exec()) {
        while (query.next()) {
            QVector<QString> row;
            for (int i = 0; i < query.record().count(); i++) {
                row.append(query.value(i).toString());
            }
            paymentData.append(row);
        }
    } else {
        qDebug() << "查询缴费数据失败:" << query.lastError().text();
    }
    return paymentData;
}

QVector<QVector<QString> > SqlManager::GetPaymentData_1(const QString &userid)
{
    QSqlQuery query;
    query.prepare("SELECT userid,payType, paymentTime, theAmountOfTheContribution, payState FROM paymentForm WHERE userid = :userid");
    query.bindValue(":userid", userid);

    QVector<QVector<QString>> paymentData;

    if (query.exec()) {
        while (query.next()) {
            QVector<QString> row;
            for (int i = 0; i < query.record().count(); i++) {
                row.append(query.value(i).toString());
            }
            paymentData.append(row);
        }
    } else {
        qDebug() << "查询缴费数据失败:" << query.lastError().text();
    }
    return paymentData;
}

QString SqlManager::GetPaymentIdByUserId(const QString &userid,const QString &paymentType,const QString &paymentAmount,const QString &paymentStatus)
{
    QSqlQuery query;
    query.prepare("SELECT id FROM paymentForm WHERE userid = :userid AND payType = :paymentType AND theAmountOfTheContribution = :paymentAmount AND payState = :paymentStatus");
    query.bindValue(":userid", userid);
    query.bindValue(":paymentType", paymentType);
    query.bindValue(":paymentAmount", paymentAmount);
    query.bindValue(":paymentStatus", paymentStatus);

    if (query.exec() && query.next()) {
        return query.value("id").toString();
    } else {
        qDebug() << "查询缴费ID失败:" << query.lastError().text();
        return QString();
    }
}

bool SqlManager::UpdateSelfPaymentRecordData(const QString &id)
{
    QSqlQuery query;
    /*
    query.prepare("UPDATE paymentForm SET payState = '已缴费' WHERE id = :id");
    query.bindValue(":id", id);
    */ //向数据库中顺便加上当前的缴费日期 让缴费状态变成已缴费
    query.prepare("UPDATE paymentForm SET payState = '已缴费', paymentTime = :paymentTime WHERE id = :id");
    query.bindValue(":id", id);
    query.bindValue(":paymentTime", QDateTime::currentDateTime().toString("yyyy-MM-dd"));

    if (query.exec()) {
        qDebug() << "缴费记录状态更新成功! ID:" << id;
        return true;
    } else {
        qDebug() << "缴费记录状态更新失败:" << query.lastError().text();
        return false;
    }
}

bool SqlManager::UpdatePaymentRecordStatus(const QString &id, const QString &status)
{
    QSqlQuery query;
    query.prepare("UPDATE paymentForm SET payState = :status,paymentTime=:paymentTime WHERE id = :id");
    query.bindValue(":id", id);
    query.bindValue(":status", status);
    //绑定当前时间
    query.bindValue(":paymentTime", QDateTime::currentDateTime().toString("yyyy-MM-dd"));
    if (query.exec()) {
        qDebug() << "缴费记录状态更新成功! ID:" << id << "状态:" << status;
        return true;
    } else {
        qDebug() << "缴费记录状态更新失败:" << id <<query.lastError().text();
        return false;
    }
}

QVector<QVector<QString> > SqlManager::GetAllPaymentData()
{
    QSqlQuery query;
    query.prepare("SELECT userid, payType, theAmountOfTheContribution, payState, paymentTime FROM paymentForm");

    QVector<QVector<QString>> allPaymentData;

    if (query.exec()) {
        while (query.next()) {
            QVector<QString> row;
            for (int i = 0; i < query.record().count(); i++) {
                row.append(query.value(i).toString());
            }
            allPaymentData.append(row);
        }
    } else {
        qDebug() << "查询所有缴费数据失败:" << query.lastError().text();
    }
    //从数据库中根据userid在User表中查询姓名并插入第二列
    for (auto &row : allPaymentData) {
        QSqlQuery userQuery;
        userQuery.prepare("SELECT name FROM User WHERE id = :id");
        userQuery.bindValue(":id", row[0]); // 使用userid作为查询条件
        if (userQuery.exec() && userQuery.next()) {
            QString name = userQuery.value("name").toString();
            row.insert(1, name); // 在第二列插入姓名
        } else {
            row.insert(1, "未知"); // 如果查询失败，插入"未知"
        }
    }
    qDebug()<< "获取所有缴费数据成功，总条数:" << allPaymentData.size();
    return allPaymentData;
}
bool SqlManager::CheckPaymentRecordStatus(const QString &userid)
{
    QSqlQuery query;
    query.prepare("SELECT payState FROM paymentForm WHERE userid = :userid");
    query.bindValue(":userid", userid);
    if (query.exec()) {
        while (query.next()) {
            QString payState = query.value("payState").toString();
            if (payState == "未缴费") {
                return true; // 存在未缴费记录
            }
        }
    } else {
        qDebug() << "查询缴费状态失败:" << query.lastError().text();
    }
    return false;
}
bool SqlManager::InsertNotice(const QString &notice)
{
    QSqlQuery query;
    query.prepare("INSERT INTO noticeForm (notice, time) VALUES (:notice, :noticeTime)");
    query.bindValue(":notice", notice);
    query.bindValue(":noticeTime", QDateTime::currentDateTime().toString("yyyy-MM-dd"));
    if (query.exec()) {
        qDebug() << "公告插入成功!";
        return true;
    } else {
        qDebug() << "公告插入失败:" << query.lastError().text();
        return false;
    }
}
QVector<QVector<QString>> SqlManager::GetAllNoticeData()
{
    QSqlQuery query;
    query.prepare("SELECT notice, time FROM noticeForm");

    QVector<QVector<QString>> allNoticeData;

    if (query.exec()) {
        while (query.next()) {
            QVector<QString> row;
            for (int i = 0; i < query.record().count(); i++) {
                row.append(query.value(i).toString());
            }
            allNoticeData.append(row);
        }
    } else {
        qDebug() << "查询所有通知数据失败:" << query.lastError().text();
    }
    return allNoticeData;
}

QString SqlManager::GetPaymentRecordIdByUserId(const QString &userid, const QString &payType, const QString &paymentTime, const QString &theAmountOfTheContribution, const QString &payState)
{
    QSqlQuery query;
    query.prepare("SELECT id FROM paymentForm WHERE userid = :userid AND payType = :payType "
                  "AND paymentTime = :paymentTime AND theAmountOfTheContribution = :theAmountOfTheContribution "
                  "AND payState = :payState");
    query.bindValue(":userid", userid);
    query.bindValue(":payType", payType);
    query.bindValue(":paymentTime", paymentTime);
    query.bindValue(":theAmountOfTheContribution", theAmountOfTheContribution);
    query.bindValue(":payState", payState);
    qDebug() << "查询缴费记录ID:" << userid << payType << paymentTime << theAmountOfTheContribution << payState;
    if (query.exec() && query.next()) {
        return query.value("id").toString();
    } else {
        qDebug() << "查询缴费记录ID失败:" << query.lastError().text();
        return QString();
    }
}












