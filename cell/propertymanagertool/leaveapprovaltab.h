#ifndef LEAVEAPPROVALTAB_H
#define LEAVEAPPROVALTAB_H

#include <QWidget>
#include "showtool/mywidget.h"
#include <QMutex>
#include "showtool/myeditablerowmodel.h"
#include "lib/sqlmanager.h" // For SQL operations
#include <QVector>
#include <QString>
#include <QDebug>
#include <QDate>



namespace Ui {
class LeaveApprovalTab;
}

class LeaveApprovalTab : public MyWidget
{
    Q_OBJECT

public:
    explicit LeaveApprovalTab(QWidget *parent = nullptr);
    ~LeaveApprovalTab();
    void init() override;

private slots:
    void on_pushButton_2_clicked();


    void on_pushButton_3_clicked();

    void on_pushButton_clicked();

    void searchSlots();

    void on_lineEdit_returnPressed();

private:
    Ui::LeaveApprovalTab *ui;
    MyEditableRowModel *model; // 表格模型


    // 线程安全锁
    QMutex dataMutex;
    bool updateOk=0;
    QVector<QVector<QString>> leaveData; // 用于存储请假数据
    QString currentId; // 当前操作的业主ID
};

#endif // LEAVEAPPROVALTAB_H
