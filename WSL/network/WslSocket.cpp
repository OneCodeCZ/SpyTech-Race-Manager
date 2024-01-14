///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  WSL - SpyTech C++ Library
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2004-2005 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  http://www.spytech.cz 
// | H |  _||   |
// |___|________|  Last changed: 2005 02
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WslSocket.h"
#include "..\generic\WslWin32Exception.h"

/*****************************************************************************/
CWslStreamSocket::CWslStreamSocket()
{
   m_hSocket = INVALID_SOCKET;
   m_hEvent = NULL;
   m_bCanReceive = m_bCanSend = m_bIsClosed = false;
}

/*****************************************************************************/
CWslStreamSocket::CWslStreamSocket(SOCKET hSocket)
{
   ASSERT(hSocket);
   m_hSocket = hSocket;

   m_hEvent = NULL;
   m_bCanReceive = m_bCanSend = m_bIsClosed = false;

   // Create an event for close/read/write

   m_hEvent = CreateEvent(NULL, false, false, NULL);
   if (!m_hEvent) throw new CWslWin32Exception();

   if (WSAEventSelect(m_hSocket, m_hEvent, FD_READ|FD_WRITE|FD_CLOSE) == SOCKET_ERROR)
   {
      CloseHandle(m_hEvent);
      throw new CWslSocketException();
   }
}

/*****************************************************************************/
CWslStreamSocket::~CWslStreamSocket()
{
   Close();
}

/*****************************************************************************/
void CWslStreamSocket::Listen(int nPort) throw (CWslException*)
{
   if (IsOpened()) throw new CWslTextException("Cannot listen, the socket is already used!");

   // Create

   m_hSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
   if (!IsOpened()) throw new CWslSocketException();

   try
   {
      // Bind

	   sockaddr_in a;
	   ZeroMemory(&a, sizeof(a));
	   a.sin_family = AF_INET;
	   a.sin_port = htons(nPort);
	   a.sin_addr.S_un.S_addr = htonl(INADDR_ANY);   

      if (bind(m_hSocket, (sockaddr*) &a, sizeof(a)) == SOCKET_ERROR)
         throw new CWslSocketException();

      // Listen

      if (listen(m_hSocket, SOMAXCONN) == SOCKET_ERROR)
         throw new CWslSocketException();   

      // Event for accept

      if (!(m_hEvent = CreateEvent(NULL, false, false, NULL))) 
         throw new CWslWin32Exception();

      if (WSAEventSelect(m_hSocket, m_hEvent, FD_ACCEPT) == SOCKET_ERROR)
         throw new CWslSocketException();
   }

   catch (...)
   {
      Close();
      throw;
   }
}

/*****************************************************************************/
CWslStreamSocketPtr CWslStreamSocket::Accept() throw (CWslException*)
{
   if (!IsOpened()) throw new CWslTextException("Cannot accept a connection, the socket is not valid!");
   ASSERT(m_hEvent);

   SOCKET s = accept(m_hSocket, NULL, NULL);
   if (s == SOCKET_ERROR) throw new CWslSocketException();

   CWslStreamSocket* p = NULL;
   
   try
   {
      p = new CWslStreamSocket(s);
   }
   catch (...)
   {
      delete p;
      closesocket(s);
      throw;
   }

   return p;
}

/*****************************************************************************/
void CWslStreamSocket::Connect(const CWslString& sAddr, int nPort) throw (CWslException*)
{
   if (IsOpened()) throw new CWslTextException("Cannot connect, the socket is already used!");

   // Create

   m_hSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
   if (!IsOpened()) throw new CWslSocketException();

   try
   {
      // Bind

	   sockaddr_in a;
	   ZeroMemory(&a, sizeof(a));
	   a.sin_family = AF_INET;
	   a.sin_port = 0;
	   a.sin_addr.s_addr = htonl(INADDR_ANY);

      if (bind(m_hSocket, (sockaddr*) &a, sizeof(a)) == SOCKET_ERROR)
         throw new CWslSocketException();

      // Event for connect/close/read/write

      if (!(m_hEvent = CreateEvent(NULL, false, false, NULL))) 
         throw new CWslWin32Exception();

      if (WSAEventSelect(m_hSocket, m_hEvent, FD_CONNECT|FD_READ|FD_WRITE|FD_CLOSE) == SOCKET_ERROR)
         throw new CWslSocketException();

      // Connect

      ZeroMemory(&a, sizeof(a));
	   a.sin_family = AF_INET;
	   a.sin_port = htons(nPort);
	   a.sin_addr.s_addr = inet_addr(sAddr);

      if (connect(m_hSocket, (sockaddr*)&a, sizeof(a)) == SOCKET_ERROR)
      {
         if (WSAGetLastError() != WSAEWOULDBLOCK)
            throw new CWslSocketException();
      }
   }

   catch (...)
   {
      Close();
      throw;
   }
}

/*****************************************************************************/
void CWslStreamSocket::Shutdown(BOOL bShutdownReceive, BOOL bShutdownSend) throw (CWslException*)
{
   if (!IsOpened()) return;

   int n = bShutdownReceive ? 
      (bShutdownSend ? SD_BOTH : SD_RECEIVE) :
      SD_SEND;
      
   if (shutdown(m_hSocket, n) == SOCKET_ERROR) throw new CWslSocketException();
}

/*****************************************************************************/
void CWslStreamSocket::Close()
{
   if (!IsOpened()) return;

   closesocket(m_hSocket); 
   m_hSocket = INVALID_SOCKET;

   if (m_hEvent) { CloseHandle(m_hEvent); m_hEvent = NULL; }
}

/*****************************************************************************/
int CWslStreamSocket::Receive(CWslArray<BYTE>& aBuffer) throw (CWslException*)
{
   if (!IsOpened()) throw new CWslTextException("Cannot receive data from an invalid socket!");
   
   unsigned long lToDo;

   if (ioctlsocket(m_hSocket, FIONREAD, &lToDo) == SOCKET_ERROR)
     throw new CWslSocketException();

   int nOldSize = aBuffer.GetSize();
   aBuffer.GrowBy(lToDo);

   int nReceived = recv(m_hSocket, (char*) aBuffer.GetData() + nOldSize, lToDo, 0);
   if (nReceived == SOCKET_ERROR) throw new CWslSocketException();

   // This should be true, but sometimes it is not :-(
   //ASSERT(nReceived == (int) lToDo);  

   aBuffer.SetSize(nOldSize + nReceived);

   return nReceived;
}

/*****************************************************************************/
int CWslStreamSocket::Send(const void* pData, int nByteCount) throw (CWslException*)
{
   if (!IsOpened()) throw new CWslTextException("The socket is invalid!");

   // PROBLEM: When the size passed to send() is too large (cca 80MB),
   // it returns the 10055 error
   // SOLUTION: We pass data to send() in smaller chunks

   int nSentCount = 0;
   const BYTE* pbyBuffer = (const BYTE*) pData;
   int nToDo = nByteCount;

   while (nToDo)
   {
      int n = send(m_hSocket, 
         (const char*) pbyBuffer,
         min(nToDo, 1024*1024), 
         0);
   
      //TRACE1("CWslStreamSocket::Send(): Sent %d bytes in one send().\n", n);

      if (n == SOCKET_ERROR) 
      {
         if (WSAGetLastError() == WSAEWOULDBLOCK) break;
         throw new CWslSocketException();
      }

      nToDo -= n;
      nSentCount += n;
      pbyBuffer += n;
   }

   return nSentCount;
}

/*****************************************************************************/
void CWslStreamSocket::GetEvents(CEvents& e) throw (CWslException*)
{
   if (!IsOpened()) throw new CWslTextException("The socket is invalid!");

   // Get events

   WSANETWORKEVENTS aEvents;

   if (WSAEnumNetworkEvents(m_hSocket, m_hEvent, &aEvents) == SOCKET_ERROR)
      throw new CWslSocketException();

   // Errors

   if ((aEvents.lNetworkEvents & FD_CONNECT) && aEvents.iErrorCode[FD_CONNECT_BIT])
      throw new CWslSocketException(aEvents.iErrorCode[FD_CONNECT_BIT]);

   if ((aEvents.lNetworkEvents & FD_READ) && aEvents.iErrorCode[FD_READ_BIT])
      throw new CWslSocketException(aEvents.iErrorCode[FD_READ_BIT]);

   if ((aEvents.lNetworkEvents & FD_WRITE) && aEvents.iErrorCode[FD_WRITE_BIT])
      throw new CWslSocketException(aEvents.iErrorCode[FD_WRITE_BIT]);

   if ((aEvents.lNetworkEvents & FD_CLOSE) && aEvents.iErrorCode[FD_CLOSE_BIT])
      throw new CWslSocketException(aEvents.iErrorCode[FD_CLOSE_BIT]);

   // Store

   e.m_bReadEvent = aEvents.lNetworkEvents & FD_READ;
   e.m_bWriteEvent = aEvents.lNetworkEvents & FD_WRITE;
   e.m_bCloseEvent = aEvents.lNetworkEvents & FD_CLOSE;
   e.m_bConnectEvent = aEvents.lNetworkEvents & FD_CONNECT;
}









