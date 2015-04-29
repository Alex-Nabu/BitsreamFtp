/*
	Description: Yeah baby definitions!!!

	Ftp fucntions are pretty minimal ATM but the code is pretty clean and all ftp functions 
	have been implimented in the form of function calls so its easy to add and edit the functions.

*/

#include "ftp_client.h"

ftp_client::ftp_client(std::string host, std::string port) : host(host), port(port), control_connection(host, port) {}

ftp_client::~ftp_client() {}


// Connect to the server
bool ftp_client::connect()
{

	std::cout << "Connecting ..........." << std::endl;

	if (!control_connection.connect_socket(TRUE))
	{
		throw std::runtime_error("could not connect to. Check the socket details. (its I/O settings or its host and port address)");
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

	return true;
}

// Set ftp data connections in passive mode
// Set up passive data connection 
bool ftp_client::set_passive_mode()
{
	ftp_response resp;

	send_command("PASV\r\n");

	resp = recieve_response();

	if (resp.response_code[0] != '2')
		return false;

	//Now set the data port
	std::string connection_info = parse_data_connection_info(resp.response_lines.back()); // give us the string with the host/port info

	std::vector<std::string> peices; // our temp vector to hold em parts
	std::stringstream ss(connection_info); // our stream of words
	std::string temp; // our temp word storage location

	while (std::getline(ss, temp, ',')) // Break it up. deliminator comma
		peices.push_back(temp);

	// Now to get the port
	int pp1 = std::stoi(peices[4]);
	int pp2 = std::stoi(peices[5]);
	int port_number = (pp1 * 256) + pp2;


	// Now to get the host IP and port in strings
	char dot = '.';
	std::string host = peices[0] + dot + peices[1] + dot + peices[2] + dot + peices[3];
	std::string port = std::to_string(port_number);

	// Set the ftp_client::data connection
	data_connection = new connection(host, port); 

	return true;
}

// Set ftp data connections in active mode
// Setup active data connection
bool ftp_client::set_active_mode()
{
	// @todo : write code to support active mode here
	// @howto : bind the ftp_client::data_connecion to a random port and send its info over to server
	return false;
}


bool ftp_client::setup_passive_data_connection()
{
	ftp_response resp;

	data_connection->connect_socket(); // Set up the data connection

	resp = recieve_response();

	if (resp.response_code != "150")
		throw std::runtime_error("failed to setup data connection. response code:" + resp.response_code); //change this to a return false

	return true;
}


bool ftp_client::set_data_representation_type(std::string type)
{
	ftp_response resp;

	send_command("TYPE " + type + "\r\n");

	resp = recieve_response();

	if (resp.response_code[0] != '2')
		throw std::runtime_error("The server failed exececute command with. response code:" + resp.response_code); // change this to return false

	return true;
}


// Give me some server info
std::string ftp_client::help()
 {
	 ftp_response resp;

	 send_command("HELP\r\n");

	 resp = recieve_response();

	 if (resp.response_code[0] != '2')
		 throw std::runtime_error("The server is having a problem. response code:"+resp.response_code+"\n"); //change this to a return false

	 return resp.response_string;
 }


 // List all the contents of the current working dir
// @todo add support for active ftp mode
std::string ftp_client::list()
 {
	 ftp_response resp;

	 if (!set_passive_mode()) //
		 return false;

	 send_command("LIST -l\r\n");
	 
	 setup_passive_data_connection();

	 std::string listings;

	 int bytes_recieved;

	 do
	 {
		 char temp_buffer[ftp_client::BUFFER_SIZE];

		 bytes_recieved = recv(data_connection->link, temp_buffer, sizeof(temp_buffer) - 1, 0);

		 temp_buffer[bytes_recieved] = '\0';
		 listings += temp_buffer;

	 } while (bytes_recieved);

	 return listings;

 }


// Return the current working directorie
std::string ftp_client::pwd()
{
	ftp_response resp;

	send_command("PWD\r\n");

	resp = recieve_response();

	if (resp.response_code[0] != '2')
		throw std::runtime_error("The server failed to execute command pwd. response code:" + resp.response_code); //change this to a return false

	return resp.response_string;

}


// List all the contents of the current working dir
bool ftp_client::cwd(std::string dir)
{
	ftp_response resp;

	std::string change_dir = "CWD " + dir + "\r\n";

	send_command(change_dir);

	resp = recieve_response();

	if (resp.response_code[0] != '2')
		throw std::runtime_error("The server failed exececute command with. response code:" + resp.response_code); //change this to a return false

	return true;
}


// Send data through the socket to an host
void ftp_client::send_command(std::string command)
{
	// Convert string to c.str and send it via send method endpoint host
	SSIZE_T bytes_sent = send(control_connection.link, command.c_str(), command.length(), 0);

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

		int bytes_recieved = recv(control_connection.link, temp_buffer, sizeof(temp_buffer) - 1, 0);

		if (bytes_recieved < 0)
			return response;

	
		temp_buffer[bytes_recieved] = '\0';

		response + temp_buffer;

		std::cout << response.response_string;

	} while (!response.complete());

	return response;

}


// parse the response of the PASV reply to get the data connection info
std::string ftp_client::parse_data_connection_info(std::string lastline)
{
	std::string::iterator sub_start = std::find(lastline.begin(), lastline.end(), '(');
	std::string::iterator sub_end = std::find(lastline.begin(), lastline.end(), ')');

	if (sub_start == lastline.end() || sub_end == lastline.end())
		throw std::runtime_error("The data sent from the server about the data connecion cannot be parsed");

	std::string connection_info(sub_start + 1, sub_end);

	if (connection_info.empty())
		throw std::runtime_error("The server didnt send us any information about how the data connection");

	return connection_info;
}

// ==================================details of the response subtype=========================================

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