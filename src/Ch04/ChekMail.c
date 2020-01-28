//
// ChekMail.c	--	Check for mail waiting on a
//					POP3 server
//
// Compile with LARGE model on 16-bit Windows
// Link with winsock.lib on 16-bit Windows
// Link with wsock32.lib on 32-bit Windows
				
#include <windows.h>
#include <string.h>
#include <stdio.h>
#include <winsock.h>

#include "chekmail.h"
#include "resource.h"

// ----- Global Variables       
// POP3 Host Name
char		gszServerName[255];
// User ID
char		gszUserId[80];  
// Password
char		gszPassword[80];
// Handle for WSAAsyncGetHostByName()
HANDLE		hndlGetHost;           
// HostEnt buffer for WSAAsyncGetHostByName()
char		bufHostEnt[MAXGETHOSTSTRUCT];
// The socket
SOCKET 		socketPOP;
// Scratch buffer for wsprintf()
char		gszTemp[255];
// Variables used for timing 
// For example only -- not really needed
DWORD		gdwTicks;
DWORD		gdwElapsed;
// recv() data buffer
char		gbufRecv[256];

// ----- Application State
int			gnAppState;
#define		STATE_CONNECTING 	1
#define		STATE_USER			2
#define		STATE_PASS			3
#define		STATE_STAT			4
#define		STATE_QUIT			5

//
// WinMain() - Entry Point
//                    
int PASCAL WinMain(
				HINSTANCE 	hinstCurrent,
				HINSTANCE 	hinstPrevious,
   				LPSTR  		lpszCmdLine,
   				int    		nCmdShow)
{
	int nReturnCode;
	WSADATA wsaData;
	#define MAJOR_VERSION_REQUIRED 1
	#define MINOR_VERSION_REQUIRED 1
	
	// Prepare version for WSAStartup()
	WORD wVersionRequired = MAKEWORD(MAJOR_VERSION_REQUIRED,
								     MINOR_VERSION_REQUIRED
									 );
    
	// Initialize the WinSock DLL
	nReturnCode = WSAStartup(wVersionRequired, &wsaData);
	if (nReturnCode != 0 ) 
	{
		MessageBox(NULL,"Error on WSAStartup()",
					"CheckMail", MB_OK);
    	return 1;
	}
	
	// Confirm that the version requested is available.
	if (wsaData.wVersion != wVersionRequired)
	{
    	// Version needed is not available.
		MessageBox(NULL,"Wrong WinSock Version",
					"CheckMail", MB_OK);
    	WSACleanup();
    	return 1; 
	}

	// Use a dialog box for our main window
	DialogBox(
			  hinstCurrent, 
			  MAKEINTRESOURCE(IDD_DIALOG_MAIN),
			  NULL, MainDialogProc);
	
	// Release WinSock DLL
	WSACleanup();
    return 0;
}


//
// MainDialogProc() - Main Window Procedure
//
BOOL CALLBACK MainDialogProc(
					HWND hwndDlg,
   					UINT msg,
   					WPARAM wParam,
   					LPARAM lParam)
{
	BOOL fRet = FALSE;  
	
	switch(msg)
	{           
		case WM_INITDIALOG:               
			// Display info from WSADATA
			Display("----- STARTUP -----\r\n");
			Display("WSAStartup() succeeded\r\n\r\n");
			break;
				
		case WM_COMMAND:
			switch(wParam)
			{ 
				// User clicked button to check mail
				case ID_CHECKMAIL:  
					// Retrieve POP info
              		// Server Name, User ID and Password
					if (!GetDlgItemText(hwndDlg, 
										IDC_SERVERNAME,
										gszServerName, 
										sizeof(gszServerName)
										))
					{         
						MessageBox(hwndDlg,
							"Please enter a server name",
							"POP Info", MB_OK);
						break;
					}
					if (!GetDlgItemText(hwndDlg, IDC_USERID,
						gszUserId, sizeof(gszUserId)))
					{         
						MessageBox(hwndDlg,
							"Please enter a user ID",
							"POP Info", MB_OK);
						break;
					}
					if (!GetDlgItemText(hwndDlg, 
										IDC_PASSWORD,
										gszPassword, 
										sizeof(gszPassword)
										))
					{         
						MessageBox(hwndDlg,
							"Please enter a password",
							"POP Info", MB_OK);
						break;
					}

					// Request a lookup of the host name
					// using the asynchronous version
					// of gethostbyname()
					// SM_GETHOST message will be posted
					// to this  window when it completes.
					Display("----- FIND HOST -----\r\n");
					Display("Calling WSAAsyncGetHostByName()"
							" to find server\r\n");
					// Time WSAAsyncGetHostByName() function for
					// example purposes.
					gdwTicks = GetTickCount();
					hndlGetHost = WSAAsyncGetHostByName(
										hwndDlg, SM_GETHOST,
										gszServerName,
										bufHostEnt,
										MAXGETHOSTSTRUCT);
					if (hndlGetHost == 0)
					{
						MessageBox(hwndDlg,
							"Error initiating "
							"WSAAsyncGetHostByName()",
							"CheckMail", MB_OK);
					}
					else
					{
						EnableButtons(FALSE);
						gnAppState = 0;
					}
					fRet = TRUE;
					break;
					
				// User pressed cancel button on main window
				case IDCANCEL:
					if (gnAppState)
						CloseSocket(socketPOP);
					PostQuitMessage(0);
					fRet = TRUE;
					break;
			}
			break;

			// Handle asynchronous gethostbyname() return
			case SM_GETHOST:
				HandleGetHostMsg(hwndDlg, wParam, lParam);
				fRet = TRUE;
				break;

			// Hanlde asynchronous messages   
			case SM_ASYNC:  
				HandleAsyncMsg(hwndDlg, wParam, lParam);
				fRet = TRUE;
				break;
	}
	return fRet;
}

//
// HandleGetHostMsg()
// Called when WSAAsyncGetHostByName() completes
void HandleGetHostMsg(
			HWND hwndDlg, 
			WPARAM wParam, 
			LPARAM lParam)
{
	SOCKADDR_IN saServ;		// Socket address for Internet 
	LPHOSTENT	lpHostEnt;	// Pointer to host entry
	LPSERVENT	lpServEnt;	// Pointer to server entry
	int nRet;				// Return code
	
	Display("SM_GETHOST message received\r\n");
	if ((HANDLE)wParam != hndlGetHost)
		return;        
		
	// For example purposes, display elapsed time
	// for WSAGetHostByName() function
	gdwElapsed = (GetTickCount() - gdwTicks);
	wsprintf((LPSTR)gszTemp,
			(LPSTR)"WSAAsyncGetHostByName() took %ld "
			" milliseconds to complete\r\n", 
			gdwElapsed);
	Display(gszTemp);

	// Check error code
	nRet = WSAGETASYNCERROR(lParam);
	if (nRet)
	{       
		wsprintf((LPSTR)gszTemp,
			(LPSTR)"WSAAsyncGetHostByName() error: %d\r\n", 
			nRet);
		Display(gszTemp);
		EnableButtons(TRUE);
		return;
	}     
 
	// Server found OK, bufHostEnt
	// contains server info
	Display("Server found OK\r\n\r\n");
	
	Display("----- CONNECT TO HOST -----\r\n");	
	// Create a socket                          
	Display("Calling socket(AF_INET, SOCK_STREAM, 0);\r\n");
	socketPOP = socket(AF_INET, SOCK_STREAM, 0);
	if (socketPOP == INVALID_SOCKET)
	{
		Display("Could not create a socket\r\n");
		EnableButtons(TRUE);
		return;
	}
	
	// Make socket non-blocking and register
	// for asynchronous notification
	Display("Calling WSAAsyncSelect()\r\n");
	if (WSAAsyncSelect(socketPOP, hwndDlg, SM_ASYNC,
				FD_CONNECT|FD_READ|FD_WRITE|FD_CLOSE))
	{         
		Display("WSAAsyncSelect() failed\r\n");
		EnableButtons(TRUE);
		return;
	}

	// Try to resolve the port number
	// with getservbyname()
	// Simpler synchronous version used since
	// it almost always completes quickly.
	// Time getservbyname() function for
	// example purposes.
	gdwTicks = GetTickCount();
	lpServEnt = getservbyname("pop3", "tcp");
	gdwElapsed = (GetTickCount() - gdwTicks);
	wsprintf((LPSTR)gszTemp,
			(LPSTR)"getservbyname() took %ld milliseconds"
			" to complete\r\n", 
			gdwElapsed);
	Display(gszTemp);
	
	// If servent not found
	if (lpServEnt == NULL) 
	{
	   // Fill in with well-known port
		saServ.sin_port = htons(110);
		Display("getservbyent() failed. Using port 110\r\n");
	}
	else
	{
		// Use port returned in servent
		saServ.sin_port = lpServEnt->s_port;
	}
	// Fill in the server address structure
	saServ.sin_family = AF_INET;
	lpHostEnt = (LPHOSTENT)bufHostEnt;
	saServ.sin_addr = *((LPIN_ADDR)*lpHostEnt->h_addr_list);
	// Connect the socket
	Display("Calling connect()\r\n");
	nRet = connect(socketPOP, 
				(LPSOCKADDR)&saServ, 
				sizeof(SOCKADDR_IN));
	if (nRet == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
			Display("Error connecting\r\n");
			EnableButtons(TRUE);
			return;
		}         
	}
	gnAppState = STATE_CONNECTING;
	Display("\r\n----- PROCESS MESSAGES -----\r\n");
}


void HandleAsyncMsg(
				    HWND hwndDlg, 
					WPARAM wParam, 
					LPARAM lParam)
{           
	int nBytesRead;
	
	switch(WSAGETSELECTEVENT(lParam))
	{
		case FD_CONNECT:
			Display("FD_CONNECT notification received\r\n");
			break;   
			
		case FD_WRITE:
			Display("FD_WRITE notification received\r\n");
			break;
			
		case FD_READ:
			Display("FD_READ notification received\r\n");
			Display("Calling recv()\r\n");       
			// Receive waiting data
			nBytesRead = recv(
				socketPOP,  		// socket
				gbufRecv, 			// buffer
				sizeof(gbufRecv), 	// max len to read
				0);					// recv flags
			// Check recv() return code
			if (nBytesRead == 0)
			{
				// Connection has been closed.
				MessageBox(hwndDlg, 
					"Connection closed unexpectedly",
					"recv() error", MB_OK);
				break;
			}
			if (nBytesRead == SOCKET_ERROR)
			{
				wsprintf((LPSTR)gszTemp,
					 (LPSTR)"recv() error: %d", nBytesRead);
				MessageBox(
					hwndDlg,
					gszTemp,
					"recv() error",
					MB_OK);
				break;
			}
			// Null terminate the buffer
			gbufRecv[nBytesRead] = '\0';
			// And pass it to be interpreted
			ProcessData(hwndDlg, gbufRecv, nBytesRead);
			break;
		case FD_CLOSE:
			Display("FD_CLOSE notification received\r\n");
			EnableButtons(TRUE);
			break;
	}
}


//
// ProcessData()
// Read incoming data and break it
// into lines
//
void ProcessData(HWND hwndDlg, LPSTR lpBuf, int nBytesRead)
{
	static char szResponse[512];
	static int nLen = 0;
	char *cp;
                         
	// Is there enough room in our buffer for the new data?
	if ((nLen + nBytesRead) > sizeof(szResponse))
	{
		Display("!!!!! Buffer overrun, data truncated\r\n");
		nLen = 0;
		szResponse[0] = '\0';
		return;
	}                           
	// Append the new data to our buffer
	strcat(szResponse, lpBuf);
	nLen = strlen(szResponse);

	// Process all full lines
	while(1)
	{
		// Does the buffer contain a full line?
		cp = strchr(szResponse, '\n');
		if (cp == NULL)
			break;
		// We have a CR/LF pair
		// Replace the LF with a NULL
		*cp = '\0';
		// And pass it to the ProcesLine() function
		ProcessLine(hwndDlg, szResponse);
		// Move the remaining data to the front
		// of the buffer
		cp++;
		if (strlen(cp))
			memmove(szResponse, cp, strlen(cp)+1);
		else
			szResponse[0] = '\0';
	}
}          

//
// ProcessLine()
// Handle response lines from server
// and decide what to do next.
//
void ProcessLine(HWND hwndDlg, LPSTR lpStr)
{                      
	int nRet;
	long lCount;
	long lSize;

	Display("Response from server:\r\n");
	Display(lpStr);
	Display("\r\n");
		
	// Check response for error
	if (lpStr[0] == '-')
	{
		Display("Negative response: ");
		switch(gnAppState)
		{
			case STATE_CONNECTING:
				Display("Connection denied\r\n");
				break;
			case STATE_USER:
				Display("Unknown UserID\r\n");
				break;
			case STATE_PASS:
				Display("Wrong Password\r\n");
				break;
			case STATE_STAT:
				Display("STAT command not supported\r\n");
				break;                                    
			case STATE_QUIT:
				Display("QUIT command not supported\r\n");
				break;                                    
		}
		Display("Sending QUIT\r\n");
		wsprintf(gszTemp, "QUIT\r\n");
		Display(gszTemp);
		nRet = send(
				socketPOP,  		// socket
				gszTemp, 			// data buffer
				strlen(gszTemp),	// length of data
				0);					// send flags
		gnAppState = STATE_QUIT;
		return;
	}	                          

	// We have a positive response
	switch(gnAppState)
	{
		case STATE_CONNECTING:
			// Send the USER portion of the login
			// request and set the app state
			Display("AppState = CONNECTING, "
			        "sending USER\r\n");
			wsprintf(gszTemp, "USER %s\r\n", gszUserId);
			nRet = send(
					socketPOP,		// socket
					gszTemp,		// data buffer
					strlen(gszTemp),// length of data
					0);				// send flags
			gnAppState = STATE_USER;
			break;
			
		case STATE_USER:
			// Send the PASSword portion of the login
			// request and set the app state
			Display("AppState = USER, sending PASS\r\n");
			wsprintf(gszTemp, "PASS %s\r\n", gszPassword);
			nRet = send(
					socketPOP,		// socket
					gszTemp,		// data buffer
					strlen(gszTemp),// length of data
					0);				// send flags
			gnAppState = STATE_PASS;
			break;
			
		case STATE_PASS:
			// Send the STAT command
			// request and set the app state
			Display("AppState = PASS, sending STAT\r\n");
			wsprintf(gszTemp, "STAT\r\n");
			nRet = send(
					socketPOP,		// socket
					gszTemp,		// data buffer
					strlen(gszTemp),// length of data
					0);				// send flags
			gnAppState = STATE_STAT;
			break;
			
		case STATE_STAT:
			// Read the STAT response
			// and print the results
			Display("AppState = STAT, reading response\r\n");
			sscanf(lpStr, "%s %ld %ld", 
				   gszTemp, &lCount, &lSize);
			Display("----- RESULT -----\r\n");
			wsprintf(gszTemp, "%ld messages %ld bytes\r\n", 
					lCount, lSize);
			Display(gszTemp);
			// Send the QUIT command
			// and set the app state
			Display("Sending QUIT\r\n");
			wsprintf(gszTemp, "QUIT\r\n");
			nRet = send(
					socketPOP,  		// socket
					gszTemp, 			// data buffer
					strlen(gszTemp),	// length of data
					0);					// send flags
			gnAppState = STATE_QUIT;
			break;                 
			
		case STATE_QUIT:
			Display("Host QUIT OK\r\n");
			Display("Closing socket\r\n");
			CloseSocket(socketPOP);
	}	
}


//
// CloseSocket()
// Closes a socket after an attempt to clear the 
// protocol stack buffers
//
void CloseSocket(SOCKET sock)
{   
	int nRet;
	char szBuf[255];
	
	// Tell the remote that we're not going
	// to send any more data
	shutdown(sock, 1);
	while(1)
	{
		// Try to receive data.
		// This clears any data that
		// may still be buffered in
		// the protocol stack
		nRet = recv(
				sock,			// socket
				szBuf,			// data buffer
				sizeof(szBuf),	// length of buffer
				0);				// recv flags
		// Stop receiving data if connection
		// closed or on any other error.
		if (nRet == 0 || nRet == SOCKET_ERROR)
			break;
	}
	// Tell the peer that we're not
	// going to receive any more either.
	shutdown(sock, 2);
	// Close the socket
	closesocket(sock);
}