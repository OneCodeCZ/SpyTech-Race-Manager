///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2005 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2005 06
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#pragma once
#include "WsfOrmModelClass.h"

/*****************************************************************************/
class WSL_API CWsfOrmModel  
{
protected:   
   CMap<CString, LPCSTR, CWsfOrmModelClassPtr, CWsfOrmModelClassPtr&> m_mapNameToClass;
   CMap<CRuntimeClass*, CRuntimeClass*, CWsfOrmModelClassPtr, CWsfOrmModelClassPtr&> m_mapRuntimeClassToClass;

public:
	CWsfOrmModel();

   // Adds a new class to the model
   CWsfOrmModelClass* AddClass(const CString& sClassName, const CString& sTableName) throw (CException*);

   // Clears the model
   void RemoveAll();

   // Empty = there is no class in the model
   BOOL IsEmpty() const { return m_mapNameToClass.IsEmpty(); }

   // Returns information on the given class. NULL if no such a class.
   CWsfOrmModelClass* GetClass(const CString& sName) const;
   CWsfOrmModelClass* GetClass(CRuntimeClass* pRuntimeClass) const;

   // Class name -> CRuntimeClass*
   CRuntimeClass* GetRuntimeClass(const CString& sClass);

   // Checks if the mapping is valid (tables, names) and if all stored objects are valid.
   void CheckMapping(CWslDatabase& db) throw (CException*);
};
