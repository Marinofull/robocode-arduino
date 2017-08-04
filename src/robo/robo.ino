/*
  Project Name: robo
  Authors:
      Marino Hohenheim
      Pedro Marques
  Version: 0.2.0
  Boards Supported:
      Arduino UNO
  Dependencies:
      XBee-Arduino Library, 0.6.0
      XCTU, 6.3.8 (usado externamente para monitorar comunicação entre xbee e arduino)
  Description:
  Descrição do projeto
*/

#include "robo_def.h"
#include <SoftwareSerial.h>
#include <Servo.h>

/*
   Globals
*/
String msg;
int mem[MEM];
int pos = 0;    // variable to store the servo position

SoftwareSerial XBee(PORTARX, PORTATX);
Servo myservo;  // create servo object to control a servo

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
  //myservo.attach(9);  // attaches the servo on pin 9 to the servo object
  //extend();
  delay(500);
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
  //pinMode(ENCODER_DIREITA, INPUT_PULLUP);
  //pinMode(ENCODER_ESQUERDA, INPUT_PULLUP);

  // Funções de Interrupção de cada um dos Encoders
  //attachInterrupt(digitalPinToInterrupt(ENCODER_DIREITA), contadorDireita, CHANGE);
  //attachInterrupt(digitalPinToInterrupt(ENCODER_ESQUERDA), contadorEsquerda, CHANGE);
}

/***********************************************************************************************************
    LOOP
 ***********************************************************************************************************/

void loop() {

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
  //sensorDebug();
  //swing();
  debugPattern(identify_pattern());

  /*
     Vendor stuff
     ACELERA_DIREITA(velocidadeDireita);
    ACELERA_ESQUERDA(velocidadeEsquerda);
    IR_PARA_FRENTE_DIREITA();
    IR_PARA_FRENTE_ESQUERDA();
    delay(1000);
    FREIO();
    delay(1500);
  */
}

/***********************************************************************************************************
    Outras Funções
 ***********************************************************************************************************/

PatternType identify_pattern() {
  bool frente = readingLine(analogRead(FRENTE));
  bool tras = readingLine(analogRead(TRAS));
  bool direita = readingLine(analogRead(DIREITA));
  bool esquerda = readingLine(analogRead(ESQUERDA));
  bool centro = readingLine(analogRead(CENTRO));

  PatternType pattern = ALL_WHITE;

  if (frente && tras && centro && direita && esquerda) {
    // tudo ligado, em cima de área preta
    pattern = ALL_BLACK;
  }

  if (frente && tras && centro && !direita && !esquerda) {
    // em cima da linha
    pattern = INLINE;
  }
  if (frente && tras && centro && direita && !esquerda) {
    // sainda da linha à direita
    // corrigir para a esquerda
    pattern = LEAVING_TO_RIGHT;
  }
  if (frente && tras && centro && !direita && esquerda) {
    // sainda da linha à esquerda
    // corrigir para a direita
    pattern = LEAVING_TO_LEFT;
  }
  if (!frente && tras && centro && direita && !esquerda) {
    // em cima de curva fechada para a direita
    pattern = RIGHT_BEND;
  }
  if (!frente && tras && centro && !direita && esquerda) {
    // em cima de curva fechada para a esquerda
    pattern = LEFT_BEND;
  }
  if (frente && !tras && centro && direita && !esquerda) {
    // em cima de curva para a direita indo para a frente
    pattern = RIGHT_BEND_REVERSE;
  }
  if (frente && !tras && centro && !direita && esquerda) {
    // em cima de curva para a esquerda indo para a frente
    pattern = LEFT_BEND_REVERSE;
  }

  if (!frente && !tras && !centro && !direita && !esquerda) {
    // tudo desligado, em cima de área branca
    pattern = ALL_WHITE;
  }

  return pattern;
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

void contadorDireita() {
  contador_direita++;
}

void contadorEsquerda() {
  contador_esquerda++;
}

void debugPattern(PatternType pattern) {
  Serial.println("\nIdentifying pattern:");
  switch (pattern) {
    case ALL_BLACK:
      Serial.println("All black");
      break;
    case INLINE:
      Serial.println("Over line");
      break;
    case LEAVING_TO_RIGHT:
      Serial.println("Saindo da linha para a direita");
      break;
    case LEAVING_TO_LEFT:
      Serial.println("Saindo da linha para a esquerda");
      break;
    case RIGHT_BEND:
      Serial.println("Canto/Curva para a direita");
      break;
    case LEFT_BEND:
      Serial.println("Canto/Curva para a esquerda");
      break;
    case RIGHT_BEND_REVERSE:
      Serial.println("Canto/Curva vindo da direita");
      break;
    case LEFT_BEND_REVERSE:
      Serial.println("Canto/Curva vindo da esquerda");
      break;
    case ALL_WHITE:
      Serial.println("All white");
      break;
    default:
      Serial.println("Not recognized");
      break;
  }
  delay(2000);
}
