#ifndef FROSTEDGLASSLABEL_H
#define FROSTEDGLASSLABEL_H

#include <QLabel>
#include <QPainter>
#include <QGraphicsBlurEffect>
#include <QPixmap>
#include <QImage>
#include <QResizeEvent>
#include <QDebug>
class FrostedGlassLabel : public QLabel
{
    Q_OBJECT
public:
    explicit FrostedGlassLabel(QWidget *parent = nullptr);

    void setBlurRadius(qreal radius);
    void setOpacity(qreal opacity);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    QImage boxBlur(const QImage &image, int radius);

private:
    qreal m_blurRadius = 8.0;
    qreal m_opacity = 0.7;
    QPixmap m_blurredCache; // 缓存模糊后的图像
    bool m_cacheValid = false; // 缓存是否有效
};

#endif // FROSTEDGLASSLABEL_H
