#include <QTRSensors.h> // Pololu's Library for the Reflective sensor
#include <SoftwareSerial.h>

//Pololu ULT8 Sensor Pins
const uint8_t refLED=10;
const uint8_t ref1=9;
const uint8_t ref2=8;
const uint8_t ref3=7; 
const uint8_t ref4=6; 
const uint8_t ref5=5; 
const uint8_t ref6=4; 
const uint8_t ref7=3; 
const uint8_t ref8=2; 
const uint8_t SensorCount = 8; //number of sensor pins
uint16_t sensorValues[SensorCount]; //Array to store sensor values
uint16_t pos;

//UART Pins
const int ARD_RX = 11;
const int ARD_TX = 12;

QTRSensors qtr;
SoftwareSerial prop(ARD_RX, ARD_TX); // RX, TX


void setup() {
  
  delay(50);// (Not necessary but...) wait for a bit of time to ensure everything has booted up
  qtr.setTypeRC(); // or setTypeAnalog() Set QTR Sensor for Digital mode
  qtr.setSensorPins((const uint8_t[]){ref1, ref2, ref3, ref4, ref5, ref6, ref7, ref8}, SensorCount); // Tells the QTR Library the pins connected to the QTR Sensor and the number of pins
  qtr.setEmitterPin(refLED);
  
  
  prop.begin(115200);

  char com;
  while(com!='c'){
    while(!prop.available()) {
    }
    com=prop.read();
  }
  /*******************************************************************************/
  ////////////////////////////Callibration/////////////////////////////////////

  
  prop.print(9000);
  prop.print('\n');
  while(com!='d'){
    if(prop.available()){
      com=prop.read();
      if(com=='d'){
        break;
      }
    }
    qtr.calibrate();
    if(prop.available()){
      com=prop.read();
      if(com=='d'){
        break;
      }
    }
    delay(10); 
  }
  
  prop.print(9500);
  prop.print('\n');
  /********************************************************************************/
}

void loop() {
  pos=qtr.readLineBlack(sensorValues);
  prop.print(pos);
  for(int i=0;i<SensorCount;i++){
    prop.print('\t');
    prop.print(sensorValues[i]);
    
  }
  prop.print('\n');
  delay(10);
}
