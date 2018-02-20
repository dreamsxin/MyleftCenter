#ifndef _CERTERCORE_H_
#define _CERTERCORE_H_


#include "portab_symbian.h"
#include <aknprogressdialog.h> 
#include <e32base.h>
#include <fbs.h>
#include <EIKFUTIL.H>
#include <AknWaitDialog.h> 
#include <MSVAPI.H>
#include "MyleftCenterAppUi.h"
#include "MdaAudioShell.h"
#include "sxml.h"
#include "FunUIStart.h"
#include "LrcSubtitle.h"
#include "chatroomtask.h"

#define 	MAX_SOCKET_NUM 	1

// PLAYER STATUS
#define	CCCB_STATUS_STOP		0
#define	CCCB_STATUS_PLAY		1
#define	CCCB_STATUS_PAUSE	2
#define	CCCB_STATUS_WAIT		3

#define  MYLEFG_API_SPECIALS	0
#define  MYLEFG_API_CHANNELS	1
#define  MYLEFG_API_CLASSIFIES	2

#define  MYLEFG_API_TYPE_MUSIC	1
#define  MYLEFG_API_TYPE_TXT	2
#define  MYLEFG_API_TYPE_LRC	3
#define  MYLEFG_API_TYPE_PIC	4

typedef struct
{
	int 	http_iap_id;
	TBuf<32> _username;
	TBuf<32> _nickname;
	TBuf<32> _password;
	TBuf<32> _userid;
//old	
/*
	int httpType;
	char defaultpath[125];	
	int  volume;
	int outtime;
	bool backrightOn;
	bool autopauser;
	bool music;
	*/
}center_setting;



//--------for  read-------beg----------------
typedef struct
{
	int R;
	int G;
	int B;
}s_color;

typedef struct
{
	bool 		bBcakPic;
	bool 		autoRool;
	bool 		downPic;
	int			autoTime;
	int 			textSize;
	int 			backPicId;
	s_color		textColor;
	s_color		backColor;
}reader_setting;

typedef struct
{
	TBuf<160>    	iaskurl;
	TBuf<64>  	title;
}read_list;

typedef struct
{
	TBuf<160>    	iaskurl;
	TBuf<64>  	title;
	TInt 			iPos;
}read_masklist;

typedef struct
{
	TInt 			classes;
	TInt 			Id;
	TInt 			typeId;
	TBuf<64>  	title;
}read_bdclass;

typedef struct
{
	TInt 			Id;
	TBuf<64>  	name;
	TBuf<32>  	auth;
	TInt			iPicID;
}book;
//--------for  music-------beg----------------
typedef struct
{
	bool 		backrightOn;
	int 			playMode;//0 sunxu ,1 xunhuan ,2 suiji
	int  			volume;
	bool 		downPic;
	bool 		downLrc;
}music_setting;

typedef struct
{
	TBuf<256>    	iaskurl;
	TBuf<64>  	title;
}music_list;

typedef struct
{
	TInt 			classes;
	TInt 			Id;
	TInt 			typeId;
	TBuf<64>  	title;
}music_bdclass;


typedef struct
{
	TBuf<256>    iaskurl;
	TInt			iPicId;
	TInt			iLrcId;
	TBuf<64>  	name;
	TBuf<32>  	singer;
	TInt  id;
}song;
//--------for  music-------end----------------
//--------for  download-------beg----------------
typedef enum
{
	EdownloadIng=0,
	EdownloadOk,
	EdownloadErr,
	EdownloadReady
}TdownloadStatus;
typedef struct
{
	TBuf<64>  	filename;
	TBuf<256>    iaskurl;
	TInt  		id;
	TInt 			size;
	TInt			length;
	TInt			typeId;
	TdownloadStatus		status;	//0 =ok,-1=err
}download;
//--------for  download-------end----------------
//--------for  chatroom-------beg----------------

#define   MAX_BODY_CHARLOG_MUN   10
#define   MAX_CHATROOM_LOG_MUN   30


typedef struct
{
	TBuf<32>  	name;
}chatR_body;

typedef struct
{
	DWORD  		time;
	TBuf<32>  	name;
	HBufC*     	a_chatlog;  
}chatR_log;


//--------for  chatroom-------end----------------

const char* const appPath="C:\\Myleft";
const char*  const dataPath="C:\\data\\Myleft";

const char* const schedulerBM="C:\\funSMusicScheduler.dat";

class	CSinaInterfaceMgr;
class   CMdaAudioShell;

class CenterCore :public CBase,public MMsvSessionObserver 
{
	
public:
	static CenterCore* NewL(const TDesC& _rootpath);
	static CenterCore* GetInstance();
	CenterCore();
	~CenterCore();
	void InitData(const TDesC& _rootpath);

	CSinaInterfaceMgr * iSinaMgr;
	CSinaInterfaceMgr * iDownMgr;
	
	//base from MMsvSessionObserver
	void HandleSessionEventL( TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3 ){}

//***************for view show---Begin-----------
public:
	CMyleftCenterAppUi* m_appUI; //CMyleftCenterAppUi
	center_setting				m_centerSetting;
	reader_setting				m_readerSetting;
	music_setting				m_musicSetting;

	TInt			m_mainFouceId;
	TUid			m_readViewid;
	TUid			m_mp3Viewid;
	
	CAknWaitDialog *iWaitNoteDialog;
	TBuf<512> _internal_buf16;
	TBool _show_DlgLD;

	TBuf<256>	m_taskInfo;
	
	int _rtsp_data_timeout;

	TBuf<32> _username;
	TBuf<32> _password;
	TBuf<32> _userid;

	TBuf<32> _sina_sp_wap_spid;
	TBuf<32> _sina_sp_wap_serviceid;
	TInt 		_sina_sp_wap_price;
	TBuf<256> _sina_sp_wap_spurl;
	TBuf<32> _sina_sp_wap_fromid;
	TInt _sina_sp_ivr_price;
	TBuf<32> _sina_sp_ivr_fromid;
	
	TInt _sina_user_loc_id;
	TInt _sina_download_mode_flag;

	void HideWaitNote () ;
	void FinishWaitNote();
	void HidePopMenu();
	static void ShowMessage (const TDesC& _message);
	void ShowFormatMessageL (const TDesC& aFormat, const TDesC& aText);
	void ShowFormatMessageL (const TDesC& aFormat, TInt a);
	static void ShowConfirmationMessage (const TDesC& _message);
	static int DisplayMessage (const TDesC& _header, TDesC& _message) ;
	void ShowWaitNote (const TDesC& _prompt);
	static void ShowWaitNoteWithCancel (const TDesC& _prompt, MFunWaitDialogObserver* _callback) ;
	void RunDlgLD(int _title_resourceID,int _text_resourceID);
	void ShowDlgLD(int _title_resourceID,int _text_resourceID);
	const TDesC& TextResource (int _resource_id);

	TInt ReSetTaskInfo();
	const TDesC& GetTaskInfo();
//****************for view show---End-----------
//***************for oem and phone data---Begin-----------
	TBuf<32> _imei;
	TBuf<64> _phone_model;
	TBuf<32> _service_center_number;	
public:
	const TDesC& GetPhoneModel();
	void GetDownPicPath(TDes& aPath);
	void SetMainDownloadPath(const TDesC& aPath );
	const TDesC& GetMainDownloadPath();
	const TDesC& GetMainpageAddress();
	const TDesC& GetDownLoadAddress();
	const TDesC& GetWapProxyAddress() ;
	TInt GetWapProxyPort() ;
	const TDesC& GetChatRoomProxyAddress() ;
	TInt GetChatRoomProxyPort();
	const TDesC& GetServiceCenterNumber() ;
	void GetDefaultServiceCenterNumber( TDes& aNumber ) ;
	int LoadConfig (TInt aDefault) ;
	int SaveConfig (TInt aDefault) ;
	int LoadOemConfig () ;
	
//***************for oem and phone data---End-----------
//***************for http data---Begin-----------
public:
	TBool _background_light;
	int _default_client_rtp_port;
	int _sound_volume, _saved_sound_volume;

	TUint32 _http_iap_id;

	int _activate_cover_id;
	int _media_portal_full_screen;
	int _pause_when_background;
	int _manual_upgrade;

	int _show_privacy_statement;
		
protected:
	/* Root paths */
	TInt  		_root_disk;
	TBuf<256> 	_root_path;
	TBuf<256> 	m_mp3SavePath;
	TBuf<256> 	m_readSavePath;
	TBuf<256> 	m_PicSavePath;
protected:
	/* OEM information */
	TBuf<32> 	_provider_name;
	TBuf<128>	 _idledown_url;
	TBuf<32> 	_cmcc_wap_address;
	TInt 			_cmcc_wap_port;
public:
	TBool 		register_success;
	
	TBuf<128> 	_myleft_address;				// sina music address
	TBuf<128> 	_down_address;				// sina music address
	TBool 		_oem_loaded;

	void setIAP(TUint32  aSelectedIap);
	static int AssureIAP (TUint _id) ;
	TInt CheckHttpWapOrNet_Obsolete();
	TInt CheckHttpWapOrNet(TInt aIapId, TDes* aServiceType) ;
	TInt CheckHttpWapOrNet_Obsolete(TInt aIapId);
	TInt CheckHttpWapOrNet(TInt aIapId) ;
	TInt CheckHttpWapOrNet() ;
	TInt GetIapServiceFromAccessPointIdL(TInt aIapId);
	TInt GetIapServiceFromAccessPointIdL(TInt aIapId, TDes& aServiceType);
	TBool FetchProxyAddressPortL(TInt aIapService, const TDesC& aServiceType, 
										   TDes& aProxyAddr, TInt& aProxyPort);
	TBool FetchProxyAddressPortL(TInt aIapService, TDes& aProxyAddr, TInt& aProxyPort);
	static void SaveNodeValueAsDesc(sxml_node* _node, TDes& _string);
	static void SaveNodeValueAsInt(sxml_node* _node, TInt& _value);
	static TInt FindNodeAndSaveToDesc(sxml_node* _root_node, const char* _find, TDes& _string);
	static TInt FindNodeAndSaveToInt(sxml_node* _root_node, const char* _find, TInt& _int);
//***************for http data---Eegin-----------

//***************for play music control data---Begin-----------
public:
	CMdaAudioShell* 	iMediaCore;
	int				m_musicStatus;
	CArrayFixFlat<music_list*> *m_localMusiclist;
	CArrayFixFlat<music_list*> *m_musicMasklist;
	CArrayFixFlat<music_bdclass*> *m_musicClassifies;
	CArrayFixFlat<music_bdclass*> *m_musicChannels;
	CArrayFixFlat<song*> *m_musiclrclist;
	CArrayFixFlat<song*> *m_musiclist;
	CArrayFixFlat<song*> *m_musicSearch;
//for player task
	song	 *		curPlayer;
	TBool 		m_IsLocalPlay;
	TInt 			m_iCurretPlayIndex;
	unsigned long m_duration;;
	CLrcSubtitle * m_subtitle;
//	TInt			m_playMode;   //0 Ë³Ðò£¬1 Ñ­»·¡£2 Ëæ»ú¡£

	void Music_SetVolume(int _szflag);
	int LoadMusicList () ;
	int SaveMusicList();
	int LoadMusicMaskList () ;
	int SaveMusicMaskList () ;
	TInt Music_GetLocalMusicCount();
	void Music_localListAppend(const TDesC& _url,const TDesC& _title );
	void Music_localListDelete(const TInt _index);
	void Music_localListReset();
	int Music_MaskListAppend(const TDesC& _url,const TDesC& _title);
	void Music_MaskListReset();
	void Music_MaskListDelete(const TInt _index);
	void Music_ClassifiesAppend(TInt _typeId,TInt _id,const TDesC& _title );
	void Music_ChannelsAppend(TInt _typeId,TInt _id,const TDesC& _title );
	void Music_bangdListReset();
	void Music_bangdListAppend(TInt _Id,const TDesC& _name,const TDesC& _auth,TInt _picID,TInt _lrcID);
	void Music_LrcListReset();
	void Music_LrcListAppend(TInt _Id,const TDesC& _name,const TDesC& _singer );
	void Music_SearchListReset();
	void Music_SearchListAppend(TInt _Id,const TDesC& _name,const TDesC& _singer,TInt _picID,TInt _lrcID );

	 void Music_playerSetMode(TInt _mode){  m_musicSetting.playMode=_mode;}
	 unsigned long Music_GetCurTime() ;
	 void Music_Seek(DWORD pTime);
	 BOOL Music_SeekAndGetLrc();
	void Music_SetPlayer(song *_curSong,TInt _duration);
	void Music_SetPlayerAndSubtitle(song *_curSong);
	int  Music_SetSubtitle(TDesC & filename);
	void Music_PlayerListNext();
	CLrcSubtitle *Music_GetSubtitle(){return m_subtitle;}
	 int Music_playerCurrentIndex(){return m_iCurretPlayIndex;}
	 void Music_playerSetCurIndex(TInt index);
	 void Music_GetPlayerListSong(song* _CurSong,TInt _index);
	 void Music_playSong(song* _CurSong);
	 void Music_playerPlay();
	 void Music_playerPause() ;
	  void Music_playerStop();
	
	void Music_GetDownMP3Path(TDes& aPath);
	void Music_SetDownMP3Path(TDesC& aPath);
//***************for play music  control data---Eegin-----------
//***************for read  data---Begin-----------
public:
	TBool		m_readStatus;  //0 stop  1 read 
	int			m_listReadPage;  //not used
	TBuf<256> 	m_readUrl;
	TBuf<56> 	m_readTitle;
	TInt 			m_readPos;
	CArrayFixFlat<read_list*> *m_localReadlist;
	CArrayFixFlat<read_masklist*> *m_readMasklist;	
	CArrayFixFlat<read_bdclass*> *m_readClassifies;
	CArrayFixFlat<read_bdclass*> *m_readChannels;
	CArrayFixFlat<book*> *m_readList;
	CArrayFixFlat<book*> *m_readSearch;
public:
	static TInt GetBackPic(TInt index, TDes& _fullName);
	static TInt GetBackPicCount();
	void UpdataLocalReadList();
	void UpdataLocalMusicList();
	void UpdataLocalAllList();
	int LoadReadList () ;
	int SaveReadList();
	int LoadReadMaskList () ;
	int SaveReadMaskList () ;
	TInt GetLocalReadCount();
	void Read_localListAppend(const TDesC& _url,const TDesC& _title );
	void Read_localListDelete(const TInt _index);
	void Read_localListReset();
	void Read_MaskListLoad(const TDesC& _url,const TDesC& _title ,const TInt _pos);
	void Read_MaskListAppend(const TDesC& _url,const TDesC& _title ,const TInt _pos);
	void Read_MaskListReset();
	void Read_MaskListDelete(const TInt _index);
	void Read_ClassifiesAppend(TInt _typeId,TInt _id,const TDesC& _title );
	void Read_ChannelsAppend(TInt _typeId,TInt _id,const TDesC& _title );
	void Read_bangdListReset();
	void Read_bangdListAppend(TInt _Id,const TDesC& _name,const TDesC& _auth,TInt _picID );
	void Read_SearchListReset();
	void Read_SearchListAppend(TInt _Id,const TDesC& _name,const TDesC& _auth,TInt _picId);
	void Read_GetDownTextPath(TDes& aPath);
	void Read_SetDownTextPath(TDesC& aPath);
	void Read_SetRead(const TDesC& _url,const TDesC& _title ,TInt _pos);
	void Read_SavedRead();
	void Read_SetBackColor(TInt _r,TInt _g,TInt _b);
	void Read_SetFontCloor(TInt _r,TInt _g,TInt _b);
//***************for read data---Eegin-----------
//***************for  download data---Begin-----------
public :
	CArrayFixFlat<download*> *m_downloadlist;
	TInt						m_curDownId;
	int LoadDownloadList () ;
	int SaveDownloadList();
	void Download_ListAppend(const TDesC& _filename, TInt _id,TInt _size, TInt _len,TdownloadStatus _status,TInt _typeid);
	void Download_ListLoad(const TDesC& _filename, TInt _id,TInt _size, TInt _len,TdownloadStatus _status,TInt _typeid);

	void Download_ListDelete(const TInt _index);
	void Download_ListGetDownloadItem(int i,TDes & itemtext,TUint& aLen, TUint& aSize,TdownloadStatus& _aStatus);
	void Download_ListSetStatus(const TInt _index, TdownloadStatus _status );
	TInt Download_ListGetType(const TInt _index );
	int 	Download_ListDownloadCheck();
	int 	Download_ListDownloadNext();
	int 	Download_SartDownload(TInt index);


//***************for download data---Eegin-----------


//******************************for  chatroom  Begin******************************
#ifdef  _ADD_CHARROOM_
     CChartRGetter* iChartRData;
	TBuf<32> 	m_chatroom_address;
	TInt 			m_chatroom_port;
	CArrayFixFlat< chatR_body*> *m_CMBody;  
	CArrayFixFlat<chatR_log*> * m_CMCommlog;
	CArrayFixFlat<chatR_log*> *m_CMPrevlog;  
	
	void ChatR_LoginChatRoom();
	void ChatR_ExitChatRoom();
	
	void ChatR_BodyListAppend(const TDesC& _name);
	void ChatR_BodyListDeleteByName(const TDesC& _name);
	void ChatR_BodyListDeleteById(const TInt _index);
	void ChatR_BodyListReset();

	void ChatR_CommLogListAppend(const TDesC& _name,const TDesC& _msg,DWORD _time);
	void ChatR_CommLogListReset();
	
	void ChatR_PrevLogListAppend(const TDesC& _name,const TDesC& _msg,DWORD _time);
	//void ChatR_BodyLogListDelete(const TInt _index);
	void ChatR_PrevLogListReset();

#endif
//******************************for  chatroom  End******************************

//***************for  control data---Begin-----------
 void  GetDirInfo(RFs& iSessionRFs,const TDes & aFileName,char * _ext);
  void GetDirInfo(RFs& iSessionRFs,const TDes & aFileName);
//***************for control data---Eegin-----------
public :
	static void GetFullPathName(TDes& aFileName);
	static void UrlParamEncode( const TDesC& strIn, TDes& strEncode, TBool bAll );
	static void DumpException(const char* _str, unsigned char _append);
	static int	IsFileExist(const TDesC& _filename);
	static TInt DeleteFile (const TDesC& _filename) {
		return EikFileUtils::DeleteFile (_filename);
	}
	static TInt CopyFile (const TDesC& aSrcName,const TDesC& aDstName )
	{
		return EikFileUtils::CopyFile( aSrcName, aDstName );
	}
	static TInt RenameFile (const TDesC& anOldName,const TDesC& aNewName)
	{
		RFs fs;
		User::LeaveIfError( fs.Connect() );

		TInt err = fs.Rename( anOldName, aNewName );

		fs.Close();

		return err;	
	}

	static int get_attribute(char* xml, char* attribute, TDes& strBuffer)
	{

		// Get to the start of the attribute
		char* as = strstr(xml, attribute);
		char *buffer=(char*)FC_malloc(256);
		memset(buffer,0,256);
		if (!as) 
		{
			strBuffer.SetLength(0);
			return (0);
		}

		// Read the attribute
		char* start = NULL;
		int numchars = 0;
		int i = 0;
		for (i = 0; i < strlen(as); i++) {

		if (start) numchars++;

		if (*(as + i) == '\'' || *(as + i) == '"') {
		if (!start)
		start = (as + i + 1);
		else
		break;
		}
		}

		// Store the result in the buffer
		strncpy(buffer, start, numchars-1);
	//	*(buffer + numchars - 1) = '\0';
		FC_CharpToTBuf16(strBuffer, buffer);
		FC_free(buffer);
		return (1);
	}

	static int get_attribute(char* xml, char* attribute, TInt *number)
	{

		// Get to the start of the attribute
		char* as = strstr(xml, attribute);
		char *buffer=(char*)FC_malloc(256);
		memset(buffer,0,256);
		if (!as) 
		{
			number=0;
			return (0);
		}

		// Read the attribute
		char* start = NULL;
		int numchars = 0;
		int i = 0;
		for (i = 0; i < strlen(as); i++) {

		if (start) numchars++;

		if (*(as + i) == '\'' || *(as + i) == '"') {
		if (!start)
		start = (as + i + 1);
		else
		break;
		}
		}

		// Store the result in the buffer		strncpy(buffer, start, numchars-1);
	//	*(buffer + numchars - 1) = '\0';
		*number=atoi(buffer);
		return (1);
	}
static int get_body (char* xml, char* _szbody, TDes& strBuffer) 
{

	char* start = strstr(xml, _szbody);
	if(!start)
	{
		strBuffer.SetLength(0);
		return (0);
	}
	
	start+=(strlen(_szbody)+2);//body><
	char*end= strstr(start, "></");  //></body>
	if(!end)
	{
		strBuffer.SetLength(0);
		return (0);
	}

	int len=end-start;
	if(!len)
	{
		strBuffer.SetLength(0);
		return (0);
	}
	
		
	char *buffer=(char*)FC_malloc(len+1);
	memset(buffer,0,len+1);
	strncpy(buffer,start,len);

	if(strstr(buffer,"CDATA"))
	{
		end=buffer+strlen(buffer)-2;// "]]"
		*end='\0';
	}
	
	FC_CharpToTBuf16(strBuffer, buffer+8);
	FC_free(buffer);	
	
	return 1;
}

static int get_body2 (char* xml, char* path, TDes& strBuffer) 
{

	const char* _xml_string = (const char*)xml;
	sxml _sxml;
	TRAPD ( r, _sxml.load_string (_xml_string) );
	if( r!=KErrNone )
	{
		strBuffer.SetLength(0);
		return 0;
	}
	sxml_node *_node;

	TPtrC8 _str8;
	ansistring _str = ansistring("");

	/* load provider_name */
	_node = _sxml.find_path (path);
	if (_node) {
		_str = _node->_value.as_string();
		_str8.Set((unsigned char*)_str.as_char(), _str.length());
		FC_Utf8ToUnicodeL (strBuffer, _str8);
	}
	else
	{
		strBuffer.SetLength(0);
		return 0;
	}

	return 1;
}

static int get_tag(char* xml, char* tag, TDes& strBuffer) 
{
    char tagopen[255];
    char tagclose[255];
    char inone[255];
    sprintf(tagopen,  "<%s", tag);
    sprintf(tagclose,"</%s>", tag);
    sprintf(inone,"<%s/>", tag);
    // If the tag is present, but has no content, stop now
    if (strstr(xml, inone))
	{
	strBuffer.SetLength(0);
	return 1;
    	}
    // Get to the start of the tag
    char* as = strstr(xml, tagopen);
    if (!as)
{
	strBuffer.SetLength(0);
	return 0;
   }
    char* ap = strstr(as, ">") + sizeof(char);
    if (!as) {
	strBuffer.SetLength(0);
	return 0;
   }

    // Find the end
    int i = 0;
    while ( *(ap + (sizeof(char) * i)) != '<' ) i++;
    // Store the result in the buffer
    	char *buffer=(char*)FC_malloc(i+1);
	memset(buffer,0,i+1);

    strncpy(buffer, ap, i);
    *(buffer + i) = '\0';

	FC_CharpToTBuf16(strBuffer, buffer);
	FC_free(buffer);

    return 1;
} 

};

#ifdef _S60_3RD_
extern	CenterCore	*centerExtern;
#endif

struct TListboxContext
{
	TInt iSelIndex;
	TInt iTopIndex;
};


#include <f32file.h>

class MFileSearchEngineNotifier
{
public:
	virtual void SearchComplete() = 0;
	virtual TBool SearchCondition(const TDesC& aFileName) = 0;
};

class CFileSearchEngine : public CActive
{
public:
	CFileSearchEngine( MFileSearchEngineNotifier* aNotifier,TInt atype);
	~CFileSearchEngine();

	enum TSearchType
	{
		ESearchRead=0,	//default
		ESearchMp3,
		ESearchAll
	};
protected:
	// from CActive
	/*!
	@function DoCancel

	@discussion cancel any outstanding operation
	*/
	void DoCancel();

	/*!
	@function RunL

	@discussion called when operation complete
	*/
	void RunL();

	MFileSearchEngineNotifier* iNotifier;
	CenterCore* iFondoGlobal;
	TBool iCancelSearch;
	RFs iFs;
	
	TSearchType iType;
	TBuf<KMaxFileName> iDir;
	TInt iCount;

	RThread iThread;
	TBool iThreadRunning;
	static TInt FThreadFunction(TAny* aData);
	void WhenExitThreadFunction();
	void DoThreadSearch();
	//
	//New functions
	//TInt SearchingInDirectory( CPlayList* aPlayList, const TDesC& aDir);
public:	
	void ConstructL();
	void StartSearchInThread();
	void CancelSearch();

};

#endif
