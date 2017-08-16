/*
  Project Name: robo
  Authors:
      Marino Hohenheim
      Pedro Marques
  Version: 0.3.0
  Boards Supported:
      Arduino UNO
  Dependencies:
      Ultrasonic, 2.1.0
      XBee-Arduino Library, 0.6.0
      XCTU, 6.3.8 (usado externamente para monitorar comunicação entre xbee e arduino)
  Description:
  Descrição do projeto
*/

#define boolean bool
#include "robo_def.h"
#include <SoftwareSerial.h>
#include <Servo.h>
#include <Ultrasonic.h>

/*
   Globals
*/
String msg;
int mem[MEM];
int pos = 0;    // variable to store the servo position

SoftwareSerial XBee(PORTARX, PORTATX);
Servo myservo;  // create servo object to control a servo
Ultrasonic ultrasonic(TRIGGER, ECHO);

/*
   Vendor stuff
*/
// Velocidade armazenada PWM
volatile int pwmDireita = 0;
volatile int pwmEsquerda = 0;

// Numero de passos do Encoder Ótico
volatile int contador_direita = 0;
volatile int contador_esquerda = 0;

// Definir velocidades
int velocidadeDireita  = 130;
int velocidadeEsquerda = 130;

/***********************************************************************************************************
    SETUP
 ***********************************************************************************************************/

void setup() {
  XBee.begin(9600);
  Serial.begin(9600);
  for (int i = 0; i < MEM; i++)
    mem[i] = 0;
  /*
    Seta as coisa reservada
  */
  myservo.attach(SERVO);
  extend();
  
  if (XBee.isListening()) {
    Serial.println("XBee está conectado!");
    XBee.println("XBee está conectado!");
  }

  /*
     Vendor stuff
  */
  // Configuração dos pinos da Ponte H
  pinMode(DIRECAO_DIREITA_1, OUTPUT);
  pinMode(DIRECAO_DIREITA_2, OUTPUT);
  pinMode(DIRECAO_ESQUERDA_1, OUTPUT);
  pinMode(DIRECAO_ESQUERDA_2, OUTPUT);

  // Configuração dos pinos do Encoder Ótico
  pinMode(ENCODER_DIREITA, INPUT_PULLUP);
  pinMode(ENCODER_ESQUERDA, INPUT_PULLUP);
  
  // Funções de Interrupção de cada um dos Encoders
  attachInterrupt(digitalPinToInterrupt(ENCODER_DIREITA), contadorDireita, RISING);
  attachInterrupt(digitalPinToInterrupt(ENCODER_ESQUERDA), contadorEsquerda, RISING);
}

/***********************************************************************************************************
    LOOP
 ***********************************************************************************************************/

void loop() {
  
  if (devoAndar()) {
    
    lookAhead();

    while(walk()); /*{
      girando_direita = contador_direita;
      girando_esquerda = contador_esquerda;
      delay(500);
      if(girando_direita == contador_direita && girando_esquerda == contador_esquerda) {
        break;
      }
    }*/

    lookLeft();
    if(!isWall()){
      rotateLeft();
      lookAhead();
    } else {
      lookRight();
      rotateRight();
      lookAhead();
    }
  } else {
    FREIO();
  }

  //Se tiver data sendo mandada pelo serial monitor
  if (Serial.available()) {
    msg = String(Serial.readString());
    Serial.print("eu li: ");
    Serial.println(msg);
    parse_msg(msg);
  }

  //Se tiver data vindo do XBee
  if (XBee.available()) {
    msg = String(XBee.readString());
    Serial.print("eu li: ");
    Serial.println(msg);
    parse_msg_xbee(msg);
  }
}

/***********************************************************************************************************
    Outras Funções
 ***********************************************************************************************************/

bool walk() {
  if(isWall()) {
    FREIO();
    delay(500);
    return false;
  }
  if(XBee.available()){
    msg = String(XBee.readString());
    parse_msg_xbee(msg);
    if(!devoAndar()) {
      return false;
    }
  }
  
  ACELERA_DIREITA(velocidadeDireita);
  ACELERA_ESQUERDA(velocidadeEsquerda);
  IR_PARA_FRENTE_DIREITA();
  IR_PARA_FRENTE_ESQUERDA();
  return true;
}

bool isWall() {
  long distance = ultrasonic.distanceRead(CM);
  if(distance >= 200 || distance <= 0) {
    //Serial.println("Out of Range");
    return false;
  } else {
    //Serial.print(distance);
    //Serial.println("cm");
  }

  return (distance <= DISTANCIA_SEGURA);
}

void rotateLeft() {
  ACELERA_DIREITA(velocidadeDireita);
  ACELERA_ESQUERDA(velocidadeEsquerda);
  IR_PARA_FRENTE_DIREITA();
  IR_PARA_TRAS_ESQUERDA();
  delay(700);
  FREIO();
  delay(500);
}

void rotateRight() {
  ACELERA_DIREITA(velocidadeDireita);
  ACELERA_ESQUERDA(velocidadeEsquerda);
  IR_PARA_FRENTE_ESQUERDA();
  IR_PARA_TRAS_DIREITA();
  delay(700);
  FREIO();
  delay(500);
}

bool devoAndar() {
  return mem[0] != 0;
}

void lookAhead() {
  myservo.write(90);
}

void lookLeft() {
  myservo.write(0);
}

void lookRight() {
  myservo.write(180);
}

void extend() {
  lookAhead();
  delay(1000);
}

void contadorDireita() {
  contador_direita++;
  //Serial.print("Contador Direita: ");
  //Serial.println(contador_direita);
}

void contadorEsquerda() {
  contador_esquerda++;
  //Serial.print("Contador Esquerda: ");
  //Serial.println(contador_esquerda);
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
      //XBee.print("could not handle ");
      //XBee.println(msg);
      break;
  }
  return 0;
}
