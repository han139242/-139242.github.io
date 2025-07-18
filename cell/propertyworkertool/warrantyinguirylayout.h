#ifndef WARRANTYINGUIRYLAYOUT_H
#define WARRANTYINGUIRYLAYOUT_H

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
class WarrantyInguiryLayout;
}
QT_END_NAMESPACE

class WarrantyInguiryLayout : public MyWidget
{
    Q_OBJECT

public:
    WarrantyInguiryLayout(QWidget *parent = nullptr);
    ~WarrantyInguiryLayout();
    void init() override;

private slots:
    void on_addButton_clicked();

    void on_deleteButton_clicked();

    void on_updateButton_clicked();

    void searchSlots();

    void on_selectButton_clicked();

    void on_lineEdit_returnPressed();

private:
    Ui::WarrantyInguiryLayout *ui;
    MyEditableRowModel *model; // Model for the table view
    QVector<QVector<QString>> warrantyData; // 用于存储质保数据
    // 线程安全锁
    QMutex dataMutex;
    bool addOk = false; // 状态标志
    bool deleteOk = false; // 状态标志
    bool updateOk = false; // 状态标志
    QString currentId; // 当前操作的质保ID
};
#endif // WARRANTYINGUIRYLAYOUT_H
