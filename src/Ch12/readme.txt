DIRECTORY CONTENTS

Socket Adapter ECHO Client and Server.

Both programs will use WinSock 2 functionality
if it is available or fall back to WinSock 1.1
if it isn't.

Uses DWINSOCK functions to dynamically
link to WS2_32.DLL or if WinSock 2 isn't 
available, dynamically link to WSOCK32.DLL.

Protocol Independent under WinSock 2
Uses UDP on WinSock 1.1


\CLIENT		Socket Adapter ECHO Client Source Code

\SERVER		Socket Adapter ECHO Server Source Code

DWINSOCK.C		Functions used to dynamically link to
			either WS2_32.DLL or WSOCK32.DLL and
			to map function addresses to global 
			function poniters.

DWINSOCK.H		Inlude for DWINSOCK.C

DWINSOCK1.INC	Macros and definitions for functions
			that are available in both WinSock 1.1 
			and 2

DWINSOCK2.INC	Macros and definitions for functions
			that are only available in WinSock 2.
			
AEGUID.H		GUID for Socket Adapter ECHO Example

