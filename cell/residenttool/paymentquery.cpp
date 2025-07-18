#include "paymentquery.h"
#include "ui_paymentquery.h"
#include "showtool/mywidget.h" // Assuming this is a custom widget base class
#include "showtool/myeditablerowmodel.h" // For editable row model
#include <QMutexLocker> // For thread-safe operations
#include <QHeaderView> // For header view
#include <QStandardItemModel> // For table view model
#include "lib/sqlmanager.h" // For SQL operations


PaymentQuery::PaymentQuery(QWidget *parent)
    : MyWidget(parent)
    , ui(new Ui::PaymentQuery)
{
    ui->setupUi(this);
    model = new MyEditableRowModel(this); // Initialize the model
    ui->tableView->setModel(model); // Set the model to the table view
    // Set the number of columns: 业主id 缴费时间 费用金额
    model->setColumnCount(4); // Set the number of columns
    model->setHeaderData(0, Qt::Horizontal, "业主ID");
    model->setHeaderData(1, Qt::Horizontal, "缴费类型");
    model->setHeaderData(2, Qt::Horizontal, "缴费时间");
    model->setHeaderData(3, Qt::Horizontal, "费用金额");
    // Set the visibility of the header
    ui->tableView->horizontalHeader()->setVisible(true);
    // Set the header to auto-adjust
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // Set the row height
    ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableView->verticalHeader()->setDefaultSectionSize(40); // Set row height to 30 pixels
    //选中整行
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows); // Set row selection behavior

}

PaymentQuery::~PaymentQuery()
{
    delete ui;
}

void PaymentQuery::init()
{
    //记录刷新前的焦点行 没有则定义为-1
    QModelIndex currentIndex = ui->tableView->currentIndex();
    int currentRow = currentIndex.isValid() ? currentIndex.row() : -1;

    // 加锁保护数据访问
    QMutexLocker locker(&dataMutex);

    // 获取缴费数据
    paymentData = SqlManager::GetInstance().GetPaymentData(StaticUserId);

    // 清空现有数据
    model->removeRows(0, model->rowCount());

    // 填充表格
    for (const auto& rowData : paymentData) {
        QList<QStandardItem*> items;
        for (const auto& text : rowData) {
            items << new QStandardItem(text);
        }
        model->appendRow(items); // 添加新行到模型
    }
    // 恢复焦点行
    if (currentRow >= 0 && currentRow < model->rowCount()) {
        QModelIndex newIndex = model->index(currentRow, 0); // 获取当前行的第一个单元格
        ui->tableView->setCurrentIndex(newIndex); // 设置焦点到该行
    } else {
        ui->tableView->clearSelection(); // 如果没有有效行，清除选择
    }
}

void PaymentQuery::searchSlots()
{
    // 获取搜索文本
    QString searchText = ui->lineEdit->text().trimmed(); // 去除首尾空格
    //遍历tableview中所有文本
    if (searchText.isEmpty()) {
        // 如果搜索文本为空，重新加载所有数据
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



void PaymentQuery::on_pushButton_clicked()
{
    searchSlots(); // 调用搜索槽函数
}


void PaymentQuery::on_lineEdit_returnPressed()
{
    searchSlots(); // 调用搜索槽函数
}

