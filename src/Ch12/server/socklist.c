//
// SOCKLIST.C -- Linked list of SOCKETS and associated
//				 protocol information.
//

#include "echoserv.h"

////////////////////////////////////////////////////////////

//
// Head of linked list
// static for internal linkage
//
static LPSOCKETENTRY lpHead = NULL;

//
// Internal function
//
static void FreeSocketEntry(LPSOCKETENTRY lpEntry);

////////////////////////////////////////////////////////////


//
// AddSocketEntry()
//
// Allocates memory for a new SOCKETENTRY structure,
// adds it to the linked list,
// assigns the SOCKET to the SOCKETENTRY,
// fills in default field values,
// and returns a pointer to the new SOCKETENTRY.
//
LPSOCKETENTRY	AddSocketEntry(SOCKET Socket, 
					   LPWSAPROTOCOL_INFO lpInfo)
{
	LPSOCKETENTRY lpEntry = NULL;
	LPSOCKETENTRY lpThis  = NULL;

	//
	// Allocate memory from the heap for the new 
	// SOCKETENTRY structure
	//
	lpThis = (LPSOCKETENTRY)malloc(sizeof(SOCKETENTRY));
	if (lpThis == NULL)
		return(NULL);

	//
	// Allocate memory for a copy of the
	// SOCKADDR
	//
	lpThis->lpSockAddr = (LPSOCKADDR)malloc(lpInfo->iMaxSockAddr);
	if (lpThis->lpSockAddr == NULL)
	{
		free(lpThis);
		return(NULL);
	}

	// If the list is empty
	if (lpHead == NULL) 
	{
		// Put this one at the head
		lpHead = lpThis;
    }
	else 
	{
		// Walk to the end of the list
		lpEntry = lpHead;
		while(lpEntry->lpNext)
			lpEntry = lpEntry->lpNext;

		// And append this one to the end
		lpEntry->lpNext = lpThis;
    }
  
	//
	// Fill in the SOCKETENTRY structure
	//
    lpThis->Socket = Socket;
	lpThis->lpNext = NULL;
	memcpy(&lpThis->Info, lpInfo, sizeof(WSAPROTOCOL_INFO));
	// And return a pointer to it    
	return (lpThis);
}

////////////////////////////////////////////////////////////

//
// GetSocketEntry()
//
// Returns the SOCKETENTRY that Socket belongs to
//
LPSOCKETENTRY GetSocketEntry(SOCKET Socket) 
{
	LPSOCKETENTRY lpEntry;
  
	//
	// Walk through the list looking
	// for this socket
	//
	lpEntry = lpHead;
	while(lpEntry != NULL)
	{
		if (lpEntry->Socket == Socket)
			break;
		lpEntry = lpEntry->lpNext;
	}

	return(lpEntry);
}

////////////////////////////////////////////////////////////

//
// DelSocketEntry()
//
// Delete the SOCKETENTRY from the list
// and free associated memory
//
void DelSocketEntry(LPSOCKETENTRY lpThis) 
{
	LPSOCKETENTRY lpEntry;
	BOOL bRet = FALSE;

	//	
	// Search for "this"
	//

	// If This is the first member
	if (lpThis == lpHead) 
	{
		// This->Next becomes the head
		lpHead = lpThis->lpNext;
	}
	else 
	{  
		// Search the list
		for (lpEntry = lpHead; lpEntry; lpEntry = lpEntry->lpNext) 
		{
			// If This is next...
			if (lpEntry->lpNext == lpThis)
			{
				// Set the pointer to skip "this"
				lpEntry->lpNext = lpThis->lpNext;
				break;
			}
		}
	}
	// free the memory
	FreeSocketEntry(lpThis);
}

////////////////////////////////////////////////////////////

//
// GetFirstSocketEntry()
//
// Return the head of the list
// even if it is NULL
//
LPSOCKETENTRY	GetFirstSocketEntry(void)
{
	return(lpHead);
}

////////////////////////////////////////////////////////////

//
// GetNextSocketEntry()
//
// Return the next SOCKETENTRY after This
//
LPSOCKETENTRY	GetNextSocketEntry(LPSOCKETENTRY lpThis)
{
	return(lpThis->lpNext);
}

////////////////////////////////////////////////////////////

//
// DelAllSocketEntries()
//
// Delete all of the SOCKETENTRYs in the list
// and free all memory
//
void DelAllSocketEntries(void)
{
	LPSOCKETENTRY lpEntry;
	LPSOCKETENTRY lpNext;

	//
	// Walk through the list
	// freeing memory
	//
	for (lpEntry = lpHead; lpEntry; ) 
	{
		lpNext = lpEntry->lpNext;
		FreeSocketEntry(lpEntry);
		lpEntry = lpNext;
	}
	lpHead = NULL;
}

////////////////////////////////////////////////////////////

static void FreeSocketEntry(LPSOCKETENTRY lpEntry)
{
	free(lpEntry->lpSockAddr);
	free(lpEntry);
}
