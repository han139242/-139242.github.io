#ifndef NOTICEBOUND_H
#define NOTICEBOUND_H

#include <QWidget>
#include "showtool/mywidget.h"

namespace Ui {
class NoticeBound;
}

class NoticeBound : public MyWidget
{
    Q_OBJECT

public:
    explicit NoticeBound(QWidget *parent = nullptr);
    ~NoticeBound();

private:
    Ui::NoticeBound *ui;
    //重写init
    void init() override;
};

#endif // NOTICEBOUND_H
