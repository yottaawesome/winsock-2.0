===========================================================

GetHTTP Example.

Stream, connection-oriented client utility to retrieve 
files from the World Wide Web via an HTTP Server.

Pass the server name and full path of the requested file on 
the command line and redirect the output to a file.  The 
program prints messages to stderr as data is retrieved from 
the server.

Example:

GetHTTP www.idgbooks.com /index.html > index.html

===========================================================

GETHTTP.CPP		Source code. 
				Compile and link with WSOCK32.LIB
				Only uses C++ so that variables can be 
				declared near where they are used for the 
				first time.

GETHTTP.EXE		Compiled program.

===========================================================
