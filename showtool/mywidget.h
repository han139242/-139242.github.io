#ifndef MYWIDGET_H
#define MYWIDGET_H

#include <QWidget>
#include <QMenu>
#include <QAction>
#include <QString>
#include <QTimer>
#include <QFile>
#include "NotifyManager.h"

// 保留全局变量
extern QString StaticUserId;

class MyWidget : public QWidget
{
    Q_OBJECT
public:


    explicit MyWidget(QWidget *parent = nullptr);
    virtual ~MyWidget();
    // 初始化函数（子类需重写）
    virtual void init();
    // 设置背景图片
    void setBackgroundImage(const QString &imagePath);
    // 设置刷新时间
    void setRefreshTime(int time);
    // 获取全局用户ID
    static QString globalUserId() { return StaticUserId; }
    //停止计时器
    void stopRefreshTimer();
    //设置字体
    void setFont(const QFont &font) { QWidget::setFont(font); };
    //继续计时器
    void startRefreshTimer();

protected:
    // 重写键盘点击事件
    void keyPressEvent(QKeyEvent *event) override;

    // 重写显示/隐藏事件
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    //初始化参数函数
    void initParameters();


    // 刷新函数（子类可重写）
    virtual void refreshItem();

    // 背景图片路径
    QString backgroundImagePath;

    // 定时器
    QTimer *refreshTimer = nullptr;

    // 通知管理器
    NotifyManager *manager = nullptr;

private slots:
    void clickRefreshItem();

private:
    //图片路径
    QString backgroundPath="";
    //字体样式
    QString styleSheet = "QWidget { background-color: white; }"; // 默认样式表
    // 初始化样式表
    void initStyleSheet();
    // 创建上下文菜单
    void createContextMenu();
    // 刷新时间（默认3s）
    int m_refreshTime = 3000;
    QPixmap background;
    bool backgroundLoaded = false;
    void loadBackground();
    QRect calculateScaledRect() const; // 新增：计算保持比例的矩形区域
    void setFontForAllWidgets(QWidget *parentWidget, const QFont &font);
    //字体变量
    QFont defaultFont ;

};

#endif // MYWIDGET_H
