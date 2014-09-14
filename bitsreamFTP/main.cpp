#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")

// Function to abstract the initalizing of win sockets

int init_win_sockets(WSADATA* socket_data)
{
	int result = WSAStartup(MAKEWORD(2, 2), socket_data);

	if (result != 0)
	{
		std::cout << "WSAStartup failed: " << result;
	}

	return result;
}

// Set addrinfo struct properties
void setup_address_info(addrinfo& host_info)
{
	host_info.ai_family = AF_UNSPEC; // Ip version not specified. Can be both
	host_info.ai_socktype = SOCK_STREAM; // Use SOCK_STREAM for TCP or SOCK_DGRAM for UDP
	host_info.ai_protocol = IPPROTO_TCP;
}


// Connect to the socket
// @todo make recusive call to check all posible host info addresses from host_info list
int connect_socket(SOCKET& my_socket, addrinfo* host_info)
{
	std::cout << "Connecting socket..........." << std::endl;

	int result = connect(my_socket, host_info->ai_addr, (int)host_info->ai_addrlen);

	if (result == SOCKET_ERROR)
	{
		closesocket(my_socket);
		my_socket = INVALID_SOCKET;
	}
	else
	{
		std::cout << "Connection made!!" << std::endl;
	}

	return result;
}


// Send data through the socket to an host
SSIZE_T send_data(SOCKET& my_socket, std::string data)
{
	// Convert string to c.str and send it via send method endpoint host
	SSIZE_T bytes_sent = send(my_socket, data.c_str(), data.length(), 0);

	std::cout << "Sending data............"<< std::endl;

	return bytes_sent; // Contains sent bytes or error code

}

// Recieve data through the socket into an input buffer
SSIZE_T recieve_data(SOCKET& my_socket, char* buffer, const size_t buffer_size)
{
	// Read data from the socket into an input buffer
	// Return the amount of bytes read

	size_t bytes_to_read = buffer_size;

	do
	{
		SSIZE_T bytes_recieved = recv(my_socket, buffer, bytes_to_read, 0);

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
		closesocket(my_socket);
		WSACleanup();
		return 1;
	}

	// Prepare to recieve data
	const size_t buffer_size = 1002;

	char data_buffer[buffer_size];

	SSIZE_T bytes_recieved = recieve_data(my_socket, data_buffer, buffer_size);

	if (bytes_recieved <= 0)
	{
		std::cout << "Something bad happened while recieving the data" <<std::endl;
		closesocket(my_socket);
		WSACleanup();
		return 1;
	}

	std::cout << bytes_recieved << " :bytes recieved" << std::endl;

	std::cout << data_buffer << std::endl;

	system("PAUSE");
	
}