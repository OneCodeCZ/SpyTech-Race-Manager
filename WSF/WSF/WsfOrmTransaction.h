///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2005 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2005 05
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

/*****************************************************************************/
class WSL_API CWsfOrmTransaction
{
private:
   class CWsfOrm& m_Orm;
   bool           m_bFinished;         // true = commited or rollbacked
public:
   DWORD          m_dwDurationInMS;

public:
   // NOTE: Doesn't not start a transaction, because Begin may be overloaded!
   CWsfOrmTransaction(CWsfOrm& Orm);

   // Rollbacks the transaction if not commited
   virtual ~CWsfOrmTransaction();

   // Starts a transaction
   virtual void Begin() throw (CException*);

   // Commits the transaction
   virtual void Commit() throw (CException*);

   // Rolls back the transaction
   // NOTE: If you implement your own Rollback(), you must call it
   // from the destructor!
   virtual void Rollback();

   bool IsFinished() const { return m_bFinished; }
};
