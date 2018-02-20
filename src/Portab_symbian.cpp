/*
============================================================================
 Name        : Portab_symbian from Portab_symbian.h
 Author      : yilizheng
 Version     :
 Copyright   : Your copyright notice
 Description : Declares document for application.
============================================================================
*/

#include "Portab_symbian.h"
#include<f32file.h>         //  efsrv.lib
#include <utf.h>			//charconv.lib 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <aknmessagequerydialog.h>
#include <bautils.h>
#include <charconv.h>
#include <commdb.h>
#include <commdbconnpref.h>
#include <Es_sock.h>
#include <in_sock.h>
#include "CenterCore.h"
#ifndef _S60_3RD_
#include <Plpvariant.h>     //LIBRARY		plpvariant.lib
#endif
#include <MyleftCenter.rsg>
#include "funPanic.h"
const int	FC_SUPPROT_AUDIO[FC_SUPPORT_AUDIO_NUM] = { 
			FC_FOURCC('a','p','c','m')
		};

typedef struct
{
	RFs ifs;
	CDir* idirList;
	int	i ;//index
	TBuf<MAX_FILEMANE> ext;
}FindStruct;


void* FC_malloc(DWORD size)
{
	void *p=NULL;
	if((size)<(KMaxTInt/2)) //KMaxTInt=0x7fffffff;
	{
		p=malloc((size));
		if(p)
			return p;
	}
	FC_Log("FC_malloc() error Malloc_Failed");
	FC_Panic(Malloc_Failed);
	return NULL;
}
/*
void	 FC_free(void * cell)			
{ 
	if(cell) 
	{
		free(cell); 	
		(cell) = NULL;
	}
	//else
		//FC_Log("FC_free() a unvalid cell");
}
*/
void UnicodeToAsciiL (TDes8& _asciiBuffer,const TDesC16& _unicodeBuffer) 
{
	CCnvCharacterSetConverter *_converter =
		CCnvCharacterSetConverter::NewLC ();

	if (CCnvCharacterSetConverter::EAvailable !=
	_converter->PrepareToConvertToOrFromL (
	KCharacterSetIdentifierAscii, CEikonEnv::Static()->FsSession())) 
	{
		User::Leave (KErrNotSupported);
	}

	TInt _ret = _converter->ConvertFromUnicode (
		_asciiBuffer, _unicodeBuffer);

	CleanupStack::PopAndDestroy (_converter);
	if (_ret) 
	{
		User::Panic (_L("FP.UnicodeToAscii"), 1);
	}
}
void FC_UnicodeToAsciiL (TDes8& _asciiBuffer,const TDesC16& _unicodeBuffer) 
{
	UnicodeToAsciiL(_asciiBuffer,_unicodeBuffer);
}

void GbkToUnicodeL (TDes16& _unicodeBuffer,const TDesC8& _asciiBuffer) 
{
	CCnvCharacterSetConverter *_converter =
		CCnvCharacterSetConverter::NewLC ();

	/* check if support Gbk */
	if (CCnvCharacterSetConverter::EAvailable !=
	_converter->PrepareToConvertToOrFromL (
	KCharacterSetIdentifierGbk, CEikonEnv::Static()->FsSession())) 
	{
		/* check if support GB2312 */
		if (CCnvCharacterSetConverter::EAvailable !=
		_converter->PrepareToConvertToOrFromL (
		KCharacterSetIdentifierGb2312, CEikonEnv::Static()->FsSession())) 
		{
			/* not a chinese device, so use ASCII */
			if (CCnvCharacterSetConverter::EAvailable !=
			_converter->PrepareToConvertToOrFromL (
			KCharacterSetIdentifierAscii, CEikonEnv::Static()->FsSession())) 
			{
				/* not support ASCII too */
				User::Leave (KErrNotSupported);
			}
		}
	}

	TInt _state = CCnvCharacterSetConverter::KStateDefault;
	TInt _ret = _converter->ConvertToUnicode (
		_unicodeBuffer, _asciiBuffer, _state);

	CleanupStack::PopAndDestroy (_converter);
}
void FC_GbkToUnicodeL (TDes16& _unicodeBuffer,const TDesC8& _asciiBuffer) 
{
	GbkToUnicodeL ( _unicodeBuffer,_asciiBuffer) ;
}

void AsciiToUnicodeL (TDes16& _unicodeBuffer,const TDesC8& _asciiBuffer) 
{
	CCnvCharacterSetConverter *_converter =
		CCnvCharacterSetConverter::NewLC ();

	if (CCnvCharacterSetConverter::EAvailable !=
	_converter->PrepareToConvertToOrFromL (
	KCharacterSetIdentifierAscii, CEikonEnv::Static()->FsSession())) 
	{
		User::Leave (KErrNotSupported);
	}

	TInt _state = CCnvCharacterSetConverter::KStateDefault;
	TInt _ret = _converter->ConvertToUnicode (
		_unicodeBuffer, _asciiBuffer, _state);
			
	CleanupStack::PopAndDestroy (_converter);
	if (_ret) 
	{
		User::Panic (_L("FP.AsciiToUnicode"), 1);
	}
}
void FC_AsciiToUnicodeL (TDes16& _unicodeBuffer,const TDesC8& _asciiBuffer)
{
	AsciiToUnicodeL(_unicodeBuffer,_asciiBuffer);
}

void Utf8ToUnicodeL (TDes16& _unicodeBuffer,const TDesC8& _utf8Buffer) 
{
	CCnvCharacterSetConverter *_converter =
		CCnvCharacterSetConverter::NewLC ();

	if (CCnvCharacterSetConverter::EAvailable !=
	_converter->PrepareToConvertToOrFromL (
	KCharacterSetIdentifierUtf8, CEikonEnv::Static()->FsSession())) 
	{
		User::Leave (KErrNotSupported);
	}

	TInt _state = CCnvCharacterSetConverter::KStateDefault;
	TInt _ret = _converter->ConvertToUnicode (
		_unicodeBuffer, _utf8Buffer, _state);
			
	CleanupStack::PopAndDestroy (_converter);
}
void FC_Utf8ToUnicodeL (TDes16& _unicodeBuffer,const TDesC8& _utf8Buffer) 
{
	int r;
	TRAP(r,Utf8ToUnicodeL( _unicodeBuffer, _utf8Buffer));
}

void UnicodeToUtf8L (TDes8& _utf8Buffer,const TDesC16& _unicodeBuffer) 
{
	CCnvCharacterSetConverter *_converter =
		CCnvCharacterSetConverter::NewLC ();
	
	if (CCnvCharacterSetConverter::EAvailable !=
	_converter->PrepareToConvertToOrFromL (
	KCharacterSetIdentifierUtf8, CEikonEnv::Static()->FsSession())) 
	{
		User::Leave (KErrNotSupported);
	}

	TInt _ret = _converter->ConvertFromUnicode (
		_utf8Buffer, _unicodeBuffer);

	CleanupStack::PopAndDestroy (_converter);
	if (_ret) {
		User::Panic (_L("UnicodeToUtf8L"), 1);
	}
}
void FC_UnicodeToUtf8L (TDes8& _utf8Buffer,const TDesC16& _unicodeBuffer) 
{
	int r;
	TRAP(r,UnicodeToUtf8L( _utf8Buffer, _unicodeBuffer));
}

void UnicodeToGbkL (TDes8& _asciiBuffer,
					const TDesC16& _unicodeBuffer) 
{
	CCnvCharacterSetConverter *_converter =
		CCnvCharacterSetConverter::NewLC ();

	/* check if support Gbk */
	if (CCnvCharacterSetConverter::EAvailable !=
	_converter->PrepareToConvertToOrFromL (
	KCharacterSetIdentifierGbk, CEikonEnv::Static()->FsSession())) 
	{
		/* check if support GB2312 */
		if (CCnvCharacterSetConverter::EAvailable !=
		_converter->PrepareToConvertToOrFromL (
		KCharacterSetIdentifierGb2312, CEikonEnv::Static()->FsSession())) 
		{
			/* not a chinese device, so use ASCII */
			if (CCnvCharacterSetConverter::EAvailable !=
			_converter->PrepareToConvertToOrFromL (
			KCharacterSetIdentifierAscii, CEikonEnv::Static()->FsSession())) 
			{
				/* not support ASCII too */
				User::Leave (KErrNotSupported);
			}
		}
	}

	TInt _ret = _converter->ConvertFromUnicode (
		_asciiBuffer, _unicodeBuffer);

	CleanupStack::PopAndDestroy (_converter);
	if (_ret) {
		User::Panic (_L("FP.UnicodeToAscii"), 1);
	}
}
void FC_UnicodeToGbkL (TDes8& _asciiBuffer,const TDesC16& _unicodeBuffer) 
{
	UnicodeToGbkL ( _asciiBuffer,_unicodeBuffer) ;
}

int FC_ACPToUtf8(const char* szAcp, char** szUtf8)
{
/*
	TPtrC8 pUtf8;
	//TPtrC16 pUnicode;
	TBuf<MAX_FILEMANE> unicBuffer;
	TBuf8<MAX_FILEMANE> utf8Buffer;
	
	pUtf8.Set((const TUint8*)szUnicode , strlen(szUnicode) );
	GbkToUnicodeL(unicBuffer,pUtf8);
	UnicodeToUtf8L(utf8Buffer,unicBuffer);
		
	*szUtf8=(char*)FC_malloc((DWORD)utf8Buffer.Length());
	char *pBuf=(char*)utf8Buffer.PtrZ();
	memcpy(*szUtf8,pBuf,utf8Buffer.Length());

	return utf8Buffer.Length();
*/
	return FC_GbkToUtf8(szAcp,szUtf8);

}
//need modify


int FC_UnicodeToUtf8(char* szUnicode, char** szUtf8)
{
#if 0
	TPtrC16 	pUnicode;
	//TPtr8 	putf8;
	HBufC8*utfBuf=HBufC8::New( strlen(szUnicode) +2);
	TPtr8 	putf8=utfBuf->Des();
	
	pUnicode.Set( (const TUint16*)szUnicode , strlen(szUnicode));
	
	*szUtf8=(char*)FC_malloc(strlen(szUnicode)+1);
	memset(*szUtf8,'\0',strlen(szUnicode)+1);

	FC_TBuf16ToCharp(*szUtf8, pUnicode);
//	UnicodeToUtf8L(putf8,pUnicode);
	

	//char *pBuf=(char*)utfBuf->Ptr();
	//memcpy(*szUtf8,pBuf,utfBuf->Length());
	
	return strlen(*szUnicode)+1;
#else 
	return 0;
#endif
}
//need modify
int	FC_Utf8ToUnicode(char* szUtf8, char** szUnicode)
{
#if 1
	TPtrC8 pUtf8;
	//TPtrC16 pUnicode;
	TBuf<MAX_FILEMANE> Buffer;
	
	pUtf8.Set((const TUint8*)szUtf8 , strlen(szUtf8) );
	Utf8ToUnicodeL(Buffer,pUtf8);
	*szUnicode=(char*)FC_malloc((DWORD)Buffer.Length()*2);
	char *pBuf=(char*)Buffer.Ptr();
	memcpy(*szUnicode,pBuf,Buffer.Length()*2);

#endif

	return Buffer.Length()*2;
}
#if 0
char* FC_TBuf16ToCharp(char* szchar,const TDesC& szDes16 )
{
	//assert(szchar); 

	char* p=szchar,*bufP=NULL;
	TBuf8<MAX_FILEMANE> Buf8;
	
	//HBufC8* Buf8=HBufC8::New( szDes16.Length()*2);
	//TPtr8 	pgbkBuf8=Buf8->Ptr();

	TPtr8 pUtf8=TPtr8(NULL,0,0);
	pUtf8.Set((TUint8*)szchar , 0,szDes16.Length()*2);
	//CnvUtfConverter::ConvertFromUnicodeToUtf8( pUtf8, szDes16 ); 
	UnicodeToUtf8L( Buf8 , szDes16);
	bufP=(char *)Buf8.PtrZ();

	for(int i=Buf8.Length();i>0;i--)
	{
		*p++=*bufP++;
	}
	
	//if(Buf8)
	//	delete Buf8;

	return szchar;
}
#else

char* FC_TBuf16ToCharAscii(char* szchar,const TDesC& szDes16 )
{
	//assert(szchar); 

	char* p=szchar,*bufP=NULL;

	HBufC8* Buf8=HBufC8::New( 512);
	TPtr8 	pBuf8=Buf8->Des();

	UnicodeToGbkL( pBuf8 , szDes16);
	bufP=(char *)pBuf8.PtrZ();

	strcpy(szchar,bufP);	
	
	if(Buf8)
		delete Buf8;

	return szchar;
}

char* FC_TBuf16ToCharp(char* szchar,const TDesC& szDes16 )
{
	//assert(szchar); 

	char* p=szchar,*bufP=NULL;

	HBufC8* Buf8=HBufC8::New( 512);
	TPtr8 	pBuf8=Buf8->Des();

	UnicodeToUtf8L( pBuf8 , szDes16);
	bufP=(char *)pBuf8.PtrZ();

	strcpy(szchar,bufP);	
	
	if(Buf8)
		delete Buf8;

	return szchar;
}
#endif
TDes* FC_CharpToTBuf16(TDes& TBuff,const char* szPath )
{
	if(!szPath)
		return NULL;
	TPtrC8 KDirp;
	KDirp.Set( (const TUint8*)szPath , strlen(szPath) );
	int r;
	TRAP(r,Utf8ToUnicodeL(TBuff,KDirp));
	//CnvUtfConverter::ConvertToUnicodeFromUtf8(TBuff,KDirp);
	return &TBuff;
}


int FC_GbkToUtf8( const char* szGbk , char** szUtf8 )
{
	if(!strlen(szGbk))
		return 0;
	TPtrC8 pGbk;
	char *pBuf=NULL;
	TBuf<256> unicodeBuf;
	TBuf8<256> utfBuf8;
	pGbk.Set( (const TUint8*)szGbk , strlen(szGbk) );
	
	GbkToUnicodeL(unicodeBuf,pGbk);
	UnicodeToUtf8L(utfBuf8,unicodeBuf);

	*szUtf8=(char*)FC_malloc((DWORD)utfBuf8.Length()+1);
	memset(*szUtf8,0,utfBuf8.Length()+1);
	pBuf=(char*)utfBuf8.Ptr();
	memcpy(*szUtf8, pBuf, utfBuf8.Length());

	return utfBuf8.Length();
}

int FC_GbkToUnicode( const char* szGbk , char** szUnicode )
{
	TBuf<512> unicodeBuf;
	TPtrC8 pGbk;
	pGbk.Set( (const TUint8*)szGbk , strlen(szGbk)+1 );

	GbkToUnicodeL(unicodeBuf,pGbk);

	*szUnicode=(char*)FC_malloc((DWORD)unicodeBuf.Length()*2);
	memset(*szUnicode,'\0',unicodeBuf.Length()*2);
	char *pBuf=(char*)unicodeBuf.Ptr();
	memcpy(*szUnicode, pBuf, unicodeBuf.Length()*2);

	return unicodeBuf.Length()*2;
}

int FC_Utf8ToGbk( const char* szUtf8 , char** szGbk  )
{
	TPtrC8 pUtf8;
	char *pBuf=NULL;
	
#if 0
	TBuf<1024> unicodeBuf;
	TBuf8<1024> gbkBuf8;
#else
	HBufC8*gbkBuf8=HBufC8::New( strlen(szUtf8) );
	TPtr8 	pgbkBuf8=gbkBuf8->Des();
	HBufC*unicodeBuf=HBufC::New( strlen(szUtf8) );
	TPtr 	punicodeBuf=unicodeBuf->Des();
#endif	

	pUtf8.Set( (const TUint8*)szUtf8 , strlen(szUtf8) );

#if 0	
	Utf8ToUnicodeL(unicodeBuf,pUtf8);
	UnicodeToGbkL(gbkBuf8,unicodeBuf);
	
	*szGbk=(char*)FC_malloc((DWORD)gbkBuf8.Length());
	pBuf=(char*)gbkBuf8.Ptr();
	memcpy(*szGbk, pBuf, gbkBuf8.Length());
#else
	Utf8ToUnicodeL(punicodeBuf,pUtf8);
	UnicodeToGbkL(pgbkBuf8,punicodeBuf);

	*szGbk=(char*)FC_malloc((DWORD)pgbkBuf8.Length());
	pBuf=(char*)pgbkBuf8.Ptr();
	memcpy(*szGbk, pBuf, pgbkBuf8.Length());
#endif

#if 1
	if(gbkBuf8)
		delete gbkBuf8;
	if(unicodeBuf)
		delete unicodeBuf;
#endif	
	return pgbkBuf8.Length();
}


// #include <Plpvariant.h>  , LIBRARY		plpvariant.lib
void FC_GetSerialNumber(DWORD* pHigh, DWORD* pLow)
{ 

#ifdef _S60_3RD_
	*pHigh=User::TickCount();
	*pLow=User::TickCount();
	
#else
	char imei[15];
	char*p=imei;

	memset(imei,'\0',15);
	
	TPlpVariantMachineId  machineID;
	PlpVariant::GetMachineIdL(machineID);
	FC_TBuf16ToCharp(imei,machineID);
	p=p+8;
	*pLow=atoi(p);  //low 8-14 byte
	p='\0';
	*pHigh=atoi(imei);//0-7 byte
#endif

}


char* FC_ChangePath(char *szPath)
{
	char a='/';
	char *p=szPath;
	for(int i=0;i<strlen(szPath);i++)
	{
		if(*p==a)
		{
			*p='\\';			
		}
		p++;
	}
	return szPath;
}
HANDLE FC_FindFirstFile(char* szPath, char* szPattern, HANDLE* phFindData)
{
	//assert(szPath);

	//RFs fs;
	CDir* dirList;
	TParse p;
	TBuf<MAX_FILEMANE> KDirpath;
	TBuf<MAX_FILEMANE> KszPattern;
	//FindStruct* data=(FindStruct*)FC_malloc(sizeof(FindStruct));
	FindStruct* data=new FindStruct;
	if(!data)
		return 0;

	//memset(data,0,sizeof(FindStruct));
	
	FC_ChangePath(szPath);
	FC_CharpToTBuf16(KDirpath,szPath);
	
	if(!strstr(szPattern,".*"))
	{
	FC_CharpToTBuf16(KszPattern,szPattern);

	
	p.Set(KszPattern,NULL,NULL);
	KszPattern=p.Ext();
	}
	
	data->ifs.Connect();
	User::LeaveIfError(data->ifs.GetDir(KDirpath,KEntryAttMaskSupported,ESortByName,dirList));
	
	if(dirList->Count())  
	{
		if(!KszPattern.Length())
		{
			data->idirList=dirList;
			data->i=0;
			data->ext=_L("*.*");
			*phFindData=(HANDLE*)data;
			return dirList;	

		}
		for (int i=0;i<dirList->Count();i++)
			{
				p.Set((*dirList)[i].iName,NULL,NULL);
				if(p.Ext()==KszPattern) //find first
				{
					data->idirList=dirList;
					data->i=i;
					data->ext=KszPattern;
					*phFindData=(HANDLE*)data;
					return dirList;	
				}
										;
			}
	}
	data->ifs.Close();
	delete dirList;
	delete data;
	*phFindData=0;
	return 0;
	
}

BOOL FC_FindNextFile( HANDLE hFind, HANDLE* phFindData)
{
 	CDir* dirList=(CDir* )hFind;
	TParse p;
	FindStruct* data=(FindStruct*)(*phFindData);
	//assert(data);
	data->i++;
	for (int i=data->i;i<dirList->Count();i++)
	{
		if(data->ext.Find(_L("*.*"))!=KErrNotFound)
		{
			data->i=i;
			return TRUE;	

		}
		p.Set((*dirList)[i].iName,NULL,NULL);
		if(p.Ext()==data->ext) //find next
		{
			data->i=i;
			return TRUE;	
		}
								;
	}
	return FALSE;

}

void FC_FindClose( HANDLE hFind, HANDLE hFindData)
{
	CDir* dir=(CDir*)hFind;
	FindStruct* data=(FindStruct*)hFindData;
	if(dir)
	{
		delete dir;
	}
	if(data)
	{
		//if(data->idirList)
		//	delete data->idirList;
		data->ifs.Close();
		delete data;
	}
	hFind=NULL;
	hFindData=NULL;
}

BOOL FC_FindDataIsDirectory(HANDLE hFindData)
{
	FindStruct* data=(FindStruct*)hFindData;
//	assert(data);
	CDir * dirList=data->idirList;
	//(*(data->idirList))[data->i];
	

	const TEntry& entry=(*dirList)[data->i];
		
	return 	(BOOL)entry.IsDir();
}

void FC_FindDataGetFileName(HANDLE hFindData, char* szName)
{
	FindStruct* data=(FindStruct*)hFindData;
	//assert(data);
	TBuf16<MAX_FILEMANE> filename;

	filename=(*(data->idirList))[data->i].iName;
	//FC_GbkToUtf8(szName,);
	FC_TBuf16ToCharp(szName,filename);
}


void FC_GetDateTime(DWORD* pYYYYMMDD, DWORD* pHHMMSS)
{
	TBuf<32>  theTime;//存储转换后的时间
	char theTimeU[64];
	TTime tt;
	char* p=NULL;
	
	tt.HomeTime();
	_LIT(KTimeFormat,"%Y%M%D%1%2%3%H%T%S");//格式为：20060304121212
	tt.FormatL(theTime,KTimeFormat);
	FC_TBuf16ToCharp(theTimeU,theTime);//get char 
	p=theTimeU;
	p=p+8;
	*pHHMMSS=atoi(p);
	*p='\0';
	*pYYYYMMDD=atoi(theTimeU);
}

BOOL	FC_IsValidDateTime(DWORD dwYYYYMMDD, DWORD dwHHMMSS)
{
	//dbg("FC_IsValidDateTime %d %d",dwYYYYMMDD,dwHHMMSS);
	DWORD	dwYYYYMMDDNow;
	DWORD	dwHHMMSSNow;

	FC_GetDateTime( &dwYYYYMMDDNow, &dwHHMMSSNow);
	if( dwYYYYMMDD > dwYYYYMMDDNow )
		return TRUE;

	if( dwYYYYMMDD == dwYYYYMMDDNow )
		return dwHHMMSS >= dwHHMMSSNow;

	return FALSE;
}

void*FC_CreateThread(DWORD lpStartAddress, void*lpParameter,DWORD* lpThreadId)
{
	return NULL;
}
void FC_CloseThread(void*hThread)
{}
void	FC_WaitThreadEnd(HANDLE hThread)
{}

void*LockCreate()
{
	return NULL;
}
void	LockDelete(void*)
{}
void	LockEnter(void*)
{}
void	LockLeave(void*)
{}


void FC_Panic(TInt code)
{
	User::Panic (_L("ERROR"), code);

}

int FC_LogT(const char *fmt,TDes& TBuff)//#include <stdarg.h>
{
	char * temp=(char *)FC_malloc(TBuff.Length()*2+1);
	FC_TBuf16ToCharAscii(temp, TBuff);
	int ret=FC_Logsp(fmt,temp);
	FC_free(temp);

	return ret;	
}

int FC_Log(const char *fmt,...)//#include <stdarg.h>
{
#ifdef FC_DEBUG_LOG

	FILE *logfile=NULL;

#if defined __WINS__
	logfile=fopen("c:\\myleft_debug.log", "a+");
#else
	logfile=fopen("e:\\myleft_debug.log", "a+");
#endif

	fwrite("\n", 1, 1, logfile); //change line
	va_list arg_ptr;
	va_start(arg_ptr, fmt);
	fprintf(logfile,"(--%d--)",User::TickCount());
	int nWrittenBytes = vfprintf(logfile, fmt, arg_ptr);

	va_end(arg_ptr);
	
	fclose(logfile);
	
	return nWrittenBytes;
#else
	return 0;
#endif
	
}

int FC_Logsp(const char *fmt,...)//#include <stdarg.h>
{
#ifdef FC_SOCKET_LOG
	FILE *logfile;

#ifdef FC_DEBUG_LOG
	#if defined __WINS__
		logfile=fopen("c:\\myleft_debug.log", "a+");
	#else
		logfile=fopen("e:\\myleft_debug.log", "a+");
	#endif
#else	
	#if defined __WINS__
			logfile=fopen("c:\\myleft_littleDebug.log", "a+");
	#else
			logfile=fopen("e:\\myleft_littleDebug.log", "a+");
	#endif
#endif

	fwrite("\n", 1, 1, logfile); //change line
	va_list arg_ptr;
	va_start(arg_ptr, fmt);
	fprintf(logfile,"(--%d--)",User::TickCount());
	int nWrittenBytes = vfprintf(logfile, fmt, arg_ptr);

	va_end(arg_ptr);

	fclose(logfile);
	
	return nWrittenBytes;
	
#endif
	return 0;
}

int FC_Logh246(const char *fmt,...)//#include <stdarg.h>
{

	FILE *logfile=fopen("c:\\H264debug.log", "a+");

	fwrite("\n", 1, 1, logfile); //change line
	va_list arg_ptr;
	va_start(arg_ptr, fmt);

	int nWrittenBytes = vfprintf(logfile, fmt, arg_ptr);

	va_end(arg_ptr);
	fclose(logfile);
	return nWrittenBytes;
	
	//return 0;
}

void FC_FreeRam(const char* countent)
{
	TInt aBiggestBlockU;
	TInt freeMemoryU = User::Available(aBiggestBlockU);
	//TInt freeMemoryH = RHeap::Available(aBiggestBlockH);
	
	FC_Log("%s : User::freeMemory=%d,aBiggestBlockU=%d",countent,freeMemoryU,aBiggestBlockU);

}


//
int	FC_MessageBox(const char* szText, const char* szTitle, DWORD dwType, DWORD dwIcon)
{
	TBuf<MAX_FILEMANE> text;
	TBuf<MAX_FILEMANE> title;
	
	 FC_CharpToTBuf16(text,szText);
	 FC_CharpToTBuf16(title,szTitle);

	
	if(dwType==FCMB_OK)
	{
		 CEikonEnv::InfoWinL(title, text);
		return 0;

	}
	else if(dwType==FCMB_OKCANCEL|| dwType==FCMB_MB_YESNO)
	{
		CAknMessageQueryDialog* _msg_query_dialog = 
		CAknMessageQueryDialog::NewL (text);
		_msg_query_dialog->SetHeaderTextL (title);
		return _msg_query_dialog->ExecuteLD (R_AVKON_MESSAGE_QUERY_DIALOG);

	}
	return 0;
}


#if 1
void	FC_SendSMS(char* szTo, char* szMessage)
{
}

void	FC_OpenIE(char* szUrl)
{
}
#endif

BOOL FC_DeleteFile(const char* szFile)
{
//	assert(szFile);
	//dbg("FC_DeleteFile %s",szFile);
	TBuf16<MAX_FILEMANE> szfilefull;
	RFs filesS;
	
	FC_CharpToTBuf16(szfilefull, szFile);

	filesS.Connect();
	if(!filesS.Delete(szfilefull))
	{
		filesS.Close();
		return true;
	}
	
	filesS.Close();
	return false;
		
}

BOOL	FC_MoveFile(const char* szFrom, const char* szTo)
{
	//assert(szFrom && szTo);
	//dbg("FC_MoveFile %s %s",szFrom,szTo);
	int Error=0;
	TBuf16<MAX_FILEMANE> szfileFrom;
	TBuf16<MAX_FILEMANE> szfileTo;
	RFs fsSession;
	
	User::LeaveIfError(fsSession.Connect()); 
	CFileMan* fileMan = CFileMan::NewL(fsSession);
	CleanupStack::PushL(fileMan); 

	FC_CharpToTBuf16(szfileFrom, szFrom);
	FC_CharpToTBuf16(szfileTo, szTo);
	
	if(!(fileMan->Move(szfileFrom,szfileTo, CFileMan::EOverWrite))) //KErrNone if successful
				Error=TRUE;
	
	CleanupStack::PopAndDestroy(); 
	fsSession.Close(); // close file server session

	return Error;
	
}

BOOL FC_DeleteDirectory(char* szDirectory)
{
	//assert(szDirectory);
	//dbg("FC_MoveFile %s %s",szFrom,szTo);
	int Error=0;
	TBuf16<MAX_FILEMANE> szDirectorybuf;
	RFs fsSession;
	
	User::LeaveIfError(fsSession.Connect()); 
	CFileMan* fileMan = CFileMan::NewL(fsSession);
	CleanupStack::PushL(fileMan); 

	FC_CharpToTBuf16(szDirectorybuf, szDirectory);
	
	if(!(fileMan->RmDir(szDirectorybuf))) //KErrNone if successful
				Error=TRUE;
	
	CleanupStack::PopAndDestroy(); 
	fsSession.Close(); // close file server session

	return Error;

}

BOOL FC_EmptyDirectory(char* szDirectory)
{
	return TRUE;
}

BOOL FC_DeleteDiretoryAll(char* szDirectory)
{
	return TRUE;
}

BOOL FC_IsDirectoryExist(char* szDirectory)
{
	if(!szDirectory)
		return FALSE;
	
	TBuf16<MAX_FILEMANE> szFileName;
	BOOL isExist;
	RFs fsSession;

	FC_CharpToTBuf16(szFileName, szDirectory);
	User::LeaveIfError(fsSession.Connect()); 
	isExist=BaflUtils::PathExists(fsSession,szFileName);

	fsSession.Close();
	
	return isExist;
}

//#include <bautils.h>  LIBRARY	bafl.lib 
BOOL FC_IsFileExist(const char* szFile)
{
	//assert(szFile);
	//dbg("FC_MoveFile %s %s",szFrom,szTo);
	int Error=0;
	TBuf16<MAX_FILEMANE> szFileName;
	RFs fsSession;
//	RFile file;
	
	User::LeaveIfError(fsSession.Connect()); 

	FC_CharpToTBuf16(szFileName, szFile);
	/*
	if(KErrNotFound==file.Open(fsSession,szFileName,EFileRead)) //not exist
				Error=TRUE;
	
	file.Close();	
	*/
	Error=BaflUtils::FileExists(fsSession, szFileName);  //need #include <bautils.h>  LIBRARY	bafl.lib 
	fsSession.Close(); // close file server session

	return Error;



}

BOOL FC_CreateDirectory(char* szDirectory)
{
	//assert(szDirectory);
	//dbg("FC_MoveFile %s %s",szFrom,szTo);
	int Error=0;
	TBuf16<MAX_FILEMANE> szDirectorybuf;
	RFs fsSession;
	
	User::LeaveIfError(fsSession.Connect()); 

	FC_CharpToTBuf16(szDirectorybuf, szDirectory);
	if(!(szDirectorybuf[szDirectorybuf.Length()-1]=='\\'))
		szDirectorybuf.Append(_L("\\"));
	if(!(fsSession.MkDirAll(szDirectorybuf))) //KErrNone if successful
				Error=TRUE;
	
	fsSession.Close(); // close file server session

	return Error;

}

#ifdef PCTEST
#define ShortCutCFG "/logs/shortcut.cfg"
//#elif defined(MOTO_E680)
//#define ShortCutCFG "/ezxlocal/download/appwrite/setup/ezx_shortcut.cfg"
#endif

void FC_WriteSysBookmark(char* szFolder, char* szTitle, char* szUrl)
{
//	dbg("FC_WriteSysBookmark %s %s %s",szFolder,szTitle,szUrl);
	
	FILE* f=fopen("d:\\system\\funvv\\bookmark","at");
	if(f)
	{
		fwrite((const char*)szTitle,1,strlen(szTitle),f);
		fwrite(" = ",1,3,f);
		fwrite((const char*)szUrl,1,strlen(szUrl),f);
		fwrite("\n",1,1,f);
		fclose(f);
	}
	else
	{
		;//err("can't open bookmark config.");
	}
}

#ifdef	FC_USE_GPRS

TUint32 FC_findIapID(TInt type);


void GprsCallBack::MtnTcpAfterConnect(TInt aErr)
{
	if(aErr==KErrNone )
	{
		FC_Log("MtnTcpAfterConnect: SockID=%d,  KErrNone	afterConnect(TRUE)",gprsdata);
		if(gprsdata->callbackC->afterConnect)
			gprsdata->callbackC->afterConnect(gprsdata->callbackC->hOwner,TRUE);
		else
			FC_Panic( EGprsNoAfterConnCallback );
	}
	else if(aErr==KErrTimedOut)
	{
		FC_Log("MtnTcpAfterConnect: SockID=%d	 	afterConnect(FC_SOCKET_TIMEOUT)",gprsdata);
			gprsdata->callbackC->afterConnect(gprsdata->callbackC->hOwner,FALSE);
	}
	else
	{
		 FC_Log("MtnTcpAfterConnect:SockID=%d	  ,KErrGeneral	 afterConnect(FALSE)",gprsdata);
	//	 FC_Logsp("MtnTcpAfterConnect:SockID=%d	  ,KErrGeneral	 afterConnect(FALSE)",gprsdata);

		 if(gprsdata->callbackC->afterConnect)
			gprsdata->callbackC->afterConnect(gprsdata->callbackC->hOwner,FALSE);		 	
	}

}

void GprsCallBack::MtnTcpAfterDisconnect(TInt aErr)
{
	CenterCore* playcore=CenterCore::GetInstance();
	GprsConnData** curGprsData=NULL;
/*
	for(int i=0;i<MAX_SOCKET_NUM;i++)
	{
		if( playcore->iGprsData[i] == gprsdata )
		{
			FC_Log("socket%d disconnected.",i);	
		//	FC_Logsp("socket%d disconnected.",i);	
			curGprsData = &playcore->iGprsData[i];
			break;
		}
	}
	*/
	gprsdata->iInUse = 0;
	gprsdata->iReadCount = 0;
	gprsdata->iRetryCount = 0;
	gprsdata->read_hbuf.SetLength(0);
	gprsdata->pBuf = NULL;
//	gprsdata->iNeedRecreate = 1;
	if( gprsdata->iNeedRecreate )
	{
		// create new socket
		GprsConnData* newGprsData;
		newGprsData=new GprsConnData;
		if(!newGprsData)
			return;
	
		newGprsData->callbackC=new GprsCallBack;
		if(!newGprsData->callbackC)
			return;
		
		newGprsData->itcpShell=CTcpShell::NewL(newGprsData->callbackC);	
		if(!newGprsData->itcpShell)
			return;
	
		newGprsData->callbackC->gprsdata=newGprsData;
		newGprsData->type=gprsdata->type;
		newGprsData->read_len=0;
		newGprsData->read_hbuf.SetLength(0);
		newGprsData->iInUse = 0;
		newGprsData->iNeedRecreate = 0;
		newGprsData->iReadCount = 0;
		newGprsData->iRetryCount = 0;
		
	#if defined __WINS__
		newGprsData->itcpShell->SetApnId(0);
		newGprsData->itcpShell->SetTimeOut(180000000);
	#else
		#ifdef FC_DEBUG_LOG
			newGprsData->itcpShell->SetTimeOut(120000000);
		#else
			newGprsData->itcpShell->SetTimeOut(40000000);
		#endif
	
		if(playcore->_http_iap_id)
			newGprsData->itcpShell->SetApnId(playcore->_http_iap_id);
		else
			newGprsData->itcpShell->SetApnId(FC_findIapID(newGprsData->type));
	#endif	
		
		FC_Log("create new socket ok.");
	//	FC_Logsp("create new socket ok.");
		// delete old socket
		if(gprsdata->itcpShell)
		{
			gprsdata->itcpShell->Cancel();
			delete gprsdata->itcpShell;	
		}
		if(gprsdata->callbackC)
			delete gprsdata->callbackC;
		delete gprsdata;
		
		FC_Log("delete old socket ok.");
	//	FC_Logsp("delete old socket ok.");
		// replace socket
		*curGprsData = newGprsData;
	}
}
void GprsCallBack::MtnTcpOnReceive(TInt aErr)
{
	if(aErr==KErrNone||aErr==KErrEof)//read_success_state
	{
		int len = gprsdata->read_len();
		
		if( aErr==KErrEof )
		{
			;//FC_Logsp("MtnTcpOnReceive: SockID=%d, aErr=%d, buflen=%d, total=%d",gprsdata,aErr, len, gprsdata->iReadCount+len);
		}
		
	//	FC_Logsp("MtnTcpOnReceive: SockID=%d  aErr=%d, buflen=%d",gprsdata,aErr, gprsdata->read_hbuf.Length());

	//	FC_Logsp("MtnTcpOnReceive: SockID=%d  aErr=%d, buflen=%d,gprsdata->pBuf=%s",
	//				gprsdata,aErr, gprsdata->read_hbuf.Length(),gprsdata->pBuf);

		//memset(precv,0,ONCE_BUF_LEN);
		
		if(len)
		{
			gprsdata->iReadCount += len;

			char* precv=(char*)(gprsdata->read_hbuf.Ptr());
			memcpy(gprsdata->pBuf, precv, len);
			FC_Log( "MtnTcpOnReceive:afterRecv FC_SOCKET_SUCCESS,readbuf= %s", gprsdata->pBuf );

			gprsdata->read_hbuf.SetLength(0);
			gprsdata->pBuf=NULL;
			gprsdata->iRetryCount = 0;
			gprsdata->callbackC->afterRecv(gprsdata->callbackC->hOwner,FC_SOCKET_SUCCESS, len);
//			FC_Log( "MtnTcpOnReceive:afterRecv FC_SOCKET_SUCCESS,readbuf= %s", gprsdata->pBuf );

		}
		else
		{
			if( gprsdata->iReadCount==0 )
			{
				FC_Log( "MtnTcpOnReceive:retry count = %d, aErr=%d", gprsdata->iRetryCount, aErr );
				FC_Log( "MtnTcpOnReceive:retry count = %d, aErr=%d", gprsdata->iRetryCount, aErr );
				gprsdata->iRetryCount ++;
				if( gprsdata->iRetryCount > 5 )	
				{	
					FC_Log("MtnTcpOnReceive:set re-create socket flag.");
				//	FC_Logsp("MtnTcpOnReceive:set re-create socket flag.");
					gprsdata->iNeedRecreate = 1;
					gprsdata->callbackC->afterRecv(gprsdata->callbackC->hOwner,FC_SOCKET_ERROR, 0);
				}
				else
				{
					User::After (200000);
					gprsdata->callbackC->afterRecv(gprsdata->callbackC->hOwner,FC_SOCKET_TIMEOUT, 0);
					return;
				}
			}
			else
			{
				FC_Log("MtnTcpOnReceive: set re-create socket flag2.");
			//	FC_Logsp("MtnTcpOnReceive: set re-create socket flag2.");
//				gprsdata->iNeedRecreate = 1;
				gprsdata->callbackC->afterRecv(gprsdata->callbackC->hOwner,FC_SOCKET_ERROR, 0);
			}
		}

	}
	else if(aErr==KErrTimedOut)
	{
		FC_Log("MtnTcpOnReceive: SockID=%d	 	afterRecv(FC_SOCKET_TIMEOUT), total=%d",gprsdata, gprsdata->iReadCount);
	//	FC_Logsp("MtnTcpOnReceive: SockID=%d	 	afterRecv(FC_SOCKET_TIMEOUT), total=%d",gprsdata, gprsdata->iReadCount);
			
		if( gprsdata->read_hbuf.Length()>0 )
		{
			gprsdata->read_len() = gprsdata->read_hbuf.Length();
			MtnTcpOnReceive( KErrNone );
		}
		else
		{
			gprsdata->callbackC->afterRecv(gprsdata->callbackC->hOwner,FC_SOCKET_TIMEOUT,0);
		}
	}
	else
	{
		FC_Log("MtnTcpOnReceive: SockID=%d	 	afterRecv(FC_SOCKET_ERROR)",gprsdata);
	//	FC_Logsp("MtnTcpOnReceive: SockID=%d	 	afterRecv(FC_SOCKET_ERROR)",gprsdata);
			gprsdata->callbackC->afterRecv(gprsdata->callbackC->hOwner,FC_SOCKET_ERROR,0);
	}
}

void GprsCallBack::MtnTcpAfterWrite(TInt aErr)
{
	if(aErr==KErrNone)//write_success_state
	{
		FC_Log("MtnTcpAfterWrite: SockID=%d	 	afterSend(FC_SOCKET_SUCCESS)",gprsdata);
		gprsdata->callbackC->afterSend(gprsdata->callbackC->hOwner,FC_SOCKET_SUCCESS,gprsdata->callbackC->len);
	}
	else if(aErr==KErrTimedOut)
	{
		FC_Log("MtnTcpAfterWrite: SockID=%d	 	afterSend(FC_SOCKET_TIMEOUT)",gprsdata);
			gprsdata->callbackC->afterSend(gprsdata->callbackC->hOwner,FC_SOCKET_TIMEOUT,0);
	}

	else
	{
		FC_Log("MtnTcpAfterWrite: 	SockID=%d	 afterSend(FC_SOCKET_ERROR)",gprsdata);
		gprsdata->callbackC->afterSend(gprsdata->callbackC->hOwner,FC_SOCKET_ERROR,0);
	}
		
}
void GprsCallBack::MtnTcpAfterCancel(TInt aErr)
{
	FC_Log("MtnTcpAfterCancel: SockID=%d",gprsdata);
	if(gprsdata->callbackC)
	{
		if(aErr==KErrNone )
			gprsdata->callbackC->afterCancel(gprsdata->callbackC->hOwner, 1);	
		else
			gprsdata->callbackC->afterCancel(gprsdata->callbackC->hOwner, 0);	
	}
}
void GprsCallBack::MtnTcpOnTimeout(TInt aErr)
{
	FC_Log("MtnTcpOnTimeout: SockID=%d",gprsdata);
	gprsdata->iInUse = 0;
}

void FC_findIapName(TUint32 _iapId,TDes & _iapName)
{
//	FC_Logsp("FC_findIapName() iapId=%d",(int)_iapId);
	int _ret=0;
	TUint32 iapid=-1;
	_iapName.SetLength(0);

	CCommsDatabase* const comDB = CCommsDatabase::NewL(EDatabaseTypeIAP);
	CleanupStack::PushL(comDB);
	CCommsDbTableView *myIapTableView = comDB->OpenTableLC(TPtrC(IAP));
	if(myIapTableView->GotoFirstRecord() == KErrNone) 
	{
		myIapTableView->ReadUintL(TPtrC(COMMDB_ID), iapid);
		if(iapid==_iapId)
		{
			myIapTableView->ReadTextL(TPtrC(COMMDB_NAME), _iapName);//GPRS_APN
			CleanupStack::PopAndDestroy(2, comDB);
		//	FC_Logsp("FC_findIapName()  _iapId=%d",(int)_iapId);
			return ;
		}
		
		while(myIapTableView->GotoNextRecord()==KErrNone)
		{	
			myIapTableView->ReadUintL(TPtrC(COMMDB_ID), iapid);
			if(iapid==_iapId)
			{
				myIapTableView->ReadTextL(TPtrC(COMMDB_NAME), _iapName);
				CleanupStack::PopAndDestroy(2, comDB);
			//	FC_Logsp("FC_findIapName()  _iapId=%d",(int)_iapId);
				return ;
			}
		}
	}	
//not find		
	CleanupStack::PopAndDestroy(2, comDB); // for myIapTableView, db release
	return;

}
TUint32 FC_findNextIapName(TUint32 _iapId,TDes & _iapName)
{
//	FC_Logsp("FC_findNextIapName() iapId=%d",(int)_iapId);
	int _ret=0;
	TUint32 iapid=-1;
	_iapName.SetLength(0);

	CCommsDatabase* const comDB = CCommsDatabase::NewL(EDatabaseTypeIAP);
	CleanupStack::PushL(comDB);
	CCommsDbTableView *myIapTableView = comDB->OpenTableLC(TPtrC(IAP));
	if(myIapTableView->GotoFirstRecord() == KErrNone) 
	{
		myIapTableView->ReadUintL(TPtrC(COMMDB_ID), iapid);
		if(iapid==_iapId)
		{
			if(myIapTableView->GotoNextRecord()==KErrNone)
			{
				myIapTableView->ReadUintL(TPtrC(COMMDB_ID), iapid);
				myIapTableView->ReadTextL(TPtrC(COMMDB_NAME), _iapName);
				CleanupStack::PopAndDestroy(2, comDB);
			//	FC_Logsp("FC_findNextIapName()  _iapId=%d",(int)_iapId);
				return iapid;
			}
		}
		
		while(myIapTableView->GotoNextRecord()==KErrNone)
		{	
			myIapTableView->ReadUintL(TPtrC(COMMDB_ID), iapid);
			if(iapid==_iapId)
			{
				_ret=1;
				if(myIapTableView->GotoNextRecord()==KErrNone)
				{
					myIapTableView->ReadUintL(TPtrC(COMMDB_ID), iapid);
					myIapTableView->ReadTextL(TPtrC(COMMDB_NAME), _iapName);
					CleanupStack::PopAndDestroy(2, comDB);
				//	FC_Logsp("FC_findNextIapName()  _iapId=%d",(int)_iapId);
					return iapid;
				}
				else
				{
					myIapTableView->GotoFirstRecord();
					myIapTableView->ReadUintL(TPtrC(COMMDB_ID), iapid);
					myIapTableView->ReadTextL(TPtrC(COMMDB_NAME), _iapName);
					CleanupStack::PopAndDestroy(2, comDB);
				//	FC_Logsp("FC_findNextIapName() _iapId=%d",(int)_iapId);
					return iapid;
					
				}
			}
		}
		
		if(_ret==0)
		{
			myIapTableView->GotoFirstRecord();
			myIapTableView->ReadUintL(TPtrC(COMMDB_ID), iapid);
			myIapTableView->ReadTextL(TPtrC(COMMDB_NAME), _iapName);
			CleanupStack::PopAndDestroy(2, comDB);
		//	FC_Logsp("FC_findNextIapName() _iapId=%d",(int)_iapId);
			return iapid;
		}
	}	

	CleanupStack::PopAndDestroy(2, comDB); // for myIapTableView, db release

	return -1;

}

TUint32 FC_findIapID(TInt type)
{

	CCommsDatabase* const comDB = CCommsDatabase::NewL(EDatabaseTypeIAP);
	CleanupStack::PushL(comDB);
	CCommsDbTableView *myIapTableView = comDB->OpenTableLC(TPtrC(IAP));
	TBuf<100> buf ;
//	TBuf<5> txt ;
	TUint16 chr_meng = 0x68A6 ; // '梦'的内码
	TUint16 chr_wang = 0x7F51 ; // '网'的内码
	TUint32 iapId = 1024 ; // 这个初始化为一个非法的接入点id
	TInt iApPos = 0 ;
	TInt i ;
	FC_Log("FC_findIapID()");
	if(myIapTableView->GotoFirstRecord() != KErrNone) 
	{
		;
	} else 
	{
		do {
			buf.Zero();
			
			myIapTableView->ReadTextL(TPtrC(COMMDB_NAME), buf);
			buf.UpperCase() ;
			
			iApPos = KErrNotFound ;
			
			if(type==1)
			{
				iApPos = buf.Find(_L("CMNET")) ; // Nokia.com AP in other phones ?
				if(iApPos != KErrNotFound) 
				{
					
					myIapTableView->ReadUintL(TPtrC(COMMDB_ID), iapId);
					FC_Log("FC_findIapID()--find CMNET,iapId=%d",(int)iapId);
				break ;
				}
				continue;
			}
			
			for(i = 0 ; i < buf.Length() ; i++) 
			{
				if((TUint16)(buf[i]) == chr_meng) 
				{
					if(((i+1) < buf.Length()) && ((TUint16)(buf[i+1]) == chr_wang)) 
					{
					iApPos = 1 ;
					break ;
					
					}
				}
			}
/*
			//find NET
			iApPos = buf.Find(_L("NET")) ; // any wap are all right
			if(iApPos != KErrNotFound) {
				myIapTableView->ReadUintL(TPtrC(COMMDB_ID), iapId) ;
				FC_Log("FC_findIapID()--find NET,iapId=%d",iapId);
				break ;
			}
*/
			if(iApPos != KErrNotFound) {
				myIapTableView->ReadUintL(TPtrC(COMMDB_ID), iapId) ;
				FC_Log("FC_findIapID()--find 移动梦网iapId=%d",iapId);
				break ;
			}
			
			
			//find WAP
			iApPos = buf.Find(_L("WAP")) ; // any wap are all right
			if(iApPos != KErrNotFound) {
				myIapTableView->ReadUintL(TPtrC(COMMDB_ID), iapId) ;
				FC_Log("FC_findIapID()--find WAP,iapId=%d",iapId);
				break ;
			}

			// find "WLAN”
			iApPos = buf.Find(_L("WLAN3")) ; 
			if(iApPos != KErrNotFound) {
				myIapTableView->ReadUintL(TPtrC(COMMDB_ID), iapId);
				FC_Log("FC_findIapID()--find WLAN,iapId=%d",iapId);

				break ;
			}
		} while (KErrNone == myIapTableView->GotoNextRecord()) ;
	}

	CleanupStack::PopAndDestroy(2, comDB); // for myIapTableView, db release

	return iapId;

	}

HANDLE FC_gprsConnect(BOOL bWap)
{	
	FC_Log("FC_gprsConnect()");
	
	CenterCore* playcore=CenterCore::GetInstance();
	if(!playcore)
		return 0;
		
	GprsConnData * gprsdata;
	bool err = false;	

	gprsdata=new GprsConnData;
	if(!gprsdata)
	{
		err = true;
		goto after_gprs_connect;	
	}

	gprsdata->callbackC=new GprsCallBack;
	if(!gprsdata->callbackC)
	{
		err = true;
		goto after_gprs_connect;
	}
	
	gprsdata->itcpShell=CTcpShell::NewL(gprsdata->callbackC);	
	if(!gprsdata->itcpShell)
	{
		err = true;
		goto after_gprs_connect;
	}

	gprsdata->callbackC->gprsdata=gprsdata;
	gprsdata->type=!bWap?  0:1; // OPTIONS_DAIL_NET : OPTIONS_DAIL_WAP;
	gprsdata->read_len=0;
	gprsdata->read_hbuf.SetLength(0);
	gprsdata->iInUse = 0;
	gprsdata->iNeedRecreate = 0;
	gprsdata->iReadCount = 0;
	gprsdata->iRetryCount = 0;
	
#if defined __WINS__
	gprsdata->itcpShell->SetApnId(0);
	gprsdata->itcpShell->SetTimeOut(60000000);
#else
	#ifdef FC_DEBUG_LOG
		gprsdata->itcpShell->SetTimeOut(120000000);
	#else
		gprsdata->itcpShell->SetTimeOut(40000000);
	#endif

	if(playcore->_http_iap_id)
		gprsdata->itcpShell->SetApnId(playcore->_http_iap_id);
	else
		gprsdata->itcpShell->SetApnId(FC_findIapID(gprsdata->type));
#endif	


after_gprs_connect:	
	if( !err )
	{
		FC_Log("FC_gprsConnect() connected.");
	}
//delete templete	
//	PlayControlAfterConnect( Owner, !err );	
	
	return gprsdata;
	
}

BOOL FC_gprsIsConnect(HANDLE hConnect, BOOL bWap)
{
	FC_Log("FC_gprsIsConnect()");
	GprsConnData * gprsdata=(GprsConnData *)hConnect;

	if(!gprsdata)
			return FALSE;
	return TRUE;
}

void	FC_gprsSetCallBack(HANDLE hConnect, HANDLE hOwner,GPRSConnectCallbackFunc FC_gprsAfterConnect)
{
	FC_Log("FC_gprsSetCallBack()");

}

void FC_gprsReleaseConnect(HANDLE hConnect)
{
	FC_Log("FC_gprsReleaseConnect()");
	
	GprsConnData * gprsdata=(GprsConnData *)hConnect;

	if( gprsdata)
	{	// delete socket
		if(gprsdata->itcpShell)
		{
			gprsdata->itcpShell->Cancel();
			delete gprsdata->itcpShell;	
		}
		if(gprsdata->callbackC)
		{
			delete gprsdata->callbackC;
		//	FC_Log("delete gprsdata->callbackC%d",i);FC_Logsp("delete gprsdata->callbackC%d",i);
		}
		delete gprsdata;
		hConnect = NULL;
	}
}

#ifndef	MULTITHREAD
 void	FC_gprsSetOwner(DWORD hOwner)
{

}
#endif

#endif // FC_USE_GPRS

#ifdef	FC_USE_SOCKET
int		FC_socketCreate(int nSocketID)
{
	FC_Log("FC_socketCreate()");

	GprsConnData * gprsdata=(GprsConnData *)nSocketID;
	if(!gprsdata)
		return 0;
	
	if( gprsdata->itcpShell->IsActive() )
	{
	//	FC_Logsp("socket still active!!!");
		gprsdata->itcpShell->Cancel();
	}
	gprsdata->iInUse = 1;

	FC_Log("leave FC_socketCreate() gprsdata=%lu",gprsdata);
	return  (int)gprsdata;
}

void	FC_socketClose(int nSocketID)
{
	FC_Log("FC_socketClose()");
	if(nSocketID)
	{
		GprsConnData * gprsdata=(GprsConnData *)nSocketID;
		gprsdata->itcpShell->Cancel();
		gprsdata->itcpShell->Disconnect();
	}
}

void	FC_socketSetCancelCallBack(int nSocketID,SocketCancelCallbackFunc fun_gprsAfterCancel)
{
	FC_Log("FC_socketSetCancelCallBack()");
	GprsConnData * gprsdata=(GprsConnData *)nSocketID;
	if( gprsdata)
	{
		gprsdata->callbackC->afterCancel=fun_gprsAfterCancel;
	}
}

void	FC_socketConnect(int nSocketID, char* szIP, int nPort, HANDLE hOwner, SocketConnectCallbackFunc funcSocketConnect)
{
	FC_Log("FC_socketConnect():	nSocketID=%d,szIP=%s",nSocketID,szIP);
	if(nSocketID)
	{
		GprsConnData * gprsdata=(GprsConnData *)nSocketID;

		TBuf<128> servName;
		FC_CharpToTBuf16(servName,szIP);
	/*	
#if defined __WINS__
		gprsdata->itcpShell->SetServerName(_L("60.191.0.157"));
		gprsdata->itcpShell->SetServerPort(8080);	//端口9999

#else
		gprsdata->itcpShell->SetServerName(servName);
		gprsdata->itcpShell->SetServerPort(nPort);	//端口80

#endif*/
		gprsdata->itcpShell->SetServerName(servName);
		gprsdata->itcpShell->SetServerPort(nPort);	//端口80
		gprsdata->callbackC->afterConnect=funcSocketConnect;
		gprsdata->callbackC->hOwner=hOwner;
		
		gprsdata->itcpShell->ConnectL();
		
	}

}
void	FC_socketSend(int nSocketID, char* pBuf, int nBytes, HANDLE hOwner, SocketSendCallbackFunc funcSocketSend)
{	
	FC_Log("FC_socketSend():nSocketID=%d ,nBytes=%d,pBuf=%s",nSocketID,nBytes,pBuf);
	if(nSocketID)
	{
		FC_Log("FC_socketSend()::	nBytes=%d,pBuf=%s",nBytes,pBuf);
			GprsConnData * gprsdata=(GprsConnData *)nSocketID;
	
			gprsdata->pDes.Set( (const TUint8*)pBuf , nBytes); 
			gprsdata->callbackC->len=nBytes;
			gprsdata->callbackC->hOwner=hOwner;
			gprsdata->callbackC->afterSend=funcSocketSend;
	//	{
	//	HBufC8 *msg_hbuf = HBufC8::NewL(1024);
	//	TPtr8 msg = msg_hbuf->Des();
	//	
	//	msg.Append( _L("GET http://wap.funvio.com HTTP/1.0\r\n") );
	//	msg.Append( _L("Host: wap.funvio.com:80\r\n") );
	//	msg.Append(_L("Accept: */*\r\n"));
	//	msg.Append(_L("Accept-Charset: gb2312, iso-8859-1, us-ascii, utf-8\r\n"));
	//	msg.Append(_L("Accept-Language: zh-ch, zh, en\r\n"));
	//	msg.Append(_L("Connection: close\r\n"));
	//	msg.Append(_L("Range: bytes=0-102399\r\n"));
	//	msg.Append(_L("User-Agent: Nokia N70\r\n"));
	//	delete msg_hbuf;
	//	}
		if(	!gprsdata->itcpShell->Write(gprsdata->pDes) )
		{
			funcSocketSend(hOwner, FC_SOCKET_ERROR, 0);	
		}
	}
}
void	FC_socketRecv(int nSocketID, char* pBuf, int nBytes, HANDLE hOwner, SocketRecvCallbackFunc funcSocketRecv)
{
	//FC_Log("FC_socketRecv(),nSocketID=%d,nBytes=%d",nSocketID,nBytes);
	//FC_Logsp("FC_socketRecv(),nSocketID=%d,nBytes=%d",nSocketID,nBytes);
	
	if(nSocketID)
	{
		GprsConnData * gprsdata=(GprsConnData *)nSocketID;

		gprsdata->pBuf=pBuf;
		gprsdata->callbackC->afterRecv=funcSocketRecv;
		gprsdata->callbackC->hOwner=hOwner;
		gprsdata->read_hbuf.SetLength(0);
		gprsdata->read_len() = 0;
		#if 1
		if( !gprsdata->itcpShell->Read(gprsdata->read_hbuf,gprsdata->read_len, 10000000) )
		//if( !gprsdata->itcpShell->ReadBlock( gprsdata->read_hbuf,10000000, gprsdata->read_len ) )
		#else
		if( !gprsdata->itcpShell->ReadBlock(gprsdata->read_hbuf,10000000) )
		#endif
		{
			funcSocketRecv(hOwner, FC_SOCKET_ERROR, 0);
		}	
	}
}

#endif // USE SOCKET

#ifdef	FC_USE_DLL

HANDLE _FC_LoadLibrary(const char* szDll)
{
	return NULL;
}

FC_FARPROC _FC_GetProcAddress(HANDLE hHandle, const char* szProcName)
{
	return NULL;
}

BOOL FC_FreeLibrary(HANDLE hHandle)
{
	return TRUE;
}

#endif


ECodeType  FC_TextHeadParse(const TDesC& _url,int & _offset)
{
	ECodeType	result=ETextCodeANSI;  /* 1	 -utf8 ;						EF BB BF
							2 	UTF-16/UCS-2, little endian		FE FF
							3	UTF-16/UCS-2, big endian		FF FE
							4	UTF-32/UCS-4, little endian.		FF FE 00 00
							5	UTF-32/UCS-4, big-endian.       	00 00 FE FF
							*/
	_offset=0;

	RFs			m_fs;
	RFile			m_file;
	TBuf8<8>	m_head;
	char bitarr[4];
	int a=(int)0x2029;
	User::LeaveIfError(m_fs.Connect());
	if (KErrNone != m_file.Open (m_fs,_url,EFileShareReadersOnly )) 
		return ETextCodeUnKnow;
	
	m_file.Read(0,m_head,8);
	char *p=(char *)m_head.Ptr();

	bitarr[0]=*p++;
	bitarr[1]=*p++;
	bitarr[2]=*p++;
	bitarr[3]=*p++;

	if(bitarr[0]==(char)0xEF && bitarr[1]==(char)0xBB && bitarr[2]==(char)0xBF)
		{result=ETextCodeUTF8;	_offset=3;}
	else if(bitarr[0]==(char)0xFE && bitarr[1]==(char)0xFF )  //UTF-16/UCS-2, little endian
		{result=ETextCodeUTF16L;	_offset=2;}
	else if(bitarr[0]==(char)0xFF && bitarr[1]==(char)0xFE && (bitarr[2] || bitarr[3]))
		{result=ETextCodeUTF16B;	_offset=2;}
	else if(bitarr[0]==(char)0xFF && bitarr[1]==(char)0xFE && bitarr[2]==(char)0x00 && bitarr[3]==(char)0x00)
		{result=ETextCodeUTF32L;	_offset=4;}
	else if(bitarr[0]==(char)0x00 && bitarr[1]==(char)0x00 && bitarr[2]==(char)0xFE && bitarr[3]==(char)0xFF)
		{result=ETextCodeUTF32B;	_offset=4;}

	if(result==ETextCodeANSI)  //maybe is UTF8 no BOM
	{
		TInt 	filsize;
		TInt count_bad_utf=0,count_good_utf=0 ;
		char current_byte,previous_byte;
		HBufC8 *_read_buf = HBufC8::NewL (5*1024);
		TPtr8 _read_tptr8 = _read_buf->Des ();
		p=(char *)_read_tptr8.Ptr();
			
		m_file.Size(filsize);
		if(filsize>1024*5)
			filsize=1024*5;

		m_file.Read(0,_read_tptr8,filsize);
		TInt i=0;
		while(i<filsize-1)
		{
			previous_byte=*p++;
			current_byte=*p;
			
			if ((current_byte & 0xC0) == 0x80)
			{
				if ((previous_byte & 0xC0) == 0xC0 )
				{
					count_good_utf ++;
				} 
				else if( (previous_byte & 0x80) == 0x00)
				{
					count_bad_utf ++;
				}
			} 
			else if ((previous_byte & 0xC0) == 0xC0 )
			{
					count_bad_utf ++;
			 }
			i++;
		}

		if ( count_good_utf >= count_bad_utf)
		{
			result=ETextCodeUTF8;
			_offset=0;
		}

		delete _read_buf;

	}
	
	m_file.Close();
	m_fs.Close();

	return result;
}

int str_replace(char* szInput, const char* szOld, const char* szNew)
{
	int		nOldLength;
	int		nSourceLen;
	int		nReplacementLen;
	int		nCount = 0;
	char*	pStart;
	char*	pEnd;
	char*	pTarget;

	nSourceLen = strlen(szOld);
	if( nSourceLen == 0 )
		return 0;

	nOldLength = strlen( szInput);
	nReplacementLen = strlen( szNew);

	pStart = szInput;
	pEnd = szInput + strlen( szInput);

	while( pStart < pEnd )
	{
		while( (pTarget = strstr(pStart, szOld)) != NULL )
		{
			int		nBalance = nOldLength - ( pTarget - szInput + nSourceLen);
			memmove( pTarget + nReplacementLen, pTarget + nSourceLen, nBalance);
			memcpy( pTarget, szNew, nReplacementLen);
			
			pStart = pTarget + nReplacementLen;
			pStart[nBalance] = '\0';
			nOldLength += (nReplacementLen - nSourceLen);
			nCount++;
		}
		pStart += strlen( pStart) + 1;
	}

	return nCount;
}

TInt  FC_TextTrim(TDesC8& _szbuf)
{
/*
	char *p=(char *)_szbuf.Ptr();
	int ret=str_replace(p,"\r\n","\r\f");
	return ret;
	*/
	return 0;
}

TInt  FC_TextTrimL(TDes& _szbuf)
{

	TInt foundAt;
//	TChar  a =TChar(0x2029);
	TBuf<1>	as;
	as.Append(TChar(0x2029));
	while ((foundAt= _szbuf.Find(_L("\n")) )!= KErrNotFound)
	{
		_szbuf.Delete(foundAt,1);
		_szbuf.Insert(foundAt,as);
	//	_szbuf.Replace(foundAt, 1,as);
	}
 

	return 0;
}
