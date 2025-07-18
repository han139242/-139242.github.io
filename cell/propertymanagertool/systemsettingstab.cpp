#include "systemsettingstab.h"
#include "ui_systemsettingstab.h"
#include "lib/sqlmanager.h" // For SQL operations
#include <QMutexLocker>
#include <QDebug>
#include <QHeaderView>
#include "showtool/myeditablerowmodel.h" // Assuming this is a custom editable row model
#include "showtool/mywidget.h" // Assuming this is a custom widget base class
#include <QStandardItemModel>
#include "showtool/errortoast.h"


SystemSettingsTab::SystemSettingsTab(QWidget *parent)
    : MyWidget(parent)
    , ui(new Ui::SystemSettingsTab)
    , model(new MyEditableRowModel(this)) // Initialize the model
{
    ui->setupUi(this);
    ui->tableView->setModel(model); // Set the model to the table view
    // 设置表格列数和标题
    model->setColumnCount(4); // Set the number of columns
    model->setHorizontalHeaderLabels({"ID", "User名称", "User密码", "User身份"});
    // 设置表格配置
    ui->tableView->horizontalHeader()->setVisible(true);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableView->verticalHeader()->setDefaultSectionSize(40); // Set row height to 30 pixels
    //只能选取一行
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection); // Set single selection mode
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows); // Set row selection behavior


}

SystemSettingsTab::~SystemSettingsTab()
{
    delete ui;
}

void SystemSettingsTab::init()
{
    //获取焦点行
    QModelIndex currentIndex = ui->tableView->currentIndex();
    int currentRow = currentIndex.isValid() ? currentIndex.row() : -1;

    // 加锁保护数据访问
    QMutexLocker locker(&dataMutex);
    // 获取物业管理员数据
    systemSettingsData = SqlManager::GetInstance().GetSystemSettingsData();

    // 清空现有数据
    model->removeRows(0, model->rowCount());

    // 填充表格
    for (const auto& rowData : systemSettingsData)
    {
        // 确保有足够的列数据
        QList<QStandardItem*> items;
        for (int j = 0; j < rowData.size(); j++)
        {
            items << new QStandardItem(rowData[j]);
        }
        model->appendRow(items); // 添加行到模型
    }

    //恢复焦点行
    if (currentRow >= 0 && currentRow < model->rowCount())
    {
        ui->tableView->setCurrentIndex(model->index(currentRow, 0));
    }
    else
    {
        qDebug() << "当前行无效，无法恢复焦点行";
    }
}

void SystemSettingsTab::on_addButton_clicked()
{
    if(addOk==0){
        //调用函数禁用计时器
        stopRefreshTimer();
        //禁用其他按钮
        ui->addButton->setEnabled(1);
        ui->deleteButton->setEnabled(0);
        ui->updateButton->setEnabled(0);
        ui->selectButton->setEnabled(0);
        //增加一行 空白
        QList<QStandardItem*> newRow;
        newRow.append(new QStandardItem("")); // UserID
        newRow.append(new QStandardItem("")); // User姓名
        newRow.append(new QStandardItem("")); // User密码
        newRow.append(new QStandardItem("")); // User身份
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
        ui->deleteButton->setEnabled(1);
        ui->updateButton->setEnabled(1);
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
        QVector<QString> newUserData;
        for (const auto& item : items) {
            newUserData.append(item->text()); // 获取每个单元格的文本
        }
        //把数据添加到数据库
        SqlManager::GetInstance().AddSystemSettingsData(newUserData);
        //抛出errortoast
        ErrorToast *toast = ErrorToast::instance(this);
        toast->showToast(tr("数据添加成功"), 0,0,ErrorToast::SuccessIcon);
    }
}


void SystemSettingsTab::on_deleteButton_clicked()
{
    //获取当前tableview焦点行
    QModelIndex currentIndex = ui->tableView->currentIndex();
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
    if (SqlManager::GetInstance().DeleteSystemSettingsData(id)) {
        qDebug() << "User数据删除成功，ID:" << id;
    } else {
        qDebug() << "User数据删除失败，ID:" << id;
    }
}


void SystemSettingsTab::on_updateButton_clicked()
{
    if(updateOk == 0) {
        //获取当前tableview焦点行
        QModelIndex currentIndex = ui->tableView->currentIndex();

        //禁用计时器
        stopRefreshTimer();

        //禁用其他按钮
        ui->addButton->setEnabled(0);
        ui->updateButton->setEnabled(1);
        ui->deleteButton->setEnabled(0);
        ui->selectButton->setEnabled(0);

        //设置焦点行可编辑
        currentIndex = ui->tableView->currentIndex();
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
        //id为当前行的第一个单元格
        currentId = model->data(model->index(row, 0)).toString(); // 获取ID
        qDebug() << "当前编辑的UserID:" << currentId;
        return;
    }
    else
    {
        //获取当前tableview焦点行
        QModelIndex currentIndex = ui->tableView->currentIndex();

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
        if (SqlManager::GetInstance().UpdateSystemSettingsData(currentId, updatedData)) {
            qDebug() << "User数据更新成功，ID:" << updatedData[0];
        } else {
            qDebug() << "User数据更新失败，ID:" << updatedData[0];
        }

    }

}

void SystemSettingsTab::searchSlots()
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


void SystemSettingsTab::on_selectButton_clicked()
{
    searchSlots(); // 调用搜索函数
}


void SystemSettingsTab::on_lineEdit_returnPressed()
{
    searchSlots(); // 当用户按下回车键时，执行搜索
}

