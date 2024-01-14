///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  WSL - CWslException, CWslTextException
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)1999-2003 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2003 08
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

// CWslException
//   Generic exception. Cannot be used directly. Use derived classes.
//   CWslException must be compatible with MFC CException!!
//
// CWslTextException 
//   Generic exception whose description is provided on construction 
//	 by a text message.

#pragma once

#ifndef WSL_API
#define WSL_API
#endif

#include "WslString.h"

#ifndef _MFC_VER

#include "WslObject.h"

//*****************************************************************************
// WSL Version
class WSL_API CWslException : public CWslObject
{
protected:
   BOOL m_bAutoDelete;
   
public:
   // Set bAutoDelete to TRUE if the exception is allocated on the heap.
   CWslException(BOOL bAutoDelete = true);
protected: 
   // Call Delete() for object destruction.
   virtual ~CWslException();
public:
   
   // Destroys the object if the m_bAutoDelete memeber is true
   void Delete();
   
   // Retrieving an error description.
   virtual CWslString GetErrorMsg() = 0;
};

#else

/*****************************************************************************/
// MFC Version
class WSL_API CWslException : public CException
{
public:
   CWslException(BOOL bAutoDelete = true) : CException(bAutoDelete) { };
   
   // Retrieving an error description.
   virtual CWslString GetErrorMsg() = 0;
   
   // Returns a message explaining the exception.
   // NOTE: For MFC compatibility! If you want the code to work
   // under WSL and MFC use this method to get the message, not the one above!
   // NOTE: This method is not exactly the same as in MFC, but
   // it most cases it works OK.
   // Returns FALSE if there was an error while getting the
   // error message. However pszMsg is always defined upon return!!!.
   // NOTE: This implementation calls GetErrorMsg(), so you do not
   // need to implement this method.
   virtual BOOL GetErrorMessage(LPTSTR lpszError, 
      UINT nMaxError, 
      PUINT pnHelpContext = NULL)
   {
      ASSERT(nMaxError > 0);
      CWslString s = GetErrorMsg();
      strncpy(lpszError, s, nMaxError);
      lpszError[nMaxError-1] = 0;
      return true;
   }
};

#endif


//*****************************************************************************
class WSL_API CWslTextException : public CWslException
{
protected:
   CWslString m_sText;
   
public:
   // AutoDelete is always TRUE! Use Delete() method to delete the object!
   CWslTextException(const char* pcszFormat, ...);
   CWslTextException(int nResId, ...);
   
   virtual ~CWslTextException() { }
   virtual CWslString GetErrorMsg() { return m_sText; };
};






