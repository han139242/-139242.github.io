#ifndef ERRORTOAST_H
#define ERRORTOAST_H

#include <QWidget>
#include <QPropertyAnimation>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTimer>
#include <QIcon>
#include <QMutex>

class ErrorToast : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)

public:
    enum IconType {
        NoIcon,
        ErrorIcon,
        WarningIcon,
        SuccessIcon,
       // InfoIcon
    };

    static ErrorToast* instance(QWidget* parent = nullptr);

    ErrorToast(const ErrorToast&) = delete;
    ErrorToast& operator=(const ErrorToast&) = delete;

    void showToast(const QString& message,
                   int downOffset = 0,
                   int rightOffset = 0,
                   IconType iconType = NoIcon);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    explicit ErrorToast(QWidget* parent = nullptr);
    ~ErrorToast() override;

    void initUI();
    void initAnim();
    qreal opacity() const;
    void setOpacity(qreal op);
    void setIcon(IconType iconType);

    QLabel* iconLabel = nullptr;
    QLabel* textLabel = nullptr;
    QHBoxLayout* contentLayout = nullptr;
    QPropertyAnimation* opacityAnim = nullptr;
    static ErrorToast* s_instance;
    static QMutex mutex; // 线程安全
};

#endif // ERRORTOAST_H
