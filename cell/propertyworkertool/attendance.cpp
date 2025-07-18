#include "attendance.h"
#include "ui_attendance.h"
#include "lib/sqlmanager.h"
#include <QDebug>
#include <QMutexLocker>

Attendance::Attendance(QWidget *parent)
    : MyWidget(parent)
    , ui(new Ui::Attendance)
    , model(new MyEditableRowModel(this))
{
    ui->setupUi(this);

    // 设置表格模型
    ui->tb_v_workPerson->setModel(model);
    model->setColumnCount(4);
    model->setHorizontalHeaderLabels({"日期", "出勤状态", "上班打卡时间", "下班打卡时间"});

    // 表格配置
    ui->tb_v_workPerson->horizontalHeader()->setVisible(true);
    ui->tb_v_workPerson->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_v_workPerson->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tb_v_workPerson->verticalHeader()->setDefaultSectionSize(40);
    ui->tb_v_workPerson->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tb_v_workPerson->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tb_v_workPerson->setSelectionBehavior(QAbstractItemView::SelectRows);

    // 初始化数据
    init();
}

Attendance::~Attendance()
{
    delete ui;
}

void Attendance::init()
{
    // 确保有有效的用户ID
    if (StaticUserId.isEmpty()) {
        qWarning() << "无法加载考勤数据: 用户ID为空";
        return;
    }

    // 加锁保护数据访问
    QMutexLocker locker(&dataMutex);

    // 获取数据
    attendanceData = SqlManager::GetInstance().GetAttendanceData(StaticUserId);

    // 清空现有数据
    model->removeRows(0, model->rowCount());

    // 填充表格
    for (int i = 0; i < attendanceData.size(); ++i) {
        const auto& rowData = attendanceData[i];

        // 确保有足够的列数据
        QList<QStandardItem*> items;
        for (int j = 0; j < 4; j++) {
            QString text = (j < rowData.size()) ? rowData[j] : "";
            items << new QStandardItem(text);
        }
        model->appendRow(items);
    }
}
