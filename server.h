#ifndef SERVER_H
#define SERVER_H

#include<stdio.h>
#include<iostream>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include <ctime>
#include<netdb.h>

#include "commands.h"

//void remS_IP_L(int fd);

void server(int port)
{
	char portNo[10];
	sprintf(portNo, "%d", port);	
	int bytes_recieved;
	fd_set master;
	fd_set read_fds;
	int fdmax;
	int newfd;
	struct addrinfo hints, *ai, *p;
	// newly accept()ed socket descriptor
	struct sockaddr_storage remoteaddr; // client address
	socklen_t addrlen;
	char buf[1024];
	int nbytes;
	// buffer for client data
	char remoteIP[INET6_ADDRSTRLEN];
	int yes=1;
	struct sockaddr_in server_address;
	socklen_t address_length;
	address_length = sizeof(server_address);
	int i, j, rv;
	// for setsockopt() SO_REUSEADDR, below
	FD_ZERO(&master);
	FD_ZERO(&read_fds);
	// clear the master and temp sets
	// get us a socket and bind it
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_protocol = IPPROTO_UDP;
	//hints.ai_flags = AI_PASSIVE;
	
	if ((rv = getaddrinfo(NULL, portNo, &hints, &ai)) != 0) 
	{
		fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
		exit(1);
	}
	for(p = ai; p != NULL; p = p->ai_next) 
	{
		sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (sockfd < 0) 
		{
			continue;
		}
		// lose the pesky "address already in use" error message
		setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
		if (bind(sockfd, p->ai_addr, p->ai_addrlen) < 0) 
		{
			close(sockfd);
			continue;
		}
			struct sockaddr_in *tad = (struct sockaddr_in*)p->ai_addr;
//			cout<<inet_ntoa(tad->sin_addr);
		break;
	}
	// if we got here, it means we didn't get bound
	if (p == NULL) 
	{
		fprintf(stderr, "selectserver: failed to bind\n");
		exit(2);
	}
	freeaddrinfo(ai); // all done with this
		// add the listener to the master set
	FD_SET(sockfd, &master);
	FD_SET(0,&master);
	initRTable();
	// keep track of the biggest file descriptor
	fdmax = sockfd; // so far, it's this one
	clock_t startTime = clock();
	initTimers();
	cout<<commands<<endl;
	int timecnt=-1;
	// main loop
	while(1) 
	{
	//	printf("\n Select routine");
		read_fds = master; // copy it
	//heckTimers();
		struct timeval tv;
		tv.tv_sec = 0;
		tv.tv_usec = 500000;
		timecnt++;	
		//cout<<"\n"<<(startTime - clock())/ CLOCKS_PER_SEC<<endl;
                if(timecnt== period*2)
                {
                //	cout<<"\n Period done :"<<endl;
                	updateTimers();
			checkTimers();
                	step();
               		timecnt =0;
                }

		if (select(fdmax+1, &read_fds, NULL, NULL, &tv) == -1 ) 
		{
			perror("select");
			exit(4);
		}		
		// run through the existing connections looking for data to read
		for(i = 0; i <= fdmax; i++) 
		{
		/*	checkTimers();
			cout<<"\n"<<(startTime - clock())/ CLOCKS_PER_SEC<<endl;
			if(((startTime - clock())/ CLOCKS_PER_SEC) == period)
			{
				cout<<"\n Period done :"<<endl;
				step();
				startTime = clock();
			}*/
			if (FD_ISSET(i, &read_fds)) 
			{ // we got one!!
				
				if (i == sockfd)
				{
				// get data sent by other server
					
					char tb[88];
					bytes_recieved = recvfrom(sockfd,&tb, 88,0,(struct sockaddr *)&server_address,&address_length);
					//cout<<"\n Error"<<errno<<endl;
					uint16_t sid = getID(&server_address);
				//	cout<<"\n R from :"<<inet_ntoa(server_address.sin_addr)<<" len "<<address_length<<endl;
				//	cout<<"\n Data received :"<<bytes_recieved<<" From :"<<sid<<endl;
					if(isNeighbor(sid))
					{
					//	cout<<"\n Is Neighbor"<<endl;
						if(bytes_recieved == sizeof(struct SendCost))
						{
							struct SendCost msg1;
							struct SendCost* msg = &msg1;
							memcpy(msg,&tb,sizeof(struct SendCost));
							updateCost(msg->myid, msg->id,msg->cost);
						//	cout<<"\n Small msg"<<endl;
						}
						else if(bytes_recieved == sizeof(struct MsgFormat))
						{
							struct MsgFormat msg1;	
							struct MsgFormat* msg= &msg1;
							memcpy(msg,&tb,sizeof(struct MsgFormat));
							npacks++;
							resetTimer(sid);
							updateRTable(sid,msg->no,msg->data);
							//cout<<"\n Big msg"<<endl;
						}
					}

				} 
			
				else if(i==0)
				{
				
					fgets(buf, 1024,stdin);
				        int len = strlen(buf) - 1;
				        if (buf[len] == '\n' )
					{
				            buf[len] = '\0';
					}
                                	int ind=0;
					
					while(buf[ind]!='\0')
					{
						if(buf[ind]==' ')
						{
							break;		
						}
						ind++;
					}
					char comnd[ind+1];
					char vals[len-ind+1];
					bool flag =true;
					if(buf[ind]=='\0')
					{
						strcpy(comnd,buf);
					}
					else
					{
						strncpy(comnd,buf,ind);
						int index=ind+1,vi=0;
						while(buf[index]!='\0')
						{
							vals[vi]=buf[index];			
							index++;
							vi++;
						}
						comnd[ind]= '\0';	
						vals[vi]='\0';
						flag=false;
						
					}
					if(flag)
					{
						if(strcmp(comnd,"step")==0)
						{
							step();
						}
						else if(strcmp(comnd,"packets")==0)
						{
							packets();
						}
						else if(strcmp(comnd,"display")==0)
						{
							display();
						}
						else if(strcmp(comnd,"crash")==0)
                                                {
							crash();
                                                }	
						else
						{
							printf("\n Wrong command \n");
						}	
					}
					else
					{
						if(strcmp(comnd,"update")==0)
                                                {
							update(vals);
                                                }
                                                else if(strcmp(comnd,"disable")==0)
                                                {
							disable(vals);
                                                }
                                                else
                                                {
                                                        printf("\n Wrong command \n");
                                                } 		
					}
				}	
					// handle data from a client
				
				else 
				{
					cout<<"\n Data received by server:"<<buf<<"\n";
					
				}	
				FD_CLR(i,&read_fds);
			} // END handle data from client

		} // END looping through file descriptors
	} // END for(;;)--and you thought it would never end!

}




#endif
