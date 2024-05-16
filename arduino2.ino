

// force Sensor - A0 and 
//resistor connected to GND and other end to power
// servo load - 10 (orange cable)
// servo power- power (red cable)
// servo ground - GND (brown cable)
//rx -tx
//tx -rx

#include <Servo.h>


////~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// Force sensor logic stuff
bool lowFood = false; 
int forceSensorPin= A0;
int forseSensorReading;

//motor stuff
Servo myServo;
const int servoPin = 10;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void setup(void) {

  Serial.begin(9600);
  myServo.attach(servoPin);
  myServo.write(55); //<--- so it starts closed

}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void loop(void) {
  
  readForceSensor(); //<--- reads first so we can use it in loop
 


  if (Serial.available() > 0){ 
    
    char recv = Serial.read(); //<--- has to get a T from arduino 1 
    if(recv == 'T'){
    while(lowFood){ //<-- when arduino 1 says so and theres less than 10 g of food
        readForceSensor();
        delay(3000);
        releaseFood();
    }
    }
   }
 
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void releaseFood() {
  
  myServo.write(130);  //<--opens lid

  delay(500); //<--- waits a little bit so food comes out
 
  myServo.write(55); //<---closes lid

  delay(3000);  ///<---unsure if this long but it was used on previous tests before sensor 

}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~`
// this function weights the food and when it reaches 10, changes the flag to false 
bool readForceSensor() {

  
   forseSensorReading = analogRead(forceSensorPin); //<---- just reading from force sensor
   delay(3000);


  //~~~~~~~~~~~~~ printing how much food we have on bowl to serial monitor
  Serial.println("Analog reading =  ");

  Serial.println(forseSensorReading);
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  
  if(forseSensorReading < 11){ //<-- lid should close if reaches 10
    lowFood = true;
  }
  else{
    lowFood = false;  ///<---- stays open if not
  }
  
  return lowFood;

}
