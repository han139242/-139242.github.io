#include "pricemaintenancelayout.h"
#include "ui_pricemaintenancelayout.h"
#include <QStandardItemModel> // For table view model
#include <QHeaderView> // For header view
#include "showtool/mywidget.h" // Assuming this is the base class for your widgets
#include "showtool/myeditablerowmodel.h" // For editable row model
#include <QDebug> // For debugging output
#include "lib/sqlmanager.h" // For SQL operations
#include <QMutexLocker> // For thread-safe operations
#include <QMessageBox> // For message boxes
#include "showtool/errortoast.h" // For error messages
#include "QXlsx/header/xlsxdocument.h"
#include "QXlsx/header/xlsxworksheet.h"
#include <QFileDialog>

PriceMaintenanceLayout::PriceMaintenanceLayout(QWidget *parent)
    : MyWidget(parent)
    , ui(new Ui::PriceMaintenanceLayout)
{
    ui->setupUi(this);
    model = new MyEditableRowModel(this); // Initialize the model
    ui->tableView_Pri->setModel(model); // Set the model to the table view
    //业主id，费用类型，费用金额，是否缴纳
    model->setColumnCount(5); // Set the number of columns
    model->setHeaderData(0, Qt::Horizontal, "业主ID");
    model->setHeaderData(1, Qt::Horizontal, "费用类型");
    model->setHeaderData(2, Qt::Horizontal, "费用金额");
    model->setHeaderData(3, Qt::Horizontal, "是否缴纳");
    model->setHeaderData(4, Qt::Horizontal, "缴费时间");
    // Set the visibility of the header
    ui->tableView_Pri->horizontalHeader()->setVisible(true);
    // Set the header to auto-adjust
    ui->tableView_Pri->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // Set the row height
    ui->tableView_Pri->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableView_Pri->verticalHeader()->setDefaultSectionSize(40); // Set row height to 30 pixels
    //选中整行
    ui->tableView_Pri->setSelectionBehavior(QAbstractItemView::SelectRows); // Set row selection behavior
}

PriceMaintenanceLayout::~PriceMaintenanceLayout()
{
    delete ui;
}

void PriceMaintenanceLayout::init()
{
    //获取焦点行
    QModelIndex currentIndex = ui->tableView_Pri->currentIndex();
    int currentRow = currentIndex.isValid() ? currentIndex.row() : -1;

    // 加锁保护数据访问
    QMutexLocker locker(&dataMutex);
    // 获取缴费记录数据
    QVector<QVector<QString>> paymentData = SqlManager::GetInstance().GetPaymentRecordData_1();

    // 清空现有数据
    model->removeRows(0, model->rowCount());

    // 填充表格
    for (const auto& rowData : paymentData) {
        // 确保有足够的列数据
        QList<QStandardItem*> items;
        for (int j = 0; j < rowData.size(); j++) {
            items << new QStandardItem(rowData[j]);
        }
        model->appendRow(items); // 添加行到模型
    }

    //恢复焦点行
    if (currentRow >= 0 && currentRow < model->rowCount()) {
        ui->tableView_Pri->setCurrentIndex(model->index(currentRow, 0)); // 恢复焦点行
    } else {
        ui->tableView_Pri->setCurrentIndex(model->index(0, 0)); // 如果没有焦点行，设置第一行为焦点行
    }
}

void PriceMaintenanceLayout::on_pushButtonaAdd_clicked()
{
    if(addOk==0){
        //调用函数禁用计时器
        stopRefreshTimer();
        //禁用其他按钮
        ui->pushButtonaAdd->setEnabled(1);
        ui->pushButtonUpdate->setEnabled(0);
        ui->pushButtonDelete->setEnabled(0);
        ui->pushButtonSelect->setEnabled(0);
        //增加一行 空白
        QList<QStandardItem*> newRow;
        newRow.append(new QStandardItem("")); // 业主ID
        newRow.append(new QStandardItem("")); // 费用类型
        newRow.append(new QStandardItem("")); // 费用金额
        newRow.append(new QStandardItem("")); // 是否缴纳
        model->appendRow(newRow); // 添加新行到模型
        addOk=1;
        //把add名字改成确认添加
        ui->pushButtonaAdd->setText("确认添加");
        //数据可编辑
        model->setEditableRow(model->rowCount() - 1); // 设置最后一行可编辑
        return;
    }
    else{
        //重置addOk状态
        addOk = 0;
        //把按钮名该回去
        ui->pushButtonaAdd->setText("添加");
        //获取新一行的数据
        //QList<QStandardItem*> items大小固定为5
        QList<QStandardItem*> items = model->takeRow(model->rowCount() - 1); // 获取最后一行数据
        //禁用编辑
        model->clearEditableRow(); // 清除可编辑行
        //启用禁用的按钮
        ui->pushButtonaAdd->setEnabled(1);
        ui->pushButtonUpdate->setEnabled(1);
        ui->pushButtonDelete->setEnabled(1);
        ui->pushButtonSelect->setEnabled(1);
        //启用计时器
        startRefreshTimer();
        //获取数据
        QVector<QString> newUserData;
        //最大值设置为5
        newUserData.reserve(5); // 预留空间以提高性能
        //初始化为空字符串
        for (int i = 0; i < 5; ++i) {
            newUserData.append(""); // 初始化为空字符串
        }
        //填充数据
        newUserData[0] = items[0]->text(); // 业主ID
        newUserData[1] = items[1]->text(); // 费用类型
        newUserData[2] = items[2]->text(); // 费用金额
        newUserData[3] = "";
        newUserData[4] = "";
        //把数据添加到数据库
        SqlManager::GetInstance().AddPaymentRecordData(newUserData);
        ErrorToast *toast = ErrorToast::instance(this);
        toast->showToast(tr("添加成功"), 0,0,ErrorToast::SuccessIcon);
    }
}


void PriceMaintenanceLayout::on_pushButtonDelete_clicked()
{
    //获取当前tableview焦点行
    QModelIndex currentIndex = ui->tableView_Pri->currentIndex();
    if (!currentIndex.isValid()) {
        qDebug() << "请先选择一行";
        return; // 如果没有选中行，直接返回
    }
    int row = currentIndex.row(); // 获取当前行号
    //加锁
    QMutexLocker locker(&dataMutex);
    // 获取当前行的userid
    QString userid = model->data(model->index(row, 0)).toString(); // 获取业主ID
    //获取payType
    QString payType = model->data(model->index(row, 1)).toString(); // 获取费用类型
    //获取paymentTime
    QString paymentTime = model->data(model->index(row, 4)).toString(); // 获取缴费时间
    //获取theAmountOfTheContribution
    QString theAmountOfTheContribution = model->data(model->index(row, 2)).toString(); // 获取费用金额
    //获取payState
    QString payState = model->data(model->index(row, 3)).toString(); // 获取是否缴纳状态
    //从数据库中根据userid查找该行的主键id
    QString id = SqlManager::GetInstance().GetPaymentRecordIdByUserId(userid,payType,paymentTime,theAmountOfTheContribution,payState);

    // 删除数据库中的业主数据
    if (SqlManager::GetInstance().DeletePaymentRecordData(id)) {
        qDebug() << "业主数据删除成功，ID:" << id;
        ErrorToast *toast = ErrorToast::instance(this);
        toast->showToast(tr("删除成功"), 0,0,ErrorToast::SuccessIcon);
    } else {
        qDebug() << "业主数据删除失败，ID:" << id;
        ErrorToast *toast = ErrorToast::instance(this);
        toast->showToast(tr("删除成功"), 0,0,ErrorToast::WarningIcon);
    }
}


void PriceMaintenanceLayout::on_pushButtonUpdate_clicked()
{
    if(updateOk == 0) {
        //获取当前tableview焦点行
        QModelIndex currentIndex = ui->tableView_Pri->currentIndex();

        //禁用计时器
        stopRefreshTimer();

        //禁用其他按钮
        ui->pushButtonaAdd->setEnabled(0);
        ui->pushButtonUpdate->setEnabled(1);
        ui->pushButtonDelete->setEnabled(0);
        ui->pushButtonSelect->setEnabled(0);

        //设置焦点行可编辑
        currentIndex = ui->tableView_Pri->currentIndex();
        //把按钮名改成确认修改
        ui->pushButtonUpdate->setText("确认修改");
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
        //获取payType
        QString payType = model->data(model->index(row, 1)).toString(); // 获取费用类型
        //获取paymentTime
        QString paymentTime = model->data(model->index(row, 4)).toString(); // 获取缴费时间
        //获取theAmountOfTheContribution
        QString theAmountOfTheContribution = model->data(model->index(row, 2)).toString(); // 获取费用金额
        //获取payState
        QString payState = model->data(model->index(row, 3)).toString(); // 获取是否缴纳状态
        //根据业主id获取paymentForm中的主键id
        id = SqlManager::GetInstance().GetPaymentRecordIdByUserId(currentId,payType,paymentTime,theAmountOfTheContribution,payState);
        qDebug() << "当前编辑的业主ID:" << currentId;
        return;

    }
    else
    {
        //获取当前tableview焦点行
        QModelIndex currentIndex = ui->tableView_Pri->currentIndex();
        int row = currentIndex.row(); // 获取当前行号
        //把按钮名改回去
        ui->pushButtonUpdate->setText("修改");
        //禁用编辑
        model->clearEditableRow(); // 清除可编
        //启用禁用的按钮
        ui->pushButtonaAdd->setEnabled(1);
        ui->pushButtonUpdate->setEnabled(1);
        ui->pushButtonDelete->setEnabled(1);
        ui->pushButtonSelect->setEnabled(1);
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
        if (SqlManager::GetInstance().UpdatePaymentFormData(id, updatedData)) {
            qDebug() << "业主数据更新成功，ID:" << updatedData[0];
            ErrorToast *toast = ErrorToast::instance(this);
            toast->showToast(tr("更新成功"), 0,0,ErrorToast::SuccessIcon);
        } else {
            qDebug() << "业主数据更新失败，ID:" << updatedData[0];
            ErrorToast *toast = ErrorToast::instance(this);
            toast->showToast(tr("更新失败"), 0,0,ErrorToast::ErrorIcon);
        }
    }
}

void PriceMaintenanceLayout::searchSlots()
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
    QModelIndex currentIndex = ui->tableView_Pri->currentIndex();
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
                ui->tableView_Pri->setCurrentIndex(index);
                return; // 找到后直接返回
            }
        }
    }
    // 如果没有找到，则将第一行设置为焦点行
    if (rowCount > 0) {
        ui->tableView_Pri->setCurrentIndex(model->index(0, 0)); // 设置第一行为焦点行
        qDebug() << "没有找到包含搜索文本的行，已将第一行设置为焦点行";
    } else {
        qDebug() << "表格为空，没有可设置的焦点行";
    }

}


void PriceMaintenanceLayout::on_pushButtonSelect_clicked()
{
    searchSlots();
}


void PriceMaintenanceLayout::on_lineEdit_returnPressed()
{
    searchSlots();
}


void PriceMaintenanceLayout::on_pushButton_clicked()
{
    //实现将数据库中payRecord中payOrNot表中显示“未缴费”的数据改为“已缴费”
    //获取当前tableview焦点行
    QModelIndex currentIndex = ui->tableView_Pri->currentIndex();
    if (!currentIndex.isValid()) {
        qDebug() << "请先选择一行";
        return; // 如果没有选中行，直接返回
    }
    int row = currentIndex.row(); // 获取当前行号
    //加锁
    //QMutexLocker locker(&dataMutex);
    //获取当前行的id
    QString ownid = model->data(model->index(row, 0)).toString(); // 获取业主ID
    //获取当前行的payType
    QString payType = model->data(model->index(row, 1)).toString(); // 获取是否缴纳状态
    //获取当前行的paymentTime
    QString paymentTime = model->data(model->index(row, 4)).toString(); // 获取是否缴纳状态
    //获取当前行的theAmountOfTheContribution
    QString theAmountOfTheContribution = model->data(model->index(row, 2)).toString(); // 获取是否缴纳状态
    //获取当前行的payState
    QString payState = model->data(model->index(row, 3)).toString(); // 获取是否缴纳状态
    // 获取当前行的payOrNot状态
    // 检查当前状态是否为“未缴费”  如果是，弹出一个QMessageBox询问是否确认缴费，点击YES后将“未缴费”改为“已缴费”
    if (payState == "未缴费") {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "确认缴费", "是否确认缴费？",
                                      QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            // 更新数据库中的缴费状态
            QString id = SqlManager::GetInstance().GetPaymentRecordIdByUserId(ownid,payType,paymentTime,theAmountOfTheContribution,payState);
            if (SqlManager::GetInstance().UpdatePaymentRecordStatus(id, "已缴费")) {
                //抛出errortoast（单例）
                ErrorToast *toast = ErrorToast::instance(this);
                toast->showToast(tr("缴费成功"), 0,0,ErrorToast::SuccessIcon);
                qDebug() << "缴费状态更新成功，ID:" << id;
                // 更新表格显示
                model->setData(model->index(row, 3), "已缴费");
            } else {
                qDebug() << "缴费状态更新失败，ID:" << id;
            }
        } else {
            qDebug() << "取消缴费操作";
        }
    } else {
        qDebug() << "当前状态不是未缴费，无需操作";
    }
}


void PriceMaintenanceLayout::on_pushButton_2_clicked()
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
    if (maxCol < 5) {
        ErrorToast *toast = ErrorToast::instance(this);
        toast->showToast(tr("Excel表头需至少5列"), 0,0,ErrorToast::ErrorIcon);
        return;
    }
    int count=0;//统计成功导入的行数
    // 4.读取excel数据 （读五列 业主id 费用类型 时间 费用多少 缴费状态）
    for (int row = 2; row <= maxRow; ++row) {
        QString ownerId = sheet->cellAt(row, 1)->value().toString();
        QString feeType = sheet->cellAt(row, 2)->value().toString();
        QString time = sheet->cellAt(row, 3)->value().toString();
        if(ownerId.isEmpty() || feeType.isEmpty() || time.isEmpty() ){
            ErrorToast *toast = ErrorToast::instance(this);
            toast->showToast(tr("请填写完整数据"), 0,0,ErrorToast::ErrorIcon);
            return;
        }
        //写入一个vector
        QVector<QString> data;
        //初始化为5个 全为”“
        for(int i=0;i<5;i++){
            data.push_back("");
        }
        //将数据写入vector
        data[0]=ownerId;
        data[1]=feeType;
        data[2]=time;
        // 5. 数据导入数据库 逐行写入
        bool ok = SqlManager::GetInstance().AddPaymentRecordData(data);
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

