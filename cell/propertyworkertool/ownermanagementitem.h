#ifndef OWNERMANAGEMENTITEM_H
#define OWNERMANAGEMENTITEM_H

#include <QWidget>
#include <QStandardItemModel> // For table view model
#include "showtool/mywidget.h" // Assuming this is a custom widget base class
#include <QMutex> // For thread safety
#include <QVector>
#include <QString>
#include <QDebug>
#include "lib/sqlmanager.h" // For SQL operations
#include "showtool/myeditablerowmodel.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class OwnerManagementItem;
}
QT_END_NAMESPACE

class OwnerManagementItem : public MyWidget
{
    Q_OBJECT

public:
    OwnerManagementItem(QWidget *parent = nullptr);
    ~OwnerManagementItem();
    void init() override;

private slots:
    void on_AddButton_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_2_clicked();


    void searchSlots();

    void on_pushButton_4_clicked();

    void on_lineEdit_returnPressed();

    void on_pushButton_clicked();

private:
    Ui::OwnerManagementItem *ui;
    MyEditableRowModel *model; // Model for the table view
    QVector<QVector<QString>> ownerData; // 用于存储业主数据
    // 线程安全锁
    QMutex dataMutex;
    //判断增加过程到哪
    bool addOk=0;
    bool deleteOk=0;
    bool updateOk=0;
    QString currentId; // 当前操作的业主ID


};
#endif // OWNERMANAGEMENTITEM_H
