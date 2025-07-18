#include "parkinglease.h"
#include "ui_parkinglease.h"
#include <QDebug>
#include <QStandardItemModel> // For table view model
#include <QMenu>
#include "lib/sqlmanager.h" // For SQL operations
#include "showtool/errortoast.h"
#include <QHeaderView> // For header view
#include <QMessageBox>


ParkingLease::ParkingLease(QWidget *parent)
    : MyWidget(parent)
    , ui(new Ui::ParkingLease)
    , model(new MyEditableRowModel(this)) // Initialize the model
{
    ui->setupUi(this);
    ui->tableView_ParkingLease->setModel(model); // Set the model to the table view
    // Set the number of columnsId（车位id） 位置 类型（是否可充电）  是否租出去 userid  租金
    model->setColumnCount(7); // Set the number of columns
    model->setHeaderData(0, Qt::Horizontal, "ID");
    model->setHeaderData(1, Qt::Horizontal, "车位id");
    model->setHeaderData(2, Qt::Horizontal, "用户id");
    model->setHeaderData(3, Qt::Horizontal, "车位位置");
    model->setHeaderData(4, Qt::Horizontal, "类型（是否可充电）");
    model->setHeaderData(5, Qt::Horizontal, "租赁状态");
    model->setHeaderData(6, Qt::Horizontal, "租金");
    ui->tableView_ParkingLease->horizontalHeader()->setVisible(true);
    ui->tableView_ParkingLease->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView_ParkingLease->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableView_ParkingLease->verticalHeader()->setDefaultSectionSize(40);
    ui->tableView_ParkingLease->setSelectionBehavior(QAbstractItemView::SelectRows);   // 选择整行


}

ParkingLease::~ParkingLease()
{
    delete ui;
}

void ParkingLease::init()
{
    //记录刷新前的焦点行 没有则定义为-1
    QModelIndex currentIndex = ui->tableView_ParkingLease->currentIndex();
    int currentRow = currentIndex.isValid() ? currentIndex.row() : -1;
    //加锁
    QMutexLocker locker(&dataMutex);
    // 获取数据
    leaseData = SqlManager::GetInstance().GetParkingLeaseData();

    // 清空现有数据
    model->removeRows(0, model->rowCount());

    // 填充表格
    for (int i = 0; i < leaseData.size(); ++i) {
        const auto& rowData = leaseData[i];

        // 确保有足够的列数据
        QList<QStandardItem*> items;
        for (const auto& data : rowData) {
            items.append(new QStandardItem(data));
        }
        model->appendRow(items);
    }

    // 恢复焦点行
    if (currentRow >= 0 && currentRow < model->rowCount()) {
        ui->tableView_ParkingLease->setCurrentIndex(model->index(currentRow, 0));
    }
}

void ParkingLease::on_addButton_clicked()
{
    if(addOk==0){
        //调用函数禁用计时器
        stopRefreshTimer();
        //禁用其他按钮
        ui->addButton->setEnabled(1);
        ui->rentButton->setEnabled(0);
        ui->cheakButton->setEnabled(0);
        //增加一行 空白
        QList<QStandardItem*> newRow;
        newRow.append(new QStandardItem("")); // 车位ID
        newRow.append(new QStandardItem("")); // 用户ID
        newRow.append(new QStandardItem("")); // 用户姓名
        newRow.append(new QStandardItem("")); // 车位
        newRow.append(new QStandardItem("")); // 车的类型（可否充电）
        newRow.append(new QStandardItem("")); // 租赁状态
        newRow.append(new QStandardItem("")); // 租金
        model->appendRow(newRow); // 添加新行到模型
        addOk=1;
        //把add名字改成确认添加
        ui->addButton->setText("确认添加");
        //数据可编辑
        model->setEditableRow(model->rowCount() - 1); // 设置最后一行可编辑
    }
    else{
        //重置addOk状态
        addOk = 0;
        //把按钮名该回去
        ui->addButton->setText("添加");
        //获取新一行的数据
        QList<QStandardItem*> items = model->takeRow(model->rowCount() - 1); // 获取最后一行数据
        //禁用编辑
        model->clearEditableRow(); // 清除可编辑行
        //启用禁用的按钮
        ui->addButton->setEnabled(1);
        ui->rentButton->setEnabled(1);
        ui->cheakButton->setEnabled(1);
        //启用计时器
        startRefreshTimer();
        //获取数据
        QVector<QString> newLeaseData;
        for (int i = 0; i < items.size(); ++i) {
            newLeaseData.append(items[i]->text());
        }
        // 添加到数据库
        if (SqlManager::GetInstance().AddParkingLeaseData(newLeaseData)) {
            ErrorToast *toast = ErrorToast::instance(this);
            toast->showToast(tr("添加成功"), 0,0,ErrorToast::SuccessIcon);
            qDebug() << "停车位租赁数据添加成功，ID:" << newLeaseData[0];
            // 重新加载数据
            init();
        } else {
            ErrorToast *toast = ErrorToast::instance(this);
            toast->showToast(tr("添加失败"), 0,0,ErrorToast::WarningIcon);
            qDebug() << "停车位租赁数据添加失败，ID:" << newLeaseData[0];
        }
    }
}



void ParkingLease::on_rentButton_clicked()
{
    //获取选中焦点行将其标记为“出租中”
    QModelIndex currentIndex = ui->tableView_ParkingLease->currentIndex();
    if (!currentIndex.isValid()) {
        qDebug() << "请先选择一行";
        return; // 如果没有选中行，直接返回
    }
    int row = currentIndex.row(); // 获取当前行号
    //获取当前行的租赁状态
    QString currentStatus = model->data(model->index(row, 5)).toString();
    //加锁
    QMutexLocker locker(&dataMutex);
    if (currentStatus == "出租中"||currentStatus == "已被使用") {
        qDebug() << "该车位已出租，请勿重复操作";
        //抛出errortoast（单例）
        ErrorToast *toast = ErrorToast::instance(this);
        toast->showToast(tr("该车位已出租，请勿重复操作"), 0,0,ErrorToast::WarningIcon);

        return; // 如果已经出租，直接返回
    }
    //如果焦点行不是“未出租”，则弹出错误提示框
    if (currentStatus != "未出租") {
        qDebug() << "该车位状态异常，请联系管理员";
        return; // 如果状态异常，直接返回
    }
    //获取当前行的车位ID
    QString currentId = model->data(model->index(row, 0)).toString(); // 获取车位ID
    SqlManager::GetInstance().UpdateParkingLeaseData(currentId); // 更新数据库中的租赁状态

}


void ParkingLease::on_cheakButton_clicked()
{
    searchSlots();
}

void ParkingLease::searchSlots()
{
    // 获取搜索文本
    QString searchText = ui->lineEdit->text().trimmed(); // 去除首尾空格
    if (searchText.isEmpty()) {
        qDebug() << "搜索文本不能为空";
        return; // 如果搜索文本为空，直接返回
    }

    // 加锁
    QMutexLocker locker(&dataMutex);
    //获取焦点行
    QModelIndex currentIndex = ui->tableView_ParkingLease->currentIndex();
    //寻找第一个在焦点行下面包含搜索文本的行 如果没找到就将第一行设置为焦点行
    int rowCount = model->rowCount();
    for (int i = currentIndex.row() + 1; i < rowCount; ++i) {
        QString id = model->data(model->index(i, 0)).toString();
        QString ownerName = model->data(model->index(i, 1)).toString();
        if (id.contains(searchText, Qt::CaseInsensitive) || ownerName.contains(searchText, Qt::CaseInsensitive)) {
            ui->tableView_ParkingLease->setCurrentIndex(model->index(i, 0)); // 设置焦点行
            return; // 找到后直接返回
        }
    }
    // 如果没有找到，则将第一行设置为焦点行
    if (rowCount > 0) {
        ui->tableView_ParkingLease->setCurrentIndex(model->index(0, 0)); // 设置第一行为焦点行
        qDebug() << "没有找到包含搜索文本的行，已将第一行设置为焦点行";
    } else {
        qDebug() << "表格为空，没有可设置的焦点行";
    }

}


void ParkingLease::on_lineEdit_returnPressed()
{
    searchSlots();
}


void ParkingLease::on_pushButton_clicked()
{
    //获得一个未分配的车位的数据 把它分给等待时间最长的用户
    QString userId = SqlManager::GetInstance().GetEarliestTimeUserId();
    qDebug()<<userId;
    if (userId.isEmpty()) {
        //抛出errortoast（单例）
        ErrorToast *toast = ErrorToast::instance(this);
        toast->showToast(tr("未找到等待时间最长的用户"), 0,0,ErrorToast::WarningIcon);
        qDebug() << "未找到等待时间最长的用户";
        return;
    }
    // 从数据库中获取未分配的车位数据
    QVector<QVector<QString>> unassignedData = SqlManager::GetInstance().GetParkingLeaseData();
    if (unassignedData.isEmpty()) {
        //抛出errortoast（单例）
        ErrorToast *toast = ErrorToast::instance(this);
        toast->showToast(tr("未找到未分配的车位数据"), 0,0,ErrorToast::WarningIcon);
        qDebug() << "未找到未分配的车位数据";
        return;
    }
    // 分配车位给用户
     // 获取第一个未分配且状态为“已出租”且用户ID为空的车位ID
    QString parkingId;
    for (int i = 0; i < unassignedData.size(); ++i) {
        if (unassignedData[i][5] == "已出租" && unassignedData[i][2].isEmpty()) {
            parkingId = unassignedData[i][1];
            break;
        }
    }
    if (parkingId.isEmpty()) {
        //抛出errortoast（单例）
        ErrorToast *toast = ErrorToast::instance(this);
        toast->showToast(tr("未找到未分配的车位数据"), 0,0,ErrorToast::WarningIcon);
        qDebug() << "未找到未分配的车位数据";
        return;
    }
    if (SqlManager::GetInstance().AssignParkingToUser(userId, parkingId)) {
    } else {
        // 分配失败，处理错误
        //抛出errortoast（单例）
        ErrorToast *toast = ErrorToast::instance(this);
        toast->showToast(tr("车位分配失败"), 0,0,ErrorToast::WarningIcon);
        qDebug() << "车位分配失败";
    }
    //抛出成功toast（单例）
    ErrorToast *toast = ErrorToast::instance(this);
    toast->showToast(tr("车位分配成功"), 0,0,ErrorToast::SuccessIcon);

}

