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
#define DIREITA A0
#define ESQUERDA A1
#define FRENTE A2
#define TRAS A3
#define CENTRO A4

int parse_msg(String msg);
int parse_msg_xbee(String msg);
void sensorDebug();
void swing();
void extend();

String msg;
int mem[MEM];
int pos = 0;    // variable to store the servo position

SoftwareSerial XBee(PORTARX, PORTATX);
Servo myservo;  // create servo object to control a servo

void setup() {
  XBee.begin(9600);
  Serial.begin(9600);
  for (int i = 0; i < MEM; i++)
    mem[i] = 0;
  /*
    Seta as coisa reservada
  */
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object
  extend();
  delay(500);
  if (XBee.isListening()) {
    Serial.println("XBee está conectado!");
    XBee.println("XBee está conectado!");
  }
}

void loop() {

  //Se tiver data sendo mandada pelo serial monitor
  if (Serial.available()) {
    msg = String(Serial.readString());
    parse_msg(msg);
  }

  //Se tiver data vindo do XBee
  if (XBee.available()) {
    msg = String(XBee.readString());
    Serial.print("eu li: ");
    Serial.println(msg);
    parse_msg_xbee(msg);
  }
  //sensorDebug();
  //swing();
  //debugPattern(identify_pattern());
}

int identify_pattern() {
  bool frente = readingLine(analogRead(FRENTE));
  bool tras = readingLine(analogRead(TRAS));
  bool direita = readingLine(analogRead(DIREITA));
  bool esquerda = readingLine(analogRead(ESQUERDA));
  bool centro = readingLine(analogRead(CENTRO));

  if(frente && tras && centro && direita && esquerda) {
    // tudo ligado, em cima de área preta
  }

  if(frente && tras && centro && !direta && !esquerda) {
    // em cima da linha
  }
  if(frente && tras && centro && direita && !esquerda) {
    // sainda da linha à direita
    // corrigir para a esquerda
  }
  if(frente && tras && centro && !direita && esquerda) {
    // sainda da linha à esquerda
    // corrigir para a direita
  }
  if(!frente && tras && centro && direita && !esquerda) {
    // em cima de curva fechada para a direita
  }
  if(!frente && tras && centro && !direita && esquerda) {
    // em cima de curva fechada para a esquerda
  }
  if(frente && !tras && centro && direita && !esquerda) {
    // em cima de curva para a direita indo para a frente
  }
  if(frente && !tras && centro && !direita && esquerda) {
    // em cima de curva para a esquerda indo para a frente
  }

  if(!frente && !tras && !centro && !direita && !esquerda) {
    // tudo desligado, em cima de área branca
  }

}


int parse_msg(String msg) {
  char c = msg[0];
  int p, value;

  switch (c) {
    case 'G':
      p = atoi(&msg[1]);
      Serial.print("R");//Response prefix
      Serial.println(mem[p]);
      break;
    case 'S':
      p = atoi(&msg[1]);
      value = atoi(&msg[msg.indexOf('=') + 1]);
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

int parse_msg_xbee(String msg) {
  char c = msg[0];
  int p, value;

  switch (c) {
    case 'G':
      p = atoi(&msg[1]);
      XBee.print("R");//Response prefix
      XBee.println(mem[p]);
      break;
    case 'S':
      p = atoi(&msg[1]);
      value = atoi(&msg[msg.indexOf('=') + 1]);
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

void sensorDebug() {
  Serial.print("Frente = ");
  Serial.println(analogRead(FRENTE));
  Serial.print("Centro = ");
  Serial.println(analogRead(CENTRO));
  Serial.print("Tras = ");
  Serial.println(analogRead(TRAS));
  Serial.print("Direita = ");
  Serial.println(analogRead(DIREITA));
  Serial.print("Esquerda = ");
  Serial.println(analogRead(ESQUERDA));

  delay(2000);
}

void swing()
{
  for (pos = 180; pos >= 0; pos -= 1) // goes from 180 degrees to 0 degrees
  {
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  for (pos = 0; pos <= 180; pos += 1) // goes from 0 degrees to 180 degrees
  { // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
}

void extend() {
  myservo.write(180);
  delay(1000);
}

bool readingLine(int value) {
  return value >= 512;
}

void debugPattern(int pattern) {

}
