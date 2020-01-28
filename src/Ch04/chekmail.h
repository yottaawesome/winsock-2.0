//
// ChekMail.h	--	Check for mail waiting on a
//					POP3 server
//

// Define MAKEWORD macro in 16-bit Windows
#ifndef WIN32
#define MAKEWORD(a, b) \
			((WORD)(((BYTE)(a)) | ((WORD)((BYTE)(b))) << 8))
#endif

// ----- Helper macros
#define Display(s) SendDlgItemMessage(hwndDlg,IDC_EDIT1, \
							EM_REPLACESEL,0,(LPARAM)((LPSTR)s))
#define EnableOneButton(id, flag) EnableWindow( \
						GetDlgItem(hwndDlg, id),\
        				flag)
#define EnableButtons(flag) EnableOneButton(ID_CHECKMAIL, flag); 
						   

// ----- Messages for asynchronous notification                    
#define SM_GETHOST	WM_USER+1
#define SM_ASYNC	WM_USER+2

// ----- Function Prototypes
BOOL CALLBACK MainDialogProc(HWND, UINT, WPARAM, LPARAM);
void HandleGetHostMsg(HWND, WPARAM, LPARAM);
void HandleAsyncMsg(HWND, WPARAM, LPARAM);
void ProcessData(HWND, LPSTR, int);
void ProcessLine(HWND, LPSTR);
void CloseSocket(SOCKET);
