#ifndef FTP_CLIENT_H
#define FTP_CLIENT_H

/*
	Description: Does all the usual ftp client stuff connect, upload, download ect.
*/

#include <iostream>
#include <stdexcept>
#include "connection.h"

class ftp_client
{

public:
	ftp_client(std::string host, std::string port = "21");
	~ftp_client();

private:
	std::string host;
	std::string port;
	connection ftp_socket;

};

#endif
