# Simple FTP Server

## Introduction
This **Simple FTP Server** is a simple implementation of a **File Transfer Protocol (FTP)** server, allowing users to upload and download files from a designated folder. It is built upon the concepts of socket programming and aims to provide a practical understanding of **client-server communication**.

## FTP (File Transfer Protocol)
The **File Transfer Protocol** facilitates the transfer of files between a client and a server using **TCP** connections. It defines commands and modes for managing file transfers, including active and passive modes, as well as commands like PORT, PASV, STOR, RETR, and LIST.

## FTP Server

### Code template
The **Simple FTP Server**  provides a code template on ***src*** directory consisting of several files:

- **common.h:** Common functions
- **ftp_server.cpp:** Main program
- **FTPServer.h:** C++ class definition of the FTP server
- **FTPServer.cpp:** C++ class implementation of the FTP server
- **ClientConnection.h:** C++ class definition of the ClientConnection class
- **ClientConnection.cpp:** Implementation of the ClientConnection class
- **Makefile:** Makefile for compilation

### How to compile and run the server
Before compiling make sure that you have the following packages installed
on your system:
- **gcc**
- **g++**
- **make**

To compile the code use the make command:
```bash
make all
```
To run the server use:
```bash
./ftp_server
```

### Report
To complement the project, a PDF document will be provided serving as a project report. This report will cover the following topics:

- **Description of the Developed Application**

- **Description of the Developed Protocol**

- **Guide for Compilation and Execution of the Source Code**

- **Test Cases**

- **Source Code**