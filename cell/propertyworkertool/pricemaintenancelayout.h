#ifndef PRICEMAINTENANCELAYOUT_H
#define PRICEMAINTENANCELAYOUT_H

#include <QWidget>
#include <QStandardItemModel> // For table view model
#include "showtool/mywidget.h"
#include "showtool/myeditablerowmodel.h" // For editable row model
#include <QHeaderView> // For header view
#include <QMutexLocker> // For thread-safe operations

QT_BEGIN_NAMESPACE
namespace Ui {
class PriceMaintenanceLayout;
}
QT_END_NAMESPACE

class PriceMaintenanceLayout : public MyWidget
{
    Q_OBJECT

public:
    PriceMaintenanceLayout(QWidget *parent = nullptr);
    ~PriceMaintenanceLayout();
    void init() override;

private slots:

    void on_pushButtonaAdd_clicked();

    void on_pushButtonDelete_clicked();

    void on_pushButtonUpdate_clicked();

    void searchSlots();



    void on_pushButtonSelect_clicked();

    void on_lineEdit_returnPressed();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::PriceMaintenanceLayout *ui;
    MyEditableRowModel *model; // Model for the table view
    //线程安全锁
    QMutex dataMutex; // Mutex for thread-safe operations
    bool addOk = 0;
    bool updateOk = 0;
    bool deleteOk = 0;
    QString currentId;
    QString id; // 用于存储当前编辑的业主ID
};
#endif // PRICEMAINTENANCELAYOUT_H
