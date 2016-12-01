#ifndef MYST_H
#define MYST_H


#include<stdio.h>
#include<iostream>
#include<string>
#include<unistd.h>
#include <cstdlib>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
using namespace std;


struct serverList
{
	uint16_t id;
	uint16_t port;
	struct in_addr IP;
	struct serverList* next;
};

struct costTable
{
	uint16_t id1 , id2 ;
	int cost;
	struct costTable* next;
};

struct RoutingT
{
	uint16_t destid;
	uint16_t nextHop;
	int cost;
	bool mod;
	struct RoutingT* next;
};
struct MsgData
{
	uint32_t IP;
	uint16_t port;
	uint16_t zero;
	uint16_t id;
	int cost;
};
struct MsgFormat 
{
	uint16_t no;
	uint16_t myport;
	uint32_t myIP;
	struct MsgData data[5];
};

struct SendCost
{
	uint16_t myid;
	uint16_t id;
	int cost;
};
struct NTimer
{
	uint16_t id;
	int strtTime;
	struct NTimer* next;	
};
#endif
