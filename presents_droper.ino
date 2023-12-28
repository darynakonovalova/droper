#include <Servo.h>


/*Parameters*/
const int BAUDRATE                   = 9600;
// TODO: PINS == SERVOS_NUM always, do we need PINS?
const int PINS                       = 12;
const int SERVOS_NUM                 = 12;
const int CHANNELS_NUM               = 3;
// TODO: DIGITAL_OUTPUT not used since digitalPinToInterrupt
const int DIGITAL_OUTPUT             = 3;
const int INPUT_PIN                  = 20;
const int PWM_STEP                   = 20;
const int LOWER_PWM_BOUNDARY         = 802;
const int UPPER_PWM_BOUNDARY         = 818;

Servo allServos[SERVOS_NUM];
volatile unsigned long highStartTime = 0;
volatile unsigned long pulseWidth = 0; 
unsigned long prevHighStartTime = 0;
unsigned long period = 0;

void InitPins(int pinsNum) 
{
    for (int i = 0; i < pinsNum; i++) 
    {
        pinMode(i + 2, OUTPUT);
    }
    pinMode(INPUT_PIN, INPUT_PULLUP);
} 

void AttachServos(int servosNum) 
{
    for (int i = 0; i < servosNum; i++) 
    {
        allServos[i].attach(i + 2);
    }
}

void HandleInterrupt() 
{
    if (digitalRead(INPUT_PIN) == HIGH) 
    {
        highStartTime = micros();
    } 
    else 
    {
        pulseWidth = micros() - highStartTime;
    }
}

void SetPWMAngle(Servo servo, byte channel) 
{
    // TODO: how to move servo to "closed" position?
    if (channel == 1) servo.writeMicroseconds(1200); //1 angle PWM in us
    if (channel == 2) servo.writeMicroseconds(1600); //2 angle PWM in us
    if (channel == 3) servo.writeMicroseconds(2000); //3 angle PWM in us
}

bool IsSignalValid(unsigned long signalPeriod) 
{
    // TODO: refine the acceptable pwm range
    if (signalPeriod >= 3000) 
    {
        return true;
    } 
    else
    {
        return false;
    }
}

void Drop(unsigned long signalDuty) 
{
    // TODO: if signalDuty is outside the working range, skip the following loop
    for (int channel = 0; channel < CHANNELS_NUM; channel++)
    {
        for (int servo = 0; servo < SERVOS_NUM; servo++)
        {
            int lowerBound = (channel * SERVOS_NUM + servo) * PWM_STEP + LOWER_PWM_BOUNDARY;
            int upperBound = (channel * SERVOS_NUM + servo) * PWM_STEP + UPPER_PWM_BOUNDARY;

            if (lowerBound < signalDuty && signalDuty < upperBound)
            {
                SetPWMAngle(allServos[servo], channel + 1);
                Serial.print("Was dropped from channel ");
                Serial.print(channel + 1);
                Serial.print(" from servo: ");
                Serial.println(servo);
            }
        }
    }

    if (signalDuty >= 2200) //to drop all
    {
        for (int channel = 0; channel < CHANNELS_NUM; channel++)
        {
            for (int servo = 0; servo < SERVOS_NUM; servo++)
            {
                SetPWMAngle(allServos[servo], channel + 1);
            }
        }
        // TODO: if there were no drops, do not send the message (flooding)
        Serial.println("All were dropped");
    }
}

void setup() 
{
    Serial.begin(BAUDRATE);
    Serial.println();

    InitPins(PINS);
    AttachServos(SERVOS_NUM);

    attachInterrupt(digitalPinToInterrupt(INPUT_PIN), HandleInterrupt, CHANGE);

    delay(1000);
}

void loop() 
{
    if (prevHighStartTime != highStartTime)
    {
        period = highStartTime - prevHighStartTime;
        prevHighStartTime = highStartTime;
    }
    
    if (IsSignalValid(period)) 
    {
        Drop(pulseWidth);
    }
    else
    {
        Serial.println("Invalid signal");
    }        
}
