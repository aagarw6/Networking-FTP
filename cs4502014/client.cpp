/* client.cpp

    This program transfers a file to a remote server, possibly through an
    intermediary relay.  The server will respond with an acknowledgement.
    
    This program then calculates the round-trip time from the start of
    transmission to receipt of the acknowledgement, and reports that along
    with the average overall transmission rate.
    
    Written by anshika agarwal for CS 450 HW1 January 2014
*/

#include <cstdlib>
#include <iostream>
    #include <stdlib.h>
    #include <stdio.h>

#include "CS450Header.h"
#include "fcntl.h"
#include "sys/stat.h"
#include "sys/mman.h"
#include "sys/socket.h"
#include "netinet/in.h"
#include "arpa/inet.h"
#include <sys/types.h>
#include <netdb.h>
#include <ctime>
#include <string.h>

using namespace std;

int main(int argc, char *argv[])
{
    // User Input
    string destination_ip;
    int pnumber;
    string relay_ip;
    int rpnumber;
    string filename;
    int fd;
    void  *addr=NULL;
    struct stat info;
    int callsocket;
    struct sockaddr_in address;
    struct hostent *hp;
    time_t start,end;
    double diff;
    CS450Header *header = (CS450Header *)malloc(sizeof (CS450Header));

    cout<< "CS 450  HW 1  author:anshika agarwal  accountid:aagarw6  UIN:652854536 \n";



    /* Check for the following from command-line args, or ask the user:

        
        Destination ( server ) name and port number
        Relay name and port number.  If none, communicate directly with server
        File to transfer.  Use OPEN(2) to verify that the file can be opened
        for reading, and ask again if necessary.
    */
    cout<<"Please enter the destination IP :";
    cin>>destination_ip;
    cout<<"Please enter the port number :";
    cin>>pnumber;
    cout<<"Please enter the relay IP, enter none for none :";
    cin>>relay_ip;
    cout<<"Please enter the relay port number, enter -1 for none :";
    cin>>rpnumber;
    cout<<"Please enter the filename :";
    cin>>filename;
    while ((fd=open(filename.c_str(),O_RDONLY))<0)
    {
        perror("file not found");
        cout<<"Enter the filename again :";
        cin>>filename;
    }
    cout<<"file found \n";
    // Use FSTAT and MMAP to map the file to a memory buffer.  That will let the


    // virtual memory system page it in as needed instead of reading it byte
    // by byte.

     if (fstat(fd, &info) <0)
    {
        perror (" fstat() error");
    }

    addr = mmap(NULL, info.st_size, PROT_READ | PROT_WRITE , MAP_PRIVATE, fd, 0);
    if (addr==MAP_FAILED)
    {
        perror("Cannot map file to memory");
    }
    // Open a Connection to the server ( or relay )  TCP for the first HW
    // call SOCKET and CONNECT and verify that the connection opened.
       callsocket = socket (AF_INET, SOCK_STREAM, 0);
    if( callsocket < 0)
        {
            perror ("does not create the socket");
            return 0;

        }
        cout<< "Socket called\n";
    hp = gethostbyname (destination_ip.c_str());
    if(!hp)
    {
        perror ("cannot find destination address");
        return 0;
    }
    address.sin_family = AF_INET;
    memcpy ((void *)&address.sin_addr, hp->h_addr_list[0], hp->h_length);
    address.sin_port = htons(pnumber);
    if (connect(callsocket, (struct sockaddr *)&address, sizeof(address))<0)
        {
        perror("bind failed");
        return 0;
        }  
        cout<< "Socket bound to the port \n"; 

    
    // Note the time before beginning transmission
        start = time (0);
    
    // Create a CS450Header object, fill in the fields, and use SEND to write
    // it to the socket.
        header->version = htonl(2);
        header->UIN = htonl(685452536);
        header->HW_number=htonl(1); 
        header->transactionNumber = htonl(1);
        strcpy(header->ACCC,"aagarw6");
        strcpy(header->filename,filename.c_str());
        header->from_IP =0;
        header->to_IP =0;
        header->packetType = htonl(1);
        header->nbytes = htonl(info.st_size);
        header->relayCommand = htonl(0);
        header->persistent = htonl(0);
        header->saveFile = htonl(1);
        
    // Use SEND to send the data file.  If it is too big to send in one gulp
    // Then multiple SEND commands may be necessary.
    if(send(callsocket,header,sizeof(CS450Header),0)<0)
    {
        perror("Error in sending file");
    }
     if(send(callsocket,addr,info.st_size,0)<0)
    {
        perror("Error in sending file");
    }
    
    // Use RECV to read in a CS450Header from the server, which should contain
    // some acknowledgement information.  
    if(recv(callsocket,header,sizeof(CS450Header),0)<0)
    {

        perror("Error in recieving header");
    }
    else
    {
       cout<<"Header received \n"; 
    }
       

    // Calculate the round-trip time and
    // bandwidth, and report them to the screen along with the size of the file
    // and output echo of all user input data.
      
    // When the job is done, either the client or the server must transmit a
    // CS450Header containing the command code to close the connection, and 
    // having transmitted it, close their end of the socket.  If a relay is 
    // involved, it will transmit the close command to the other side before
    // closing its connections.  Whichever process receives the close command
    // should close their end and also all open data files at that time.
    
    // If it is desired to transmit another file, possibly using a different
    // destination, protocol, relay, and/or other parameters, get the
    // necessary input from the user and try again.
    
    // When done, report overall statistics and exit.
    
    //system("PAUSE");

    end = time (0);
    diff = difftime(end,start);

    cout<<" Overall Statistics \n";
    cout<<" number of bytes recieved by server:\t"<<ntohl(header->nbytes);
    cout<<" \n number of bytes sent by client:\t"<<info.st_size;
    cout<<"\n the round-trip time is:\t"<<diff<<"\n";

        
    shutdown(callsocket,SHUT_RDWR);
    return EXIT_SUCCESS;
}
