#include <stdio.h>
#include <stdlib.h>

#define false 0
#define true !false
#define MAX 5
#define PAREDE 1
#define NUMFUI 0
#define JAFUI 2
#define initialx 0
#define initialy 1

int labyrinth[5][5]=
{
   {1, 0, 1, 1, 1}, //posx = 0
   {1, 0, 0, 0, 1}, //posx = 1
   {1, 1, 1, 0, 1}, //posx = 2
   {1, 0, 0, 0, 1}, //posx = 3
   {1, 0, 1, 1, 1}  //posx = 4
};
int dirx[5] = {0, 1, 0, -1};
int diry[5] = {-1, 0, 1, 0};

void printTabuleiro();
int walk(int posx, int posy);
int ehValido(int x, int y);

int main(int argc, char *argv[]) {
    int posx = initialx;
    int posy = initialy;

    walk(posx, posy);
    printTabuleiro();

    return 0;
}

int walk(int posx, int posy) {
    #ifdef DEBUG
    printf("new step\n");
    printTabuleiro();
    #endif
    if (posx == MAX-1){
        labyrinth[posx][posy] = JAFUI;
        return true;
    }

    int i;
    for (i=0; i<MAX; i++)
        if (ehValido(posx + dirx[i],
                    posy + diry[i])) {
            labyrinth[posx][posy] = JAFUI;
            posx += dirx[i];
            posy += diry[i];
            if (walk(posx, posy))
                return true;
        }

    return false;
}

int ehValido(int x, int y) {
    if (labyrinth[x][y] == NUMFUI)
        return true;
    return false;
}

void printTabuleiro() {
    int i, j;

    printf("\n");
    for (i=0; i<MAX; i++){
        for (j=0; j<MAX; j++)
            printf("%d ", labyrinth[i][j]);
        printf("\n");
    }
}
