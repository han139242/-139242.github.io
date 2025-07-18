#ifndef SELFPAYMENT_H
#define SELFPAYMENT_H

#include <QWidget>
#include "showtool/mywidget.h" // Assuming this is a custom widget base class
#include <QMutex> // For thread safety
#include <QStandardItemModel> // For table view model
#include "showtool/myeditablerowmodel.h" // For editable row model


namespace Ui {
class SelfPayment;
}

class SelfPayment : public MyWidget
{
    Q_OBJECT

public:
    explicit SelfPayment(QWidget *parent = nullptr);
    ~SelfPayment();
    void init() override; // Initialize the widget

public slots:
    void searchSlots(); // Slot for searching payment records

private slots:
    void on_pushButton_2_clicked();

    void on_lineEdit_returnPressed();

    void on_pushButton_clicked();

private:
    Ui::SelfPayment *ui;
    MyEditableRowModel *model; // Model for the table view
    QVector<QVector<QString>> paymentData; // Data for payment records
    QMutex dataMutex; // Mutex for thread-safe operations
};

#endif // SELFPAYMENT_H
