#include "frostedglasslabel.h"


FrostedGlassLabel::FrostedGlassLabel(QWidget *parent) : QLabel(parent)
{
    setAttribute(Qt::WA_TranslucentBackground);
    m_blurRadius = 8.0; // 设置默认模糊半径
    m_opacity = 0.7;    // 设置默认透明度
}

void FrostedGlassLabel::setBlurRadius(qreal radius)
{
    if (m_blurRadius != radius) {
        m_blurRadius = radius;
        m_cacheValid = false; // 半径改变，缓存失效
        update();
    }
}

void FrostedGlassLabel::setOpacity(qreal opacity)
{
    if (m_opacity != opacity) {
        m_opacity = opacity;
        update(); // 透明度改变，直接重绘
    }
}

void FrostedGlassLabel::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    m_cacheValid = false; // 尺寸改变，缓存失效
}

void FrostedGlassLabel::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    if (!parentWidget())
        return;

    // 如果缓存无效，重新生成模糊图像
    if (!m_cacheValid) {
        // 捕获父窗口背景
        QPixmap background = parentWidget()->grab();

        // 调整为当前大小
        QPixmap scaledBg = background.scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

        // 应用模糊效果 - 使用更高效的方式
        QImage image = scaledBg.toImage();
        QImage blurredImage = image;

        // 简单的模糊处理 - 替代QGraphicsBlurEffect
        // 注意：这是一个简化的模糊算法，性能较好但不如QGraphicsBlurEffect效果好
        int blurRadius = static_cast<int>(m_blurRadius);
        if (blurRadius > 0) {
            // 简单的盒式模糊
            blurredImage = boxBlur(image, blurRadius);
        }

        m_blurredCache = QPixmap::fromImage(blurredImage);
        m_cacheValid = true;
    }

    // 绘制最终效果
    QPainter painter(this);
    painter.setOpacity(m_opacity);
    painter.drawPixmap(rect(), m_blurredCache);

    // 绘制原始内容
    painter.setOpacity(1.0);
    QLabel::paintEvent(event);
}

// 简单的盒式模糊算法实现
QImage FrostedGlassLabel::boxBlur(const QImage &image, int radius)
{
    if (radius <= 0 || image.isNull())
        return image;

    QImage result = image.copy();
    int width = image.width();
    int height = image.height();
    int channels = 4; // ARGB32格式有4个通道

    // 水平模糊
    QImage temp = image.copy();
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int r = 0, g = 0, b = 0, a = 0;
            int count = 0;

            for (int i = -radius; i <= radius; i++) {
                int xi = x + i;
                if (xi >= 0 && xi < width) {
                    QRgb pixel = image.pixel(xi, y);
                    r += qRed(pixel);
                    g += qGreen(pixel);
                    b += qBlue(pixel);
                    a += qAlpha(pixel);
                    count++;
                }
            }

            if (count > 0) {
                QRgb blurredPixel = qRgba(r / count, g / count, b / count, a / count);
                temp.setPixel(x, y, blurredPixel);
            }
        }
    }

    // 垂直模糊
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int r = 0, g = 0, b = 0, a = 0;
            int count = 0;

            for (int i = -radius; i <= radius; i++) {
                int yi = y + i;
                if (yi >= 0 && yi < height) {
                    QRgb pixel = temp.pixel(x, yi);
                    r += qRed(pixel);
                    g += qGreen(pixel);
                    b += qBlue(pixel);
                    a += qAlpha(pixel);
                    count++;
                }
            }

            if (count > 0) {
                QRgb blurredPixel = qRgba(r / count, g / count, b / count, a / count);
                result.setPixel(x, y, blurredPixel);
            }
        }
    }

    return result;
}
