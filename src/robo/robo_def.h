#ifndef ROBO_DEF_H
#define ROBO_DEF_H

/********************************
          Definitions
 ********************************/
// Memoria do arduino para os valores de gets e sets
#define MEM 50

// comenta isso para poder jogar, caso contrario o robo apenas vai tentar andar
#define JUSTMOVE

// Jogo da Velha
#define AI_PLAYER 1
#define HUMAN_PLAYER -1
typedef enum {
  BOOTING,
  PLAYING,
  MOVING,
  REMATCH
} GameState;

//#define DLINE

// usar 2 e 3 quando a chave estiver em dline, e usar rx(0) e tx(1) quando tiver em uart
#ifdef DLINE
          #define PORTARX 2 // 0 RX 
          #define PORTATX 3 // 1 TX 
#else
          #define PORTARX 0
          #define PORTATX 1
#endif

// Sensores de luz
#define DIREITA A0
#define ESQUERDA A1
#define FRENTE A2
#define TRAS A3
#define CENTRO A4

// Padrões possíveis de reconhecer através dos sensores
typedef enum {
  ALL_BLACK,
  INLINE,
  LEAVING_TO_RIGHT,
  LEAVING_TO_LEFT,
  RIGHT_BEND,
  LEFT_BEND,
  RIGHT_BEND_REVERSE,
  LEFT_BEND_REVERSE,
  END_OF_LINE,
  BEGINNING_OF_LINE,
  ALL_WHITE
} PatternType;

#define ENCODER_DIREITA 2
#define ENCODER_ESQUERDA 3

#define ACELERADOR_DIREITA 5
#define ACELERADOR_ESQUERDA 6

//  Definição das entradas para os motores de cada lado, observem que alguns robôs precisaram ter os pinos trocados devido aos módulos de comunicação que utilizam.
//  Vejam os 4 fios unidos da ponte H que são conectados no Arduino para saberem o padrão do robô que vocês tem em mãos.
#define DIRECAO_DIREITA_1 7   //4
#define DIRECAO_DIREITA_2 8   //7

#define DIRECAO_ESQUERDA_1 9  //8
#define DIRECAO_ESQUERDA_2 10 //9

#define IR_PARA_FRENTE_DIREITA() do { digitalWrite(DIRECAO_DIREITA_1, HIGH); digitalWrite(DIRECAO_DIREITA_2, LOW); } while(false)
#define IR_PARA_FRENTE_ESQUERDA() do { digitalWrite(DIRECAO_ESQUERDA_1, HIGH); digitalWrite(DIRECAO_ESQUERDA_2, LOW); } while(false)
#define IR_PARA_FRENTE() do { IR_PARA_FRENTE_DIREITA(); IR_PARA_FRENTE_ESQUERDA(); } while(false)

#define IR_PARA_TRAS_DIREITA() do { digitalWrite(DIRECAO_DIREITA_1, LOW); digitalWrite(DIRECAO_DIREITA_2, HIGH); } while(false)
#define IR_PARA_TRAS_ESQUERDA() do { digitalWrite(DIRECAO_ESQUERDA_1, LOW); digitalWrite(DIRECAO_ESQUERDA_2, HIGH); } while(false)
#define IR_PARA_TRAS() do { IR_PARA_TRAS_DIREITA(); IR_PARA_TRAS_ESQUERDA(); } while(false)

#define ACELERA_DIREITA(VELOCIDADE) do { pwmDireita = VELOCIDADE; analogWrite(ACELERADOR_DIREITA, VELOCIDADE); } while(false)
#define ACELERA_ESQUERDA(VELOCIDADE) do { pwmEsquerda = VELOCIDADE; analogWrite(ACELERADOR_ESQUERDA, VELOCIDADE); } while(false)
#define ACELERA(VELOCIDADE) do { ACELERA_DIREITA(VELOCIDADE); ACELERA_ESQUERDA(VELOCIDADE); } while(false)

#define FREIO_DIREITA() { ACELERA_DIREITA(0); digitalWrite(DIRECAO_DIREITA_1, LOW); digitalWrite(DIRECAO_DIREITA_2, LOW); } while(false)
#define FREIO_ESQUERDA() { ACELERA_ESQUERDA(0); digitalWrite(DIRECAO_ESQUERDA_1, LOW); digitalWrite(DIRECAO_ESQUERDA_2, LOW); } while(false)
#define FREIO() do { FREIO_DIREITA(); FREIO_ESQUERDA(); } while(false)

#endif
