#include "paymentsumsee.h"
#include "ui_paymentsumsee.h"
#include <QDebug>
#include "showtool/myeditablerowmodel.h"
#include "showtool/mywidget.h"
#include <QLabel>
#include <QGraphicsProxyWidget>
#include <QMutexLocker>
#include "lib/sqlmanager.h"
#include <QBarSet>
#include "showtool/QxlsxOut.h"
#include "showtool/errortoast.h"

// 构造函数
PaymentSumSee::PaymentSumSee(QWidget *parent)
    : MyWidget(parent)  // 继承自定义的MyWidget基类
    , ui(new Ui::PaymentSumSee)  // 创建UI界面对象
    , m_chart(new QChart())  // 创建图表对象
    , m_chartView(new QChartView(m_chart))  // 创建图表视图并关联图表（但未使用）
{
    ui->setupUi(this);  // 初始化UI界面
    // 初始化月份名称列表
    m_months = {"1月", "2月", "3月", "4月", "5月", "6月",
                "7月", "8月", "9月", "10月", "11月", "12月"};

    model = new MyEditableRowModel(this);  // 初始化可编辑行模型
    ui->tableView->setModel(model);  // 设置模型到表格视图
    // 设置表格列数和标题
    model->setColumnCount(6);  // 设置列数为6
    model->setHorizontalHeaderLabels({"userid", "姓名", "费用类型", "金额", "支付状态", "支付时间"});
    // 设置表格配置
    ui->tableView->horizontalHeader()->setVisible(true);  // 显示水平表头
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);  // 设置列宽自适应
    ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);  // 设置垂直表头固定
    ui->tableView->verticalHeader()->setDefaultSectionSize(40);  // 设置行高为30像素
    //选中一行
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);  // 设置单选模式
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);  // 设置行选中行为


}

// 析构函数
PaymentSumSee::~PaymentSumSee()
{
    delete ui;  // 删除UI对象
}

void PaymentSumSee::init()
{
    // 获取焦点行
    QModelIndex currentIndex = ui->tableView->currentIndex();
    int currentRow = currentIndex.isValid() ? currentIndex.row() : -1;

    // 加锁保护数据访问
    QMutexLocker locker(&dataMutex);
    // 获取支付数据
    paymentData = SqlManager::GetInstance().GetAllPaymentData();
    // 清空现有数据
    model->removeRows(0, model->rowCount());
    // 填充表格
    for (const auto& rowData : paymentData) {
        // 确保有足够的列数据
        QList<QStandardItem*> items;
        for (int j = 0; j < rowData.size(); j++) {
            items << new QStandardItem(rowData[j]);
        }
        model->appendRow(items);  // 添加行到模型
    }

    // 恢复焦点行
    if (currentRow >= 0 && currentRow < model->rowCount()) {
        QModelIndex newIndex = model->index(currentRow, 0);  // 获取当前行的第一个单元格
        ui->tableView->setCurrentIndex(newIndex);  // 设置焦点到该单元格
    }
    initBarChart();
}

void PaymentSumSee::initBarChart()
{
    // 清空之前的数据图表，并创建新的数据图表 共四个数据集，分别为“物业费”、“水费”、“电费”、“车位租金”
    m_chart->removeAllSeries();  // 移除之前的所有系列
    m_chart->axes(Qt::Horizontal).clear();  // 清除之前的X轴
    m_chart->axes(Qt::Vertical).clear();  // 清除之前的Y轴
    m_chart->setTitle("");  // 清除之前的标题
    //显式删除轴对象
    for (auto axis : m_chart->axes(Qt::Horizontal)) {
        delete axis;  // 删除X轴对象
    }
    for (auto axis : m_chart->axes(Qt::Vertical)) {
        delete axis;  // 删除Y轴对象
    }
    // 初始化数据集
    m_setPropertyFee = new QBarSet("物业费");  // 创建物业费数据集
    m_setWaterFee = new QBarSet("水费");  // 创建水费数据集
    m_setElectricityFee = new QBarSet("电费");  // 创建电费数据集
    m_setParkingFee = new QBarSet("车位租金");  // 创建车位租金数据集
    //根据数据库中paymentForm表中的paymentTime和payType获取数据，并且按payType分类，再将同一月份的数据相加，再导入到对应的柱状图数据集中
    QVector<double> propertyFees(12, 0);  // 初始化物业费数据集（12个月）
    QVector<double> waterFees(12, 0);     // 初始化水费数据集（12个月）
    QVector<double> electricityFees(12, 0); // 初始化电费数据集（12个月）
    QVector<double> parkingFees(12, 0);   // 初始化车位租金数据集（12个月）
    // 遍历支付数据，按月份分类并累加
    for (const auto& row : paymentData) {
        if (row.size() < 6) continue;  // 确保行数据完整
        QString payTime = row[5];  // 获取支付时间
        QString payType = row[2];   // 获取支付类型
        double amount = row[3].toDouble();  // 获取金额

        // 解析月份  如果月份为空或者无效，则跳过
        QDate date = QDate::fromString(payTime, "yyyy-MM-dd");
        if (!date.isValid()|| payTime.isEmpty()) {
            qDebug() << "无效的支付时间格式：" << payTime;
            continue;  // 跳过无效日期
        }
        int month = date.month() - 1;  // 获取月份（0-11）

        // 根据支付类型累加金额
        if (payType == "物业费") {
            propertyFees[month] += amount;
        } else if (payType == "水费") {
            waterFees[month] += amount;
        } else if (payType == "电费") {
            electricityFees[month] += amount;
        } else if (payType == "车位租金") {
            parkingFees[month] += amount;
        }
    }
    // 将累加的费用数据添加到对应的数据集中
    for (int i = 0; i < 12; ++i) {
        *m_setPropertyFee << propertyFees[i];  // 添加物业费数据
        *m_setWaterFee << waterFees[i];        // 添加水费数据
        *m_setElectricityFee << electricityFees[i]; // 添加电费数据
        *m_setParkingFee << parkingFees[i];    // 添加车位租金数据
    }
    // 如果所有数据集都为空，则显示无数据提示
    if (propertyFees.isEmpty() && waterFees.isEmpty() && electricityFees.isEmpty() && parkingFees.isEmpty()) {
        showNoDataPrompt();  // 显示无数据提示
        return;  // 退出函数
    } else {
        // 隐藏无数据提示
        if (noDataLabel) {
            noDataLabel->hide();
        }
    }
    // 如果没有数据集，则显示无数据提示
    if (m_setPropertyFee->count() == 0 && m_setWaterFee->count() == 0 &&
        m_setElectricityFee->count() == 0 && m_setParkingFee->count() == 0) {
        showNoDataPrompt();  // 显示无数据提示
        return;  // 退出函数
    }
    // 如果没有无数据提示标签，则创建一个
    if (!noDataLabel) {
        noDataLabel = new QLabel("暂无数据", this);  // 创建无数据提示标签
        noDataLabel->setAlignment(Qt::AlignCenter);  // 设置文本居中
        noDataLabel->setStyleSheet("font-size: 16px; color: red;");  // 设置样式
        noDataLabel->setGeometry(0, 0, 200, 50);  // 设置位置和大小
        QGraphicsProxyWidget *proxy = m_chart->scene()->addWidget(noDataLabel);  // 将标签添加到图表场景中
        proxy->setZValue(1);  // 设置标签在图表上的层级
    }
    // 如果有数据集，则隐藏无数据提示
    if (noDataLabel) {
        noDataLabel->hide();  // 隐藏无数据提示标签
    }

    // 3. 创建柱状图序列并添加数据集
    QBarSeries *series = new QBarSeries();  // 创建柱状图系列
    series->append(m_setPropertyFee);  // 添加物业费数据集
    series->append(m_setWaterFee);     // 添加水费数据集
    series->append(m_setElectricityFee); // 添加电费数据集
    series->append(m_setParkingFee);    // 添加车位租金数据集

    // 4. 将序列添加到图表
    m_chart->addSeries(series);  // 将系列添加到图表
    m_chart->setTitle("费用统计");  // 设置图表标题
    //m_chart->setAnimationOptions(QChart::SeriesAnimations);  // 启用系列动画

    // 5. 设置图例位置
    m_chart->legend()->setVisible(true);  // 显示图例
    m_chart->legend()->setAlignment(Qt::AlignBottom);  // 图例位于图表底部

    // 6. 设置X轴分类标签（月份）
    QBarCategoryAxis *axisX = new QBarCategoryAxis();  // 创建分类轴
    axisX->append(m_months);  // 添加月份标签
    axisX->setTitleText("月份");  // 设置X轴标题
    m_chart->addAxis(axisX, Qt::AlignBottom);  // 将X轴添加到图表底部
    series->attachAxis(axisX);  // 将系列关联到X轴

    // 7. 设置Y轴  动态计算最大值
    QValueAxis *axisY = new QValueAxis();  // 创建数值轴
    double maxValue = 0;  // 初始化最大值
    // 计算所有数据集中最大值
    for (const auto& set : {m_setPropertyFee, m_setWaterFee, m_setElectricityFee, m_setParkingFee}) {
        for (int i = 0; i < set->count(); ++i) {
            maxValue = qMax(maxValue, set->at(i));  // 更新最大值
        }
    }
    axisY->setRange(0, maxValue * 1.2);  // 设置Y轴范围为0到最大值的1.2倍
    axisY->setTitleText("金额（元）");  // 设置Y轴标题
    m_chart->addAxis(axisY, Qt::AlignLeft);  // 将Y轴添加到图表左侧
    series->attachAxis(axisY);  // 将系列关联到Y轴


    // 8. 配置图表视图
    // 设置抗锯齿渲染（但这里应为m_chartView或ui->graphicsView）
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);

    // 9. 设置柱状图样式（颜色）
    m_setPropertyFee->setColor(QColor(65, 105, 225));   // 物业费柱状图颜色（蓝色）
    m_setWaterFee->setColor(QColor(70, 130, 180));      // 水费柱状图颜色（钢蓝色）
    m_setElectricityFee->setColor(QColor(255, 140, 0)); // 电费柱状图颜色（橙色）
    m_setParkingFee->setColor(QColor(46, 139, 87));     // 车位租金柱状图颜色（绿色）

    //启用柱状图标签显示  将字体设置为红色并且要平行于Y轴放置
    QFont labelFont;
    labelFont.setPointSize(9);   // 设置字体大小
    labelFont.setBold(true);      // 设置字体为粗体
    labelFont.setItalic(false);   // 设置字体为非斜体

    // 为每个数据集设置标签字体和颜色
    m_setPropertyFee->setLabelFont(labelFont);
    m_setPropertyFee->setLabelColor(Qt::red);
    m_setWaterFee->setLabelFont(labelFont);
    m_setWaterFee->setLabelColor(Qt::red);
    m_setElectricityFee->setLabelFont(labelFont);
    m_setElectricityFee->setLabelColor(Qt::red);
    m_setParkingFee->setLabelFont(labelFont);
    m_setParkingFee->setLabelColor(Qt::red);
    // 在系列上设置标签位置和角度
    series->setLabelsVisible(true);
    series->setLabelsPosition(QAbstractBarSeries::LabelsOutsideEnd);
    series->setLabelsAngle(90);  // 设置标签角度为90度（平行于Y轴）
    series->setLabelsFormat("@value元");


    ui->graphicsView->setChart(m_chart);  // 将图表设置到图表视图中
}

void PaymentSumSee::showNoDataPrompt()
{
    // 如果没有数据，显示无数据提示
    if (!noDataLabel) {
        noDataLabel = new QLabel("暂无数据", this);  // 创建无数据提示标签
        noDataLabel->setAlignment(Qt::AlignCenter);  // 设置文本居中
        noDataLabel->setStyleSheet("font-size: 16px; color: red;");  // 设置样式
        noDataLabel->setGeometry(0, 0, 200, 50);  // 设置位置和大小
        QGraphicsProxyWidget *proxy = m_chart->scene()->addWidget(noDataLabel);  // 将标签添加到图表场景中
        proxy->setZValue(1);  // 设置标签在图表上的层级
    }
    noDataLabel->show();  // 显示无数据提示标签
}


void PaymentSumSee::searchSlots()
{
    // 获取搜索文本
    QString searchText = ui->lineEdit->text().trimmed(); // 去除首尾空格
    //遍历tableview中所有文本
    if (searchText.isEmpty()) {
        // 如果搜索文本为空，重新加载所有数据
        init();
        return;
    }
    // 加锁
    QMutexLocker locker(&dataMutex);
    //获取焦点行
    QModelIndex currentIndex = ui->tableView->currentIndex();
    //获取焦点行行数
    int currentRow = currentIndex.isValid() ? currentIndex.row() : -1;
    //寻找第一个在焦点行下面包含搜索文本的行 如果没找到就将第一行设置为焦点行
    int rowCount = model->rowCount();
    for (int i = currentRow+1; i < rowCount; ++i) {
        for(int p=0;p< model->columnCount(); ++p) {
            QModelIndex index = model->index(i, p);
            QString cellText = model->data(index).toString();
            if (cellText.contains(searchText, Qt::CaseInsensitive)) {
                // 如果找到匹配的文本，设置该行作为焦点行
                ui->tableView->setCurrentIndex(index);
                return; // 找到后直接返回
            }
        }
    }
    // 如果没有找到，则将第一行设置为焦点行
    if (rowCount > 0) {
        ui->tableView->setCurrentIndex(model->index(0, 0)); // 设置第一行为焦点行
        qDebug() << "没有找到包含搜索文本的行，已将第一行设置为焦点行";
    } else {
        qDebug() << "表格为空，没有可设置的焦点行";
    }
}


void PaymentSumSee::on_pushButton_2_clicked()
{
    searchSlots();
}


void PaymentSumSee::on_lineEdit_returnPressed()
{
    searchSlots();
}


void PaymentSumSee::on_pushButton_clicked()
{
    QStringList headers;
    for(int i=0;i<model->columnCount();++i) {
        headers.append(model->headerData(i, Qt::Horizontal).toString());
    }
    //传入一个const QVector<QVector<QVariant>>& data
    QVector<QVector<QVariant>> data;
    for(int i=0;i<model->rowCount();++i) {
        QVector<QVariant> row;
        for(int p=0;p<model->columnCount();++p) {
            row.append(model->data(model->index(i, p)).toString());
        }
        data.append(row);
    }
    exportDataToExcel(headers,data);
    //导出成功提示
    ErrorToast *toast = ErrorToast::instance(this);
    toast->showToast(tr("导出成功"), 0,0,ErrorToast::SuccessIcon);
    qDebug() << "导出成功";
}

