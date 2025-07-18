#include "parkingapplication.h"
#include "ui_parkingapplication.h"
#include "showtool/mywidget.h" // Assuming this is a custom widget base class
#include "lib/sqlmanager.h" // For SQL operations
#include <QDebug>
#include <QDateTime>
#include <QMessageBox>
#include "showtool/errortoast.h"



ParkingApplication::ParkingApplication(QWidget *parent)
    : MyWidget(parent)
    , ui(new Ui::ParkingApplication)
{
    ui->setupUi(this);
}

ParkingApplication::~ParkingApplication()
{
    delete ui;
}

void ParkingApplication::on_pushButton_clicked()
{
    // 获取lineEdit（申请人）和lineEdit_2(车牌号)中的数据
    QString applicant = ui->lineEdit->text().trimmed();
    QString licensePlate = ui->lineEdit_2->text().trimmed();
    // 检查申请人和车牌号是否为空
    if (applicant.isEmpty() || licensePlate.isEmpty()) {
        qDebug() << "申请人和车牌号不能为空";
        return; // 如果有空字段，直接返回
    }
    // 将申请人和车牌号以及当前时间以年-月-日-时-分插入到数据库
    QDateTime currentTime = QDateTime::currentDateTime();
    QString formattedTime = currentTime.toString("yyyy-MM-dd hh:mm");

    // 将申请人和车牌号插入到数据库  //删了个id不知道哪来的
    if (SqlManager::GetInstance().InsertParkingApplication(StaticUserId,applicant, licensePlate,formattedTime)) {
        qDebug() << "申请提交成功";
        ErrorToast *toast = ErrorToast::instance(this);
        toast->showToast(tr("提交成功!"), 0,0,ErrorToast::SuccessIcon);
        // 清空输入框
        ui->lineEdit->clear();
        ui->lineEdit_2->clear();
    } else {
        ErrorToast *toast = ErrorToast::instance(this);
        toast->showToast(tr("提交失败!"), 0,0,ErrorToast::WarningIcon);
        qDebug() << "申请提交失败";
    }

}

