#include <stdio.h>
#include <stdlib.h>

#define false 0
#define true !false
#define MAX 8

int tabuleiro[100][100];
int destino;
int T = MAX;

void printTabuleiro();
int ehValido(int x, int y);
int nRainhas(int nR);

int main(int argc, char *argv[]) {
    int nR = MAX;
    nRainhas(nR);
    printTabuleiro();
    return 0;
}

int nRainhas(int nR) {
    if (nR == 0)
        return true;
    int i, j;
    for (i=0; i<T;i++)
        for(j=0; j<T;j++)
            if (ehValido(i, j)) {
                tabuleiro[i][j] = 1;
                if (nRainhas(nR-1))
                    return true;
                tabuleiro[i][j] = 0;
            }
    return false;
}

int ehValido(int x, int y) {
    int i, j;

    for (j=0; j<T; j++)
        if (tabuleiro[x][j] == 1)
            return false;
    for (i=0; i<T; i++)
        if (tabuleiro[i][y] == 1)
            return false;
    for (i=0; i<T; i++)
        for (j=0; j<T; j++){
            if (i+j == x+y && (i!=x && j!=y))
                if (tabuleiro[i][j])
                    return false;
            if (i-j == x-y && (i!=x && j!=y))
                if (tabuleiro[i][j])
                    return false;
        }
    return true;
}

void printTabuleiro() {
    int i, j;

    for (i=0; i<T; i++){
        for (j=0; j<T; j++)
            printf("%d ", tabuleiro[i][j]);
        printf("\n");
    }
}

/*
algoritmoGenerico(entrada, pontoPartida)
    se pontoPartida==destino
        return true
    se pontoPartida==null
        escolhaPontodePartida
    para cada caminho a partir daqui
        se caminhoValido
            entra no caminho
            se algoritmoGenerico(entrada, caminho) == true
                return true
            desfaz escolha do caminho
    return false
*/
