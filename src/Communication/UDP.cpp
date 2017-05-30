/*
 * UDP.cpp
 *
 *  Created on: 30 Kas 2013
 *      Author: Farabi
 */

#include "Communication/UDP.hpp"


UDP::UDP() {
	// TODO Auto-generated constructor stub
	isConnected=false;
}

UDP::~UDP() {
	// TODO Auto-generated destructor stub
}

bool UDP::Listen(int port)
{
		Port = port;
	    s = socket(AF_INET, SOCK_DGRAM, 0);
	    if (s < 0)
	    {
	        error( const_cast<char *>("ERROR opening socket") );
	        return false;
	    }

	    bzero((char *) &si_me, sizeof(si_me));


	    int yes = 1;
	    if ( setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1 )
	    {
	        perror("setsockopt");
	    }

	    /*
	    struct timeval tv;
	    tv.tv_sec = 0;
	    tv.tv_usec = 500000;
	    if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0) {
	        perror("Error");
	    }
	    */

	    si_me.sin_family = AF_INET;
	    si_me.sin_addr.s_addr = INADDR_ANY;
	    //si_me.sin_addr.s_addr = inet_addr("192.168.1.14");
	    si_me.sin_port = htons( port );

	    if (bind(s, (struct sockaddr *) &si_me,
	             sizeof(si_me)) < 0)
	    {
	      error( const_cast<char *>( "ERROR on binding" ) );
	      return false;
	    }

	    ip = inet_ntoa(si_me.sin_addr);

	    return true;
}
bool UDP::CreateBroadcastServer(int port)
{
	Port = port;
	struct ifreq ifr;
	strcpy(ifr.ifr_name,"wlan0");

    s = socket( AF_INET, SOCK_DGRAM, 0 );
    if (s < 0)
    {
        error( const_cast<char *>("ERROR opening socket") );
        return false;
    }



    ioctl(s,SIOCGIFBRDADDR,&ifr);
    char *broadcastip = inet_ntoa(((struct sockaddr_in *)&ifr.ifr_broadaddr)->sin_addr);
    printf("[UDP] Broadcast Ip%s\n",broadcastip);




    memset((char *) &si_me, 0, sizeof(si_me));
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(port);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);


    if (bind(s, (struct sockaddr *)&si_me, sizeof(si_me))<0)
    {
        error( const_cast<char *>( "ERROR on binding" ) );
        return false;
    }


    int broadcastEnable = 1;
    if(setsockopt(s,SOL_SOCKET,SO_BROADCAST,&broadcastEnable,sizeof(broadcastEnable))==-1)
    {
    	perror("error setsockopt");
    }

    return true;
}

int UDP::ReadSocket(unsigned char* buffer)
{
	int n;
	int length=65535;


	//ioctl(s, FIONREAD, &length);
	n = recvfrom(s, buffer, length, MSG_WAITALL, (struct sockaddr *)&si_other,&slen);
	ip = inet_ntoa(si_other.sin_addr);

	return n;
}


int UDP::ReadSocket(unsigned char* buffer,int length) const
{
	int n;

	//n = recvfrom(s, buffer, length, MSG_WAITALL, (struct sockaddr *)&si_other,&slen);
	n = recv(s, buffer, length, MSG_WAITALL);

	if (n<0)
	{
		//error( const_cast<char *>( "ERROR reading from socket") );
		printf("UDP ReadError %d\n",n);
		return -1;
	}
	else if(n==0)
	{
		printf("UDP ReadError %d\n",n);
		return -1;
	}
	else
	{
		return n;
	}
}


bool UDP::WriteSocket(unsigned char* buffer,int length) const
{
    int n;
	//if ( (n = write( newsockfd, buffer, length) ) < 0 )
    //ip = inet_ntoa(si_other.sin_addr);
    n = sendto(s,buffer,length, MSG_NOSIGNAL,(struct sockaddr *)&si_other,slen);
	if (n < 0 )
	{
	  //error( const_cast<char *>( "ERROR writing to socket") );
	  perror("setsockopt");
      printf("UDP WriteError %d\n",n);
      usleep(100000);
      //isConnected = false;
	  return false;
	}
	else
	{
		return true;
	}
}

void UDP::error( char *msg ) {
  //perror(  msg );
	//printf(msg);
	//cout<<msg<<endl;
    exit(1);
}

void UDP::SetTargetIp(char *ip,int port)
{
    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(port);
    si_other.sin_addr.s_addr = inet_addr(ip);//inet_addr("239.0.0.1");//inet_addr("255.255.255.0");//inet_addr("192.168.169.100");//
    slen = sizeof(si_other);

    isConnected = true;
}
