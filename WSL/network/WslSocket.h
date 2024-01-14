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

#pragma once
#include "WslSocketException.h"
#include "..\generic\WslPtr.h"

// CWslSocket is a non-blocking socket

class CWslStreamSocket;
typedef CWslPtr<CWslStreamSocket> CWslStreamSocketPtr;

/*****************************************************************************/
class WSL_API CWslStreamSocket
{
public:
   class CEvents
   {
   public:
      BOOL m_bCloseEvent;
      BOOL m_bReadEvent;
      BOOL m_bWriteEvent;
      BOOL m_bConnectEvent;

   public:
      BOOL CanReceive() const { return m_bReadEvent; }
      BOOL CanSend() const { return m_bWriteEvent; }
      BOOL IsClosed() const { return m_bCloseEvent; }
      BOOL IsConnected() const { return m_bConnectEvent; }
   };

protected:
   BOOL     m_bServer;
   SOCKET   m_hSocket;
   HANDLE   m_hEvent;

   BOOL     m_bCanReceive;
   BOOL     m_bCanSend;
   BOOL     m_bIsClosed;

public:
   CWslStreamSocket();
   ~CWslStreamSocket();

   // Protected constructor for creating sockects for accepted connections.
   // If it fails, it's up to you to free the socket handle.
protected:
   CWslStreamSocket(SOCKET hSocket) throw (CWslException);
public:

   BOOL IsOpened() const { return m_hSocket != INVALID_SOCKET; }

   //---------------------------------------------------------------------------
   // Connection initiating
   //---------------------------------------------------------------------------

   // SERVER 1. STEP
   // Puts the socket to the state, where it listens for client
   // connections request.
   void Listen(int nPort) throw (CWslException*);

   // SERVER 2. STEP
   // Accepts an incomming connection and returns a new socket for that connection.
   // If there is no connection, NULL is returned.
   CWslStreamSocketPtr Accept() throw (CWslException*);

   // CLIENT
   // Connects the socket to a given server socket.
   // The socket is connect when the CONNECT event arrives.
   void Connect(const CWslString& sAddr, int nPort) throw (CWslException*);

   // BOTH
   // Disables the given operations on the socket and finishes any pendind operations.
   // You can close the socket after the CLOSE event arrives.  
   void Shutdown(BOOL bShutdownReceive, BOOL bShutdownSend) throw (CWslException*);

   // BOTH
   // Closes the socket immediatelly.
   void Close();

   //---------------------------------------------------------------------------
   // IO
   //---------------------------------------------------------------------------

   // Receives data and appends it at the end of the buffer
   // Returns the number of received bytes.
   // Returns ZERO if there are no data.
   // Data are available when the RECEIVE event arrives
   int Receive(CWslArray<BYTE>& aBuffer) throw (CWslException*);

   // Sends the given buffer. 
   // Returns the number of bytes sent or ZERO if no data could have been sent.
   // When ZERO is returned, you can send more data after the SEND event arrives.
   int Send(const void* pData, int nByteCount) throw (CWslException*);

   //---------------------------------------------------------------------------
   // EVENTS
   //---------------------------------------------------------------------------
   
   // Windows event that signals when some socket event is available
   HANDLE GetSystemEvent() const { return m_hEvent; }

   // Gets socket events
   void GetEvents(CEvents& e) throw (CWslException*);
};








