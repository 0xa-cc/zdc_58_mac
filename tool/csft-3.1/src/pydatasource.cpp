#include <iostream>
#include <fstream>

#include <exception>
#include <ostream>
#include <vector>
#include <sys/stat.h>
#include <sys/types.h>
#include <ctype.h>

#include "sphinx.h"
#include "sphinxutils.h"
#include "pydatasource.h"

#if USE_PYTHON

int FindDatasource(PyObject *pMain, std::vector<std::string> & datasource_list);
PyObject* GetObjectAttr(PyObject *pInst, char* name);

CSphSource_Python::CSphSource_Python ( const char * sName )
				 : CSphSource_Document ( sName ),m_uMaxFetchedID(0)
{
	//sName the data source's name
	main_module = NULL;
	builtin_module = NULL;
	time_module = NULL;
	m_pInstance = NULL;
	m_pScheme = NULL;
	doc_id_col = NULL;
	for(int i = 0; i< SPH_MAX_FIELDS; i++){
		m_dFields[i] = NULL;
	}
}

CSphSource_Python::~CSphSource_Python (){
	Disconnect ();
	//can not be in Disconnect for OnIndexFinished still needs this.
	if(m_pInstance) {
		Py_XDECREF(m_pInstance);
		m_pInstance = NULL;
	}
	if(m_pScheme) {
		Py_XDECREF(m_pScheme);
		m_pScheme = NULL;
	}
	if (time_module) { 
		Py_XDECREF(main_module);
		Py_XDECREF(builtin_module);
		Py_XDECREF(time_module);
		time_module = NULL;
	}
}

bool	CSphSource_Python::Setup ( const CSphConfigSection & hSource, const char * sCommand){
	std::vector<std::string>  datasource_list;
	int nCount = 0;
	//1 try  to init python env.
	if (!Py_IsInitialized()) {
		Py_Initialize();
		//PyEval_InitThreads();

		if (!Py_IsInitialized()) {
			return false;
		}
	}
	//FIXME: what's happen when needs read multi-python file
	try{
		InitCoreSeekFullText();
		if (LoadConfigFile(sCommand) !=0 )
			goto DONE;
	}catch (...) {
		PyErr_Print();
		PyErr_Clear(); //is function can be undefined
		goto DONE;
	}
	
	main_module = PyImport_AddModule("__main__");  //+1
	builtin_module =  PyImport_AddModule("__builtin__"); //+1
	time_module = PyImport_AddModule("time"); //+1

	if (!main_module) { goto DONE; }
	if (!builtin_module) { 
		Py_XDECREF(builtin_module);
		goto DONE; 
	}
	if (!time_module) { 
		Py_XDECREF(main_module);
		Py_XDECREF(builtin_module);
		goto DONE; 
	}
	//do schema read, ready to used.

	nCount =
		FindDatasource(main_module,datasource_list);
	/* //debug code
	std::cout <<"Found DataSource "<<nCount<<":"<<std::endl;
	for(int i=0;i<nCount;i++)
	{
		std::cout<<datasource_list[i].c_str()<<std::endl;
	}
	*/
	if(nCount == 0) {
		printf("No Data Source Define found.\n");
		return false;
	}
	/*
	if(nCount > 1) {
		printf("More than one Data Source found. Use 1st only.\n");
	}
	*/
	//load schema
	m_pScheme = PyObject_GetAttrString(main_module, "scheme"); //+1
	if (!m_pScheme)
	{
		Py_XDECREF(m_pScheme);
		m_pScheme = NULL;
		PyErr_Clear();
		return false;
	}
	{
		CSphString sName = hSource["python_name"];
		//std::string& ds_name = datasource_list[0];
		if(InitDataSchema(hSource, sName.cstr())!=0)
			return false;
	}
	return true;
DONE:
	return false;
}

void CSphSource_Python::CleanUp(){
	if (Py_IsInitialized()) {
		//to avoid crash in release mode.
#ifndef DEBUG
		try{
#endif // _DEBUG

			Py_Finalize();
#ifndef DEBUG
		}catch(...){
			return;
		}
#endif // _DEBUG

	}
}

/// connect to the source (eg. to the database)
/// connection settings are specific for each source type and as such
/// are implemented in specific descendants
bool	CSphSource_Python::Connect ( CSphString & sError ){
	//init the schema
	if (!m_pInstance) {
		return false;
	}
	//all condition meets
	m_tSchema.m_dFields.Reset ();
	{
		//enum all attrs
		PyObject* pArgs = NULL;
        PyObject* pResult = NULL; 
        PyObject* pFunc = PyObject_GetAttrString(m_pInstance, "getScheme"); // +1
        if (!pFunc)
            return NULL; //Next Document must exist

        if(!pFunc||!PyCallable_Check(pFunc)){
            Py_XDECREF(pFunc);
            return NULL;
        }
        pArgs  = Py_BuildValue("()");

        pResult = PyEval_CallObject(pFunc, pArgs);    
        Py_XDECREF(pArgs);
        Py_XDECREF(pFunc);

		//check dict type
		if(PyDict_Check(pResult)){
			PyObject *key, *value;
			PyObject *csft_string_const =  PyObject_GetAttrString(main_module, "csft_string_const");
			PyObject *csft_string_fulltext =  PyObject_GetAttrString(main_module, "csft_string_fulltext");
			PyObject *csft_number =  PyObject_GetAttrString(main_module, "csft_number");
			PyObject *csft_float =  PyObject_GetAttrString(main_module, "csft_float");
			PyObject *csft_boolean =  PyObject_GetAttrString(main_module, "csft_boolean");
			PyObject *csft_timestamp =  PyObject_GetAttrString(main_module, "csft_timestamp");
			PyObject *csft_list =  PyObject_GetAttrString(main_module, "csft_list");
			PyObject *csft_file_pdf =  PyObject_GetAttrString(main_module, "csft_file_pdf");
			PyObject *csft_file_office =  PyObject_GetAttrString(main_module, "csft_file_office");
			
			PyObject * typeKey = PyObject_GetAttrString(m_pScheme, "type");
			PyObject * docidKey = PyObject_GetAttrString(m_pScheme, "docid");
			
			int pos = 0; 
			int i = 0;
			while (PyDict_Next(pResult, &pos, &key, &value)) { //all borrowed.
				char* strkey = PyString_AsString(key);
				std::string skey(strkey);
				PyObject * propValue;
				//check docid
				propValue = PyDict_GetItem(value, docidKey); //+0
				if(propValue && CheckResult(propValue)){
					doc_id_col = strkey;
					continue;
				}

				propValue = PyDict_GetItem(value, typeKey); 

				if(propValue == csft_float){
					CSphColumnInfo tCol ( strkey, SPH_ATTR_FLOAT );
					tCol.m_iIndex = i; //m_tSchema.GetAttrsCount (); //should be i in pList?
					m_tSchema.AddAttr ( tCol );
				}else
				if(propValue == csft_number){
					CSphColumnInfo tCol ( strkey, SPH_ATTR_INTEGER );
					tCol.m_iIndex = i; //m_tSchema.GetAttrsCount (); //should be i in pList?
					m_tSchema.AddAttr ( tCol );
				}else
				if(propValue == csft_list){
					CSphColumnInfo tCol ( strkey, SPH_ATTR_INTEGER );
					tCol.m_iIndex = i; //m_tSchema.GetAttrsCount (); //should be i in pList?
					tCol.m_eAttrType = SPH_ATTR_INTEGER | SPH_ATTR_MULTI;
					tCol.m_eSrc = SPH_ATTRSRC_FIELD;
					m_tSchema.AddAttr ( tCol );
				}else
				if(propValue == csft_boolean){
					CSphColumnInfo tCol ( strkey, SPH_ATTR_BOOL );
					tCol.m_iIndex = i; //m_tSchema.GetAttrsCount (); //should be i in pList?
					m_tSchema.AddAttr ( tCol );
				}else
				if(propValue == csft_string_const){
					CSphColumnInfo tCol ( strkey, SPH_ATTR_ORDINAL );
					tCol.m_iIndex = i; //m_tSchema.GetAttrsCount (); //should be i in pList?
					m_tSchema.AddAttr ( tCol );
				}else
				if(propValue == csft_timestamp){
					CSphColumnInfo tCol ( strkey, SPH_ATTR_TIMESTAMP );
					tCol.m_iIndex = i; //m_tSchema.GetAttrsCount (); //should be i in pList?
					m_tSchema.AddAttr ( tCol );
				}else
				//if(propValue == csft_string_fulltext)
				{
					//default fulltext field
					AddFieldToSchema(skey.c_str(), i);
				}				
				//Py_XDECREF(propValue);
				i++;
			} //end while
			
			Py_XDECREF(typeKey);
			Py_XDECREF(docidKey);

			Py_XDECREF(csft_string_const);
			Py_XDECREF(csft_string_fulltext);
			Py_XDECREF(csft_number);
			Py_XDECREF(csft_float);
			Py_XDECREF(csft_boolean);
			Py_XDECREF(csft_timestamp);
			Py_XDECREF(csft_list);
			Py_XDECREF(csft_file_pdf);
			Py_XDECREF(csft_file_office);		
			
		}else{
			fprintf(stderr,"getScheme read error\n");
			PyErr_Clear();
			Py_XDECREF(pResult);
			return false;
		}
        Py_XDECREF(pResult);

		if(!doc_id_col) {
			fprintf(stderr,"Must set docid = True attribute in DataSource Scheme to declare document unique id\n");
			PyErr_Clear();
			return false;
		}

		/*
		char sBuf [ 1024 ];
		snprintf ( sBuf, sizeof(sBuf), "pysource(%s)", m_sCommand.cstr() );
		m_tSchema.m_sName = sBuf;
		*/

		m_tDocInfo.Reset ( m_tSchema.GetRowSize() );
	}
	//try to do connect
	{
		if (!m_pInstance)
		{
			PyErr_Print();
			return false;
		}else{
			PyObject* pArgs = NULL;
			PyObject* pResult = NULL; 
			PyObject* pFunc = PyObject_GetAttrString(m_pInstance, "Connected");
			if (!pFunc){
				fprintf(stderr,"You must have a 'Connected' function in DataSource.\n");
				PyErr_Clear(); 
				return false;
			}				 

			if(!pFunc||!PyCallable_Check(pFunc)){
				Py_XDECREF(pFunc);
				return false;
			}
			pArgs  = Py_BuildValue("()");

			pResult = PyEval_CallObject(pFunc, pArgs);    
			Py_XDECREF(pArgs);
			Py_XDECREF(pFunc);
			//check result
			if(!CheckResult(pResult)) {
				Py_XDECREF(pResult);
				return false;
			} 
			Py_XDECREF(pResult);
		}
	}
	return true;
}
bool CSphSource_Python::CheckResult(PyObject * pResult)
{
	if(!pResult) //no return default true
		return true;

	if(PyBool_Check(pResult)){
		if(Py_False == pResult)			
			return false;
	}
	if(PyInt_Check(pResult)) {
		if(PyInt_AsLong(pResult) == 0)
			return false;
	}
	return true;
}
/// disconnect from the source
void	CSphSource_Python::Disconnect (){
	m_tSchema.Reset ();
}
/// check if there are any attributes configured
/// note that there might be NO actual attributes in the case if configured
/// ones do not match those actually returned by the source
bool	CSphSource_Python::HasAttrsConfigured () {
	return true;
}

void	CSphSource_Python::PostIndex ()
{
	if (!m_pInstance)
	{
		PyErr_Print();
		goto DONE;
	}else{
		PyObject* pArgs = NULL;
		PyObject* pResult = NULL; 
		PyObject* pFunc = PyObject_GetAttrString(m_pInstance, "OnIndexFinished");
		if (!pFunc)
			PyErr_Clear(); //is function can be undefined

		if(!pFunc||!PyCallable_Check(pFunc)){
			Py_XDECREF(pFunc);
			goto DONE;
		}
		pArgs  = Py_BuildValue("()");

		pResult = PyEval_CallObject(pFunc, pArgs);    
		Py_XDECREF(pArgs);
		Py_XDECREF(pFunc);
		Py_XDECREF(pResult);
	}
DONE:
	return ;
}

/// begin iterating document hits
/// to be implemented by descendants
bool	CSphSource_Python::IterateHitsStart ( CSphString & sError ){
	int iFieldMVA = 0;	
	///TODO: call on_before_index function
	if (!m_pInstance)
	{
		PyErr_Print();
		goto DONE;
	}else{
		PyObject* pArgs = NULL;
		PyObject* pResult = NULL; 
		PyObject* pFunc = PyObject_GetAttrString(m_pInstance, "OnBeforeIndex");
		if (!pFunc)
			PyErr_Clear(); //is function can be undefined

		if(!pFunc||!PyCallable_Check(pFunc)){
			Py_XDECREF(pFunc);
			goto DONE;
		}
		pArgs  = Py_BuildValue("()");

		pResult = PyEval_CallObject(pFunc, pArgs);    
		Py_XDECREF(pArgs);
		Py_XDECREF(pFunc);

		if(!CheckResult(pResult)) {
			Py_XDECREF(pResult);
			return false;
		} 
		Py_XDECREF(pResult);
	}

	m_iFieldMVA = 0;
	m_iFieldMVAIterator = 0;
	m_dAttrToFieldMVA.Resize ( 0 );

	for ( int i = 0; i < m_tSchema.GetAttrsCount (); i++ )
	{
		const CSphColumnInfo & tCol = m_tSchema.GetAttr ( i );
		if ( ( tCol.m_eAttrType & SPH_ATTR_MULTI ) && tCol.m_eSrc == SPH_ATTRSRC_FIELD )
			m_dAttrToFieldMVA.Add ( iFieldMVA++ );
		else
			m_dAttrToFieldMVA.Add ( -1 );
	}

	m_dFieldMVAs.Resize ( iFieldMVA );
	ARRAY_FOREACH ( i, m_dFieldMVAs )
		m_dFieldMVAs [i].Reserve ( 16 );
DONE:
	return true;
}

/// begin iterating values of out-of-document multi-valued attribute iAttr
/// will fail if iAttr is out of range, or is not multi-valued
/// can also fail if configured settings are invalid (eg. SQL query can not be executed)
bool	CSphSource_Python::IterateMultivaluedStart ( int iAttr, CSphString & sError ){
	return true;
}

/// get next multi-valued (id,attr-value) tuple to m_tDocInfo
bool	CSphSource_Python::IterateMultivaluedNext (){
	return true;
}

/// begin iterating values of multi-valued attribute iAttr stored in a field
/// will fail if iAttr is out of range, or is not multi-valued
bool	CSphSource_Python::IterateFieldMVAStart ( int iAttr, CSphString & sError ){
	if ( iAttr<0 || iAttr>=m_tSchema.GetAttrsCount() )
		return false;

	if ( m_dAttrToFieldMVA [iAttr] == -1 )
		return false;

	m_iFieldMVA = iAttr;
	m_iFieldMVAIterator = 0;
	return true;
}

/// get next multi-valued (id,attr-value) tuple to m_tDocInfo
bool	CSphSource_Python::IterateFieldMVANext (){
	int iFieldMVA = m_dAttrToFieldMVA [m_iFieldMVA];
	if ( m_iFieldMVAIterator >= m_dFieldMVAs [iFieldMVA].GetLength () )
		return false;

	const CSphColumnInfo & tAttr = m_tSchema.GetAttr ( m_iFieldMVA );
	m_tDocInfo.SetAttr ( tAttr.m_tLocator, m_dFieldMVAs [iFieldMVA][m_iFieldMVAIterator] );

	++m_iFieldMVAIterator;
	return true;
}

BYTE **	CSphSource_Python::NextDocument ( CSphString & sError ){
	//1st call document to load the data into pyobject.
	//call on_nextdocument function in py side.
	//call get_docId function to get the DocID attr's name
	// __getattr__ not work with PyObject_GetAttrString
	//clean the m_dFields 's data
	ARRAY_FOREACH ( i, m_tSchema.m_dFields ) {
		if(m_dFields[i])
			free(m_dFields[i]);
		m_dFields[i] = NULL;
	}

	if(!doc_id_col) 
		return NULL; //no init yet!
	{
		//call next_document
		if (!m_pInstance)
		{
			PyErr_Print();
			return NULL;
		}else{
			PyObject* pArgs = NULL;
			PyObject* pResult = NULL; 
			PyObject* pFunc = PyObject_GetAttrString(m_pInstance, "NextDocument");
			if (!pFunc)
				return NULL; //Next Document must exist

			if(!pFunc||!PyCallable_Check(pFunc)){
				Py_XDECREF(pFunc);
				return NULL;
			}
			pArgs  = Py_BuildValue("()");

			pResult = PyEval_CallObject(pFunc, pArgs);    
			Py_XDECREF(pArgs);
			Py_XDECREF(pFunc);
			if(!pResult) {
				printf("Exception happens in python source.\n");
				m_tDocInfo.m_iDocID = 0;
				goto CHECK_TO_CALL_AFTER_INDEX;
			}

			if(!CheckResult(pResult)) {
				Py_XDECREF(pResult);
				m_tDocInfo.m_iDocID = 0;
				goto CHECK_TO_CALL_AFTER_INDEX;
				//return NULL; //if return false , the source finished
			} 
			Py_XDECREF(pResult);
			//We do NOT care about doc_id, but doc_id must be > 0
		}
	}
	{
		PyObject* pDocId = GetObjectAttr(m_pInstance, doc_id_col);
#if USE_64BIT
		m_tDocInfo.m_iDocID = PyInt_AsLong(pDocId));
#else
		m_tDocInfo.m_iDocID = (SphDocID_t)(PyInt_AsLong(pDocId));
#endif
		Py_XDECREF(pDocId);

		m_uMaxFetchedID = Max ( m_uMaxFetchedID, m_tDocInfo.m_iDocID );
	}
CHECK_TO_CALL_AFTER_INDEX:
	//check doc_id
	if(m_tDocInfo.m_iDocID <= 0)
	{
		//call sql_query_post
		PyObject* pArgs = NULL;
		PyObject* pResult = NULL; 
		PyObject* pFunc = PyObject_GetAttrString(m_pInstance, "OnAfterIndex");
		if (!pFunc)
			PyErr_Clear(); //is function can be undefined

		if(!pFunc||!PyCallable_Check(pFunc)){
			Py_XDECREF(pFunc);
			goto DONE;
		}
		pArgs  = Py_BuildValue("()");

		pResult = PyEval_CallObject(pFunc, pArgs)			;    
		Py_XDECREF(pArgs);
		Py_XDECREF(pFunc);
		Py_XDECREF(pResult); //we do not care about the result.
DONE:
		return NULL;
	}

	for ( int i=0; i<m_tDocInfo.m_iRowitems; i++ )
		m_tDocInfo.m_pRowitems[i] = 0;
	
	ARRAY_FOREACH ( i, m_tSchema.m_dFields ) {
		int nIdx = m_tSchema.m_dFields[i].m_iIndex;
		char* ptr_Name = (char*)m_tSchema.m_dFields[i].m_sName.cstr();
		PyObject* item = PyObject_GetAttrString(m_pInstance,ptr_Name);

		//PyList_GetItem(pList,m_tSchema.m_dFields[i].m_iIndex);
		//check as string?
		//nobody can CHANGE the buffer!!!
		//FIXME:should check is not Null.
		if(item && Py_None!=item && PyString_Check(item)) {
			char* data = PyString_AsString(item);
			//m_dFields[i] = (BYTE*)PyString_AsString(item); //error!!! this pointer might be move later.
			m_dFields[i] = (BYTE*)strdup(data);
		}
		else
			m_dFields[i] = NULL;
		Py_XDECREF(item);
	}

	int iFieldMVA = 0;
	for ( int i=0; i<m_tSchema.GetAttrsCount(); i++ ) {
		const CSphColumnInfo & tAttr = m_tSchema.GetAttr(i); // shortcut
		if ( tAttr.m_eAttrType & SPH_ATTR_MULTI )
		{
			m_tDocInfo.SetAttr ( tAttr.m_tLocator,0);
			if ( tAttr.m_eSrc == SPH_ATTRSRC_FIELD ) {
				//all the MVA fields in this data source is SPH_ATTRSRC_FIELD
				//deal the python-list
				PyObject* pList = PyObject_GetAttrString(m_pInstance, (char*)tAttr.m_sName.cstr());
				size_t size = PyList_Size(pList);
				m_dFieldMVAs [iFieldMVA].Resize ( 0 );
				for(size_t j = 0; j < size; j++) {
					//PyList_GetItem just a borrowed reference
					PyObject* item = PyList_GetItem(pList,j);
					long dVal =  0;
					if(item && (PyInt_Check(item)||PyLong_Check(item)))
						dVal = PyInt_AsLong(item);
					m_dFieldMVAs [iFieldMVA].Add ( (DWORD)dVal);
				}
				iFieldMVA++;
				Py_XDECREF(pList);
			}
			continue;
		}
		//normal attribute
		if(tAttr.m_eAttrType == SPH_ATTR_FLOAT){
			PyObject* item = PyObject_GetAttrString(m_pInstance, (char*)tAttr.m_sName.cstr());
			double dVal = 0.0;
			if(item && PyFloat_Check(item))
				dVal = PyFloat_AsDouble(item);
			m_tDocInfo.SetAttrFloat ( tAttr.m_tLocator, (float)dVal);
			Py_XDECREF(item);
		}else
		if(tAttr.m_eAttrType == SPH_ATTR_INTEGER){
			// just store as uint by default
			// FIXME: should check the return, and a warning.
			PyObject* item = PyObject_GetAttrString(m_pInstance, (char*)tAttr.m_sName.cstr());
			long dVal =  0;
			if(item && (PyInt_Check(item)||PyLong_Check(item)))
				dVal =  PyInt_AsLong(item);
			m_tDocInfo.SetAttr ( tAttr.m_tLocator,(DWORD)dVal);
			Py_XDECREF(item);
		}else 
		if(tAttr.m_eAttrType == SPH_ATTR_BOOL){
			// just store as uint by default
			PyObject* item = PyObject_GetAttrString(m_pInstance, (char*)tAttr.m_sName.cstr());
			long dVal =  (item == Py_True)?1:0;
			m_tDocInfo.SetAttr ( tAttr.m_tLocator,(DWORD)dVal);
			Py_XDECREF(item);
		}else 
		if(tAttr.m_eAttrType == SPH_ATTR_TIMESTAMP){
			// just store as uint by default
			PyObject* item = PyObject_GetAttrString(m_pInstance, (char*)tAttr.m_sName.cstr());
			//time stamp can be float and int
			long dVal = 0;
			if(item && PyLong_Check(item))
				dVal = PyLong_AsLong(item);
			if(item && PyFloat_Check(item))
				dVal = (long)PyFloat_AsDouble(item);
			m_tDocInfo.SetAttr (tAttr.m_tLocator,(DWORD)dVal);
			Py_XDECREF(item);
		}
	}
	return m_dFields;
}
// helper functions
void CSphSource_Python::SetupFieldMatch ( CSphColumnInfo & tCol )
{
	bool bPrefix = m_iMinPrefixLen > 0 && IsPrefixMatch ( tCol.m_sName.cstr () );
	bool bInfix =  m_iMinInfixLen > 0  && IsInfixMatch ( tCol.m_sName.cstr () );

	if ( bPrefix && m_iMinPrefixLen > 0 && bInfix && m_iMinInfixLen > 0)
	{
		fprintf (stderr,"field '%s' is marked for both infix and prefix indexing", tCol.m_sName.cstr() );
		return;
	}

	if ( bPrefix )
		tCol.m_eWordpart = SPH_WORDPART_PREFIX;

	if ( bInfix )
		tCol.m_eWordpart = SPH_WORDPART_INFIX;
}

void CSphSource_Python::AddFieldToSchema ( const char * szName , int iIndex)
{
	CSphColumnInfo tCol ( szName );
	SetupFieldMatch ( tCol );
	tCol.m_iIndex = iIndex; 
	m_tSchema.m_dFields.Add ( tCol );
}

int CSphSource_Python::InitDataSchema(const CSphConfigSection & hSource,const char* dsName) {
	
	PyObject* m_pTypeObj = PyObject_GetAttrString(main_module, (char*)dsName); //+1
	if (m_pTypeObj == NULL) {
		//m_sLastError.SetSprintf ( "docid==DOCID_MAX (source broken?)" );
		printf("Can NOT found datasource %s.\n", dsName);
		return 0;
	}

	if (!PyClass_Check(m_pTypeObj)) {
		Py_XDECREF(m_pTypeObj);
		return -1; //not a valid type file
	}
	
	if(!m_pTypeObj||!PyCallable_Check(m_pTypeObj)){
		Py_XDECREF(m_pTypeObj);
		return  -2;
	}else{
		PyObject* pConf = PyDict_New(); // +1
		hSource.IterateStart ();
		while ( hSource.IterateNext() ){
			//Add ( hSource.IterateGet(), hSource.IterateGetKey() );
			const char* key = hSource.IterateGetKey().cstr();
			const char* val = hSource.IterateGet().cstr();
			//hSource.IterateGet();
			PyDict_SetItemString(pConf, key, PyString_FromString(val));
			/*
			PyObject *pValue = PyDict_GetItemString(pConf, key);
			if(!pValue)
				PyDict_SetItemString(pConf, key, PyString_FromString(val));
			else{
				//check pValue type
				if (PyList_Check(pValue)){
					//append new value
					//FIXME: should check the result value is not -1
					PyList_Append(pValue, PyString_FromString(val));
				}
				if (PyString_Check(pValue)){
					//create newTuple
					PyObject* pArg = PyList_New(2);
					PyList_SetItem(pArg, 0, pValue);
					PyList_SetItem(pArg, 1, PyString_FromString(val));
					PyDict_SetItem(pConf, PyString_FromString(key), pArg);
				}				
			}
			*/
		}

		PyObject* pargs  = Py_BuildValue("O", pConf); //+1
		PyObject* pArg = PyTuple_New(1); //+1
		PyTuple_SetItem(pArg, 0, pargs);

		m_pInstance  = PyEval_CallObject(m_pTypeObj, pArg);   
		if(!m_pInstance){
			PyErr_Print();
			
			Py_XDECREF(pArg);
			Py_XDECREF(pargs);
			Py_XDECREF(m_pTypeObj);
			return -3; //source file error.
		}
		Py_XDECREF(pArg);
		Py_XDECREF(pargs);
		Py_XDECREF(pConf);
		
	}
	Py_XDECREF(m_pTypeObj);
	return 0;
}
int CSphSource_Python::InitCoreSeekFullText(){
	/*
	import sys
	sys.path.insert(0, '/tmp/example.zip')
	*/
	//init base type
	//PyRun_SimpleString("class DataSource:\n\tdef __init__(self):\n\t\tpass\n\n");
	//import base module.
	//PyRun_SimpleString("import time\n\n");
	PyRun_SimpleString("import sys\nsys.path.insert(0, '.')\n");
	return 0;
}

int Run_PySimpleFile(const char* sConfName) {
	char buf_pyfile[512] = {0};
	char* buf_ptr = buf_pyfile;
	const char* sConfName_ptr = sConfName;
	
	while(*sConfName_ptr && sConfName_ptr-sConfName <500) {
		*buf_ptr = *sConfName_ptr;
		if(*sConfName_ptr == '\\') {
			buf_ptr++;
			*buf_ptr = '\\';
		}
		sConfName_ptr++;
		buf_ptr++;
	}
	buf_ptr[0]=0;

	char buf[512] = {0};
	char buffer[780] = {0};
	//set the file variable
	snprintf(buffer,sizeof(buffer),"__file__='%s'",buf_pyfile);
	PyRun_SimpleString(buffer);
	PyRun_SimpleString("__g_cs__ =  globals()\n__g_cs__['__name__'] = '__coreseek__' \n");
	snprintf(buf,sizeof(buf),"execfile('%s', __g_cs__ )",buf_pyfile);
	int nRet = PyRun_SimpleString(buf); 
	PyRun_SimpleString("__g_cs__['__name__'] = '__main__' \n");
	if(nRet != 0){
		//have error
		//PyRun_SimpleString
	}
	return nRet;
}

int CSphSource_Python::LoadConfigFile(const char* sConfName){
	return Run_PySimpleFile(sConfName);
}

PyObject* GetObjectAttr(PyObject *pInst, char* name)
{
	PyObject* item = PyObject_GetAttrString(pInst, name);
	if(item)
		return item;
	PyObject* pFunc = PyObject_GetAttrString(pInst, "__getattr__");
	if(!pFunc)
		return NULL;
	PyObject* pArgsKey  = Py_BuildValue("(s)",name);
	PyObject* pResult = PyEval_CallObject(pFunc, pArgsKey);
	Py_XDECREF(pArgsKey);
	Py_XDECREF(pFunc);
	return pResult;
}

int FindDatasource(PyObject *pMain, std::vector<std::string> & datasource_list)
{

	PyObject* typeobj = NULL;

	PyObject * pList = PyObject_Dir(pMain); //list current locales +1
	if (!pList)
	{
		PyErr_Print();
		return -1;
	}

	//dump the list
	if (!PyList_Check(pList) )
	{
		Py_XDECREF(pList);
		return -2;
	}

	PyObject* basetypeobj = PyObject_GetAttrString(pMain, "DataSource");
	//check return
	if (!basetypeobj)
	{
		Py_XDECREF(pList);
		PyErr_Clear();
		return -3;
	}

	{
		size_t i = 0;
		size_t size = PyList_Size(pList);
		for(i = 0; i < size; i++) {
			//printf
			PyObject* item = PyList_GetItem(pList,i); // +0 
			PyObject* item_str = PyObject_Str(item); //+1

			char* strval = PyString_AsString(item_str); //refer to inner buffer
			std::string sval(strval);

			Py_XDECREF(item_str); //-1 , no needs to keep this now.

			if(strncmp(sval.c_str(),"DataSource",strlen("DataSource")) == 0){
				//skip itself.
				//datasource_list.push_back(strval);
				continue;
			}

			typeobj = PyObject_GetAttrString(pMain, (char*)sval.c_str()); //+1

			if (!typeobj) 
				continue;

			int nRet = 0;
			nRet =  PyObject_IsSubclass(typeobj,basetypeobj);//worked!
			if (nRet == -1)
			{
				//ignore IsSubclass.
				Py_XDECREF(typeobj);
				PyErr_Clear();
				continue;
			}

			if (nRet == 1)
			{
				//is a data sourc-sub class
				datasource_list.push_back(sval);
				//break;
			}
			
			Py_XDECREF(typeobj);
		}

	}

	if(basetypeobj)
		Py_XDECREF(basetypeobj);
	Py_XDECREF(pList);

	return (int)datasource_list.size();
}

#endif
