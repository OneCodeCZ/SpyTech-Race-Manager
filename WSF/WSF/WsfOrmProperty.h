///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2004 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2004 08
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#pragma once
class CWsfOrm;

// An ORM class property 
// - can be NULL
// - modified flag
// - DB load/store 

template<class TYPE, class TYPE_ARG> class CWsfOrmProperty;

/*****************************************************************************
*
* DEFAULT PROPERTY TYPES
*
*****************************************************************************/

typedef CWsfOrmProperty<CString, const CString&> CWsfOrmString;
inline void WsfOrmSetNullValue(CString& v) { v.Empty(); }

typedef CWsfOrmProperty<int, int> CWsfOrmInt;
typedef CWsfOrmProperty<int, int> CWsfOrmBool;
inline void WsfOrmSetNullValue(int& v) { v = 0; }

typedef CWsfOrmProperty<CWslDate, const CWslDate&> CWsfOrmDate;
inline void WsfOrmSetNullValue(CWslDate& v) { v.SetNull(); }

typedef CWsfOrmProperty<CWslDateTime, const CWslDateTime&> CWsfOrmDateTime;
inline void WsfOrmSetNullValue(CWslDateTime& v) { v.SetNull(); }


/*****************************************************************************
*
* PROPERTY
*
*****************************************************************************/

/*****************************************************************************/
class IWsfOrmPropertyPersistance
{
public:
   virtual void Load(CWslDbRecordset& rs, const CString& sColumn, CWsfOrm& Orm) throw (CException*) = 0;
   virtual void Save(CWslDbCommand& c, const CString& sColumn) throw (CException*) = 0;
};


/*****************************************************************************/
template<class TYPE, class TYPE_ARG> class CWsfOrmProperty : public IWsfOrmPropertyPersistance
{
protected:
   TYPE  m_Value;
   BOOL  m_bModified;
   BOOL  m_bNull;

public:
   CWsfOrmProperty() { m_bModified = false; m_bNull = true; WsfOrmSetNullValue(m_Value); }

   //---------------------------------------------------------------------------
   // Value
   //---------------------------------------------------------------------------

   // Retrieving the value.
   // Do not use if it's NULL.
   operator TYPE_ARG() const { return (TYPE_ARG) m_Value; }

   // Setting the value
   void operator = (TYPE_ARG v) 
   { 
      if (m_bNull || m_Value != v) 
      {
         m_bModified = true; 
         m_bNull = false;
         m_Value = v;
      }
   }

   // Null
   BOOL IsNull() const { return m_bNull; }
   void SetNull() { m_bNull = true; m_bModified = true; WsfOrmSetNullValue(m_Value); }

   //---------------------------------------------------------------------------
   // Modified flag 
   //---------------------------------------------------------------------------

   void SetModified(BOOL bModified) { m_bModified = bModified; }
   BOOL IsModified() const { return m_bModified; }

   //---------------------------------------------------------------------------
   // Comparsion
   //---------------------------------------------------------------------------

   bool operator ==(const CWsfOrmProperty<TYPE, TYPE_ARG>& p) const
   {
      return (m_bNull && p.m_bNull) || (!m_bNull && !p.m_bNull && m_Value==p.m_Value);
   }

   bool operator !=(const CWsfOrmProperty<TYPE, TYPE_ARG>& p) const
   {
      return !operator ==(p);
   }

   //---------------------------------------------------------------------------
   // IWsfOrmPropertyPersistance
   //---------------------------------------------------------------------------

   // Loads a value from a DB. 
   virtual void Load(CWslDbRecordset& rs, const CString& sColumn, CWsfOrm& Orm) throw (CException*)
   {
      ASSERT(!sColumn.IsEmpty());

      CWslVariant v;
      rs.GetFieldValue(sColumn, v);
      m_bNull = v.IsNull();

      if (m_bNull)
         WsfOrmSetNullValue(m_Value);
      else 
         (*this) = (TYPE_ARG) v;
      
      m_bModified = false;
   }

   // Saves the value to a DB
   virtual void Save(CWslDbCommand& c, const CString& sColumn) throw (CException*)
   {
      ASSERT(!sColumn.IsEmpty());

      if (m_bModified) 
      {
         if (m_bNull)
            c.AddCol(sColumn);
         else
            c.AddCol(sColumn, m_Value);
         m_bModified = false;
      }
   }
};




