#ifndef PHONE_H
#define PHONE_H

#include <QObject>

class Phone : public QObject
{
    Q_OBJECT
public:
    explicit Phone(QObject *parent = 0);

    virtual void handleKeyEventCode(int keyCode) = 0;

    virtual uint getCurrentBatteryCharge() = 0;

signals:
    /* Keys */
    void playKeyPressed();
    void pauseKeyPressed();
    void stopKeyPressed();

    void backwardKeyPressed();
    void forwardKeyPressed();

    void volumeUpKeyPressed();
    void volumeDownKeyPressed();

    void greenKeyPressed();
    void redKeyPressed();

    void leftSoftKeyPressed();
    void rightSoftKeyPressed();

    void deleteKeyPressed();

    void number0KeyPressed();
    void number1KeyPressed();
    void number2KeyPressed();
    void number3KeyPressed();
    void number4KeyPressed();
    void number5KeyPressed();
    void number6KeyPressed();
    void number7KeyPressed();
    void number8KeyPressed();
    void number9KeyPressed();

    void starKeyPressed();
    void sharpKeyPressed();

    /* Calls */

    void ringing();
    void idle();

    /* Battery */
    void batteryChargeChanged(uint charge);

public slots:
    virtual void lockKeypad() = 0;
};

#endif // PHONE_H
