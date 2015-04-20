#include <iostream>
#include "ftp_client.h"

int main()
{
	
	std::string host = "ftp.ci.heliohost.org", port = "21";

	try
	{
		// Create our ftp session
		ftp_client ftp(host, port);

		// Try to connect to the server
		ftp.connect();

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