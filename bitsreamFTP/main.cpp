#include <iostream>
#include <fstream>
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

		//Try to login to the server
		ftp.login("alexnabu", "******");

		// Get some server info
		std::cout << ftp.help();

		// Print the current dir
		ftp.pwd();
		
		// change dir
		ftp.cwd("public_html");

		// try and get a listing of our directorires
		std::cout << ftp.list();

		// download a file from the server
		std::string file_name = "index.html";
		std::ofstream mystream(file_name, std::ofstream::binary, std::ofstream::trunc);

		ftp.download("index.html", mystream);

		// close down all connection
		ftp.quit();

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