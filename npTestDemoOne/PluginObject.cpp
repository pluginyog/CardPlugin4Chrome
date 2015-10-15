
#include "PluginObject.h"
#include "winscard.h"
#include "atlstr.h"
#pragma comment(lib,"winscard.lib")

extern NPNetscapeFuncs NPNFuncs;


//这里要添加
PluginObject::PluginObject(NPP npp):
npp(npp)
{

	m_SCardEstablishContext = NPN_GetStringIdentifier("InitializeContext");
	m_SCardListReaders = NPN_GetStringIdentifier("ListCard");
	m_SCardConnect = NPN_GetStringIdentifier("ConnectCard");
	m_ScardTransmit = NPN_GetStringIdentifier("TransmitCard");
	m_SCardDisconnect = NPN_GetStringIdentifier("DisconnectCard");
	m_GetErrMsg = NPN_GetStringIdentifier("GetErrMsg");
	
	
}


PluginObject::~PluginObject(void)
{
	
}

void PluginObject::deallocate()
{
	
}

void PluginObject::invalidate()
{
	
}
//这里要添加
bool PluginObject::hasMethod(NPIdentifier methodName)
{
	bool bRev = false;

	NPUTF8 *pName = NPNFuncs.utf8fromidentifier(methodName);

	if (strcmp(pName, "InitializeContext") == 0)
	{
		return true;
	}
	if (strcmp(pName, "ListCard") == 0)
	{
		return true;
	}
	if (strcmp(pName, "ConnectCard") == 0)
	{
		return true;
	}
	if (strcmp(pName, "TransmitCard") == 0)
	{
		return true;
	}
	if (strcmp(pName, "DisconnectCard") == 0)
	{
		return true;
	}
	if (strcmp(pName, "GetErrMsg") == 0)
	{
		return true;
	}
	
	
	
	return false;
}

bool PluginObject::invokeDefault(const NPVariant *args, uint32_t argCount, NPVariant *result)
{
	return true;
}
//添加函数实现
bool PluginObject::invoke(NPIdentifier methodName, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
	bool bRev = false;
	if(methodName==m_SCardEstablishContext)
	{
		LONG  lReturn; 
		lReturn = SCardEstablishContext(SCARD_SCOPE_USER, NULL, NULL, &m_hSc); 
		
		if ( lReturn!=SCARD_S_SUCCESS ) 
		{
			INT32_TO_NPVARIANT(0,*result);
		}else
		{
			INT32_TO_NPVARIANT(1,*result);
		}
		
		return true;
	}
	if(methodName==m_SCardListReaders)
	{

		LONG  lReturn; 

			lReturn = SCardEstablishContext(SCARD_SCOPE_USER, NULL, NULL, &m_hSc); 
			if ( lReturn!=SCARD_S_SUCCESS ) 
			{
				INT32_TO_NPVARIANT(0,*result);
				m_ErrMsg="初始化运行环境失败";
				return true;
			}




		DWORD cchReaders = SCARD_AUTOALLOCATE;
		lReturn = SCardListReaders(m_hSc, NULL, (char*)&szReaders, &cchReaders); 
		if ( lReturn==SCARD_S_SUCCESS ) 
		{
			//string tmpstr = (string)szReaders;
			string hexstr="";
			for (DWORD i=0; i< cchReaders; i++)
			{
				
				if(szReaders[i]=='\0'){
					hexstr +=';';
				}else{
					hexstr +=szReaders[i];
				}
			}
			char* npOutString =(char*) NPN_MemAlloc(hexstr.length());
			
			strcpy(npOutString, hexstr.c_str());
			STRINGZ_TO_NPVARIANT(npOutString,*result);
			SCardFreeMemory(m_hSc,szReaders);
			m_ErrMsg="";
			return true;
		}
		INT32_TO_NPVARIANT(0,*result);
		m_ErrMsg="初始化运行环境失败";
		return true;
		
	}
	if(methodName==m_SCardConnect)
	{
		LONG  lReturn; 
		if(!m_hSc)
		{
			lReturn = SCardEstablishContext(SCARD_SCOPE_USER, NULL, NULL, &m_hSc); 
			if ( lReturn!=SCARD_S_SUCCESS ) 
			{
				INT32_TO_NPVARIANT(0,*result);
				m_ErrMsg="初始化运行环境失败";
				return true;
			}
		}
		NPVariant npvClass = args[0];
		if (NPVARIANT_IS_STRING(npvClass))
		{
			NPString rString = NPVARIANT_TO_STRING(npvClass);
			char strBuf[255] = {0};
			memcpy(strBuf, rString.UTF8Characters, rString.UTF8Length);
			lReturn = SCardConnect( m_hSc, (LPSTR)strBuf,SCARD_SHARE_SHARED, SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1, &m_hCardHandle, &m_dwAP );
			if ( lReturn!=SCARD_S_SUCCESS ) 
			{
				INT32_TO_NPVARIANT(1,*result);
				m_ErrMsg="连接读卡器失败";
			}else
			{
				m_ErrMsg="";
				INT32_TO_NPVARIANT(0,*result);
			}
			return true;
		}else
		{
			m_ErrMsg="未选择读卡器";
			INT32_TO_NPVARIANT(1,*result);
		}
		return true;
		
		
	}
	if(methodName==m_SCardDisconnect)
	{
		LONG lReturn=0;
		if(m_hCardHandle)
		{
			lReturn = SCardDisconnect(m_hCardHandle, SCARD_EJECT_CARD);
		}
		INT32_TO_NPVARIANT((int)lReturn,*result);
		m_ErrMsg="";
		return true;

	}
	
	if(methodName==m_ScardTransmit)
	{
		if(!m_hCardHandle)
		{
			INT32_TO_NPVARIANT(6999,*result);
			m_ErrMsg="未选择读卡器";
			return true;
		}
		
		NPVariant npvClass = args[0];
		
		if (NPVARIANT_IS_STRING(npvClass))
		{
			NPString rString = NPVARIANT_TO_STRING(npvClass);
			char strBuf[255] = {0};
			memcpy(strBuf, rString.UTF8Characters, rString.UTF8Length);

			CString tmpstr,hexstr,sApdu;
			sApdu=CString(strBuf);
			BYTE tmpchar,c;
			int j=0;
			DWORD iLength=sApdu.GetLength();
			BYTE *pDataSend=(BYTE *)malloc(iLength);
			memset(pDataSend,0,iLength);
			for(DWORD i=0;i<iLength;i++)
			{
				tmpchar=sApdu.GetAt(i);
				tmpstr.Format("%c",tmpchar);
				hexstr+=tmpstr;
				tmpchar=sApdu.GetAt(i+1);
				tmpstr.Format("%c",tmpchar);
				hexstr+=tmpstr;
				i++;
				string te="0x"+hexstr;
				c=TransData(te);
				hexstr="";
				pDataSend[j]=c;
				j++;
			}

			BYTE  receive_data[Buffer_Max_Len];
			LONG lReturn;
			memset(receive_data,0,Buffer_Max_Len);
			DWORD dwRecvlen=Buffer_Max_Len,senddatalen=j;;

			if(m_dwAP==SCARD_PROTOCOL_T1)
			{
				lReturn = SCardTransmit(m_hCardHandle,
									SCARD_PCI_T1,
									(unsigned char*)pDataSend,
									senddatalen,
									NULL,
									(unsigned char*)receive_data,
									&dwRecvlen );
			}
			else
			{
				lReturn = SCardTransmit(m_hCardHandle,
									SCARD_PCI_T0,
									(unsigned char*)pDataSend,
									senddatalen,
									NULL,
									(unsigned char*)receive_data,
									&dwRecvlen );
			}
			if(lReturn!=SCARD_S_SUCCESS)
			{
				INT32_TO_NPVARIANT((int)lReturn,*result);
				m_ErrMsg="数据发送失败";
				return true;
			}
			hexstr="";
			for(DWORD i=0;i<dwRecvlen;i++)
			{
				tmpstr.Format("%02X ",receive_data[i]);
				hexstr+=tmpstr;
			}
			free(pDataSend);
			string tempStr=(string)hexstr;
			char* npOutString =(char*) NPN_MemAlloc(tempStr.length());
			strcpy(npOutString, tempStr.c_str());
			STRINGZ_TO_NPVARIANT(npOutString,*result);
			m_ErrMsg="";
			return true;
			
		}
		
		INT32_TO_NPVARIANT(0,*result);
		return true;
		
	}
	if(methodName==m_GetErrMsg)
	{
		char* npOutString =(char*) NPN_MemAlloc(m_ErrMsg.length());
		strcpy(npOutString, m_ErrMsg.c_str());
		STRINGZ_TO_NPVARIANT(npOutString,*result);
		return true;
	}
	return bRev;
}

char  PluginObject::TransData(string   InStr)
{
	char* npOutString =(char*) NPN_MemAlloc(InStr.length()+1);	
	strcpy(npOutString, InStr.c_str());
	char cTemp[5] = {0};
	char tmpHex[5];
	int num;
	strncpy(tmpHex,npOutString,4);
	tmpHex[4]='\0';
	sscanf(tmpHex,"%x",&num);
	*((unsigned int *)cTemp) = num;
	return cTemp[0];
}

bool PluginObject::hasProperty(NPIdentifier propertyName)
{
	return false;
}

bool PluginObject::getProperty(NPIdentifier propertyName, NPVariant *result)
{
	return false;
}

bool PluginObject::setProperty(NPIdentifier name,const NPVariant *value)
{
	return true;
}

bool PluginObject::removeProperty(NPIdentifier name)
{
	return true;
}

bool PluginObject::enumerate(NPIdentifier **identifier,uint32_t *count)
{
    return false;
}

bool PluginObject::construct(const NPVariant *args,uint32_t argCount, NPVariant *result)
{
	return true;
}

// ========================================静态函数===============================================================
NPObject *PluginObject::_allocate(NPP npp,NPClass *aClass)
{
	return new PluginObject(npp);
}

void PluginObject::_deallocate(NPObject *npobj)
{
	((PluginObject*)npobj)->deallocate();

	if(npobj)
	{
		delete npobj;
	}
}

void PluginObject::_invalidate(NPObject *npobj)
{
	((PluginObject*)npobj)->invalidate();
}

bool PluginObject::_hasMethod(NPObject* obj, NPIdentifier methodName)
{
	return ((PluginObject*)obj)->hasMethod(methodName);
}

bool PluginObject::_invokeDefault(NPObject *obj, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
	return ((PluginObject*)obj)->invokeDefault(args,argCount,result);
}

bool PluginObject::_invoke(NPObject* obj, NPIdentifier methodName, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
	return ((PluginObject*)obj)->invoke(methodName,args,argCount,result);
}

bool PluginObject::_hasProperty(NPObject *obj, NPIdentifier propertyName)
{
	return ((PluginObject*)obj)->hasProperty(propertyName);
}

bool PluginObject::_getProperty(NPObject *obj, NPIdentifier propertyName, NPVariant *result)
{
	return ((PluginObject*)obj)->getProperty(propertyName,result);
}

bool PluginObject::_setProperty(NPObject *npobj, NPIdentifier name,const NPVariant *value)
{
	return ((PluginObject*)npobj)->setProperty(name,value);
}

bool PluginObject::_removeProperty(NPObject *npobj, NPIdentifier name)
{
	return ((PluginObject*)npobj)->removeProperty(name);
}

bool PluginObject::_enumerate(NPObject *npobj, NPIdentifier **identifier,uint32_t *count)
{
	return ((PluginObject*)npobj)->enumerate(identifier,count);
}

bool PluginObject::_construct(NPObject *npobj, const NPVariant *args,uint32_t argCount, NPVariant *result)
{
	return ((PluginObject*)npobj)->construct(args,argCount,result);
}

// ========================================静态函数===============================================================