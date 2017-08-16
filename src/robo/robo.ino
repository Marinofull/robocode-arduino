/*
  Project Name: robo
  Authors:
      Marino Hohenheim
      Pedro Marques
  Version: 0.2.0
  Boards Supported:
      Arduino UNO
  Dependencies:
      Ultrasonic, 2.1.0
      XBee-Arduino Library, 0.6.0
      XCTU, 6.3.8 (usado externamente para monitorar comunicação entre xbee e arduino)
  Description:
  Descrição do projeto
*/

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
int board[9];   // tabuleiro do jogo da velha
GameState game_state;
int turn;
bool human_starts;
bool devoAndar;

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
  for (int i = 0; i < 9; i++)
    board[i] = 0;
  game_state = BOOTING;
  #ifdef JUSTMOVE
     game_state = MOVING;
  #endif
  devoAndar = (mem[0] != 0);
  turn = 0;
  human_starts = true;
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

  // Se não estiver em DLINE e usando os pinos digitais para serial
  #ifndef DLINE
    // Configuração dos pinos do Encoder Ótico
    pinMode(ENCODER_DIREITA, INPUT_PULLUP);
    pinMode(ENCODER_ESQUERDA, INPUT_PULLUP);
    
    // Funções de Interrupção de cada um dos Encoders
    attachInterrupt(digitalPinToInterrupt(ENCODER_DIREITA), contadorDireita, CHANGE);
    attachInterrupt(digitalPinToInterrupt(ENCODER_ESQUERDA), contadorEsquerda, CHANGE);
  #endif
}

/***********************************************************************************************************
    LOOP
 ***********************************************************************************************************/

void loop() {

  if (game_state == BOOTING) {
    draw_board();
    Serial.println("Seu turno (Posições de S1 a S9)");
    game_state = PLAYING;
  }

  if (game_state == REMATCH) {
    for (int i = 0; i < 9; i++)
      board[i] = 0;
    human_starts = !human_starts;
    if (!human_starts) {
      int choices[7] = {0, 2, 4, 4, 4, 6, 8};
      int choice = random(7);
      board[choices[choice]] = 1;
      turn = 1;
    } else {
      turn = 0;
    }
    game_state = BOOTING;
  }
  
  if (game_state == MOVING && devoAndar) {
    // verifica se está no destino, se não se move
    //se_move();
    lookAhead();

    while(walk());

    lookLeft();
    if(!isWall()){
      rotateLeft();
      delay(2000);
      lookAhead();
    } else {
      lookRight();
      rotateRight();
      delay(2000);
      lookAhead();
    }
    // se está no destino volta para o jogo
    #ifndef JUSTMOVE
      game_state = PLAYING;
    #endif
  }

  if(!devoAndar) {
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
  
  //sensorDebug();
  //swing();
  //debugPattern(identify_pattern());
}

/***********************************************************************************************************
    Outras Funções
 ***********************************************************************************************************/

void se_move() {
  PatternType padrao = identify_pattern();
  switch(padrao) {
    case BEGINNING_OF_LINE:
    case INLINE:
      ACELERA_DIREITA(velocidadeDireita);
      ACELERA_ESQUERDA(velocidadeEsquerda);
      IR_PARA_FRENTE_DIREITA();
      IR_PARA_FRENTE_ESQUERDA();
      break;
    case LEAVING_TO_LEFT:
      FREIO_DIREITA();
      ACELERA_ESQUERDA(velocidadeEsquerda/2);
      IR_PARA_FRENTE_ESQUERDA();
      break;
    case LEAVING_TO_RIGHT:
      FREIO_ESQUERDA();
      ACELERA_DIREITA(velocidadeDireita/2);
      IR_PARA_FRENTE_DIREITA();
      break;
    case LEFT_BEND:
      FREIO_ESQUERDA();
      ACELERA_DIREITA(velocidadeDireita);
      IR_PARA_FRENTE_DIREITA();
      break;
    case RIGHT_BEND:
      FREIO_DIREITA();
      ACELERA_ESQUERDA(velocidadeEsquerda);
      IR_PARA_FRENTE_ESQUERDA();
      break;
    case END_OF_LINE:
      FREIO();
      delay(1000);
      ACELERA_DIREITA(velocidadeDireita);
      ACELERA_ESQUERDA(velocidadeEsquerda);
      IR_PARA_TRAS_DIREITA();
      IR_PARA_TRAS_ESQUERDA();
    default:
      FREIO();
  }
}

bool walk() {
  if(isWall()) {
    //digitalWrite(led,HIGH);
    FREIO();
    delay(500);
    return false;
  }
  //digitalWrite(led,LOW);
  ACELERA_DIREITA(velocidadeDireita);
  ACELERA_ESQUERDA(velocidadeEsquerda);
  IR_PARA_FRENTE_DIREITA();
  IR_PARA_FRENTE_ESQUERDA();
  delay(500);
  return true;
}

void rotateLeft() {
  ACELERA_DIREITA(velocidadeDireita);
  ACELERA_ESQUERDA(velocidadeEsquerda);
  IR_PARA_FRENTE_DIREITA();
  IR_PARA_TRAS_ESQUERDA();
  delay(500);
  FREIO();
  delay(500);
}

void rotateRight() {
  ACELERA_DIREITA(velocidadeDireita);
  ACELERA_ESQUERDA(velocidadeEsquerda);
  IR_PARA_FRENTE_ESQUERDA();
  IR_PARA_TRAS_DIREITA();
  delay(500);
  FREIO();
  delay(500);
}

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
  if (frente && !tras && !direita && !esquerda) {
    // no começo de uma linha, centro irrelevante
    pattern = BEGINNING_OF_LINE;
  }
  if (!frente && tras && !direita && !esquerda) {
    // fim de uma linha, centro irrelevante
    pattern = END_OF_LINE;
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
      if (1 <= p && p <= 9 && game_state == PLAYING && board[p-1] == 0) {
        human_move(p - 1);
      }
      if(p == 0) {
        if(value == 0) {
          devoAndar = false;
        } else {
          devoAndar = true;
        }
      }
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

bool isWall() {
  long distance = ultrasonic.distanceRead(CM);
  if(distance >= 200 || distance <= 0) {
    Serial.println("Out of Range");
    return false;
  } else {
    Serial.print(distance);
    Serial.println("cm");
  }

  return distance < DISTANCIA_SEGURA;
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


/***********************************************************************************************************
     TicTacToe
 ***********************************************************************************************************/

char displayChar(int c) {
  switch (c) {
    case 0:
      return ' ';
    case AI_PLAYER:
      return '0';
    case HUMAN_PLAYER:
      return 'X';
  }
}

void draw_board() {
  Serial.print(" "); Serial.print(displayChar(board[0])); Serial.print(" | "); Serial.print(displayChar(board[1])); Serial.print(" | "); Serial.print(displayChar(board[2])); Serial.println(" ");
  Serial.println("---+---+---");
  Serial.print(" "); Serial.print(displayChar(board[3])); Serial.print(" | "); Serial.print(displayChar(board[4])); Serial.print(" | "); Serial.print(displayChar(board[5])); Serial.println(" ");
  Serial.println("---+---+---");
  Serial.print(" "); Serial.print(displayChar(board[6])); Serial.print(" | "); Serial.print(displayChar(board[7])); Serial.print(" | "); Serial.print(displayChar(board[8])); Serial.println(" ");
}

int win_situation() {
  unsigned wins[8][3] = {{0, 1, 2}, {3, 4, 5}, {6, 7, 8}, {0, 3, 6}, {1, 4, 7}, {2, 5, 8}, {0, 4, 8}, {2, 4, 6}};
  int i;
  for (i = 0; i < 8; ++i) {
    if (board[wins[i][0]] != 0 &&
        board[wins[i][0]] == board[wins[i][1]] &&
        board[wins[i][0]] == board[wins[i][2]])
      return board[wins[i][2]];
  }
  return 0;
}

int minimax(int player) {
  int winner = win_situation();
  if (winner != 0) return winner * player;

  int movement = -1;
  int score = -2;
  for (int i = 0; i < 9; i++) {
    if (board[i] == 0) {
      board[i] = player;
      int this_score = - minimax(player * -1);
      if (this_score > score) {
        score = this_score;
        movement = i;
      }
      board[i] = 0;
    }
  }
  if (movement == -1) return 0;
  return score;
}

void ai_move() {
  int movement = -1;
  int score = -2;
  int i;
  for (i = 0; i < 9; i++) {
    if (board[i] == 0) {
      board[i] = 1;
      int temp_score = -1 * minimax(-1);
      if (temp_score > score) {
        score = temp_score;
        movement = i;
      }
      board[i] = 0;
    }
  }
  board[movement] = 1;

  if (!check_win_state())
    Serial.println("Seu turno (Posições de S1 a S9)");
}

void human_move(int i) {
  board[i] = HUMAN_PLAYER;
  if (!check_win_state()) {
    Serial.println("Computer Move");
    ai_move();
  }
}

bool check_win_state() {
  draw_board();
  turn++;
  int check_win = 0;
  if (turn >= 5) check_win = win_situation();
  if (turn >= 9 || check_win != 0) {
    Serial.println("Game Over");
    finish_game(check_win);
    return true;
  }
  return false;
}

void finish_game(int result) {
  switch (result) {
    case 0:
      Serial.println("\nA draw. How droll.\n");
      break;
    case AI_PLAYER:
      draw_board();
      Serial.println("\nYou lose.\n");
      break;
    case HUMAN_PLAYER:
      Serial.println("\nYou win. Inconceivable!\n");
      break;
  }
  game_state = REMATCH;
  turn = 0;
}

/***********************************************************************************************************
  Debug
  ***********************************************************************************************************/

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
