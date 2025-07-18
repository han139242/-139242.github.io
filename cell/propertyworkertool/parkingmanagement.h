#ifndef PARKINGMANAGEMENT_H
#define PARKINGMANAGEMENT_H

#include <QWidget>
#include <QStandardItemModel> // For table view model
#include "showtool/mywidget.h" // Assuming this is the base class for your widgets
#include <showtool/myeditablerowmodel.h> // Assuming this is a custom model for editable rows


QT_BEGIN_NAMESPACE
namespace Ui {
class ParkingManagement;
}
QT_END_NAMESPACE

class ParkingManagement : public MyWidget
{
    Q_OBJECT

public:
    ParkingManagement(QWidget *parent = nullptr);
    ~ParkingManagement();
    void init() override;

private slots:
    void on_pushButtonAdd_clicked();

    void on_pushButtonDelete_clicked();

    void on_pushButtonUpdate_clicked();

    void on_pushButtonSelect_clicked();
    void searchSlots();

    void on_lineEdit_2_returnPressed();
    //查询函数
    void searchParkingData();

    void on_pushButton_12_clicked();

    void on_lineEdit_3_returnPressed();

    void on_pushButton_10_clicked();

    void on_pushButton_9_clicked();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::ParkingManagement *ui;
    MyEditableRowModel *model; // Model for the table view
    MyEditableRowModel *model2; // Model for the second table view



    QVector<QVector<QString>> parkingData; // 用于存储车位数据
    QVector<QVector<QString>> vehicleEntryDate; // 用于存储车辆进入时间数据

    // 线程安全锁
    QMutex dataMutex;
    QString currentId; // 当前操作的车位ID
    QString currentId2; // 当前操作的车牌ID
    bool addOk = false; // 判断是否添加成功
    bool deleteOk = false; // 判断是否删除成功
    bool updateOk = false; // 判断是否更新成功
    bool selectOk = false; // 判断是否查询成功
};
#endif // PARKINGMANAGEMENT_H
