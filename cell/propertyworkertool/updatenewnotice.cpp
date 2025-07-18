#include "updatenewnotice.h"
#include "ui_updatenewnotice.h"
#include "showtool/errortoast.h"
#include "lib/sqlmanager.h"

UpdateNewNotice::UpdateNewNotice(QWidget *parent)
    : MyWidget(parent)
    , ui(new Ui::UpdateNewNotice)
{
    ui->setupUi(this);
}

UpdateNewNotice::~UpdateNewNotice()
{
    delete ui;
}

void UpdateNewNotice::on_pushButton_clicked()
{
    QString notice = ui->noticeEdit->toPlainText();
    if(notice.isEmpty()) {
        ErrorToast *toast = ErrorToast::instance(this);
        toast->showToast(tr("请输入公告内容"), 0,0,ErrorToast::ErrorIcon);
        return;
    }
    //发布公告 sql是单例
    SqlManager& sqlManager=SqlManager::GetInstance();
    if(sqlManager.InsertNotice(notice)) {
        ErrorToast *toast = ErrorToast::instance(this);
        toast->showToast(tr("发布成功"), 0,0,ErrorToast::SuccessIcon);
    } else {
        ErrorToast *toast = ErrorToast::instance(this);
        toast->showToast(tr("发布失败"), 0,0,ErrorToast::ErrorIcon);
    }
    //清空输入栏
    ui->noticeEdit->clear();
}

