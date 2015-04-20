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

		std::string operator+(const char* additional_string)
		{
			response_string += additional_string; // Add what we got

			response_lines = parse_lines(); // Fill our lines vector with the lines of our response string

			return std::string(response_string);
		}


		std::vector<std::string> parse_lines()
		{
			std::string marker = "\r\n";
			std::vector<std::string> temp_lines;

			std::string::iterator response_start = response_string.begin(), respond_end = response_string.end();

			while (response_start != respond_end)
			{

				std::string::iterator substring_start = response_start;
			    std::string::iterator substring_end = std::search(substring_start, respond_end, marker.begin(), marker.end()) + marker.size();

				std::string line(substring_start, substring_end);

				temp_lines.push_back(line);

				response_start = substring_end;

			}

			return temp_lines;

		}


		bool complete()
		{
			if (response_lines.empty())
				return false;

			std::string lastline = response_lines.back(); //lastline of response we have

			if (lastline.size() < 4) //Four character of the ftp response should be a empty space
				return false;

			if(lastline[3] == ' ') 
			{
				response_code = lastline.substr(0,3);
				return true;
			}
			else
			{
					return false;
			}
		}

	};


};

#endif
