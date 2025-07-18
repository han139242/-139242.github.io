#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include "showtool/mywidget.h"

namespace Ui {
class CellWidget;
}

class loginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit loginDialog(QWidget *parent = nullptr);
    ~loginDialog();

private slots:
    void on_pushButton_clicked();

    void on_loginButton_clicked();

private:
    Ui::CellWidget *ui;
};

#endif // LOGINDIALOG_H
