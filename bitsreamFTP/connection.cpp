#include "connection.h"

connection::connection(std::string host, std::string port) : 

	connection_host(host),
	connection_port(port),
	link(INVALID_SOCKET)

{

	int result = WSAStartup(MAKEWORD(2, 2), &win_socket_data);

	if (result != 0)
	{
		throw std::runtime_error("could'nt start windows OS sockets");
	}


	// Make sure all struct fields are NULL
	ZeroMemory(&host_info, sizeof(host_info));


	host_info.ai_family = AF_UNSPEC; // Ip version not specified. Can be both
	host_info.ai_socktype = SOCK_STREAM; // Use SOCK_STREAM for TCP or SOCK_DGRAM for UDP
	host_info.ai_protocol = IPPROTO_TCP;


	// Resolve the server address and port
	// Fill the linked list 'host_info_list' of addrinfo structs for each ip resolved .
	int status = getaddrinfo(host.c_str(), port.c_str(), &host_info, &host_info_list);

	if (status != 0)
	{
		// Free the linked list containing host_info for the socket connection
		freeaddrinfo(host_info_list);

		throw std::runtime_error("getaddrinfo failed: ");
	}

	// Create a socket with our host_info criteria
	std::cout << "Creating socket......." << std::endl;
	link = socket(host_info_list->ai_family, host_info_list->ai_socktype, host_info_list->ai_protocol);

	if (link == INVALID_SOCKET)
	{
		// Convert an int into a string
		std::ostringstream converter;
		converter << WSAGetLastError();

		std::string error_code = converter.str(), error_msg = "Socket creation failed with error code ";
		
		throw std::runtime_error(error_msg+error_code);
	}


}


connection::~connection()
{

	// Free the linked list containing host_info for the socket connection
	freeaddrinfo(host_info_list);

	WSACleanup();
	closesocket(link);
}


// Connect to a socket given the necsessary info
// @todo make recusive call to check all posible host info addresses from host_info list
bool connection::connect_socket(bool blocking)
{

	int result = ::connect(link, host_info_list->ai_addr, (int)&host_info_list->ai_addrlen);

	// if the socket gave an error connecting or setting the I/O mode
	if ( result == SOCKET_ERROR || !set_non_blocking(blocking) )
	{
		link = INVALID_SOCKET;
		return false;
	}
	else
	{
		return true;
	}

}


// Set a socket blocking or non blocking based on @blocking param
bool connection::set_non_blocking(bool blocking)
{
	// If iMode!=0, non-blocking mode is enabled.
	u_long iMode = blocking ? 0 : 1;

	int result = ioctlsocket(link, FIONBIO, &iMode);

	if (result != NO_ERROR)
	{
		link = INVALID_SOCKET;
		return false;
	}
	
	return true;

}
