#ifndef FTP_CLIENT_H
#define FTP_CLIENT_H

/*
	Description: Does all the usual ftp client stuff connect, upload, download ect.
*/

#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <vector>
#include <algorithm>
#include "connection.h"

class ftp_client
{

public:
	const static int BUFFER_SIZE = 4096;

	ftp_client(std::string host, std::string port = "21");
	~ftp_client();

	bool connect();
	std::string	recieve_response();

private:
	std::string host;
	std::string port;
	connection ftp_socket;

	SSIZE_T	send_data(std::string data);
	SSIZE_T recieve_data(char* buffer, const size_t buffer_size);

	struct ftp_response
	{

		std::string response_string;

		std::vector<std::string> response_lines;

		std::string response_code;

		std::string operator+(const char* additional_string);

		std::vector<std::string> parse_lines();

		bool complete();

	};


};

#endif