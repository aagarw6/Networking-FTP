/*  server.cpp

    This program receives a file from a remote client and sends back an 
    acknowledgement after the entire file has been received.
    
    Two possible options for closing the connection:
        (1) Send a close command along with the acknowledgement and then close.
        (2) Hold the connection open waiting for additional files / commands.
        The "persistent" field in the header indicates the client's preference.
        
    Written by anshika agarwal January 2014 for CS 450
*/


#include <cstdlib>
#include <iostream>

#include "CS450Header.h"
#include "sys/socket.h"
#include "netinet/in.h"
#include "arpa/inet.h"
#include "netdb.h"
#include <ctime>
 #include <unistd.h> 
#include <fcntl.h>
    #include <stdlib.h>
    #include <stdio.h>
    #include <string.h>

using namespace std;

int main(int argc, char *argv[])
{
    int pnumber;
    int callsocket;
    struct sockaddr_in addr; 
    int accptrequest;
    struct sockaddr_in clientaddr;
    socklen_t alen;
    struct hostent *hp;
    CS450Header *header = (CS450Header *)malloc(sizeof (CS450Header));
    char *addR = NULL;
    int fd;
    char sysAddr [100] ;

    cout<< "CS 450  HW 1  author:anshika agarwal  accountid:aagarw6  UIN:652854536 \n";
    // User Input
    
    /* Check for the following from command-line args, or ask the user:
        
        Port number to listen to.  Default = 54321.
    */
    cout<< "Please enter the desired port number: ";
    cin>> pnumber;
     //return 0;
    //  Call SOCKET to create a listening socket
    callsocket = socket (AF_INET, SOCK_STREAM, 0);
    if( callsocket < 0)
        {
            perror ("does not create the socket");
            return 0;

        }
        cout<< "Socket called\n";
    //  Call BIND to bind the socket to a particular port number
    addr.sin_family = AF_INET;
    addr.sin_port = htons(pnumber);
    gethostname(sysAddr,100);
    cout<<sysAddr;

    hp = gethostbyname (sysAddr);
    if(!hp)
    {
        perror ("cannot find destination address");
        return 0;
    }
    memcpy ((void *)&addr.sin_addr, hp->h_addr_list[0], hp->h_length);
    if (bind(callsocket, (struct sockaddr *)&addr, sizeof(addr))<0)
        {
        perror("bind failed");
        return 0;
        }  
        cout<< "Socket bound to the port \n";    
    //  Call LISTEN to set the socket to listening for new connection requests.
    if(listen(callsocket , 7)< 0)
        {
            perror("listen failed");
            return 0;
        }

    // For HW1 only handle one connection at a time
    
    // Call ACCEPT to accept a new incoming connection request.
        if((accptrequest = accept (callsocket, (struct sockaddr *)&clientaddr, &alen)) < 0)
        {
            perror("accept failed");
            return 0;
        }
        cout<<"Connection established\n";
    // The result will be a new socket, which will be used for all further
    // communications with this client.
    
    // Call RECV to read in one CS450Header struct
    if(recv(accptrequest,header,sizeof(CS450Header),0)<0)
    {

        perror("Error in sending file");
    }
    // Then call RECV again to read in the bytes of the incoming file.
    // If "saveFile" is non-zero, save the file to disk under the name
    // "filename".  Otherwise just read in the bytes and discard them.
    // If the total # of bytes exceeds a limit, multiple RECVs are needed.
     addR = (char*) malloc (ntohl(header->nbytes));
     if(recv(accptrequest,addR,ntohl(header->nbytes),0)<0)
    {
        perror("Error in sending file");
    }

    if(ntohs(header->saveFile) != 0)
    {
        fd=creat(header->filename,S_IRWXU);
        if(fd<0)
            {
                perror("File does not open");
            }
            else
            {
               if(write(fd,addR,ntohl(header->nbytes))<0)
               {
                perror("cannot write the file");
               }
            }
    }
    
    // Send back an acknowledgement to the client, indicating the number of 
    // bytes received.  Other information may also be included.

    header->from_IP = 0;
    header->to_IP = 0;
    header->packetType = htonl(2);
    header->nbytes = htonl(strlen(addR));
    header->relayCommand = htonl(1);
    header->persistent = htonl(0);
    header->saveFile = htonl(0);
     if(send(accptrequest,header,sizeof(CS450Header),0)<0)
    {
        perror("Error in sending file");
    }
    else
    {
        cout<<"header sent\n";
    }

    
    // If "persistent" is zero, then include a close command in the header
    // for the acknowledgement and close the socket.  Go back to ACCEPT to 
    // handle additional requests.  Otherwise keep the connection open and
    // read in another Header for another incoming file from this client.
    
    
    //system("PAUSE");
    shutdown(callsocket, SHUT_RDWR);
    return EXIT_SUCCESS;
}
