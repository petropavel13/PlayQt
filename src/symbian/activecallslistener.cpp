#include "activecallslistener.h"

ActiveCallsListener* ActiveCallsListener::NewLC(CallsObserver& aObserver)
{
    ActiveCallsListener* self = new (ELeave) ActiveCallsListener(aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

ActiveCallsListener* ActiveCallsListener::NewL(CallsObserver& aObserver)
{
    ActiveCallsListener* self = ActiveCallsListener::NewLC(aObserver);
    CleanupStack::Pop(); // self;
    return self;
}


ActiveCallsListener::ActiveCallsListener(CallsObserver& aCallBack)
    :CActive(EPriorityStandard),iCallBack(aCallBack),iCurrentStatusPckg(iCurrentStatus)
{
    CActiveScheduler::Add(this);
}

ActiveCallsListener::~ActiveCallsListener()
{
    Cancel();
    delete iTelephony;
}

void ActiveCallsListener::ConstructL(void)
{
    iTelephony = CTelephony::NewL();
    StartListening();
}

void ActiveCallsListener::CancelOperation(void)
{
    Cancel();
}

void ActiveCallsListener::DoCancel()
{
    iTelephony->CancelAsync(CTelephony::EVoiceLineStatusChangeCancel);
}


void ActiveCallsListener::RunL()
{
    iCallBack.callStatusChanged(iCurrentStatus.iStatus,iStatus.Int());
    if(iStatus != KErrCancel)
        StartListening();
}

void ActiveCallsListener::StartListening()
{
    Cancel();
    iCurrentStatus.iStatus = CTelephony::EStatusUnknown;
    iTelephony->NotifyChange(iStatus,CTelephony::EVoiceLineStatusChange,iCurrentStatusPckg);
    SetActive();
}
