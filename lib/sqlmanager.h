#ifndef SQLMANAGER_H
#define SQLMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QVector>
#include <QString>
#include <QDateTime>

class SqlManager
{
public:
    static SqlManager& GetInstance();
    bool InitDatabase();
    void Print();

    // 用户认证相关
    bool Login(const QString &id, const QString &password, const QString &identity);

    // 业主信息管理
    bool AddOwnerInfo(const QString &id, const QString &ownerName,
                      const QString &propertyInformation, const QString &parkingInformation,
                      const QString &contact, const QString &licensePlateNumber);
    QVector<QVector<QString>> GetOwnerInfo(const QString &id);

    // 考勤管理
    QVector<QVector<QString>> GetAttendanceData(const QString &id);
    QString GetAttendanceState(const QString &id);
    QString GetClockInTime(const QString &id);
    QString GetClockOutTime(const QString &id);

    // 打卡功能
    bool InsertClockInRecord(const QString &id, const QDateTime &clockInTime);
    bool UpdateClockOutRecord(const QString &id, const QDateTime &clockOutTime);
    bool GetTodayAttendanceStatus(const QString &id, QDateTime &clockInTime, QDateTime &clockOutTime);


    //获取业主数据
    QVector<QVector<QString>> GetOwnerData();
    //增加业主数据，传入一个QVector<QString>类型的参数
    bool AddOwnerData(const QVector<QString> &ownerData);
    // 删除业主数据
    bool DeleteOwnerData(const QString &id);
    //修改业主数据
    bool UpdateOwnerData(const QString &id, const QVector<QString> &ownerData);
    //查询业主数据
    QVector<QVector<QString>> QueryOwnerData(const QString &id);


    //获取停车位租赁数据
    QVector<QVector<QString>> GetParkingLeaseData();
    //增加停车位租赁数据
    bool AddParkingLeaseData(const QVector<QString> &leaseData);
    //更新停车位租赁数据
    bool UpdateParkingLeaseData(const QString &id);


    //请假
    bool insertLeaveData(const QString &id, const QString &reason, const QString &startTime, const QString &endTime);
    bool insertLeaveData(const QString &id, const QString &reason, const QString &startTime, const QString &endTime , const QString &leaveOfAbsenceStatus);
    //向出勤表内插入请假数据 传入开始日期和结束日期
    bool InsertLeaveDataToAttendance(const QString &id, const QDate &startDate, const QDate &endDate);

    //获取当前用户的请假数据
    QVector<QVector<QString>> GetLeaveData(const QString &id);
    QVector<QVector<QString>> GetLeaveData(const QString &id ,const QString &leaveOfAbsenceStatus);
    bool DeleteLeaveData(const QString &id);
    bool InsertLeaveData(const QString &id, const QString &reason, const QString &startTime, const QString &finshTime);


    //获得所有用户的请假数据
    QVector<QVector<QString>> GetAllLeaveData();
    //更新请假数据
    bool UpdateLeaveData(const QString &id,const QString &startTime, const QString &handingOfComments);
    //通过请假
    bool ApproveLeave(const QString &id, const QString &startTime,const QString &handingOfComments,const QString &handlers);
    //拒绝请假
    bool RejectLeave(const QString &id, const QString &startTime);

    //获取所有员工数据
    QVector<QVector<QString>> GetStaffData();
    //添加员工数据
    bool AddStaffData(const QVector<QString> &staffData);
    //删除员工数据
    bool DeleteStaffData(const QString &id);
    //更新员工数据
    bool UpdateStaffData(const QString &id, const QVector<QString> &staffData);
    //查询员工数据
    QVector<QVector<QString>> QueryStaffData(const QString &id);


    //从User表里查出所有物业工作人员的name和id
    QVector<QVector<QString>> GetStaffNamesAndIds();
    //InsertFaultRepairData(staffId, faultDescription)
    bool InsertFaultRepairData(const QString &id,const QString &repairman,const QString &maintenanceLocation,const QString &submissionReason,const QString &repairedItems,const QString &repairProgress);
    //获取所有故障报修数据
    QVector<QVector<QString>> GetRepairEvaluationData();
    //UpdateRepairEvaluationData(id, score)
    bool UpdateRepairEvaluationData(const QString &id, const QString &score, const QString &ownerEvaluation, const QString &repairProgress);//评分
    //维修进程（获取维修数据）
    QVector<QVector<QString>> GetRepairProcessData();
    //保修物品
    //QVector<QVector<QString>> GetWarrantyData();
    //维修过程yes键
    bool   UpdateRepairProcessStatus(const QString &newId, const QString &repairProgress);
    //维修过程no键
    bool UpdateRepairProcessStatusNo(const QString &id, const QString &repairProgress);
    //通过id,repairman,maintenceLocation,repairedItems,submissionReason查询newid
    QString GetNewIdFromRepairProcess(const QString &id,const QString &repairman,const QString &maintenanceLocation,const QString &repairedItems,const QString &submissionReason);


    //获取当前id的name 灏
    QString GetNameById(const QString &id);
    //查找有没有未审批的请假 灏
    bool HasPendingLeaveRequests();
    //查询有没有未销假的请假 灏
    bool HasUnfinishedLeaveRequests(const QString &id);
    //查询有没有当前id未完成的维修请求
    bool HasUnfinishedRepairRequests(const QString &id);

    //从User表获取当前用户的password 宇
    QString GetPasswordById(const QString &id);
    //改变当前用户的密码 宇
    bool ChangePassword(const QString &id, const QString &newPassword);
    //获取数据库中车位数据 宇
    QVector<QVector<QString>> GetParkingData();
    // 添加车位数据 宇
    bool AddParkingData(const QVector<QString> &parkingData);
    // 删除车位数据 宇
    bool DeleteParkingData(const QString &id);
    // 更新车位数据 宇
    bool UpdateParkingData(const QString &id, const QVector<QString> &parkingData);
    // 查询车位数据 宇
    QVector<QVector<QString>> QueryParkingData(const QString &id);
    //将车位申请中申请人的姓名，申请车位传入数据库 宇
    bool InsertParkingApplication(const QString &userid,const QString &userName, const QString &parkingPosition,const QString &time);

    //向车辆出入表中新加 灏
    bool InsertVehicleEntryDate(const QString &plateNumber,const QString &inOrOut ,const QString &time );
    //初始化车辆出入表
    QVector<QVector<QString>> InitVehicleEntryDate();
    //删除所有的车辆出入表数据
    bool DeleteAllVehicleEntryDate();

    //获取车位表中time最早的且车位数据为空的用户id
    QString GetEarliestTimeUserId();
    //给用户分配车位
    bool AssignParkingToUser(const QString &userId, const QString &parkingId);

    //GetSystemSettingsData()
    QVector<QVector<QString>> GetSystemSettingsData(); //刘涵  systemsetting
    //增加User表数据 传入一个QVector<QString>类型的参数
    bool AddSystemSettingsData(const QVector<QString> &systemSettingsData); //刘涵  systemsetting
    //删除User表数据
    bool DeleteSystemSettingsData(const QString &id); //刘涵  systemsetting
    //修改User表数据
    bool UpdateSystemSettingsData(const QString &id, const QVector<QString> &systemSettingsData); //刘涵  systemsetting

    //GetUserData
    QVector<QVector<QString>> GetUserData(); //刘涵  usermanagement
    //清除数据库中propertyTable表中的所有数据
    bool ClearUserData(); //刘涵  usermanagement

    //保修物品
    QVector<QVector<QString>> GetWarrantyData();//范家权
    //增加objectTable数据
    bool AddWarrantyData(const QVector<QString> &warrantyData);//范家权
    //删除objectTable数据
    bool DeleteWarrantyData(const QString &newId);//范家权
    //修改objectTable数据
    bool UpdateWarrantyData(const QString &newId, const QVector<QString> &warrantyData);//范家权

    //获取我的车位
    QVector<QVector<QString>> GetmyParking(const QString &userid);

    //GetPaymentRecordData
    QVector<QVector<QString>> GetPaymentRecordData(); //刘涵 paymentrecord
    //增加支付记录数据
    bool AddPaymentRecordData(const QVector<QString> &paymentData); //刘涵 paymentrecord
    //GetPaymentRecordData_1
    QVector<QVector<QString>> GetPaymentRecordData_1(); //刘涵 paymentrecord
    //删除支付记录数据
    bool DeletePaymentRecordData(const QString &id); //刘涵 paymentrecord
    //修改支付记录数据
    bool UpdatePaymentFormData(const QString &id, const QVector<QString> &paymentData); //刘涵 paymentrecord
    //获取缴费数据 刘韩;
    QVector<QVector<QString>> GetPaymentData(const QString &userid);
    //获取缴费数据_1 刘韩
    QVector<QVector<QString>> GetPaymentData_1(const QString &userid);
    //通过业主id获取主键id 加上缴费类型 金额 状态
    QString GetPaymentIdByUserId(const QString &userid,const QString &paymentType,const QString &paymentAmount,const QString &paymentStatus);
    //通过id更新缴费记录数据
    bool UpdateSelfPaymentRecordData(const QString &id);
    //更改数据库中缴费记录的状态
    bool UpdatePaymentRecordStatus(const QString &id, const QString &status);
    //获取所有支付数据
    QVector<QVector<QString>> GetAllPaymentData();

    //检测业主有没有未缴费记录
    bool CheckPaymentRecordStatus(const QString &userid);

    //向noticeForm表中插入notice
    bool InsertNotice(const QString &notice);
    //获取所有通知数据
    QVector<QVector<QString>> GetAllNoticeData();
    //根据userid,payType,paymentTime,theAmountOfTheContribution,payState获取id
    QString GetPaymentRecordIdByUserId(const QString &userid,const QString &payType,
                                       const QString &paymentTime,const QString &theAmountOfTheContribution,
                                       const QString &payState);



private:
    SqlManager();
    ~SqlManager();
    SqlManager(const SqlManager&) = delete;
    SqlManager& operator=(const SqlManager&) = delete;

    QSqlDatabase db;
};

#endif // SQLMANAGER_H
