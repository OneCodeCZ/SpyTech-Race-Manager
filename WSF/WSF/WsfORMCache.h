///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2004-2005 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2005 06
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

/*****************************************************************************/
class CWsfOrmCache  
{
protected:
   CMap<int, int, CWsfOrmObjectPtr, CWsfOrmObjectPtr&> m_mapId2Obj;

public:
	CWsfOrmCache();
	virtual ~CWsfOrmCache();

   void Add(CWsfOrmObjectPtr& pObject);
   void Remove(int nId);

   CWsfOrmObjectPtr Get(int nId);

   void Save(CWslDatabase& db) throw (CException*);
   void Empty();

   BOOL IsEmpty() const { return m_mapId2Obj.IsEmpty(); }
};


          