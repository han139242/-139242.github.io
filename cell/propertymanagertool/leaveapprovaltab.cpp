#include "leaveapprovaltab.h"
#include "ui_leaveapprovaltab.h"
#include "lib/sqlmanager.h" // For SQL operations
#include "showtool/myeditablerowmodel.h" // Assuming this is a custom editable row model
#include "showtool/errortoast.h"
#include <QDebug>
#include <QMutexLocker>
#include <QHeaderView>
#include <QStandardItemModel>
#include "showtool/mywidget.h" // Assuming this is a custom widget base class

LeaveApprovalTab::LeaveApprovalTab(QWidget *parent)
    : MyWidget(parent)
    , ui(new Ui::LeaveApprovalTab)
{
    ui->setupUi(this);
    model = new MyEditableRowModel(this); // Initialize the model
    ui->tableView_leaveApproval->setModel(model); // Set the model to the table view
    // 设置表格列数和标题
    model->setColumnCount(8); // Set the number of columns
    model->setHorizontalHeaderLabels({"ID", "姓名","请假状态", "开始时间", "结束时间", "请假理由", "处理人","处理意见"});
    // 设置表格配置
    ui->tableView_leaveApproval->horizontalHeader()->setVisible(true);
    ui->tableView_leaveApproval->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView_leaveApproval->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableView_leaveApproval->verticalHeader()->setDefaultSectionSize(40); // Set row height to 30 pixels
    //只能选取一行
    ui->tableView_leaveApproval->setSelectionMode(QAbstractItemView::SingleSelection); // Set single selection mode
    ui->tableView_leaveApproval->setSelectionBehavior(QAbstractItemView::SelectRows); // Set row selection behavior
}

LeaveApprovalTab::~LeaveApprovalTab()
{
    delete ui;
}

void LeaveApprovalTab::init()
{

    //获取焦点行
    QModelIndex currentIndex = ui->tableView_leaveApproval->currentIndex();
    int currentRow = currentIndex.isValid() ? currentIndex.row() : -1;

    // 加锁保护数据访问
    QMutexLocker locker(&dataMutex);
    // 获取请假数据
    leaveData = SqlManager::GetInstance().GetAllLeaveData();
    // 清空现有数据
    model->removeRows(0, model->rowCount());
    // 填充表格
    for (const auto& rowData : leaveData) {
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
        ui->tableView_leaveApproval->setCurrentIndex(newIndex); // 设置焦点到该行
    } else {
        ui->tableView_leaveApproval->clearSelection(); // 如果没有有效行，清除选择
    }
}

void LeaveApprovalTab::on_pushButton_2_clicked()  //审批
{
    if(updateOk == 0)
    {
        //获取当前tableview焦点行
        QModelIndex currentIndex = ui->tableView_leaveApproval->currentIndex();
        if (!currentIndex.isValid()) {
            qDebug() << "请先选择一行";
            return; // 如果没有选中行，直接返回
        }
        //加锁
        QMutexLocker locker(&dataMutex);
        //禁用计时器
        stopRefreshTimer();

        //禁用其他按钮
        ui->pushButton_2->setEnabled(1);
        ui->pushButton->setEnabled(0);

        updateOk = 1; // 设置更新状态为已开始
        //把按钮名改成确认修改
        ui->pushButton_2->setText("确认审批");

        int row = currentIndex.row(); // 获取当前行号
        //设置这一行可编辑
        model->setEditableRow(row); // 设置当前行可编辑
        //设置焦点行可编辑
        currentIndex = ui->tableView_leaveApproval->currentIndex();
        return;
    }
    else
    {
        //获取当前tableview焦点行
        QModelIndex currentIndex = ui->tableView_leaveApproval->currentIndex();

        int row = currentIndex.row(); // 获取当前行号
        //把按钮名改回去
        ui->pushButton_2->setText("审批");
        //禁用编辑
        model->clearEditableRow(); // 清除可编
        //启用禁用的按钮
        ui->pushButton_2->setEnabled(1);
        ui->pushButton->setEnabled(1);
        //传回
        //获取当前行的id
        QString id = model->data(model->index(row, 0)).toString(); // 获取请假ID
        //Qdebug输出
        qDebug() << "当前审批的请假ID:" << id;
        //获取当前行的处理意见
        QString handlingComments = model->data(model->index(row, 7)).toString(); // 获取处理意见
        //QDebug输出处理意见
        qDebug() << "处理意见:" << handlingComments;
        //获取当前行的开始时间
        QString startTime = model->data(model->index(row, 3)).toString(); // 获取开始时间
        //QDebug输出开始时间
        qDebug() << "开始时间:" << startTime;
        //传入数据库
        if (SqlManager::GetInstance().ApproveLeave(id, startTime,handlingComments,StaticUserId)) {
            qDebug() << "请假审批成功，ID:" << id;
            //抛出errortoast（单例）
            ErrorToast *toast = ErrorToast::instance(this);
            toast->showToast(tr("请假审批成功"), 0,0,ErrorToast::SuccessIcon);
        } else {
            qDebug() << "请假审批失败，ID:" << id;
            //抛出errortoast（单例）
            ErrorToast *toast = ErrorToast::instance(this);
            toast->showToast(tr("请假审批失败，请稍后再试"), 0,0,ErrorToast::ErrorIcon);
        }
        //获得开始时间和结束时间 转化为QDateTime
        QDateTime startDateTime = QDateTime::fromString(startTime, "yyyy-MM-dd");
        QDateTime endDateTime = QDateTime::fromString(model->data(model->index(row, 4)).toString(), "yyyy-MM-dd");
        //更新考勤表
        //SqlManager::GetInstance().InsertLeaveDataToAttendance(StaticUserId,startDate1,endDate1);
        SqlManager::GetInstance().InsertLeaveDataToAttendance(id, startDateTime.date(), endDateTime.date());



        //启用计时器
        startRefreshTimer();
        updateOk = 0;

    }
}

void LeaveApprovalTab::on_pushButton_clicked()
{
    searchSlots();
}

void LeaveApprovalTab::searchSlots()
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
    QModelIndex currentIndex = ui->tableView_leaveApproval->currentIndex();
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
                ui->tableView_leaveApproval->setCurrentIndex(index);
                return; // 找到后直接返回
            }
        }
    }
    // 如果没有找到，则将第一行设置为焦点行
    if (rowCount > 0) {
        ui->tableView_leaveApproval->setCurrentIndex(model->index(0, 0)); // 设置第一行为焦点行
        qDebug() << "没有找到包含搜索文本的行，已将第一行设置为焦点行";
    } else {
        qDebug() << "表格为空，没有可设置的焦点行";
    }
}


void LeaveApprovalTab::on_pushButton_3_clicked()
{
    //获取焦点行
    QModelIndex currentIndex = ui->tableView_leaveApproval->currentIndex();
    if (!currentIndex.isValid()) {
        //抛出errortoast（单例）
        ErrorToast *toast = ErrorToast::instance(this);
        toast->showToast(tr("请先选择一行"), 0,0,ErrorToast::WarningIcon);
        return; // 如果没有选中行，直接返回
    }
    int row = currentIndex.row(); // 获取当前行号
    //加锁
    QMutexLocker locker(&dataMutex);
    // 获取当前行的id
    QString id = model->data(model->index(row, 0)).toString(); // 获取请假ID
    //如果该行不是未审批
    QString leaveStatus = model->data(model->index(row, 2)).toString(); // 获取请假状态
    //获取开始时间
    QString startTime = model->data(model->index(row, 3)).toString(); // 获取开始时间
    if (leaveStatus != "未审批") {
        //抛出errortoast（单例）
        ErrorToast *toast = ErrorToast::instance(this);
        toast->showToast(tr("只能审批未审批的请假申请"), 0,0,ErrorToast::WarningIcon);
        return;
    }
    //拒绝请假
    if (SqlManager::GetInstance().RejectLeave(id,startTime)) {
        qDebug() << "请假拒绝成功，ID:" << id;
        //抛出errortoast（单例）
        ErrorToast *toast = ErrorToast::instance(this);
        toast->showToast(tr("请假拒绝成功"), 0,0,ErrorToast::SuccessIcon);
    } else {
        qDebug() << "请假拒绝失败，ID:" << id;
        //抛出errortoast（单例）
        ErrorToast *toast = ErrorToast::instance(this);
        toast->showToast(tr("请假拒绝失败，请稍后再试"), 0,0,ErrorToast::ErrorIcon);
    }
}


void LeaveApprovalTab::on_lineEdit_returnPressed()
{
    searchSlots();
}

