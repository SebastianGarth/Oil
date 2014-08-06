#include <iostream>
#include <sstream>
#include "oil.hpp"

using namespace std;
using namespace oil;

int main(int argc, char** argv) 
{
	string endl = "\n";
	if(argc == 1)
	{
		cout << "--- View Default HTTP Server Response ---" << endl;
		cout << "Usage:    " << *argv << "  [host #1] ..." << endl;		
		cout << "Example:  " << *argv << "  www.abc.com  127.0.0.1  xyz.org" << endl;	
	}
	else while(*(++argv))
	{
		try
		{
			client client;
			ip host;
			host.address = *argv;
			host.port = 80;
			host.protocol = IPPROTO_TCP; 
			cout << "Connecting to " << host.address() << " ..." << endl;
			if(!client.connect(lookup(host)))
				throw "host not found";
			ip local = client.local();
			cout << "Local: " << local.address() << "\\" << local.port() << endl;
			ip remote = client.remote();
			cout << "Host: " << remote.address() << "\\" << remote.port() << endl;	
			stringstream request;
			request << "GET / HTTP/1.0" << endl << "Connection: close" << endl << "Host: " << host.address() << endl << endl;		
			if(!client.send(request.str()))
				throw "transmission interrupted";
			cout << "Awaiting response..." << endl;
			string response;
			if(!client.recv(response))		
				throw "connection reset while receiving data";
			cout << response << endl;
		}
		catch(char const* what)
		{
			cout << "Error: " << what << endl;
		}		
	}	
} 
