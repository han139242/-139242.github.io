#include "attendanceregistration.h"
#include "ui_attendanceregistration.h"
#include "lib/sqlmanager.h"
#include "showtool/errortoast.h"
#include <QDebug>
#include <QMessageBox>

AttendanceRegistration::AttendanceRegistration(QWidget *parent)
    : MyWidget(parent)
    , ui(new Ui::AttendanceRegistration)
{
    ui->setupUi(this);
    init();
}

void AttendanceRegistration::init()
{
    QString currentUserId = getCurrentUserId();
    QDateTime clockInTime, clockOutTime;

    // 初始化UI状态
    ui->checkInButton->setEnabled(true);
    ui->checkOutButton->setEnabled(false);
    ui->lblCheckInTime->setText("未打卡");
    ui->lblCheckOutTime->setText("未打卡");

    // 使用SqlManager获取当天考勤状态
    if (SqlManager::GetInstance().GetTodayAttendanceStatus(currentUserId, clockInTime, clockOutTime)) {
        // 如果有上班打卡记录
        if (clockInTime.isValid()) {
            ui->checkInButton->setEnabled(false); // 禁用上班按钮

            // 更新UI显示打卡时间（只显示时间部分）
            ui->lblCheckInTime->setText(clockInTime.time().toString("hh:mm"));

            // 如果还没有下班打卡记录
            if (!clockOutTime.isValid()) {
                ui->checkOutButton->setEnabled(true); // 启用下班按钮
                ui->lblCheckOutTime->setText("未打卡");
            } else {
                // 如果已经下班打卡，禁用所有按钮
                ui->checkOutButton->setEnabled(false);
                ui->lblCheckOutTime->setText(clockOutTime.time().toString("hh:mm"));
            }
        }
    }
}
QString AttendanceRegistration::getCurrentUserId()
{
    return StaticUserId;
}

AttendanceRegistration::~AttendanceRegistration()
{
    delete ui;
}

void AttendanceRegistration::on_checkInButton_clicked()
{
    QString currentUserId = getCurrentUserId();
    QDateTime currentTime = QDateTime::currentDateTime();

    // 使用SqlManager插入上班打卡记录
    if (SqlManager::GetInstance().InsertClockInRecord(currentUserId, currentTime)) {
        // 更新UI状态
        ui->checkInButton->setEnabled(false);
        ui->checkOutButton->setEnabled(true);
        ui->lblCheckInTime->setText(currentTime.time().toString("hh:mm")); // 只显示时间
        ErrorToast *toast = ErrorToast::instance(this);
        toast->showToast(tr("上班打卡成功"), 0,0,ErrorToast::SuccessIcon);
    } else {
        QMessageBox::warning(this, "打卡失败", "上班打卡失败，请稍后再试。");
    }
}

void AttendanceRegistration::on_checkOutButton_clicked()
{
    QString currentUserId = getCurrentUserId();
    QDateTime currentTime = QDateTime::currentDateTime();

    // 使用SqlManager更新下班打卡记录
    if (SqlManager::GetInstance().UpdateClockOutRecord(currentUserId, currentTime)) {
        // 更新UI状态
        ui->checkOutButton->setEnabled(false);
        ui->lblCheckOutTime->setText(currentTime.time().toString("hh:mm")); // 只显示时间
        ErrorToast *toast = ErrorToast::instance(this);
        toast->showToast(tr("下班打卡成功"), 0,0,ErrorToast::SuccessIcon);
    } else {
        QMessageBox::warning(this, "打卡失败", "下班打卡失败，请稍后再试。");
    }
}
