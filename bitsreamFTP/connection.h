/*

Description: Basically this class is one big windows socket wrapper used to make instanciating a socket less verbose

*/

#ifndef CONNECTION_H
#define CONNECTION_H

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <stdexcept>
#include <iostream>
#include <sstream>
#pragma comment(lib, "Ws2_32.lib")

class connection
{
public:
	SOCKET link;
	struct addrinfo host_info, *host_info_list = NULL;

	connection(std::string host, std::string port);
	~connection();

	bool connect_socket(bool blocking = FALSE);
	bool set_non_blocking(bool blocking);

private:
	std::string connection_host;
	std::string connection_port;

	WSADATA win_socket_data;


};

#endif