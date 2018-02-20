
#ifndef		_HTTPGET_H_
#define		_HTTPGET_H_

#define	GET_STATUS_STOP			0
#define	GET_STATUS_REQUESTING	1
#define	GET_STATUS_REQUESTED	2
#define	GET_STATUS_READED		3

#define	MAX_SOCKET_TRY_NUM		6
#define	MAX_HTTP_HEAD_LEN		5120


#if 0
typedef	struct	_SHttpGet	SHttpGet;

#include	"DataBuf.h"

#ifndef MULTITHREAD
typedef void	(*HttpSourceCallbackFunc)(HANDLE pOwner);
#endif

struct _SHttpGet
{
	int			m_nStatus;
	char		m_szUrl[MAXPATH];
	char		m_szReferer[MAXPATH];
	char		m_szPost[MAXPATH];
	CBOOL		m_bIsMoved;
	DWORD		m_dwFileLength;
	DWORD		m_dwCurRead;

	CBOOL		m_bStop;
	int			m_nSocketID;

	char		m_szServerAddr[MAXPATH];
	char		m_szObject[MAXPATH];
	DWORD		m_dwServerHttpPort;

	char		m_szProxy[32];
	int			m_nProxyPort;

	DWORD		m_dwResponseSaveLen;
	char		m_pResponseSave[MAX_HTTP_HEAD_LEN];

	char*		m_szRequestHeader;
	char*		m_szResponseHeader;

	int			m_nLoopTry;
	int			m_nTryNum;
	CBOOL		m_bSendAgain;
	DWORD		m_dwFrom;
	DWORD		m_dwTo;
	DWORD		m_dwLenSave;

	SDataBuf*	m_pDataBuf;
	CBOOL		m_bThreadRunning;
	HANDLE		m_hThread;
	DWORD		m_dwThreadID;

#ifndef MULTITHREAD
	HttpSourceCallbackFunc	m_funcHttpSourceCallback;
	HANDLE					m_hOwner;

	CBOOL		m_bReadRespone;
	char*		m_pWriteBuf;
	DWORD		m_dwNeedWrite;
	DWORD		m_dwCurWrite;

	CBOOL		m_bReadError;
	char*		m_pReadBuf;
	DWORD		m_dwNeedRead;
	DWORD		m_dwReaded;
#endif
};

//void	HttpGetInit(SHttpGet* pHttpGet, char* szUrl, char* szProxy, int nPort);
void	HttpGetInit(SHttpGet* pHttpGet, char* szUrl, char* szProxy, int nPort, char* szReferer, char* szPost);
void	HttpGetUnInit(SHttpGet* pHttpGet);

CBOOL	HttpGetStart(SHttpGet* pHttpGet, SDataBuf* pDataBuf);
void	HttpGetStop(SHttpGet* pHttpGet);

CBOOL	HttpGetIsAlive(SHttpGet* pHttpGet);
CBOOL	HttpGetIsMoved(SHttpGet* pHttpGet);
DWORD	HttpGetGetFileLength(SHttpGet* pHttpGet);
char*	HttpGetGetMovedUrl(SHttpGet* pHttpGet);
DWORD	HttpGetGetCurRead(SHttpGet* pHttpGet);
DWORD	HttpGetGetStatus(SHttpGet* pHttpGet);

#ifndef MULTITHREAD
void	HttpGetSetCallback( SHttpGet* pHttpGet, HANDLE hOwner, HttpSourceCallbackFunc funcHttpSourceCallback);

void	HttpGetGetThreadBranch(SHttpGet* pHttpGet);

void	HttpGetSendRequestLoop(SHttpGet* pHttpGet);
void	HttpGetAfterSendRequest(SHttpGet* pHttpGet, CBOOL bSuccess);

void	HttpGetConnectServerBranch(SHttpGet* pHttpGet);
void	HttpGetAfterConnect(HANDLE pOwner, CBOOL bSuccess);

void	HttpGetSocketReadBranch(SHttpGet* pHttpGet);
void	HttpGetAfterSocketRead(HANDLE pOwner, int nStatus, int nLen);

void	HttpGetSocketWriteBranch(SHttpGet* pHttpGet);
void	HttpGetAfterSocketWrite(HANDLE pOwner, int nStatus, int nLen);

void	HttpGetSocketReadResponseBranch(SHttpGet* pHttpGet, char* pBuf, DWORD dwLen);
void	HttpGetAfterSocketReadResponse(SHttpGet* pHttpGet, CBOOL bSuccess);
#endif

#endif

#define EV_TYPE_FORE_GETWAY_ERR -3
#define EV_TYPE_FORE_GETWAY_OK -2
#define EV_TYPE_ERR -1
#define EV_TYPE_PING 0
#define EV_TYPE_AUTH 10
#define EV_TYPE_AUTH_OTHER_LOGIN 11 //是同一个用户在其他地方登陆
#define EV_TYPE_USER_LOGIN 20
#define EV_TYPE_USER_LOGOUT 21
#define EV_TYPE_USER_ADD 22  //已经在线的人 名单 
#define EV_TYPE_MESSAGE 30


#define MAX_BUFFER_LEN  512 
#define MAX_SAVE_BLOCK   5

class CChartRGetter : public MFunTimeOutNotify,public CBase
						//, public MChatRGetterObserver
{
public: // Constructors and destructor
	/**
	* Destructor.
	*/
enum TLatestThing {
		do_Nothing,		//0
		do_Longin,		// 1

		do_Receive,		// 2
		do_Send,
		};
	
	~CChartRGetter();

	/**
	* Two-phased constructor.
	*/
	static CChartRGetter* NewL(char *_szProxy,int _szProxyPort,char * _szUsername,char* _szPassword);

protected:
	/**
	* Constructor for performing 1st stage construction
	*/
	CChartRGetter();

	/**
	* EPOC default constructor for performing 2nd stage construction
	*/
	void ConstructL(char *_szProxy,int _szProxyPort,char * _szUsername,char* _szPassword);
	void MftonTimerExpired() ;		//MFunTimeOutNotify
public:
	/*
	From MDownloadShellNotifier
	*/
//	virtual void MdsDownloaderError( TInt aErr );
//	virtual void MdsDownloaderMessage( TInt aResId );
//	virtual void MdsDownloaderComplete( TInt aErr );
//	virtual void MdsInstallNotifyComplete( TInt aErr );
//	virtual TInt MdsGetProtected();
//	virtual void MdsAfterUserSelectIAP(TInt aErr, TInt aIap);
	void StartTimer() ;

public:
	CenterCore* iCenterCore;
	 GprsConnData* iGprsData;
	 CFunTimeOutTimer* iTimer;
	 
	char		m_szProxy[32];
	int		m_nProxyPort;
	char		m_szServerAddr[MAXPATH];

	int 		m_msgType;
	char *	m_sendbuf;
	char*	m_dwCurRead;  // 512
	char*	m_RecvBuf;   //all received   5*512
	TLatestThing	m_latest;
//	int 		m_dwCurWrite;
	int 		m_nTryNum;
	TBool	m_isLocked;

	char *	 _username;
	char* 	password;
//	MHttpGetterObserver* iObserver;
	// 
public:
	TInt Connect() ;
	void Close();
	void Receive();
	void Send( char* pBuf, int nBytes);
	void SendLogin();
	void SendMessage( char * number,char* ptext);
	void SendMessage(const TDesC& _name,const TDesC& _msg);
	void SendMessageAgain();
	
	void ReceivedThreadMgr();
	TInt GetEventTpye(char * _szMessage);
	void AppendToRecvbuf();
	TInt GetMsgFormRecvbuf(char * content);
};



#endif // _HTTPGET_H_
