#pragma once
#include <pluginbase.h>
#include <string>
#include "atlstr.h"
using namespace std;


class PluginObject : public NPObject
{
public:
	PluginObject(NPP);
	~PluginObject(void);
public:
	void deallocate();
	void invalidate();
	bool hasMethod(NPIdentifier methodName);
	bool invokeDefault(const NPVariant *args, uint32_t argCount, NPVariant *result);
	bool invoke(NPIdentifier methodName, const NPVariant *args, uint32_t argCount, NPVariant *result);
	bool hasProperty(NPIdentifier propertyName);
	bool getProperty(NPIdentifier propertyName, NPVariant *result);
	bool setProperty(NPIdentifier name,const NPVariant *value);
	bool removeProperty(NPIdentifier name);
	bool enumerate(NPIdentifier **identifier,uint32_t *count);
	bool construct(const NPVariant *args,uint32_t argCount, NPVariant *result);
	char TransData(string   InStr);
	
public:
	static NPObject* _allocate(NPP npp,NPClass* aClass);
	static void _deallocate(NPObject *npobj);
	static void _invalidate(NPObject *npobj);
	static bool _hasMethod(NPObject* obj, NPIdentifier methodName);
	static bool _invokeDefault(NPObject *obj, const NPVariant *args, uint32_t argCount, NPVariant *result);
	static bool _invoke(NPObject* obj, NPIdentifier methodName, const NPVariant *args, uint32_t argCount, NPVariant *result);
	static bool _hasProperty(NPObject *obj, NPIdentifier propertyName);
	static bool _getProperty(NPObject *obj, NPIdentifier propertyName, NPVariant *result);
	static bool _setProperty(NPObject *npobj, NPIdentifier name,const NPVariant *value);
	static bool _removeProperty(NPObject *npobj, NPIdentifier name);
	static bool _enumerate(NPObject *npobj, NPIdentifier **identifier,uint32_t *count);
	static bool _construct(NPObject *npobj, const NPVariant *args,uint32_t argCount, NPVariant *result);
public:
	SCARDCONTEXT        m_hSc; 
	LPTSTR szReaders, szRdr;
	SCARDHANDLE     m_hCardHandle; 
	DWORD           m_dwAP; 
	string m_ErrMsg;
//����Ҫ��ӷ���
private:
	NPP npp;
	NPIdentifier m_SCardEstablishContext;
	NPIdentifier m_SCardListReaders;
	NPIdentifier m_SCardConnect;
	NPIdentifier m_ScardTransmit;
	NPIdentifier m_SCardDisconnect;
	NPIdentifier m_GetErrMsg;

};
#define Buffer_Max_Len      600

#ifndef __object_class
#define __object_class

static NPClass objectClass =
{	
	NP_CLASS_STRUCT_VERSION,
	PluginObject::_allocate,
	PluginObject::_deallocate,
	PluginObject::_invalidate,
	PluginObject::_hasMethod,
	PluginObject::_invoke,
	PluginObject::_invokeDefault,
	PluginObject::_hasProperty,
	PluginObject::_getProperty,
	PluginObject::_setProperty,
	PluginObject::_removeProperty,
	PluginObject::_enumerate,
	PluginObject::_construct
};


#endif