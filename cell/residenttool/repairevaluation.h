#ifndef REPAIREVALUATION_H
#define REPAIREVALUATION_H

#include <QWidget>
#include "ui_ownermanagementitem.h"
#include "lib/sqlmanager.h" // For SQL operations
#include <QStandardItemModel> // For table view model
#include <QHeaderView> // For header view
#include "showtool/myeditablerowmodel.h"
#include <QMutex> // For thread safety
#include "showtool/mywidget.h" // Assuming this is a custom widget base class

namespace Ui {
class RepairEvaluation;
}

class RepairEvaluation : public MyWidget
{
    Q_OBJECT

public:
    explicit RepairEvaluation(QWidget *parent = nullptr);
    ~RepairEvaluation();
    void init() override;

private slots:
    void on_pushButton_clicked();

private:
    Ui::RepairEvaluation *ui;
    MyEditableRowModel *model; // Model for the table view
    QVector<QVector<QString>> repairData; // 用于存储维修数据;
    // 线程安全锁
    QMutex dataMutex;
};

#endif // REPAIREVALUATION_H
