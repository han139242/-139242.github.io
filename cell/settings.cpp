#include "settings.h"
#include "ui_settings.h"
#include "showtool/mywidget.h"
#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include "lib/sqlmanager.h"

//初始化静态成员变量（初始为 nullptr）
Settings* Settings::m_instance = nullptr;

// 实现单例获取方法
Settings* Settings::getInstance(QWidget *parent)
{
    // 如果实例不存在，则创建
    if (m_instance == nullptr) {
        m_instance = new Settings(parent);
    }
    return m_instance;
}

//私有构造函数
Settings::Settings(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Settings)
{
    ui->setupUi(this);
    // 设置窗口标题
    setWindowTitle(tr("设置"));
    // 设置窗口图标
    setWindowIcon(QIcon(":/new/prefix1/picture/icon.ico"));
    //读取初始化文件
    QString path = QApplication::applicationDirPath() + "/db/init.txt";
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "无法打开配置文件进行读取:" << path;
        return;
    }
    QTextStream in(&file);
    QString backgroundFilePath;
    QString font;
    QString refreshTime;
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.startsWith("背景路径:")) {
            backgroundFilePath = line.mid(5).trimmed();
            ui->backgroundEdit->setText(backgroundFilePath); // 设置到文本框
        } else if (line.startsWith("字体模式:")) {
            font = line.mid(5).trimmed();
            QFont selectedFont(font);
            ui->fontComboBox->setCurrentFont(selectedFont); // 设置字体
        } else if (line.startsWith("刷新时间:")) {
            refreshTime = line.mid(5).trimmed();
            ui->spinBox->setValue(refreshTime.toInt()); // 设置刷新时间
        }
    }
    file.close();
    // 设置默认字体
    ui->nowPasswordEdit->setEchoMode(QLineEdit::Password);
    ui->newPasswordEdit->setEchoMode(QLineEdit::Password);
    ui->againEdit->setEchoMode(QLineEdit::Password);
}

//私有析构函数
Settings::~Settings()
{
    delete ui;
    m_instance = nullptr; // 防止悬空指针（可选）
}

void Settings::writeTextFile(const QString &backgroundFilePath, const QString &font, const QString &refreshTime)
{
    QString path= QApplication::applicationDirPath() + "/db/init.txt";
    QFile file(path);
    //文件里的格式为 背景路径:背景文件路径 换行 字体模式:字体 换行 刷新时间:刷新时间
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "无法打开文件进行写入:" << path;
        return;
    }
    QTextStream out(&file);
    out << "背景路径:" << backgroundFilePath << "\n";
    out << "字体模式:" << font << "\n";
    out << "刷新时间:" << refreshTime << "\n";
    file.close();
}


void Settings::on_backgroundButton_clicked()
{
    // 打开文件对话框选择背景图片
    QString filePath = QFileDialog::getOpenFileName(this, tr("选择背景图片"), "", tr("图片文件 (*.png *.jpg *.jpeg *.bmp)"));
    if (!filePath.isEmpty()) {
        // 设置背景图片路径到文本框
        ui->backgroundEdit->setText(filePath);
    }
}


void Settings::on_okButton_clicked()
{
    // 获取背景图片路径、字体和刷新时间
    QString backgroundFilePath = ui->backgroundEdit->text();
    QString font = ui->fontComboBox->currentFont().family(); // 获取选中的字体
    QString refreshTime = ui->spinBox->text(); // 获取刷新时间
    // 写入到文本文件
    writeTextFile(backgroundFilePath, font, refreshTime);
    // 关闭设置窗口
    this->close();
    //提示 重启后生效
    QMessageBox::information(this, tr("设置已保存"), tr("设置已保存，重启应用程序后生效。"));
}


void Settings::on_pushButton_clicked()
{
    //读取nowPasswordEdit
    QString nowPassword = ui->nowPasswordEdit->text();
    //读取newPasswordEdit
    QString newPassword = ui->newPasswordEdit->text();
    //读取confirmPasswordEdit
    QString confirmPassword = ui->againEdit->text();
    //判断当前密码对不对
    if (nowPassword.isEmpty() || newPassword.isEmpty() || confirmPassword.isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("请填写所有字段。"));
        return;
    }
    //获取当前用户id
    QString currentUserId = StaticUserId;
    //检查当前密码是否正确
    QString currentPassword = SqlManager::GetInstance().GetPasswordById(currentUserId);
    if (currentPassword != nowPassword) {
        QMessageBox::warning(this, tr("错误"), tr("当前密码不正确，请重试。"));
        return;
    }
    //检查新密码和确认密码是否一致
    if (newPassword != confirmPassword) {
        QMessageBox::warning(this, tr("错误"), tr("新密码和确认密码不一致，请重试。"));
        return;
    }
    //更新密码
    if (SqlManager::GetInstance().ChangePassword(currentUserId, newPassword)) {
        QMessageBox::information(this, tr("成功"), tr("密码修改成功，请重新登录。"));
        // 关闭设置窗口
        this->close();
        // 退出应用程序
        QApplication::quit();
    } else {
        QMessageBox::warning(this, tr("错误"), tr("密码修改失败，请稍后重试。"));
    }

}

