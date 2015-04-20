/*
	Description: Yeah baby definitions!!!
*/

#include "ftp_client.h"

ftp_client::ftp_client(std::string host, std::string port) : host(host), port(port), ftp_socket(host, port) {}

ftp_client::~ftp_client() {}


// Connect to the server
bool ftp_client::connect()
{

	std::cout << "Connecting ..........." << std::endl;

	if (!ftp_socket.connect_socket(TRUE))
	{
		throw std::runtime_error("could not connect to. Check the socket details. (its I/O settings or its host and port address)");
		return false;
	}
		
	std::cout << "Connection made!!" << std::endl;

	// get some server info
	std::cout << recieve_response();

	return true;

}


// Recieve data through the socket into an input buffer
std::string  ftp_client::recieve_response()
{

	ftp_response response;

	do
	{
		char temp_buffer[ftp_client::BUFFER_SIZE];

		int bytes_recieved = recv(ftp_socket.link, temp_buffer, sizeof(temp_buffer) - 1, 0);

		if (bytes_recieved < 0)
			return response.response_code;

		std::cout << "Revieving......... " << bytes_recieved << " bytes of data" << std::endl;
	
		temp_buffer[bytes_recieved] = '\0';

		response + temp_buffer;

	} while (!response.complete());

	std::cout << response.response_string << std::endl;

	return response.response_code;

}


// Send data through the socket to an host
SSIZE_T  ftp_client::send_data(std::string data)
{
	// Convert string to c.str and send it via send method endpoint host
	SSIZE_T bytes_sent = send(ftp_socket.link, data.c_str(), data.length(), 0);

	std::cout << "Sending data............" << std::endl;

	return bytes_sent; // Contains sent bytes or error code

}


// Recieve data through the socket into an input buffer
SSIZE_T ftp_client::recieve_data(char* buffer, const size_t buffer_size)
{

	size_t bytes_to_read = buffer_size;

	do
	{
		SSIZE_T bytes_recieved = recv(ftp_socket.link, buffer, bytes_to_read-1, 0);

		if (bytes_recieved < 1)
		{
			return bytes_recieved; // Connection closed or error occured
		}

		std::cout << "Revieving......... " << bytes_recieved << " bytes of data" << std::endl;

		bytes_to_read -= bytes_recieved;
		buffer += bytes_recieved;
	} while (bytes_to_read > 0);

	return buffer_size; // The total amount of bytes recieved

}


//==================================details of the response subclass=========================================

// What happends when we append to our ftp_response 
std::string ftp_client::ftp_response::operator+(const char* additional_string)
{
	response_string += additional_string; // Add what we got

	response_lines = parse_lines(); // Fill our lines vector with the lines of our response string

	return std::string(response_string);
}


// Parse an ftp response to and return a vector of all its lines
std::vector<std::string> ftp_client::ftp_response::parse_lines()
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

// Check if we have reached the end of our response
bool ftp_client::ftp_response::complete()
{
	if (response_lines.empty())
		return false;

	std::string lastline = response_lines.back(); //lastline of response we have

	if (lastline.size() < 4) //Four character of the ftp response should be a empty space
		return false;

	if (lastline[3] == ' ')
	{
		response_code = lastline.substr(0, 3);
		return true;
	}
	else
	{
		return false;
	}
}