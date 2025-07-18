#ifndef MAINTENANCEPROCESSTAB_H
#define MAINTENANCEPROCESSTAB_H

#include <QWidget>
#include <QStandardItemModel> // For table view model
#include "showtool/mywidget.h" // Assuming this is a custom widget base class
#include <QMutex> // For thread safety

QT_BEGIN_NAMESPACE
namespace Ui {
class MaintenanceProcessTab;
}
QT_END_NAMESPACE

class MaintenanceProcessTab : public MyWidget
{
    Q_OBJECT

public:
    MaintenanceProcessTab(QWidget *parent = nullptr);
    ~MaintenanceProcessTab();
    void init() override;

private slots:
    void on_pushButtonYES_clicked();

    void on_pushButtonNO_clicked();

    void on_pushButton_Select_clicked();

    void on_lineEdit_returnPressed();
    void searchSlots();

private:
    Ui::MaintenanceProcessTab *ui;
    QStandardItemModel *model; // Model for the table view
    QVector<QVector<QString>> maintenanceData; // 用于存储维修数据;

    // 线程安全锁
    QMutex dataMutex;

};
#endif // MAINTENANCEPROCESSTAB_H
