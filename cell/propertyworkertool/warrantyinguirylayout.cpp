#include "warrantyinguirylayout.h"
#include "ui_warrantyinguirylayout.h"
#include "lib/sqlmanager.h" // For SQL operations
#include <QStandardItemModel> // For table view model
#include <QHeaderView> // For header view
#include "showtool/myeditablerowmodel.h"
#include "showtool/errortoast.h"


WarrantyInguiryLayout::WarrantyInguiryLayout(QWidget *parent)
    : MyWidget(parent)
    , ui(new Ui::WarrantyInguiryLayout)
{
    ui->setupUi(this);
    model = new MyEditableRowModel(this); // Initialize the model
    ui->tableView_War->setModel(model); // Set the model to the table view
    // Set the number of columns: ID,  保修物品, 保修结束时间
    model->setColumnCount(3); // Set the number of columns
    model->setHeaderData(0, Qt::Horizontal, "ID");
    model->setHeaderData(1, Qt::Horizontal, "保修物品");
    model->setHeaderData(2, Qt::Horizontal, "保修结束时间");
    // Set the visibility of the header
    ui->tableView_War->horizontalHeader()->setVisible(true);
    // Set the header to auto-adjust
    ui->tableView_War->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // Set the row height
    ui->tableView_War->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableView_War->verticalHeader()->setDefaultSectionSize(40); // Set row height to 30 pixels
    //选中一行
    ui->tableView_War->setSelectionMode(QAbstractItemView::SingleSelection); // Set single selection mode
    ui->tableView_War->setSelectionBehavior(QAbstractItemView::SelectRows); // Set row selection behavior



}

WarrantyInguiryLayout::~WarrantyInguiryLayout()
{
    delete ui;
}

void WarrantyInguiryLayout::init()
{
    // 记录刷新前的焦点行 没有则定义为-1
    QModelIndex currentIndex = ui->tableView_War->currentIndex();
    int currentRow = currentIndex.isValid() ? currentIndex.row() : -1;

    // 加锁保护数据访问
    QMutexLocker locker(&dataMutex);

    // 获取保修数据
    warrantyData = SqlManager::GetInstance().GetWarrantyData();

    // 清空现有数据
    model->removeRows(0, model->rowCount());

    // 填充表格
    for (const auto& rowData : warrantyData) {
        QList<QStandardItem*> items;
        for (const auto& text : rowData) {
            items << new QStandardItem(text);
        }
        model->appendRow(items); // 添加新行到模型
    }

    // 恢复焦点行
    if (currentRow >= 0 && currentRow < model->rowCount()) {
        QModelIndex newIndex = model->index(currentRow, 0); // 获取当前行的第一个单元格
        ui->tableView_War->setCurrentIndex(newIndex); // 设置焦点到新索引
    }
    else
    {
        ui->tableView_War->clearSelection(); // 如果没有有效行，清除选择
    }
}


void WarrantyInguiryLayout::on_addButton_clicked()
{
    if(addOk==0){
        //调用函数禁用计时器
        stopRefreshTimer();
        //禁用其他按钮
        ui->addButton->setEnabled(1);
        ui->updateButton->setEnabled(0);
        ui->deleteButton->setEnabled(0);
        ui->selectButton->setEnabled(0);
        //增加一行 空白
        QList<QStandardItem*> newRow;
        newRow.append(new QStandardItem("")); // ID
        newRow.append(new QStandardItem("")); // 保修物品
        newRow.append(new QStandardItem("")); // 保修结束时间
        model->appendRow(newRow); // 添加新行到模型
        addOk=1;
        //把add名字改成确认添加
        ui->addButton->setText("确认添加");
        //数据可编辑
        model->setEditableRow(model->rowCount() - 1); // 设置最后一行可编辑
        return;
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
        ui->updateButton->setEnabled(1);
        ui->deleteButton->setEnabled(1);
        ui->selectButton->setEnabled(1);
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
        QVector<QString> newWarData;
        for (const auto& item : items) {
            newWarData.append(item->text()); // 获取每个单元格的文本
        }
        //把数据添加到数据库
        SqlManager::GetInstance().AddWarrantyData(newWarData);
        ErrorToast *toast = ErrorToast::instance(this);
        toast->showToast(tr("添加成功"), 0,0,ErrorToast::SuccessIcon);
    }
}


void WarrantyInguiryLayout::on_deleteButton_clicked()
{
    //获取当前tableview焦点行
    QModelIndex currentIndex = ui->tableView_War->currentIndex();
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
    if (SqlManager::GetInstance().DeleteWarrantyData(id)) {
        qDebug() << "保修数据删除成功，ID:" << id;
        ErrorToast *toast = ErrorToast::instance(this);
        toast->showToast(tr("删除成功"), 0,0,ErrorToast::SuccessIcon);
    } else {
        qDebug() << "保修数据删除失败，ID:" << id;
        ErrorToast *toast = ErrorToast::instance(this);
        toast->showToast(tr("删除失败"), 0,0,ErrorToast::WarningIcon);
    }

}


void WarrantyInguiryLayout::on_updateButton_clicked()
{
    if(updateOk == 0) {
        //获取当前tableview焦点行
        QModelIndex currentIndex = ui->tableView_War->currentIndex();

        //禁用计时器
        stopRefreshTimer();

        //禁用其他按钮
        ui->addButton->setEnabled(0);
        ui->updateButton->setEnabled(1);
        ui->deleteButton->setEnabled(0);
        ui->selectButton->setEnabled(0);

        //设置焦点行可编辑
        currentIndex = ui->tableView_War->currentIndex();
        //把按钮名改成确认修改
        ui->updateButton->setText("确认修改");
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
        QModelIndex currentIndex = ui->tableView_War->currentIndex();

        int row = currentIndex.row(); // 获取当前行号
        //把按钮名改回去
        ui->updateButton->setText("修改");
        //禁用编辑
        model->clearEditableRow(); // 清除可编
        //启用禁用的按钮
        ui->addButton->setEnabled(1);
        ui->updateButton->setEnabled(1);

        ui->deleteButton->setEnabled(1);
        ui->selectButton->setEnabled(1);
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
        if (SqlManager::GetInstance().UpdateWarrantyData(currentId, updatedData)) {
            qDebug() << "保修数据更新成功，ID:" << updatedData[0];
            ErrorToast *toast = ErrorToast::instance(this);
            toast->showToast(tr("更新成功"), 0,0,ErrorToast::SuccessIcon);
        } else {
            qDebug() << "保修数据更新失败，ID:" << updatedData[0];
            ErrorToast *toast = ErrorToast::instance(this);
            toast->showToast(tr("更新失败"), 0,0,ErrorToast::WarningIcon);
        }

    }
}

void WarrantyInguiryLayout::searchSlots()
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
    QModelIndex currentIndex = ui->tableView_War->currentIndex();
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
                ui->tableView_War->setCurrentIndex(index);
                return; // 找到后直接返回
            }
        }
    }
    // 如果没有找到，则将第一行设置为焦点行
    if (rowCount > 0) {
        ui->tableView_War->setCurrentIndex(model->index(0, 0)); // 设置第一行为焦点行
        qDebug() << "没有找到包含搜索文本的行，已将第一行设置为焦点行";
    } else {
        qDebug() << "表格为空，没有可设置的焦点行";
    }

}


void WarrantyInguiryLayout::on_selectButton_clicked()
{
    searchSlots(); // 调用搜索函数
}


void WarrantyInguiryLayout::on_lineEdit_returnPressed()
{
    searchSlots(); // 当按下回车键时调用搜索函数
}
