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