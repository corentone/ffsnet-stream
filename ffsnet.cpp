/* Author:  Corentin Debains
 * Email:   cdebains@iit.edu
 *
 *
 */

#include <iostream>
#include <udt.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <malloc.h>
#include <string.h>
#include "ffsnetCPP.h"

int failure = -1;

int * Transfer_init(UDTArray Ssocks, struct metadata * meta, int operation){

		/* use this function to initialize the UDT library */
		UDT::startup();

		/* allocate the struct and create the sockets */
		Ssocks = (struct UDTArray_s *) malloc(sizeof(struct UDTArray_s));
		
		int socksNumber = meta->k + meta->m;
		int socksreqNumber;
		switch(operation){
			case CLIENT_SENDBUF:
				socksreqNumber = meta->k + meta->m;
				break;
			case CLIENT_RECVBUF:
				socksreqNumber = meta->k;
				break;
			default:
				socksreqNumber = meta->k + meta->m;
				break;
		}
		
		int * indexArray = (int *) malloc(sizeof(int)*socksreqNumber);
		int curIndex = 0;
		
		/* Calculate buffer numbers */
		int buffernumbers = meta->fileSize / meta->bufsize;
		if(meta->fileSize % meta->bufsize != 0) buffernumbers++; 
		

		/* create sockets */
		struct addrinfo hints, *peer;
		bool block = false;

		memset(&hints, 0, sizeof(struct addrinfo));
		hints.ai_flags = AI_PASSIVE;
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;

		UDTSOCKET fhandle;
		struct comTransfer * current = meta->loc->transfers;
		
		int i;
		for(i=0; i < socksNumber; i++){
			fhandle = UDT::socket(hints.ai_family, hints.ai_socktype, hints.ai_protocol);
			Ssocks->socks[i] = fhandle;
			
			char port_str[10];
			sprintf(port_str, "%d", current->port);
			
			if (0 != getaddrinfo(current->hostName, port_str, &hints, &peer)) {
				cerr << "incorrect server/peer address. " << current->hostName << ":" << port_str << endl;
				return &failure;
			}
			
			if(i < socksreqNumber){
				if (UDT::ERROR == UDT::connect(fhandle, peer->ai_addr, peer->ai_addrlen)) {
					cerr << "connect: " << UDT::getlasterror().getErrorMessage() << endl;
					if(socksreqNumber < socksNumber-1) socksreqNumber++;
					else return &failure;
				}
				else{
					// Add the socket to the index (this socket is open and available)
					indexArray[curIndex++]=socksNumber;
					
					// send the request type
					int opeAddress = operation;
					if (UDT::ERROR == UDT::send(fhandle, (char*)&operation, sizeof(int), 0))	{
						cout << "opeSend: " << UDT::getlasterror().getErrorMessage() << endl;
						return &failure;
					}
					// send the filename length
					int len = strlen(current->distantChunkName);
					if (UDT::ERROR == UDT::send(fhandle, (char*)&len, sizeof(int), 0)) {
						cout << "filename len Send: " << UDT::getlasterror().getErrorMessage() << endl;
						return &failure;
					}
					// send the filename
					if (UDT::ERROR == UDT::send(fhandle, current->distantChunkName, len, 0)) {
						cout << "filename send: " << UDT::getlasterror().getErrorMessage() << endl;
						return &failure;
					}
					
					switch(operation){
						case CLIENT_SENDBUF:{
							/* send buffersize information */
							char int_str[10];
							sprintf(int_str, "%d", meta->bufsize);
							const char * bufsize_str = int_str;
							if (UDT::ERROR == UDT::send(fhandle, bufsize_str, sizeof(int), 0)) {
								cout << "buffersize send: " << UDT::getlasterror().getErrorMessage() << endl;
								return &failure;
							}
							/* send buffernumbers information */
							sprintf(int_str, "%d", buffernumbers);
							const char * bufnumbers_str = int_str;;
							if (UDT::ERROR == UDT::send(fhandle, bufnumbers_str, sizeof(int), 0)) {
								cout << "buffernumbers send: " << UDT::getlasterror().getErrorMessage() << endl;
								return &failure;
							}
							UDT::setsockopt(fhandle, 0, UDT_SNDSYN, &block, sizeof(bool)); 
							break;
							}
						case CLIENT_RECVBUF:
							break;
						default:
							break;
					
					}
				}
			}
			current=current->next;
		}
		
		freeaddrinfo(peer);
		
		
		Ssocks->indexArray = indexArray;
		return indexArray;
		

		
}
int bufferSend(UDTArray Ssocks, int index, char * buffer){
	
	if (UDT::ERROR == UDT::send(Ssocks->socks[index], buffer, Ssocks->meta->bufsize, 0)) {
		cout << "Send: " << UDT::getlasterror().getErrorMessage() << endl;
		return 1;
	}

	return 0;
}
int bufferRecv(UDTArray Ssocks, int index, char * buffer){
	
	if (UDT::ERROR == UDT::recv(Ssocks->socks[index], buffer, Ssocks->meta->bufsize, 0)) {
		cout << "Receive: " << UDT::getlasterror().getErrorMessage() << endl;
		return 1;
	}

	return 0;

}

int Transfer_destroy(UDTArray Ssocks){
	int i;
	for(i=0; i < Ssocks->socks.size(); i++){
		UDT::close(Ssocks->socks[i]);
	}
	
	free(Ssocks->indexArray);
	free(Ssocks);
	
	/* use this function to release the UDT library */
	UDT::cleanup();	
	
	return 0;
}
