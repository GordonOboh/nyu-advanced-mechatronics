import cv2
from cv2 import aruco
#import keyboard
from time import sleep
import RPi.GPIO as GPIO
from playsound import playsound

#___________________________________________________________________________
#Friend or Foe Function
def FriendOrFoe(TagID):
    ontime=1
    if TagID<=9:
        print("Friend with Tag ID " + str(TagID) + " is Identified")
        playsound("Hasta La Vista Baby.mp3")
        for i in range(ontime):
            GPIO.output(FriendPin,GPIO.HIGH)
            sleep(0.5)
        GPIO.output(FriendPin,GPIO.LOW)

    elif TagID>9:
        print("Enemy with Tag ID " + str(TagID) + " is Identified")
        playsound("MK.mp3")
        for i in range(ontime):
            GPIO.output(FoePin,GPIO.HIGH)
            sleep(0.5)
        GPIO.output(FoePin,GPIO.LOW)
    else:
        print(TagID)
#__________________________________________
#def StartOrStop(PropPin,TagID):
#	#while True:
#		PropCheck=GPIO.input(PropPin)
#		if(PropCheck==True):                                                       ####
#			GPIO.output(FriendPin,GPIO.HIGH)
#			GPIO.output(FoePin,GPIO.HIGH)
#			sleep(2)
#			GPIO.output(FriendPin,GPIO.LOW)
#			GPIO.output(FoePin,GPIO.LOW)
#		else:
#			GPIO.output(FriendPin,GPIO.LOW)
#			GPIO.output(FoePin,GPIO.LOW)
#___________________________________________________________________________

camPort=0
markerSize=6
totalMarkers=250

camHandle = cv2.VideoCapture(camPort)#,cv2.CAP_DSHOW)
# Change the fps
camHandle.set(cv2.CAP_PROP_FPS,5)

key=getattr(aruco,f'DICT_{markerSize}X{markerSize}_{totalMarkers}')
arucoDict = aruco.Dictionary_get(key)

#____________________
NoneCompare=None
print("You can Scan now")                                 #####
#____________________

#_________________________________________________________Needed for FriendOrFoe() and StartOrStop()
# Pin Initialization
FriendPin=2
FoePin=3
PropPin=4

# IDK
GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)

# Pin Direction
GPIO.setup(FriendPin,GPIO.OUT)
GPIO.setup(FoePin,GPIO.OUT)
GPIO.setup(PropPin,GPIO.IN)


while True:
	                                 				   #answer=str(input('Run again? (y/n): '))
	answer = 'y'                                       #start_sig=True True #Replace this with your code where you check for the start signal
	if answer == "y":                                  #start_sig==True
		ret,img = camHandle.read()
		gray=cv2.cvtColor(img,cv2.COLOR_BGR2GRAY)
		bboxs,ids,rejected=aruco.detectMarkers(gray,arucoDict)
		#_________________________________________________
		if type(NoneCompare)!=type(ids):
			TagID=ids[0][0]
			print(TagID)
			FriendOrFoe(TagID)
			#sleep(1)
			answer = str('n')
			#img=0
			print("You can scan again")
		#_________________________________________________
	elif answer == 'n':                                  #start_sig == False:
		sleep(0.25)
		#answer = str('y')	