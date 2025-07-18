#include "repairevaluation.h"
#include "ui_repairevaluation.h"
#include "lib/sqlmanager.h" // For SQL operations
#include "showtool/myeditablerowmodel.h"
#include <QMutexLocker>
#include <QDebug>
#include <QStandardItemModel>
#include <QHeaderView>
#include "showtool/mywidget.h" // Assuming this is a custom widget base class
#include "showtool/errortoast.h"

RepairEvaluation::RepairEvaluation(QWidget *parent)
    : MyWidget(parent)
    , ui(new Ui::RepairEvaluation)
{
    ui->setupUi(this);
    model = new MyEditableRowModel(this); // Initialize the model
    ui->tableView->setModel(model); // Set the model to the table view
    // 设置表格列数和标题
    model->setColumnCount(6); // Set the number of columns
    model->setHorizontalHeaderLabels({"维修人员ID", "维修人员姓名", "维修地点", "申请原因","维修物品", "评分"});
    // 设置表格配置
    ui->tableView->horizontalHeader()->setVisible(true);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableView->verticalHeader()->setDefaultSectionSize(40); // Set row height to 30 pixels
    //选择整行
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);   // 选择整行


}

RepairEvaluation::~RepairEvaluation()
{
    delete ui;
}

void RepairEvaluation::init()
{
    //记录刷新前的焦点行 没有则定义为-1
    QModelIndex currentIndex = ui->tableView->currentIndex();
    int currentRow = currentIndex.isValid() ? currentIndex.row() : -1;

    // 加锁保护数据访问
    QMutexLocker locker(&dataMutex);

    // 获取维修数据
    repairData = SqlManager::GetInstance().GetRepairEvaluationData();

    // 清空现有数据
    model->removeRows(0, model->rowCount());

    // 填充表格
    for (const auto& rowData : repairData) {
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

void RepairEvaluation::on_pushButton_clicked()
{
    // 检查是否有选中行
    QModelIndexList selectedIndexes = ui->tableView->selectionModel()->selectedRows();
    if (selectedIndexes.isEmpty()) {
        qDebug() << "没有选中任何行";
        return; // 如果没有选中行，直接返回
    }
    // 获取选中的行索引
    int row = selectedIndexes.first().row();
    //获取选中行的id
    QString id = model->data(model->index(row, 0)).toString(); // 获取第一列的ID
    //获取选中行的维修人员姓名
    QString repairPersonName = model->data(model->index(row, 1)).toString(); // 获取第二列的维修人员姓名
    //获取选中行的维修地点
    QString maintenanceLocation = model->data(model->index(row, 2)).toString(); // 获取第三列的维修地点
    //获取选中行的维修物品
    QString repairedItems = model->data(model->index(row, 3)).toString(); // 获取第四列的维修物品
    //获取选中行的申请原因
    QString submissionReason = model->data(model->index(row, 4)).toString(); // 获取第五列的申请原因
    //根据以上内容获取数据库中的newid
    QString newId = SqlManager::GetInstance().GetNewIdFromRepairProcess(id, repairPersonName, maintenanceLocation, submissionReason, repairedItems);
    //在数据库中此id对应的行中添加ui中comboBox的数据到数据库的score中
    QString score = ui->comboBox->currentText().trimmed(); // 获取评分
    //获取评分文本编辑器中的内容
    QString ownerEvaluation = ui->textEdit->toPlainText().trimmed(); // 获取评分文本编辑器中的内容

    if (score.isEmpty()) {
        qDebug() << "评分不能为空";
        return; // 如果评分为空，直接返回
    }
    if(ownerEvaluation.isEmpty()){
        qDebug() << "评分文本编辑器不能为空";
        return; // 如果评分文本编辑器为空，直接返回
    }
    // 加锁保护数据访问
    QMutexLocker locker(&dataMutex);
    // 更新数据库中的评分
    if (SqlManager::GetInstance().UpdateRepairEvaluationData(newId, score,ownerEvaluation, "已完成"))
    {
        qDebug() << "评分更新成功!";
        ErrorToast *toast = ErrorToast::instance(this);
        toast->showToast(tr("提交成功!"), 0,0,ErrorToast::SuccessIcon);
        //清除textEdit的数据
        ui->textEdit->clear(); // 清除评分文本编辑器
        // 清除下拉框选择
        ui->comboBox->setCurrentIndex(-1); // 清除下拉框选择

    } else {
        qDebug() << "评分更新失败";
    }



}

