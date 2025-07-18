#include "ErrorToast.h"
#include <QPainter>
#include <QStyleOption>
#include <QPixmap>

// 静态成员初始化
ErrorToast* ErrorToast::s_instance = nullptr;
QMutex ErrorToast::mutex;

ErrorToast* ErrorToast::instance(QWidget* parent)
{
    QMutexLocker locker(&mutex); // 线程安全
    if (!s_instance) {
        s_instance = new ErrorToast(parent);
        s_instance->setAttribute(Qt::WA_DeleteOnClose);
    }
    return s_instance;
}

ErrorToast::ErrorToast(QWidget* parent)
    : QWidget(parent)
{
    initUI();
    initAnim();
    hide();
}

ErrorToast::~ErrorToast()
{
    if (opacityAnim) {
        opacityAnim->stop();
        delete opacityAnim;
        opacityAnim = nullptr;
    }
}

void ErrorToast::initUI()
{
    // 初始化图标Label
    iconLabel = new QLabel(this);
    iconLabel->setFixedSize(16, 16);
    iconLabel->setAlignment(Qt::AlignCenter);
    iconLabel->hide();

    // 初始化文本Label
    textLabel = new QLabel(this);
    textLabel->setAlignment(Qt::AlignCenter);
    textLabel->setWordWrap(true);

    // 初始化布局
    contentLayout = new QHBoxLayout();
    contentLayout->addWidget(iconLabel);
    contentLayout->addWidget(textLabel);
    contentLayout->setSpacing(8);
    contentLayout->setContentsMargins(16, 10, 16, 10);



    // 设置主布局
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(contentLayout);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
}

void ErrorToast::initAnim()
{
    opacityAnim = new QPropertyAnimation(this, "opacity", this);
    opacityAnim->setDuration(500);
}

void ErrorToast::setIcon(IconType iconType)
{
    if (!iconLabel) return; // 安全检查

    iconLabel->hide();

    QPixmap pixmap;
    switch(iconType) {
    case ErrorIcon:
        pixmap = QIcon(":/new/prefix1/picture/error.png").pixmap(16, 16);
        break;
    case WarningIcon:
        pixmap = QIcon(":/new/prefix1/picture/warning.png").pixmap(16, 16);
        break;
    case SuccessIcon:
        pixmap = QIcon(":/new/prefix1/picture/success.png").pixmap(16, 16);
        break;
        /*
    case InfoIcon:
        pixmap = QIcon(":/icons/info").pixmap(16, 16);
        break;
    case NoIcon:
*/
    default:
        return;
    }

    iconLabel->setPixmap(pixmap);
    iconLabel->show();
}

void ErrorToast::showToast(const QString& message,
                           int downOffset,
                           int rightOffset,
                           IconType iconType)
{
    if (!textLabel) return; // 安全检查

    textLabel->setText(message);
    setIcon(iconType);

        adjustSize();
        if (width()<120) {
            setFixedWidth(120);
        }

    if (QWidget* mainWindow = parentWidget()) {
        QRect mainRect = mainWindow->geometry();
        int x = mainRect.right() - width() - rightOffset;
        int y = mainRect.bottom() - height() - downOffset;
        move(x, y);
    }

    //如果是错误
    if (iconType == ErrorIcon) {
    setStyleSheet(R"(
ErrorToast {
    /* 背景：半透明白底 + 红色渐变，更有层次感 */
    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                               stop:0 rgba(255, 255, 255, 0.1),
                               stop:1 rgba(211, 47, 47, 0.95));
    border-radius: 10px;         /* 稍大圆角更柔和 */
    border: 1px solid rgba(255, 255, 255, 0.2); /* 更明显的白边区分 */
    box-shadow: 0 6px 18px rgba(0, 0, 0, 0.2);  /* 阴影更立体 */
    min-width: 140px;            /* 扩大最小宽度 */
    max-width: 400px;            /* 限制最大宽度避免过宽 */
    padding: 12px 20px;          /* 内边距更舒适 */
    /* 容器自身若有文字，这里单独控制（一般用不到，因为文字在 QLabel 里） */
    color: transparent; /* 让容器自身文字透明，避免影响 QLabel */
}

/* 专门控制 ErrorToast 内部 QLabel 的样式，与容器样式解耦 */
ErrorToast > QLabel {
    color: #333333;              /* 强制文字为黑色，不受容器影响 */
    font-family: "Nunito", "PingFang SC", "Microsoft YaHei", sans-serif;
    font-size: 18px;             /* 字体大小 */
    font-weight: 500;            /* 字体加粗 */
    margin: 0;                   /* 清除默认外边距 */
    padding: 0;                  /* 清除默认内边距 */
    text-align: center;          /* 文本水平居中 */
    vertical-align: middle;      /* 文本垂直居中（可选） */
    word-wrap: break-word;       /* 自动换行 */
    white-space: normal;         /* 允许多行 */
    /* 覆盖可能继承的背景等样式，确保独立 */
    background: transparent;
    border: none;
    box-shadow: none;
}
    )");}
    else if (iconType == WarningIcon) {
            setStyleSheet(R"(
        /* 警告提示框容器样式 */
        ErrorToast {
        /* 背景：半透明白底 + 橙色渐变（警告场景主色调） */
        background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                            stop:0 rgba(255, 255, 255, 0.1),
                            stop:1 rgba(255, 152, 0, 0.95)); /* 橙色主色（#FF9800） */
            border-radius: 10px;         /* 保持与错误提示一致的圆角 */
        border: 1px solid rgba(255, 255, 255, 0.3); /* 白边透明度略高，适配橙色背景 */
            box-shadow: 0 6px 18px rgba(255, 152, 0, 0.25); /* 阴影带橙色倾向，增强主题感 */
            min-width: 140px;            /* 统一最小宽度 */
            max-width: 400px;            /* 统一最大宽度 */
        padding: 12px 20px;          /* 内边距与错误提示一致 */
        color: transparent;          /* 避免容器文字干扰内部 Label */
        }

        /* 警告提示框内部 QLabel 样式（独立于容器） */
        ErrorToast > QLabel {
        color: #333333;;              /* 深灰色文字（比 ErrorToast 稍浅，适配橙色背景） */
            font-family: "Nunito", "PingFang SC", "Microsoft YaHei", sans-serif; /* 统一字体 */
            font-size: 18px;             /* 字号与错误提示一致 */
            font-weight: 500;            /* 半粗体保持清晰度 */
        margin: 0;
        padding: 0;
            text-align: center;
            vertical-align: middle;
            word-wrap: break-word;
            white-space: normal;
        background: transparent;     /* 彻底独立于容器背景 */
        border: none;
            box-shadow: none;
    )");}
    else if(iconType ==SuccessIcon ){
        setStyleSheet(R"(
        /* 成功提示框容器样式 *//* 成功提示框容器样式 */
ErrorToast {
    /* 背景：半透明白底 + 绿色渐变（成功主题色） */
    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                               stop:0 rgba(255, 255, 255, 0.1),
                               stop:1 rgba(76, 175, 80, 0.95)); /* 绿色主色（#4CAF50） */
    border-radius: 10px;         /* 统一圆角尺寸 */
    border: 1px solid rgba(255, 255, 255, 0.3); /* 白边适配绿色背景，透明度与警告提示一致 */
    box-shadow: 0 6px 18px rgba(76, 175, 80, 0.25); /* 绿色倾向阴影，增强主题融合感 */
    min-width: 140px;            /* 与其他提示框统一最小宽度 */
    max-width: 400px;            /* 统一最大宽度 */
    padding: 12px 20px;          /* 内边距保持一致 */
    color: transparent;          /* 避免容器文字干扰内部Label */
}

/* 成功提示框内部QLabel样式 */
ErrorToast > QLabel {
    color: #3A3A3A;              /* 深灰色文字（比警告提示稍深，适配绿色背景） */
    font-family: "Nunito", "PingFang SC", "Microsoft YaHei", sans-serif; /* 统一字体族 */
    font-size: 18px;             /* 字号与其他提示框一致 */
    font-weight: 500;            /* 半粗体保持清晰度 */
    margin: 0;
    padding: 0;
    text-align: center;
    vertical-align: middle;
    word-wrap: break-word;
    white-space: normal;
    background: transparent;     /* 独立于容器背景 */
    border: none;
    box-shadow: none;
}
)");

    }
    show();
    if (opacityAnim && opacityAnim->state() == QAbstractAnimation::Running) {
        opacityAnim->stop();
    }

    if (opacityAnim) {
        opacityAnim->setStartValue(0.0);
        opacityAnim->setEndValue(1.0);
        opacityAnim->start();

        QTimer::singleShot(2000, this, [this]() {
            if (!this || !opacityAnim || !isVisible()) return;

            opacityAnim->setStartValue(1.0);
            opacityAnim->setEndValue(0.0);
            opacityAnim->start();

            QTimer::singleShot(opacityAnim->duration(), this, [this]() {
                if (this && isVisible()) {
                    hide();
                }
            });
        });
    }
}

qreal ErrorToast::opacity() const
{
    return windowOpacity();
}

void ErrorToast::setOpacity(qreal op)
{
    setWindowOpacity(op);
}

void ErrorToast::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    QWidget::paintEvent(event);
}

