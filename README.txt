------------------------------------------------------
OIL (IPV4/IPV6 Object-oriented IPV4/IPV6 Library) for C++
------------------------------------------------------

OIL (formerly known as the now-defunct ITSA project) has been designed with two things in mind: to provide support for the new IPV6 standard and to simplify the manipulation of POSIX sockets. And while there are plenty of free socket libraries around, most seem to suffer one or more major drawbacks, be it complex compilation, external library dependancies, bloat, or of course a lack of IPV6 support. OIL helps to fill that gap, packaged as a single header file and providing a clean, simple API. That said, only the most essential features are implemented (the 'T' in OIL) - special-purpose socket manipulation can be achieved by passing oil::SOCKET handles (see service::connection() and server::binding()) to the standard socket API functions. If nothing else, OIL can be useful tool for rapid prototyping, or what have you.

Features:

~ Seemless IPV6 integration.
~ Read/write using raw arrays, std::strings, std::vectors, or even user-defined types.
~ Sockets are now first-class objects with reasonable ownership semantics.
~ Exception propagation enabled/disabled with "the flip of a switch".
~ Rigorous error checking ensures highly-stateful objects.
~ Oft-used but non-standard identifiers safely redefined in the oil namspace**.
~ Member function names to mirror POSIX equivalents, where applicable. 

Unfortunately, documentation is still lacking, so please refer to the examples for now to get an idea on usage. Also, bear in mind that the code here is still quite fresh and thus bug-prone - what you see here is a weekend project turned work-in-progress. I will continue to test things out as best as I can, but in the meantime please submit any reports/fixes to sebastiangarth@gmail.com - all changes will be noted (with appropriate attributions) in subsequent releases. Enjoy!


**Several of the so-called "de-facto" socket API defines are *non-standard*:
SOCKET, SOCKET_ERROR, INVALID_SOCKET, SD_RECEIVE, SD_SEND, and SD_BOTH. Thus, for maximum portability you should instead reference from your code the corresponding (and identically named) defines found in the oil namespace (eg: oil::SD_BOTH).
