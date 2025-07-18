#ifndef PARKINGAPPLICATION_H
#define PARKINGAPPLICATION_H

#include <QWidget>
#include "showtool/mywidget.h"

namespace Ui {
class ParkingApplication;
}

class ParkingApplication : public MyWidget
{
    Q_OBJECT

public:
    explicit ParkingApplication(QWidget *parent = nullptr);
    ~ParkingApplication();

private slots:
    void on_pushButton_clicked();

private:
    Ui::ParkingApplication *ui;
};

#endif // PARKINGAPPLICATION_H
