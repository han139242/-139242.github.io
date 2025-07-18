#ifndef UPDATENEWNOTICE_H
#define UPDATENEWNOTICE_H

#include <QWidget>
#include "showtool/mywidget.h"

namespace Ui {
class UpdateNewNotice;
}

class UpdateNewNotice : public MyWidget
{
    Q_OBJECT

public:
    explicit UpdateNewNotice(QWidget *parent = nullptr);
    ~UpdateNewNotice();

private slots:
    void on_pushButton_clicked();

private:
    Ui::UpdateNewNotice *ui;
};

#endif // UPDATENEWNOTICE_H
