#include "resident.h"
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
#include "lib/sqlmanager.h"
#include "showtool/NotifyManager.h"

Resident::Resident(QWidget *parent)
    : QWidget(parent)
    //, ui(new Ui::Resident)
{
    initCurrentUserString();
    //业主类似propertyworker 也是一个管理系统
    //界面要和它相似 也是靠纯代码构建界面
    //一级界面有车位管理 缴费管理 维修管理 信息通知
    //二级界面有车位管理（申请车位，我的车位） 缴费管理（缴费查询，自助缴费） 维修管理（故障报修，维修过程，维修评价）
    //创建QTreeWidget
    QWidget *centralWidget = new QWidget(this);
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    // 创建左侧树状列表
    QTreeWidget *leftTreeWidget = new QTreeWidget(this);
    leftTreeWidget->setHeaderLabels({"功能列表"});
    // 创建一级节点 车位管理 缴费管理 维修管理
    QTreeWidgetItem *parkingManagement = new QTreeWidgetItem(leftTreeWidget, {"车位管理"});
    parkingManagement->setIcon(0, QIcon(":/icon3/picture/residenticon/parkingManagement.png"));
    QTreeWidgetItem *paymentManagement = new QTreeWidgetItem(leftTreeWidget, {"缴费管理"});
    paymentManagement->setIcon(0, QIcon(":/icon3/picture/residenticon/paymentManagement.png"));
    QTreeWidgetItem *maintenanceManagement = new QTreeWidgetItem(leftTreeWidget, {"维修管理"});
    maintenanceManagement->setIcon(0, QIcon(":/icon3/picture/residenticon/maintenanceManagement.png"));
    QTreeWidgetItem *noticeManagement = new QTreeWidgetItem(leftTreeWidget, {"信息通知"});
    noticeManagement->setIcon(0, QIcon(":/icon3/picture/residenticon/noticeManagement.png"));
    // 创建一级节点 设置
    QTreeWidgetItem *settingsItem = new QTreeWidgetItem(leftTreeWidget, {"设置"});
    settingsItem->setIcon(0, QIcon(":/icon3/picture/residenticon/settingsItem.png"));
    // 创建二级节点 车位管理（申请车位，我的车位） 缴费管理（缴费查询，自助缴费） 维修管理（故障报修，维修过程，维修评价）
    QTreeWidgetItem *noticeItem = new QTreeWidgetItem(noticeManagement, {"小区公告"});
    noticeItem->setIcon(0, QIcon(":/icon3/picture/residenticon/noticeItem.png"));
    QTreeWidgetItem *parkingApplication = new QTreeWidgetItem(parkingManagement, {"申请车位"});
    parkingApplication->setIcon(0, QIcon(":/icon3/picture/residenticon/parkingApplication.png"));
    QTreeWidgetItem *myParking = new QTreeWidgetItem(parkingManagement, {"我的车位"});
    myParking->setIcon(0, QIcon(":/icon3/picture/residenticon/myParking.png"));
    QTreeWidgetItem *paymentQuery = new QTreeWidgetItem(paymentManagement, {"缴费查询"});
    paymentQuery->setIcon(0, QIcon(":/icon3/picture/residenticon/paymentQuery.png"));
    QTreeWidgetItem *selfPayment = new QTreeWidgetItem(paymentManagement, {"自助缴费"});
    selfPayment->setIcon(0, QIcon(":/icon3/picture/residenticon/selfPayment.png"));
    QTreeWidgetItem *faultRepair = new QTreeWidgetItem(maintenanceManagement, {"故障报修"});
    faultRepair->setIcon(0, QIcon(":/icon3/picture/residenticon/faultRepair.png"));
    QTreeWidgetItem *repairProcess = new QTreeWidgetItem(maintenanceManagement, {"维修过程"});
    repairProcess->setIcon(0, QIcon(":/icon3/picture/residenticon/repairProcess.png"));
    QTreeWidgetItem *repairEvaluation = new QTreeWidgetItem(maintenanceManagement, {"维修评价"});
    repairEvaluation->setIcon(0, QIcon(":/icon3/picture/residenticon/repairEvaluation.png"));
    // 设置中央窗口
    setLayout(mainLayout);
    // 设置窗口标题
    setWindowTitle(tr("业主管理系统"));
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
    noticeBoundTab = new NoticeBound();
    parkingApplicationTab = new ParkingApplication();
    myParkingTab = new MyParking();
    paymentQueryTab = new PaymentQuery();
    selfPaymentTab = new SelfPayment();
    faultRepairTab = new FaultRepair();
    repairProcessTab = new RepairProcess();
    repairEvaluationTab = new RepairEvaluation();
    // 添加标签页
    rightDownTabWidget->addTab(noticeBoundTab, "小区公告");
    rightDownTabWidget->addTab(parkingApplicationTab, "申请车位");
    rightDownTabWidget->addTab(myParkingTab, "我的车位");
    rightDownTabWidget->addTab(paymentQueryTab, "缴费查询");
    rightDownTabWidget->addTab(selfPaymentTab, "自助缴费");
    rightDownTabWidget->addTab(faultRepairTab, "故障报修");
    rightDownTabWidget->addTab(repairProcessTab, "维修过程");
    rightDownTabWidget->addTab(repairEvaluationTab, "维修评价");
    // 添加右侧标签页到主布局
    mainLayout->addWidget(rightDownTabWidget);
    //右侧布局上侧也放一个布局 里边左边放一个label 右边放一个时间label
    QWidget *topWidget = new QWidget(this);
    // 创建左侧标签 添加到布局里
    QLabel *titleLabel = new QLabel("智慧小区管理系统 >> 业主管理", topWidget);
    QLabel *timeLabel = new QLabel(topWidget);
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
    //上下比例1：25
    rightLayout->setStretchFactor(topWidget, 1);
    rightLayout->setStretchFactor(rightDownTabWidget, 25);
    // 设置右侧布局的间距
    rightLayout->setContentsMargins(0, 0, 0, 0);
    // 添加右侧rightLayout到主布局
    mainLayout->addLayout(rightLayout);
    //控制一下样式表
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
        QString mainTitle = "智慧小区管理系统 >> 业主管理";
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
Resident {
    background-color: #F5F8FC;
    font-family: "Microsoft YaHei", sans-serif;
}

/* ================ 左侧树状列表 ================ */
Resident QTreeWidget {
    border: none;
    margin: 5px;
    background-color: #E8F0F8;
    border-radius: 8px;
    indentation: 12px;
    outline: none; /* 去掉选中时的虚线黑框 */
}

/* 树头部 */
Resident QTreeWidget::header {
    background-color: #C3E0F6;
    color: #333333;
    font-size: 14px;
    font-weight: bold;
    border-top-left-radius: 8px;
    border-top-right-radius: 8px;
    padding: 6px 10px;
}

/* 树节点 */
Resident QTreeWidget::item {
    background-color: transparent;
    color: #333333;
    font-size: 13px;
    padding: 8px 12px;
    border-bottom: 1px solid #C3E0F6;
}

/* 节点选中/hover效果 */
Resident QTreeWidget::item:selected,
Resident QTreeWidget::item:hover {
    background-color: #B3D8F3; /* 优化选中颜色 */
    color: #333333;
    border-radius: 4px;
}

/* 一级节点 */
Resident QTreeWidget::item:has-children {
    font-weight: bold;
}

/* ================ 优化展开符号（纯样式调整） ================ */
Resident QTreeWidget::branch {
    /* 调整展开符号的颜色，使其更明显 */
    color: #4A86E8; /* 更鲜明的蓝色 */

    /* 增加展开符号的内边距，使其更大 */
    padding: 2px;
}

/* 增大展开/闭合状态下的默认箭头 */
Resident QTreeWidget::branch:has-children:closed,
Resident QTreeWidget::branch:has-children:open {
    /* 增加边距让箭头更突出 */
    margin-left: 4px;
    margin-right: 4px;
}

/* 单独调整闭合状态的箭头样式 */
Resident QTreeWidget::branch:has-children:closed {
    /* 可选：调整闭合箭头的颜色深度 */
    color: #3A76D8;
}

/* 单独调整展开状态的箭头样式 */
Resident QTreeWidget::branch:has-children:open {
    /* 可选：调整展开箭头的颜色深度 */
    color: #5AA8F8;
}

/* ================ 右侧内容区 ================ */
Resident QTabWidget {
    border: none;
    margin: 5px;
    background-color: #FFFFFF;
    border-radius: 8px;
}

Resident QTabBar {
    visibility: hidden;
    height: 0;
}

Resident QWidget#qt_tabwidget_stackedwidget {
    background-color: #FFFFFF;
    border-radius: 8px;
    padding: 10px;
}

/* ================ 基础控件 ================ */
Resident QLabel {
    color: #666666;
    font-size: 14px;
    margin: 5px 0;
}

/* ================ 布局间距 ================ */
Resident QHBoxLayout {
    spacing: 10px;
    margin: 0;
    padding: 0;
}
)");
    rightDownTabWidget->tabBar()->hide();
}

Resident::~Resident()
{
    //delete ui;
}


void Resident::initCurrentUserString()
{
    //通过当前id调sql函数获得名字
    currentUserString = SqlManager::GetInstance().GetNameById(StaticUserId);
    //如果没有名字则设置为默认值
    if (currentUserString.isEmpty()) {
        currentUserString = "业主";
    }
    //检查是否有未缴费记录
    bool hasUnpaidRecord = SqlManager::GetInstance().CheckPaymentRecordStatus(StaticUserId);
    if (hasUnpaidRecord) {
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

        manager->notify("缴费提醒", QString("您有未处理的缴费"));


    }
}

