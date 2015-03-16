#include "FtpClient.h"

/**
*@todo cleanup code. Impliment proper interfaces
*
*@todo begin ftp protocal
*/

int main()
{

	WSADATA win_socket_data; 
	
	// Initialise  winsockets
	if (init_win_sockets(&win_socket_data) != 0)
	{
		std::cout << "Winsockets cannot be initalized check the dll";
		system("PAUSE");
		return -1;
	}
		

	struct addrinfo host_info, *host_info_list = NULL;

	// Make sure all struct fields are NULL
	ZeroMemory(&host_info, sizeof(host_info));

	// Set addrinfo struct properties
	setup_address_info(host_info);

	// Resolve the server address and port
	// Fill the linked list 'host_info_list' of addrinfo structs for each ip resolved .
	int status = getaddrinfo("www.google.com", "80", &host_info, &host_info_list);

	if (status != 0)
	{
		std::cout << "getaddrinfo failed: " << status;
		WSACleanup();
		return 1;
	}

	// Declare our socket variable
	SOCKET my_socket = INVALID_SOCKET;

	// Create a socket with our host_info criteria
	std::cout << "Creating socket......." << std::endl;
	my_socket = socket(host_info_list->ai_family, host_info_list->ai_socktype, host_info_list->ai_protocol);

	if (my_socket == INVALID_SOCKET)
	{
		std::cout << "Error at socket :" << WSAGetLastError();
		freeaddrinfo(host_info_list);
		WSACleanup();
		return 1;
	}

	// Try to connect our socket
	int socket_status = connect_socket(my_socket, host_info_list);

	// Free the linked list containing host_info for the socket connection
	freeaddrinfo(host_info_list);

	if (socket_status == INVALID_SOCKET)
	{
		std::cout << "Unable to establish socket connection to server" << std::endl;
		WSACleanup();
		return 1;
	}

	std::string homepage = "GET / HTTP/1.1\nhost: www.google.com\n\n";

	if (send_data(my_socket, homepage) == SOCKET_ERROR)
	{
		std::cout << "Error sendig data through socket connection" << std::endl;
		close_connection(my_socket);
		return 1;
	}

	// Prepare to recieve data
	const size_t buffer_size = 1002;

	char data_buffer[buffer_size];

	SSIZE_T bytes_recieved = recieve_data(my_socket, data_buffer, buffer_size);

	if (bytes_recieved <= 0)
	{
		std::cout << "Something bad happened while recieving the data" <<std::endl;
		close_connection(my_socket);
		return 1;
	}

	std::cout << bytes_recieved << " :bytes recieved" << std::endl;

	std::cout << data_buffer << std::endl;

	close_connection(my_socket);
	system("PAUSE");
	return 0;
	
}