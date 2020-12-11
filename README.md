# WinSock 2.0 (1997) source code

## Introduction

This is an unofficial source code repo for Lewis Napper's [WinSock 2.0 (1997)](https://dl.acm.org/doi/book/10.5555/522582) book. The source code is reproduced here for reference (so you don't need to dig up the physical media of the book) and remains the copyright of the book authors.

## Using this repository

To build, you'll need Visual Studio 2019 (any version) with the _Desktop development with C++_ workload installed. Some samples also use MFC, so you'll also need the _C++ MFC for latest v142 build tools (x86 & x64)_ component installed.

## Changes

I'm currently going through the code, restructuring the directories, and adding Visual Studio 2019 projects to build the projects (converted projects are listed below). As this is decades old Win32 code, I don't plan on fixing compiler or security warnings, but if you do copy any code from this repository, it's strongly advised you heed and understand these warnings.

## Conversion status

The conversion effort is ongoing and will take some time. Items listed below are successfully converted samples.

<table>
  <thead>
    <tr>
      <th>Chapter</th>
      <th>Sample</th>
      <th>Conversion notes</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td align="center" rowspan="5">2</td>
      <td><a href="https://github.com/yottaawesome/winsock-2.0/tree/master/src/Ch02/DataGram">Datagram client and server</a></td>
      <td>OK.</td>
    </tr>
    <tr>
      <td><a href="https://github.com/yottaawesome/winsock-2.0/tree/master/src/Ch02/GetHTTP">GetHTTP</a></td>
      <td>OK.</td>
    </tr>
    <tr>
      <td><a href="https://github.com/yottaawesome/winsock-2.0/tree/master/src/Ch02/HostInfo">HostInfo</a></td>
      <td>OK.</td>
    </tr>
    <tr>
      <td><a href="https://github.com/yottaawesome/winsock-2.0/tree/master/src/Ch02/wsver">WsVer</a></td>
      <td>OK.</td>
    </tr>
    <tr>
      <td><a href="https://github.com/yottaawesome/winsock-2.0/tree/master/src/Ch02/Stream">Stream client and server</a></td>
      <td>OK.</td>
    </tr>
    <tr>
      <td align="center">4</td>
      <td><a href="https://github.com/yottaawesome/winsock-2.0/tree/master/src/Ch04">Chekmail</a></td>
      <td>OK. Requires MFC.</td>
    </tr>
    <tr>
      <td align="center" rowspan="2">5</td>
      <td><a href="https://github.com/yottaawesome/winsock-2.0/tree/master/src/Ch05/Ping">Ping</a></td>
      <td>OK.</td>
    </tr>
    <tr>
      <td><a href="https://github.com/yottaawesome/winsock-2.0/tree/master/src/Ch05/PingI">PingI</a></td>
      <td>OK.</td>
    </tr>
    <tr>
      <td align="center" rowspan="2">6</td>
      <td><a href="https://github.com/yottaawesome/winsock-2.0/tree/master/src/Ch06/SendMail">SendMail</a></td>
      <td>OK. Disabled an MFC call to address compiler warning C4996.</td>
    </tr>
    <tr>
      <td><a href="https://github.com/yottaawesome/winsock-2.0/tree/master/src/Ch06/WSTERM">Wsterm</a></td>
      <td>OK. Updated <code>WSTERM.RC2</code> resource file.</td>
    </tr>
    <tr>
      <td align="center" rowspan="2">7</td>
      <td><a href="https://github.com/yottaawesome/winsock-2.0/tree/master/src/Ch07/EnumProt">EnumProt</a></td>
      <td>
        <ul>
          <li>
            The <code>ws2dnet.h</code> header, which defines types for the now defunct <a href="https://en.wikipedia.org/wiki/DECnet">DECnet protocol suite</a>, is no longer part of the Windows SDK. The references to <code>ws2dnet.h</code> and its types have been commented out.
          </li>
          <li>
            After the conversion, the build process complained about a missing bitmap file <code>bitmap1.bmp</code>. I'm not sure what this file is about, and I can't find any reference to it in the repository, so I added a dummy bitmap to make the build happy.
          </li>
          <li>
            The ordering of the inclusion of <code>Windows.h</code> and <code>winsock2.h</code> was changed to prevent redefinitions.
          </li>
        </ul>
      </td>
    </tr>
    <tr>
      <td><a href="https://github.com/yottaawesome/winsock-2.0/tree/master/src/Ch07/SelProto">SelProto</a></td>
      <td>OK.</td>
    </tr>
    <tr>
      <td align="center">8</td>
      <td><a href="https://github.com/yottaawesome/winsock-2.0/tree/master/src/Ch08/Services">Services</a></td>
      <td>
      <ul>
          <li>
            The <code>ws2dnet.h</code> header, which defines types for the now defunct <a href="https://en.wikipedia.org/wiki/DECnet">DECnet protocol suite</a>, is no longer part of the Windows SDK. The references to <code>ws2dnet.h</code> and its types have been commented out.
          </li>
          <li>
            Windows NT 4.0 (<code>_WIN32_WINNT 0x400</code>) is no longer supported by MFC, and so MFC would not build against this version. I've bumped the version to Windows 10.
          </li>
          <li>
            Disabled an MFC call to address compiler warning C4996.
          </li>
        </ul>
      </td>
    </tr>
    <tr>
      <td align="center" rowspan="2">9</td>
      <td><a href="https://github.com/yottaawesome/winsock-2.0/tree/master/src/Ch09/GETHTTP2">GetHttp2</a></td>
      <td>OK. Changed <code>main()</code> signature.</td>
    </tr>
    <tr>
      <td><a href="https://github.com/yottaawesome/winsock-2.0/tree/master/src/Ch09/Gethttp3">GetHttp3</a></td>
      <td>OK. Changed <code>main()</code> signature.</td>
    </tr>
    <tr>
      <td align="center">10</td>
      <td><a href="https://github.com/yottaawesome/winsock-2.0/tree/master/src/Ch10">httpa</a></td>
      <td>OK. There's actually a bug in this sample where requesting for a file in the hosted directory causes a 404 due to an extraneous "/" being prepended to the file name -- I may eventually fix this.</td>
    </tr>
    <tr>
      <td align="center">11</td>
      <td><a href="https://github.com/yottaawesome/winsock-2.0/tree/master/src/Ch11">httpmt</a></td>
      <td>OK.</td>
    </tr>
  </tbody>
</table>

## Additional resources

* [MSDN WinSock2 docs](https://docs.microsoft.com/en-us/windows/win32/winsock/windows-sockets-start-page-2)
