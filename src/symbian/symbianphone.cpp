#include "symbianphone.h"

SymbianPhone::SymbianPhone(QObject *parent):
    Phone(parent)
{
    lastBatteryLevel = 100;
    QT_TRAP_THROWING(iInterfaceSelector = CRemConInterfaceSelector::NewL());
    QT_TRAP_THROWING(iCoreTarget = CRemConCoreApiTarget::NewL(*iInterfaceSelector, *this));
    iInterfaceSelector->OpenTargetL();

    callsListener = ActiveCallsListener::NewL(*this);
    batteryListener = ActiveBatteryListener::NewL(*this);
}

void SymbianPhone::MrccatoCommand(TRemConCoreApiOperationId aOperationId, TRemConCoreApiButtonAction aButtonAct)
{
    switch( aOperationId )
    {
    case ERemConCoreApiPlay:
        emit playKeyPressed();
        break;
    case ERemConCoreApiStop:
        emit stopKeyPressed();
        break;
    case ERemConCoreApiPause:
        emit pauseKeyPressed();
        break;
    case ERemConCoreApiForward:
        emit forwardKeyPressed();
        break;
    case ERemConCoreApiBackward:
        emit backwardKeyPressed();
        break;
    case ERemConCoreApiVolumeUp:
        emit volumeUpKeyPressed();
        break;
    case ERemConCoreApiVolumeDown:
        emit volumeDownKeyPressed();
        break;
    default:
        break;
    }
}

uint SymbianPhone::getCurrentBatteryCharge()
{
    return lastBatteryLevel;
}

void SymbianPhone::lockKeypad()
{
    RAknKeyLock keyLock;

    User::LeaveIfError(keyLock.Connect());

    keyLock.EnableKeyLock();

    keyLock.Close();
}

void SymbianPhone::batteryLevelChaged(TUint aChargeStatus, CTelephony::TBatteryStatus aStatus)
{
    lastBatteryLevel = aChargeStatus;
    emit batteryChargeChanged(aChargeStatus);
}

void SymbianPhone::callStatusChanged(CTelephony::TCallStatus &aStatus, TInt aError)
{
    switch (aStatus)
    {
    case CTelephony::EStatusUnknown:
        break;
    case CTelephony::EStatusIdle:
        emit idle();
        break;
    case CTelephony::EStatusDialling:
        break;
    case CTelephony::EStatusRinging:
        emit ringing();
        break;
    case CTelephony::EStatusAnswering:
        break;
    case CTelephony::EStatusConnecting:
        break;
    case CTelephony::EStatusConnected:
        break;
    case CTelephony::EStatusReconnectPending:
        break;
    case CTelephony::EStatusDisconnecting:
        break;
    case CTelephony::EStatusHold:
        break;
    case CTelephony::EStatusTransferring:
        break;
    case CTelephony::EStatusTransferAlerting:
        break;
    default:
        break;
    }
}

SymbianPhone::~SymbianPhone()
{
    delete callsListener;
    delete batteryListener;
}

void SymbianPhone::handleKeyEventCode(int keyCode)
{
    switch(keyCode)
    {
    case LEFT_KEY:
        emit leftSoftKeyPressed();
        break;
    case RIGHT_KEY:
        emit rightSoftKeyPressed();
        break;
    case C_KEY:
        emit deleteKeyPressed();
        break;
    case GREEN_KEY:
        emit greenKeyPressed();
        break;
    case RED_KEY:
        emit redKeyPressed();
        break;
    case NUMBER_1:
        emit number1KeyPressed();
        break;
    case NUMBER_2:
        emit number2KeyPressed();
        break;
    case NUMBER_3:
        emit number3KeyPressed();
        break;
    case NUMBER_4:
        emit number4KeyPressed();
        break;
    case NUMBER_5:
        emit number5KeyPressed();
        break;
    case NUMBER_6:
        emit number6KeyPressed();
        break;
    case NUMBER_7:
        emit number7KeyPressed();
        break;
    case NUMBER_8:
        emit number8KeyPressed();
        break;
    case NUMBER_9:
        emit number9KeyPressed();
        break;
    case STAR:
        emit starKeyPressed();
        break;
    case SHARP:
        emit sharpKeyPressed();
        break;
    default:
        break;
    }
}
