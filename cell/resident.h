#ifndef RESIDENT_H
#define RESIDENT_H

#include <QWidget>
#include "residenttool/faultrepair.h"
#include "residenttool/myparking.h"
#include "residenttool/parkingapplication.h"
#include "residenttool/paymentquery.h"
#include "residenttool/selfpayment.h"
#include "residenttool/repairevaluation.h"
#include "residenttool/repairprocess.h"
#include "residenttool/noticebound.h"



class Resident : public QWidget
{
    Q_OBJECT

public:
    explicit Resident(QWidget *parent = nullptr);
    ~Resident();

private:
    //Ui::Resident *ui;
    FaultRepair *faultRepairTab;
    MyParking *myParkingTab;
    ParkingApplication *parkingApplicationTab;
    PaymentQuery *paymentQueryTab;
    SelfPayment *selfPaymentTab;
    RepairEvaluation *repairEvaluationTab;
    RepairProcess *repairProcessTab;
    NoticeBound *noticeBoundTab;
    QString currentUserString="阿布 杜古里";
    void initCurrentUserString();
};

#endif // RESIDENT_H
