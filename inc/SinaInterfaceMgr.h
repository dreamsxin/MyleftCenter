#ifndef _SINA_INTERFACE_MGR_H_
#define _SINA_INTERFACE_MGR_H_


#include "MusicContentGetter.h"
#include <msvapi.h> // MMsvSessionObserver, CMsvSession, CMsvEntry
#include <mmsclient.h> // CMmsClientMtm
#include <mtclreg.h> // CClientMtmRegistry
#include <mtmdef.h> // KMsvMessagePartBody
#include <smsclnt.h> // CSmsClientMtm
#include <smscmds.h> // ESmsMtmCommandScheduleCopy
#include <smuthdr.h> // CSmsHeader
#include <smutset.h> // CSmsSettings
#include "MusicImagesFetchTask.h"
#include "TimeoutTimer.h"
#include "symbian_def.h"
#include "sxml.h"
#include "FunUIStart.h"



struct TSinaAdInfo
{
	TBuf<128> iDescription;
	TBuf<128> iLink;
	TBuf<128> iImage;
};

class CSinaInterfaceMgr:	public CBase
							,public MMusicContentGetterNotifier
							,public MMsvSessionObserver
							,public MMusicImagesFetchTaskObserver
							,public MTimeOutNotify
							,public MFunWaitDialogObserver
{
public:
		static CSinaInterfaceMgr* NewL(CenterCore* owner);
		~CSinaInterfaceMgr();
		void ConstructL (CenterCore * owner) ;
//		void SetSearchContainer (CFunSMusicContainerSearch* _aContainer){iSearchContainer=(CFunSMusicContainerSearch*)_aContainer;}

 protected: // Data
		CenterCore* iCenterCore;
//		CFunSMusicContainerSearch* iSearchContainer;
	
	public:
	//	CPlaybackObjects* iPlaybackObjects;
		CMusicContentGetter* iDownLoadGetter;
		CMusicContentGetter* iMusicGetter;
		CMusicImagesFetchTask* iImageFetcher;
		TInt iDownTask;		/*1001 =down text

							*/
		TInt iTaskType;		/* 
							0000
							0001	get bangdan's class
							0002	get read list
							0021	get music list
							
							0003	sreach read new
							0004	sreach read append
							
							0005	sreach music new
							0006	sreach music append				

							0008	sreach lrc

							5555      test url
							*/
								
		
		TBool iFullLyric;
		
		//
		/*
			From MMusicContentGetterNotifier
		*/
		TInt McgnOnComplete(TInt aErr);
		void McgnAfterUserSelectIAP(TInt aErr, TInt aIap) ;
		TBool IsFetchingMusicInfo();
		TInt IsFetchingType();
		TInt IsDownLoading();
		void CancelFetchMusicInfo();
		void CancelFetchDownload();
		void OnDismissDialogL( TInt aButtonId, TBool& aCanExit );
		void GetLengthAndSize( TUint& aLen, TUint& aSize );
		//
		// From MMusicImagesFetchTaskObserver
		TInt MmiftoAfterFetchImage(const TDesC& aId, void* aData, const TDesC& aFileName);
		//
		// from MMsvSessionObserver
		virtual void HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3);
		TBool iObservingSms;
		TBuf<32> iObservingNumber;
		TBuf<128> iObservingContent;
		CMsvSession* iMsvSession;
		CMsvEntry* iMsvEntry;
		TMsvId iNewMessageId;
		CClientMtmRegistry* iClientMtmReg;
		CSmsClientMtm* iSmsMtm;

		TBuf<32> iFetchingAlbumId;
		//TBuf<128> iFetchingAlbumName;
		TBuf<1024> iPostBuffer;
		TBuf<256> iSavedRbtAddress;
		TBuf<32> iRegisterPassword;
		//CSpItem *iCurrentRbtItem;
		TBuf<32> iBuySongId;
		TBuf<32> iBuySongName;
		TBuf<32> iBuyArtistName;
		TBuf<32> iBuyPrice;

		TBuf<128> iLoadSongPath;
		TBuf<256>  idownloadDes;
		TInt LaunchGetTestUrl( const TDesC& aUrl);
	//	TInt LaunchGetFile(const TDesC& aName,const TDesC& aUrl, TInt _typeId);
		TInt LaunchGetReadFile(TInt _Id,const TDesC& aName,TInt _typeId);
		TInt LaunchGetList( TInt _id,TInt _typeid,TInt _class);
		TInt ParseGetList(TDes8& aText);
		TInt LaunchGetBangdanClass(TInt _classes,TInt _typeID);
		TInt ParseReadBangdanClass(TDes8& aText);

		TInt LaunchSearchRead(const TDesC& _szKey);
		TInt LaunchSearchReadAppend(const TDesC& _szKey,TInt _szPage);
		TInt LaunchSearchMusic(const TDesC& _szKey);
		TInt LaunchSearchMusicAppend(const TDesC& _szKey,TInt _szPage);

		TInt LaunchSearch( TInt _szclass,const TDesC& _szKey,TInt _szPage,TInt _iTaskType);
		TInt ParseReadSearch(TDes8& aText);

		TInt LaunchSearchLrcUrl(const TDesC& aSigner, const TDesC& aName,const TDesC& album,const TDesC& aDesPath );
		TInt ParseSearchLrc(TDes8& aText);



/*		
		TInt LaunchGetPayfsx(TBool isSingle);
		TInt LaunchGetBaseInfo();
		TInt LaunchGetBaseInfo2();
		TInt LaunchGetSongList(const TDesC& aId, const TDesC& aName );
		TInt LaunchPlayingLrcUrlManual(const TDesC& aSigner, const TDesC& aName,const TDesC& album,const TDesC& aDesPath );
		TInt LaunchPlayingLrcUrl(const TDesC& aSigner, const TDesC& aName,const TDesC& album,const TDesC& aDesPath );
		TInt LaunchGetPlayerlrc( const TDesC& alrcUrl,const TDesC& aName );	
		TInt LaunchSearch(const TDesC& aKeyword, const TDesC& aMethod, TInt aTaskType, TInt aStartIndex, TInt aPageLength);
		TInt LaunchSearchSong(const TDesC& aKeyword, TInt aStartPage,TInt aPageLength);
		TInt LaunchSearchSongAppend(const TDesC& aKeyword, TInt aStartPage,TInt aPageLength);

*/
		void StartObserveSms(const TDesC& aObservNumber);
		void StopObserveSms();
		void SetDesPath(const TDesC& afilepath){iLoadSongPath.Copy(afilepath);}

		TInt FindNodeAndSaveToDesc(sxml_node* _root_node, const char* _find, TDes& _string);
		TInt FindNodeAndSaveToInt(sxml_node* _root_node, const char* _find, TInt& _int);
/*
		TInt ParseUserInfoAfterLogon(TDes8& aText);
		TInt ParseUserInfoAfterLogon2(TDes8& aText);
		void ParseUserAlbumInfo(sxml_node* aAlbumNode, TBool aSave=EFalse);
	//	TInt ParseHotlist(TDes8& aText);
		TInt ParseTop10list(TDes8& aText);
		TInt ParseSearchSong(TDes8& aText);
		TInt ParsePlayingInfo(TDes8& aText);
		TInt ParseManualPlayingInfo(TDes8& aText);
*/		
		CTimeOutTimer* iTaskTimer;	// task timer
		TInt iTimerTaskType;		// 1=fav search song, 2=fav search album, 
									// 3=same singer's albums
									// 4=wait for register
		void StartTimer(TInt aMSecond);
		void StopTimer();
		void ReleaseTimer();
		void TimerExpired ();
		TBuf<64> iFavTaskDoingInfo;

		TSinaAdInfo iAdInfo;
		void CheckFetchAdImageL();
		
};









#endif
