#include "leavereguest.h"
#include "ui_leavereguest.h"
#include "lib/sqlmanager.h" // For SQL operations
#include "showtool/errortoast.h" // For error toast notifications
#include "showtool/myeditablerowmodel.h" // Assuming this is a custom editable row model
#include <QMessageBox>
#include <QDateTime>
#include <QDebug>
#include <QVector>
#include <QDate>
#include "showtool/mywidget.h" // Assuming this is a custom widget base class




LeaveReguest::LeaveReguest(QWidget *parent)
    : MyWidget(parent)
    , ui(new Ui::LeaveReguest)
{
    ui->setupUi(this);
    model = new MyEditableRowModel(this); // Initialize the model
    ui->tableView_delete->setModel(model); // Set the model to the table view
    // 设置表格列数和标题
    model->setColumnCount(6); // Set the number of columns
    model->setHorizontalHeaderLabels({"id", "请假状态", "开始时间", "结束时间","请假理由", "处理人"});
    // 设置表格配置
    ui->tableView_delete->horizontalHeader()->setVisible(true);
    ui->tableView_delete->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView_delete->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableView_delete->verticalHeader()->setDefaultSectionSize(40); // Set row height to 30 pixels
    // 设置表格行为
    ui->tableView_delete->setEditTriggers(QAbstractItemView::NoEditTriggers); // Disable editing
    ui->tableView_delete->setSelectionMode(QAbstractItemView::SingleSelection); // Set single selection mode
    ui->tableView_delete->setSelectionBehavior(QAbstractItemView::SelectRows); // Set row selection behavior
    // 设置日期控件范围

    ui->startDateEdit->setDate(QDate::currentDate());
    ui->endDateEdit->setDate(QDate::currentDate().addDays(1));

    // 设置日期格式
    ui->startDateEdit->setDisplayFormat("yyyy-MM-dd");
    ui->endDateEdit->setDisplayFormat("yyyy-MM-dd");

    // 添加处理人选项 (示例)
    //ui->handlerComboBox->addItems({"张经理", "李主管", "王总监"})
}

LeaveReguest::~LeaveReguest()
{
    delete ui;
}

void LeaveReguest::init()
{
    //获取当前日期，检查是否在数据库里面的请假开始时间和结束时间之内，如果在禁用请假按钮，如果不在，或者数据库里面没有数据，启用按钮
    QString currentUserId = StaticUserId; // Assuming StaticUserId is defined somewhere globally
    QDate currentDate = QDate::currentDate();
    QDate startDate, endDate;
    //记录刷新前的焦点行 没有则定义为-1
    QModelIndex currentIndex = ui->tableView_delete->currentIndex();
    int currentRow = currentIndex.isValid() ? currentIndex.row() : -1;
    //加锁
    //加锁保护数据访问
    QMutexLocker locker(&dataMutex);

    // 获取当前用户的请假数据
    leaveData = SqlManager::GetInstance().GetLeaveData(currentUserId);

    QVector<QVector<QString>> newLeaveData = SqlManager::GetInstance().GetLeaveData(currentUserId,"未销假");
    model->removeRows(0, model->rowCount()); // 清空现有数据
    for (const auto& rowData : newLeaveData) {
        QList<QStandardItem*> items;
        for (const auto& text : rowData) {
            items << new QStandardItem(text);
        }
        model->appendRow(items); // 添加新行到模型
    }
    newLeaveData = SqlManager::GetInstance().GetLeaveData(currentUserId,"已销假");
    for (const auto& rowData : newLeaveData) {
        QList<QStandardItem*> items;
        for (const auto& text : rowData) {
            items << new QStandardItem(text);
        }
        model->appendRow(items); // 添加新行到模型
    }
    newLeaveData = SqlManager::GetInstance().GetLeaveData(currentUserId,"未审批");
    for (const auto& rowData : newLeaveData) {
        QList<QStandardItem*> items;
        for (const auto& text : rowData) {
            items << new QStandardItem(text);
        }
        model->appendRow(items); // 添加新行到模型
    }
    newLeaveData = SqlManager::GetInstance().GetLeaveData(currentUserId,"未通过");
    for (const auto& rowData : newLeaveData) {
        QList<QStandardItem*> items;
        for (const auto& text : rowData) {
            items << new QStandardItem(text);
        }
        model->appendRow(items); // 添加新行到模型
    }
    // 恢复焦点行
    if (currentRow >= 0 && currentRow < model->rowCount()) {
        QModelIndex newIndex = model->index(currentRow, 0); // 获取当前行的第一个单元格
        ui->tableView_delete->setCurrentIndex(newIndex); // 设置焦点到该行
    } else {
        ui->tableView_delete->clearSelection(); // 如果没有有效行，清除选择
    }

    ui->submitButton->setEnabled(true); // 当前日期不在任何请假范围内，启用提交按钮
}


void LeaveReguest::on_pushButton_clicked()
{}


void LeaveReguest::on_submitButton_clicked()
{


    QString currentUserId = StaticUserId; // Assuming StaticUserId is defined somewhere globally
    //QString name = ui->nameLineEdit->text();
    QString startDate = ui->startDateEdit->text();
    QString endDate = ui->endDateEdit->text();
    QString reason = ui->reasonTextEdit->toPlainText();
    QDate startDate1 = ui->startDateEdit->date();
    QDate endDate1 = ui->endDateEdit->date();
    //QString cancellation = ui->fakeLeaveCheckBox->isChecked() ? "是" : "否";
   //QString handler = ui->handlerComboBox->currentText();

    // 验证输入//


    if (reason.isEmpty()) {
        //抛出errortoast（单例）
        ErrorToast *toast = ErrorToast::instance(this);
        toast->showToast(tr("请假理由不能为空，请重新操作"), 0,0,ErrorToast::WarningIcon);
        return;
    }
    if (startDate > endDate) {
        //抛出errortoast（单例）
        ErrorToast *toast = ErrorToast::instance(this);
        toast->showToast(tr("开始时间不能晚于结束时间，请重新操作"), 0,0,ErrorToast::WarningIcon);
        return;
    }

    // 保存到数据库
    QMutexLocker locker(&dataMutex); // 加锁以确保线程安全
    if(SqlManager::GetInstance().insertLeaveData(currentUserId, reason, startDate, endDate,"未审批")) {
        //抛出errortoast（单例）
        ErrorToast *toast = ErrorToast::instance(this);
        toast->showToast(tr("请假申请递交成功"), 0,0,ErrorToast::SuccessIcon);
        // 将数据库中leaveOfAbsence表中leaveOfAbsenceStatus显示为“未审批”的数据添加到model中
        // 清空输入框

        //ui->nameLineEdit->clear();
        ui->reasonTextEdit->clear();
        ui->startDateEdit->setDate(QDate::currentDate());
        ui->endDateEdit->setDate(QDate::currentDate().addDays(1));
    } else {

        ErrorToast *toast = ErrorToast::instance(this);
        toast->showToast(tr("请假申请递交失败，请稍后再试"), 0,0,ErrorToast::ErrorIcon);
    }
}



void LeaveReguest::on_deleteButton_clicked()
{
    //获取焦点行
    QModelIndex currentIndex = ui->tableView_delete->currentIndex();
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
    //如果该行不是未销假
    QString leaveStatus = model->data(model->index(row, 1)).toString(); // 获取请假状态
    if (leaveStatus != "未销假") {
        //抛出errortoast（单例）
        ErrorToast *toast = ErrorToast::instance(this);
        toast->showToast(tr("只能销假未销假的请假申请"), 0,0,ErrorToast::WarningIcon);
        return;
    }
    // 删除数据库中的请假数据
    if (SqlManager::GetInstance().DeleteLeaveData(id)) {
        qDebug() << "销假成功，ID:" << id;
        //抛出errortoast（单例）
        ErrorToast *toast = ErrorToast::instance(this);
        toast->showToast(tr("销假成功"), 0,0,ErrorToast::SuccessIcon);
    } else {
        qDebug() << "销假失败，ID:" << id;
        //抛出errortoast（单例）
        ErrorToast *toast = ErrorToast::instance(this);
        toast->showToast(tr("销假失败，请稍后再试"), 0,0,ErrorToast::ErrorIcon);
    }
}


