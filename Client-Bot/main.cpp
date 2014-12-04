#include <iostream>
#include <exception>
#include <boost/asio.hpp>
#include "udp_connection.h"

using namespace std;

int main()
{
	try
	{
		boost::asio::io_service io_service;
		udp_connection server(io_service);
		server.Connect("127.0.0.1", "8086");
		io_service.run();
	}
	catch (exception& e)
	{
		cerr << e.what() << endl;
	}
    return 0;
}
