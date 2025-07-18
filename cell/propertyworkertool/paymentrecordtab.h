#ifndef PAYMENTRECORDTAB_H
#define PAYMENTRECORDTAB_H

#include <QWidget>
#include <QStandardItemModel> // For table view model
#include "showtool/mywidget.h" // Assuming this is the base class for your widgets
#include "showtool/myeditablerowmodel.h" // For editable row model
#include <QHeaderView> // For header view
#include <QMutexLocker> // For thread-safe operations

QT_BEGIN_NAMESPACE
namespace Ui {
class PaymentRecordTab;
}
QT_END_NAMESPACE

class PaymentRecordTab : public MyWidget
{
    Q_OBJECT

public:
    PaymentRecordTab(QWidget *parent = nullptr);
    ~PaymentRecordTab();
    void init() override;


private slots:
    void on_pushButton_4_clicked();

    void searchSlots();

    void on_lineEdit_returnPressed();

    void on_pushButton_clicked();

private:
    Ui::PaymentRecordTab *ui;
    MyEditableRowModel *model; // Model for the table view
    QVector<QVector<QString>> paymentData; // Data for payment records
    QMutex dataMutex; // Mutex for thread-safe operations

};
#endif // PAYMENTRECORDTAB_H
