#ifndef MYUT_H
#define MYUT_H


#include<stdio.h>
#include<iostream>
#include<string>
#include <cstdlib>
#include <sys/types.h>
#include<unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "myStructs.h"
using namespace std;

extern string commands;
extern int lport;
extern int sockfd;
extern int npacks;
extern int period;
extern int myID;
extern struct NTimer* timers;
extern struct RoutingT *RoutTable;
extern struct serverList *Server_List;
extern struct costTable *Cost_Table;
void insertInList(struct serverList stemp);
void insertInTable(struct costTable* stemp);
void insertInList(struct serverList* stemp)
{
	
	if(Server_List == NULL)
	{
		Server_List = (struct serverList *)malloc(sizeof (struct serverList));	
		Server_List->id = stemp->id;
		Server_List->port=stemp->port;
		Server_List->IP.s_addr= stemp->IP.s_addr;
		Server_List->next = NULL;
	}
	else
	{
		struct serverList *temp2= Server_List;
		struct serverList *prev=temp2;
		while(temp2 !=NULL)
		{
			prev=temp2;
			temp2 = temp2->next;
		}
		prev->next = (struct serverList *)malloc(sizeof (struct serverList));	
		prev->next->id = stemp->id;
		prev->next->port=stemp->port;
		prev->next->IP.s_addr=stemp->IP.s_addr;
		prev->next->next = NULL;
		//cout<<"\n Connected to : \n"<<inet_ntoa(temp2->peer.s_addr);
	}
}
void insertInTable(struct costTable* stemp)
{
	if(Cost_Table == NULL)
	{
		Cost_Table = (struct costTable *)malloc(sizeof (struct costTable));	
		Cost_Table->id1 = stemp->id1;
		Cost_Table->id2=stemp->id2;
		Cost_Table->cost= stemp->cost;
		Cost_Table->next = NULL;
	}
	else
	{
		struct costTable *temp2= Cost_Table;
		struct costTable *prev=temp2;
		while(temp2 !=NULL)
		{
			prev=temp2;
			temp2 = temp2->next;
		}
		prev->next = (struct costTable *)malloc(sizeof (struct costTable));	
		prev->next->id1 = stemp->id1;
		prev->next->id2=stemp->id2;
		prev->next->cost=stemp->cost;
		prev->next->next = NULL;
		//cout<<"\n Connected to : \n"<<inet_ntoa(temp2->peer.s_addr);
	}
}

#endif

