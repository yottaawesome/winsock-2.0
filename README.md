# WinSock 2.0 (1997) source code

## Introduction

This is an unofficial source code repo for Lewis Napper's [WinSock 2.0 (1997)](https://dl.acm.org/doi/book/10.5555/522582) book. The source code is reproduced here for reference (so you don't need to dig up the physical media of the book) and remains the copyright of the book authors.

## Using this repository

To build, you'll need Visual Studio 2019 (any version) with the _Desktop development with C++_ workload installed. Some samples also use MFC, so you'll also need the _C++ MFC for latest v142 build tools (x86 & x64)_ component installed.

## Changes

I'm currently going through the code, restructuring the directories, and adding Visual Studio 2019 projects to build the projects (converted projects are listed below). As this is decades old Win32 code, I don't plan on fixing compiler or security warnings, but if you do copy any code from this repository, it's strongly advised you heed and understand these warnings.

## Conversion status

The conversion effort is ongoing and will take some time. Items listed below are successfully converted samples.

* **Chap02**
  * [Datagram client and server](https://github.com/yottaawesome/winsock-2.0/tree/master/src/Ch02/DataGram)
  * [GetHTTP](https://github.com/yottaawesome/winsock-2.0/tree/master/src/Ch02/GetHTTP)
  * [HostInfo](https://github.com/yottaawesome/winsock-2.0/tree/master/src/Ch02/HostInfo)
  * [WsVer](https://github.com/yottaawesome/winsock-2.0/tree/master/src/Ch02/wsver)
  * [Stream client and server](https://github.com/yottaawesome/winsock-2.0/tree/master/src/Ch02/Stream)
* **Chap04**
  * [Chekmail](https://github.com/yottaawesome/winsock-2.0/tree/master/src/Ch04)
* **Chap05**
  * [Ping](https://github.com/yottaawesome/winsock-2.0/tree/master/src/Ch05/Ping)
  * [PingI](https://github.com/yottaawesome/winsock-2.0/tree/master/src/Ch05/PingI)
* **Chap06**
  * [SendMail](https://github.com/yottaawesome/winsock-2.0/tree/master/src/Ch06/SendMail)
  * [Wsterm](https://github.com/yottaawesome/winsock-2.0/tree/master/src/Ch06/WSTERM)
* **Chap09**
  * [GetHttp2](https://github.com/yottaawesome/winsock-2.0/tree/master/src/Ch09/GETHTTP2)
  * [GetHttp3](https://github.com/yottaawesome/winsock-2.0/tree/master/src/Ch09/Gethttp3)

## Additional resources

* [MSDN WinSock2 docs](https://docs.microsoft.com/en-us/windows/win32/winsock/windows-sockets-start-page-2)
