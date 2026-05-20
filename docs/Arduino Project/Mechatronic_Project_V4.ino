#include <QTRSensors.h> // Pololu's Library for the Reflective sensor
#include <Servo.h>

//Servo Motor Pins
int leftServoPin=10;
int rightServoPin=11;

//LED Indicator Pins
const int leftLED=12;  //Blue Led
const int rightLED=13; //Red Led

//Left UltraSonic Sensor Pins
const int leftUltTrig=5;
const int leftUltEcho=4;

//Front UltraSonic Sensor Pins
const int frontUltTrig=2;
const int frontUltEcho=3;

//Right UltraSonic Sensor Pins
const int rightUltTrig=6;
const int rightUltEcho=7;

//Pololu ULT8 Sensor Pins
const int ref1=8;
const int ref2=9;
const int ref3=14; //A0
const int ref4=15; //A1
const int ref5=16; //A2
const int ref6=17; //A3
const int ref7=18; //A4
const int ref8=19; //A5
const uint8_t SensorCount = 8; //number of sensor pins
uint16_t sensorValues[SensorCount]; //Array to store sensor values

const unsigned int maxDist=40; //Maximum distance of detectable object from ultrasonic sensor;
const unsigned int stopDist=7; // Minimum distance to stop in front of object
const int desiredPos=3500; //Used as reference position for line following
const int refThresh=725; //Threshold used for detecting intersections
const int errorThresh=250; // Threshold used for error correction
int pos = 0;  //Variable to store Position from Reflectance Sensor
int errorVal = 0; // Used to determine how much Position has deviated from the desired Position

byte stage=0;//Used to differenciate sections/ stages of track
byte interCount=0;//Variable to store the number of passed intersections
byte numInter=5;//Total number of intersections on track;

bool objRight=false;//Indicates whether an object was detected on the right;
bool objLeft=false;//Indicates whether an object was detected on the left;

bool curveFlag=false;//Make true if home curve radii are quite large
bool doneFlag=false;//Gets set after final delivery at C1;

Servo leftServo; //create servo object for Left Servo motor
Servo rightServo;//create servo object for Right Servo motor

QTRSensors qtr; // Create Object for Reflectance Sensor 

void setup() {
  DDRB=(DDRB&B11000000)|B00111100;//Set Pins 8-9 as inputs and pins 10-13 as outputs
  DDRD=(DDRD&B00000011)|B01100100;//Set Pins 3,4,7 as inputs and pins 2,5,6 as outputs, leaves pins 0,1 unchanged
  PORTB&=B11000011;//Set all PORTB(Pins 8-13) LOW
  PORTD&=B10011011;//Set all Output pins on PORTD(Pins 0-7) to LOW
  
  //each sequence takes 1 second
  
  leftServo.attach(leftServoPin); //attaches the servo pin to the appropraite Object
  rightServo.attach(rightServoPin); //attaches the servo pin to the appropraite Object

  qtr.setTypeRC(); // Set QTR Sensor for Digital mode
  qtr.setSensorPins((const uint8_t[]){8, 9, 14, 15, 16, 17, 18, 19}, SensorCount); // Tells the QTR Library the pins connected to the QTR Sensor and the number of pins
  Stop(); // Custom funtion that stops the motors
  Serial.begin(115200);  // Change to 9600

  /*************************************************************************************************/
  /************************************ CALIBRATION SEQUENCE ***************************************/
  flashSequence(5);//5 second indication delay
  for(int t=0;t<1;t++){
    turnLeft(4);
    //delay(500);
    for (uint8_t i = 0; i < 50; i++)
    {
      qtr.calibrate();
      delay(20);
    }//*/
    Stop();
    turnRight(3);
    //delay(1000);
    for (uint8_t i = 0; i < 50; i++)
    {
      qtr.calibrate();
      delay(20);
    }//*/
    Stop();
    turnLeft(4);
    //delay(500);
    for (uint8_t i = 0; i < 39; i++)
    {
      qtr.calibrate();
      delay(20);
    }//*/
  Stop();
  flashSequence(5);
  }
  /*************************************************************************************************/
  Stop();
}

void loop() {
  
  
  switch(stage){
  /***************************************************************************************************************************************************/  
  case 0:{ // This Case gets the robot from the starting point to i0
    pos = qtr.readLineBlack(sensorValues); //returns an estimated position of the black line
    errorVal = pos-desiredPos;
    moveForward(5);
    delay(70);
  
    if ((sensorValues[0] > refThresh) && (sensorValues[1] > refThresh) && (sensorValues[2] > refThresh)&&(sensorValues[3] > refThresh) && (sensorValues[4] > refThresh) && (sensorValues[5] > refThresh)&&(sensorValues[6] > refThresh) && (sensorValues[7] > refThresh)){
      if(curveFlag==true){
      Stop();
      Stop();
      stage=1;
      intersectionSequence();
      delay(1000);
      moveForward(5);
      delay(250);
      break;
      }
      else{
        curveFlag=true;
        delay(200);
      }
    }

    
    
    if(errorVal<-errorThresh){
      while(errorVal<-errorThresh){
        turnLeft(3);
        delay(10);
        Stop();
        pos = qtr.readLineBlack(sensorValues);
        errorVal = pos-desiredPos;
        //delay(5);
      }
    }
    if(errorVal>errorThresh){
      while(errorVal>errorThresh){
        turnRight(3);
        delay(10);
        Stop();
        pos = qtr.readLineBlack(sensorValues);
        errorVal = pos-desiredPos;
        //delay(5);
      }
    }
    //moveForward(20);
    //delay(40);
    Serial.print(pos);// print the line position
    Serial.print('\t');
    Serial.println(errorVal);// print the line position
    //delay(250);
    break;
  }

  /*******************************************************************************************************************************************************/
  case 1:{ // This case gets the Robot from i0 to i5 and checks for "cars" on the way to i5

    pos = qtr.readLineBlack(sensorValues); // returns an estimated position of the black line
    errorVal = pos-desiredPos; // Error correcting code that compares the present position to the desired position
    moveForward(5);
    delay(50);
    
  
    if ((sensorValues[0] > refThresh) && (sensorValues[1] > refThresh) && (sensorValues[2] > refThresh)&&(sensorValues[3] > refThresh) && (sensorValues[4] > refThresh) && (sensorValues[5] > refThresh)&&(sensorValues[6] > refThresh) && (sensorValues[7] > refThresh)){
      Stop();
      interCount++;
      intersectionSequence();
      delay(1000);
      moveForward(5);
      delay(20);
      Stop();
      
      if(readUltraSonic(rightUltTrig,rightUltEcho)<maxDist){ // Checks for "cars" on the right side of the track with the right ultrasonic sensor
        objRight=true;
        digitalWrite(rightLED,objRight);
        delay(1000);
        digitalWrite(rightLED,LOW);
      }
      
      else{
        objRight=false;
      }
      

      if(readUltraSonic(leftUltTrig,leftUltEcho)<maxDist){ // Checks for "cars" on the left side of the track with the left ultrasonic sensor
        objLeft=true;
        digitalWrite(leftLED,objLeft);
        delay(1000);
        digitalWrite(leftLED,LOW);
      }
      else{
        objLeft=false;
      }
      
      moveForward(5);
      delay(200);

      if((objRight==true)||(objLeft==true)){
        Stop();
        moveForward(5);
        delay(500);
        if(objRight==true){
          turnRight(3);
          delay(1800);
          Stop();
        }
        else if(objLeft==true){
          turnLeft(3);
          delay(1800);
          Stop();
        }
        stage=4;
        break;
      }

      
      if(interCount==5){
        Stop();
        stage=2;
        break;
      }
    }

    
    
    if(errorVal<-errorThresh){ // Error correcting code that keeps the Robot on track with respect to the line
      while(errorVal<-errorThresh){ // While The error exist keep making adjustements to the position
        turnLeft(3);
        delay(10);
        Stop();
        pos = qtr.readLineBlack(sensorValues);
        errorVal = pos-desiredPos; // Error correcting code that compares the present position to the desired position
        //delay(5);
      }
    }
    if(errorVal>errorThresh){ // Error correcting code that keeps the Robot on track with respect to the line
      while(errorVal>errorThresh){ // While The error exist keep making adjustements to the position
        turnRight(3);
        delay(10);
        Stop();
        pos = qtr.readLineBlack(sensorValues);
        errorVal = pos-desiredPos; // Error correcting code that compares the present position to the desired position
        //delay(5);
      }
    }
    Serial.print(pos);// print the line position
    Serial.print('\t');
    Serial.println(errorVal);// print the difference between the position and the desired line position

    break;
  }
  
  /*********************************************************************************************************************************************/
  case 2:{ // This case checks for objects in front of the front facing ultrasonic sensor and stops when it gets to the appropriate distance from the object
    
    pos = qtr.readLineBlack(sensorValues);
    errorVal = pos-desiredPos;
    moveForward(5);
    delay(50);
  
    if (readUltraSonic(frontUltTrig,frontUltEcho)<stopDist){ // if the robot is within distance it goes to case 3
      Stop();
      stage=3;
      break;
    }

    
    
    if(errorVal<-errorThresh){
      while(errorVal<-errorThresh){
        turnLeft(3);
        delay(10);
        Stop();
        pos = qtr.readLineBlack(sensorValues);
        errorVal = pos-desiredPos;
        //delay(5);
      }
    }
    if(errorVal>errorThresh){
      while(errorVal>errorThresh){
        turnRight(3);
        delay(10);
        Stop();
        pos = qtr.readLineBlack(sensorValues);
        errorVal = pos-desiredPos;
        //delay(5);
      }
    }
    //moveForward(20);
    //delay(40);
    /*
    Serial.print(pos);// print the line position
    Serial.print('\t');
    Serial.println(errorVal);// print the line position
    */
    //delay(250);
    break;
  }
  /**************************************************************************************************************************************/
  case 3:{ // This case stops the robot and flashes the LED and marks the end of the operation
    Stop();
    flashSequence(1);
    doneFlag=true;
    break;
  }
  
/**************************************************************************************************************************************/
  case 4:{ // This case branches the Robot of the main track and makes the robot approach the object(s) on the side
      
      
      pos = qtr.readLineBlack(sensorValues);
      errorVal = pos-desiredPos;
      moveForward(5);
      delay(30);
      
      if(readUltraSonic(frontUltTrig,frontUltEcho)<stopDist){ //if the robot is within distance of the object
        Stop();
        delay(500);
        objSequence();
        delay(200);

        if(objRight==true){
          turnLeft(5);
          delay(1700);
          pos = qtr.readLineBlack(sensorValues);
          errorVal = pos-desiredPos;
          while(abs(errorVal)>errorThresh){
            turnLeft(5);
            delay(10);
            Stop();
            pos = qtr.readLineBlack(sensorValues);
            errorVal = pos-desiredPos;
            //delay(5);
          }
          Stop();
        }

        else if(objLeft==true){
          turnRight(5);
          delay(1700);
          pos = qtr.readLineBlack(sensorValues);
          errorVal = pos-desiredPos;
          while(abs(errorVal)>errorThresh){
            turnRight(5);
            delay(10);
            Stop();
            pos = qtr.readLineBlack(sensorValues);
            errorVal = pos-desiredPos;
            //delay(5);
          }
          Stop();
        }
        
        stage=5;
        break;
      }

      if(errorVal<-errorThresh){
        while(errorVal<-errorThresh){
          turnLeft(3);
          delay(10);
          Stop();
          pos = qtr.readLineBlack(sensorValues);
          errorVal = pos-desiredPos;
          //delay(5);
        }
      }
      if(errorVal>errorThresh){
        while(errorVal>errorThresh){
          turnRight(3);
          delay(10);
          Stop();
          pos = qtr.readLineBlack(sensorValues);
          errorVal = pos-desiredPos;
          //delay(5);
        }
      }

      break;    
  }
/***************************************************************************************************************************************************/  
  case 5:{ // This case makes the Robot return back to the main track
    pos = qtr.readLineBlack(sensorValues);
    errorVal = pos-desiredPos;
    moveForward(5);
    delay(30);
  
    if ((sensorValues[0] > refThresh) && (sensorValues[1] > refThresh) && (sensorValues[2] > refThresh)&&(sensorValues[3] > refThresh) && (sensorValues[4] > refThresh) && (sensorValues[5] > refThresh)&&(sensorValues[6] > refThresh) && (sensorValues[7] > refThresh)){
      if(objRight==true&&objLeft==true){
      Stop();
      objRight=false;
      moveForward(5);
      delay(100);
      stage=4;
      break;
      }
      else if(objRight==true){
        moveForward(5);
        delay(500);
        turnRight(3);
        objRight=false;        
      }
      else{
        moveForward(5);
        delay(500);
        turnLeft(3); 
        objLeft=false;
      }

      delay(1800);
      moveForward(5);
      delay(500);
      stage=1;
      break;
    }

    
    
    if(errorVal<-errorThresh){
      while(errorVal<-errorThresh){
        turnLeft(3);
        delay(10);
        Stop();
        pos = qtr.readLineBlack(sensorValues);
        errorVal = pos-desiredPos;
        //delay(5);
      }
    }
    if(errorVal>errorThresh){
      while(errorVal>errorThresh){
        turnRight(3);
        delay(10);
        Stop();
        pos = qtr.readLineBlack(sensorValues);
        errorVal = pos-desiredPos;
        //delay(5);
      }
    }
    
    break;
  }
  
  /*******************************************************************************************************************************************************/
  default:
  break;
  }
  pos=0;
  errorVal=0;
  //objRight=false;
  //objLeft=false;
}

unsigned int readUltraSonic(int trig, int echo){
  unsigned long duration=0;
  unsigned int distance=0;
  // Clears the trigger Pin
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  for(int j=0;j<20;j++){
    // Sets the trigger Pin on HIGH state for 10 micro seconds
    digitalWrite(trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig, LOW);
    // Reads the echo Pin, returns the sound wave travel time in microseconds
    duration = pulseIn(echo, HIGH);
    distance += duration * 0.034 / 2.0; 
    delayMicroseconds(20);
  }
  distance/=20.0;
   /*
  Serial.print("Distance From object: ");
  Serial.print(distance);
  Serial.println(" cm");*/
  return distance;
}

void flashSequence(byte iters){
  for(int i=0;i<iters;i++){
    PORTB=(PORTB&0b11001111)|B00100000;
    delay(250);
    PINB=0b00110000;
    delay(250);
    PINB=0b00110000;
    delay(250);
    PINB=0b00110000;
    delay(250);
  }
  PORTB&=B11001111;
}

void intersectionSequence(){
  int delz=1000;
  PORTB=(PORTB&0b11001111)|B00110000;
  delay(delz);
  while(delz>0){
    PINB=0b00110000;
    delz-=200;
    delay(delz); 
  }
  PORTB&=0b11001111;
}

void objSequence(){
  PORTB=(PORTB&0b11001111)|B00100000;
  delay(500);
  PINB=B00100000;
  delay(500);
  PINB=B00100000;
  delay(500);
  PINB=B00010000;
  delay(500);
  PINB=B00010000;
  delay(500);
  PINB=B00010000;
  delay(500);
  PINB=B00110000;
}

void Stop() {
  //leftServo.write(90);
  //rightServo.write(90);
  leftServo.writeMicroseconds(1500); 
  rightServo.writeMicroseconds(1500);
}

void moveForward(byte Speed) {//Give speed value between 0 and 20 (1300us to 1700us duty)
  Speed=map(Speed,0,20,0,200);
  leftServo.writeMicroseconds(1500+Speed);
  rightServo.writeMicroseconds(1500-Speed);
}

void moveBackward(byte Speed) {//Give speed value between 0 and 20 (1300us to 1700us duty)
  Speed=map(Speed,0,20,0,200);
  leftServo.writeMicroseconds(1500-Speed);
  rightServo.writeMicroseconds(1500+Speed);
}

void turnLeft(byte Speed) {//Give speed value between 0 and 20 (1300us to 1700us duty)
  Speed=map(Speed,0,20,0,200);
  leftServo.writeMicroseconds(1500-Speed);
  rightServo.writeMicroseconds(1500-Speed);
}

void turnRight(byte Speed) {//Give speed value between 0 and 20 (1300us to 1700us duty)
  Speed=map(Speed,0,20,0,200);
  leftServo.writeMicroseconds(1500+Speed);
  rightServo.writeMicroseconds(1500+Speed);
}
