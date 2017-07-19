/**
 *  * Xbee base code example;
 *   */

#include <stdio.h>
#include <string.h>

int main(){
  /* Porta serial do Xbee */
  FILE *serial_port;
  char s[200], device[11], c[2];

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
    scanf(" %[^\n]s%*c", s);
    if(s[0] == 'e') return;

    /* Envia para o Xbee */
    fprintf(serial_port, "%s\n", s);

    /* Recebe a resposta */
    fscanf(serial_port, "%s%*c", s);

    /* Imprime o resultado */
    printf("RESPOSTA -> %s\n", s);

    /* Fecha a porta serial */
    fclose(serial_port);
    scanf("%s", c); //foi na sorte isso aqui
  }

  return 0;
}
