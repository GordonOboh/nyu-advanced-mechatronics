/*
  Blank Simple Project.c
  http://learn.parallax.com/propeller-c-tutorials 
*/
#include "simpletools.h"                      // Include simple tools
#include "fdserial.h"
#include "ping.h" 
#include "servo.h"

static volatile int pos, ardcog, ultcog, indcog, servocog, lfcog, frtDist, fPin, lftDist, lPin, rgtDist, rPin, sensorVals[8], buzzPin; 
static volatile int motorSpeed, motorDir, endDist, endPos, lftServo, rgtServo, desiredPos, endLF, endServo, errorVal, LED1, LED2, calDone;
static volatile int armServo, friendPin, foePin, startPin, rxPin, txPin, indMode, indIters, interDelay, frtSense,lftSense, rgtSense;

static volatile int totalDeliveries=8; //Total number of deliveries which need to be made
static volatile int maxDist=7; //Maximum distance of detectable object from ultrasonic sensor;
static volatile int stopDist=6; // Minimum distance to stop in front of object
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
void soundLights(void* par3);
void spinServos(void* par4);
void lineFollower(void* par5);

void Forward(int speed);
void TurnLeft(int speed);
void TurnRight(int speed);
void Stop(void);
int detectIntersection(void);
void intersectionSequence(void);
void deliverySequence(void);
void Follow(int samp);
void Turn180(void);
void Turn90L(void);
void Turn90R(void);
void Spin90L(void);
void Spin90R(void);
void ChangeVar(volatile int* vari,int val);


unsigned int ardstack[40+60];
unsigned int ultstack[40+20];
unsigned int indstack[40+20];
unsigned int servostack[40+30];
unsigned int lfstack[40+20];

int main()                                    // Main function
{
    /* start device */
                    
  interDelay=800;
  desiredPos=3500;
  motorSpeed=50;
  LED1=1;
  LED2=2;
  rPin=3;
  fPin=4;
  lPin=5;
  startPin=9;
  foePin=10;
  friendPin=11;
  buzzPin=12;
  rxPin=13;
  txPin=14;
  armServo=15;
  lftServo=16;
  rgtServo=17;
  ChangeVar(&indMode,1);
  ChangeVar(&indIters,1);
  indcog=cogstart((void*)soundLights,NULL,indstack,sizeof(indstack));
  servocog=cogstart((void*)spinServos,NULL,servostack,sizeof(servostack));
  pause(100);
  pause(1000);
  ultcog=cogstart((void*)getDist, NULL, ultstack, sizeof(ultstack));
  ardcog=cogstart((void*)getPos,NULL,ardstack,sizeof(ardstack));
  high(LED1);
  high(LED2);
  //ChangeVar(&curveFlag,1);
  while(!calDone){//wait for calibration to finish
    //print("%04d \n",pos);
    //pause(500);
  }
  servo_angle(armServo, 0); 
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
    pause(50);
    print(HOME,CLS);*/   
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    /*
     *servo_angle(armServo, 0);                         // P16 servo to 0 degrees
      pause(3000);  
     *
     */
    
    
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
          pause(100);
          ChangeVar(&stage,1);
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




      /***********************************************[Test Case]********************************************************/
      /*case 1:{
        pause(550);
        Follow(70);
        while(!detectIntersection()){
          
        }
        intersectionSequence();
        //endLF=1;
        pause(200);
        //Turn90L();
        Follow(70);
        while(!detectIntersection()){
          if((lftDist<maxDist)&&(rgtDist<maxDist)){
            ChangeVar(&endLF,1);
            ChangeVar(&motorDir,0);
            pause(50);
            deliverySequence();
            Spin90R();            
            ChangeVar(&motorDir,0);
            pause(1000);
            Turn180();
            ChangeVar(&motorDir,0);
            stage=7;
            break;
          }
          else if(lftDist<maxDist){
            ChangeVar(&endLF,1);
            ChangeVar(&motorDir,0);
            pause(50);
            deliverySequence();
            Spin90L();            
            ChangeVar(&motorDir,0);
            pause(1000);
            stage=7;
            break;
          }
          else if(rgtDist<maxDist){
            ChangeVar(&endLF,1);
            ChangeVar(&motorDir,0);
            pause(50);
            deliverySequence();
            Spin90R();            
            ChangeVar(&motorDir,0);
            pause(1000);
            stage=7;
            break;
          }                                    
          
        }
        intersectionSequence();
        
        
        //pause(50);
        ChangeVar(&endLF,1);
        ChangeVar(&motorDir,0);
        stage=7;
        break;      
      }      
      /**************************************************************************************************************/




      /***********************************************[Case 1]********************************************************/
      case 1:{
        pause(350);
        Follow(70);
        while(frtDist>stopDist){
          if(detectIntersection()){
            interCount++;
            intersectionSequence();
            pause(interDelay);
          }            
        }
        //ChangeVar(&motorDir,1);
        ChangeVar(&endLF,1);
        Turn180();
        ChangeVar(&frtSense,0);
        Follow(70);
        do{
          if(detectIntersection()){
            interCount--;
            intersectionSequence();
            pause(interDelay);
          }            
        }while(interCount!=1);
        
        while(!detectIntersection()){
          ;
        }
        intersectionSequence();
        ChangeVar(&endLF,1);
        ChangeVar(&stage,2);
        ChangeVar(&motorDir,3);          
        break;
      }        
      /**************************************************************************************************************/




      /***********************************************[Case 2]********************************************************/
      case 2:{
        ChangeVar(&endLF,1);
        ChangeVar(&endLF,1);
        ChangeVar(&motorDir,3);    
        pause(800);
        while((pos<4500)){//||(pos>4000)){;
          ;
        }
        while((pos>4000)){
          ;
        }    
        ChangeVar(&motorDir,1);
        pause(250);
        Follow(70);
        while(!detectIntersection()){
          
        }
        pause(50);
        ChangeVar(&endLF,1);
        
        intersectionSequence();
        if(frtSense==1){
          deliveryCount++;
          deliverySequence();
        }
        
        if(deliveryCount==totalDeliveries){
          ChangeVar(&motorDir,0);
          ChangeVar(&stage,7);
          break;
        }
        else{            
          Turn90R();      
          ChangeVar(&stage,3);
        }          
        break;
      }        
      /**************************************************************************************************************/



      /***********************************************[Case 3]********************************************************/
      case 3:{
        Follow(70);
        pause(500);
        ChangeVar(&lftSense,0);
        ChangeVar(&rgtSense,0);
        ChangeVar(&frtSense,0); 
        
        do{
          if(detectIntersection()){
            interCount++;
            intersectionSequence();
            pause(interDelay);
                      
            
          if((lftSense)&&(loopFlag==0)){
            deliveryCount++;
            ChangeVar(&endLF,1);
            ChangeVar(&motorDir,0); 
            deliverySequence();
            Spin90L();
            high(startPin);
            pause(500);
            while((friendPin==0)&&(foePin==0)){
                
              }
              
              if(foePin==1){              
            servo_angle(armServo, 1800);
            pause(3000);
            //}              
          
            servo_angle(armServo, 0);
            pause(1000);
            ChangeVar(&motorDir,5);
            ChangeVar(&motorSpeed,50);
            pause(500);
            while((pos<4500)){//||(pos>4000)){;
                ;
            }
            while((pos>4000)){
                ;
            }  
            Follow(70);
            pause(200);
          
            ChangeVar(&lftSense,0);
            ChangeVar(&rgtSense,0);
            ChangeVar(&frtSense,0);   
          }      
              
          if(deliveryCount==totalDeliveries){
            ChangeVar(&motorDir,0);
            ChangeVar(&stage,7);
            break;
          }             
        }                    
      }while(interCount<4);
        
        if(deliveryCount==totalDeliveries){
          ChangeVar(&motorDir,0);
          ChangeVar(&stage,7);
          break;
        } 
        else{  
          if(loopFlag==1){
            Follow(70);
            pause(200);
            ChangeVar(&lftSense,0);
            ChangeVar(&rgtSense,0);
            ChangeVar(&frtSense,0);
          }
          else{
            
            ChangeVar(&endLF,1);
            
            ChangeVar(&motorDir,3);
            ChangeVar(&motorSpeed,50);
            pause(3000);
            while((pos<4500)){//||(pos>4000)){;
              ;
            }
            while((pos>4000)){
              ;
            }   
            //ChangeVar(&motorDir,1);
            //pause(200);  
            Follow(70);
            pause(300);
            ChangeVar(&lftSense,0);
            ChangeVar(&rgtSense,0);
            ChangeVar(&frtSense,0);
          }    
        }                
        
        while(!detectIntersection()){
          
        }
        intersectionSequence();
        if(loopFlag==1){
          
          ChangeVar(&stage,7);
          ChangeVar(&endLF,1);
          ChangeVar(&motorDir,0);
          pause(50);
          deliverySequence();
          break;  
        }
        else{
          pause(interDelay);
        }          
                  
        while(!detectIntersection()){
          
        }
        ChangeVar(&endLF,1);
        //ChangeVar(&motorDir,1);
        intersectionSequence();
        if(frtSense==1){
          deliveryCount++;
          deliverySequence();
        }
        
        if(deliveryCount==totalDeliveries){
          ChangeVar(&motorDir,0);
          ChangeVar(&stage,7);
          break;
        }
        else{  
          ChangeVar(&endLF,1);          
          //motorDir=1;
          //pause(250);
          Turn90R();
                 
          ChangeVar(&stage,4);
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
            pause(interDelay);
            if(lftSense==1){
              deliveryCount++;
              deliverySequence();
              ChangeVar(&motorDir,0);
              ChangeVar(&endLF,1);
              pause(50);
              deliverySequence();/////////
              Spin90L();
              high(startPin);
              pause(1000);
              while((friendPin==0)&&(foePin==0)){
                
              }
              if(foePin==1){               
                servo_angle(armServo, 1800);
              }              
              pause(1000);
              servo_angle(armServo, 0);
              Spin90R();
              Follow(70);
            }
            pause(interDelay+1000);
            ChangeVar(&lftSense,0);
            ChangeVar(&rgtSense,0);
            ChangeVar(&frtSense,0);  
            }
            
            if(deliveryCount==totalDeliveries){
              ChangeVar(&motorDir,0);
              ChangeVar(&stage,7);
              break;
            } 
            
        }while((interCount>1)&&(stage==4));
        
        if(deliveryCount==totalDeliveries){
          ChangeVar(&motorDir,0);
          ChangeVar(&stage,7);
          break;
        } 
        else{
          
          ChangeVar(&endLF,1);
          ChangeVar(&motorDir,3);
          ChangeVar(&motorSpeed,50);
          pause(3000);
          while((pos<4500)){//||(pos>4000)){;
            ;
          }
          while((pos>4000)){
            ;
          }  
          Follow(70);
        }          

        Follow(70);
        while(!detectIntersection()){
          
        }
        ChangeVar(&loopFlag,1);
        intersectionSequence();
        pause(interDelay);
        while(!detectIntersection()){
          
        }
        ChangeVar(&endLF,1);
        intersectionSequence();
            
        Turn90R();       
        ChangeVar(&stage,3);          
        
        break;
      }        
      /**************************************************************************************************************/




      /***********************************************[Default]*****************************************************/
      default:{
        ChangeVar(&endLF,1);
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
   
   fdserial *ard = fdserial_open(rxPin,txPin,0,115200);
   
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
   
   pause(3500);
   
   for(int i=0;i<1;i++){
     motorSpeed=30;
     motorDir=4;
     pause(1500);
  
     motorDir=5;
     pause(3000);
  
     motorDir=4;
     pause(1100);
  
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
   

   aid=1;

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


/**********************************************[UltraSonic Cog]****************************************************/
void getDist(void* par2){
  while(!endDist){
    frtDist = ping_cm(fPin);                 // Get cm distance from Ping)))
    lftDist = ping_cm(lPin);
    rgtDist = ping_cm(rPin);
    if((lftDist<maxDist)&&(lftSense==0)){
      ChangeVar(&lftSense,1);
    }
    if((frtDist<maxDist)&&(frtSense==0)){
      ChangeVar(&frtSense,1);
    }
    if((rgtDist<maxDist)&&(rgtSense==0)){
      ChangeVar(&rgtSense,1);
    } 
    /*
    if((lftDist>maxDist)&&(lftSense==1)){
      ChangeVar(&lftSense,0);
    }
    if((frtDist>maxDist)&&(frtSense==1)){
      ChangeVar(&frtSense,0);
    }
    if((rgtDist>maxDist)&&(rgtSense==1)){
      ChangeVar(&rgtSense,0);
    } */     
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
        servo_set(lftServo,1500);
        servo_speed(rgtServo,-motorSpeed);
        break;
      }
      
      case 3:{

        //Turn Right
        servo_speed(lftServo,motorSpeed);
        servo_set(rgtServo,1500);
        break;
      }
      
      case 4:{

        //Spin Left
        servo_speed(lftServo,-motorSpeed);
        servo_speed(rgtServo,-motorSpeed);
        break;
      }
      
      case 5:{

        //Spin Right
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
        TurnLeft(25);
        pause(10);
      }
    }
    if(errorVal>errorThresh){
      while(errorVal>errorThresh){
        TurnRight(25);
        pause(10);
      }
    }
    //Forward(50);
    //pause(5);
  }
  cogstop(lfcog);    
}  
/**************************************************************************************************************/




/**********************************************[Indicator Cog]****************************************************/
void soundLights(void* par3){
  while(1){
    switch(indMode){
      
      case 0:{
        break;
      }      
        
      case 1:{
        for(int i=0;i<indIters;i++){
         high(buzzPin);
         pause(500);
         low(buzzPin);
         pause(500); 
       }
       low(buzzPin);
       ChangeVar(&indMode,0);
       break;
     } 
     
     case 2:{
       
       for(int i=0;i<indIters;i++){
         high(LED1); 
         low(LED2);
         pause(500);
         low(LED1);
         high(LED2);
         pause(500); 
       }
       low(LED1);
       low(LED2);
       ChangeVar(&indMode,0);
       break;
     }
     
     default: {
       ChangeVar(&indMode,0);
       break;
     }        
   }
 }   
}   
/**************************************************************************************************************/







//////////////////////[Functions]//////////////////////////////
void Forward(int speed){
  ChangeVar(&motorDir,1);
  ChangeVar(&motorSpeed,speed);
}

  
void TurnLeft(int speed){
  ChangeVar(&motorDir,2);
  ChangeVar(&motorSpeed,speed);
}
 
  
void TurnRight(int speed){
  ChangeVar(&motorDir,3);
  ChangeVar(&motorSpeed,speed);
}

  
void Stop(){
  ChangeVar(&motorDir,0);
  ChangeVar(&motorSpeed,0);
}  


int detectIntersection(void){
  int interFound=0;
  if ((sensorVals[0] > refThresh) && (sensorVals[1] > refThresh) && (sensorVals[2] > refThresh)&&(sensorVals[3] > refThresh) && (sensorVals[4] > refThresh) && (sensorVals[5] > refThresh)&&(sensorVals[6] > refThresh) && (sensorVals[7] > refThresh)){
    interFound=1;
  }
  return interFound;    
}  


void intersectionSequence(void){
  ChangeVar(&indMode,1);
  ChangeVar(&indIters,1); 
} 


void deliverySequence(void){
  ChangeVar(&endLF,1);
  ChangeVar(&motorDir,0);
  ChangeVar(&indMode,2);
  ChangeVar(&indIters,1);
  pause(2000);
} 


void Follow(int samp){
  lfcog=cogstart((void*) lineFollower,(void*) samp,lfstack, sizeof(lfstack));
}  


void Turn180(void){
  ChangeVar(&endLF,1);
  ChangeVar(&motorDir,5);
  ChangeVar(&motorSpeed,30);
  pause(1800);
  while((pos<4500)){//||(pos>4000)){;
    ;
  }
  while((pos>4000)){
    ;
  } 
}


void Turn90R(void){
  ChangeVar(&endLF,1);
  ChangeVar(&motorDir,3);    
  pause(1400);
  while((pos<4500)){//||(pos>4000)){;
    ;
  }
  while((pos>4000)){
    ;
  }    
  ChangeVar(&motorDir,1);
  pause(250);
}

void Turn90L(void){
  ChangeVar(&endLF,1);
  ChangeVar(&motorDir,2);
  pause(1400);
  while((pos>2500)){//||(pos>4000)){;
    ;
  }
  while((pos<3000)){
    ;
  } 
  ChangeVar(&motorDir,1);
  pause(250);
}

void Spin90R(void){
  ChangeVar(&endLF,1);
  ChangeVar(&motorDir,5);
  ChangeVar(&motorSpeed,30);    
  pause(1600);
  ChangeVar(&motorDir,0);
}

void Spin90L(void){
  ChangeVar(&endLF,1);
  ChangeVar(&motorDir,4); 
  ChangeVar(&motorSpeed,30);   
  pause(1600);
  ChangeVar(&motorDir,0);
}

void ChangeVar(volatile int* vari,int val){
  do{
    *vari=val;
  }while(*vari!=val); 
}  