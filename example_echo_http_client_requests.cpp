#include <iostream>
#include <sstream>
#include <algorithm>
#include "oil.hpp"

using namespace std;
using namespace oil;

int main(int argc, char** argv) 
{
	string endl = "\n";
	try
	{
		cout << "--- Echo Client HTTP Requests ---" << endl;
		server server;
		ip interface;
	/*
		// Set to the system's IPV4 "loopback" address - change to "0.0.0.0" to allow for external network access (!!!) to this server
		  // Note: For IPV6 use instead "::1" for loopback and "::0" otherwise
	*/
		interface.address = "127.0.0.1";
		interface.port = 80;	
		interface.protocol = IPPROTO_TCP;
		cout << "Attempting to bind to interface: " << interface.address() << "\\" << interface.port() << endl;
		if(!server.bind(interface))
			throw "interface is bound to another service";
		for(;;)
		{
			cout << "Waiting for requests..." << endl;
			if(!server.accept())
				throw "subsystem failure";	
			ip local = server.local();
			cout << "Local: " << local.address() << "\\" << local.port() << endl;
			ip remote = server.remote();
			cout << "Client: " << remote.address() << "\\" << remote.port() << endl;	
		/*
			Read one byte at a time until we detect a properly-terminated HTTP client request 
		*/			
			string request;	
			string eof_standard = "\r\n\r\n";
			string eof_compatible = endl + endl;
			for(;;)
			{
				char byte;
				if(!server.recv(byte))
					break;
				request += byte;
				if
				(
					request.end() != search
					(request.begin(), request.end(), eof_standard.begin(), eof_standard.end())
					||
					request.end() != search
					(request.begin(), request.end(), eof_compatible.begin(), eof_compatible.end())
				)
					break;
			}
			cout << "Request: " << endl << request << endl;	
			string html = "<html><body><pre>" + endl + request + "</pre></body></html>";
			stringstream response;
			response << "HTTP/1.0 200 OK" << endl << "Content-Length: " << html.size() << endl << endl << html;			
			if(!server.send(response.str()))
				cout << "Warning: connection was reset" << endl;
		}	
	}
	catch(char const* what)
	{
		cout << "Error: " << what << endl;
	}	
} 
