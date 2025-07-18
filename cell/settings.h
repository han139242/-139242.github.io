#ifndef SETTINGS_H
#define SETTINGS_H

#include <QWidget>
#include <QFile>

namespace Ui {
class Settings;
}

class Settings : public QWidget
{
    Q_OBJECT

public:
    // 删除默认的构造函数和析构函数（改为私有）
    // explicit Settings(QWidget *parent = nullptr);
    // ~Settings();

    //提供全局访问点（静态方法获取单例实例）
    static Settings* getInstance(QWidget *parent = nullptr);

    // 删除拷贝构造函数和赋值运算符（防止复制单例）
    Settings(const Settings&) = delete;
    Settings& operator=(const Settings&) = delete;

private slots:
    void on_backgroundButton_clicked();

    void on_okButton_clicked();

    void on_pushButton_clicked();

private:
    //私有化构造函数（确保外部无法直接创建实例）
    explicit Settings(QWidget *parent = nullptr);
    ~Settings();

    Ui::Settings *ui;

    //静态成员变量保存唯一实例
    static Settings* m_instance;
    void writeTextFile(const QString &backgroundFilePath, const QString &font, const QString &refreshTime);
    QVector<QString> readTextFile();
};

#endif // SETTINGS_H
