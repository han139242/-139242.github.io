#ifndef PAYMENTQUERY_H
#define PAYMENTQUERY_H

#include <QWidget>
#include "showtool/mywidget.h" // Assuming this is a custom widget base class"
#include "showtool/myeditablerowmodel.h"
#include <QMutexLocker> // For thread-safe operations
#include <QHeaderView> // For header view
#include <QStandardItemModel> // For table view model

namespace Ui {
class PaymentQuery;
}

class PaymentQuery : public MyWidget
{
    Q_OBJECT

public:
    explicit PaymentQuery(QWidget *parent = nullptr);
    ~PaymentQuery();
    void init() override;
public slots:
    void searchSlots();


private slots:
    void on_pushButton_clicked();

    void on_lineEdit_returnPressed();

private:
    Ui::PaymentQuery *ui;
    MyEditableRowModel *model; // Model for the table view
    QVector<QVector<QString>> paymentData; // Data for payment records
    QMutex dataMutex; // Mutex for thread-safe operations
};

#endif // PAYMENTQUERY_H
