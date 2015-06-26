
#include <stdio.h>      /* for printf(), fprintf() */
#include <winsock.h>    /* for socket(),... */
#include <stdlib.h>
#include <string.h>  /* for exit() */
#include <string>
#include <cstring>
#include<sstream>
#include<iostream>
#include<iomanip>
#include <vector>
#include<conio.h>
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#define RCVBUFSIZE 5210   /* Size of receive buffer */

#define IAC 255
#define DO 253
#define WILL 251

#define WONT 0xfc
#define DONT 0xfe
#define CMD 0xff
#define CMD_ECHO 01
#define CMD_WINDOW_SIZE 31

using namespace std;
void DieWithError(char *errorMessage)
{
	fprintf(stderr, "%s: %d\n", errorMessage, WSAGetLastError());
	// exit(1);
}

string int_to_hex(int i)
{
	stringstream stream;
	stream << hex << i;
	if (stream.str().length() < 2)
	{
		return "0" + stream.str();
	}
	else
	{
		return stream.str();
	}

}

int receive(unsigned char *e, int s)
{
	//vector<string> Respond;
	int i, b;
	//printf("\n Received: \n");                /* Setup to print the echoed string */

	if ((b = recv(s,(char*) e, RCVBUFSIZE - 1, 0)) <= 0)
		DieWithError("recv() failed or connection closed prematurely");
	e[b] = '\0';
	printf("%s\n", e); //print response to the screen
	// to print hex
	/*for (int i = 0; i < b; i++)
	{
		Respond.push_back(int_to_hex((int)e[i]));
		cout << int_to_hex((int)e[i]) << " " ;
	}*/

	return 1;
}
void main(int argc, char *argv[])
{

	int sock;                        /* Socket descriptor */

	struct sockaddr_in telnetServAddr; /* telnet server address */
	unsigned short telnetServPort;     /* telnet server port */
	char *servIP;                    /* Server IP address (dotted quad) */
	char *telnetString;   /* String to send to ftp server */
	unsigned char telnetBuffer[RCVBUFSIZE];     /* Buffer for ftp string */
	char e[RCVBUFSIZE];
	int telnetStringLen;               /* Length of string to ftp */
	int bytesRcvd, totalBytesRcvd;   /* Bytes read in single recv() and total bytes read */
	WSADATA wsaData;                 /* Structure for WinSock setup communication */



	servIP =        "127.0.0.1"; // "172.16.42.76" ; //   "192.168.1.3";  // /* First arg: server IP address (dotted quad) */

	telnetServPort = 23;  /* otherwise, 7 is the well-known port for the ftp service */

	if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) /* Load Winsock 2.0 DLL */
	{
		fprintf(stderr, "WSAStartup() failed");
		exit(1);
	}

	/* Create a reliable, stream socket using TCP */
	if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
		DieWithError("socket() failed");

	/* Construct the server address structure */
	memset(&telnetServAddr, 0, sizeof(telnetServAddr));     /* Zero out structure */
	telnetServAddr.sin_family = AF_INET;             /* Internet address family */
	telnetServAddr.sin_addr.s_addr = inet_addr(servIP);   /* Server IP address */
	telnetServAddr.sin_port = htons(telnetServPort); /* Server port */


	cout<<"****************";

	/* Establish the connection to the ftp server */
	if (connect(sock, (struct sockaddr *) &telnetServAddr, sizeof(telnetServAddr)) < 0)
		DieWithError("connect() failed");
	receive(telnetBuffer, sock);
	

	// 1 )
	/* Send the string, including the null terminator, to the server */
	char buf[3] = {IAC , WILL , 37 };
	printf(" sent: \n %s", buf);
	if (send(sock, buf, 3, 0) == -1)
		perror("send");
	receive(telnetBuffer, sock);

	// 2 )
	/* Send the string, including the null terminator, to the server */
	char buf2[] = { IAC , DO , 1 ,
		IAC , DO , 3 ,
		IAC , WILL , 39 ,
		IAC , WILL , 31 ,
		IAC , WILL , 0 ,
		IAC , DO , 0,
		IAC , 250 , 37 , 0 , 15 , 0 , 0 , 
		IAC , 240 ,
		IAC , 250 , 37 , 0 , 15 , 0 , 2 ,
		IAC , 240 ,
		IAC , 250 , 39 , 0 , 
		IAC , 240 ,
	};
	printf(" sent: \n %s", buf2);
	if (send(sock, buf2, 42 , 0) == -1)
		perror("send");
	receive(telnetBuffer, sock);


	// -- login :
	char login[10] ;
	cin >> login ;
	if (send(sock, login, 10,  0) == -1)
		perror("send");
	receive(telnetBuffer, sock);

	if (send(sock, "\r", 1 ,  0) == -1)
		perror("send");
	receive(telnetBuffer, sock);

	// -- pass :
	char pass[10];
	cin>>pass;
	if (send(sock,pass, 10 ,  0) == -1)
		perror("send");
	//receive(telnetBuffer, sock);
	if (send(sock, "\r", 1 ,  0) == -1)
		perror("send");
	receive(telnetBuffer, sock);

	// 3 )
	char buflast[] = {
		IAC , 0xfc ,0x18
	};
	if (send(sock, buflast, 3 ,  0) == -1)
		perror("send");
	receive(telnetBuffer, sock);
	
	cin.ignore();   

	while(true)
	{
		
	string com ;
	getline(  cin , com ) ;
	com+="\r\n";
	if (send(sock, &com[0] , com.length() ,  0) == -1)
		perror("send");
	 receive(telnetBuffer, sock);
	 
	}

	closesocket(sock);
	WSACleanup();  /* Cleanup Winsock */
	//	getch();
	exit(0);

}