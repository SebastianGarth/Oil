#include <iostream>
#include <cstdlib>
#include "oil.hpp"

using namespace std;
using namespace oil;

int main(int argc, char** argv) 
{	
	int max = ((1 << 16) - 1);
	int next;
	int last;
	bool invalid = false;
	if(argc == 2)
		next = last = atoi(argv[1]);
	else if(argc == 3)
	{
		next = atoi(argv[1]);
		last = atoi(argv[2]);
	}		
	else
		invalid = true;
	if(invalid || (last > max) || (next > last))
	{
		next = 1;
		// You might want to grab a chair...
		last = max;			
		cout << "--- TCP Port Scanner ---" << endl;
		cout << argv[0] << endl;
		cout << "(Scan all " << max <<" ports)" << endl;
		cout << argv[0] << "  [target]" << endl;
		cout << "(Scan port# target)" << endl;		
		cout << argv[0] << "  [first]  [last]" << endl;
		cout << "(Scan all ports ranging from first to last, inclusive)" << endl;	
	}
	client service;	
	ip interface;
	// Set to the system's "loopback" address - no need to check the IPV6 loopback "::1", this will suffice...
	interface.address = "127.0.0.1";
	interface.protocol = IPPROTO_TCP;
	interface.port = next;
	cout << "...scanning..." << endl;
	for(;;)
	{
		if(service.connect(interface))
			cout << "listening: "<< interface.port << endl;		
		if(interface.port++ == last)
			break;
	}
} 
