===========================================================

Datagram, connectionless client and server example.

This demonstrates the steps required for a VERY 
simple server and client using datagram sockets.
Both programs can be run on one machine. If you dial
in to your network, you must be connected even if you
are going to run the programs on one machine.

Example:

First, run DServer in the first DOS box,

	dserver 2000

The server program will bind to port 2000 and print it's
host name to stdout.

	"Server named MachineName waiting on port 2000"

Then run DClient in a second DOS box, using the host
name and port number printed out in step 1.

	dclient MachineName 2000

At that point, the client will send the string 
"From the client" to the server. The server will then
respond by sending the string "From the server" back to
the client. Both programs will then exit.

===========================================================

DSERVER.CPP		Server source code. 
			Compile and link with WSOCK32.LIB
			Only uses C++ so that variables can be 
			declared near where they are used for the 
			first time.

DSERVER.EXE		Compiled server program.

DCLIENT.CPP		Client source code. 
			Compile and link with WSOCK32.LIB
			Only uses C++ so that variables can be 
			declared near where they are used for the 
			first time.

DCLIENT.EXE		Compiled client program.

===========================================================
