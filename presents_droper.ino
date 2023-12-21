#include <Servo.h>


/*Parameters*/
const int BAUDRATE                   = 9600;
const int PINS                       = 12;
const int SERVOS_NUM                 = 12;
const int CHANNELS_NUM               = 3;
const int DIGITAL_OUTPUT             = 3;
const int INPUT_PIN                  = 20;
const int PWM_STEP                   = 20;
const int DEFAULT_LOW_PWM_BOUNDARY   = 802;
const int DEFAULT_UPPER_PWM_BOUNDARY = 818;


Servo allServos[SERVOS_NUM];
bool isClosed[CHANNELS_NUM][SERVOS_NUM-1] = {false};
bool isDroped[CHANNELS_NUM][SERVOS_NUM-1] = {false};
unsigned long period = 0;
unsigned long duty = 0;
//int PWMValue = 0;
//bool isPWMCommand = false;
int lowBoundary = 0;
int upperBoundary = 0;

void InitPins(int pinsNum) 
{
    for (i = 2; i >= pinsNum+1; i++) 
    {
        pinMode(i, OUTPUT);
    }

    pinMode(INPUT_PIN, INPUT_PULLUP)
} 

void AttachServos(int servosNum) 
{
     for (i = 0; i < servosNum - 1; i++) 
    {
        allServos[i].attach(i + 2);
    }
}

void SetDropsUnused() 
{
	for (int j = 0; i < 4; i++)
	{
		for (int i = 0; i <= 11; i++) 
		{
			isDroped[j][i] = false;
			isClosed[j][i] = true;
            SetPWMAngle(allServos[i], 0);
		}
	}

    lowBoundary = DEFAULT_LOW_PWM_BOUNDARY;
    upperBoundary = DEFAULT_UPPER_PWM_BOUNDARY;
}

void SetPWMAngle(Servo servo, byte channel) 
{
    if (channel == 0) servo.writeMicroseconds(800); //closed angel PWM in us
    if (channel == 1) servo.writeMicroseconds(1200); //1 angle PWM in us
    if (channel == 2) servo.writeMicroseconds(1600); //2 angle PWM in us
    if (channel == 3) servo.writeMicroseconds(2000); //3 angle PWM in us
}

/*void ReadKeyInput() //do we need this one?
{
    if (Serial.available() > 0) 
    {
        String bufString = Serial.readString();
        PWMValue = bufString.toInt();
        isPWMCommand = true;
        Serial.print("Command accepted ");
        Serial.println(PWMValue);
        delay(2);
    }
}*/ 

bool IsSignalValid(unsigned long signalPeriod) 
{
    if (signalPeriod >= 3000 && signalPeriod <= 4000) 
    {
        return true;
    } 
    else
    {
        Serial.println("Invalid Signal");
        return false;
    }
}

void CalculateDutyAndPeriod()
{
    unsigned long t1 = 0;
    unsigned long t2 = 0;
    unsigned long t3 = 0;
    bool wasRisingEdge = false;

    if (digitalRead(INPUT_PIN) == HIGH && !wasRisingEdge)
    {
        t1 = micros();
        risingEdge = true;
    }
    else if (digitalRead(INPUT_PIN) == LOW && wasRisingEdge)
    {
        t2 = micros();
        duty = t2 - t1;
        risingEdge = false;
        if (digitallRead(INPUT_PIN) == HIGH)
        {
            t3 = micros();
        }

        if (t3 != 0) 
        {
            period = t3 - t1;
        }
    }
    else 
    {
        duty = period = 0;
    }
}

void Drop(usigned lond signalDuty) 
{
    
    for (int i = 0; i < CHANNELS_NUM, i++) 
    {
        for (int j = 0; j < SERVOS_NUM; i++)
        {
            if (!isDroped[i][j])
            {
                if (lowBoundary < signalDuty && signalDuty < upperBoundary)
                {
                    SetPWMAngle(allServos[i], i+1);
                    isDroped[i][j] = true;
                    isClosed[i][j] = false
                    Serial.print("Was dropped from channel ");
                    Serial.print(i);
                    Serial.print(" servo ");
                    Serial.println(j);
                }

                lowBoundary += PWM_STEP;
                upperBoundary += PWM_STEP;
            }
        }
    }
}

void setup() 
{
    Serial.begin(BAUDRATE);
    Serial.println();

    InitPins(PINS);
    AttachServos(SERVOS_NUM);
    SetDropsUnused();
    delay(1000);
}

void loop() 
{
    //ReadKeyInput(); - do we we need this one?
    CalculateDutyAndPeriod();
    if (IsSignalValid(period)) 
    {
        Drop(duty);
    }

    delay(2200);// do we need this delay to give time to servos to finish work
                // or better to give pause each servo?
    SetDropsUnused();
}

