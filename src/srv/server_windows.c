/*
*
*	XBee communication on windows;
*
*/

#include <stdio.h>
#include <windows.h>

int main(int argc, char **argv) {

  // handle usado pelo windows para porta serial
  HANDLE serial_port;

  // portas COM1 a COM9 no windows são reservadas do sistema e podem ser
  // declaradas como tal
  // a cima disso tem de ser declarado como "\\\\.\\COM#" - # sendo o número da
  // porta
  char *port_name = "COM4";

  serial_port = CreateFile(port_name,                    // nome da porta
                           GENERIC_READ | GENERIC_WRITE, // leitura e escrita
                           0,    // sem compartilhamento em portas seriais
                           NULL, // sem segurança, apenas para arquivos
                           OPEN_EXISTING, // abrindo uma porta existente apenas
                           0,     // sem overlapping, usado para multithreads
                           NULL); // nulo para comunicação, apenas para arquivos

  // checa se conseguiu abrir a porta
  if (serial_port == INVALID_HANDLE_VALUE) {
    printf("Error opening serial port\n");
    CloseHandle(serial_port);
    return 0;
  } else {
    printf("Opening serial port successful\n");
  }

  char s[1000];

  // lê a entrada
  printf("Entrada -> ");
  scanf(" %[^\n]s%*c", s);

  // envia para o xbee
  fprintf(serial_port, "%s\n", s);

  // recebe a resposta
  fscanf(serial_port, "%s%*c", s);

  // imprime o resultado
  printf("Resposta -> %s\n", s);

  // fecha a porta serial;
  CloseHandle(serial_port);

  return 0;
}
