#include "propertyworker.h"
#include "propertyworkertool/attendance.h"
#include "showtool/NotifyManager.h"
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
propertyWorker::propertyWorker(QWidget *parent)
    : QWidget(parent)
    //, ui(new Ui::propertyWorker)
{
    //显示通知
    showLeaveApprovalNotification();
    showMaintenanceNotification();
    //ui->setupUi(this);
    //创建QTreeWidget
    QWidget *centralWidget = new QWidget(this);
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    // 创建左侧树状列表
    QTreeWidget *leftTreeWidget = new QTreeWidget(this);
    leftTreeWidget->setHeaderLabels({"功能列表"});
    // 创建一级节点 人事管理 业主管理 车位管理 缴费管理 维修管理
    QTreeWidgetItem *hrManagement = new QTreeWidgetItem(leftTreeWidget, {"人事管理"});
    hrManagement->setIcon(0, QIcon(":/icon/picture/hrManagement.png"));
    QTreeWidgetItem *ownerManagement = new QTreeWidgetItem(leftTreeWidget, {"业主管理"});
    ownerManagement->setIcon(0, QIcon(":/icon/picture/ownerManagement.png"));
    QTreeWidgetItem *parkingManagement = new QTreeWidgetItem(leftTreeWidget, {"车位管理"});
    parkingManagement->setIcon(0, QIcon(":/icon/picture/parkingManagement.png"));
    QTreeWidgetItem *paymentManagement = new QTreeWidgetItem(leftTreeWidget, {"缴费管理"});
    paymentManagement->setIcon(0, QIcon(":/icon/picture/paymentManagement.png"));
    QTreeWidgetItem *maintenanceManagement = new QTreeWidgetItem(leftTreeWidget, {"维修管理"});
    maintenanceManagement->setIcon(0, QIcon(":/icon/picture/maintenanceManagement.png"));
    // 创建二级节点 人事管理（月度出勤,请销假,出勤登记） 业主管理（管理业主） 车位管理（车位管理，车位承租，车辆进出） 缴费管理（价格维护，缴费记录） 维修管理（保修查询，维修过程）
    QTreeWidgetItem *attendance = new QTreeWidgetItem(hrManagement, {"月度出勤"});
    attendance->setIcon(0, QIcon(":/icon/picture/attendance.png"));
    QTreeWidgetItem *leaveRequest = new QTreeWidgetItem(hrManagement, {"请销假"});
    leaveRequest->setIcon(0, QIcon(":/icon/picture/leaveRequest.png"));
    QTreeWidgetItem *attendanceRegistration = new QTreeWidgetItem(hrManagement, {"出勤登记"});
    attendanceRegistration->setIcon(0, QIcon(":/icon/picture/attendanceRegistration.png"));
    QTreeWidgetItem *ownerManagementItem = new QTreeWidgetItem(ownerManagement, {"业主管理"});
    ownerManagementItem->setIcon(0, QIcon(":/icon/picture/ownerManagementItem.png"));
    QTreeWidgetItem *updateNewNotice = new QTreeWidgetItem(ownerManagement, {"更新通知"});
    updateNewNotice->setIcon(0, QIcon(":/icon/picture/updateNewNotice.png"));
    QTreeWidgetItem *parkingManagementItem = new QTreeWidgetItem(parkingManagement, {"车位管理"});
    parkingManagementItem->setIcon(0, QIcon(":/icon/picture/parkingManagementItem.png"));
    QTreeWidgetItem *parkingLease = new QTreeWidgetItem(parkingManagement, {"车位承租"});
    parkingLease->setIcon(0, QIcon(":/icon/picture/parkingLease.png"));
    QTreeWidgetItem *priceMaintenance = new QTreeWidgetItem(paymentManagement, {"价格维护"});
    priceMaintenance->setIcon(0, QIcon(":/icon/picture/priceMaintenance.png"));
    QTreeWidgetItem *paymentRecord = new QTreeWidgetItem(paymentManagement, {"缴费记录"});
    paymentRecord->setIcon(0, QIcon(":/icon/picture/paymentRecord.png"));
    QTreeWidgetItem *warrantyInquiry = new QTreeWidgetItem(maintenanceManagement, {"保修查询"});
    warrantyInquiry->setIcon(0, QIcon(":/icon/picture/warrantyInquiry.png"));
    QTreeWidgetItem *maintenanceProcess = new QTreeWidgetItem(maintenanceManagement, {"维修过程"});
    maintenanceProcess->setIcon(0, QIcon(":/icon/picture/maintenanceProcess.png"));
    QTreeWidgetItem *settingsItem = new QTreeWidgetItem(leftTreeWidget, {"设置"});
    settingsItem->setIcon(0, QIcon(":/icon/picture/settingsItem.png"));
    // 设置中央窗口
    setLayout(mainLayout);
    // 设置窗口标题
    setWindowTitle(tr("物业工作人员管理系统"));
    // 设置窗口图标
    setWindowIcon(QIcon(":/new/prefix1/picture/icon.ico"));
    // 设置窗口大小
    resize(800, 600);
    // 设置左侧树状列表的大小
    leftTreeWidget->setMinimumWidth(200);
    // 添加左侧树状列表到主布局
    mainLayout->addWidget(leftTreeWidget);
    QTabWidget *rightDownTabWidget = new QTabWidget(this);
    // 创建各个标签页对应的界面
    attendanceItTab = new Attendance();
    leaveRequestTab = new LeaveReguest();
    attendanceRegistrationTab = new AttendanceRegistration();
    ownerManagementTab = new OwnerManagementItem();
    updateNewNoticeTab = new UpdateNewNotice();
    parkingManagementTab = new ParkingManagement();
    parkingLeaseTab = new ParkingLease();
    priceMaintenanceTab = new PriceMaintenanceLayout();
    paymentRecordTab = new PaymentRecordTab();
    warrantyInquiryTab = new WarrantyInguiryLayout();
    maintenanceProcessTab = new MaintenanceProcessTab();
    // 添加标签页

    rightDownTabWidget->addTab(attendanceItTab, "月度出勤");
    rightDownTabWidget->addTab(leaveRequestTab, "请销假");
    rightDownTabWidget->addTab(attendanceRegistrationTab, "出勤登记");
    rightDownTabWidget->addTab(ownerManagementTab, "业主管理");
    rightDownTabWidget->addTab(updateNewNoticeTab, "更新通知");
    rightDownTabWidget->addTab(parkingManagementTab, "车位管理");
    rightDownTabWidget->addTab(parkingLeaseTab, "车位承租");
    rightDownTabWidget->addTab(priceMaintenanceTab, "价格维护");
    rightDownTabWidget->addTab(paymentRecordTab, "缴费记录");
    rightDownTabWidget->addTab(warrantyInquiryTab, "保修查询");
    rightDownTabWidget->addTab(maintenanceProcessTab, "维修过程");


    //右侧布局上侧也放一个布局 里边左边放一个label 右边先空着
    QWidget *topWidget = new QWidget(this);
    // 创建左侧标签 添加到布局里 内容为 智慧小区管理系统>>物业管理>>出勤登记等类似过程 在换到每一个tabel里都不一样 先进行初始化
    QLabel *titleLabel = new QLabel("智慧小区管理系统 >> 物业管理 >> 月度出勤", topWidget);
    //在右侧添加一个label 用于提示现在是什么时间以及哪个用户在登陆
    QLabel *timeLabel=new QLabel();
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

    //上下比例1：15
    rightLayout->setStretchFactor(topWidget, 1);
    rightLayout->setStretchFactor(rightDownTabWidget, 25);
    // 添加右侧rightLayout到主布局
    mainLayout->addLayout(rightLayout);

    //控制一下按钮和label的样式表
    titleLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #333;");
    timeLabel->setStyleSheet("font-size: 14px; color: #666;");

    // 创建定时器
    QTimer *timer = new QTimer(this);

    // 连接定时器信号到槽函数
    QObject::connect(timer, &QTimer::timeout, this,[this,timeLabel]() {
        QDateTime current = QDateTime::currentDateTime(); // 获取当前时间
        timeLabel->setText(currentUserString+"      "+current.toString("yyyy-MM-dd hh:mm:ss"));
    });
    timer->start(1000);

    // 立即更新一次
    timeLabel->setText(currentUserString+"      "+QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));


    // 连接信号与槽
    connect(leftTreeWidget, &QTreeWidget::itemClicked, this, [=](QTreeWidgetItem *item, int column) {
        QString itemText = item->text(column);
        int tabIndex = -1;
        for (int i = 0; i < rightDownTabWidget->count(); ++i) {
            if (rightDownTabWidget->tabText(i) == itemText) {
                tabIndex = i;
                break;
            }
        }
        if (tabIndex != -1) {
            rightDownTabWidget->setCurrentIndex(tabIndex);
        }
        QString mainTitle = "智慧小区管理系统 >> 物业管理";
        QString subTitle = itemText;
        if (item->parent() && item->parent()->text(0) != mainTitle) {
            subTitle = item->parent()->text(0) + " >> " + subTitle;
        }
        titleLabel->setText(mainTitle + " >> " + subTitle);
        //  处理 Settings 界面唤起逻辑
        if (item->text(0) == "设置") {  // 直接比较文本，而不是指针
            Settings *settings = Settings::getInstance();
            settings->show();
        }

    });
    //左右侧比例
    mainLayout->setStretchFactor(leftTreeWidget, 1);
    mainLayout->setStretchFactor(rightLayout, 8);
    //取消右侧上侧的索引
    rightDownTabWidget->tabBar()->hide();

    // 设置自身样式
    // 设置自身样式
    this->setStyleSheet(R"(
/* ================ 主窗口基础 ================ */
propertyWorker {
    background-color: #F5F8FC;
    font-family: "Microsoft YaHei", sans-serif;
}

/* ================ 左侧树状列表 ================ */
propertyWorker QTreeWidget {
    border: none;
    margin: 5px;
    background-color: #E8F0F8;
    border-radius: 8px;
    indentation: 12px;
    outline: none; /* 去掉选中时的虚线黑框 */
}

/* 树头部 */
propertyWorker QTreeWidget::header {
    background-color: #C3E0F6;
    color: #333333;
    font-size: 14px;
    font-weight: bold;
    border-top-left-radius: 8px;
    border-top-right-radius: 8px;
    padding: 6px 10px;
}

/* 树节点 */
propertyWorker QTreeWidget::item {
    background-color: transparent;
    color: #333333;
    font-size: 13px;
    padding: 8px 12px;
    border-bottom: 1px solid #C3E0F6;
}

/* 节点选中/hover效果 */
propertyWorker QTreeWidget::item:selected,
propertyWorker QTreeWidget::item:hover {
    background-color: #B3D8F3; /* 优化选中颜色 */
    color: #333333;
    border-radius: 4px;
}

/* 一级节点 */
propertyWorker QTreeWidget::item:has-children {
    font-weight: bold;
}

/* ================ 优化展开符号（纯样式调整） ================ */
propertyWorker QTreeWidget::branch {
    /* 调整展开符号的颜色，使其更明显 */
    color: #4A86E8; /* 更鲜明的蓝色 */

    /* 增加展开符号的内边距，使其更大 */
    padding: 2px;
}

/* 增大展开/闭合状态下的默认箭头 */
propertyWorker QTreeWidget::branch:has-children:closed,
propertyWorker QTreeWidget::branch:has-children:open {
    /* 增加边距让箭头更突出 */
    margin-left: 4px;
    margin-right: 4px;
}

/* 单独调整闭合状态的箭头样式 */
propertyWorker QTreeWidget::branch:has-children:closed {
    /* 可选：调整闭合箭头的颜色深度 */
    color: #3A76D8;
}

/* 单独调整展开状态的箭头样式 */
propertyWorker QTreeWidget::branch:has-children:open {
    /* 可选：调整展开箭头的颜色深度 */
    color: #5AA8F8;
}

/* ================ 右侧内容区 ================ */
propertyWorker QTabWidget {
    border: none;
    margin: 5px;
    background-color: #FFFFFF;
    border-radius: 8px;
}

propertyWorker QTabBar {
    visibility: hidden;
    height: 0;
}

propertyWorker QWidget#qt_tabwidget_stackedwidget {
    background-color: #FFFFFF;
    border-radius: 8px;
    padding: 10px;
}

/* ================ 基础控件 ================ */
propertyWorker QLabel {
    color: #666666;
    font-size: 14px;
    margin: 5px 0;
}

/* ================ 布局间距 ================ */
propertyWorker QHBoxLayout {
    spacing: 10px;
    margin: 0;
    padding: 0;
}
)");

    // 设置窗口图标
    setWindowIcon(QIcon(":/new/prefix1/picture/icon.ico"));
    // 设置窗口标题
    setWindowTitle(tr("物业工作人员管理系统"));
    // 设置窗口大小
    resize(1000, 750);


/*
    //test NotifyManager
    NotifyManager *manager = new NotifyManager(this);
    manager->setMaxCount(5);
    manager->setDisplayTime(5000);
    manager->setNotifyWndSize(300, 80);
    manager->setStyleSheet("#notify-background {"
                           "background: black;"
                           "}"
                           "#notify-title{"
                           "font: bold 14px 黑体;"
                           "color: #eeeeee;"
                           "}"
                           "#notify-body{"
                           "font: 12px 黑体;"
                           "color: #dddddd;"
                           "}"
                           "#notify-close-btn{ "
                           "border: 0;"
                           "color: #999999;"
                           "}"
                           "#notify-close-btn:hover{ "
                           "background: #444444;"
                           "}", "black");

    int count = 1;
    srand(QDateTime::currentMSecsSinceEpoch());
    manager->notify(tr("新消息%1").arg(count++), "欢迎登陆");
*/

}

propertyWorker::~propertyWorker()
{
    //delete ui;
}


void propertyWorker::showLeaveApprovalNotification()
{
    //通过当前id调sql函数获得名字
    currentUserString = SqlManager::GetInstance().GetNameById(StaticUserId);
    //检查是否有未审批的请假请求
    if (SqlManager::GetInstance().HasUnfinishedLeaveRequests(StaticUserId)) {
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

        manager->notify("请假审批提醒", QString("您有未销假的请假请求"));
    }
}



void propertyWorker::showMaintenanceNotification()
{
    //检查是否有未处理的维修请求
    if (SqlManager::GetInstance().HasUnfinishedRepairRequests(StaticUserId)) {
        //如果有未处理的维修请求，弹出消息提醒
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

        manager->notify("维修请求提醒", QString("您有未处理的维修请求"));
    }
}

