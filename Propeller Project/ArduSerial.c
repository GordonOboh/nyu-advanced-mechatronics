/*
  Blank Simple Project.c
  http://learn.parallax.com/propeller-c-tutorials 
*/
#include "simpletools.h"                      // Include simple tools
#include "fdserial.h"
//#include "wavplayer.h"
#include "ping.h" 
#include "servo.h"

static volatile int pos, ardcog, ultcog, lightcog, audiocog, buzzcog, servocog, lfcog, frtDist, fPin, lftDist, lPin, rgtDist, rPin, sensorVals[8], buzzPin; 
static volatile int motorSpeed, motorDir, endBuzz, endDist, endPos, lftServo, rgtServo, desiredPos, endLF, endServo, errorVal, LED1, LED2, calDone, endLight;

static volatile int maxDist=20; //Maximum distance of detectable object from ultrasonic sensor;
static volatile int stopDist=7; // Minimum distance to stop in front of object
static volatile int refThresh=725;//Threshold used for detecting intersections
static volatile int errorThresh=250;


static volatile int stage;//Used to differenciate sections/ stages of track
static volatile int interCount;//Variable to store the number of passed intersections
static volatile int deliveryCount;//Variable to store the number of deliveries made
static volatile int numInter;//Total number of intersections on track;

static volatile int objRight;//Indicates whether an object was detected on the right;
static volatile int objLeft;//Indicates whether an object was detected on the left;

static volatile int curveFlag;//Make true if home curve radii are quite large
static volatile int loopFlag;//Gets set after final delivery at C1;

void getPos(void* par1);
void getDist(void* par2);
void soundBuzzer(void* par3);
//void playAudio(void* audfile);
void spinServos(void* par4);
void lineFollower(void* par5);
void Lights(void* par6);

void Forward(int speed);
void TurnLeft(int speed);
void TurnRight(int speed);
void Stop(void);
int detectIntersection(void);
void intersectionSequence(void);
void Follow(int samp);
void Turn180(void);
void Turn90L(void);
void Turn90R(void);


unsigned int ardstack[40+60];
unsigned int buzzstack[40+15];
unsigned int audiostack[40+50];
unsigned int ultstack[40+20];
unsigned int servostack[40+30];
unsigned int lfstack[40+20];
unsigned int lightstack[40+5];

int main()                                    // Main function
{
    /* start device */
                    
 
  /*const char i2[] = {"i2.wav"};       // Set up techloop string
  const char i3[] = {"i3.wav"};   
  const char i5[] = {"i5.wav"};   
  const char deli[] = {"Delivery2.wav"};   
  const char gmovr[] = {"gmovr.wav"};
  const char start[] = {"start.wav"};
  const char inter[] = {"inter.wav"};
  const char reverse[] = {"reverse.wav"};*/
  desiredPos=3500;
  motorSpeed=50;
  LED1=2;
  LED2=1;
  fPin=4;
  lPin=5;
  rPin=3;
  buzzPin=13;
  lftServo=16;
  rgtServo=17;
  buzzcog=cogstart((void*)soundBuzzer,1,buzzstack,sizeof(buzzstack));
  servocog=cogstart((void*)spinServos,NULL,servostack,sizeof(servostack));
  pause(100);
  pause(1000);
  ultcog=cogstart((void*)getDist, NULL, ultstack, sizeof(ultstack));
  ardcog=cogstart((void*)getPos,NULL,ardstack,sizeof(ardstack));
  high(LED1);
  high(LED2);
  while(!calDone){//wait for calibration to finish
    //print("%04d \n",pos);
    //pause(500);
  }
  pause(2100);
  
  low(LED1);
  low(LED2);
  while(1)
  { 
    
    ////////////////////////////////////////////DEBUGGING///////////////////////////////////////////////////////
    /*print("%04d\n",pos);
    //print("%04d\n",sensorVals[0]);
    print("\n%04d \t%04d \t%04d \t%04d \t%04d \t%04d \t%04d \t%04d",sensorVals[0],sensorVals[1],sensorVals[2],sensorVals[3],sensorVals[4],sensorVals[5],sensorVals[6],sensorVals[7]);
    print("\nleftDist = %04d \t frontDist = %04d \t rightDist = %04d\n", lftDist,frtDist,rgtDist);           // Display distance
    pause(500);
    print(HOME,CLS);*/
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    switch(stage){
      /***********************************************[Case 0]***************************************************************/
      case 0:{
      
        cogstart((void*) lineFollower,(void*) 70,lfstack, sizeof(lfstack));
        while(!detectIntersection()){
          
        }
        endLF=1;
        motorDir=1;
        if(curveFlag==1){
          intersectionSequence();
          stage=1;
          break;
        }            
        else{
          curveFlag=1;
          Follow(70);
        }
        pause(190);
        break;
      }        
      /**************************************************************************************************************/




      /***********************************************[Case 1]********************************************************/
      case 1:{
        pause(550);
        Follow(70);
        while(frtDist>stopDist){
          if(detectIntersection()){
            interCount++;
            intersectionSequence();
          }            
        }
        motorDir=1;
        endLF=1;
        motorDir=1;
        Turn180();
        Follow(70);
        do{
          if(detectIntersection()){
            interCount--;
            intersectionSequence();
          }            
        }while(interCount!=1);
        
        while(!detectIntersection()){
          
        }
        intersectionSequence();
        endLF=1;
        stage=2;          
        break;
      }        
      /**************************************************************************************************************/




      /***********************************************[Case 2]********************************************************/
      case 2:{
        endLF=1;
        Turn90R();
        Follow(70);
        while(!detectIntersection()){
          
        }
        pause(50);
        endLF=1;
        
        intersectionSequence();
        if(frtDist<maxDist){
          deliveryCount++;
          deliverySequence();
        }
        
        if(deliveryCount==2){
          motorDir=0;
          stage=7;
          break;
        }
        else{            
          Turn90R();
          low(LED1);
          low(LED2);       
          stage=3;
        }          
        break;
      }        
      /**************************************************************************************************************/



      /***********************************************[Case 3]********************************************************/
      case 3:{
        Follow(70);
        do{
          if(detectIntersection()){
            interCount++;
            intersectionSequence();
            if((lftDist<maxDist)&&(loopFlag==0)){
              deliveryCount++;
              deliverySequence();
            }
            
            if(deliveryCount==2){
              motorDir=0;
              stage=7;
              break;
            } 
          }            
        }while(interCount<4);
        
        if(deliveryCount==2){
          motorDir=0;
          stage=7;
          break;
        } 
        
        if(loopFlag==1){
          Follow(70);  
        }
        else{
          endLF=1;
          intersectionSequence();
          motorDir=1;
          pause(5);
          motorDir=3;
          motorDir=3;
          pause(1100);
          
          low(LED1);
          low(LED2);
          motorDir=1;
          pause(200);
          Follow(70);
        }          
        
        while(!detectIntersection()){
          
        }
        intersectionSequence();
        if(loopFlag==1){
          
          stage=7;
          endLF=1;
          motorDir=0;
          deliverySequence();
          break;  
        }
                  
        while(!detectIntersection()){
          
        }
        endLF=1;
        motorDir=1;
        intersectionSequence();
        if(frtDist<maxDist){
          deliveryCount++;
          deliverySequence();
        }
        
        if(deliveryCount==2){
          motorDir=0;
          stage=7;
          break;
        }
        else{  
          //endLF=1;          
          //motorDir=1;
          //pause(250);
          motorDir=3;
          pause(900);
          
          low(LED1);
          low(LED2);
          motorDir=1;
          pause(400);       
          stage=4;
        }          
        
        break;
      }        
      /**************************************************************************************************************/




            /***********************************************[Case 4]********************************************************/
      case 4:{
        Follow(70);
        do{
          if(detectIntersection()){
            interCount--;
            intersectionSequence();
            if(lftDist<maxDist){
              deliveryCount++;
              deliverySequence();
            }
            
            if(deliveryCount==2){
              motorDir=0;
              stage=7;
              break;
            } 
          }            
        }while((interCount>1)&&(stage==4));
        
        if(deliveryCount==2){
          motorDir=0;
          stage=7;
          break;
        } 
        endLF=1;
        
        intersectionSequence();
        motorDir=1;
        pause(10);
        motorDir=3;
        pause(1000);
        
        low(LED1);
        low(LED2);
        motorDir=1;
        pause(130);
        Follow(70);
        while(!detectIntersection()){
          
        }
        loopFlag=1;
        intersectionSequence();
        while(!detectIntersection()){
          
        }
        endLF=1;
        intersectionSequence();
        motorDir=1;
        motorDir=1;
        pause(250);
            
        Turn90R();       
        stage=3;          
        
        break;
      }        
      /**************************************************************************************************************/




      /***********************************************[Default]*****************************************************/
      default:{
        endLF=1;
        Stop();
        break;
      }        
      /**************************************************************************************************************/

    }    
  }  
}
/**************************************************************************************************************/





/**********************************************[Position Cog]****************************************************/
void getPos(void* par1){
   
   int   buflen=100;
   char buffer[buflen];
   
   fdserial *ard = fdserial_open(14,15,0,115200);
   
   ///////////////////////////////////////////CALIBRATION/////////////////////////////////////////////////////////
   writeChar(ard,'c');
   pause(1);
   while(fdserial_rxReady(ard)==0){
     writeChar(ard,'c');
     pause(1);     
   }
   readStr(ard, buffer, buflen);
   sscan(buffer, "%d", &pos);  
   while(pos!=9000){
   }
   pos=0;        
   //Start motion
   int aid=4;
   
   //audiocog=cogstart((void*)playAudio,(void*)aid,audiostack,sizeof(audiostack));
   pause(3500);
   
   for(int i=0;i<1;i++){
     motorSpeed=30;
     motorDir=2;
     pause(1500);
  
     motorDir=3;
     pause(2700);
  
     motorDir=2;
     pause(1600);
  
     motorDir=0;
     //End motion
   }   
   
   pause(1000);
   
   
   writeChar(ard,'d');
   pause(1);
   while(fdserial_rxReady(ard)==0){
     pos=-1;
     writeChar(ard,'d');
     pause(1);
   }
   readStr(ard, buffer, buflen);
   sscan(buffer, "%d", &pos);  
   while(pos!=9500){
   }
   
   //pos=0;
   //Play start sound
   aid=1;
   //audiocog=cogstart((void*)playAudio,1,audiostack,sizeof(audiostack));
   calDone=1;
   pause(2000);
   /////////////////////////////////////////////////////////////////////////////////////////////
   
   
   //Begin reading positions
   
   while(!endPos){
     if(fdserial_rxReady(ard)){
      readStr(ard, buffer, buflen);
      //sscan(buffer, "%d %d", &pos, &sensorVals[0]);
      sscan(buffer, "%d %d %d %d %d %d %d %d %d", &pos, &sensorVals[0], &sensorVals[1], &sensorVals[2], &sensorVals[3], &sensorVals[4], &sensorVals[5], &sensorVals[6], &sensorVals[7]);
      //pause(249);
      errorVal = pos-desiredPos;
    }    
  }
  cogstop(ardcog);    
} 
/**************************************************************************************************************/




/**********************************************[Audio Cog]****************************************************
void playAudio(void* audfile){
  // Mount SD card
  high(LED1);
  high(LED2);
  int DO = 22, CLK = 23, DI = 24, CS = 25;        // SD I/O pins
  sd_mount(DO, CLK, DI, CS);
  pause(10);    
  int ida= (int) audfile; 
  int ptime;
  if(ida==1){
    wav_play("start.wav");
    ptime=1500;
  }
  else if(ida==2){
    wav_play("i2.wav");
    ptime=2500;
  }
  
  else if(ida==3){
    wav_play("i3.wav");
    ptime=2500;
  } 
  
  else if(ida==4){
    wav_play("reverse.wav");
    ptime=6500;
  }
  
  else if(ida==5){
    wav_play("i5.wav");
    ptime=2500;
  }
  
  else if(ida==6){
    wav_play("inter.wav");
    ptime=1500;
  } 
  
  else if(ida==7){
    wav_play("Delivery2.wav");
    ptime=2000;
  }
  
  else if(ida==8){
    wav_play("gmovr.wav");
    ptime=5000;
  }        
                            
  wav_volume(10);                                  // Adjust volume
  pause(ptime);
  wav_stop();
  low(LED1);
  low(LED2);
  cogstop(audiocog);
}  
**************************************************************************************************************/




/**********************************************[UltraSonic Cog]****************************************************/
void getDist(void* par2){
  while(!endDist){
    frtDist = ping_cm(fPin);                 // Get cm distance from Ping)))
    lftDist = ping_cm(lPin);
    rgtDist = ping_cm(rPin);
  }
  cogstop(ultcog);    
}  
/**************************************************************************************************************/




/**********************************************[Servo Cog]****************************************************/
void spinServos(void* par4){
  
  while(!endServo){
    switch(motorDir){
      case 0:{
   
        //Stop
        servo_set(lftServo,1500);
        servo_set(rgtServo,1500);
        break;
      }
      
      case 1:{

        //Forward
        servo_speed(lftServo,motorSpeed);
        servo_speed(rgtServo,-motorSpeed);      
        break;  
      }
      
      case 2:{

        //Turn Left
        servo_speed(lftServo,-motorSpeed);
        servo_speed(rgtServo,-motorSpeed);
        break;
      }
      
      case 3:{

        //Turn Right
        servo_speed(lftServo,motorSpeed);
        servo_speed(rgtServo,motorSpeed);
        break;
      }
      
      default:{
        motorDir=0;
        break;
      }        
    }                  
  }    
}  
/**************************************************************************************************************/




/**********************************************[Line Follower Cog]**********************************************/
void lineFollower(void* par5){
  int lfPause = (int) par5;
  endLF=0;
  while(!endLF){
    Forward(50);
    pause(lfPause);
    
    if(errorVal<-errorThresh){
      while(errorVal<-errorThresh){
        TurnLeft(30);
        pause(10);
      }
    }
    if(errorVal>errorThresh){
      while(errorVal>errorThresh){
        TurnRight(30);
        pause(10);
      }
    }
    
  }
  cogstop(lfcog);    
}  
/**************************************************************************************************************/




/**********************************************[Buzzer Cog]****************************************************/
void soundBuzzer(void* par3){
  endBuzz=0;
  int iters= (int) par3;
  for(int i=0;i<iters;i++){
   high(buzzPin);
   pause(500);
   low(buzzPin);
   pause(500); 
 }
 low(buzzPin); 
 endBuzz=1;
 cogstop(buzzcog);  
}   
/**************************************************************************************************************/




/**********************************************[Light Cog]****************************************************/
/*{
  endLight=0;
  int iters= (int) par6;
  for(int i=0;i<iters;i++){
   high(LED1); 
   high(LED2);
   pause(500);
   low(LED1);
   low(LED2);
   pause(500); 
 }
 low(LED1);
 low(LED2); 
 endLight=1;
 cogstop(lightcog);  */
}   
/**************************************************************************************************************/






//////////////////////[Functions]//////////////////////////////
void Forward(int speed){
  motorDir=1;
  motorSpeed=speed;
}

  
void TurnLeft(int speed){
  motorDir=2;
  motorSpeed=speed;
}
 
  
void TurnRight(int speed){
  motorDir=3;
  motorSpeed=speed;
}

  
void Stop(){
  motorDir=0;
  motorSpeed=0;
}  


int detectIntersection(void){
  int interFound=0;
  if ((sensorVals[0] > refThresh) && (sensorVals[1] > refThresh) && (sensorVals[2] > refThresh)&&(sensorVals[3] > refThresh) && (sensorVals[4] > refThresh) && (sensorVals[5] > refThresh)&&(sensorVals[6] > refThresh) && (sensorVals[7] > refThresh)){
    interFound=1;
  }
  return interFound;    
}  


void intersectionSequence(void){
  high(buzzPin);
  pause(500);
  low(buzzPin); 
} 


void deliverySequence(void){
  high(LED1);
  high(LED2);
} 


void Follow(int samp){
  lfcog=cogstart((void*) lineFollower,(void*) samp,lfstack, sizeof(lfstack));
}  


void Turn180(void){
  motorDir=3;
  pause(1);
  motorDir=3;
  pause(1800);
}


void Turn90R(void){
  motorDir=1;
  pause(90);
  motorDir=3;
  pause(1);
  motorDir=3;
  pause(1000);
  motorDir=1;
  pause(250);
}

void Turn90L(void){
  motorDir=1;
  pause(100);
  motorDir=2;
  pause(1000);
  motorDir=1;
  pause(90);
}