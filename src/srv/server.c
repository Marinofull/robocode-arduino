/**
 *  * Xbee base code example;
 *   */

#include <stdio.h>
#include <string.h>

void w_move(FILE *serial_port);
void s_move(FILE *serial_port);
void q_move(FILE *serial_port);
void e_move(FILE *serial_port);
void a_move(FILE *serial_port);
void d_move(FILE *serial_port);
void speedup(FILE *serial_port, int *n);//j
void breaker(FILE *serial_port, int *n);//k
int normalize(int *n, int i);

int main(){
  /* Porta serial do Xbee */
  FILE *serial_port;
  char s[200], device[11], c[2], move;
  int gas_pedall = 0;

  printf("Set device /dev/ttyUSB number -> ");
  scanf("%s", c);
  strcpy(device, "/dev/ttyUSB");
  strcat(device, c);


  for(;;){

    serial_port = fopen(device, "r+");

    /* Se nulo a porta está sendo usada ou você não tem privilégios */
    if(serial_port == NULL){
      printf("Error opening port");
      return 1;
    }

    /* Lê a entrada */
    printf("ENTRADA  -> ");
    scanf(" %c", &move);
    //scanf(" %[^\n]s%*c", s);
    //if(s[0] == 'e') return;
    if(move == 'x') return;

    /* Envia para o Xbee */
    //fprintf(serial_port, "%s\n", s);

    switch(move){
      case 'w':
        w_move(serial_port); break;
      case 's':
        s_move(serial_port); break;
      case 'q':
        q_move(serial_port); break;
      case 'e':
        e_move(serial_port); break;
      case 'a':
        a_move(serial_port); break;
      case 'd':
        d_move(serial_port); break;
      case 'k':
        speedup(serial_port, &gas_pedall); break;
      case 'j':
        breaker(serial_port, &gas_pedall); break;
    }

    /* Recebe resposta */
    //fscanf(serial_port, "%s%*c", s);
    //printf("RESPOSTA -> %s\n", s);

    /* Fecha a porta serial */
    fclose(serial_port);
  }

  return 0;
}

void w_move(FILE *serial_port){
  fprintf(serial_port, "S0=1\n");
  fprintf(serial_port, "S1=1\n");
}
void s_move(FILE *serial_port){
  fprintf(serial_port, "S0=-1\n");
  fprintf(serial_port, "S1=-1\n");
}
void q_move(FILE *serial_port){
  fprintf(serial_port, "S0=1\n");
  fprintf(serial_port, "S1=0\n");
}
void e_move(FILE *serial_port){
  fprintf(serial_port, "S0=0\n");
  fprintf(serial_port, "S1=1\n");
}
void a_move(FILE *serial_port){
  fprintf(serial_port, "S0=-1\n");
  sleep(1);
  fprintf(serial_port, "S1=0\n");
}
void d_move(FILE *serial_port){
  fprintf(serial_port, "S0=0\n");
  sleep(1);
  fprintf(serial_port, "S1=-1\n");
}
void speedup(FILE *serial_port, int *n){
  int i = normalize(n, 17);
  fprintf(serial_port, "S2=%d\n", i);
  fprintf(serial_port, "S3=%d\n", i);
}
void breaker(FILE *serial_port, int *n){
  int i = normalize(n, -17);
  fprintf(serial_port, "S2=%d\n", i);
  fprintf(serial_port, "S3=%d\n", i);
}

int normalize(int *n, int i){
  if (*n+i > 255 || *n+i < 0)
    return *n;
  *n = *n + i; return *n;
}
