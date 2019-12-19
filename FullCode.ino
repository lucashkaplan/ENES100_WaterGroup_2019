#include <Servo.h>
#include "Enes100.h"
Servo myservo;

//wifi variables
int markerid = 12;
int wifitx = 10, wifirx = 11;

//motor stuff
int rightDir1 = 7;
int rightDir2 = 8;
int rightSpeed = 5;

int leftDir1 = 12;
int leftDir2 = 13;
int leftSpeed = 6;

//pump stuff
int pumpDir1 = 2;
int pumpSpeed = 3;

//sensor stuff
int trigPin = 4;
int leftDistanceSensor = 14;//A0
int rightDistanceSensor = 15;//A1
int salinitySensor = 16;//A2
int photoresistor = 17;//A3  LED is powered with 5V (always on)
int depthSensor = 18;//A4

double distance;
double duration;
int salinity;
int pollution;
double depth;

//navigation stuff
double x, y, theta, y1, y2, msX, msY, difference, leftSensor, rightSensor;
double xTemp, yTemp, thetaTemp;
 

void setup(){
  delay(3000);

  //Serial.begin(9600);
  Enes100.begin("Water Those", WATER, markerid, wifitx, wifirx);

  delay(3000);
 
  //motor stuff
  pinMode(rightDir1,OUTPUT);
  pinMode(rightDir2,OUTPUT);
  pinMode(rightSpeed,OUTPUT);

  pinMode(leftDir1,OUTPUT);
  pinMode(leftDir2,OUTPUT);
  pinMode(leftSpeed,OUTPUT);

  //pump stuff
  pinMode(pumpDir1,OUTPUT);
  pinMode(pumpSpeed,OUTPUT);
 
  //servo stuff
  myservo.attach(9);
 
  //sensor stuff
  pinMode(salinitySensor,INPUT);
  pinMode(photoresistor,INPUT);
  pinMode(depthSensor,INPUT);

  pinMode(trigPin,OUTPUT);
 
 
  double xPos;
  double yPos;
  double theta;

  double xPoolPos;
  double yPoolPos;
 

  //make sure everything's stopped
  servoStop();
  pumpOff();
  stopMoving();

}

void loop(){
 
  //prints location and moves only if coordinates are accurate and location updates or else it will stop moving
  if(Enes100.updateLocation()&&(Enes100.destination.y != 0 && Enes100.destination.x != 0)){
    Enes100.println("X-Destination: " + String(Enes100.destination.x));
    Enes100.println("Y-Destination: " + String(Enes100.destination.y));
    Enes100.println("(" + String(Enes100.location.x) + ", " + String(Enes100.location.y) + ", " + String(Enes100.location.theta) + ")");
    forward(150);

        if (Enes100.location.y >= 1.7){
          Enes100.println("Upper Wall: " + String(Enes100.location.y));
          upperWall();
        }
        if (Enes100.location.y <= 0.3){
          Enes100.println("Lower Wall: " + String(Enes100.location.y));
          lowerWall();
        }
        if (Enes100.location.theta >= 1.55 && Enes100.location.theta <=3.14 || Enes100.location.theta <= -1.57 && Enes100.location.theta >= -3.14) {
           Enes100.println("Wrong Direction: " + String(Enes100.location.theta));
          turnWrong();
        }
      
        //turns left if both sensors measure less than 10 cm
      if (readLeftDistanceSensor() <= 10.0){
          delay(200);
          if (readRightDistanceSensor() <= 10.0){
                delay(200);
                backward(200);
                delay(1000);
                Enes100.println("Both sensors close");
                turnLeftCheck();
          }
          }
        
        
        //turns left if it right sensor measures less than or equal to 10 cm
        if (readLeftDistanceSensor() >= 10.0) {
            delay(200);
            if(readRightDistanceSensor() <= 10.0){
                delay(200);
                backward(200);
                delay(2000);
                
                Enes100.println("Right sensor close");
                turnLeftCheck();
            }
           }
          
        
      
        //turns right if left sensor measures less than or equal to 10 cm 
            delay(200);
        if (readLeftDistanceSensor() <= 10.0) {
            delay(200);
            Enes100.println(readLeftDistanceSensor());
            delay(200);
            if (readRightDistanceSensor() >= 10.0){
                delay(200);
                backward(200);
                delay(2000);
                Enes100.println("Left sensor close");
              turnRightCheck();
            }
        }
        missionSite();    
  } else {
    Enes100.println("Error!");
    //Enes100.println("Left Sensor " + String (leftSensor) + "cm");
    //Enes100.println("Right Sensor " + String (rightSensor) + "cm");
    stopMoving();
  }

}

//makes sure OSV is never facing backwards
void turnWrong() {
    turnLeft(150);
    while (true) {
        Enes100.updateLocation();
        if (Enes100.location.theta >= -.03 && Enes100.location.theta <= 0.03) {
            break;
        }
    }
}

//turns left when it hits an obstacle
void turnLeftCheck() {
    turnLeft(150);
    while (true){
        Enes100.updateLocation();
        if (Enes100.location.theta >=0.87 && Enes100.location.theta <=0.93){
            forward(150);
            delay(3000);
            turnRight(150);
            while (true) {
                Enes100.updateLocation();
                if (Enes100.location.theta >=-0.05 && Enes100.location.theta <=0.05){
                    stopMoving();
                    break;
                }
            }
            break;
        }
    }
}

//turns right when it hits an obstacle
void turnRightCheck() {
    turnRight(150);
    while (true){
        Enes100.updateLocation();
        if (Enes100.location.theta <=-0.76 && Enes100.location.theta >=-0.82){
            forward(150);
            delay(3000);
            turnLeft(150);
            while (true) {
                Enes100.updateLocation();
                if (Enes100.location.theta >=-0.05 && Enes100.location.theta <=0.05){
                    stopMoving();
                    break;
                }
            }
            break;
        }
    }
}

//moves to mission site when in proper x-coordinate value
void missionSite(){
    msY = Enes100.destination.y;
    msX = Enes100.destination.x + 0.05;
    difference = absoluteValue(msX, Enes100.location.x - 0.07);
    if (difference < 0.05) {
        //if mission site y-coordinate is greater than the y-coordinate of the OSV, then it moves up
        if (msY > Enes100.location.y) {
            Enes100.println("Turn left");
            turnLeft(125);
            while (true) {
                Enes100.updateLocation();
                Enes100.println(Enes100.location.theta);
                if (Enes100.location.theta > 1.565 && Enes100.location.theta < 1.585){
                    forward(150);
                    while (true) {
                        Enes100.updateLocation();
                        if (absoluteValue(msX,Enes100.location.x) < 0.05) {
                            if (absoluteValue(msY,Enes100.location.y) < 0.20){
                                stopMoving();
                                break;
                          }
                     }
                  }
              }
          }
      }
      //if mission site y-coordinate is less than the y-coordinate of the OSV, then it moves down
      if (msY < Enes100.location.y) {
          Enes100.println("Turn right");
          turnRight(125);
          while (true) {
              Enes100.updateLocation();
               Enes100.println(Enes100.location.theta);
              if (Enes100.location.theta < -1.565 && Enes100.location.theta > -1.585){
                  forward(150);
                  while (true) {
                      Enes100.updateLocation();
                      if (absoluteValue(msX,Enes100.location.x) < 0.05) {
                          if (absoluteValue(msY,Enes100.location.y) < 0.20){
                              stopMoving();
                        }
                   }
                }
            }
        }
      }
     
    }
  //Arrived at pool
  //Arm down
  servoForward(210);
  delay(2000);
  //Arm stop
  servoStop();
  //Collect water
  pumpOn(210);
  delay(5000);
  pumpOff();
  //Collect sensor values
  salinity = analogRead(salinitySensor);
  pollution = analogRead(photoresistor);
  depth = analogRead(depthSensor);
  //Interpret and transmit values
  //interpret salinity
  if(salinity>100){
    Enes100.println("Salt Water");
  }else{
    Enes100.println("Fresh Water");
  }
  //interpret pollution
  if(pollution>100){
    Enes100.println("Polluted");
  }else{
    Enes100.println("Clean");
  }
  Enes100.println(String(double(depth*.01))+"cm");
  //
  //Arm up
  servoBackward(210);
  delay(2000);
  //Arm stop
  servoStop();
  //Mission Complete

}

//adjusts OSV if it hits the upper wall
void upperWall() {
    turnRight(150);
    while (true) {
        Enes100.updateLocation();
        //turns OSV until it faces downward
        if (Enes100.location.theta <= -1.55 && Enes100.location.theta >= -1.59) {
            forward(150);
            while (true){
                Enes100.updateLocation();
                delay(2000);
                turnLeft(150);
                //moves forward for two seconds and turns until it faces right
                while (true) {
                    Enes100.updateLocation();
                    if (Enes100.location.theta >= -0.02 && Enes100.location.theta <=0.02) {
                        break;
                    }
                }
                break;
              
              
            }
            break;
        }
    }
}

//adjusts OSV if it hits the lower wall
void lowerWall() {
    turnLeft(150);
    while (true) {
        Enes100.updateLocation();
         //turns OSV until it faces upward
        if (Enes100.location.theta >= 1.56 && Enes100.location.theta <= 1.58) {
            forward(150);
            while (true){
             Enes100.updateLocation();
             turnRight(150);
             //moves forward for two seconds and turns until it faces right
             while (true) {
                 Enes100.updateLocation();
                 if (Enes100.location.theta >= -0.01 && Enes100.location.theta <=0.01) {
                      break;
                 }
             }
             break;
              
            }
            break;
        }
    }
}

//absolute value, simulator did not have an absolute value function
double absoluteValue(double a, double b){
    if (a > b){
        return a - b;
    }
    if (a <= b) {
        return b - a;
    }
}

//rate goes from 0 (stopped) to 255 (full speed)
//Dir1 = 1 and Dir2 = 0 is forward, reverse is backward
void pumpOn(int rate){
    digitalWrite(pumpDir1,1);
    digitalWrite(pumpSpeed,rate);
}

void pumpOff(){
    digitalWrite(pumpDir1,1);
    digitalWrite(pumpSpeed,0);
}

//rate goes from 0 (stopped) to 90 (full speed)
void servoForward(int rate){
  myservo.write(90+rate);
}

void servoBackward(int rate){
  myservo.write(90-rate);
}

void servoStop(){
  myservo.write(90);
}

void turnLeft(int rate){
    digitalWrite(rightDir1,1);
    digitalWrite(rightDir2,0);
   
    digitalWrite(leftDir1,0);
    digitalWrite(leftDir2,1);
   
    analogWrite(rightSpeed,rate);
    analogWrite(leftSpeed,rate);
}

void turnRight(int rate){
    digitalWrite(rightDir1,0);
    digitalWrite(rightDir2,1);
   
    digitalWrite(leftDir1,1);
    digitalWrite(leftDir2,0);
   
    analogWrite(rightSpeed,rate);
    analogWrite(leftSpeed,rate);
}

void forward(int rate){
    digitalWrite(rightDir1,1);
    digitalWrite(rightDir2,0);
   
    digitalWrite(leftDir1,1);
    digitalWrite(leftDir2,0);
   
    analogWrite(rightSpeed,rate);
    analogWrite(leftSpeed,rate);
}

void backward(int rate){
    digitalWrite(rightDir1,0);
    digitalWrite(rightDir2,1);
   
    digitalWrite(leftDir1,0);
    digitalWrite(leftDir2,1);
   
    analogWrite(rightSpeed,rate);
    analogWrite(leftSpeed,rate);
}

void stopMoving(){
    analogWrite(rightSpeed,0);
    analogWrite(leftSpeed,0);
}

//trigPin is always trigPin, echoPin is either leftDistanceSensor, rightDistanceSensor or depthSensor
double readDistanceSensor(int trigPin, int echoPin){
    // Clears the trigPin
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    // Sets the trigPin on HIGH state for 10 micro seconds
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(echoPin, HIGH);
    // Calculating the distance
    distance= duration*0.034/2;
    return distance;
}

double readLeftDistanceSensor(){
  distance=readDistanceSensor(trigPin, leftDistanceSensor);
    return distance;
}

double readRightDistanceSensor(){
  distance=readDistanceSensor(trigPin, rightDistanceSensor);
    return distance;
}

double readDepth(){
  return readDistanceSensor(trigPin, depthSensor);
}
