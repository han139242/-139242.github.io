#ifndef PAYMENTSUMSEE_H
#define PAYMENTSUMSEE_H

#include <QBarSet>              // 柱状图数据集
#include <QBarSeries>           // 柱状图序列
#include <QChart>               // 图表
#include <QChartView>           // 图表视图
#include <QBarCategoryAxis>     // 分类轴（用于X轴）
#include <QValueAxis>           // 数值轴（用于Y轴）
#include <QWidget>
#include "showtool/mywidget.h"
#include <QMutex>
#include "showtool/myeditablerowmodel.h"
#include "lib/sqlmanager.h" // For SQL operations
#include <QVector>
#include <QString>
#include <QDebug>
#include <QDate>
#include <QLabel>              // 用于显示无数据提示

namespace Ui {
class PaymentSumSee;
}

class PaymentSumSee : public MyWidget
{
    Q_OBJECT

public:
    explicit PaymentSumSee(QWidget *parent = nullptr);
    ~PaymentSumSee();

    void init() override;

    void initBarChart();// 初始化柱状图

    void showNoDataPrompt();// 显示无数据提示


public slots:
    void searchSlots();

private slots:
    void on_pushButton_2_clicked();

    void on_lineEdit_returnPressed();

    void on_pushButton_clicked();

private:
    Ui::PaymentSumSee *ui;
    QChart *m_chart;         // 图表对象
    QChartView *m_chartView; // 图表视图
    // 数据集（可选：如果需要在类中保留引用）
    QBarSet *m_setPropertyFee;    // 物业费数据集
    QBarSet *m_setWaterFee;       // 水费数据集
    QBarSet *m_setElectricityFee; // 电费数据集
    QBarSet *m_setParkingFee; // 车位租金数据集
    // 月份标签
    QVector<QString> m_months;
    MyEditableRowModel *model;
    QVector<QVector<QString>> paymentData; // 用于存储支付数据
    //QLabel
     QLabel *noDataLabel; // 用于显示无数据提示
    // 线程安全锁
    QMutex dataMutex; // 用于保护数据访问的互斥锁

};

#endif // PAYMENTSUMSEE_H
