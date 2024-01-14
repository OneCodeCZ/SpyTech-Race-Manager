///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  WSL Generic SQL Database Support
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2001-2004 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2004 01
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef WSL_API
#define WSL_API
#endif

class CWslDatabase;
class CWslVariant;
class CWslDbRecordset;

//*****************************************************************************
class WSL_API CWslDbCommand 
#ifdef _MFC_VER
: public CObject
#endif
{
friend CWslDatabase;
friend CWslDbRecordset;

public:
	struct CParam
	{
		CWslString	sName;
		CWslVariant	Value;
		long			nLen;
		BOOL			bInsertAsParam;
	};

public:
	CWslObArray<CWslPtr<CParam> >	m_aParams;
	CWslString	m_sSql;

public: 
	CWslDbCommand();
	virtual ~CWslDbCommand();

   //---------------------------------------------------------------------------
	// General
	//---------------------------------------------------------------------------
	
	// Sets SQL command.
	void SetSQL(const char* pcszSQL, ...);

   // Builds INSERT SQL command 
	void BuildInsert(const char* pcszTbl, BOOL bUseReplace = false);
	void BuildInsert(const char* pcszTbl, const char* pcszPrimaryKeyColumn, int nPrimaryKey);

   // Builds UPDATE SQL command
   void BuildUpdate(const char* pcszTbl, const char* pcszPrimaryKeyColumn, int nPrimaryKey);
	void BuildUpdate(const char* pcszTbl, const char* sWhere);

   // If bNew is TRUE, the INSERT SQL command is build.
   // Otherwise the UPDATE SQL is BUILD
   void BuildSave(BOOL bNew, 
      const char* pcszTbl, 
      const char* pcszPrimaryKeyColumn, 
      int nPrimaryKey);

	// Clears stored SQL command and all params.
	void Clear();

	// Returns TRUE if no SQL is set and there are no params defined.
	inline BOOL IsEmpty() const { return m_sSql.IsEmpty() && m_aParams.GetSize() == 0; }

  //---------------------------------------------------------------------------
	// Columns-data pair for bulding SQL commands
	//---------------------------------------------------------------------------

	// Adds column data. This data will be used in BuildXXXXX() methods.
	void AddCol(const char* pcszCol, const CWslString& sVal, BOOL bUseNULL=false);
	void AddCol(const char* pcszCol, const char* pcszVal, BOOL bUseNULL=false);
	void AddCol(const char* pcszCol, int nVal, BOOL bUseNULL=false, int nNullValue=0);
	void AddCol(const char* pcszCol, const CWslDate& d, BOOL bUseNULL=false);
	void AddCol(const char* pcszCol, const TIMESTAMP_STRUCT* pts);
	void AddCol(const char* pcszCol, const CWslVariant& v);
	void AddCol(const char* pcszCol, float fVal);
	void AddCol(const char* pcszCol, double dblVal);
	void AddCol(const char* pcszCol, const CWslDateTime& d);
	void AddCol(const char* pcszCol, bool bVal);
	void AddColExpression(const char* pcszCol, const char* pcszVal);

   // The column is set to NULL.
   void AddCol(const char* pcszCol);

	// NOTE: The data are not copied, so you MUST NOT release
	// the buffer after the call. The class overtakes the
	// responsibility for the pointer and the buffer will be automatically 
	// released.
	// NOTE: The buffer must be allocated using the NEW operator.
	void AddCol(const char* pcszCol, BYTE* pbyData, int nSize);

   int GetColCount() const { return m_aParams.GetSize(); }

   //---------------------------------------------------------------------------
	// Params data for parametric SQL commands ( ... WHERE id=? and s=? and v=?)
	//---------------------------------------------------------------------------

	// Adds data for parameter in SQL command.
	inline void AddParam(CWslString &sVal) { AddCol(NULL, sVal); }
	inline void AddParam(const char* pcszVal) { AddCol(NULL, pcszVal); }
	inline void AddParam(int nVal) { AddCol(NULL, nVal); }
	inline void AddParam(CWslDate &d) { AddCol(NULL, d); }
	inline void AddParam(TIMESTAMP_STRUCT *pts) { AddCol(NULL, pts); }
	inline void AddParam(CWslVariant &v) { AddCol(NULL, v); }
	inline void AddParam(double dVal) { AddCol(NULL, dVal); }

   int GetParamCount() const { return m_aParams.GetSize(); }

  //---------------------------------------------------------------------------
	// Serialization 
	//---------------------------------------------------------------------------
#ifdef _MFC_VER
	virtual void Serialize(CArchive& ar);
	DECLARE_SERIAL(CWslDbCommand)
#endif
};





