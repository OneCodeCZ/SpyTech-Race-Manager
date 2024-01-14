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

#include "stdafx.h"
#include "main.h"
#include "WsfOrm.h"
#include "WsfOrmCache.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#ifdef _DEBUG
static int g_nCacheQueryCount, g_nCacheHitCount;
#endif

/*****************************************************************************/
CWsfOrmCache::CWsfOrmCache()
{
}

/*****************************************************************************/
CWsfOrmCache::~CWsfOrmCache()
{
   Empty();
}

/*****************************************************************************/
void CWsfOrmCache::Add(CWsfOrmObjectPtr& pObject)
{
   m_mapId2Obj.SetAt(pObject->GetId(), pObject);
}

/*****************************************************************************/
void CWsfOrmCache::Remove(int nId)
{
   CWsfOrmObjectPtr pObj;

   if (!m_mapId2Obj.Lookup(nId, pObj)) { ASSERT(false); return; }

   if (pObj->IsValid()) pObj->MakeOffline();
   m_mapId2Obj.RemoveKey(nId);
}

/*****************************************************************************/
CWsfOrmObjectPtr CWsfOrmCache::Get(int nId)
{
   CWsfOrmObjectPtr p;
   m_mapId2Obj.Lookup(nId, p);

#ifdef _DEBUG
   g_nCacheQueryCount++;
   if (p != NULL) g_nCacheHitCount++;
#endif
   
   return p;
}

/*****************************************************************************/
void CWsfOrmCache::Save(CWslDatabase& db) throw (CException*)
{
   int nId;
   CWsfOrmObjectPtr pObj;
   POSITION p = m_mapId2Obj.GetStartPosition();

   while (p)
   {
      m_mapId2Obj.GetNextAssoc(p, nId, pObj);
      if (pObj->IsValid()) pObj->Save();
   }
}

/*****************************************************************************/
void CWsfOrmCache::Empty()
{
   int nId;
   CWsfOrmObjectPtr pObj;
   POSITION p = m_mapId2Obj.GetStartPosition();

   while (p)
   {
      m_mapId2Obj.GetNextAssoc(p, nId, pObj);
      if (pObj->IsValid()) pObj->MakeOffline();
   }

   m_mapId2Obj.RemoveAll();

   // Statistics

#ifdef _DEBUG
   if (g_nCacheQueryCount)
   {
      TRACE("ORM: Cache efficiency: %.2f%%\n", (double)g_nCacheHitCount/g_nCacheQueryCount*100.0);
   }
   g_nCacheQueryCount = g_nCacheHitCount = 0;
#endif
}

