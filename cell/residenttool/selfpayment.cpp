#include "selfpayment.h"
#include "ui_selfpayment.h"
#include "showtool/mywidget.h" // Assuming this is a custom widget base class
#include "showtool/myeditablerowmodel.h" // For editable row model
#include <QMutexLocker> // For thread-safe operations
#include <QHeaderView> // For header view
#include "lib/sqlmanager.h" // For SQL operations
#include <QMessageBox>
#include "showtool/errortoast.h"

SelfPayment::SelfPayment(QWidget *parent)
    : MyWidget(parent)
    , ui(new Ui::SelfPayment)
{
    ui->setupUi(this);
    model = new MyEditableRowModel(this); // Initialize the model
    ui->tableView->setModel(model); // Set the model to the table view
    model->setColumnCount(5); // Set the number of columns
    model->setHeaderData(0, Qt::Horizontal, "业主ID");
    model->setHeaderData(1, Qt::Horizontal, "缴费类型");
    model->setHeaderData(2, Qt::Horizontal, "缴费时间");
    model->setHeaderData(3, Qt::Horizontal, "费用金额");
    model->setHeaderData(4, Qt::Horizontal, "缴费状态");
    // Set the visibility of the header
    ui->tableView->horizontalHeader()->setVisible(true);
    // Set the header to auto-adjust
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // Set the row height
    ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableView->verticalHeader()->setDefaultSectionSize(40); // Set row height to 30 pixels
    // Select whole row
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows); // Set row selection behavior
}

SelfPayment::~SelfPayment()
{
    delete ui;
}

void SelfPayment::init()
{
    //记录刷新前的焦点行 没有则定义为-1
    QModelIndex currentIndex = ui->tableView->currentIndex();
    int currentRow = currentIndex.isValid() ? currentIndex.row() : -1;

    // 加锁保护数据访问
    QMutexLocker locker(&dataMutex);

    // 获取缴费数据
    paymentData = SqlManager::GetInstance().GetPaymentData_1(StaticUserId);

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
        ui->tableView->setCurrentIndex(model->index(currentRow, 0));
    }
}

void SelfPayment::searchSlots()
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

void SelfPayment::on_pushButton_2_clicked()
{
    searchSlots();
}


void SelfPayment::on_lineEdit_returnPressed()
{
    searchSlots();
}


void SelfPayment::on_pushButton_clicked()
{
    // 检查是否有选中行
    QModelIndexList selectedIndexes = ui->tableView->selectionModel()->selectedRows();
    if (selectedIndexes.isEmpty()) {
        qDebug() << "没有选中任何行";
        return; // 如果没有选中行，直接返回
    }
    // 获取选中的行索引
    int row = selectedIndexes.first().row();
    //获取选中行的业主id
    QString userid = model->data(model->index(row, 0)).toString(); // 获取第一列的ID
    //获取选中行的缴费类型
    QString paymentType = model->data(model->index(row, 1)).toString(); // 获取第二列的ID
    //获取选中行的缴费时间
    QString paymentTime = model->data(model->index(row, 2)).toString(); // 获取第三列的ID
    //获取选中行的费用金额
    QString paymentAmount = model->data(model->index(row, 3)).toString(); // 获取第四列的ID
    //获取选中行的缴费状态
    QString paymentStatus = model->data(model->index(row, 4)).toString(); // 获取第五列的ID

    //通过选中行业主id从数据库中查找id
    QString id= SqlManager::GetInstance().GetPaymentIdByUserId(userid,paymentType,paymentAmount,paymentStatus);


    // 在数据库中更新该ID对应的缴费记录状态为已缴费
    if (SqlManager::GetInstance().UpdateSelfPaymentRecordData(id)) {
        ErrorToast *toast = ErrorToast::instance(this);
        toast->showToast(tr("缴费成功!"), 0,0,ErrorToast::SuccessIcon);
        qDebug() << "缴费记录更新成功，ID:" << id;
    } else {
        ErrorToast *toast = ErrorToast::instance(this);
        toast->showToast(tr("缴费记录更新失败!"), 0,0,ErrorToast::WarningIcon);
        qDebug() << "缴费记录更新失败，ID:" << id;
    }
}

