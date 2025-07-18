#ifndef PROPERTYWORKER_H
#define PROPERTYWORKER_H

#include <QWidget>
#include <QTabWidget>
#include "propertyworkertool/attendance.h"
#include "propertyworkertool/leavereguest.h"
#include "propertyworkertool/parkingmanagement.h"
#include "propertyworkertool/parkinglease.h"
#include "propertyworkertool/pricemaintenancelayout.h"
#include "propertyworkertool/paymentrecordtab.h"
#include "propertyworkertool/warrantyinguirylayout.h"
#include "propertyworkertool/maintenanceprocesstab.h"
#include "propertyworkertool/ownermanagementitem.h"
#include "propertyworkertool/attendanceregistration.h"
#include "propertyworkertool/updatenewnotice.h"

class propertyWorker : public QWidget
{
    Q_OBJECT

public:
    explicit propertyWorker(QWidget *parent = nullptr);
    ~propertyWorker();

private:
    //Ui::propertyWorker *ui;
    Attendance *attendanceItTab;
    LeaveReguest *leaveRequestTab;
    AttendanceRegistration *attendanceRegistrationTab;
    OwnerManagementItem *ownerManagementTab;
    ParkingManagement *parkingManagementTab;
    ParkingLease *parkingLeaseTab;
    PriceMaintenanceLayout *priceMaintenanceTab;
    PaymentRecordTab *paymentRecordTab;
    WarrantyInguiryLayout *warrantyInquiryTab;
    MaintenanceProcessTab *maintenanceProcessTab;
    UpdateNewNotice *updateNewNoticeTab;
    QString currentUserString="阿布 杜古里";
    //请假审批消息提醒函数（未销假提醒
    void showLeaveApprovalNotification();//灏
    //维修消息提醒函数
    void showMaintenanceNotification(); //灏
};

#endif // PROPERTYWORKER_H
