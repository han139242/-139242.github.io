#ifndef LEAVEREGUEST_H
#define LEAVEREGUEST_H
#include "showtool/mywidget.h"
#include <QWidget>
#include <QMutex>
#include "showtool/myeditablerowmodel.h"


QT_BEGIN_NAMESPACE
namespace Ui {
class LeaveReguest;
}
QT_END_NAMESPACE

class LeaveReguest : public MyWidget
{
    Q_OBJECT

public:
    LeaveReguest(QWidget *parent = nullptr);
    ~LeaveReguest();
    void init() override; // 初始化方法

private slots:
    void on_pushButton_clicked();

    void on_submitButton_clicked();

    void on_deleteButton_clicked();

private:
    Ui::LeaveReguest *ui;
    MyEditableRowModel *model; // 表格模型
    QVector<QVector<QString>> leaveData; // 用于存储请假数据
    // 线程安全锁
    QMutex dataMutex;
};
#endif // LEAVEREGUEST_H
