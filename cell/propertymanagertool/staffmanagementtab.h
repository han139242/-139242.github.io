#ifndef STAFFMANAGEMENTTAB_H
#define STAFFMANAGEMENTTAB_H

#include <QWidget>
#include "showtool/mywidget.h"
#include <QMutex>
#include "showtool/myeditablerowmodel.h"
#include <QVector>
#include <QString>
#include <QDebug>
#include "lib/sqlmanager.h" // For SQL operations
#include <QDate>


namespace Ui {
class StaffManagementTab;
}

class StaffManagementTab : public MyWidget
{
    Q_OBJECT

public:
    explicit StaffManagementTab(QWidget *parent = nullptr);
    ~StaffManagementTab();
    void init() override;

private slots:
    void on_addButton_clicked();

    void on_deleteButton_clicked();

    void on_updateButton_clicked();
    void searchSlots();


    void on_lineEdit_returnPressed();

    void on_checkButton_clicked();



    void on_pushButton_clicked();

private:
    Ui::StaffManagementTab *ui;
    bool addOk=0;
    bool deleteOk=0;
    bool updateOk=0;
    QString currentId; // 当前操作的员工ID
    MyEditableRowModel *model; // 表格模型
    QVector<QVector<QString>> staffData; // 用于存储员工数据
    // 线程安全锁
    QMutex dataMutex;



};

#endif // STAFFMANAGEMENTTAB_H
