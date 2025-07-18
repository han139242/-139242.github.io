#include "mywidget.h"
#include <QContextMenuEvent>
#include <QKeyEvent>
#include <QMenu>
#include <QAction>
#include <QDateTime>
#include <QTimer>
#include <QPainter>
#include <QApplication>
#include <QFile>
#include <QDebug>


// 保留全局变量
QString StaticUserId = "";

MyWidget::MyWidget(QWidget *parent)
    : QWidget(parent)
{
    initParameters();
    // 创建通知管理器
    manager = new NotifyManager(this);
    // 设置背景图片变化
    loadBackground();
    // 创建上下文菜单
    createContextMenu();

    // 设置默认大小
    setMinimumSize(800, 600);

    // 初始化样式表
    initStyleSheet();

    // 创建定时器（默认不启动）
    refreshTimer = new QTimer(this);
    refreshTimer->setInterval(m_refreshTime);
    connect(refreshTimer, &QTimer::timeout, this, &MyWidget::refreshItem);

}

MyWidget::~MyWidget()
{
    // 确保定时器停止
    if (refreshTimer && refreshTimer->isActive()) {
        refreshTimer->stop();
    }
}

void MyWidget::stopRefreshTimer()
{
    if (refreshTimer && refreshTimer->isActive()) {
        refreshTimer->stop();
    }
}

void MyWidget::startRefreshTimer()
{
    if (refreshTimer && !refreshTimer->isActive()) {
        refreshTimer->start();
    }
}

void MyWidget::init()
{
    // 基类默认实现为空
}

void MyWidget::setRefreshTime(int time)
{
    m_refreshTime = time;
    if (refreshTimer) {
        refreshTimer->setInterval(m_refreshTime);
    }
}

void MyWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_F5) {
        refreshItem();
    } else {
        QWidget::keyPressEvent(event);
    }
}

void MyWidget::showEvent(QShowEvent *event)
{
    // 窗口显示时启动定时器
    if (refreshTimer && !refreshTimer->isActive()) {
        refreshTimer->start();
    }
    QWidget::showEvent(event);
    // 窗口显示时渲染背景 不显示时不渲染


}

void MyWidget::hideEvent(QHideEvent *event)
{
    // 窗口隐藏时停止定时器
    if (refreshTimer && refreshTimer->isActive()) {
        refreshTimer->stop();
    }
    QWidget::hideEvent(event);
}

void MyWidget::refreshItem()
{
    // 默认实现调用init()
    init();
}

void MyWidget::clickRefreshItem()
{
    if (!manager) return;

    manager->setMaxCount(5);
    manager->setDisplayTime(5000);
    manager->setNotifyWndSize(300, 80);
    manager->setStyleSheet(
        "#notify-background { background: black; }"
        "#notify-title { font: bold 14px 黑体; color: #eeeeee; }"
        "#notify-body { font: 12px 黑体; color: #dddddd; }"
        "#notify-close-btn { border: 0; color: #999999; }"
        "#notify-close-btn:hover { background: #444444; }",
        "black"
        );

    manager->notify("刷新通知", "刷新成功");
    refreshItem();
}

void MyWidget::initStyleSheet()
{
    QString styleSheet;
    //如果background为空
    if (background.isNull()) {
        styleSheet = QString(R"(
/* 基础样式 */
        QWidget {
            background-color: #f8f9fa;
            color: #333333;
            font-family: "%1";
            font-size: 14px;
        }

        /* 滚动条美化 */
        QScrollBar:vertical {
            background: #f0f0f0;
            width: 10px;
            margin: 0px;
            border-radius: 5px;
        }

        QScrollBar::handle:vertical {
            background: #c0c0c0;
            min-height: 30px;
            border-radius: 5px;
        }

        QScrollBar::add-line:vertical,
        QScrollBar::sub-line:vertical {
            height: 0px;
        }

        /* 按钮样式增强 */
        QPushButton {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #4a90e2, stop:1 #3a7bd5);
            color: white;
            border: none;
            padding: 8px 16px;
            border-radius: 4px;
            font-weight: 500;
            transition: all 0.2s;
        }

        QPushButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #5a9ff2, stop:1 #4a8be5);
            transform: translateY(-1px);
            box-shadow: 0 2px 4px rgba(0,0,0,0.1);
        }

        QPushButton:pressed {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #3a7bd5, stop:1 #2a6bc5);
            transform: translateY(0);
            box-shadow: none;
        }

        /* 文本输入框增强 */
        QLineEdit {
            border: 1px solid #e0e0e0;
            border-radius: 4px;
            padding: 6px 10px;
            background: white;
            transition: border-color 0.2s;
        }

        QLineEdit:focus {
            border-color: #4a90e2;
            box-shadow: 0 0 0 2px rgba(74, 144, 226, 0.2);
            outline: none;
        }

        /* 表格样式优化 */
        QTableView {
            gridline-color: #e8e8e8;
            background-color: white;
            border: 1px solid #e0e0e0;
            border-radius: 4px;
            selection-background-color: #e6f7ff;
            selection-color: #333333;
        }

        QTableView::item {
            padding: 6px 8px;
            border-bottom: 1px solid #f0f0f0;
        }

        QTableView::item:selected {
            background-color: #d6eaf8;
        }

        QHeaderView::section {
            background-color: #f5f7fa;
            color: #666666;
            padding: 6px 8px;
            border-right: 1px solid #e0e0e0;
            border-bottom: 1px solid #e0e0e0;
            font-weight: 500;
        }

        QHeaderView::section:last {
            border-right: none;
        }

        /* 标签样式优化 */
        QLabel {
            color: #333333;
            font-size: 14px;
        }

        QLabel[class="Title"] {
            font-size: 18px;
            font-weight: 600;
            color: #222222;
        }

        QLabel[class="Subtitle"] {
            font-size: 16px;
            font-weight: 500;
            color: #444444;
        }

        /* 分组框样式 */
        QGroupBox {
            border: 1px solid #e0e0e0;
            border-radius: 4px;
            margin-top: 10px;
            padding: 8px;
        }

        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 5px;
            color: #666666;
        }
    )");
        //绑定%1为字体defaultFont
        styleSheet = styleSheet.arg(defaultFont.family());
    }
    else{
        styleSheet = QString(R"(
/* 基础样式 - 半透明背景 */
        QWidget {
            background-color: rgba(248, 249, 250, 0.3);  /* RGBA（红, 绿, 蓝, 透明度） */
            color: #333333;
            font-family: "%1";
            font-size: 14px;
        }

        /* 滚动条美化 - 半透明 */
        QScrollBar:vertical {
            background: rgba(240, 240, 240, 0.3);  /* 背景色半透明 */
            width: 10px;
            margin: 0px;
            border-radius: 5px;
        }

        QScrollBar::handle:vertical {
            background: rgba(192, 192, 192, 0.5);  /* 手柄颜色半透明 */
            min-height: 30px;
            border-radius: 5px;
        }

        QScrollBar::add-line:vertical,
        QScrollBar::sub-line:vertical {
            height: 0px;
        }

        /* 按钮样式增强 - 半透明渐变 */
        QPushButton {
            background: qlineargradient(
                x1:0, y1:0, x2:0, y2:1,
                stop:0 rgba(74, 144, 226, 0.8),  /* 起始颜色半透明 */
                stop:1 rgba(58, 123, 213, 0.8)   /* 结束颜色半透明 */
            );
            color: white;
            border: none;
            padding: 8px 16px;
            border-radius: 4px;
            font-weight: 500;
            transition: all 0.2s;
        }

        QPushButton:hover {
            background: qlineargradient(
                x1:0, y1:0, x2:0, y2:1,
                stop:0 rgba(90, 159, 242, 0.9),  /* 悬停起始颜色更透明 */
                stop:1 rgba(74, 139, 229, 0.9)   /* 悬停结束颜色更透明 */
            );
            transform: translateY(-1px);
            box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
        }

        QPushButton:pressed {
            background: qlineargradient(
                x1:0, y1:0, x2:0, y2:1,
                stop:0 rgba(58, 123, 213, 0.7),  /* 按下起始颜色更透明 */
                stop:1 rgba(42, 107, 197, 0.7)   /* 按下结束颜色更透明 */
            );
            transform: translateY(0);
            box-shadow: none;
        }

        /* 文本输入框增强 - 半透明背景 */
        QLineEdit {
            border: 1px solid rgba(224, 224, 224, 0.5);  /* 边框颜色半透明 */
            border-radius: 4px;
            padding: 6px 10px;
            background: rgba(255, 255, 255, 0.5);  /* 背景色半透明 */
            transition: border-color 0.2s;
        }

        QLineEdit:focus {
            border-color: rgba(74, 144, 226, 0.7);  /* 焦点边框颜色更透明 */
            box-shadow: 0 0 0 2px rgba(74, 144, 226, 0.3);  /* 焦点阴影半透明 */
            outline: none;
        }

        /* 表格样式优化 - 半透明 */
        QTableView {
            gridline-color: rgba(232, 232, 232, 0.5);  /* 网格线颜色半透明 */
            background-color: rgba(255, 255, 255, 0.5);  /* 背景色半透明 */
            border: 1px solid rgba(224, 224, 224, 0.5);  /* 边框颜色半透明 */
            border-radius: 4px;
            selection-background-color: rgba(230, 247, 255, 0.7);  /* 选中背景色半透明 */
            selection-color: #333333;  /* 选中文字颜色保持不透明 */
        }

        QTableView::item {
            padding: 6px 8px;
            border-bottom: 1px solid rgba(240, 240, 240, 0.5);  /* 分隔线颜色半透明 */
        }

        QTableView::item:selected {
            background-color: rgba(214, 234, 248, 0.8);  /* 选中项背景色半透明 */
        }

        QHeaderView::section {
            background-color: rgba(245, 247, 250, 0.5);  /* 表头背景色半透明 */
            color: rgba(102, 102, 102, 0.9);  /* 表头文字颜色稍透明 */
            padding: 6px 8px;
            border-right: 1px solid rgba(224, 224, 224, 0.5);  /* 右侧边框颜色半透明 */
            border-bottom: 1px solid rgba(224, 224, 224, 0.5);  /* 底部边框颜色半透明 */
            font-weight: 500;
        }

        QHeaderView::section:last {
            border-right: none;
        }

        /* 标签样式优化 - 半透明 */
        QLabel {
            color: rgba(51, 51, 51, 0.9);  /* 文字颜色稍透明 */
            font-size: 14px;
        }

        QLabel[class="Title"] {
            font-size: 18px;
            font-weight: 600;
            color: rgba(34, 34, 34, 0.9);  /* 标题文字颜色稍透明 */
        }

        QLabel[class="Subtitle"] {
            font-size: 16px;
            font-weight: 500;
            color: rgba(68, 68, 68, 0.9);  /* 副标题文字颜色稍透明 */
        }

        /* 分组框样式 - 半透明 */
        QGroupBox {
            border: 1px solid rgba(224, 224, 224, 0.5);  /* 边框颜色半透明 */
            border-radius: 4px;
            margin-top: 10px;
            padding: 8px;
            background-color: rgba(255, 255, 255, 0.3);  /* 分组框背景色半透明 */
        }

        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 5px;
            color: rgba(102, 102, 102, 0.9);  /* 分组框标题文字颜色稍透明 */
        }
        QStackedWidget {
            background-color: rgba(248, 249, 250, 0); /* 透明背景 */
        }

)");
        //绑定%1为字体defaultFont
        styleSheet = styleSheet.arg(defaultFont.family());
    }


    setStyleSheet(styleSheet);
}

void MyWidget::createContextMenu()
{
    setContextMenuPolicy(Qt::ActionsContextMenu);

    QAction *refreshAction = new QAction("刷新(F5)", this);
    connect(refreshAction, &QAction::triggered, this, &MyWidget::clickRefreshItem);

    addAction(refreshAction);
}
void MyWidget::loadBackground()
{
    background = QPixmap(backgroundPath);
    if (!background.isNull()) {
        backgroundLoaded = true;
        setAutoFillBackground(false); // 禁用自动填充，改用手动绘制
    } else {
        qWarning() << "Failed to load background image!";
    }
}

QRect MyWidget::calculateScaledRect() const
{
    if (background.isNull() || size().isEmpty())
        return QRect();

    // 计算保持比例的缩放尺寸
    QSize scaledSize = background.size();
    scaledSize.scale(size(), Qt::KeepAspectRatioByExpanding);

    // 居中对齐
    int x = (width() - scaledSize.width()) / 2;
    int y = (height() - scaledSize.height()) / 2;

    return QRect(QPoint(x, y), scaledSize);
}



void MyWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    update(); // 触发重绘
}

void MyWidget::initParameters()
{
    //读取配置文件中的参数
    QString path= QApplication::applicationDirPath() + "/db/init.txt";
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "无法打开配置文件进行读取:" << path;
        return;
    }
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.startsWith("背景路径:")) {
            backgroundPath = line.mid(5).trimmed();
            loadBackground(); // 加载背景图片
        } else if (line.startsWith("字体模式:")) {
            QString fontName = line.mid(5).trimmed();
            //改变defaultFont
            defaultFont = QFont(fontName);

        } else if (line.startsWith("刷新时间:")) {
            bool ok;
            int refreshTime = line.mid(5).trimmed().toInt(&ok);
            if (ok) {
                setRefreshTime(refreshTime * 1000); // 转换为毫秒
            }
        }
    }
}

void MyWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 绘制背景
    if (backgroundLoaded) {
        QRect targetRect = calculateScaledRect();
        painter.drawPixmap(targetRect, background);
    }

    // 绘制其他内容...
}
