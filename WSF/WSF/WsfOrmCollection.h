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

// Items are loaded when first accessed!

/*****************************************************************************/
// TYPE must be a CWslOrmPtr<> class
template <class TYPE> class CWsfOrmCollection : public CCmdTarget
{
private:
   class CWsfOrm*    m_pOrm;
   BOOL              m_bLoaded;
   CWslObArray<TYPE> m_aObjects;
   
public:
   CWsfOrmCollection(CWsfOrm& Orm) { m_bLoaded = false; m_pOrm = &Orm; }
   CWsfOrmCollection() { m_bLoaded = true; m_pOrm = NULL; }

   // Empties the collection and reloads it when first accessed.
   void Refresh() { m_aObjects.RemoveAll(); m_bLoaded = false; }

   // TRUE if the collection is empty.
   BOOL IsEmpty() throw (CException*) { if (!m_bLoaded) Load(); return m_aObjects.IsEmpty(); }

   // Number of items in the collection
   int GetCount() throw (CException*) { if (!m_bLoaded) Load(); return m_aObjects.GetSize(); }

   // Returns a given collection item
   TYPE& operator [](int nIdx) throw (CException*) { if (!m_bLoaded) Load(); return m_aObjects[nIdx]; }
   TYPE& GetAt(int nIdx) throw (CException*) { if (!m_bLoaded) Load(); return m_aObjects[nIdx]; }

   //---------------------------------------------------------------------------
   // Building collection manually
   //---------------------------------------------------------------------------

   // Removes a given collection item
   void Append(TYPE& item) { m_bLoaded = true; m_aObjects.Append(item); }

   // Removes a given collection item
   void RemoveAt(int nIdx) { m_aObjects.RemoveAt(nIdx); }

   // Removes all items
   void RemoveAll() { m_bLoaded = true; m_aObjects.RemoveAll(); }

   //---------------------------------------------------------------------------
   // Loading collection - TO BE OVERRIDDEN
   //---------------------------------------------------------------------------
protected:

   // Called when items of this collection must be loaded
   protected_ virtual void OnLoad(CWslDatabase& db) throw (CException*) { ASSERT(false); }

   // Returns the OR-mapper.
   CWsfOrm& GetOrm() { ASSERT(m_pOrm); return *m_pOrm; }

   //---------------------------------------------------------------------------
   // Internals
   //---------------------------------------------------------------------------
private:
   void Load() throw (CException*)
   {
      ASSERT(m_pOrm);
      m_pOrm->PrepareForQuery();
      m_aObjects.RemoveAll();
      OnLoad(*m_pOrm->GetDb());
      m_bLoaded = true;
   }
};

