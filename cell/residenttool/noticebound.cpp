#include "noticebound.h"
#include "ui_noticebound.h"
#include <QTextEdit>
#include "lib/sqlmanager.h"

NoticeBound::NoticeBound(QWidget *parent)
    : MyWidget(parent)
    , ui(new Ui::NoticeBound)
{
    ui->setupUi(this);
}

NoticeBound::~NoticeBound()
{
    delete ui;
}
void NoticeBound::init()
{

    // 记录当前选中的标签页索引
    int currentIndex = ui->tabWidget->currentIndex();

    // 清除所有现有标签页
    ui->tabWidget->clear();

    // 获取所有通知数据
    QVector<QVector<QString>> allNoticeData = SqlManager::GetInstance().GetAllNoticeData();

    // 按日期降序排序（最新的日期排在前面）
    std::sort(allNoticeData.begin(), allNoticeData.end(), [](const QVector<QString>& a, const QVector<QString>& b) {
        return a[1] > b[1]; // 日期晚的（新的）排在前面
    });

    // 为每条通知创建一个标签页
    for (const auto &noticeData : allNoticeData) {
        // 创建新的页面部件
        QWidget *tabPage = new QWidget();

        // 创建文本编辑控件
        QTextEdit *textEdit = new QTextEdit(tabPage);
        textEdit->setReadOnly(true);

        // 设置内容（通知文本 + 日期）
        QString notice = noticeData[0];
        QString date = noticeData[1];
        textEdit->setPlainText(notice + "\n\n" + date);

        // 创建布局并设置边距为0（确保贴合边缘）
        QGridLayout *layout = new QGridLayout(tabPage);
        layout->setContentsMargins(0, 0, 0, 0); // 关键设置：消除边距
        layout->addWidget(textEdit, 0, 0);

        // 将页面添加到TabWidget
        ui->tabWidget->addTab(tabPage, "通知");
    }

    // 恢复之前选中的标签页索引（如果有效）
    if (currentIndex >= 0 && currentIndex < ui->tabWidget->count()) {
        ui->tabWidget->setCurrentIndex(currentIndex);
    } else if (ui->tabWidget->count() > 0) {
        // 如果原索引无效但有新页面，默认选中第一个
        ui->tabWidget->setCurrentIndex(0);
    }


}
