#include "Arduino.h"
#include <NewPing.h>
#include <SPI.h>

volatile byte slaveValueReceived;

// currentDevice is set from the Spi bus,
// once set, that device's data is sent back over the bus
volatile int currentDevice = 0;

// we receive 4 bytes from the master.
// the returned data should be as follows.
// we need to return a 16 bit number so will have to bit shift it

// byte[0] = this is 0 as it was set from previous packet.
// byte[1] = the selected sensor number
// byte[2] = the bit shifted number first 3 bits
// byte[3] = the last 3 bits.

unsigned long previousMillis = 0; // will store last time for loop
volatile long interval = 20;      // time  of loop

// sensor board pinout
// sensor : trigger : Echo
// 1      : 2       : 5
// 2      : 7       : 6
// 3      : 8       : 3
// 4      : 9       : 4

//Sensor 1
#define TRIGGER_PIN_S1 2
#define ECHO_PIN_S1 5 // you  can use a single pin for trigger/echo if the board is a 5v one.
//Sensor 2
#define TRIGGER_PIN_S2 7
#define ECHO_PIN_S2 6
//Sensor 3
#define TRIGGER_PIN_S3 8
#define ECHO_PIN_S3 3
//Sensor 4
#define TRIGGER_PIN_S4 9
#define ECHO_PIN_S4 4

#define MAX_DISTANCE 100 // max distance in cm. same for all

//initialise the sensors
NewPing sonar1(TRIGGER_PIN_S1, ECHO_PIN_S1, MAX_DISTANCE);
NewPing sonar2(TRIGGER_PIN_S2, ECHO_PIN_S2, MAX_DISTANCE);
NewPing sonar3(TRIGGER_PIN_S3, ECHO_PIN_S3, MAX_DISTANCE);
NewPing sonar4(TRIGGER_PIN_S4, ECHO_PIN_S4, MAX_DISTANCE);

// if you change the sonars array change
// this number as well..
const int NUMBER_OF_SENSORS = 4;

// create an array of sonar sensors
NewPing sonars[NUMBER_OF_SENSORS] = {
    sonar1,
    sonar2,
    sonar3,
    sonar4};

// deviceMode is for speeding up reading of the device
// if we only need one sensor then we can only read that one
//mode 10: all data is read
//mode 11: left ping only
//mode 12: right ping only
//mode 13: center ping only
//mode 14: compass only
// mode 15: all data is read and debug output
volatile int deviceMode = 10;

//initialise the sensors values
//int sensorValues[4]={3,7,13,27};
int sensorValues[4] = {0, 0, 0, 0};

ISR(SPI_STC_vect)
{
  slaveValueReceived = SPDR;
  switch (SPDR)
  {
  case 0:
    //Serial.print("Current device: 0");
    currentDevice = SPDR;

    break;
  case 1:
    //Serial.print("Current device: 1");
    currentDevice = SPDR;

    break;
  case 2:
    // Serial.print("Current device: 2");
    currentDevice = SPDR;

    break;
  case 3:
    //Serial.print("Current device: 3");
    currentDevice = SPDR;
    break;

  case 4:
    // Serial.print("Current device: 4");
    currentDevice = SPDR;
    break;

  case 5:
    // Serial.print("Current device: 5");
    currentDevice = SPDR;
    break;

  case 10:
    deviceMode == 10;
    interval = 80;
    break;
  case 11:
    deviceMode == 11;
    interval = 20;
    break;
  case 12:
    deviceMode == 12;
    interval = 20;
    break;
  case 13:
    deviceMode == 13;
    interval = 20;
    break;
  case 14:
    deviceMode == 15;
    interval = 20;
    break;
  case 15:
    deviceMode == 15;
    interval = 80;
    break;

  // send back the high bits
  case 200:
    SPDR = sensorValues[currentDevice] >> 8;

    break;

  // send back the low bits
  case 201:
    SPDR = sensorValues[currentDevice] & 0xff;
    break;

  case 255: // end of bytes sent in packet.
    //Serial.print("\n ");
    SPDR = 0;
    break;

  default:
    Serial.print("    err: ");
    Serial.print(SPDR);
  }
}

void setup()
{
  Serial.begin(115200); // start serial for output

  // Wire.begin();

  //Spi setup
  pinMode(MISO, OUTPUT);
  // turn on SPI in slave mode.
  SPCR |= _BV(SPE);
  // now turn on the interrupts
  SPI.attachInterrupt();

  Serial.println("Ready!");
  pinMode(LED_BUILTIN, OUTPUT);
}

void printValue(int sensorId, int distance)
{
  Serial.print("PingSensor");
  Serial.print(++sensorId);
  Serial.print(": ");
  Serial.print(distance);
  Serial.println("cm");
}

void PrintAllData()
{

  int asize = sizeof(sensorValues);
  for (int i = 0; i < NUMBER_OF_SENSORS; i++)
  {
    Serial.print(" ");
    Serial.print(i);
    Serial.print(":");
    Serial.print(sensorValues[i]);
  }
  Serial.print(" \n");
}

void getAllData()
{
  for (int i = 0; i < NUMBER_OF_SENSORS; i++)
  {

    int tempval = sonars[i].ping_cm();

    //only change value if it has changed.
    if (tempval != sensorValues[i])
    {
      //set values
      sensorValues[i] = tempval;
    }
  }
}

void loop()
{
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;

    switch (deviceMode)
    {
    case 10:
      // get all sensor values
      getAllData();
      break;
    case 11:
      // leftPing only
      sensorValues[0] = sonars[0].ping_cm();
      break;
    case 12:
      //rightPing only
      sensorValues[1] = sonars[1].ping_cm();
      break;
    case 13:
      // centerPing only
      sensorValues[2] = sonars[2].ping_cm();
      break;
    // case 14:
    // compass only
    // getMag();
    case 15:
      // all data is read and debug output
      getAllData();
      PrintAllData();
      break;
    default:
      Serial.print("    err: loop deviceMode not set");
    }
  }
}
