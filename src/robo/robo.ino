/*
  Project Name: robo
  Authors:
  * Marino Hohenheim
  * Pedro Marques
  Version: 0.1.0
  Boards Supported:
  * Arduino UNO*
  Dependencies:
  Description:
  Descrição do projeto
*/

#include <string.h>

#define MEM 50
String msg;
int mem[MEM];

void setup(){
    Serial.begin(9600);
    for(int i=0; i<MEM; i++)
        mem[i]=-1;
        /*
        Seta as coisa reservada
        */
}

int parse_msg(String msg);

void loop(){

    if(Serial.available()){
        msg = String(Serial.readString());
        parse_msg(msg);
    }
}


int parse_msg(String msg){
    char c = msg[0];
    int p, value;

    switch(c){
        case 'G':
            p = atoi(&msg[1]) -1;
            Serial.print("R");//Response prefix
            Serial.println(mem[p]);
            break;
        case 'S':
            p = atoi(&msg[1]) -1;
            value = atoi(&msg[msg.indexOf('=')+1]);
            mem[p] = value;
            Serial.print("R");//Response prefix
            Serial.println(mem[p]);
            break;
        case 'M':
            Serial.println(&msg[1]);
            break;
        default:
        break;
    }
    return 0;
}
