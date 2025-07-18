#include "maintenanceprocesstab.h"
#include "ui_maintenanceprocesstab.h"
#include "showtool/mywidget.h"
#include "lib/sqlmanager.h"
#include <QMutex>
#include <QStandardItemModel>
#include <QHeaderView>
#include "showtool/errortoast.h"


MaintenanceProcessTab::MaintenanceProcessTab(QWidget *parent)
    : MyWidget(parent)
    , ui(new Ui::MaintenanceProcessTab)
{
    ui->setupUi(this);
    model = new QStandardItemModel(this);
    ui->tableView->setModel(model);
    // 设置表格列数和标题
    model->setColumnCount(6); // 设置列数
    model->setHorizontalHeaderLabels({"维修人员ID", "维修人员姓名", "维修地点" ,"申请原因",  "维修物品","维修进程"});
    // 设置表格配置
    ui->tableView->horizontalHeader()->setVisible(true);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableView->verticalHeader()->setDefaultSectionSize(40); // 设置行高为30像素
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);   // 选择整行


}
void MaintenanceProcessTab::init()
{
    // 记录刷新前的焦点行 没有则定义为-1
    QModelIndex currentIndex = ui->tableView->currentIndex();
    int currentRow = currentIndex.isValid() ? currentIndex.row() : -1;

    // 加锁保护数据访问
    QMutexLocker locker(&dataMutex);

    // 获取维修数据
    maintenanceData = SqlManager::GetInstance().GetRepairProcessData();

    // 清空现有数据
    model->removeRows(0, model->rowCount());

    // 填充表格
    for (const auto& rowData : maintenanceData) {
        QList<QStandardItem*> items;
        for (const auto& text : rowData) {
            items << new QStandardItem(text);
        }
        model->appendRow(items); // 添加新行到模型
    }

    // 恢复焦点行
    if (currentRow >= 0 && currentRow < model->rowCount()) {
        QModelIndex newIndex = model->index(currentRow, 0); // 获取当前行的第一个单元格
        ui->tableView->setCurrentIndex(newIndex); // 设置焦点到新索引
    }
}
MaintenanceProcessTab::~MaintenanceProcessTab()
{
    delete ui;

}

void MaintenanceProcessTab::on_pushButtonYES_clicked()
{
    // 获取当前选中的行
    QModelIndex currentIndex = ui->tableView->currentIndex();
    if (!currentIndex.isValid()) {
        qDebug() << "请先选择一行";
        return; // 如果没有选中行，直接返回
    }
    int row = currentIndex.row(); // 获取当前行号

    // 获取当前行的维修人员ID
    QString repairId = model->data(model->index(row, 0)).toString();
    QString repairman = model->data(model->index(row, 1)).toString();
    QString maintenanceLocation = model->data(model->index(row, 2)).toString();
    QString repairedItems = model->data(model->index(row, 3)).toString();
    QString submissionReason = model->data(model->index(row, 4)).toString();
    //通过id,repairman,maintenanceLocation,repairedItems,submissionReason查询数据库中的newid
    QString newId = SqlManager::GetInstance().GetNewIdFromRepairProcess(repairId, repairman, maintenanceLocation, submissionReason,repairedItems);
    qDebug() << "获取到的维修进程ID:" << newId;
    // 更新维修进程状态为“已完成”
    if (SqlManager::GetInstance().UpdateRepairProcessStatus(newId, "已完成")) {
        qDebug() << "维修进程更新成功，ID:" << newId;
        ErrorToast *toast = ErrorToast::instance(this);
        toast->showToast(tr("维修进程更新成功"), 0,0,ErrorToast::SuccessIcon);
    } else {
        qDebug() << "维修进程更新失败，ID:" << newId;
        ErrorToast *toast = ErrorToast::instance(this);
        toast->showToast(tr("维修进程更新失败"), 0,0,ErrorToast::WarningIcon);
    }
}


void MaintenanceProcessTab::on_pushButtonNO_clicked()
{
    // 获取当前选中的行
    QModelIndex currentIndex = ui->tableView->currentIndex();
    if (!currentIndex.isValid()) {
        qDebug() << "请先选择一行";
        return; // 如果没有选中行，直接返回
    }
    int row = currentIndex.row(); // 获取当前行号

    // 获取当前行的维修人员ID
    QString repairId = model->data(model->index(row, 0)).toString();
    QString repairman = model->data(model->index(row, 1)).toString();
    QString maintenanceLocation = model->data(model->index(row, 2)).toString();
    QString repairedItems = model->data(model->index(row, 3)).toString();
    QString submissionReason = model->data(model->index(row, 4)).toString();
    //通过id查询数据库中的newid
    QString newId = SqlManager::GetInstance().GetNewIdFromRepairProcess(repairId, repairman, maintenanceLocation,  submissionReason,repairedItems);

    // 更新维修进程状态为“已拒绝”
    if (SqlManager::GetInstance().UpdateRepairProcessStatus(newId, "已拒绝")) {
        ErrorToast *toast = ErrorToast::instance(this);
        toast->showToast(tr("维修进程更新成功"), 0,0,ErrorToast::SuccessIcon);
        qDebug() << "维修进程更新成功，ID:" << newId;
    } else {
        ErrorToast *toast = ErrorToast::instance(this);
        toast->showToast(tr("维修进程更新失败"), 0,0,ErrorToast::WarningIcon);
        qDebug() << "维修进程更新失败，ID:" << newId;
    }
}


void MaintenanceProcessTab::on_pushButton_Select_clicked()
{
    searchSlots();
}


void MaintenanceProcessTab::on_lineEdit_returnPressed()
{
    searchSlots();
}

void MaintenanceProcessTab::searchSlots()
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

