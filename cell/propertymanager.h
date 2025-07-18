#ifndef PROPERTYMANAGER_H
#define PROPERTYMANAGER_H

#include <QWidget>
#include "showtool/mywidget.h"
#include "propertymanagertool/leaveapprovaltab.h"
#include "propertymanagertool/staffmanagementtab.h"
#include "propertymanagertool/systemsettingstab.h"
#include "propertymanagertool/usermanagementtab.h"
#include "propertymanagertool/paymentsumsee.h"
class PropertyManager : public QWidget
{
    Q_OBJECT

public:
    explicit PropertyManager(QWidget *parent = nullptr);
    ~PropertyManager();

private:
    LeaveApprovalTab *leaveApprovalTab;
    StaffManagementTab *staffManagementTab;
    SystemSettingsTab *systemSettingsTab;
    UserManagementTab *userManagementTab;
    PaymentSumSee *paymentSumSeeTab;
    QString currentUserString="阿布 杜古里"; // 当前用户字符串
    //请假审批消息提醒函数
    void showLeaveApprovalNotification();//灏

};

#endif // PROPERTYMANAGER_H
