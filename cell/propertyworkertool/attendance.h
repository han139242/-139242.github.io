#ifndef ATTENDANCE_H
#define ATTENDANCE_H

#include <QWidget>
#include <QStandardItemModel>
#include "showtool/mywidget.h"
#include "showtool/myeditablerowmodel.h"
#include <QMutex>

QT_BEGIN_NAMESPACE
namespace Ui {
class Attendance;
}
QT_END_NAMESPACE

class Attendance : public MyWidget
{
    Q_OBJECT

public:
    explicit Attendance(QWidget *parent = nullptr);
    ~Attendance();

    void init() override;

private:
    Ui::Attendance *ui;
    MyEditableRowModel *model; // 表格模型
    QVector<QVector<QString>> attendanceData;

    // 线程安全锁
    QMutex dataMutex;
};

#endif // ATTENDANCE_H
