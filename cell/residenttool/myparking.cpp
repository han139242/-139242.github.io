#include "myparking.h"
#include "ui_myparking.h"
#include "lib/sqlmanager.h" // For SQL operations
#include "showtool/myeditablerowmodel.h"
#include <QMutexLocker>
#include <QDebug>
#include <QStandardItemModel>
#include <QHeaderView>
#include "showtool/mywidget.h" // Assuming this is a custom widget base class
#include "showtool/errortoast.h"


MyParking::MyParking(QWidget *parent)
    : MyWidget(parent)
    , ui(new Ui::MyParking)
{
    ui->setupUi(this);
    model = new MyEditableRowModel(this); // Initialize the model
    ui->tableView->setModel(model); // Set the model to the table view
    // 设置表格列数和标题
    model->setColumnCount(4); // Set the number of columns
    model->setHorizontalHeaderLabels({"车位位置","当前使用人", "是否可充电", "租金"});
    // 设置表格配置
    ui->tableView->horizontalHeader()->setVisible(true);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableView->verticalHeader()->setDefaultSectionSize(40); // Set row height to 30 pixels
    //选择整行
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);   // 选择整行


}

MyParking::~MyParking()
{
    delete ui;
}

void MyParking::init()
{
    //记录刷新前的焦点行 没有则定义为-1
    QModelIndex currentIndex = ui->tableView->currentIndex();
    int currentRow = currentIndex.isValid() ? currentIndex.row() : -1;

    // 加锁保护数据访问
    QMutexLocker locker(&dataMutex);

    // 获取车位
    myParking = SqlManager::GetInstance().GetmyParking(StaticUserId);


    // 清空现有数据
    model->removeRows(0, model->rowCount());

    // 填充表格
    for (const auto& rowData : myParking) {
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
