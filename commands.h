#ifndef COMND_H
#define COMND_H

#include<stdio.h>
#include<iostream>
#include<string>
#include <cstdlib>
#include<unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "myUtil.h"
#include <errno.h>
string commands;
struct serverList *Server_List;
struct costTable *Cost_Table;
struct RoutingT *RoutTable;
int myID;
int lport;
struct NTimer* timers;
int npacks;
int sockfd;
int period;

uint32_t getIP4mId(uint16_t id);
uint32_t getPort4mId(uint16_t id);
int getCost(int id);
void resetTimer(int id);
void updateTimers();
void checkTimers();
void initTimers();
void initRTable();
int getCost(int id,int myID1,int cnt, struct MsgData RTemp[]);
void updateLinkCostInRT(int id1,int id2,int ecost);
void deletNeighbor(int index);
int getID(struct sockaddr_in* servr);
void updateRTable(int sid, int cnt,struct MsgData RTemp[]);
void display();
void step();
bool isNeighbor(int id);
void packets();
void crash();
void sendUpdate2S(uint16_t id2,int cost);
void updateCost(uint16_t sid1,uint16_t sid2,int ecost);
void update(char *vals);
void disable(char *vals);


uint32_t getIP4mId(uint16_t id)
{
	if(Server_List!=NULL){
		struct serverList* temp2= Server_List;
		while(temp2 !=NULL)
		{
			if(id == temp2->id)
			{
				return temp2->IP.s_addr;
			}
			temp2 = temp2->next;
		}
	}
}
uint32_t getPort4mId(uint16_t id)
{
	if(Server_List!=NULL){
		struct serverList* temp2= Server_List;
		while(temp2 !=NULL)
		{
			if(id == temp2->id)
			{
				return temp2->port;
			}
			temp2 = temp2->next;
		}
	}
}
int getCost(int id)
{
	if(id == myID)
		return 0;
	if(Cost_Table!=NULL)
	{
		struct costTable* temp2= Cost_Table;
		while(temp2 !=NULL)
		{
			if((myID==temp2->id1 && id==temp2->id2) || (id==temp2->id1 && myID==temp2->id2))
			{
				return temp2->cost;
								
			}
			temp2 = temp2->next;
		}
	}
	return -1;
}
void resetTimer(int id)
{
	if(timers!=NULL)
	{
		struct NTimer* temp2 = timers;
		while(temp2 !=NULL)
		{
			if(temp2->id == id)
			{
				temp2->strtTime = 0;
				return;			
			}
			temp2 = temp2->next;
		}
	}
}
void updateTimers()
{
	struct NTimer* temp2 = timers;
        

        while(temp2!=NULL)
        {
		temp2->strtTime++;
                temp2 = temp2->next;
                

        }
        if(temp2==NULL)
        {
                return;
        }



}
void checkTimers()
{
	struct NTimer* temp2 = timers;
	struct NTimer* prev = timers;
	
	while(temp2!=NULL)
	{
		if(temp2->strtTime > 3)
		{
			deletNeighbor(temp2->id);
			if(temp2 == timers)
			{
				timers=temp2->next;
				temp2 = temp2->next;
				prev = temp2;
			}
			else
			{
				prev->next = temp2->next;
				temp2 = temp2->next;
			}
			
		}
		else
		{
			prev = temp2;
			temp2 = temp2->next;
		}
		
	}
	if(temp2==NULL)
	{
		return;
	}
}
void initTimers()
{
	struct costTable* temp2= Cost_Table;
	struct NTimer* ttimer = timers;
 
	while(temp2 !=NULL)
	{
		
		if(timers==NULL)
		{
			ttimer = (struct NTimer*) malloc(sizeof(struct NTimer));
			timers = ttimer;
		}
		else
		{
			ttimer->next = (struct NTimer*) malloc(sizeof(struct NTimer));
			ttimer = ttimer->next;
		}
		if(temp2->id1 != myID)
			ttimer-> id = temp2->id1;
		else
			ttimer-> id = temp2->id2;
		ttimer-> strtTime = 0;
		ttimer-> next = NULL;
		temp2 = temp2->next;
		
	}



}
void initRTable()
{
	struct serverList* temp2= Server_List;
	struct RoutingT* tRout = RoutTable;
	
	while(temp2 !=NULL)
	{
		
		if(RoutTable==NULL)
		{
			tRout = (struct RoutingT*) malloc(sizeof(struct RoutingT));
			RoutTable = tRout;
		}
		else
		{
			tRout->next = (struct RoutingT*) malloc(sizeof(struct RoutingT));
			tRout = tRout->next;
		}
		tRout-> destid = temp2->id;
		tRout-> nextHop = temp2->id;
		tRout-> cost = getCost(temp2->id);
		tRout->mod = true;
		tRout-> next = NULL;
		temp2 = temp2->next;
		
	}
}
int getCost(int id,int myID1,int cnt, struct MsgData RTemp[])
{
	if(id == myID1)
		return 0;
	int i=0;
	while(i<cnt)
	{
			if(RTemp[i].id == id)
			{
				return RTemp[i].cost;
								
			}
		i++;
	}		
	return -1;
}
void updateLinkCostInRT(int id1,int id2,int ecost)
{
	int id;
	if(myID == id1)
		id = id2;
	else
		id = id1;
	if(isNeighbor(id)){
	struct RoutingT* temp2 = RoutTable;
//	cout<<"\nCost to be updated:"<<ecost<<endl;
	while(temp2 !=NULL)
	{
		if(((temp2->destid == id) && temp2->nextHop == id) || (temp2->destid == id && temp2->cost>ecost) )
		{
			temp2-> nextHop = id;
			temp2-> cost = ecost;
		//	temp2->mod = true;
		}
	/*	else if(temp2->nextHop == id)
		{
			temp2-> cost = ecost;
			temp2->mod = true;
		}*/
		temp2 = temp2->next;
	}}
	else
		cout<<"Update : Err Not a Neighbor"<<endl;
}
void deletNeighbor(int index)
{
	struct costTable* temp2= Cost_Table;
	struct costTable* prev= Cost_Table;
	while(temp2!=NULL)
	{
		if(temp2->id1 == index || temp2->id2 == index)
		{
			if(temp2==Cost_Table)
			{
				updateLinkCostInRT(myID,index,-1);
				Cost_Table=temp2->next;
				return;
			}
			updateLinkCostInRT(myID,index,-1);
			prev->next = temp2->next;
	//		updateLinkCostInRT(myID,index,-1);
			return;
		}
		prev = temp2;
		temp2 = temp2->next;
		
	}
}

int getID(struct sockaddr_in* servr)
{
	if(Server_List!=NULL){
		struct serverList* temp2= Server_List;
		while(temp2 !=NULL)
		{
			if(temp2->IP.s_addr == servr->sin_addr.s_addr)
			{
				return temp2->id;
			}
			temp2 = temp2->next;
		}
	}
	return -1;
}
void updateRTable(int sid, int cnt,struct MsgData RTemp[])
{
	int c2S = getCost(sid);
	
	struct RoutingT* temp2 = RoutTable;
	while(temp2 !=NULL)
	{
		int SCost =  getCost(temp2->destid,sid,cnt,RTemp);
		if(((temp2->cost == -1) || (temp2->cost > c2S+SCost)) && (SCost != -1) && (c2S!=-1))
		{
			temp2-> nextHop = sid;
			temp2-> cost = c2S+SCost;
			temp2->mod = true;
		}
		temp2 = temp2->next;
	}
}
void display()
{
	printf("\n******************************** Routing Table ********************************\n");
	if(RoutTable!=NULL)
	{
		struct RoutingT* temp2= RoutTable;
		while(temp2 !=NULL)
		{
			cout<<"\n "<<temp2->destid<<"\t"<<temp2->nextHop<<"\t"<<temp2->cost<<"\n";
			temp2 = temp2->next;
		}
	}
	else
		printf("\n DISPLAY : Err : Not Initialised yet\n");
	cout<<"\n DISPLAY : SUCCESS"<<endl;
}
void step()
{
	struct MsgFormat myMsg;
	int count=0;
//	cout<<"\n Step : Sending.."<<endl;
	if(RoutTable!=NULL)
	{
		struct RoutingT* temp2= RoutTable;
		while(temp2 !=NULL)
		{
			uint32_t IP = getIP4mId(temp2->destid);
			int port = getPort4mId(temp2->destid);
			if(temp2->destid == myID)
			{
				myMsg.myIP = IP;
				myMsg.myport = port;
			}
			myMsg.data[count].IP = IP;
			myMsg.data[count].port = port;
			myMsg.data[count].id = temp2->destid;
			myMsg.data[count].cost = temp2->cost;
			myMsg.data[count].zero =0; 
			count++;			
			temp2 = temp2->next;
		}
	}
	myMsg.no = count+1; 
	if(Server_List!=NULL){
		struct serverList* temp2= Server_List;
		while(temp2 !=NULL)
		{
			if(temp2->id != myID && isNeighbor(temp2->id))
			{
				//char *buff = "Update :"; 
				struct sockaddr_in IP1;
				IP1.sin_family = AF_INET;
				IP1.sin_port =htons( temp2->port);
				IP1.sin_addr.s_addr = temp2->IP.s_addr;
				memset(&IP1.sin_zero, 0, sizeof (IP1.sin_zero));
			//	cout<<IP1.sin_port<<endl;
			//	cout<<inet_ntoa(IP1.sin_addr)<<endl;
			//	cout<<sockfd<<endl;
				sendto(sockfd, &myMsg, sizeof(myMsg), 0,(struct sockaddr*)&IP1, sizeof (IP1));
				//cout<<"\n Errno :"<<errno<<endl;
			}
			temp2 = temp2->next;
		}
	}
	//cout<<"\n Step : Sent all"<<endl;
	
}
bool isNeighbor(int id)
{
	if(Cost_Table!=NULL){
		struct costTable* temp2= Cost_Table;
		while(temp2 !=NULL)
		{
			if(id == temp2->id1 || id == temp2->id2)
			{
				return true;
			}
			temp2 = temp2->next;
		}
	}
	return false;
}
void packets()
{
	cout<<"\n PACKETS : SUCCESS Number of Packets :"<<npacks<<endl;
	npacks =0;
}
void crash()
{
	close(sockfd);
	exit(0);
}
void sendUpdate2S(uint16_t id2,int cost)
{
	if(isNeighbor(id2)){
	struct sockaddr_in IP;
	if(Server_List!=NULL){
		struct serverList* temp2= Server_List;
		while(temp2 !=NULL)
		{
			if(temp2->id == id2)
			{
			/*	IP.sin_family = AF_UNSPEC;
				IP.sin_port = temp2->port;
				IP.sin_addr.s_addr = temp2->IP.s_addr;
				memset(&IP.sin_zero, 0, sizeof IP.sin_zero);*/
//				struct sockaddr_in IP1;
                                IP.sin_family = AF_INET;
                                IP.sin_port =htons( temp2->port);
                                IP.sin_addr.s_addr = temp2->IP.s_addr;
                                memset(&IP.sin_zero, 0, sizeof (IP.sin_zero));

				break;
			}
			temp2 = temp2->next;
		}
	}
	struct SendCost msg;
	msg.myid = myID;
	msg.id=id2;
	msg.cost = cost;
	sendto(sockfd, (const void*)&msg, sizeof(msg), 0,(struct sockaddr*)&IP, sizeof IP);
	}

	
}
void updateCost(uint16_t sid1,uint16_t sid2,int ecost)
{
	if(Cost_Table!=NULL){
		struct costTable* temp2= Cost_Table;
		while(temp2 !=NULL)
		{
			if((sid1==temp2->id1 && sid2==temp2->id2) || (sid2==temp2->id1 && sid1==temp2->id2))
			{
				temp2->cost = ecost;
				break;
			}
			temp2 = temp2->next;
		}
	}
	
	updateLinkCostInRT(sid1,sid2,ecost);
}
void update(char *vals)
{
	int len = strlen(vals);
	int ind=0;
	while(vals[ind]!='\0')
	{
		if(vals[ind]==' ')
		{
			break;		
		}
		ind++;
	}
	char id1[ind+1];
	char val2[len-ind+1];
	if(vals[ind]=='\0')
	{
		cout<<"\n UPDATE : Err Wrong arguments passed\n";
		return;
	}
	else
	{
		strncpy(id1,vals,ind);
		int index=ind+1,vi=0;
		while(vals[index]!='\0')
		{
			val2[vi]=vals[index];			
			index++;
			vi++;
		}
		id1[ind]= '\0';	
		val2[vi]='\0';
	}
	int len1 = strlen(val2);
	int ind1=0;
	while(val2[ind1]!='\0')
	{
		if(val2[ind1]==' ')
		{
			break;		
		}
		ind1++;
	}
	char id2[ind1+1];
	char cost[len1-ind1+1];
	if(val2[ind1]=='\0')
	{
		cout<<"\n UPDATE : Err Wrong arguments passed\n";
		return;
	}
	else
	{
		strncpy(id2,val2,ind1);
		int index1=ind1+1,vi1=0;
		while(val2[index1]!='\0')
		{
			cost[vi1]=val2[index1];			
			index1++;
			vi1++;
		}
		id2[ind1]= '\0';	
		cost[vi1]='\0';
	}
	uint16_t sid1,sid2;
	int ecost;
	try
	{	
		sid1 = atoi(id1);
		sid2 = atoi(id2);
		if(strcmp(cost,"inf")==0)
		{
			ecost = -1;
		}
		else
			ecost=atoi(cost);
	}
	catch(const char* msg)
	{
		cout<<"\n UPDATE : Err Wrong Arguments";
		return;
	}
	if(Cost_Table!=NULL){
		struct costTable* temp2= Cost_Table;
		while(temp2 !=NULL)
		{
			if((sid1==temp2->id1 && sid2==temp2->id2) || (sid2==temp2->id1 && sid1==temp2->id2))
			{
				temp2->cost = ecost;
				printf("\n UPDATE : SUCCESS\n");
				break;
			}
			temp2 = temp2->next;
		}
	}
	else
		printf("\n UPDATE : Err No such link\n");
	if(myID == sid1)
		sendUpdate2S(sid2,ecost);
	else
		sendUpdate2S(sid1,ecost);
//	cout<<"\n Sending cost:"<<ecost<<endl;
	updateLinkCostInRT(sid1,sid2,ecost);

}
void disable(char *vals)
{
	int index=-1,fd;
	try
	{	
		index = atoi(vals);
	}
	catch(const char* msg)
	{
		cout<<"\n DISABLE : Err Wrong Arguments";
		return;
	}
	struct costTable* temp2= Cost_Table;
	struct costTable* prev= Cost_Table;
	
	while(temp2!=NULL)
	{
		if(temp2->id1 == index || temp2->id2 == index)
		{
			if(temp2==Cost_Table)
			{
				Cost_Table=temp2->next;
				printf("\n DISABLE : SUCCESS\n");
				return;
			}
			prev->next = temp2->next;
			printf("\n DISABLE : SUCCESS\n");
			updateLinkCostInRT(myID,index,-1);
			return;
		}
		prev = temp2;
		temp2 = temp2->next;
		
	}
	if(temp2==NULL)
	{
		return;
	}
	cout<<"\n DISABLE : Err No link with this Server";

}

#endif
