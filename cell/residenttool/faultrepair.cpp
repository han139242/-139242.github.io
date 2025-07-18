#include "faultrepair.h"
#include "ui_faultrepair.h"
#include <QStandardItemModel>
#include "showtool/mywidget.h"
#include "lib/sqlmanager.h" // For SQL operations
#include "showtool/myeditablerowmodel.h"
#include "showtool/errortoast.h"


FaultRepair::FaultRepair(QWidget *parent)
    : MyWidget(parent)
    , ui(new Ui::FaultRepair)
{
    ui->setupUi(this);
    //初始化  把通过 QVector<QVector<QString>> GetStaffNamesAndIds();查到的物业工作人员的name和id以name(id)的格式放入下拉框中
    ui->comboBox->setEditable(true); // 设置下拉框可编辑
    QStandardItemModel *model = new QStandardItemModel(this);
    QVector<QVector<QString>> staffNamesAndIds = SqlManager::GetInstance().GetStaffNamesAndIds();
    for (const auto& staff : staffNamesAndIds) {
        QString displayText = QString("%1(%2)").arg(staff[1], staff[0]); // name(id)格式
        QStandardItem *item = new QStandardItem(displayText);
        model->appendRow(item);
    }
    ui->comboBox->setModel(model); // 设置下拉框模型
    ui->comboBox->setCurrentIndex(-1); // 设置当前无选中项
    // 设置下拉框的提示文本
}

FaultRepair::~FaultRepair()
{
    delete ui;
}




void FaultRepair::on_submissionButton_clicked()
{
    // 获取输入的内容
    QString faultDescription = ui->reasonTextEdit->toPlainText().trimmed();
    QString staffInfo = ui->comboBox->currentText().trimmed(); // 获取下拉框选中的文本
    //获取positionEdit和itemEdit的内容
    QString position = ui->positionLineEdit->text().trimmed();
    QString item = ui->itemLineEdit->text().trimmed();


    if (faultDescription.isEmpty() || staffInfo.isEmpty()) {
        qDebug() << "请填写完整的故障描述和选择处理人员";
        return; // 如果有空字段，直接返回
    }

    // 解析下拉框选中的文本，提取ID
    QStringList parts = staffInfo.split('(');
    if (parts.size() != 2) {
        qDebug() << "处理人员信息格式错误";
        return; // 如果格式不正确，直接返回
    }
    QString staffId = parts[1].chopped(1); // 去掉最后的')'
    QString staffName = parts[0]; // 获取姓名部分

    // 插入故障报修数据到数据库
    if (SqlManager::GetInstance().InsertFaultRepairData(staffId,staffName,position, faultDescription,item,"正在进行中")) {
        qDebug() << "故障报修提交成功!";
        ErrorToast *toast = ErrorToast::instance(this);
        toast->showToast(tr("故障报修提交成功!"), 0,0,ErrorToast::SuccessIcon);
        ui->reasonTextEdit->clear(); // 清空文本编辑器
        ui->comboBox->setCurrentIndex(-1); // 清空下拉框选择
        ui->positionLineEdit->clear(); // 清空位置输入框
        ui->itemLineEdit->clear(); // 清空物品输入框
    } else {
        qDebug() << "故障报修提交失败";
        ErrorToast *toast = ErrorToast::instance(this);
        toast->showToast(tr("故障报修提交失败!"), 0,0,ErrorToast::WarningIcon);
    }

}


void FaultRepair::on_deleteButton_clicked()
{
    ui->reasonTextEdit->clear(); // 清空文本编辑器
    ui->comboBox->setCurrentIndex(-1); // 清空下拉框选择
    ui->positionLineEdit->clear(); // 清空位置输入框
    ui->itemLineEdit->clear(); // 清空物品输入框

}

