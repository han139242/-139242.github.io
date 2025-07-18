#ifndef MYPARKING_H
#define MYPARKING_H

#include <QWidget>
#include <QWidget>
#include "ui_ownermanagementitem.h"
#include "lib/sqlmanager.h" // For SQL operations
#include <QStandardItemModel> // For table view model
#include <QHeaderView> // For header view
#include "showtool/myeditablerowmodel.h"
#include <QMutex> // For thread safety
#include "showtool/mywidget.h" // Assuming this is a custom widget base class

namespace Ui {
class MyParking;
}

class MyParking : public MyWidget
{
    Q_OBJECT

public:
    explicit MyParking(QWidget *parent = nullptr);
    ~MyParking();
    void init() override;
private:
    Ui::MyParking *ui;
    // 线程安全锁
    QMutex dataMutex;
    QVector<QVector<QString>> myParking; // 用于存储数据;
    MyEditableRowModel *model; // Model for the table view
};

#endif // MYPARKING_H
