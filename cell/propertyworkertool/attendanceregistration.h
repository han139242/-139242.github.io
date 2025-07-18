#ifndef ATTENDANCEREGISTRATION_H
#define ATTENDANCEREGISTRATION_H

#include <QWidget>
#include "showtool/mywidget.h"
#include <QMutex>


QT_BEGIN_NAMESPACE
namespace Ui {
class AttendanceRegistration;
}
QT_END_NAMESPACE

class AttendanceRegistration : public MyWidget
{
    Q_OBJECT

public:
    AttendanceRegistration(QWidget *parent = nullptr);
    ~AttendanceRegistration();
    void init() override;
    QString getCurrentUserId();
private slots:


    void on_checkInButton_clicked();

    void on_checkOutButton_clicked();

private:
    Ui::AttendanceRegistration *ui;
    QVector<QVector<QString>> attendanceData; // 用于存储考勤数据
    // 线程安全锁
    QMutex dataMutex;


};
#endif // ATTENDANCEREGISTRATION_H
