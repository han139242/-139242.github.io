#include "ownermanagementitem.h"
#include "ui_ownermanagementitem.h"
#include "lib/sqlmanager.h" // For SQL operations
#include <QStandardItemModel> // For table view model
#include <QHeaderView> // For header view
#include "showtool/errortoast.h"
#include "showtool/QxlsxOut.h"


OwnerManagementItem::OwnerManagementItem(QWidget *parent)
    : MyWidget(parent)
    , ui(new Ui::OwnerManagementItem)
    ,model(new MyEditableRowModel(this)) // Initialize the model
{
    ui->setupUi(this);
    ui->tableView_Own->setModel(model); // Set the model to the table view
    // 业主id 业主姓名 房产信息 车位信息 联系方式 车牌号
    // Set the number of columns

    model->setColumnCount(6); // Set the number of columns
    model->setHeaderData(0, Qt::Horizontal, "业主ID");
    model->setHeaderData(1, Qt::Horizontal, "业主姓名");
    model->setHeaderData(2, Qt::Horizontal, "房产信息");
    model->setHeaderData(3, Qt::Horizontal, "车位信息");
    model->setHeaderData(4, Qt::Horizontal, "联系方式");
    model->setHeaderData(5, Qt::Horizontal, "车牌号");



    ui->tableView_Own->horizontalHeader()->setVisible(true);
    // Set the header to auto-adjust
    ui->tableView_Own->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // Set the row height
    ui->tableView_Own->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableView_Own->verticalHeader()->setDefaultSectionSize(40); // Set row height to 30 pixels
    // Add some example data


    ui->tableView_Own->setSelectionBehavior(QAbstractItemView::SelectRows);   // 选择整行
}
OwnerManagementItem::~OwnerManagementItem()
{
    delete ui;
}

void OwnerManagementItem::init()
{
    //记录刷新前的焦点行 没有则定义为-1
    QModelIndex currentIndex = ui->tableView_Own->currentIndex();
    int currentRow = currentIndex.isValid() ? currentIndex.row() : -1;
    //加锁
    QMutexLocker locker(&dataMutex);
    // 获取数据
    ownerData = SqlManager::GetInstance().GetOwnerData();

    // 清空现有数据
    model->removeRows(0, model->rowCount());

    // 填充表格
    for (int i = 0; i < ownerData.size(); ++i) {
        const auto& rowData = ownerData[i];

        // 确保有足够的列数据
        QList<QStandardItem*> items;
        for (int j = 0; j < 6; j++) {
            QString text = (j < rowData.size()) ? rowData[j] : "";
            items << new QStandardItem(text);
        }
        model->appendRow(items);
    }
    // 恢复焦点行
    if (currentRow >= 0 && currentRow < model->rowCount()) {
        QModelIndex newIndex = model->index(currentRow, 0); // 获取当前行的第一个单元格
        ui->tableView_Own->setCurrentIndex(newIndex); // 设置焦点到该行
    } else {
        ui->tableView_Own->clearSelection(); // 如果没有有效行，清除选择
    }
}

void OwnerManagementItem::on_AddButton_clicked() //添加
{
    if(addOk==0){
        //调用函数禁用计时器
        stopRefreshTimer();
        //禁用其他按钮
        ui->AddButton->setEnabled(1);
        ui->pushButton_2->setEnabled(0);
        ui->pushButton_3->setEnabled(0);
        ui->pushButton_4->setEnabled(0);
        //增加一行 空白
        QList<QStandardItem*> newRow;
        newRow.append(new QStandardItem("")); // 业主ID
        newRow.append(new QStandardItem("")); // 业主姓名
        newRow.append(new QStandardItem("")); // 房产信息
        newRow.append(new QStandardItem("")); // 车位信息
        newRow.append(new QStandardItem("")); // 联系方式
        newRow.append(new QStandardItem("")); // 车牌号
        model->appendRow(newRow); // 添加新行到模型
        addOk=1;
        //把add名字改成确认添加
        ui->AddButton->setText("确认添加");
        //数据可编辑
        model->setEditableRow(model->rowCount() - 1); // 设置最后一行可编辑
        return;
    }
    else{
        //重置addOk状态
        addOk = 0;
        //把按钮名该回去
        ui->AddButton->setText("添加");
        //获取新一行的数据
        QList<QStandardItem*> items = model->takeRow(model->rowCount() - 1); // 获取最后一行数据
        //禁用编辑
        model->clearEditableRow(); // 清除可编辑行
        //启用禁用的按钮
        ui->AddButton->setEnabled(1);
        ui->pushButton_2->setEnabled(1);
        ui->pushButton_3->setEnabled(1);
        ui->pushButton_4->setEnabled(1);
        //启用计时器
        startRefreshTimer();
        // 检查是否所有字段都已填写
        for (int i = 0; i < items.size(); ++i) {
            if (items[i]->text().isEmpty()) {
                qDebug() << "请填写所有字段";
                return; // 如果有空字段，直接返回
            }
        }
        //获取数据
        QVector<QString> newOwnerData;
        for (const auto& item : items) {
            newOwnerData.append(item->text()); // 获取每个单元格的文本
        }
        //把数据添加到数据库
        SqlManager::GetInstance().AddOwnerData(newOwnerData);
        ErrorToast *toast = ErrorToast::instance(this);
        toast->showToast(tr("添加成功"), 0,0,ErrorToast::SuccessIcon);
    }
}


void OwnerManagementItem::on_pushButton_3_clicked() //删除
{
    //获取当前tableview焦点行
    QModelIndex currentIndex = ui->tableView_Own->currentIndex();
    if (!currentIndex.isValid()) {
        qDebug() << "请先选择一行";
        return; // 如果没有选中行，直接返回
    }
    int row = currentIndex.row(); // 获取当前行号
    //加锁
    QMutexLocker locker(&dataMutex);
    // 获取当前行的id
    QString id = model->data(model->index(row, 0)).toString(); // 获取业主ID

    // 删除数据库中的业主数据
    if (SqlManager::GetInstance().DeleteOwnerData(id)) {
        ErrorToast *toast = ErrorToast::instance(this);
        toast->showToast(tr("删除成功"), 0,0,ErrorToast::SuccessIcon);
        qDebug() << "业主数据删除成功，ID:" << id;
    } else {
        ErrorToast *toast = ErrorToast::instance(this);
        toast->showToast(tr("添加成功"), 0,0,ErrorToast::WarningIcon);
        qDebug() << "业主数据删除失败，ID:" << id;
    }

}


void OwnerManagementItem::on_pushButton_2_clicked() //修改
{
    if(updateOk == 0) {
        //获取当前tableview焦点行
        QModelIndex currentIndex = ui->tableView_Own->currentIndex();

        //禁用计时器
        stopRefreshTimer();

        //禁用其他按钮
        ui->AddButton->setEnabled(0);
        ui->pushButton_2->setEnabled(1);
        ui->pushButton_3->setEnabled(0);
        ui->pushButton_4->setEnabled(0);

        //设置焦点行可编辑
        currentIndex = ui->tableView_Own->currentIndex();
        //把按钮名改成确认修改
        ui->pushButton_2->setText("确认修改");
        if (!currentIndex.isValid()) {
            qDebug() << "请先选择一行";
            return; // 如果没有选中行，直接返回
        }
        int row = currentIndex.row(); // 获取当前行号
        //加锁
        QMutexLocker locker(&dataMutex);
        // 设置当前行可编辑
        model->setEditableRow(row); // 设置当前行可编辑
        updateOk = 1; // 设置更新状态为已开始
        //业主id为当前行的第一个单元格
        currentId = model->data(model->index(row, 0)).toString(); // 获取业主ID
        qDebug() << "当前编辑的业主ID:" << currentId;
        return;

    }
    else
    {
        //获取当前tableview焦点行
        QModelIndex currentIndex = ui->tableView_Own->currentIndex();

        int row = currentIndex.row(); // 获取当前行号
        //把按钮名改回去
        ui->pushButton_2->setText("修改");
        //禁用编辑
        model->clearEditableRow(); // 清除可编
        //启用禁用的按钮
        ui->AddButton->setEnabled(1);
        ui->pushButton_2->setEnabled(1);

        ui->pushButton_3->setEnabled(1);
        ui->pushButton_4->setEnabled(1);
        //启用计时器
        startRefreshTimer();
        updateOk = 0; // 重置更新状态
        if (!currentIndex.isValid()) {
            qDebug() << "请先选择一行";
            return; // 如果没有选中行，直接返回
        }
        //获取数据
        QVector<QString> updatedData;
        for (int i = 0; i < model->columnCount(); ++i) {
            updatedData.append(model->data(model->index(row, i)).toString());
        }

        //更新数据库中的业主数据
        if (SqlManager::GetInstance().UpdateOwnerData(currentId, updatedData)) {
            ErrorToast *toast = ErrorToast::instance(this);
            toast->showToast(tr("更新成功"), 0,0,ErrorToast::SuccessIcon);
            qDebug() << "业主数据更新成功，ID:" << updatedData[0];
        } else {
            ErrorToast *toast = ErrorToast::instance(this);
            toast->showToast(tr("更新失败"), 0,0,ErrorToast::WarningIcon);
            qDebug() << "业主数据更新失败，ID:" << updatedData[0];
        }

    }

}

void OwnerManagementItem::searchSlots()
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
    QModelIndex currentIndex = ui->tableView_Own->currentIndex();
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
                ui->tableView_Own->setCurrentIndex(index);
                return; // 找到后直接返回
            }
        }
    }
    // 如果没有找到，则将第一行设置为焦点行
    if (rowCount > 0) {
        ui->tableView_Own->setCurrentIndex(model->index(0, 0)); // 设置第一行为焦点行
        qDebug() << "没有找到包含搜索文本的行，已将第一行设置为焦点行";
    } else {
        qDebug() << "表格为空，没有可设置的焦点行";
    }

}

void OwnerManagementItem::on_pushButton_4_clicked()
{
    //调用搜索槽函数
    searchSlots();
}


void OwnerManagementItem::on_lineEdit_returnPressed()
{
    // 当用户按下回车键时，调用搜索槽函数
    searchSlots();
}


void OwnerManagementItem::on_pushButton_clicked()
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

