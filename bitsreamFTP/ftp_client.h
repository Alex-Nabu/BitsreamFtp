#ifndef FTP_CLIENT_H
#define FTP_CLIENT_H

/*
	Description: Does all the usual ftp client stuff connect, upload, download ect.
*/

#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <stdexcept>
#include <vector>
#include <algorithm>
#include "connection.h"


class ftp_client
{
	// Internal type to handle ftp control replies
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

	bool			connect();
	bool			login(std::string username, std::string password);
	std::string		list();
	std::string		help();
	std::string		pwd();
	bool			cwd(std::string dir);
	bool			download(std::string file_name, std::ofstream& stream);
	bool			quit();

private:
	std::string		host;
	std::string		port;
	connection		control_connection;
	connection*		data_connection; // This is a pointer because we only know the port number which is required for construction after the fact

	void			set_data_representation_type(std::string type = "I");

	void			send_command(std::string command);
	ftp_response	recieve_response();

	void			set_passive_mode();
	void			setup_passive_data_connection();
	std::string		parse_data_connection_info(std::string lastline);

	void			set_active_mode(); 
};

#endif