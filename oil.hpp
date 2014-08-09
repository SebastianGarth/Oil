/*
	OIL (Object-Oriented IPV4/IPV6 Library) [v0.190983005]
	
	Copyright (c) 2014 Sebastian Garth (sebastiangarth@gmail.com)
	
	Use and distribution of this software in any form whatsoever, be it source, binary, or otherwise 
	is subject to the following terms. Free use is hereby granted for all NON-COMMERCIAL purposes. 
	THIS SOFTWARE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, 
	WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. 
*/

#ifndef OIL_HPP_INCLUDED
#define OIL_HPP_INCLUDED

#ifndef OIL_DEFAULT_BUFFER_LENGTH
#define OIL_DEFAULT_BUFFER_LENGTH 4096
#endif 

#include <list>
#include <string>
#include <cstdio>/*  sprintf() */
/* NOTE: Winsock code added, but not compiled (in other words: may need some tweaking). */
#ifdef OIL_WINSOCK_PLATFORM
#include <winsock.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>	/* close() */
#endif	/* !OIL_WINSOCK_PLATFORM */

namespace oil {
	
namespace detail {	
	
#ifdef OIL_WINSOCK_PLATFORM
void close(SOCKET handle)
{
	closesocket(data);
}

class winsock_synchronize
{
public:
	
	enum
	{
		version_high = 1, 
		version_low = 1
	};
	
	winsock_synchronize(void)
	{
		if(0 != WSAStartup(MAKEWORD(version_low, version_high), &m_subsystem))
		/*
			Not really sure what best to do here. For now, nothing...
		*/
		/* fail<exception>() */
			;
	}

	~winsock_synchronize(void)
	{		
		WSACleanup();
	}
	
protected:
	
	WSADATA m_subsystem;
} 
	startup();
	
#else
	
typedef int SOCKET;
	
enum
{	
	SD_RECEIVE = 0,
	SD_SEND = 1,
	SD_BOTH = 2,
	SOCKET_ERROR = -1,
	INVALID_SOCKET = -1
};

#endif	/* !OIL_WINSOCK_PLATFORM */

bool valid_port(int value)
{
	if(value < 0)
		return false;
	int two_2_the_16th = unsigned(1) << 16;
/*
	If the following is true then we're most definitely dealing with a number less than 2^16...
*/	
	if(two_2_the_16th == 0)
		return true;
	return value < two_2_the_16th;
}

template<typename Exception>
void fail(bool condition = true)
{
	if(condition)
		throw Exception();
}

template<typename Type>
struct functor_like
{
	functor_like(void)
	: m_value(Type())
	{	}	
	
	template<typename Whatever>
	functor_like(Whatever const& initial)
	: m_value(initial)
	{	}
	
	template<typename Whatever>
	functor_like& operator()(Whatever const& value)
	{
		m_value = value;
		return *this;
	}	
	
	Type const& operator()(void) const
	{
		return m_value;
	}	

	Type& operator()(void)
	{
		return m_value;
	}	
	
	operator Type const&(void) const
	{
		return m_value;
	}
	
	operator Type&(void)
	{
		return m_value;
	}
	
protected:	
	
	Type m_value;
};

template<typename Type>
struct functor_like<Type const>
{	
	functor_like(void)
	: m_value(Type())
	{	}	
	
	template<typename Whatever>
	explicit functor_like(Whatever const& initial)
	: m_value(initial)
	{	}	
	
	Type operator()(void) const
	{
		return m_value;
	}	
	
	operator Type(void)
	{
		return m_value;
	}
	
	Type& access(void)
	{
		return m_value;
	}
	
protected:	
	
	Type m_value;
};

/*
	The purpose here is to prevent multiple linker references
*/
template<typename IGNORED = void> struct settings_prevent_multiple_references_to_statics
{
	static bool s_exceptions;
	static int s_default_buffer_length;
}; 

template<typename IGNORED> bool settings_prevent_multiple_references_to_statics<IGNORED>::s_exceptions = false; 
template<typename IGNORED> int settings_prevent_multiple_references_to_statics<IGNORED>::s_default_buffer_length = OIL_DEFAULT_BUFFER_LENGTH; 

struct settings : settings_prevent_multiple_references_to_statics<>
{
	static void exceptions(bool value)
	{
		s_exceptions = value;
	}
	static bool exceptions(void)
	{
		return s_exceptions;
	}	
	static void default_buffer_length(int value)
	{
		s_default_buffer_length = value;
	}
	static int default_buffer_length(void)
	{
		return s_default_buffer_length;
	}	

protected:
	
	using settings_prevent_multiple_references_to_statics<>::s_exceptions;
	using settings_prevent_multiple_references_to_statics<>::s_default_buffer_length;	
};

struct exception
{		
	static void propagate(void)
	{
		if(settings::exceptions())
			throw;
	}	
};	

struct cleanup_exception : exception
{	};

struct shutdown_exception : cleanup_exception
{	};

struct close_exception : cleanup_exception
{	};

struct bind_exception : exception
{	};

struct accept_exception : exception
{	};

struct connect_exception : exception
{	};

struct send_exception : exception
{	};

struct recv_exception : exception
{	};

struct conversion_exception : exception
{	};

struct lookup_exception : exception
{	};

class socket : public functor_like<SOCKET>
{
public:
	
	functor_like<int const> family;
	functor_like<int const> protocol;

	socket(void)
	: functor_like<SOCKET>(INVALID_SOCKET)
	{	}

	socket(socket const& goner)
	: functor_like<SOCKET>(INVALID_SOCKET)
	{	
		*this = goner;	
	}
	
	socket(int family, int protocol)
	: functor_like<SOCKET>(::socket(family, 0, protocol)), family(family), protocol(protocol)
	{	}	
	
	socket(SOCKET value, int family, int protocol)
	: functor_like<SOCKET>(value), family(family), protocol(protocol)
	{	}	
	
	socket& operator =(socket const& goner)
	{
		shutdown();
		((functor_like<SOCKET>&)*this) = goner;
		family = goner.family;
		protocol = goner.protocol;		
		const_cast<socket&>(goner).invalidate();
		return *this;
	}

	void shutdown(int which = SD_BOTH)
	{	
		try
		{		
			if(valid())
			{
				fail<shutdown_exception>(::shutdown(*this, which) != 0);
				if(which == SD_BOTH)
				{
					fail<close_exception>(::close(*this) != 0);				
					invalidate();
				}	
			}
		}	
		catch(...)
		{
			invalidate();
			exception::propagate();
		}
	}	

	void invalidate(void)
	{
		((functor_like<SOCKET>&)*this) = INVALID_SOCKET;
		family.access() = 0;
		protocol.access() = 0;		
	}
	
	bool valid(void) const
	{
		return *this != INVALID_SOCKET;	
	}

	SOCKET handle(void) const
	{
		return *this;
	}
	
	virtual ~socket(void)
	{
		shutdown();
	}
};

struct ip;

class endpoint
{
public:
	
	union implementation_specific
	{
		sockaddr_in ipv4;
		sockaddr_in6 ipv6;
		struct
		{
			u_int16_t family;
			u_int16_t port;
		};
	}
		traits;
	
	enum
	{
		sizeof_implementation = sizeof(implementation_specific)
	};		
			
	endpoint(void)
	{	
		invalidate();
	}

	bool set(ip const& details);
		
	void invalidate(void)
	{
		traits = implementation_specific();
	}	

	functor_like<int> protocol;	
};			

struct ip
{
/*
	Normally this will be in some numeric implementation_specific - hostnames 
		are ONLY used when and if this ip is passed to lookup()
*/
	functor_like<std::string> address;	
/* 
	Note: Standard dictates that the port number NOT exceed ((2^16)-1)	
*/
	functor_like<int> port;	
/* 
	Example: IPPROTO_TCP, IPPROTO_UDP, etc...	
*/		
	functor_like<int> protocol;	
/*
	Warning: DO NOT CHANGE THE DEFAULT VALUES HERE!
*/	
	ip(std::string const& address = "", int port = 0, int protocol = 0)
	: address(address), port(port), protocol(protocol)
	{	}
	
	void invalidate(void)
	{
		*this = ip();
	}
	
	bool set(endpoint const& details)
	{
		try
		{
			char buffer[INET6_ADDRSTRLEN];
			void const* text;
			if(details.traits.family == AF_INET6) 
				text = &details.traits.ipv6.sin6_addr;
			else if(details.traits.family == AF_INET)
				text = &details.traits.ipv4.sin_addr;
			else
				fail<conversion_exception>();
			fail<conversion_exception>(NULL == inet_ntop(details.traits.family, text, buffer, INET6_ADDRSTRLEN));	
			address = buffer;	
			port = ntohs(details.traits.port);
			fail<conversion_exception>(false == valid_port(port));			
			protocol = details.protocol;
			m_family = details.traits.family;
			return true;
		}
		catch(...)
		{
			invalidate();
			exception::propagate();
			return false;
		}	
	}	
	
	endpoint details(void) const
	{
		endpoint result;
		result.set(*this);	
		return result;
	}
/*
	KLUDGE: the user may have set address, so we can't use a functor here
	TODO: put into functor interface
*/	
	int family(void) const
	{
		try
		{
			if(m_family == 0)
				return details().traits.family;
		}
		catch(...)
		{
			return 0;
		}
		return m_family;
	}
	
	bool valid(void) const
	{
		endpoint result;
		return true == result.set(*this);		
	}
	
protected:
	
	functor_like<int> m_family;		
};

bool endpoint::set(ip const& details)
{
	try
	{
		if(0 < inet_pton(AF_INET6, details.address().c_str(), &traits.ipv6.sin6_addr))
			traits.family = AF_INET6;
		else if(0 < inet_pton(AF_INET, details.address().c_str(), &traits.ipv4.sin_addr))
			traits.family = AF_INET;
		else
		{
			traits.family = 0;
			fail<conversion_exception>();
		}
		fail<conversion_exception>(false == socket(traits.family, details.protocol).valid());
		protocol = details.protocol;	
		fail<conversion_exception>(false == valid_port(details.port));
		traits.port = htons(details.port);	
		return true;
	}
	catch(...)
	{
		invalidate();
		exception::propagate();
		return false;
	}		
}		

template<typename PushBackInterface>
bool lookup(ip const& host, PushBackInterface& result)
{
	addrinfo* link = NULL;
	try
	{
		result.clear();
		addrinfo settings = addrinfo();
		settings.ai_family = AF_UNSPEC;
		settings.ai_protocol = host.protocol;	
		fail<lookup_exception>(false == valid_port(host.port));
		char host_port_text[6];
		sprintf(host_port_text, "%.5d", host.port);
		fail<lookup_exception>(0 != getaddrinfo(host.address().c_str(), host_port_text, &settings, &link));
		ip data;
		for(; link != 0; link = link->ai_next)
		{
			endpoint details;
			memcpy(&details, link->ai_addr, endpoint::sizeof_implementation);
			details.protocol = link->ai_protocol;
			fail<lookup_exception>(false == data.set(details));
			result.push_back(data);
		}	
		fail<lookup_exception>(0 == result.size());
		freeaddrinfo(link);		
		return true;
	}
	catch(...)
	{
		if(link != NULL)
			freeaddrinfo(link);
		exception::propagate();
		return false;
	}
}

ip lookup(ip const& host)
{
	std::list<ip> result;
	return lookup(host, result) ? *result.begin() : ip();
}

struct box
{
	box(void)
	: data(NULL), size(0), width(0)
	{	}
	
	box(char const& data)
	: data(&data), size(1), width(1)
	{	}

	box(unsigned char const& data)
	: data(&data), size(1), width(1)
	{	}		
		
	template<typename ArrayLike, typename SFINAE = typename ArrayLike::value_type>
	box(ArrayLike const& buffer)
	{	
		width = sizeof(buffer[0]);
		size = buffer.size() * width;
		if(size != 0)
			data = &buffer[0];
		else
			data = NULL;
	}		
		
	template<typename Array>
	box(Array const* array, int length)
	{	
		data = array;
		width = sizeof(array[0]);
		size = length * width;
	}
		
	template<typename Array>
	box(Array const* array)
	{
		data = array;
		width = sizeof(array[0]);		
		Array const* end = array;
		while(*end)
			++end;
		size = (end - array) * width;
	}			
		
	void const* data;
	int size;
	int width;
};
	
class service
{		
public:	

	socket connection;
	functor_like<int> flags_send;
	functor_like<int> flags_recv;
	
	bool send(box const& buffer)
	{
		try
		{
			int index = 0;
			int size = buffer.size;
			char* data = (char*)buffer.data;
			for(;;)
			{
				int count = ::send(connection, &data[index], size - index, flags_send());
				fail<send_exception>(count == SOCKET_ERROR);
				index += count;
				if(index == size)
					break;				
			}
			return true;
		}
		catch(...)
		{		
			shutdown();
			exception::propagate();			
			return false;
		}						
	}

	bool send(char const& data)
	{	
		return send(box(data));
	}

	bool send(unsigned char const& data)
	{	
		return send(box(data));
	}		
	
	template<typename ArrayLike, typename SFINAE = typename ArrayLike::value_type>
	bool send(ArrayLike const& data)
	{	
		return send(box(data));
	}		
	
	template<typename Array>
	bool send(Array const* data, int length)
	{	
		return send(box(data, length));
	}
	
	template<typename Array>
	bool send(Array const* data)
	{
		return send(box(data));
	}

	int recv(box const& buffer)
	{
		try
		{
			int index = 0;
			int size = buffer.size;
			char* data = (char*)buffer.data;
			for(;;)
			{
				int count = ::recv(connection, &data[index], size - index, flags_recv());
				fail<recv_exception>(count == SOCKET_ERROR);
				index += count;
				if(count == 0 || index == size)
					break;
			}
			fail<recv_exception>(index % buffer.width != 0);
			return index / buffer.width;
		}
		catch(...)
		{		
			shutdown();
			exception::propagate();
			return 0;
		}		
	}	
	
	int recv(char& data)
	{	
		return recv(box(data));
	}

	int recv(unsigned char& data)
	{	
		return recv(box(data));
	}		
	
	template<typename Array>
	int recv(Array* data, int length)
	{	
		return recv(box(data, length));
	}
	
	template<typename ArrayLike, typename SFINAE = typename ArrayLike::value_type>
	int recv(ArrayLike& buffer)
	{
		try
		{
			int index = 0;
			int length = settings::default_buffer_length();			
			buffer.resize(length);
			for(;;)
			{
				int count = recv(&buffer[index], length);
				fail<recv_exception>(false == connected());
				if(count == 0)
					break;
				index += count;
				fail<recv_exception>(index < 0);
				length *= 2;
				fail<recv_exception>(length < 0);
				for(;;)
				{
					try
					{
						int adjusted = index + length;
						fail<recv_exception>(adjusted < 0);
						buffer.resize(adjusted);	
						break;
					}
					catch(std::bad_alloc const&)
					{
						length /= 2;
						if(length < 1)
							exception::propagate();
					}
				}
			}
			buffer.resize(index);
			return index;
		}
		catch(...)
		{
			shutdown();
			buffer.clear();
			exception::propagate();
			return 0;			
		}
	}
	
	void shutdown(int which = SD_BOTH)
	{
		connection.shutdown(which);		
	}
	
	operator bool(void) const
	{
		return connected();
	}	
	
	ip remote(void) const
	{
		try
		{
			ip result;
			endpoint details;
			socklen_t size = sizeof(details);
			fail<exception>(!connection.valid());
			fail<exception>(0 != getpeername(connection, (::sockaddr*)&details, &size));
			details.protocol = connection.protocol();
			fail<exception>(false == result.set(details));
			return result;
		}
		catch(...)
		{
			return ip();
		}		
	}
	
	ip local(void) const
	{
		try
		{
			ip result;
			endpoint details;
			socklen_t size = sizeof(details);
			fail<exception>(!connection.valid());
			fail<exception>(0 != getsockname(connection, (::sockaddr*)&details, &size));
			details.protocol = connection.protocol();
			fail<exception>(false == result.set(details));
			return result;
		}
		catch(...)
		{
			return ip();
		}	
	}		
	
	bool connected(void) const
	{
		return connection.valid();
	}	
};

class client : public service
{
public:
	
	client(void)
	{	}

	client(ip const& host)
	{
		connect(host);
	}	
	
	bool connect(ip const& host)
	{
		try
		{
			shutdown();
			endpoint details = host.details();
			connection = socket(details.traits.family, host.protocol);
			fail<connect_exception>(false == connection.valid());	
			fail<connect_exception>(SOCKET_ERROR == ::connect(connection, (::sockaddr*)&details, sizeof(details)));
		}
		catch(...)
		{
			shutdown();
			exception::propagate();
		}
		return connected();
	}
};

class server : public service
{
public:

	socket binding;

	server(void)
	{	}
	
	server(ip const& interface, bool accept_connections_immediately = false)
	{	
		bind(interface);
		if(accept_connections_immediately)
			accept();
	}	

	bool bind(ip const& interface)
	{
		try
		{
			unbind();
			endpoint details = interface.details();
			binding = socket(details.traits.family, interface.protocol);
			fail<bind_exception>(false == binding.valid());
			fail<bind_exception>(SOCKET_ERROR == ::bind(binding, (::sockaddr*)&details, sizeof(details)));
			fail<bind_exception>(SOCKET_ERROR == ::listen(binding, SOMAXCONN));
		}
		catch(...)
		{
			binding.shutdown();
			exception::propagate();
		}		
		return bound();
	}
	
	bool accept(void)
	{
		try
		{
			shutdown();
			endpoint details;
			socklen_t size = sizeof(details);
			connection = socket(::accept(binding, (::sockaddr*)&details, &size), binding.family(), binding.protocol());
			fail<accept_exception>(false == connection.valid());
		}
		catch(...)
		{
			shutdown();
			exception::propagate();
		}				
		return connected();
	}	
	
	void unbind(void)
	{
		shutdown();
		binding.shutdown();
	}

	bool bound(void) const
	{
		return binding.valid();
	}	
};

}	/* namespace detail */

using detail::box;
using detail::valid_port;
using detail::functor_like;
using detail::settings;
using detail::ip;
using detail::service;
using detail::server;
using detail::client;
using detail::lookup;
using detail::fail;
using detail::exception;
using detail::cleanup_exception;
using detail::shutdown_exception;
using detail::close_exception;
using detail::bind_exception;
using detail::accept_exception;
using detail::connect_exception;
using detail::send_exception;
using detail::recv_exception;
using detail::conversion_exception;
using detail::lookup_exception;
using detail::socket;
using detail::SOCKET;
using detail::SD_RECEIVE;
using detail::SD_SEND;
using detail::SD_BOTH;
using detail::SOCKET_ERROR;
using detail::INVALID_SOCKET;

}	/* namespace oil */

#endif	/* OIL_HPP_INCLUDED */
