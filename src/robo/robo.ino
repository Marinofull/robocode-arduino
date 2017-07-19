/*
  Project Name: robo
  Authors:
  * Marino Hohenheim
  * Pedro Marques
  Version: 0.2.0
  Boards Supported:
    * Arduino UNO
  Dependencies:
  * XBee-Arduino Library, 0.6.0
  * XCTU, 6.3.8 (usado externamente para monitorar comunicação entre xbee e arduino)
  Description:
  Descrição do projeto
*/

#include <SoftwareSerial.h>
#include <Servo.h>

#define MEM 50
#define PORTARX 2
#define PORTATX 3

int parse_msg(String msg);
int parse_msg_xbee(String msg);
void sensor();
void swing();
void extend();

String msg;
int mem[MEM];
int pos = 0;    // variable to store the servo position

SoftwareSerial XBee(PORTARX, PORTATX);
Servo myservo;  // create servo object to control a servo

void setup(){
    XBee.begin(9600);
    Serial.begin(9600);
    for(int i=0; i<MEM; i++)
        mem[i]=0;
        /*
        Seta as coisa reservada
        */
    myservo.attach(9);  // attaches the servo on pin 9 to the servo object
    extend();
    delay(500);
    if(XBee.isListening()) {
      Serial.println("XBee está conectado!");
            XBee.println("XBee está conectado!");
    }
}

void loop(){

    //Se tiver data sendo mandada pelo serial monitor
    if(Serial.available()){
        msg = String(Serial.readString());
        parse_msg(msg);
    }

    //Se tiver data vindo do XBee
    if(XBee.available()) {
        msg = String(XBee.readString());
        Serial.print("eu li: ");
        Serial.println(msg);
        parse_msg_xbee(msg);
    }
    //sensor();
    //swing();
}


int parse_msg(String msg){
    char c = msg[0];
    int p, value;

    switch(c){
        case 'G':
            p = atoi(&msg[1]);
            Serial.print("R");//Response prefix
            Serial.println(mem[p]);
            break;
        case 'S':
            p = atoi(&msg[1]);
            value = atoi(&msg[msg.indexOf('=')+1]);
            mem[p] = value;
            Serial.print("R");//Response prefix
            Serial.println(mem[p]);
            break;
        case 'M':
            Serial.println(&msg[1]);
            break;
        default:
            Serial.print("could not handle ");
            Serial.println(msg);
        break;
    }
    return 0;
}

int parse_msg_xbee(String msg){
    char c = msg[0];
    int p, value;

    switch(c){
        case 'G':
            p = atoi(&msg[1]);
            XBee.print("R");//Response prefix
            XBee.println(mem[p]);
            break;
        case 'S':
            p = atoi(&msg[1]);
            value = atoi(&msg[msg.indexOf('=')+1]);
            mem[p] = value;
            XBee.print("R");//Response prefix
            XBee.println(mem[p]);
            break;
        case 'M':
            XBee.println(&msg[1]);
            break;
        default:
//        XBee.print("could not handle ");
  //      XBee.println(msg);
        break;
    }
    return 0;
}

void sensor(){
  Serial.print("Frente = ");
  Serial.println(analogRead(A3));
  Serial.print("Tras = ");
  Serial.println(analogRead(A2));
  Serial.print("Esquerda = ");
  Serial.println(analogRead(A0));
  Serial.print("Direita = ");
  Serial.println(analogRead(A1));

  delay(1000);
}

void swing()
{
  for(pos = 180; pos>=0; pos-=1)     // goes from 180 degrees to 0 degrees
  {
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  for(pos = 0; pos <= 180; pos += 1) // goes from 0 degrees to 180 degrees
  {                                  // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
}

void extend(){
  myservo.write(180);
  delay(1000);
}
