#include "logindialog.h"
#include "ui_logindialog.h"
#include "showtool/errortoast.h"
#include "lib/sqlmanager.h"
#include <QIcon>
#include <QPixmap>
#include <QLabel>
#include <QMessageBox>
#include <QDebug>
loginDialog::loginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CellWidget)
{
    ui->setupUi(this);
    //设置icon
    setWindowIcon(QIcon(":/new/prefix1/picture/icon.ico"));
    //设置标题
    setWindowTitle(tr("智慧小区"));
    //设置密码Edit为密码模式
    ui->passwordEdit->setEchoMode(QLineEdit::Password);
    ErrorToast *toast = ErrorToast::instance(this);
    toast->showToast(tr("用户名或密码错误"), 10000,10000,ErrorToast::WarningIcon);
    toast->showToast(tr("用户名或密码错误"), 10000,10000,ErrorToast::ErrorIcon);
    toast->showToast(tr("用户名或密码错误"), 10000,10000,ErrorToast::SuccessIcon);

    ui->userLineEdit->clear();
}

loginDialog::~loginDialog()
{
    delete ui;
}
void loginDialog::on_pushButton_clicked()
{

}
void loginDialog::on_loginButton_clicked()
{
    //获取用户名和密码
    QString id = ui->userLineEdit->text().trimmed();
    QString password = ui->passwordEdit->text().trimmed();
    //获取权限
    QString identity = ui->userBox->currentText().trimmed();
    //判断用户名和密码是否为空
    if (id.isEmpty() || password.isEmpty()) {

        ErrorToast *toast = ErrorToast::instance(this);
        toast->showToast(tr("用户名或密码不能为空"), 300,300,ErrorToast::ErrorIcon);
        return;

    }
    //调用SqlManager的Login方法进行登录验证
    SqlManager &sqlManager = SqlManager::GetInstance();
    if (!sqlManager.Login(id, password, identity)) {
        //登录失败
        ErrorToast *toast = ErrorToast::instance(this);
        toast->showToast(tr("用户名或密码错误"), 300,300,ErrorToast::ErrorIcon);
        ui->userLineEdit->clear();
        ui->passwordEdit->clear();
        return;
    }
    //判断用户名和密码是否正确
    StaticUserId= ui->userLineEdit->text().trimmed();
    if("物业管理员"==identity) {
        setResult(1);
        hide();
    } else if(identity=="业主") {
        setResult(2);
        hide();
    } else if(identity=="物业工作人员") {
        setResult(3);
        hide();
    }

    /*
    QString identity = ui->userBox->currentText().trimmed();

    if("物业管理员"==identity) {
        setResult(1);
        hide();
    } else if(identity=="业主") {
        setResult(2);
        hide();
    } else if(identity=="物业工作人员") {
        setResult(3);
        hide();
    }
*/
}
