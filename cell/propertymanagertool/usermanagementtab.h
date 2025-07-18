#ifndef USERMANAGEMENTTAB_H
#define USERMANAGEMENTTAB_H

#include <QWidget>
#include "showtool/mywidget.h"
#include "showtool/myeditablerowmodel.h"
#include <QMutex>
#include <QStandardItemModel>
#include <QDebug>


namespace Ui {
class UserManagementTab;
}

class UserManagementTab : public MyWidget
{
    Q_OBJECT

public:
    explicit UserManagementTab(QWidget *parent = nullptr);
    ~UserManagementTab();
    void init() override;

private slots:


    void on_clearButton_clicked();

    void searchSlots();

    void on_selectButton_clicked();

    void on_lineEdit_returnPressed();

    void on_pushButton_clicked();

private:
    Ui::UserManagementTab *ui;
    MyEditableRowModel *model; // 使用自定义的可编辑行模型
    QVector<QStringList> userData; // 存储User数据
    QString currentId; // 当前编辑的User ID
    QMutex dataMutex; // 用于保护数据访问的互斥锁
};

#endif // USERMANAGEMENTTAB_H
