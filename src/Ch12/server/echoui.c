//
// ECHOUI.C		Datagram ECHO Server
//
//				Dynamically links to WS2_32.DLL or
//				if WinSock 2 isn't available, it
//				dynamically links to WSOCK32.DLL.
//
//				Protocol Independent under WinSock 2
//				Uses UDP on WinSock 1.1
//
//				Do NOT link to either 
//				WS2_32.LIB or WSOCK32.LIB
//

#include "echoserv.h"

TCHAR gszAppName[]  = _TEXT("SocketAdapter ECHO Server");

////////////////////////////////////////////////////////////

int WINAPI WinMain(HINSTANCE hInstance, 
				   HINSTANCE hPrevInstance,
				   LPSTR lpCmdLine,
				   int nCmdShow)
{
	int nVersion;
		
	//
	// Initialize WinSock
	//
	nVersion = DWSInitWinSock();
	if (!nVersion)
	{
		MessageBox(NULL, 
				   _TEXT("No WinSock Available"),
				   gszAppName, 
				   MB_OK);
		return 0;
	}

	//
	// Use new common controls
	//
	InitCommonControls();

	//
	// Use a dialog as a main window
	//
	DialogBox(hInstance,
			  MAKEINTRESOURCE(IDD_MAINWND),
			  NULL, 
			  MainWndProc);

	DWSFreeWinSock();
	return(0);
}

////////////////////////////////////////////////////////////

BOOL CALLBACK MainWndProc(HWND hwnd, 
						  UINT uMsg,
						  WPARAM wParam, 
						  LPARAM lParam) 
{
	switch (uMsg) 
	{
		HANDLE_DLG_MSG(hwnd, WM_INITDIALOG, OnInitDialog);
		HANDLE_DLG_MSG(hwnd, WM_COMMAND,	OnCommand);

		case UM_ASYNC:  
			HandleAsyncMsg(hwnd, wParam, lParam);
			return TRUE;
		case UM_ECHO:
		{
			switch(wParam)
			{
				case ECHO_STATS_MSG:
					UpdateStats(hwnd, (LPSTATS)lParam);
					break;
				case ECHO_EVENT_MSG:
					ShowEvent(hwnd, (LPCSTR)lParam);
					break;
			}
			return TRUE;
		}
	}
	return FALSE;
}

////////////////////////////////////////////////////////////

BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam) 
{
	LV_COLUMN col;
	HWND hwndList;
	TEXTMETRIC tm;

	//
	// Associate an icon with the dialog box.
	//
	SetClassLong(hwnd, 
				 GCL_HICON, 
				 (LONG)LoadIcon(GetWindowInstance(hwnd),
				 MAKEINTRESOURCE(IDI_APP)));
	if (!StartServer(hwnd, UM_ASYNC, UM_ECHO))
	{
		MessageBox(NULL, 
				   _TEXT("Could not start server"),
				   gszAppName, 
				   MB_OK);
	}

	//
	// Setup List Control
	//
	hwndList = GetDlgItem(hwnd, IDC_PROTOSTATS);
	GetTextMetrics(GetDC(hwnd), &tm);

	col.mask    = LVCF_FMT|LVCF_TEXT|LVCF_WIDTH;
	col.fmt     = LVCFMT_LEFT;
	col.pszText = _TEXT("Protocol");
	col.cx      = tm.tmAveCharWidth*15;
	ListView_InsertColumn(hwndList, 0, &col);

	col.fmt     = LVCFMT_RIGHT;
	col.pszText = _TEXT("Datagrams");
	col.cx      = tm.tmAveCharWidth*10;
	ListView_InsertColumn(hwndList, 1, &col);

	return(TRUE);
}

////////////////////////////////////////////////////////////

void OnCommand(HWND hwnd, int nId, HWND hwndCtl, UINT codeNotify) 
{
	switch (nId) 
	{
		case IDCANCEL:
			StopServer();
			EndDialog(hwnd, 0);
			break;
	}
}

////////////////////////////////////////////////////////////

void UpdateStats(HWND hwnd, LPSTATS lpStats)
{
	TCHAR szBuf[256];
	DWORD dwHitCount;
	LV_FINDINFO fi;
	LV_ITEM		lvItem;
	int nItem;
	static DWORD dwDatagrams = 0;
	static DWORD dwRecv = 0;
	static DWORD dwSend = 0;
	HWND hwndList = GetDlgItem(hwnd, IDC_PROTOSTATS);

	#define UPDATENUMBER(n, id)	\
		wsprintf(szBuf,_TEXT("%ld"), n); \
		SetDlgItemText(hwnd, id, szBuf);

	//
	// Update the totals
	//
	dwDatagrams += lpStats->dwDatagrams;
	dwRecv	    += lpStats->dwRecv;
	dwSend		+= lpStats->dwSend;

	UPDATENUMBER(dwDatagrams, IDC_DATAGRAMS);
	UPDATENUMBER(dwRecv, IDC_RECV);
	UPDATENUMBER(dwSend, IDC_SEND);

	//
	// Update the protocol specific totals
	// See if the item is already in the list
	//
	fi.flags = LVFI_STRING;
	fi.psz   = lpStats->pInfo->szProtocol;
	nItem = ListView_FindItem(hwndList, -1, &fi);

	//
	// If it is, increment the hit count
	//
	if (nItem != -1)
	{
		lvItem.mask = LVIF_TEXT;
		lvItem.iItem = nItem;
		lvItem.iSubItem = 1;
		lvItem.pszText = szBuf;
		lvItem.cchTextMax = sizeof(szBuf) / sizeof(TCHAR);
		if (ListView_GetItem(hwndList, &lvItem))
		{
			dwHitCount = _ttol(szBuf);
			dwHitCount++;
			wsprintf(szBuf, _TEXT("%ld"), dwHitCount);
			ListView_SetItem(hwndList, &lvItem);
			return;
		}
	}

	//
	// Add a new item to the list
	//
	lvItem.mask = LVIF_TEXT;
	lvItem.iItem = 0;
	lvItem.iSubItem = 0;
	lvItem.pszText = lpStats->pInfo->szProtocol;
	nItem = ListView_InsertItem(hwndList, &lvItem);
	lvItem.iItem = nItem;
	lvItem.iSubItem = 1;
	lvItem.pszText = "1";
	ListView_SetItem(hwndList, &lvItem);
}

////////////////////////////////////////////////////////////

void ShowEvent(HWND hwnd, LPCTSTR lpStr)
{
	TCHAR szBuf[284];
	SYSTEMTIME st;
	HWND hwndEvents = GetDlgItem(hwnd, IDC_EVENTLOG);

	GetLocalTime(&st);
	wsprintf(szBuf,_TEXT("%02d:%02d:%02d.%03d\t%s"),
			st.wHour,   st.wMinute,
			st.wSecond, st.wMilliseconds,
			lpStr);
	ListBox_AddString(hwndEvents, szBuf);
	if (ListBox_GetCount(hwndEvents) > 500)
		ListBox_DeleteString(hwndEvents, 0);
}
