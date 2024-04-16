//****************************************************************************
//                         REDES Y SISTEMAS DISTRIBUIDOS
//                      
//                     2º de grado de Ingeniería Informática
//                       
//              This class processes an FTP transaction.
// 
//****************************************************************************



#include <cstring>
#include <cstdarg>
#include <cstdio>
#include <cerrno>
#include <netdb.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <locale.h>
#include <langinfo.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/stat.h> 
#include <iostream>
#include <dirent.h>

#include "common.h"

#include "ClientConnection.h"




ClientConnection::ClientConnection(int s) {
  char buffer[MAX_BUFF];

  control_socket = s;
  // Check the Linux man pages to know what fdopen does.
  fd = fdopen(s, "a+");
  if (fd == NULL) {
	  std::cout << "Connection closed" << std::endl;

    fclose(fd);
    close(control_socket);
    ok = false;
    return ;
  }
    
  ok = true;
  data_socket = -1;
  parar = false;
};


ClientConnection::~ClientConnection() {
 	fclose(fd);
	close(control_socket); 
}

/*
  PORT a1, a2, a3, a4, p1, p2;
  
  fscanf(f, "%d.%d.%d.%d.%d.%d", &a1, &a2, &a3, &a4, &p1, &p2);

  address (entero de 32, se segmenta en 4 partes, donde se colocan los valores de a1, a2, a3, a4)
  Para colocarlos en su lugar:
  address = (a1 << 24) | (a2 << 16) | (a3 << 8) | a4; (o al revés, se prueba a ver si funciona)

  Con el puerto, ocurre lo mismo, pero solo se necesita un entero de 16 bits:
  port = (p1 << 8) | p2;

  fprintf(f, "200 PORT command successful.\n");

*/
int connect_TCP(uint32_t address,  uint16_t  port) {
  // Implement your code to define a socket here
  struct sockaddr_in sin;
  struct hostent *hent;
  int s;

  memset(&sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_port = htons(port);
  sin.sin_addr.s_addr = address;

  s = socket(AF_INET, SOCK_STREAM, 0);
  if (s < 0) {
    errexit("Can't create socket: %s\n", strerror(errno));
  }
  if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
    errexit("Can't connect to %s.%d: %s\n", address, port, strerror(errno));
  }
  
  return s; // You must return the socket descriptor.
}


void ClientConnection::stop() {
  close(data_socket);
  close(control_socket);
  parar = true;
}

    
#define COMMAND(cmd) strcmp(command, cmd)==0

// This method processes the requests.
// Here you should implement the actions related to the FTP commands.
// See the example for the USER command.
// If you think that you have to add other commands feel free to do so. You 
// are allowed to add auxiliary methods if necessary.

void ClientConnection::WaitForRequests() {
  if (!ok) return;
    
  fprintf(fd, "220 Service ready\n");
  
  while(!parar) {
    fscanf(fd, "%s", command);
    if (COMMAND("USER")) {
      fscanf(fd, "%s", arg);
      fprintf(fd, "331 User name ok, need password\n");
    }
    else if (COMMAND("PWD")) {

    }
    else if (COMMAND("PASS")) {
      fscanf(fd, "%s", arg);
      if(strcmp(arg,"1234") == 0){
        fprintf(fd, "230 User logged in\n");
      }
      else{
        fprintf(fd, "530 Not logged in.\n");
        parar = true;
      }
    }
    else if (COMMAND("PORT")) {
      int a1, a2, a3, a4, p1, p2;
      int fd = socket(AF_INET, SOCK_STREAM, 0);
      FILE *f = fdopen(fd, "a+");
      
      fscanf(f, "%d,%d,%d,%d,%d,%d", &a1, &a2, &a3, &a4, &p1, &p2);
      uint32_t address = (a1 << 24) | (a2 << 16) | (a3 << 8) | a4;
      uint16_t port = (p1 << 8) | p2;

      data_socket = connect_TCP(address, port);

      fprintf(f, "200 PORT command successful.\n");
    }
    else if (COMMAND("PASV")) {
    // To be implemented by students
    }
    else if (COMMAND("STOR") ) {
    // To be implemented by students
    }
    else if (COMMAND("RETR")) {
    // To be implemented by students
    }
    else if (COMMAND("LIST")) {
    // To be implemented by students	
    }
    else if (COMMAND("SYST")) {
      fprintf(fd, "215 UNIX Type: L8.\n");   
    }
    else if (COMMAND("TYPE")) {
      fscanf(fd, "%s", arg);
      fprintf(fd, "200 OK\n");   
    }
    else if (COMMAND("QUIT")) {
      fprintf(fd, "221 Service closing control connection. Logged out if appropriate.\n");
      close(data_socket);	
      parar=true;
      break;
    }
    else  {
      fprintf(fd, "502 Command not implemented.\n"); fflush(fd);
      printf("Comando : %s %s\n", command, arg);
      printf("Error interno del servidor\n");
    }
  }
  fclose(fd);

  return;
};
