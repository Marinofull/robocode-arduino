/**
 *  * Xbee base code example;
 *   */

#include <stdio.h>
#include <string.h>

int main(){
    /* Porta serial do Xbee */
    FILE *serial_port = fopen("/dev/ttyUSB1", "r+");

    /* Se nulo a porta está sendo usada ou você não tem privilégios */
    if(serial_port == NULL){
        printf("Error opening port");
        return 1;
    }

    char c, s[1000];
    int inteiro;

    /* Lê a entrada */
    printf("ENTRADA  -> ");
    /*
     * */
    //scanf("%c%d", &c, &inteiro);
    scanf(" %[^\n]s%*c", s);

    /* Envia para o Xbee */
    /*
     * */
    //fprintf(serial_port, "%c%d\n", c, inteiro);
    fprintf(serial_port, "%s\n", s);

    /* Recebe a resposta */
    /*
     * */
    //fscanf(serial_port, "%c%d%*c", &c, &inteiro);
    fscanf(serial_port, "%s%*c", s);

    /* Imprime o resultado */
    //printf("RESPOSTA -> %c%d\n", c, inteiro);
    printf("RESPOSTA -> %s\n", s);

    /* Fecha a porta serial */
    fclose(serial_port);

    return 0;
}
