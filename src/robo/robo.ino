/*
  Project Name: robo
  Authors:
  * Marino Hohenheim
  * Pedro Marques
  Version: 0.2.0
  Boards Supported:
    * Arduino UNO
  Dependencies:
  * XBee-Arduino Library, 0.6.0
  * XCTU, 6.3.8 (usado externamente para monitorar comunicação entre xbee e arduino)
  Description:
  Descrição do projeto
*/

#include <SoftwareSerial.h>

#define MEM 50
#define PORTARX 2
#define PORTATX 3

String msg;
int mem[MEM];

SoftwareSerial XBee(PORTARX, PORTATX);

void setup(){
    XBee.begin(9600);
    Serial.begin(9600);
    for(int i=0; i<MEM; i++)
        mem[i]=-1;
        /*
        Seta as coisa reservada
        */
    delay(500);
    if(XBee.isListening()) {
      Serial.println("XBee está conectado!");
            XBee.println("XBee está conectado!");
    }
}

int parse_msg(String msg);
int parse_msg_xbee(String msg);

void loop(){

    //Se tiver data sendo mandada pelo serial monitor
    if(Serial.available()){
        msg = String(Serial.readString());
        parse_msg(msg);
    }

    //Se tiver data vindo do XBee
    if(XBee.available()) {
        msg = String(XBee.readString());
        Serial.print("eu li: ");
        Serial.println(msg);
        parse_msg_xbee(msg);
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
            Serial.print("could not handle ");
            Serial.println(msg);
        break;
    }
    return 0;
}

int parse_msg_xbee(String msg){
    char c = msg[0];
    int p, value;

    switch(c){
        case 'G':
            p = atoi(&msg[1]) -1;
            XBee.print("R");//Response prefix
            XBee.println(mem[p]);
            break;
        case 'S':
            p = atoi(&msg[1]) -1;
            value = atoi(&msg[msg.indexOf('=')+1]);
            mem[p] = value;
            XBee.print("R");//Response prefix
            XBee.println(mem[p]);
            break;
        case 'M':
            XBee.println(&msg[1]);
            break;
        default:
        XBee.print("could not handle ");
        XBee.println(msg);
        break;
    }
    return 0;
}
