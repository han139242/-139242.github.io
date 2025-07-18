#ifndef SYSTEMSETTINGSTAB_H
#define SYSTEMSETTINGSTAB_H

#include <QWidget>
#include "showtool/mywidget.h"
#include <QMutex>
#include "lib/sqlmanager.h" // For SQL operations
#include <QMutexLocker>
#include "showtool/myeditablerowmodel.h" // Assuming this is a custom editable row model
#include <QStandardItemModel>
#include <QDebug>
#include <QHeaderView>
#include <QModelIndex>
#include <QVector>
#include <QDateTime>
#include <QStandardItem>
#include <QCoreApplication>
#include <QSqlQuery>
#include <QSqlError>



namespace Ui {
class SystemSettingsTab;
}

class SystemSettingsTab : public MyWidget
{
    Q_OBJECT
public:
    explicit SystemSettingsTab(QWidget *parent = nullptr);
    ~SystemSettingsTab();
    void init() override;

private slots:
    void on_addButton_clicked();

    void on_deleteButton_clicked();

    void on_updateButton_clicked();

    void searchSlots();

    void on_selectButton_clicked();

    void on_lineEdit_returnPressed();

private:
    Ui::SystemSettingsTab *ui;
    MyEditableRowModel *model; // 自定义可编辑行模型
    QVector<QVector<QString>> systemSettingsData; // 系统设置数据
    //线程安全锁
    QMutex dataMutex; // 用于保护数据访问的互斥锁
    bool addOk = false;
    bool deleteOk = false;
    bool updateOk = false;
    QString currentId; // 当前操作的系统设置ID

};

#endif // SYSTEMSETTINGSTAB_H
