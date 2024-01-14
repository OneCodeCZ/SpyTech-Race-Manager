
#ifndef __WslIniFile__
#define __WslIniFile__
#ifndef WSL_API
#define WSL_API
#endif

/*

  Supports INI files in this format:
  ----------------------------------
  //remark
  [  section_name  ]    //remark
  field_name=p1,p2,p3,,p5,"p 6"   //remark

*/

//******************************************************************************
//*** CWslInitFile Class *******************************************************
//******************************************************************************

class WSL_API CWslIniFile
{
private:
  CWslFile f;
  const char *m_pcszData;
  const char *m_pcszCurSecStart;
  DWORD m_dwSize;

public:
  CWslIniFile();
  ~CWslIniFile();

  // Open an INI file
  
  void Open(const char *pcszName);

  // Close

  void Close();

  // Sets active section in which fields will be searched

  bool SetSection(const char *pcszName);   

  // Sets active section with given name. Section will be
  // search from the current active section.

  bool NextSection(const char *pcszName);

  // Get value associated with given field. Field will be
  // search in the active section.

  char *GetValue(const char *pcszFieldName, 
                 char *pszBuffer, 
                 int pos = 0);        // Pos is counted from 0

  int GetValue(const char *pcszFieldName, 
               int pos = 0);

  // Set position to the top of the INI file. Subsequant call to NextSection
  // will set the first section with the given name from the top of the file.

  inline void ResetPos() { m_pcszCurSecStart = m_pcszData; }
};


#endif //__WslIniFile__
