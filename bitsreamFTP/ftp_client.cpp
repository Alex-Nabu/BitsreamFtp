/*
	Description: Yeah baby definitions!!!

	Ftp fucntions are pretty minimal ATM but the code is pretty clean and all ftp functions 
	have been implimented in the form of function calls so its easy to add and edit the functions.

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

	ftp_response resp = recieve_response();

	// get some server info
	if (resp.response_code[0] != '2')
		throw std::runtime_error("The server for some could not connect with error code " + resp.response_code);

	// Note the server can also return a 1xx code asking to wait a bit

	return true;

}


// Login to the ftp server given the user credentials
 bool ftp_client::login(const std::string username, const std::string password)
{
	ftp_response resp;
	std::string user_command = "USER " + username + "\r\n";
	std::string password_command = "PASS " + password + "\r\n";


	send_command(user_command);

	resp =  recieve_response(); // fetch the response

	if (resp.response_code[0] == '2') // If no password is required for login and we get immediate success 
		return true;
	
	if (resp.response_code[0] != '3')
		throw std::runtime_error("The server is having a problem logging In. Check USERNAME and passowrd \n"); //change this to a return false
	
	send_command(password_command);

	resp = recieve_response(); 

	if (resp.response_code[0] != '2')
		throw std::runtime_error("The server is having a problem loging in. Check username and PASSWORD \n"); //change this to a return false

	return TRUE;
}


 // Give me some server info
 bool ftp_client::help()
 {
	 ftp_response resp;

	 send_command("HELP\r\n");

	 resp = recieve_response();

	 if (resp.response_code[0] != '2')
		 throw std::runtime_error("The server is having a problem. response code:"+resp.response_code+"\n"); //change this to a return false
 }

 // List all the contents of the current working dir
 bool ftp_client::list()
 {
	 ftp_response resp;

	 send_command("LIST\r\n");
	 
	 resp = recieve_response();

	 if (resp.response_code[0] != '2')
		 throw std::runtime_error("The server failed to list the contents of the dir. response code:"+resp.response_code); //change this to a return false

	 return true;

 }


// Send data through the socket to an host
void ftp_client::send_command(std::string command)
{
	// Convert string to c.str and send it via send method endpoint host
	SSIZE_T bytes_sent = send(ftp_socket.link, command.c_str(), command.length(), 0);

	std::cout << "Sending data............" << std::endl;

}


// Recieve data through the socket into an input buffer
ftp_client::ftp_response  ftp_client::recieve_response()
{

	ftp_response response;

	do
	{

		char temp_buffer[ftp_client::BUFFER_SIZE];

		std::cout << "Revieving......... " << std::endl;

		int bytes_recieved = recv(ftp_socket.link, temp_buffer, sizeof(temp_buffer) - 1, 0);

		if (bytes_recieved < 0)
			return response;

	
		temp_buffer[bytes_recieved] = '\0';

		response + temp_buffer;

	} while (!response.complete());

	std::cout << response.response_string << std::endl;

	return response;

}


// ==================================details of the response subclass=========================================

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

	if (lastline.size() < 4) //Fourth character of the lastline of ftp response should be a empty space
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