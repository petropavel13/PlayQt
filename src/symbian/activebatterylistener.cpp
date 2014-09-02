#include "activebatterylistener.h"

ActiveBatteryListener* ActiveBatteryListener::NewLC(BatteryObserver& aObserver)
{
    ActiveBatteryListener* self = new (ELeave) ActiveBatteryListener(aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

ActiveBatteryListener* ActiveBatteryListener::NewL(BatteryObserver& aObserver)
{
    ActiveBatteryListener* self = ActiveBatteryListener::NewLC(aObserver);
    CleanupStack::Pop(); // self;
    return self;
}

void ActiveBatteryListener::ConstructL(void)
{
    iTelephony = CTelephony::NewL();
    GetBatteryInfo();
}

ActiveBatteryListener::~ActiveBatteryListener()
{
    Cancel();
    delete iTelephony;
}

ActiveBatteryListener::ActiveBatteryListener(BatteryObserver& aObserver)
    : CActive(EPriorityStandard),iObserver(aObserver),iBatteryInfoV1Pckg(iBatteryInfoV1)
{
    CActiveScheduler::Add(this);
}

void ActiveBatteryListener::GetBatteryInfo()
{
    if(iTelephony && !IsActive())
    {
        iGettingBattery = ETrue;
        iTelephony->GetBatteryInfo(iStatus, iBatteryInfoV1Pckg);
        SetActive();
    }
}

void ActiveBatteryListener::RunL()
{
    iObserver.batteryLevelChaged(iBatteryInfoV1.iChargeLevel,iBatteryInfoV1.iStatus);

    if(iStatus != KErrCancel)
    {
        iTelephony->NotifyChange(iStatus,CTelephony::EBatteryInfoChange,
                                 iBatteryInfoV1Pckg);
        SetActive();
    }

    iGettingBattery = EFalse;
}

void ActiveBatteryListener::DoCancel()
{
    if(iGettingBattery)
        iTelephony->CancelAsync(CTelephony::EGetBatteryInfoCancel);
    else
        iTelephony->CancelAsync(CTelephony::EBatteryInfoChangeCancel);
}
