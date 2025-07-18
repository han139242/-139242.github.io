#include "parkingmanagement.h"
#include "ui_parkingmanagement.h"
#include <QDebug>
#include <QStandardItemModel> // For table view model
#include <QMenu>
#include "lib/sqlmanager.h" // For SQL operations
#include "showtool/errortoast.h"
#include <QHeaderView> // For header view
#include <QMessageBox>
#include <QMutexLocker> // For thread safety
#include "showtool/errorToast.h"
#include "showtool/QxlsxOut.h"
#include <QVector>
#include <QList>
#include <QStandardItem> // For standard item model
#include <QDebug>
#include <QCoreApplication>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QDate>
#include <QSqlRecord>
#include <showtool/myeditablerowmodel.h> // Assuming this is a custom model for editable rows
#include "QXlsx/header/xlsxdocument.h"
#include "QXlsx/header/xlsxworksheet.h"
#include <QFileDialog>


ParkingManagement::ParkingManagement(QWidget *parent)
    : MyWidget(parent)
    , ui(new Ui::ParkingManagement)
{
    ui->setupUi(this);
    model = new MyEditableRowModel(this); // Initialize the model
    ui->tableView_Car->setModel(model); // Set the model to the table view
    //Id（车位id）,位置 ,类型（是否可充电）,是否租出去 ,userid ,租金
    model->setColumnCount(6); // Set the number of columns
    model->setHeaderData(0, Qt::Horizontal, "车位ID");
    model->setHeaderData(1, Qt::Horizontal, "位置");
    model->setHeaderData(2, Qt::Horizontal, "类型（是否可充电）");
    model->setHeaderData(3, Qt::Horizontal, "是否出租");
    model->setHeaderData(4, Qt::Horizontal, "用户ID");
    model->setHeaderData(5, Qt::Horizontal, "租金");
    // Set the visibility of the header
    ui->tableView_Car->horizontalHeader()->setVisible(true);
    // Set the header to auto-adjust
    ui->tableView_Car->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // Set the row height
    ui->tableView_Car->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableView_Car->verticalHeader()->setDefaultSectionSize(40); // Set row height to 30 pixels
    ui->tableView_Car->setSelectionBehavior(QAbstractItemView::SelectRows);   // 选择整行
    // Add some example data
    // 设置右键菜单
    ui->tableView_Car->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableView_Car, &QTableView::customContextMenuRequested, this, [this](const QPoint &pos) {
        QModelIndex index = ui->tableView_Car->indexAt(pos);
        if (index.isValid()) {
            QMenu contextMenu;
            QAction *actionEdit = contextMenu.addAction("编辑");
            QAction *actionDelete = contextMenu.addAction("删除");
            QAction *selectedAction = contextMenu.exec(ui->tableView_Car->viewport()->mapToGlobal(pos));
            if (selectedAction == actionEdit) {
                // 编辑逻辑
                qDebug() << "编辑选中行";
            } else if (selectedAction == actionDelete) {
                // 删除逻辑
                qDebug() << "删除选中行";
            }
        }
    }

            );

    model2 = new MyEditableRowModel(this); // Initialize the model for the second table
    ui->tableView_Car_2->setModel(model2); // Set the model to the second table view
    // Set the number of columns for the second table
    //车牌号，进入还是离开，时间
    model2->setColumnCount(3); // Set the number of columns
    model2->setHeaderData(0, Qt::Horizontal, "车牌号");
    model2->setHeaderData(1, Qt::Horizontal, "进入还是离开");
    model2->setHeaderData(2, Qt::Horizontal, "时间");
    // Set the visibility of the header for the second table
    ui->tableView_Car_2->horizontalHeader()->setVisible(true);
    // Set the header to auto-adjust for the second table
    ui->tableView_Car_2->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // Set the row height for the second table
    ui->tableView_Car_2->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableView_Car_2->verticalHeader()->setDefaultSectionSize(40); // Set row height to 30 pixels
    ui->tableView_Car_2->setSelectionBehavior(QAbstractItemView::SelectRows);   // 选择整行
}

ParkingManagement::~ParkingManagement()
{
    delete ui;

}

void ParkingManagement::init()
{
    //记录刷新前的焦点行 没有则定义为-1
    QModelIndex currentIndex = ui->tableView_Car->currentIndex();
    int currentRow = currentIndex.isValid() ? currentIndex.row() : -1;
    //加锁
    QMutexLocker locker(&dataMutex);
    // 获取数据
    parkingData = SqlManager::GetInstance().GetParkingData();


    // 清空现有数据
    model->removeRows(0, model->rowCount());


    // 填充表格
    for (int i = 0; i < parkingData.size(); ++i) {
        const auto& rowData = parkingData[i];

        // 确保有足够的列数据
        QList<QStandardItem*> items;
        for (const auto& data : rowData) {
            items.append(new QStandardItem(data));
        }
        model->appendRow(items);
    }

    // 恢复焦点行
    if (currentRow >= 0 && currentRow < model->rowCount()) {
        ui->tableView_Car->setCurrentIndex(model->index(currentRow, 0));
    }
    //对另一个表
    //获取焦点行
    QModelIndex currentIndex2 = ui->tableView_Car_2->currentIndex();
    int currentRow2 = currentIndex2.isValid() ? currentIndex2.row() : -1;
    // 获取数据
    vehicleEntryDate = SqlManager::GetInstance().InitVehicleEntryDate();
    // 清空现有数据
    model2->removeRows(0, model2->rowCount());
    // 填充表格
    for (int i = 0; i < vehicleEntryDate.size(); ++i) {
        const auto& rowData = vehicleEntryDate[i];

        // 确保有足够的列数据
        QList<QStandardItem*> items;
        for (const auto& data : rowData) {
            items.append(new QStandardItem(data));
        }
        model2->appendRow(items);
    }
    // 恢复焦点行
    if (currentRow2 >= 0 && currentRow2 < model2->rowCount()) {
        ui->tableView_Car_2->setCurrentIndex(model2->index(currentRow2, 0));
    }
    

}

void ParkingManagement::on_pushButtonAdd_clicked()
{
    if(addOk==0){
        //调用函数禁用计时器
        stopRefreshTimer();
        //禁用其他按钮
        ui->pushButtonAdd->setEnabled(1);
        ui->pushButtonDelete->setEnabled(0);
        ui->pushButtonUpdate->setEnabled(0);
        ui->pushButtonSelect->setEnabled(0);
        //增加一行 空白
        QList<QStandardItem*> newRow;
        newRow.append(new QStandardItem(""));
        newRow.append(new QStandardItem(""));
        newRow.append(new QStandardItem(""));
        newRow.append(new QStandardItem(""));
        newRow.append(new QStandardItem(""));
        newRow.append(new QStandardItem(""));
        model->appendRow(newRow); // 添加新行到模型
        addOk=1;
        //把add名字改成确认添加
        ui->pushButtonAdd->setText("确认添加");
        //数据可编辑
        model->setEditableRow(model->rowCount() - 1); // 设置最后一行可编辑
        return;
    }
    else{
        //重置addOk状态
        addOk = 0;
        //把按钮名该回去
        ui->pushButtonAdd->setText("添加");
        //获取新一行的数据
        QList<QStandardItem*> items = model->takeRow(model->rowCount() - 1); // 获取最后一行数据
        //禁用编辑
        model->clearEditableRow(); // 清除可编辑行
        //启用禁用的按钮
        ui->pushButtonAdd->setEnabled(1);
        ui->pushButtonDelete->setEnabled(1);
        ui->pushButtonUpdate->setEnabled(1);
        ui->pushButtonSelect->setEnabled(1);
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
        if (SqlManager::GetInstance().AddParkingData(newOwnerData)) {
            ErrorToast *toast = ErrorToast::instance(this);
            toast->showToast(tr("添加成功"), 0,0,ErrorToast::SuccessIcon);
            qDebug() << "车位数据添加成功";

        } else {
            ErrorToast *toast = ErrorToast::instance(this);
            toast->showToast(tr("添加失败"), 0,0,ErrorToast::WarningIcon);
            qDebug() << "车位数据添加失败";

        }
    }
}


void ParkingManagement::on_pushButtonDelete_clicked()
{
    //获取当前tableview焦点行
    QModelIndex currentIndex = ui->tableView_Car->currentIndex();
    if (!currentIndex.isValid()) {
        qDebug() << "请先选择一行";
        return; // 如果没有选中行，直接返回
    }
    int row = currentIndex.row(); // 获取当前行号
    QString currentId = model->data(model->index(row, 0)).toString(); // 获取车位ID
    if (SqlManager::GetInstance().DeleteParkingData(currentId)) {
        qDebug() << "车位数据删除成功，ID:" << currentId;
        model->removeRow(row); // 从模型中删除行
        ErrorToast *toast = ErrorToast::instance(this);
        toast->showToast(tr("删除成功"), 0,0,ErrorToast::SuccessIcon);
    } else {
        qDebug() << "车位数据删除失败，ID:" << currentId;
    }
}


void ParkingManagement::on_pushButtonUpdate_clicked()
{
    //获取当前tableview焦点行
    QModelIndex currentIndex = ui->tableView_Car->currentIndex();
    if (!currentIndex.isValid()) {
        qDebug() << "请先选择一行";
        return; // 如果没有选中行，直接返回
    }
    int row = currentIndex.row(); // 获取当前行号
    if(updateOk==0){
        //调用函数禁用计时器
        stopRefreshTimer();
        //禁用其他按钮
        ui->pushButtonAdd->setEnabled(0);
        ui->pushButtonDelete->setEnabled(0);
        ui->pushButtonUpdate->setEnabled(1);
        ui->pushButtonSelect->setEnabled(0);
        //把按钮名改成确认修改
        ui->pushButtonUpdate->setText("确认修改");
        //数据可编辑
        model->setEditableRow(row); // 设置当前行可编辑
        updateOk=1;
        return;
    }
    else{
        //重置updateOk状态
        updateOk = 0;
        //把按钮名该回去
        ui->pushButtonUpdate->setText("修改");
        //禁用编辑
        model->clearEditableRow(); // 清除可编辑行
        //启用禁用的按钮
        ui->pushButtonAdd->setEnabled(1);
        ui->pushButtonDelete->setEnabled(1);
        ui->pushButtonUpdate->setEnabled(1);
        ui->pushButtonSelect->setEnabled(1);
        ErrorToast *toast = ErrorToast::instance(this);
        toast->showToast(tr("修改成功"), 0,0,ErrorToast::SuccessIcon);
        //启用计时器
        startRefreshTimer();
    }
}


void ParkingManagement::on_pushButtonSelect_clicked()
{
    searchSlots();
}

void ParkingManagement::searchSlots()
{
    // 获取搜索文本
    QString searchText = ui->lineEdit_2->text().trimmed(); // 去除首尾空格
    //遍历tableview中所有文本
    if (searchText.isEmpty()) {
        // 如果搜索文本为空，重新加载所有数据
        init();
        return;
    }
    // 加锁
    QMutexLocker locker(&dataMutex);
    //获取焦点行
    QModelIndex currentIndex = ui->tableView_Car->currentIndex();
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
                ui->tableView_Car->setCurrentIndex(index);
                return; // 找到后直接返回
            }
        }
    }
    // 如果没有找到，则将第一行设置为焦点行
    if (rowCount > 0) {
        ui->tableView_Car->setCurrentIndex(model->index(0, 0)); // 设置第一行为焦点行
        qDebug() << "没有找到包含搜索文本的行，已将第一行设置为焦点行";
    } else {
        qDebug() << "表格为空，没有可设置的焦点行";
    }
}


void ParkingManagement::on_lineEdit_2_returnPressed()
{
    searchSlots();
}

void ParkingManagement::searchParkingData()
{
    // 获取搜索文本
    QString searchText = ui->lineEdit_3->text().trimmed(); // 去除首尾空格
    //遍历tableview中所有文本
    if (searchText.isEmpty()) {
        // 如果搜索文本为空，重新加载所有数据
        init();
        return;
    }
    // 加锁
    QMutexLocker locker(&dataMutex);
    //获取焦点行
    QModelIndex currentIndex = ui->tableView_Car_2->currentIndex();
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
                ui->tableView_Car_2->setCurrentIndex(index);
                return; // 找到后直接返回
            }
        }
    }
    // 如果没有找到，则将第一行设置为焦点行
    if (rowCount > 0) {
        ui->tableView_Car_2->setCurrentIndex(model->index(0, 0)); // 设置第一行为焦点行
        qDebug() << "没有找到包含搜索文本的行，已将第一行设置为焦点行";
    } else {
        qDebug() << "表格为空，没有可设置的焦点行";
    }

}


void ParkingManagement::on_pushButton_12_clicked()
{
    searchParkingData();
    qDebug()<<"调用成功";
}


void ParkingManagement::on_lineEdit_3_returnPressed()
{
    searchParkingData();
    qDebug()<<"调用成功";
}


void ParkingManagement::on_pushButton_10_clicked()
{
    //删除按钮 直接调用sql函数删除全部数据
    bool ok = SqlManager::GetInstance().DeleteAllVehicleEntryDate();
    if(ok){
        qDebug() << "删除成功";
        ErrorToast *toast = ErrorToast::instance(this);
        toast->showToast(tr("删除成功"), 0,0,ErrorToast::SuccessIcon);
    }
    else{
        qDebug() << "删除失败";
        ErrorToast *toast = ErrorToast::instance(this);
        toast->showToast(tr("删除失败"), 0,0,ErrorToast::ErrorIcon);
    }
}


void ParkingManagement::on_pushButton_9_clicked()
{
    //通过execl导入车辆进出数据
    // 1. 选择 Excel 文件
    QString filePath = QFileDialog::getOpenFileName(
        this,
        tr("选择Excel文件"),
        "",
        tr("Excel文件 (*.xlsx *.xls)")
        );
    if (filePath.isEmpty()) return;
    // 2. 加载 Excel 文档
    QXlsx::Document xlsx(filePath);
    qDebug() << "选择的文件路径:" << filePath;
    QXlsx::Worksheet *sheet = dynamic_cast<QXlsx::Worksheet*>(xlsx.sheet("Sheet1"));
    if (!sheet) {
        //使用errortoast抛出警告
        ErrorToast *toast = ErrorToast::instance(this);
        toast->showToast(tr("请选择Sheet1工作表"), 0,0,ErrorToast::ErrorIcon);
        return;
    }
    // 3. 获取数据范围
    QXlsx::CellRange range = sheet->dimension();
    int maxRow = range.rowCount();
    int maxCol = range.columnCount();
    if (maxCol < 3) {
        ErrorToast *toast = ErrorToast::instance(this);
        toast->showToast(tr("Excel表头需至少3列"), 0,0,ErrorToast::ErrorIcon);
        return;
    }
    int count=0;//统计成功导入的行数
    // 4.读取excel数据 （读三列 车牌号 出还是入 时间）
    for (int row = 2; row <= maxRow; ++row) {
        QString carNumber = sheet->cellAt(row, 1)->value().toString();
        QString inOrOut = sheet->cellAt(row, 2)->value().toString();
        QString time = sheet->cellAt(row, 3)->value().toString();
        if (carNumber.isEmpty() || inOrOut.isEmpty() || time.isEmpty()) {
            ErrorToast *toast = ErrorToast::instance(this);
            toast->showToast(tr("请填写完整数据"), 0,0,ErrorToast::ErrorIcon);
            return;
        }
        // 5. 数据导入数据库 逐行写入
        bool ok = SqlManager::GetInstance().InsertVehicleEntryDate(carNumber, inOrOut, time);
        if(ok){
            qDebug() << "导入成功";
            count++;
        }
        else{
            qDebug() << "导入失败";
        }
    }
        // 6. 导入完成后 提示用户 
        if(count==maxRow-1){
            ErrorToast *toast = ErrorToast::instance(this);
            toast->showToast(tr("导入成功,共导入")+QString::number(count)+tr("条数据"), 0,0,ErrorToast::SuccessIcon);
        }
        else{
            ErrorToast *toast = ErrorToast::instance(this);
            toast->showToast(tr("导入失败,共导入")+QString::number(count)+tr("条数据"), 0,0,ErrorToast::ErrorIcon);
        }

}


void ParkingManagement::on_pushButton_clicked()
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


void ParkingManagement::on_pushButton_2_clicked()
{
    QStringList headers;
    for(int i=0;i<model2->columnCount();++i) {
        headers.append(model2->headerData(i, Qt::Horizontal).toString());
    }
    //传入一个const QVector<QVector<QVariant>>& data
    QVector<QVector<QVariant>> data;
    for(int i=0;i<model2->rowCount();++i) {
        QVector<QVariant> row;
        for(int p=0;p<model2->columnCount();++p) {
            row.append(model2->data(model2->index(i, p)).toString());
        }
        data.append(row);
    }
    exportDataToExcel(headers,data);
    //导出成功提示
    ErrorToast *toast = ErrorToast::instance(this);
    toast->showToast(tr("导出成功"), 0,0,ErrorToast::SuccessIcon);
    qDebug() << "导出成功";
}

