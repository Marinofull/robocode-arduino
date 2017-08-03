/*
*
*	XBee communication on windows;
*
*/

#include <Windows.h>
#include <stdio.h>

int main(int argc, char **argv) {

  // handle usado pelo windows para porta serial
  HANDLE serial_port;
  BOOL Status;
  DWORD dwEventMask;
  char TempChar;
  char SerialBuffer[256];
  DWORD NoBytesRead;
  int i = 0;

  // portas COM1 a COM9 no windows são reservadas do sistema e podem ser
  // declaradas como tal
  // a cima disso tem de ser declarado como "\\\\.\\COM#" - # sendo o número da
  // porta
  char port_name[] = "COM3";

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

  DCB dcbSerialParams = {0};
  dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

  Status = GetCommState(serial_port, &dcbSerialParams);

  if (Status == FALSE) {
    printf("\n Erro em GetCommState()");
  }

  dcbSerialParams.BaudRate = CBR_9600;
  dcbSerialParams.ByteSize = 8;
  dcbSerialParams.StopBits = ONESTOPBIT;
  dcbSerialParams.Parity = NOPARITY;

  Status = SetCommState(serial_port, &dcbSerialParams);
  if (Status == FALSE) {
    printf("\n Erro setando a estrutura DCB");
  }

  COMMTIMEOUTS timeouts = {0};
  timeouts.ReadIntervalTimeout = 50; // todos em millisegundos
  timeouts.ReadTotalTimeoutConstant = 50;
  timeouts.ReadTotalTimeoutMultiplier = 10;
  timeouts.WriteTotalTimeoutConstant = 50;
  timeouts.WriteTotalTimeoutMultiplier = 10;

  Status = SetCommTimeouts(serial_port, &timeouts);
  if (Status == FALSE) {
    printf("\n Erro setando timeouts");
  }

  char s[1000];

  // lê a entrada
  printf("Entrada -> ");
  scanf(" %[^\n]s%*c", s);

  DWORD dNoOfBytestoWrite;
  DWORD dNoOfBytesWritten = 0;

  dNoOfBytestoWrite = sizeof(s);

  // envia para o xbee
  Status =
      WriteFile(serial_port, s, dNoOfBytestoWrite, &dNoOfBytesWritten, NULL);
  // fprintf(serial_port, "%s\n", s);

  if (Status == TRUE) {
    printf("\n\n   %s - Written to %s", s, port_name);
  } else {
    printf("\n\n   Error %d in writting to serial port", GetLastError());
  }

  // recebe a resposta
  Status = SetCommMask(serial_port, EV_RXCHAR);
  if (Status == FALSE) {
    printf("\n  Erro setando comm mask");
  }

  printf("\n\n Waiting for Data Reception");

  Status = WaitCommEvent(serial_port, &dwEventMask, NULL);
  if (Status == FALSE) {
    printf("\n  Erro setando WaitCommEvent");
  } else {
    printf("\n\n Reading");
    do {
      Status = ReadFile(serial_port, &TempChar, sizeof(TempChar), &NoBytesRead,
                        NULL);
      SerialBuffer[i] = TempChar;
      i++;
    } while (NoBytesRead > 0);

    // imprime o resultado
    printf("\n\n");
    for (int j = 0; j < i - 1; j++) {
      printf("%c", SerialBuffer[j]);
    }
  }
  // fscanf(serial_port, "%s%*c", s);

  // imprime o resultado
  // printf("Resposta -> %s\n", s);

  // fecha a porta serial;
  CloseHandle(serial_port);

  return 0;
}
