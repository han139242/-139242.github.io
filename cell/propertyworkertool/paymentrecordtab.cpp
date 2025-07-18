#include "paymentrecordtab.h"
#include "ui_paymentrecordtab.h"
#include <QStandardItemModel> // For table view model
#include <QHeaderView> // For header view
#include "showtool/mywidget.h" // Assuming this is the base class for your widgets
#include "showtool/myeditablerowmodel.h" // For editable row model
#include "lib/sqlmanager.h" // For SQL operations
#include "showtool/errortoast.h"
#include "showtool/QxlsxOut.h"

PaymentRecordTab::PaymentRecordTab(QWidget *parent)
    : MyWidget(parent)
    , ui(new Ui::PaymentRecordTab)
{
    ui->setupUi(this);
    model = new MyEditableRowModel(this); // Initialize the model
    ui->tableView_Pay->setModel(model); // Set the model to the table view
    // Set the number of columns: 业主ID, 费用类型, 费用金额, 缴费时间
    model->setColumnCount(4); // Set the number of columns
    model->setHeaderData(0, Qt::Horizontal, "业主ID");
    model->setHeaderData(1, Qt::Horizontal, "费用类型");
    model->setHeaderData(2, Qt::Horizontal, "费用金额");
    model->setHeaderData(3, Qt::Horizontal, "缴费时间");
    // Set the visibility of the header
    ui->tableView_Pay->horizontalHeader()->setVisible(true);
    // Set the header to auto-adjust
    ui->tableView_Pay->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // Set the row height
    ui->tableView_Pay->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableView_Pay->verticalHeader()->setDefaultSectionSize(40); // Set row height to 30 pixels
    // Add some example data
    QList<QStandardItem*> row1;
    row1.append(new QStandardItem("1001"));
    row1.append(new QStandardItem("物业费"));
    row1.append(new QStandardItem("200"));
    row1.append(new QStandardItem("2023-10-01"));
    model->appendRow(row1);
    // Set the table to be non-editable but allow copying
    ui->tableView_Pay->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView_Pay->setSelectionMode(QAbstractItemView::SingleSelection); // Set single selection mode
    ui->tableView_Pay->setSelectionBehavior(QAbstractItemView::SelectRows); // Set row selection behavior


}

PaymentRecordTab::~PaymentRecordTab()
{
    delete ui;
}

void PaymentRecordTab::init()
{
    //获取焦点行
    QModelIndex currentIndex = ui->tableView_Pay->currentIndex();
    int currentRow = currentIndex.isValid() ? currentIndex.row() : -1;

    // 加锁保护数据访问
    QMutexLocker locker(&dataMutex);
    // 获取支付记录数据
    paymentData = SqlManager::GetInstance().GetPaymentRecordData();

    // 清空现有数据
    model->removeRows(0, model->rowCount());

    // 填充表格
    for (const auto& rowData : paymentData) {
        // 确保有足够的列数据
        QList<QStandardItem*> items;
        for (int j = 0; j < rowData.size(); j++) {
            items << new QStandardItem(rowData[j]);
        }
        model->appendRow(items); // 添加行到模型
    }

    //恢复焦点行
    if (currentRow >= 0 && currentRow < model->rowCount()) {
        ui->tableView_Pay->setCurrentIndex(model->index(currentRow, 0)); // 恢复焦点行
    } else {
        ui->tableView_Pay->setCurrentIndex(model->index(0, 0)); // 如果没有焦点行，设置第一行为焦点行
    }
}

void PaymentRecordTab::searchSlots()
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
    QModelIndex currentIndex = ui->tableView_Pay->currentIndex();
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
                ui->tableView_Pay->setCurrentIndex(index);
                return; // 找到后直接返回
            }
        }
    }
    // 如果没有找到，则将第一行设置为焦点行
    if (rowCount > 0) {
        ui->tableView_Pay->setCurrentIndex(model->index(0, 0)); // 设置第一行为焦点行
        qDebug() << "没有找到包含搜索文本的行，已将第一行设置为焦点行";
    } else {
        qDebug() << "表格为空，没有可设置的焦点行";
    }

}

void PaymentRecordTab::on_pushButton_4_clicked()
{
    searchSlots();
}
void PaymentRecordTab::on_lineEdit_returnPressed()
{
    searchSlots();
}


void PaymentRecordTab::on_pushButton_clicked()
{
    //调用Qxlsx中的函数 导出excel
    //传入一个QStringList& headers
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

