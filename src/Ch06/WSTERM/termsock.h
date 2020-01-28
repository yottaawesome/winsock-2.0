//
// TERMSOCK.H -- Socket derived from CAsyncSocket
//

#ifndef __TERMSOCK_H__
#define __TERMSOCK_H__

class CTermSocket : public CAsyncSocket
{
	DECLARE_DYNAMIC(CTermSocket);

// Implementation
protected:
	virtual void OnConnect(int nErrorCode);
	virtual void OnSend(int nErrorCode);
	virtual void OnReceive(int nErrorCode);
	virtual void OnClose(int nErrorCode);
};
#endif // __TERMSOCK_H__
