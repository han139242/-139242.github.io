#include "repairprocess.h"
#include "ui_repairprocess.h"
#include "showtool/mywidget.h" // Assuming this is a custom widget base class
#include <QStandardItemModel>
#include <QMutexLocker>
#include "lib/sqlmanager.h" // For SQL operations

RepairProcess::RepairProcess(QWidget *parent)
    : MyWidget(parent)
    , ui(new Ui::RepairProcess)
{
    ui->setupUi(this);
    // 初始化表格模型
    model = new QStandardItemModel(this);
    ui->tableView->setModel(model);
    // 设置表格列数和标题
    model->setColumnCount(6); // 设置列数
    model->setHorizontalHeaderLabels({"维修人员ID", "维修人员姓名", "维修地点", "申请原因", "维修物品", "维修进程"});
    // 设置表格配置
    ui->tableView->horizontalHeader()->setVisible(true);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableView->verticalHeader()->setDefaultSectionSize(40); // 设置行高为30像素
}

RepairProcess::~RepairProcess()
{
    delete ui;
}

void RepairProcess::init()
{
    // 记录刷新前的焦点行 没有则定义为-1
    QModelIndex currentIndex = ui->tableView->currentIndex();
    int currentRow = currentIndex.isValid() ? currentIndex.row() : -1;

    // 加锁保护数据访问
    QMutexLocker locker(&dataMutex);

    // 获取维修数据
    repairData = SqlManager::GetInstance().GetRepairProcessData();

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
        ui->tableView->setCurrentIndex(newIndex); // 设置焦点到新索引
    }
}
