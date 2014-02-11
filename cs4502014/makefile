allfiles: client server

client : client.o
	g++ client.o -o client

server : server.o
	g++ server.o -o server


client.o : client.cpp CS450Header.h
	g++ -Wall -c -g client.cpp

	
server.o : server.cpp CS450Header.h
	g++ -Wall -c -g server.cpp

clean:	
	rm -rf client client.o server server.o