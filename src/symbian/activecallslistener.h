#ifndef ACTIVECALLSLISTENER_H
#define ACTIVECALLSLISTENER_H

#include <Etel3rdParty.h>

class CallsObserver
{
    friend class ActiveCallsListener;
private:
    virtual void callStatusChanged(CTelephony::TCallStatus& aStatus, TInt aError)=0;
};

class ActiveCallsListener : public CActive
{
public:
    ~ActiveCallsListener();
    static ActiveCallsListener* NewLC(CallsObserver& aObserver);
    static ActiveCallsListener* NewL(CallsObserver& aObserver);

private:
    ActiveCallsListener(CallsObserver& aCallBack);
    void ConstructL();

protected:
    void DoCancel();
    void RunL();

private:
    void CancelOperation(void);
    void StartListening();

private:
    CallsObserver&                iCallBack;
    TInt                          iState;
    CTelephony::TCallStatusV1     iCurrentStatus;
    CTelephony::TCallStatusV1Pckg iCurrentStatusPckg;
    CTelephony*                   iTelephony;
};

#endif // ACTIVECALLSLISTENER_H
