/*
 * UDP.h
 *
 *  Created on: 30 Kas 2013
 *      Author: Farabi
 */

#ifndef UDP_H_
#define UDP_H_

#include <iostream>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>

class UDP {

private:
    struct sockaddr_in si_me, si_other;
    int s;
    socklen_t slen;

    void error( char *msg );

public:
	UDP();
	virtual ~UDP();
	bool Listen(int port);
	bool CreateBroadcastServer(int port);
	bool WriteSocket(unsigned char* buffer,int length) const;
	void SetTargetIp(char *ip,int port);
	int ReadSocket(unsigned char* buffer,int length) const;
	int ReadSocket(unsigned char* buffer);

	bool isConnected;
	char* ip;
	int Port;
};

#endif /* UDP_H_ */
