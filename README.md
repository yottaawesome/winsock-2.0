# WinSock 2.0

## Introduction

This is an unofficial source code repo for Lewis Napper's [WinSock 2.0](https://dl.acm.org/doi/book/10.5555/522582) (1997) book. The source code is reproduced here for reference (so you don't need to dig up the physical media of the book) and remains the copyright of the book authors.

## Using this repository

To build, you'll need Visual Studio 2019 (any version) with the _Desktop development with C++_ workload installed. Some samples also use MFC, so you'll also need the _C++ MFC for latest v142 build tools (x86 & x64)_ component installed.

## Changes

I'm currently going through the code, restructuring the directories, and adding Visual Studio 2019 projects to build the projects (converted projects are listed below). As this is decades old Win32 code, I don't plan on fixing compiler or security warnings, but if you do copy any code from this repository, it's strongly advised you heed and understand these warnings.

## Conversion status

The conversion effort is ongoing and will take some time.

* **Chap02**
  * [Datagram client and server](https://github.com/yottaawesome/winsock-2.0/tree/master/src/Ch02/DataGram)
  * [GetHTTP](https://github.com/yottaawesome/winsock-2.0/tree/master/src/Ch02/GetHTTP)
* **Chap04**
  * [Chekmail](https://github.com/yottaawesome/winsock-2.0/tree/master/src/Ch04)

## Additional resources

* [MSDN WinSock2 docs](https://docs.microsoft.com/en-us/windows/win32/winsock/windows-sockets-start-page-2)
