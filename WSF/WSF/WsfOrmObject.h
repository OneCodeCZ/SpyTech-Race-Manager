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
class CWsfOrm;
class IWsfOrmPropertyPersistance;
class CWsfOrmModelClass;

/*****************************************************************************/
class WSL_API CWsfOrmObject : public CCmdTarget
{
protected:
   class WSL_API CRuntimeClassInfo
   {
   public:
      // ORM properties
      // The order must be the same as in the ORM model
      CWslArray<IWsfOrmPropertyPersistance*> m_aProperties;

   public:
      void Load(CWslDbRecordset& rs, CWsfOrmModelClass& ModelClass, CWsfOrm& Orm) throw (CException*);
      void Save(CWslDbCommand& cmd, CWsfOrmModelClass& ModelClass) throw (CException*);
   };

protected:
   enum EStates { CPP_CONSTRUCTED, CREATED, LOADED, DELETED, OFFLINE };

protected:
   CWsfOrm* m_pOrm;
   int      m_nId;   
   EStates  m_eState;

   // Runtime information on the class inheritance structure and on all ORM properties
   // - This is here because C++ doesn't support runtime class information by default
   // - It's a down -> top class sequence, which must be the same as in the ORM model
   CWslPtrArray<CRuntimeClassInfo> m_aClasses;

protected:
   CWsfOrmObject(); 
public:  
   // NOTE: DECLARE_SERIAL registers the class in the application
   // class list. DECLARE_DYNCREATE does not!
   DECLARE_SERIAL(CWsfOrmObject)

   //---------------------------------------------------------------------------
   // Construction
   //---------------------------------------------------------------------------

   // Call when the object is properly constructed
   // and can be stored into the DB == is a valid DB object
   void Constructed() throw (CException*);

   //---------------------------------------------------------------------------
   // API
   //---------------------------------------------------------------------------
public:   

   // Returns the ORM object responsible for this object
   CWsfOrm& GetOrm() { ASSERT(m_pOrm); return *m_pOrm; }

   // Object's ID in the DB
   int GetId() const { return m_nId; }
   
   // True, if the object was created and is not saved yet
   BOOL IsNew() const { return m_eState==CREATED; }

   // Returns TRUE if this object represents a valid DB object
   BOOL IsValid() const { ASSERT_VALID(this); return m_eState==CREATED || m_eState==LOADED; }

   // Deletes the object in the DB and invalidates 
   // the object in memory (IsValid()==false)
   // You can override the method and chech if the object can be deleted.
   // If it cannot, throw an exception.
   virtual void Delete() throw (CException*);

   // Returns the ORM pointer to this object.
   // NOTE: Use this method for retrieving this object, not the THIS keyword!
   CWsfOrmObjectPtr GetPtr() throw (CException*);

   //---------------------------------------------------------------------------
   // Internal API
   //---------------------------------------------------------------------------

   // Initialization after the dynamic create.
   void Init(CWsfOrm& Orm, int nId);

   // Registers properties and relationships with the ORM model
   // Builds the runtime class info (m_aClasses)
   virtual void RegisterPropertiesAndRelationships(CWsfOrm& orm) throw (CException*) { }

   // Save the object's state to the DB
   void Save() throw (CException*);

   // Loads the object from a recordset
   void Load(CWslDbRecordset& rs) throw (CException*);
   
   // Loads the object from a DB. 
   // Returns FALSE if the object doesn't exist.
   BOOL Load(CWslDatabase& db) throw (CException*);

   // Returns object model class
   class CWsfOrmModelClass* GetModelClass();

   // Removes the object from the ORM object space and makes it available 
   // outside the transaction.
   // - An offline object cannot be save to the DB.
   // - Note: do not use object's pointer properties because the lazy loading 
   //   cannot work in offline. It would cause app to crash!
   void MakeOffline();

protected:

   // Used be the ORM_MAP to build the runtime class info
   void AddClass();
   void AddProperty(IWsfOrmPropertyPersistance& PropertyPersistance);
};


/*****************************************************************************/
#define DECLARE_ORM_MAP() virtual void RegisterPropertiesAndRelationships(CWsfOrm& orm) throw (CException*) ;

/*****************************************************************************/
#define BEGIN_ORM_MAP(THIS_CLASS, PARENT_CLASS, sTableName) \
   void THIS_CLASS::RegisterPropertiesAndRelationships(CWsfOrm& orm) \
   { \
      ASSERT(m_eState==CPP_CONSTRUCTED);\
      \
      PARENT_CLASS::RegisterPropertiesAndRelationships(orm); \
      \
      bool bRegWithModel = orm.GetModel().GetClass(#THIS_CLASS) == NULL; \
      CWsfOrmModelClass* pModelClass; \
      \
      if (bRegWithModel) pModelClass = orm.GetModel().AddClass(#THIS_CLASS, sTableName); \
      AddClass();

/*****************************************************************************/
#define ORM_PROPERTY(property, column) \
   if (bRegWithModel) pModelClass->AddProperty(column); \
   AddProperty(property);

/*****************************************************************************/
#define ORM_RELATIONSHIP(property, column, MASTER_CLASS, bCascadeOnDelete) \
   if (bRegWithModel) \
   { \
      CWsfOrmModelClass* pMasterModelClass = orm.GetModel().GetClass(#MASTER_CLASS); \
      if (!pMasterModelClass) throw new CWslTextException("ORM_RELATIONSHIP(): The master class '%s' is not added to the model! Add it first.", #MASTER_CLASS); \
      pModelClass->AddRelationship(column, *pMasterModelClass, bCascadeOnDelete); \
   } \
   \
   AddProperty(property);

/*****************************************************************************/
#define END_ORM_MAP() }
      
   
      


