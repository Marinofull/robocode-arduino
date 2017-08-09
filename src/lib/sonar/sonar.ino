#include <Servo.h>

#define trigPin 11
#define echoPin 12
#define led 13
#define led2 10
#define secureDistance 10
#define sonnarCoef 27.623

Servo myservo;  // create servo object to control a servo
int pos = 0;    // variable to store the servo position
int limiar = secureDistance;
long distance = 200;

void setup() {
  Serial.begin (9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(led, OUTPUT);
  //pinMode(led2, OUTPUT);
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object
  extend();

}

void loop() {
  while(walk());
  lookAtLeft();
  if (isWall()){
    //rotateRight();
    delay(2000);
    extend();
  } else{
    //rotateLeft();
    delay(1000);
    extend();
  }
}

void swing()
{
  for (pos = 180; pos >= 0; pos--) // goes from 180 degrees to 0 degrees
  {
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  for (pos = 0; pos <= 180; pos++) // goes from 0 degrees to 180 degrees
  { // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
}

int walk()
{
  long duration;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(10);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration/2) / sonnarCoef;

  //print status on serial pane
  if (distance >= 200 || distance <= 0){
    Serial.println("Out of range");
  }
  else {
    Serial.print(distance);
    Serial.println(" cm");
  }
  //turn on led when its close the wall
  if (isWall()) {
    digitalWrite(led,HIGH);
    return 0;
  }
  digitalWrite(led,LOW);
  delay(500);
  return 1;
}

int isWall(){
  if (distance < limiar)
    return 1;
  return 0;
}

void lookAtLeft()
{
  myservo.write(0);
}

void lookAtRight()
{
  myservo.write(180);
}

void extend() {
  myservo.write(90);
  delay(1000);
}