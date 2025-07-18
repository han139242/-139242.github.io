#include "propertymanager.h"
#include "settings.h"
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QTabWidget>
#include <QDateTime>

PropertyManager::PropertyManager(QWidget *parent)
    : QWidget(parent)
{
    //显示请假审核通知
    showLeaveApprovalNotification();
    //创建QTreeWidget
    QWidget *centralWidget = new QWidget(this);
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    // 创建左侧树状列表
    QTreeWidget *leftTreeWidget = new QTreeWidget(this);
    leftTreeWidget->setHeaderLabels({"功能列表"});
    // 创建一级节点 人事管理 系统管理
    QTreeWidgetItem *hrManagement = new QTreeWidgetItem(leftTreeWidget, {"人事管理"});
    hrManagement->setIcon(0, QIcon(":/icon2/picture/managericon/hrManagement.png"));
    QTreeWidgetItem *systemManagement = new QTreeWidgetItem(leftTreeWidget, {"系统管理"});
    systemManagement->setIcon(0, QIcon(":/icon2/picture/managericon/systemManagement.png"));
    QTreeWidgetItem *moneyManagement = new QTreeWidgetItem(leftTreeWidget, {"财务管理"});
    moneyManagement->setIcon(0, QIcon(":/icon2/picture/managericon/moneyManagement.png"));
    // 创建一级节点 设置
    QTreeWidgetItem *settingsItem = new QTreeWidgetItem(leftTreeWidget, {"设置"});
    settingsItem->setIcon(0, QIcon(":/icon2/picture/managericon/settingsItem.png"));
    // 创建二级节点 人事管理（工作人员，请假审核） 系统管理（系统设置，用户管理）
    QTreeWidgetItem *staffManagement = new QTreeWidgetItem(hrManagement, {"工作人员"});
    staffManagement->setIcon(0, QIcon(":/icon2/picture/managericon/staffManagement.png"));
    QTreeWidgetItem *leaveApproval = new QTreeWidgetItem(hrManagement, {"请假审核"});
    leaveApproval->setIcon(0, QIcon(":/icon2/picture/managericon/leaveApproval.png"));
    QTreeWidgetItem *systemSettings = new QTreeWidgetItem(systemManagement, {"系统设置"});
    systemSettings->setIcon(0, QIcon(":/icon2/picture/managericon/systemSettings.png"));
    QTreeWidgetItem *userManagement = new QTreeWidgetItem(hrManagement, {"出勤记录"});
    userManagement->setIcon(0, QIcon(":/icon2/picture/managericon/userManagement.png"));
    QTreeWidgetItem *paymentSumSee = new QTreeWidgetItem(moneyManagement,{"财务统计"});
    paymentSumSee->setIcon(0, QIcon(":/icon2/picture/managericon/paymentSumSee.png"));
    // 设置中央窗口
    setLayout(mainLayout);
    // 设置窗口标题
    setWindowTitle(tr("物业管理系统"));
    // 设置窗口图标
    setWindowIcon(QIcon(":/new/prefix1/picture/icon.ico"));
    // 设置窗口大小
    resize(1000, 750);
    // 设置左侧树状列表的大小
    leftTreeWidget->setMinimumWidth(200);
    // 添加左侧树状列表到主布局
    mainLayout->addWidget(leftTreeWidget);
    QTabWidget *rightDownTabWidget = new QTabWidget(this);
    // 创建各个标签页对应的界面
    staffManagementTab = new StaffManagementTab();
    leaveApprovalTab = new LeaveApprovalTab();
    systemSettingsTab = new SystemSettingsTab();
    userManagementTab = new UserManagementTab();
    paymentSumSeeTab = new PaymentSumSee();
    // 添加标签页
    rightDownTabWidget->addTab(staffManagementTab, "工作人员");
    rightDownTabWidget->addTab(leaveApprovalTab, "请假审核");
    rightDownTabWidget->addTab(systemSettingsTab, "系统设置");
    rightDownTabWidget->addTab(userManagementTab, "出勤记录");
    rightDownTabWidget->addTab(paymentSumSeeTab,"财务统计");

    // 添加右侧标签页到主布局
    mainLayout->addWidget(rightDownTabWidget);
    //右侧布局上侧也放一个布局 里边左边放一个label 右边先空着
    QWidget *topWidget = new QWidget(this);
    // 创建左侧标签 添加到布局里
    QLabel *titleLabel = new QLabel("智慧小区管理系统 >> 物业管理", topWidget);
    //在右侧添加一个label 用于提示现在是什么时间以及哪个用户在登陆
    QLabel *timeLabel = new QLabel();
    QHBoxLayout *topLayout = new QHBoxLayout(topWidget);
    topLayout->addWidget(titleLabel);
    topLayout->addStretch(); // 添加伸缩空间使按钮靠右
    topLayout->addWidget(timeLabel);
    //把右侧下标签页和右上布局放到一个整体的右布局里
    QVBoxLayout *rightLayout = new QVBoxLayout();
    rightLayout->addWidget(topWidget);
    rightLayout->addWidget(rightDownTabWidget);
    //减小右布局里上下两部分的距离
    rightLayout->setSpacing(0);
    // 设置右侧布局的间距
    rightLayout->setContentsMargins(0, 0, 0, 0);
    // 添加右侧rightLayout到主布局
    mainLayout->addLayout(rightLayout);
    //控制一下按钮和label的样式表
    titleLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #333;");
    timeLabel->setStyleSheet("font-size: 14px; color: #666;");
    // 创建定时器
    QTimer *timer = new QTimer(this);
    // 连接定时器信号到槽函数
    QObject::connect(timer, &QTimer::timeout, this,[timeLabel]() {
        QDateTime current = QDateTime::currentDateTime(); // 获取当前时间
        timeLabel->setText(current.toString("yyyy-MM-dd hh:mm:ss"));
    });
    timer->start(1000); // 每秒更新一次时间
    // 立即更新一次
    timeLabel->setText(currentUserString+"      "+QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    //连接信号与槽
    connect(leftTreeWidget, &QTreeWidget::itemClicked, this, [=](QTreeWidgetItem *item, int column) {
        // 获取点击的节点文本
        QString itemText = item->text(column);
        // 根据点击的节点文本切换右侧标签页
        int tabIndex = -1;
        for (int i = 0; i < rightDownTabWidget->count(); ++i) {
            if (rightDownTabWidget->tabText(i) == itemText) {
                tabIndex = i;
                break;
            }
        }
        // 如果找到了对应的标签页，则切换到该标签页
        if (tabIndex != -1) {
            rightDownTabWidget->setCurrentIndex(tabIndex);
        }
        // 更新标题标签内容
        QString mainTitle = "智慧小区管理系统 >> 物业管理";
        QString subTitle = itemText;
        if (item->parent() && item->parent()->text(0) != mainTitle) {
            subTitle = item->parent()->text(0) + " >> " + subTitle;
        }
        titleLabel->setText(mainTitle + " >> " + subTitle);
        // 处理 Settings 界面唤起逻辑
        if (item->text(0) == "设置") {  // 直接比较文本，而不是指针
            Settings *settings = Settings::getInstance();
            settings->show();
        }
    });
    //左右侧比例1：8
    mainLayout->setStretchFactor(leftTreeWidget, 1);
    mainLayout->setStretchFactor(rightLayout, 8);

    this->setStyleSheet(R"(
/* ================ 主窗口基础 ================ */
PropertyManager {
    background-color: #F5F8FC;
    font-family: "Microsoft YaHei", sans-serif;
}

/* ================ 左侧树状列表 ================ */
PropertyManager QTreeWidget {
    border: none;
    margin: 5px;
    background-color: #E8F0F8;
    border-radius: 8px;
    indentation: 12px;
    outline: none; /* 去掉选中时的虚线黑框 */
}

/* 树头部 */
PropertyManager QTreeWidget::header {
    background-color: #C3E0F6;
    color: #333333;
    font-size: 14px;
    font-weight: bold;
    border-top-left-radius: 8px;
    border-top-right-radius: 8px;
    padding: 6px 10px;
}

/* 树节点 */
PropertyManager QTreeWidget::item {
    background-color: transparent;
    color: #333333;
    font-size: 13px;
    padding: 8px 12px;
    border-bottom: 1px solid #C3E0F6;
}

/* 节点选中/hover效果 */
PropertyManager QTreeWidget::item:selected,
PropertyManager QTreeWidget::item:hover {
    background-color: #B3D8F3; /* 优化选中颜色 */
    color: #333333;
    border-radius: 4px;
}

/* 一级节点 */
PropertyManager QTreeWidget::item:has-children {
    font-weight: bold;
}

/* ================ 优化展开符号（纯样式调整） ================ */
PropertyManager QTreeWidget::branch {
    /* 调整展开符号的颜色，使其更明显 */
    color: #4A86E8; /* 更鲜明的蓝色 */

    /* 增加展开符号的内边距，使其更大 */
    padding: 2px;
}

/* 增大展开/闭合状态下的默认箭头 */
PropertyManager QTreeWidget::branch:has-children:closed,
PropertyManager QTreeWidget::branch:has-children:open {
    /* 增加边距让箭头更突出 */
    margin-left: 4px;
    margin-right: 4px;
}

/* 单独调整闭合状态的箭头样式 */
PropertyManager QTreeWidget::branch:has-children:closed {
    /* 可选：调整闭合箭头的颜色深度 */
    color: #3A76D8;
}

/* 单独调整展开状态的箭头样式 */
PropertyManager QTreeWidget::branch:has-children:open {
    /* 可选：调整展开箭头的颜色深度 */
    color: #5AA8F8;
}

/* ================ 右侧内容区 ================ */
PropertyManager QTabWidget {
    border: none;
    margin: 5px;
    background-color: #FFFFFF;
    border-radius: 8px;
}

PropertyManager QTabBar {
    visibility: hidden;
    height: 0;
}

PropertyManager QWidget#qt_tabwidget_stackedwidget {
    background-color: #FFFFFF;
    border-radius: 8px;
    padding: 10px;
}

/* ================ 基础控件 ================ */
PropertyManager QLabel {
    color: #666666;
    font-size: 14px;
    margin: 5px 0;
}

/* ================ 布局间距 ================ */
PropertyManager QHBoxLayout {
    spacing: 10px;
    margin: 0;
    padding: 0;
}
)");
    // 设置窗口图标
    setWindowIcon(QIcon(":/new/prefix1/picture/icon.ico"));
    // 设置窗口标题
    setWindowTitle(tr("物业管理系统"));
    // 设置窗口大小
    resize(1000, 750);
    //删除QtabletWidget的tabbar
    rightDownTabWidget->tabBar()->setVisible(false);



}

PropertyManager::~PropertyManager()
{
}


void PropertyManager::showLeaveApprovalNotification()
{
    //通过当前id调sql函数获得名字
    currentUserString = SqlManager::GetInstance().GetNameById(StaticUserId);
    //检查是否有未审批的请假请求
    if (SqlManager::GetInstance().HasPendingLeaveRequests()) {
        //如果有未审批的请假请求，弹出消息提醒
        NotifyManager *manager = new NotifyManager(this);
        manager->setMaxCount(5);
        manager->setDisplayTime(-1);
        manager->setNotifyWndSize(300, 80);
        manager->setStyleSheet(
            "#notify-background { background: black; }"
            "#notify-title { font: bold 14px 黑体; color: #eeeeee; }"
            "#notify-body { font: 12px 黑体; color: #dddddd; }"
            "#notify-close-btn { border: 0; color: #999999; }"
            "#notify-close-btn:hover { background: #444444; }",
            "black"
            );

        manager->notify("请假审批提醒", QString("您有未审批的请假请求"));
    }

}

