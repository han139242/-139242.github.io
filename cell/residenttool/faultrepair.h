#ifndef FAULTREPAIR_H
#define FAULTREPAIR_H

#include <QWidget>
#include "showtool/mywidget.h"

namespace Ui {
class FaultRepair;
}

class FaultRepair : public MyWidget
{
    Q_OBJECT

public:
    explicit FaultRepair(QWidget *parent = nullptr);
    ~FaultRepair();



private slots:
    void on_submissionButton_clicked();

    void on_deleteButton_clicked();

private:
    Ui::FaultRepair *ui;
};

#endif // FAULTREPAIR_H
