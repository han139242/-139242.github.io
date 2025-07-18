#ifndef PARKINGLEASE_H
#define PARKINGLEASE_H

#include <QWidget>
#include <QStandardItemModel> // For table view model
#include "showtool/mywidget.h" // Assuming mywidget.h is in showtool directory
#include <QMutex> // For thread safety
#include <QVector>
#include <QString>
#include <QDebug>
#include "lib/sqlmanager.h" // For SQL operations
#include "showtool/myeditablerowmodel.h" // Assuming this is a custom editable row model

QT_BEGIN_NAMESPACE
namespace Ui {
class ParkingLease;
}
QT_END_NAMESPACE

class ParkingLease : public MyWidget
{
    Q_OBJECT

public:
    ParkingLease(QWidget *parent = nullptr);
    ~ParkingLease();
    void init() override;

private slots:
    void on_addButton_clicked();

    void on_rentButton_clicked();

    void on_cheakButton_clicked();
    void searchSlots();

    void on_lineEdit_returnPressed();

    void on_pushButton_clicked();

private:
    Ui::ParkingLease *ui;
    MyEditableRowModel *model; // Model for the table view
    QVector<QVector<QString>> leaseData; // 用于存储停车位租赁数据
    // 线程安全锁
    QMutex dataMutex;
    //判断增加过程到哪
    bool addOk = 0; // 用于判断是否处于添加状态
    bool deleteOk = 0; // 用于判断是否处于删除状态
    bool updateOk = 0; // 用于判断是否处于更新状态
    QString currentId; // 当前操作的停车位ID

};
#endif // PARKINGLEASE_H
