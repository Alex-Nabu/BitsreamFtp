#include <iostream>
#include "connection.h"

int main()
{
	
	std::string host = "google.com", port = "80";

	try
	{
		connection socket = connection(host, port);
	}
	catch (std::runtime_error e)
	{
		std::cout << e.what();
		system("PAUSE");
		return -1;
	}
	catch (...)
	{
		return -1;
	}

	system("PAUSE");
	return 0;
}