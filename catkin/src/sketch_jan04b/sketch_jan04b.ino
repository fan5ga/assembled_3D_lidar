#include <Servo.h> 
#include <Wire.h>

//Defines
#define PAN_PIN 10        //Pan Servo Digital Pin
#define TILT_PIN 11       //Tilt Servo Digital Pin



#define PUSHBUTTON_PIN 4  //Pushbutton Digital Pin


//max/min puse values in microseconds to send to the servo
#define PAN_MIN      0  //full counterclockwise for RobotGeek 180 degree servo
#define PAN_MAX      180 //full clockwise for RobotGeek 180 degree servo
#define TILT_MIN  90    //full counterclockwise for RobotGeek 180 degree servo
#define TILT_MAX  150    //full clockwise for RobotGeek 180 degree servo

#define INCREMENT 1

#define DEBUG 0

Servo panServo, tiltServo;  // create servo objects to control the pan and tilt servos



int buttonState;             // the current state of the pushbuton

float button[32];


void setup() 
{
  Wire.begin(); 
  //initialize servos
  panServo.attach(PAN_PIN);  // attaches/activates the pan servo on pin PAN_PIN and sets lower/upper limits that can be sent to the servo
  tiltServo.attach(TILT_PIN);  // attaches/activates the tilt servo on pin TILT_PIN and sets lower/upper limits that can be sent to the servo

  //initalize digital pins
  pinMode(PUSHBUTTON_PIN, INPUT);
 
  
  Serial.begin(115200);
  panServo.write(0);
  tiltServo.write(TILT_MIN);
  delay(1500);
  //Serial.println("setup");

} 

void loop() 
{ 
  char panValue = PAN_PIN;
  char tileValue = TILT_MIN;
  
  while(1)
  {
    //Serial.println("start");
    for (int tiltValue=TILT_MIN; tiltValue<=TILT_MAX; tiltValue+=INCREMENT)
    {
      
      tiltServo.write(tiltValue);
      delay(100);
      


      


      //Clockwise
      Serial.write("*,1,");
      for(int panValue=PAN_MIN; panValue<=PAN_MAX; panValue+=INCREMENT)
      {

        panServo.write(panValue);
        delay(35);
        
        //Scan distance
        float distanceInMeter = readScan();

        Serial.write(String(distanceInMeter,2).c_str());
        Serial.write(',');
        

        if(panValue + INCREMENT >  PAN_MAX)
        {
           Serial.println(String(tiltValue,DEC));

          delay(35);
        }
        
        if(digitalRead(PUSHBUTTON_PIN) == HIGH) stops();
      }
      if(digitalRead(PUSHBUTTON_PIN) == HIGH) stops();


    


      //Counterclockwise
      
      Serial.write("*,-1,");
      for(int panValue=PAN_MAX; panValue>=PAN_MIN; panValue-=INCREMENT)
      {
        
        panServo.write(panValue);
        delay(50);
        
        //Scan distance
        float distanceInMeter = readScan();
        Serial.write(String(distanceInMeter,2).c_str());
        Serial.write(',');
        if(panValue - INCREMENT <  PAN_MIN)
        {
          Serial.println(String(tiltValue,DEC));
          
          delay(80);
        }
        if(digitalRead(PUSHBUTTON_PIN) == HIGH) stops();

      }
      // button interrupt
     if(digitalRead(PUSHBUTTON_PIN) == HIGH) stops();
     
      
      //panServo.write(0);
      delay(500);

    }

  }
  
  
}
float readScan()
{
  Wire.requestFrom(0x55, 2);
  unsigned long timeNow = millis();
  while (Wire.available() < 2)
  {
    if(millis()-timeNow > 500)
      return -1;
  };

  int byteH = Wire.read();
  int byteL = Wire.read();
  float distanceInMeter = (byteH * 256 + byteL)/100.0;
  return distanceInMeter;
  
  
  
} 
void stops()
{
  Serial.println("stop");
  while(1){};
}

