#include<stdio.h>
#include<iostream>
#include<string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "commands.h"
#include "myStructs.h"
#include "myUtil.h"
#include "server.h"
#include <fstream>
#include <sstream>
void intialiseS();
void readfile(char *file);

int main(int argc, char *argv[])
{
	cout<<"\n\n****************** Distance Vector Routing Protocol****************"<<endl;
 	if(argc!=6)
	{
		cout<<"Wrong Arguments!!";
		return 0;
	}
	if(strcmp(argv[1],"server")==0)
	{
		if(strcmp(argv[2],"-t")==0 && strcmp(argv[4],"-i")==0)
		{
			period = atoi(argv[5]);
//			cout<<"\n Period :"<<period;
			intialiseS();
//			cout<<"\n Initialised"<<endl;
			readfile(argv[3]);
//			cout<<"\nREadFile"<<endl;
			server(lport);
		}
		else if(strcmp(argv[2],"-i")==0 && strcmp(argv[4],"-t")==0)
		{
			period = atoi(argv[3]);
			intialiseS();
			readfile(argv[5]);
			server(lport);
		}
		else
			cout<<"\n Wrong arguments";
	}
	else
		cout<<"\n Wrong arguments";		
		
	  
        return 0;
}
void readfile(char *file)
{
	string line;
	int count=0;
	int servers = 0, neigh = 0;
	int max;
	char ac[80];
	if (gethostname(ac, sizeof(ac))<0 ) 
	{
        	cout<<"\nError\n"; 
        	return;
    	}
	struct hostent *phe = gethostbyname(ac);
  	if (phe == 0) {
        cout << "Yow! Bad host lookup." << endl;
                return;
        }
  	struct in_addr myaddr;
  	for (int i = 0; phe->h_addr_list[i] != 0; ++i) {
              // struct in_addr addr;
  
              memcpy(&myaddr, phe->h_addr_list[i], sizeof(struct in_addr));
        }	
//	cout<<"\n My IP:"<<myaddr.s_addr<<endl;
//	cout<<"\nStarted REading"<<endl;
	bool found = false, flag = false;
	ifstream myfile (file);
  	if (myfile.is_open())
  	{
	    	while ( getline (myfile,line) )
	    	{
	      		if(count ==0)
			{
				servers = atoi(line.c_str());
		//		cout<<"\n Servers :"<<servers<<endl;
			}
			else if(count == 1)
			{
				neigh = atoi(line.c_str());
		//		cout<<"\nNeighbors :"<<neigh<<endl;
				max = servers + neigh +2;
			}
			else if(count > (max-1))
			{
				cout << "\n Wrong file Parameters \n";
				exit(0);
			}
			else
			{
				if(servers > 0)
				{
					istringstream data(line);
					int c =0;
					int idt;
					struct serverList stemp;
					do
					{
						if(c == 0)
						{
							data >> stemp.id;
		//					cout<<"\n "<<servers<<":id :"<<stemp.id<<endl;
							idt=stemp.id;
						}
						else if(c == 1)
						{
		//					cout<<"\n Here :"<<endl;
							string IP;
							data >> IP;
		//					cout<<"\nIP :"<<IP<<endl;
							inet_pton(AF_INET, IP.c_str(), &(stemp.IP));
							//struct hostent *he;
							//he = gethostbyaddr(&stemp.IP, sizeof stemp.IP, AF_UNSPEC);	
							//stemp.IP.s_addr=s_addr1;
					//		cout<<"\n IP :"<<stemp.IP.s_addr<<endl;
					//		cout<<"\n MyIP :"<<ac<<endl;
					//		cout<<"\n MyIP 2:"<<myaddr.s_addr<<endl;
							if(stemp.IP.s_addr == myaddr.s_addr)
							{
								found = true;
								myID = idt;
								flag = true;
					//			cout<<"Found me"<<endl;
							}
						}
						else if(c == 2)
						{
							
							if(found)
							{
						//		cout<<"Getting port"<<endl;	
								data >> stemp.port;
								lport = stemp.port;
						//		cout<<stemp.port<<endl;
							//	sprintf(lport, "%d", stemp.port);
								found = false;
							}
							else
								data>>stemp.port;
						//	cout<<"\n Port"<<stemp.port<<endl;
						}
						else
						{
						//	cout << "\n data \n"<<data<<endl;
						//	exit(0);
						}
						c++;
					}
					while(c<3);
					insertInList(&stemp);
			//		cout<<"\n Inserted"<<endl;
					servers--;
				}
				else if(neigh > 0)
				{
					istringstream data(line);
					int c =0;
					struct costTable stemp;
					do
					{
						if(c == 0)
						{
							data >> stemp.id1;
						}
						else if(c == 1)
						{
							data >> stemp.id2;
						}
						else if(c == 2)
						{
							data >> stemp.cost;
						}
						else
						{
							cout << "\n Wrong file Parameters \n";
							exit(0);
						}
						c++;
					}
					while(c<3);
					insertInTable(&stemp);
					neigh--;
				}	
			}
			count++;
		}
		if(!flag)
		{
			cout << "\n Self port No available \n";
			exit(0);	
		}
		myfile.close();
	//	cout<<"\n File read";
		return;
	}

	else cout << "Unable to open file";
	exit(0);
}
void intialiseS()
{
	commands = "\n Commands: \n 1.update <server-ID1> <server-ID2> <Link Cost> \n 2.step \n 3.packets \n 4.display \n 5.disable<server-ID> \n 6.crash \n";
	Cost_Table =NULL;
//	cout<<"\n1";
	Server_List = NULL; 
//	cout<<"\n2";
	RoutTable = NULL;
//	cout<<"\n3";
	timers = NULL;
	//lport="";
//	cout<<"\n4";
//	cout<<"\n Returning"<<endl;
	return;
}

