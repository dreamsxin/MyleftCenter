
#ifndef	_PORTAB_SYMBIAN_H_
#define	_PORTAB_SYMBIAN_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <assert.h>
#include <e32std.h>
#include "symbian_def.h"

#undef IS_LITTLE_ENDIAN
#undef IS_BIG_ENDIAN

typedef unsigned long       DWORD; //4//4
typedef int                 BOOL; // 4
typedef int			CBOOL;// 4
typedef unsigned char       BYTE; // 1
typedef unsigned short      WORD;// 2
typedef	void*				HANDLE;
//typedef	__int64			int64; //8

#define MAX_FILEMANE	216
#define TARGET_SYMBIAN
#define UNICODE
#define FC_DEBUG_LOG   
#define FC_SOCKET_LOG
#define   _SINA_PAY_NOT_FREE_
#define _SUPPORT_MUSIC_PAY_
#define _ADD_CHARROOM_                //chatroom
//#define  _USE_PLAY_BACK_IMG

#define FC_BACKSLASH
#define FC_USE_AO
#define _USE_GPRSDATA_ARR_
#ifdef __MARM__
#define ARM
#endif // end __MARM__

#define DLLEXPORT
#define DLLIMPORT
#define CALLBACK
#define	STDCALL

#define	FC_SUPPORT_AUDIO_NUM		1 //unknow
extern const int	FC_SUPPROT_AUDIO[FC_SUPPORT_AUDIO_NUM];

void* FC_malloc(DWORD size);
#define FC_free( cell)			free(cell)


#define	FC_open(szFile, szMode)		fopen(szFile, szMode)
#define	FC_close(hFile)				fclose( (FILE*)hFile)
#define	FC_read(hFile,pData,dwLen)	fread( pData, 1, dwLen, (FILE*)hFile)
#define	FC_write(hFile,pData,dwLen)	fwrite( pData, 1, dwLen, (FILE*)hFile)
#define	FC_tell(hFile)				ftell( (FILE*)hFile)
#define	FC_seek(hFile,nOffset,nPos)	fseek( (FILE*)hFile, nOffset, nPos)

#define	FC_Sleep(dwMS)				sleep(dwMS)
#undef ASSERT
#define ASSERT(cond)				assert(cond)

void FC_AsciiToUnicodeL (TDes16& _unicodeBuffer,const TDesC8& _asciiBuffer) ;
void FC_UnicodeToAsciiL (TDes8& _asciiBuffer,const TDesC16& _unicodeBuffer) ;
void FC_Utf8ToUnicodeL (TDes16& _unicodeBuffer,const TDesC8& _utf8Buffer) ;
void FC_UnicodeToGbkL (TDes8& _asciiBuffer,const TDesC16& _unicodeBuffer) ;
char* FC_TBuf16ToCharp(char* szchar,const TDesC& szDes16 );
TDes* FC_CharpToTBuf16(TDes& TBuff,const char* szPath );
void FC_GbkToUnicodeL (TDes16& _unicodeBuffer,const TDesC8& _asciiBuffer) ;
int FC_GbkToUtf8( const char* szGbk , char** szUtf8 );
void FC_UnicodeToUtf8L(TDes8& _utf8Buffer,const TDesC16& _unicodeBuffer);
int FC_Utf8ToGbk( const char* szUtf8 , char** szGbk  );
int FC_GbkToUnicode( const char* szGbk , char** szUnicode );
int FC_ACPToUtf8(const char* szAcp, char** szUtf8);

HANDLE	FC_FindFirstFile(char* szPath, char* szPattern, HANDLE* phFindData);
BOOL	FC_FindNextFile( HANDLE hFind, HANDLE* phFindData);
void	FC_FindClose( HANDLE hFind, HANDLE hFindData);
BOOL	FC_FindDataIsDirectory(HANDLE hFindData);
void	FC_FindDataGetFileName(HANDLE hFindData, char* szName);

void	FC_GetDateTime(DWORD* pYYYYMMDD, DWORD* pHHMMSS);
BOOL	FC_IsValidDateTime(DWORD dwYYYYMMDD, DWORD dwHHMMSS);

//#define MULTITHREAD

#define PTHREAD
#define ARM

#define FC_USE_GPRS
#define FC_USE_SOCKET
//#define FC_USE_DLL

#ifdef FC_USE_DLL
#define SOURCE_USE_DLL
#define DECODER_USE_DLL
#endif


//#include <pthread.h>
#include <unistd.h>


#if 0



//#include <pthread.h>
#include <unistd.h>

#define linux_thread_join(h) _linux_thread_join(h)
#define linux_lock_enter(p) _linux_lock_enter(p)

void* linux_thread_create(void*(*lpStartAddress)(void *), void* lpParameter, int* lpThreadId);
void linux_thread_destroy(void* hThread);
void _linux_thread_join(void* hThread);
void* linux_lock_create();
void linux_lock_destroy(void* p);
void _linux_lock_enter(void* p);
void linux_lock_leave(void* p);

#endif

void*FC_CreateThread(DWORD lpStartAddress, void*lpParameter, DWORD*lpThreadId); 
void FC_CloseThread(void*hThread);
void	FC_WaitThreadEnd(HANDLE hThread);

void*LockCreate();
void	LockDelete(void*);
void	LockEnter(void*);
void	LockLeave(void*);


#if defined(ARM) || defined(MIPS) || defined(SH3) || defined(_M_IX86)
#define IS_LITTLE_ENDIAN
#else
#define IS_BIG_ENDIAN
#endif

#define MAX_INT	0x7FFFFFFF
#define MAXPATH 256
#define MAXMIME 16

#define DLLEXPORT
#define DLLIMPORT
#define	CALLBACK
#define	STDCALL
#define LPVOID		void*
#define WINAPI
#define FAR

#define FOURCCBE(a,b,c,d) \
	(((BYTE)(a) << 24) | ((BYTE)(b) << 16) | \
	((BYTE)(c) << 8) | ((BYTE)(d) << 0))

#define FOURCCLE(a,b,c,d) \
	(((BYTE)(a) << 0) | ((BYTE)(b) << 8) | \
	((BYTE)(c) << 16) | ((BYTE)(d)<< 24))

#ifdef IS_BIG_ENDIAN
#define FC_FOURCC(a,b,c,d) FOURCCBE(a,b,c,d)
#else
#define FC_FOURCC(a,b,c,d) FOURCCLE(a,b,c,d)
#endif

#ifndef min
#  define min(x,y)  ((x)>(y)?(y):(x))
#endif

#ifndef TRUE
#	define	TRUE	1
#endif

#ifndef FALSE
#	define	FALSE	0
#endif

#ifndef max
#  define max(x,y)  ((x)<(y)?(y):(x))
#endif

#ifndef sign
#  define sign(x) ((x)<0?-1:1)
#endif


#define SAFE_FREE(x)		{ if(x) FC_free(x); (x) = NULL;		}

//#define FC_Log(fmt,args...) do{fprintf(stderr,"[%s@%d]",__FILE__,__LINE__);_FC_Log(fmt,##args);}while(0);

void FC_Panic(TInt code);
int FC_LogT(const char *fmt,TDes& TBuff);//#include <stdarg.h>
int FC_Log(const char *fmt,...);
void FC_FreeRam(const char* countent);
int FC_Logh246(const char *fmt,...);//#include <stdarg.h>

int FC_Logsp(const char *fmt,...);//#include <stdarg.h>
#define	FCMB_OK				0
#define	FCMB_OKCANCEL			1
#define	FCMB_MB_YESNO			2

#define	FCMB_ICONERROR			0
#define	FCMB_ICONQUESTION		1
#define	FCMB_ICONINFORMATION		2

int	FC_MessageBox(const char* szText, const char* szTitle, DWORD dwType, DWORD dwIcon);

#if 1

void	FC_SendSMS(char* szTo, char* szMessage);
void	FC_OpenIE(char* szUrl);


#endif


void		FC_GetSerialNumber(DWORD* pHigh, DWORD* pLow);
int		FC_UnicodeToUtf8(char* szUnicode, char** pszUtf8);
int		FC_Utf8ToUnicode(char* szUtf8, char** pszUnicode);

BOOL	FC_DeleteFile(const char* szFile);
BOOL	FC_MoveFile(const char* szFrom, const char* szTo);
BOOL	FC_DeleteDirectory(char* szDirectory);
BOOL	FC_EmptyDirectory(char* szDirectory);
BOOL	FC_DeleteDiretoryAll(char* szDirectory);
BOOL	FC_IsDirectoryExist(char* szDirectory);
BOOL	FC_IsFileExist(const char* szFile);
BOOL	FC_CreateDirectory(char* szDirectory);


void	FC_WriteSysBookmark(char* szFolder, char* szTitle, char* szUrl);



#ifdef	FC_USE_GPRS

#include	"tcpshell.h"


#define ONCE_BUF_LEN  512
class GprsCallBack;

typedef struct
{
	CTcpShell * itcpShell;
//	TRequestStatus iStatus;
	GprsCallBack * callbackC;
	char* 		pBuf;
	TBuf8<ONCE_BUF_LEN>	read_hbuf ;
	TInt 			type;
	TPtrC8  		pDes; 
	TSockXfrLength 	read_len;

	int iInUse;
	int iNeedRecreate;
	int iReadCount;
	int iRetryCount;

}GprsConnData;



HANDLE	FC_gprsConnect(BOOL bWap);
void	FC_gprsReleaseConnect(HANDLE hConnect);
BOOL	FC_gprsIsConnect(HANDLE hConnect, BOOL bWap);
#ifndef	MULTITHREAD
 void		FC_gprsSetOwner(DWORD hOwner);
#endif

typedef void	(*GPRSConnectCallbackFunc)(HANDLE pOwner, int nStatus);
void	FC_gprsSetCallBack(HANDLE hConnect, HANDLE hOwner,GPRSConnectCallbackFunc FC_gprsAfterConnect);

//int  FC_findIapCount();
void FC_findIapName(TUint32 _iapId,TDes & _iapName);
TUint32 FC_findNextIapName(TUint32 iapId,TDes & iapName);
TUint32 FC_findIapID(TInt type);
/*
class FCGPrsNotifier:public  MGPrsNotifier
{
public:
	 void MtnGPrsAfterConnect(void* contr,TBool aErr);
	 void MtnGPrsAfterCancel(void* contr,TInt aErr) ;
	 void MtnGPrsOnTimeout(void* contr,TInt aErr) ;
};
*/
#endif	// FC_USE_GPRS


#ifdef	FC_USE_SOCKET
#include <sys/socket.h>

#define	FC_SOCKET_SUCCESS	1
#define	FC_SOCKET_TIMEOUT	2
#define	FC_SOCKET_ERROR		3

#define FC_SOCKET_TCP		SOCK_STREAM
#define FC_SOCKET_UDP		SOCK_DGRAM

int		FC_socketCreate(int nSocketID);
void	FC_socketClose(int nSocketID);

typedef void	(*SocketConnectCallbackFunc)(HANDLE pOwner, BOOL bSuccess);
typedef void	(*SocketCancelCallbackFunc)(HANDLE pOwner, BOOL bSuccess);
typedef void	(*SocketSendCallbackFunc)(HANDLE pOwner, int nStatus, int nLen);
typedef void	(*SocketRecvCallbackFunc)(HANDLE pOwner, int nStatus, int nLen);

void	FC_socketConnect(int nSocketID, char* szIP, int nPort, HANDLE hOwner, SocketConnectCallbackFunc funcSocketConnect);
void	FC_socketSend(int nSocketID, char* pBuf, int nBytes, HANDLE hOwner, SocketSendCallbackFunc funcSocketSend);
void	FC_socketRecv(int nSocketID, char* pBuf, int nBytes, HANDLE hOwner, SocketRecvCallbackFunc funcSocketRecv);
void	FC_socketSetCancelCallBack(int nSocketID,SocketCancelCallbackFunc fun_gprsAfterCancel);

class GprsCallBack:public  MTcpNotifier
{
    public: // Functions from base classes
			void MtnTcpAfterConnect(TInt aErr);
			void MtnTcpAfterDisconnect(TInt aErr);
			void MtnTcpOnReceive(TInt aErr);
			void MtnTcpAfterWrite(TInt aErr);
			void MtnTcpAfterCancel(TInt aErr);
			void MtnTcpOnTimeout(TInt aErr);
			
	public:  	
		GprsConnData * gprsdata;
		HANDLE 			hOwner;
		TInt 				len;
		GPRSConnectCallbackFunc			afterGPRSConnect;
		SocketConnectCallbackFunc			afterConnect;
		SocketSendCallbackFunc			afterSend;
		SocketRecvCallbackFunc			afterRecv;
		SocketCancelCallbackFunc			afterCancel;
};
#endif	//FC_USE_SOCKET


#ifdef	FC_USE_DLL

#define FC_LoadLibrary(h) _FC_LoadLibrary(h);do{fprintf(stderr,"[%s@%d]",__FILE__,__LINE__);}while(0);
#define FC_GetProcAddress(h,n) _FC_GetProcAddress(h,n);do{fprintf(stderr,"[%s@%d]",__FILE__,__LINE__);}while(0);


typedef int (FAR WINAPI *FC_FARPROC)();

HANDLE		_FC_LoadLibrary(const char* szDll);
FC_FARPROC	_FC_GetProcAddress(HANDLE hHandle, const char* szProcName);
BOOL		FC_FreeLibrary(HANDLE hHandle);


#endif //end ifdef FC_USE_DLL

void _chkstk(); 



typedef enum
{
	ETextCodeUnKnow=-1,
	ETextCodeANSI=0,     //  GB18030
	ETextCodeUTF8,			// 	1	 -utf8 ;						EF BB BF
	ETextCodeUTF16L,		//	2 	UTF-16/UCS-2, little endian		FE FF
	ETextCodeUTF16B,		//	3 	UTF-16/UCS-2, big endian			FF FE
	ETextCodeUTF32L,		//	4	UTF-32/UCS-4, little endian.		FF FE 00 00	
	ETextCodeUTF32B			//	5	UTF-32/UCS-4, big-endian.       00 00 FE FF			
}ECodeType;


ECodeType  FC_TextHeadParse(const TDesC& _url,int & _offset);
TInt  FC_TextTrim(TDesC8& _szbuf);
TInt  FC_TextTrimL(TDes& _szbuf);
#endif // _PORTAB_SYMBIAN_H_

