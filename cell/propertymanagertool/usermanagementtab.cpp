#include "usermanagementtab.h"
#include "ui_usermanagementtab.h"
#include "showtool/myeditablerowmodel.h"
#include "lib/sqlmanager.h" // For SQL operations
#include <QStandardItemModel>
#include <QMessageBox>
#include "showtool/errortoast.h"
#include "showtool/QxlsxOut.h"

UserManagementTab::UserManagementTab(QWidget *parent)
    : MyWidget(parent)
    , ui(new Ui::UserManagementTab)
    , model(new MyEditableRowModel(this)) // Initialize the model
{
    ui->setupUi(this);
    ui->tableView->setModel(model); // Set the model to the table view
    // 设置表格列数和标题
    model->setColumnCount(5); // Set the number of columns
    model->setHorizontalHeaderLabels({"ID", "姓名", "出勤状态", "上班打卡时间", "下班打卡时间"});
    // 设置表格配置
    ui->tableView->horizontalHeader()->setVisible(true);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    // Set the row height
    ui->tableView->verticalHeader()->setDefaultSectionSize(40); // Set row height to 30 pixels
    //只能选取一行
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection); // Set single selection mode
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows); // Set row selection behavior

}

UserManagementTab::~UserManagementTab()
{
    delete ui;
}

void UserManagementTab::init()
{
    //获取焦点行
    QModelIndex currentIndex = ui->tableView->currentIndex();
    int currentRow = currentIndex.isValid() ? currentIndex.row() : -1;

    // 加锁保护数据访问
    QMutexLocker locker(&dataMutex);
    // 获取User数据
    userData = SqlManager::GetInstance().GetUserData();

    // 清空现有数据
    model->removeRows(0, model->rowCount());

    // 填充表格
    for (const auto& rowData : userData) {
        // 确保有足够的列数据
        QList<QStandardItem*> items;
        for (int j = 0; j < rowData.size(); j++) {
            items << new QStandardItem(rowData[j]);
        }
        model->appendRow(items); // 添加行到模型
    }

    //恢复焦点行
    if (currentRow >= 0 && currentRow < model->rowCount()) {
        QModelIndex newIndex = model->index(currentRow, 0); // 获取当前行的第一个单元格
        ui->tableView->setCurrentIndex(newIndex); // 设置焦点到该行
    } else {
        ui->tableView->clearSelection(); // 如果没有有效行，清除选择
    }
}

void UserManagementTab::on_clearButton_clicked()
{
    //清除该tableView和数据库中propertyTable表中的所有数据  如果成功弹出一个QMessageBox询问是否清除数据
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "清除数据", "是否清除所有用户数据？",
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        // 清除模型中的所有行
        model->removeRows(0, model->rowCount());
        // 清除数据库中的数据
        SqlManager::GetInstance().ClearUserData();
        qDebug() << "所有用户数据已清除";
        //抛出errortoast（单例）
        ErrorToast *toast = ErrorToast::instance(this);
        toast->showToast(tr("数据清除成功"), 0,0,ErrorToast::SuccessIcon);
    } else {
        qDebug() << "取消清除用户数据";
    }
}

void UserManagementTab::searchSlots()
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

void UserManagementTab::on_selectButton_clicked()
{
    searchSlots();
}

void UserManagementTab::on_lineEdit_returnPressed()
{
    searchSlots();
}


void UserManagementTab::on_pushButton_clicked()
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

