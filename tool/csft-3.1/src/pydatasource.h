#ifndef _PY_DATASOURCE_H_
#define _PY_DATASOURCE_H_
#include "sphinx.h"

#if USE_PYTHON

#ifdef _DEBUG
#define D_E_B_U_G
#undef   _DEBUG
#endif
#include   <Python.h>    
#ifdef	D_E_B_U_G
#undef  D_E_B_U_G
#define _DEBUG
#endif

class CSphSource_Python : public CSphSource_Document
{
public:
					CSphSource_Python ( const char * sName );
					~CSphSource_Python ();		
	bool			Setup ( const CSphConfigSection & hSource, const char * sCommand);	
public:
	/// connect to the source (eg. to the database)
	/// connection settings are specific for each source type and as such
	/// are implemented in specific descendants
	virtual bool						Connect ( CSphString & sError );

	/// disconnect from the source
	virtual void						Disconnect ();
	/// check if there are any attributes configured
	/// note that there might be NO actual attributes in the case if configured
	/// ones do not match those actually returned by the source
	virtual bool						HasAttrsConfigured ();

	/// begin iterating document hits
	/// to be implemented by descendants
	virtual bool						IterateHitsStart ( CSphString & sError );

	/// begin iterating values of out-of-document multi-valued attribute iAttr
	/// will fail if iAttr is out of range, or is not multi-valued
	/// can also fail if configured settings are invalid (eg. SQL query can not be executed)
	virtual bool						IterateMultivaluedStart ( int iAttr, CSphString & sError );

	/// get next multi-valued (id,attr-value) tuple to m_tDocInfo
	virtual bool						IterateMultivaluedNext ();

	/// begin iterating values of multi-valued attribute iAttr stored in a field
	/// will fail if iAttr is out of range, or is not multi-valued
	virtual bool						IterateFieldMVAStart ( int iAttr, CSphString & sError );

	/// get next multi-valued (id,attr-value) tuple to m_tDocInfo
	virtual bool						IterateFieldMVANext ();

	virtual bool	IterateKillListStart ( CSphString & )			{ return false; }
	virtual bool	IterateKillListNext ( SphDocID_t & )			{ return false; }
	
	virtual BYTE **			NextDocument ( CSphString & sError );
	virtual void						PostIndex ();
public:
	static void CleanUp();
protected:
	int InitCoreSeekFullText();
    int LoadConfigFile(const char* sConfName);
	int InitDataSchema(const CSphConfigSection & hSource,const char* dsName);
	void SetupFieldMatch ( CSphColumnInfo & tCol );
	void AddFieldToSchema ( const char * szName, int iIndex );
	bool CheckResult(PyObject * pResult);
protected:
	PyObject * main_module;
	PyObject * builtin_module;
	PyObject * time_module;
	PyObject * m_pInstance;
	PyObject * m_pScheme;

	char* doc_id_col;
protected:
	BYTE *				m_dFields [ SPH_MAX_FIELDS ];
	SphDocID_t			m_uMaxFetchedID;///< max actually fetched ID

	CSphVector < CSphVector <DWORD> > m_dFieldMVAs;
	CSphVector < int >	m_dAttrToFieldMVA;
	int m_iFieldMVA ;
	int m_iFieldMVAIterator;
};

int Run_PySimpleFile(const char* sConfName);
#endif //USE_PYTHON

#endif

