#ifndef SYMBIANPHONE_H
#define SYMBIANPHONE_H

#include "phone.h"

/* Media keys */

#include <remconcoreapitargetobserver.h>
#include <remconcoreapitarget.h>
#include <remconinterfaceselector.h>

/* Other keys*/

#define LEFT_KEY 63554
#define RIGHT_KEY 63555

#define GREEN_KEY 63586
#define RED_KEY 63587

#define C_KEY 8

#define UP_ARROW 63497
#define RIGHT_ARROW 63496
#define BOTTOM_ARROW 63498
#define LEFT_ARROW 63495

#define NUMBER_0 48
#define NUMBER_1 49
#define NUMBER_2 50
#define NUMBER_3 51
#define NUMBER_4 52
#define NUMBER_5 53
#define NUMBER_6 54
#define NUMBER_7 55
#define NUMBER_8 56
#define NUMBER_9 57

#define STAR 42
#define SHARP 35

/* Phone calls */

#include "activecallslistener.h"

/* Battery */

#include "activebatterylistener.h"

/* Keypad lock */

#include <aknkeylock.h>

class SymbianPhone : public Phone, private MRemConCoreApiTargetObserver, public BatteryObserver, public CallsObserver
{
public:
    SymbianPhone(QObject* parent = 0);

    ~SymbianPhone();

    void handleKeyEventCode(int keyCode);

    uint getCurrentBatteryCharge();

public slots:
    void lockKeypad();

private:
    void MrccatoCommand(TRemConCoreApiOperationId aOperationId, TRemConCoreApiButtonAction aButtonAct);

    void batteryLevelChaged(TUint aChargeStatus, CTelephony::TBatteryStatus aStatus);
    void callStatusChanged(CTelephony::TCallStatus& aStatus, TInt aError);

    CRemConInterfaceSelector* iInterfaceSelector;
    CRemConCoreApiTarget*     iCoreTarget;

    ActiveCallsListener* callsListener;
    ActiveBatteryListener* batteryListener;

    uint lastBatteryLevel;

};

#endif // SYMBIANPHONE_H
