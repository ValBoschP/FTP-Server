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

    // ========== COMMAND USER ==========
    if (COMMAND("USER")) {
      fscanf(fd, "%s", arg);
      fprintf(fd, "331 User name ok, need password\n");
    }
    // ========== COMMAND PWD ==========
    else if (COMMAND("PWD")) {

    }
    // ========== COMMAND PASS ==========
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
    // ========== COMMAND PORT ==========
    else if (COMMAND("PORT")) {
      int a1, a2, a3, a4, p1, p2;
      
      fscanf(fd, "%d,%d,%d,%d,%d,%d", &a1, &a2, &a3, &a4, &p1, &p2);
      uint32_t address = (a4 << 24) | (a3 << 16) | (a2 << 8) | a1;
      uint16_t port = (p1 << 8) | p2;
      std::cout << a1 << " " << a2 << " " << a3 << " " << a4 << " " << p1 << " " << p2 << std::endl;
      std::cout << "Address: " << address << " Port: " << port << std::endl;
      data_socket = connect_TCP(address, port);
      fprintf(fd, "200 PORT command successful.\n");
    }
    // ========== COMMAND PASV ==========
    else if (COMMAND("PASV")) {
    // To be implemented by students
    }
    // ========== COMMAND STOR ==========
    else if (COMMAND("STOR") ) {
    // To be implemented by students
      fscanf(fd, "%s", arg);
      FILE *f = fopen(arg, "w");
      char buffer[MAX_BUFF];
      do {
        fwrite(buffer, sizeof buffer[0], MAX_BUFF, f);
      } while (read(data_socket, buffer, MAX_BUFF) > 0);
      fprintf(fd, "226 Closing data connection. Requested file action successful.\n");
      fclose(f);
      close(data_socket);
    }
    // ========== COMMAND RETR ==========
    else if (COMMAND("RETR")) {
      fscanf(fd, "%s", arg);
      FILE *f = fopen(arg, "r");
      if (f == NULL) {
        fprintf(fd, "550 File not found.\n");
        break;
      }
      else {
        fprintf(fd, "150 File status okay; about to open data connection.\n");
        char buffer[MAX_BUFF];
        int s;
        while((s = fread(buffer, 1, MAX_BUFF, f)) > 0){
          write(data_socket, buffer, s);
        }
        fclose(f);
        close(data_socket);
        fprintf(fd, "226 Closing data connection. Requested file action successful.\n");
      }
    }
    // ========== COMMAND LIST ==========
    else if (COMMAND("LIST")) {
    // To be implemented by students
      fprintf(fd, "150 File status okay; about to open data connection.\n");
      DIR *d = opendir(".");
      struct dirent *dir;
      while ((dir = readdir(d)) != NULL) {
        fprintf(fd, "%s\n", dir->d_name);
      }
      fprintf(fd, "226 Closing data connection. Requested file action successful.\n");
      closedir(d);
      close(data_socket);
    }
    // ========== COMMAND SYST ==========
    else if (COMMAND("SYST")) {
      fprintf(fd, "215 UNIX Type: L8.\n");   
    }
    // ========== COMMAND TYPE ==========
    else if (COMMAND("TYPE")) {
      fscanf(fd, "%s", arg);
      fprintf(fd, "200 OK\n");   
    }
    // ========== COMMAND MDTM ==========
    else if (COMMAND("MDTM")) {
      fscanf(fd, "%s", arg);
      struct stat st;
      if (stat(arg, &st) == 0) {
        fprintf(fd, "213 %ld\n", st.st_mtime);
      }
      else {
        fprintf(fd, "550 File not found.\n");
      }
    }
    // ========== COMMAND FEAT ==========
    else if (COMMAND("FEAT")) {
      fprintf(fd, "502 Command not implemented.\n");   
    }
    // ========== COMMAND SIZE ==========
    else if (COMMAND("SIZE")) {
      fscanf(fd, "%s", arg);
      struct stat st;
      if (stat(arg, &st) == 0) {
        fprintf(fd, "213 %ld bytes\n", st.st_size);
      }
      else {
        fprintf(fd, "550 File not found.\n");
      }
    }
    // ========== COMMAND QUIT ==========
    else if (COMMAND("QUIT")) {
      fprintf(fd, "221 Service closing control connection. Logged out if appropriate.\n");
      close(data_socket);	
      parar=true;
      break;
    }
    else {
      fprintf(fd, "502 Command not implemented.\n"); fflush(fd);
      printf("Comando : %s %s\n", command, arg);
      printf("Error interno del servidor\n");
    }
  }
  fclose(fd);

  return;
};
