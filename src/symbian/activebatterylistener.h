#ifndef ACTIVEBATTERYLISTENER_H
#define ACTIVEBATTERYLISTENER_H

#include <Etel3rdParty.h>

class BatteryObserver
{
    friend class ActiveBatteryListener;
private:
    virtual void batteryLevelChaged(TUint aChargeStatus, CTelephony::TBatteryStatus aStatus) = 0;
};

class ActiveBatteryListener : public CActive
{
public:
    ~ActiveBatteryListener();
    static ActiveBatteryListener* NewL(BatteryObserver& aObserver);
    static ActiveBatteryListener* NewLC(BatteryObserver& aObserver);

private:
    ActiveBatteryListener(BatteryObserver& aObserver);
    void ConstructL(void);

private:
    void GetBatteryInfo();
    void RunL();
    void DoCancel();

private:
    BatteryObserver&               iObserver;
    CTelephony*                     iTelephony;
    CTelephony::TBatteryInfoV1      iBatteryInfoV1;
    CTelephony::TBatteryInfoV1Pckg  iBatteryInfoV1Pckg;
    TBool                           iGettingBattery;
};

#endif // ACTIVEBATTERYLISTENER_H
