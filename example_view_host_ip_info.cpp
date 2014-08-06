#include <iostream>
#include <vector>
#include "oil.hpp"

using namespace std;
using namespace oil;

int main(int argc, char** argv) 
{
	if(argc == 1)
	{
		cout << "--- View Host IP Info ---" << endl;
		cout << "Usage:    " << *argv << "  [host #1] ..." << endl;		
		cout << "Example:  " << *argv << "  www.abc.com  127.0.0.1  xyz.org" << endl;	
	}
	else while(*(++argv))
	{
		try
		{
			ip host;
			host.address = *argv;
			host.port = 80;
			host.protocol = IPPROTO_TCP; 
			cout << "Looking up '" << host.address() << "'..." << endl;
			vector<ip> interfaces;
			if(!lookup(host, interfaces))
				throw "host not found";
			int count = interfaces.size();
			for(int index = 0; index < count; ++index)
				cout << interfaces[index].address() << "\\" << interfaces[index].port() << endl;			
		}	
		catch(char const* what)
		{
			cout << "Error: " << what << endl;
		}		
	}	
} 
