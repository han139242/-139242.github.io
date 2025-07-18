#include <QApplication>
#include <QMessageBox>
#include "logindialog.h"
#include "propertyworker.h"
#include "resident.h"
#include "propertymanager.h"
#include "lib/sqlmanager.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // 初始化数据库
    SqlManager &sqlManager = SqlManager::GetInstance();
    if (!sqlManager.InitDatabase()) {
        return 0; // 如果数据库初始化失败，退出程序
        //错误弹窗 请检查文件是否完整
        QMessageBox::critical(nullptr, "错误", "数据库初始化失败，请检查文件是否完整！");
    }
    loginDialog c;
    c.show();
    int ret=c.exec();
    if(3==ret){
        propertyWorker w;
        w.show();
        return a.exec();
    }
    else if(2==ret){
        Resident r;
        r.show();
        return a.exec();
    }
    if(1==ret){
        PropertyManager p;
        p.show();
        return a.exec();
    }

    return a.exec();
}
