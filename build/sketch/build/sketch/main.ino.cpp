#line 1 "c:\\Dev\\2017\\RebelMakers\\Arduino\\DalekbotSensors\\main.ino"
#line 1 "c:\\Dev\\2017\\RebelMakers\\Arduino\\DalekbotSensors\\main.ino"

#include "Arduino.h"
#include <NewPing.h>
#include <Wire.h>
#include <SPI.h>
#include <LSM303.h>

LSM303 compass;



volatile boolean process_it;
volatile byte slaveValueReceived, slaveValueSent;
int jj = 0;

volatile int a = 400;
int b = 450;
int c = 200;
volatile int currentDevice=0;
// we receive 4 bytes from the master.
// the returned data should be as follows.
// we need to return a 16 bit number so will have to bit shift it 

// byte[0] = this is 0 as it was set from previous packet.
// byte[1] = the selected sensor number
// bute[2] = the bit shifted number first 3 bits
// byte[3] = the last 3 bits.

//#####End  SPI Settings


////////////////////////////////////////////////
// Each time a request is made from the master device
// a value from the sensorValue array is returned.
// The sensorValue array is looped through and
// depending on the i2cMode, a different ranges of
// values are returned.
// When a value is received from the master device it changes
// the i2cMode
///////////////////////////////////////////////
unsigned long previousMillis = 0; // will store last time for loop
const long interval = 100;        // time  of loop

// the i2c addess used by both the master and slave device
#define SLAVE_ADDRESS 0x05

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

#define MAX_DISTANCE 200 // max distance in cm. same for all

//initialise the sensors
NewPing sonar1(TRIGGER_PIN_S1, ECHO_PIN_S1, MAX_DISTANCE);
NewPing sonar2(TRIGGER_PIN_S2, ECHO_PIN_S2, MAX_DISTANCE);
NewPing sonar3(TRIGGER_PIN_S3, ECHO_PIN_S3, MAX_DISTANCE);
NewPing sonar4(TRIGGER_PIN_S4, ECHO_PIN_S4, MAX_DISTANCE);

// if you change the sonars array change
// this number as well..
// remember it is a zero based array
const int NUMBER_OF_SENSORS = 4;

// create an array of sonar sensors
NewPing sonars[NUMBER_OF_SENSORS] = {
    sonar1,
    sonar2,
    sonar3,
    sonar4};

//not used now
//int returnValueForI2c = 0;

// i2cMode is set by the master device
int i2cMode = 1; // ALL = 1, ULTRASONIC = 2

//initialise the sensors values
//int sensorValues[4]={3,7,13,27};
// fifth one is Mag
int sensorValues[5];




void getMag()
{
  compass.read();
  float heading = compass.heading();
  Serial.println(heading);

  if (heading <= 360)
  {
    sensorValues[4]  =int(heading);
    // if (sensorValues[4]+ 20) 

  }
}


ISR(SPI_STC_vect)
{
  
  slaveValueReceived = SPDR;
  switch(SPDR){
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


    // send back the high bits
    case 200:
    SPDR = sensorValues[currentDevice]   >> 8;
    
    break;

    // send back the low bits
    case 201:
    SPDR = sensorValues[currentDevice]  & 0xff;
    break;


    case 255: // end of bytes sent in packet.
    //Serial.print("\n ");
    SPDR =0;
    break;

    default:
    Serial.print("    err: ");
    Serial.print(SPDR);
  }
}




void setup()
{
  Serial.begin(115200); // start serial for output
 
  Wire.begin();
  compass.init();
  compass.enableDefault();
  

    /*
  https://github.com/pololu/lsm303-arduino
  Calibration values; the default values of +/-32767 for each axis
  lead to an assumed magnetometer bias of 0. Use the Calibrate example
  program to determine appropriate values for your particular unit.
  */
  // compass.m_min = (LSM303::vector<int16_t>){-334, -717, -584}; 
  // compass.m_max = (LSM303::vector<int16_t>){+556, +211, +344}; 
  // compass.m_min = (LSM303::vector<int16_t>){-639, -1047, -559};
  // compass.m_max = (LSM303::vector<int16_t>){+706, +439, +662};
  // compass.m_min = (LSM303::vector<int16_t>){-304, -736, -528};
  // compass.m_max = (LSM303::vector<int16_t>){+513, +402, +570};


  // use these on the adafruit board
  // compass.m_min = (LSM303::vector<int16_t>){-627, -694, -536};
  // compass.m_max = (LSM303::vector<int16_t>){+472, +356, +141};

 // min: {  -673,   -613,   -754}    max: {  +407,   +491,   +323}
  compass.m_min = (LSM303::vector<int16_t>){-673, -613, -754};
  compass.m_max = (LSM303::vector<int16_t>){+407, +491, +323};

  // min: {  -304,   -862,   -541}    max: {  +513,   +402,   +570
   //min: {  -304,   -736,   -528}    max: {  +562,   +499,   +498
  // min: {  -639,  -1047,   -559}    max: {  +706,   +439,   +662}
     

  // min: {  -308,   -548,   -536}    max: {  +347,   +102,   -405}
  // min: {  -627,   -694,   -536}    max: {  +472,   +356,   -141}

  //Spi setup
  pinMode(MISO, OUTPUT);
  // turn on SPI in slave mode.
  SPCR |= _BV(SPE);
  // get ready for an interrupt
  process_it = false;
  // now turn on the interrupts
  SPI.attachInterrupt();


  Serial.println("Ready!");
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
  int asize =sizeof(sensorValues) ;
  for (int i = 0; i < 5 ; i++)
  {
    Serial.print(i);
    Serial.print(":");
    Serial.print(sensorValues[i]);
    Serial.print(" ");
  }
  Serial.print(" \n");
}

void loop()
{
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;

    for (int i = 0; i < NUMBER_OF_SENSORS; i++)
    {
       
      int tempval = sonars[i].ping_cm();

      //only change value if it has changed.
      if (tempval != sensorValues[i])
      {
        //print values to serial
       // printValue(i, tempval);
        //set values
        sensorValues[i] = tempval;
      }
      // sensor out of range so set to maximum
      if ( tempval == 0){
        sensorValues[i] = 254;
      }
    }
    getMag();
     PrintAllData();
  }
}

