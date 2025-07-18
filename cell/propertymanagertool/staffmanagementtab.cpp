#include "staffmanagementtab.h"
#include "ui_staffmanagementtab.h"
#include "showtool/mywidget.h"
#include "showtool/myeditablerowmodel.h"
#include "showtool/errortoast.h"
#include <QDebug>
#include <QMutexLocker>
#include <QHeaderView>
#include "lib/sqlmanager.h" // For SQL operations
#include <QStandardItemModel>
#include <QDateTime>
#include <QDate>
#include <QVector>
#include <QModelIndex>
#include <QMutex>
#include <QCoreApplication>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QStandardItem>
#include "showtool/QxlsxOut.h"




StaffManagementTab::StaffManagementTab(QWidget *parent)
    : MyWidget(parent)
    , ui(new Ui::StaffManagementTab)
{
    ui->setupUi(this);
    model = new MyEditableRowModel(this); // Initialize the model
    ui->tableView_staff->setModel(model); // Set the model to the table view
    // 设置表格列数和标题
    model->setColumnCount(5); // Set the number of columns
    model->setHorizontalHeaderLabels({"员工ID", "姓名", "职位", "联系方式", "入职日期"});
    // 设置表格配置
    ui->tableView_staff->horizontalHeader()->setVisible(true);
    // Set the header to auto-adjust
    ui->tableView_staff->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // Set the row height
    ui->tableView_staff->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableView_staff->verticalHeader()->setDefaultSectionSize(40); // Set row height to 30 pixels
    //只能选取一行
    ui->tableView_staff->setSelectionMode(QAbstractItemView::SingleSelection); // Set single selection mode
    ui->tableView_staff->setSelectionBehavior(QAbstractItemView::SelectRows); // Set row selection behavior


}

StaffManagementTab::~StaffManagementTab()
{
    delete ui;
}

void StaffManagementTab::init()
{
    //记录刷新前的焦点行 没有则定义为-1
    QModelIndex currentIndex = ui->tableView_staff->currentIndex();
    int currentRow = currentIndex.isValid() ? currentIndex.row() : -1;
    //加锁
    QMutexLocker locker(&dataMutex);
    // 获取数据
    staffData = SqlManager::GetInstance().GetStaffData();

    // 清空现有数据
    model->removeRows(0, model->rowCount());

    // 填充表格
    for (int i = 0; i < staffData.size(); ++i)
    {
        const auto& rowData = staffData[i];

        // 确保有足够的列数据
        QList<QStandardItem*> items;
        for (int j = 0; j < 5; j++)
        {
            QString text = (j < rowData.size()) ? rowData[j] : "";
            items << new QStandardItem(text);
        }
        model->appendRow(items);
    }
    // 恢复焦点行
    if (currentRow >= 0 && currentRow < model->rowCount())
    {
        QModelIndex newIndex = model->index(currentRow, 0); // 获取当前行的第一个单元格
        ui->tableView_staff->setCurrentIndex(newIndex); // 设置焦点到该行
    }
    else
    {
        ui->tableView_staff->clearSelection(); // 如果没有有效行，清除选择
    }
}

void StaffManagementTab::on_addButton_clicked()//添加
{
    if(addOk == 0)
    {
        //获取当前tableview焦点行
        QModelIndex currentIndex = ui->tableView_staff->currentIndex();

        //禁用计时器
        stopRefreshTimer();

        //禁用其他按钮
        ui->addButton->setEnabled(1);
        ui->deleteButton->setEnabled(0);
        ui->updateButton->setEnabled(0);
        ui->checkButton->setEnabled(0);
        //增加一行空白
        QList<QStandardItem*> newRow;
        newRow.append(new QStandardItem("")); // 员工ID
        newRow.append(new QStandardItem("")); // 姓名
        newRow.append(new QStandardItem("")); // 职位
        newRow.append(new QStandardItem("")); // 联系方式
        newRow.append(new QStandardItem("")); // 入职日期
        model->appendRow(newRow); // 添加新行到模型
        addOk=1;
        //把按钮名改成确认添加
        ui->addButton->setText("确认添加");
        //数据可编辑
        model->setEditableRow(model->rowCount() - 1); // 设置最后一行可编辑
        return;


    }
    else
    {
        //重置addOk状态
        addOk = 0;
        //把按钮名改回去
        ui->addButton->setText("添加");
        //获取新一行数据
        QList<QStandardItem*> items = model->takeRow(model->rowCount() - 1); // 获取最后一行数据

        //禁用编辑
        model->clearEditableRow(); // 清除可编
        //启用禁用的按钮
        ui->addButton->setEnabled(1);
        ui->deleteButton->setEnabled(1);
        ui->updateButton->setEnabled(1);
        ui->checkButton->setEnabled(1);

        //启用计时器
        startRefreshTimer();
        //获取数据
        QVector<QString> newStaffData;
        for (const auto& item : items)
        {
            newStaffData.append(item->text()); // 获取每个单元格的文本
        }
        //添加到数据库
        SqlManager::GetInstance().AddStaffData(newStaffData);
        qDebug() << "新员工数据添加成功";
        //抛出errortoast
        ErrorToast *toast = ErrorToast::instance(this);
        toast->showToast(tr("新员工数据添加成功"), 0,0,ErrorToast::SuccessIcon);

    }
}


void StaffManagementTab::on_deleteButton_clicked()//删除
{
    //获取焦点行
    QModelIndex currentIndex = ui->tableView_staff->currentIndex();
    if (!currentIndex.isValid())
    {
        //抛出errortoast（单例）
        ErrorToast *toast = ErrorToast::instance(this);
        toast->showToast(tr("请先选择一行"), 0,0,ErrorToast::WarningIcon);
        return; // 如果没有选中行，直接返回
    }
    int row = currentIndex.row(); // 获取当前行号
    //加锁
    QMutexLocker locker(&dataMutex);
    //获取当前行的员工ID
    QString staffId = model->data(model->index(row, 0)).toString(); // 获取员工ID
    //删除员工数据
    if (SqlManager::GetInstance().DeleteStaffData(staffId))
    {
        qDebug() << "员工数据删除成功，ID:" << staffId;
        //抛出errortoast（单例）
        ErrorToast *toast = ErrorToast::instance(this);
        toast->showToast(tr("员工数据删除成功"), 0,0,ErrorToast::SuccessIcon);
    } else {
        qDebug() << "员工数据删除失败，ID:" << staffId;
        //抛出errortoast（单例）
        ErrorToast *toast = ErrorToast::instance(this);
        toast->showToast(tr("员工数据删除失败，请稍后再试"), 0,0,ErrorToast::ErrorIcon);
    }
}


void StaffManagementTab::on_updateButton_clicked()//修改
{
    if(updateOk == 0)
    {
        //获取当前tableview焦点行
        QModelIndex currentIndex = ui->tableView_staff->currentIndex();
        if (!currentIndex.isValid())
        {
            qDebug() << "请先选择一行";
            return; // 如果没有选中行，直接返回
        }
        //加锁
        QMutexLocker locker(&dataMutex);
        //禁用计时器
        stopRefreshTimer();

        //禁用其他按钮
        ui->addButton->setEnabled(0);
        ui->deleteButton->setEnabled(0);
        ui->updateButton->setEnabled(1);
        ui->checkButton->setEnabled(0);

        updateOk = 1; // 设置更新状态为已开始
        //把按钮名改成确认修改
        ui->updateButton->setText("确认修改");

        int row = currentIndex.row(); // 获取当前行号
        //设置这一行可编辑
        model->setEditableRow(row); // 设置当前行可编辑
        //设置焦点行可编辑
        currentIndex = ui->tableView_staff->currentIndex();
        return;

    }
    else
    {
        //获取当前tableview焦点行
        QModelIndex currentIndex = ui->tableView_staff->currentIndex();
        if (!currentIndex.isValid())
        {
            qDebug() << "请先选择一行";
            return; // 如果没有选中行，直接返回
        }
        int row = currentIndex.row(); // 获取当前行号
        //把按钮名改回去
        ui->updateButton->setText("修改");
        //禁用编辑
        model->clearEditableRow(); // 清除可编
        //启用禁用的按钮
        ui->addButton->setEnabled(1);
        ui->deleteButton->setEnabled(1);
        ui->updateButton->setEnabled(1);
        ui->checkButton->setEnabled(1);
        //传回数据到数据库
        QVector<QString> updatedData;
        for (int i = 0; i < model->columnCount(); ++i)
        {
            updatedData.append(model->data(model->index(row, i)).toString());
        }

        QString staffId = updatedData[0]; // 获取员工ID

        if (SqlManager::GetInstance().UpdateStaffData(staffId, updatedData))
        {
            qDebug() << "员工数据更新成功，ID:" << staffId;
            //抛出errortoast（单例）
            ErrorToast *toast = ErrorToast::instance(this);
            toast->showToast(tr("员工数据更新成功"), 0,0,ErrorToast::SuccessIcon);
        }
    }
}
void StaffManagementTab::searchSlots()
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
    QModelIndex currentIndex = ui->tableView_staff->currentIndex();
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
                ui->tableView_staff->setCurrentIndex(index);
                return; // 找到后直接返回
            }
        }
    }
    // 如果没有找到，则将第一行设置为焦点行
    if (rowCount > 0) {
        ui->tableView_staff->setCurrentIndex(model->index(0, 0)); // 设置第一行为焦点行
        qDebug() << "没有找到包含搜索文本的行，已将第一行设置为焦点行";
    } else {
        qDebug() << "表格为空，没有可设置的焦点行";
    }

}


void StaffManagementTab::on_checkButton_clicked()
{
    searchSlots();
}


void StaffManagementTab::on_lineEdit_returnPressed()
{
    searchSlots();
}


void StaffManagementTab::on_pushButton_clicked()
{
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

