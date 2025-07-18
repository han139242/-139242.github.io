#ifndef REPAIRPROCESS_H
#define REPAIRPROCESS_H

#include <QWidget>
#include "showtool/mywidget.h" // Assuming this is a custom widget base class
#include <QMutex> // For thread safety
#include <QStandardItemModel> // For table view model

namespace Ui {
class RepairProcess;
}

class RepairProcess : public MyWidget
{
    Q_OBJECT

public:
    explicit RepairProcess(QWidget *parent = nullptr);
    ~RepairProcess();
    void init() override;

private:
    Ui::RepairProcess *ui;
    QStandardItemModel *model; // Model for the table view
    QVector<QVector<QString>> repairData; // 用于存储维修数据
    QMutex dataMutex; // 线程安全锁，用于保护数据访问

};

#endif // REPAIRPROCESS_H
