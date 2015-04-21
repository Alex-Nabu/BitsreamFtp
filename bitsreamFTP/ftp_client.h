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
	// Igonore the Internal type unless u care about how the ftp response is handled
	class ftp_response
	{

		public:
			std::string response_string;

			std::vector<std::string> response_lines;

			std::string response_code;

			std::string operator+(const char* additional_string);

			bool complete();

		private:
			std::vector<std::string> parse_lines();

	};

public:
	const static int BUFFER_SIZE = 4096;

	ftp_client(std::string host, std::string port = "21");
	~ftp_client();

	bool connect();
	bool login(std::string username, std::string password);
	bool list();
	bool help();

private:
	std::string host;
	std::string port;
	connection ftp_socket;

	void			send_command(std::string command);
	ftp_response	recieve_response();;

};

#endif