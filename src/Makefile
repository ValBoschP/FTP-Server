
all: ftp_server


ftp_server: ClientConnection.cpp FTPServer.cpp ftp_server.cpp
	g++ -g -std=gnu++0x  ClientConnection.cpp FTPServer.cpp ftp_server.cpp -o ftp_server -lpthread
	
git:
	git add .
	git commit -m "update"
	git push

clean:
	$(RM) ftp_server *~
