/*
	Description: Yeah baby definitions!!!
*/

#include "ftp_client.h"

ftp_client::ftp_client(std::string host, std::string port) : host(host), port(port), ftp_socket(host, port) {}

ftp_client::~ftp_client() {}
