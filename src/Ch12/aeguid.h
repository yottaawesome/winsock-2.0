//
// AEGUID.H		GUID for Socket Adapter ECHO Example
//

#ifndef _AEGUID_H
#define _AEGUID_H

#define AE_SERVICECLASSNAME "SocketAdapter ECHO"
#define AE_INSTANCENAME		"SocketAdapter ECHO Server v1"
#define AE_UPDPORT			7
#define AE_SAPID			0x2FFF
#define AE_IPXPORT			0x2FFF

// {FB649400-119F-11d1-85E2-444553540000}
static GUID guidAdapterEcho = 
{ 0xfb649400, 0x119f, 0x11d1, { 0x85, 0xe2, 0x44, 0x45, 0x53, 0x54, 0x0, 0x0 } };

#endif _AEGUID_H
