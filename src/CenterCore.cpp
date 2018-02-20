/*
============================================================================
 Name        : Fplayercoer 
 Author      : yilizheng
 Version     :
 Copyright   : Your copyright notice
 Description : for connect FunCore and symbian UI
============================================================================
*/
#include "portab_symbian.h"
#include "CenterCore.h"
#include <MyleftCenter.rsg>

#include <string.h>
#include <StringLoader.h> 
#include <aknmessagequerydialog.h> //CAknMessageQueryDialog
#include <AknGlobalNote.h>   //CAknGlobalNote

#include <msvapi.h> // MMsvSessionObserver, CMsvSession, CMsvEntry
#include <mmsclient.h> // CMmsClientMtm
#include <mtclreg.h> // CClientMtmRegistry
#include <mtmdef.h> // KMsvMessagePartBody
#include <smsclnt.h> // CSmsClientMtm
#include <smscmds.h> // ESmsMtmCommandScheduleCopy
#include <smuthdr.h> // CSmsHeader
#include <smutset.h> // CSmsSettings
#include <commdb.h>
#include <e32std.h>
#include <e32def.h>
#include <utf.h>
#include <charconv.h>
#include <eikapp.h>
#include <eikenv.h>
#include <eikappui.h>
#include <BAUTILS.H>
#include <ImageConversion.h>//for CImageDecoder
//end fc headers
#include "start.h"

#include "MyleftCenterAppUi.h"
#include "sxml.h"
#include "const.h"
#include "SinaInterfaceMgr.h"
#include "MdaAudioShell.h"
#include "chatroomtask.h"

#include  "MyleftCenterView.h"
#include  "MyleftCenterContainer.h" 
#include  "MyleftCenterViewMusic.h"
#include "MyleftCenterContainerMusic.h"
#include  "MyleftCenterViewChatRoom.h"
#include  "MyleftCenterContainerChatRoom.h"

#ifndef AUDIO_FORMAT_SIGNED_16
#define AUDIO_FORMAT_SIGNED_16    0x110
#endif

#ifdef _S60_3RD_
CenterCore	* centerExtern;
#endif

CenterCore* CenterCore::NewL(const TDesC& _rootpath)
{
    CenterCore* self = new (ELeave) CenterCore( );
    CleanupStack::PushL( self );
    self->InitData(_rootpath);
    CleanupStack::Pop();


    return self;
}


CenterCore::CenterCore()
{
	m_centerSetting.http_iap_id=0;
	m_centerSetting._password.SetLength(0);
	m_centerSetting._userid.SetLength(0);
	m_centerSetting._username.SetLength(0);
	m_centerSetting._nickname.SetLength(0);

	m_musicSetting.backrightOn=EFalse;
	m_musicSetting.playMode=0;
	m_musicSetting.volume=5;

	m_readerSetting.autoRool=EFalse;
	m_readerSetting.backPicId=1;
	m_readerSetting.bBcakPic=1;
	m_readerSetting.autoTime=20;
	m_readerSetting.backColor.R=1;
	m_readerSetting.backColor.G=1;
	m_readerSetting.backColor.B=1;
	m_readerSetting.textColor.R=153;
	m_readerSetting.textColor.G=153;
	m_readerSetting.textColor.B=105;
	m_readerSetting.textSize=160;  //60-100  step=5
		
}

CenterCore::~CenterCore()
{
	if(iMediaCore)
	{
		iMediaCore->StopL();	
		delete iMediaCore;
	}
	iMediaCore=NULL;
	
	ChatR_ExitChatRoom();
	
	if(curPlayer)
		delete curPlayer;
	curPlayer=NULL;

	if(iSinaMgr)
		delete iSinaMgr;

	if(iDownMgr)
		delete iDownMgr;

	if(iChartRData)
		delete iChartRData;

	SaveDownloadList();
	SaveConfig(1);
	SaveMusicList();
	SaveMusicMaskList();
	SaveReadList();
	SaveReadMaskList();
	
	for(TInt i=0; i<m_localReadlist->Count(); i++ )
	{
		read_list* list =  m_localReadlist->At(i);
		if(list)
			delete list;
	}
	delete m_localReadlist;
	m_localReadlist=NULL;

	for(TInt i=0; i<m_readMasklist->Count(); i++ )
	{
		read_masklist* list =  m_readMasklist->At(i);
		if(list)
			delete list;
	}
	delete m_readMasklist;
	m_readMasklist=NULL;

	for(TInt i=0; i<m_readChannels->Count(); i++ )
	{
		read_bdclass* list =  m_readChannels->At(i);
		if(list)
			delete list;
	}
	delete m_readChannels;
	m_readChannels=NULL;

	for(TInt i=0; i<m_readClassifies->Count(); i++ )
	{
		read_bdclass* list =  m_readClassifies->At(i);
		if(list)
			delete list;
	}
	delete m_readClassifies;
	m_readClassifies=NULL;

	for(TInt i=0; i<m_readList->Count(); i++ )
	{
		book* list =  m_readList->At(i);
		if(list)
			delete list;
	}
	delete m_readList;
	m_readList=NULL;

	for(TInt i=0; i<m_downloadlist->Count(); i++ )
	{
		download* list =  m_downloadlist->At(i);
		if(list)
			delete list;
	}
	delete m_downloadlist;
	m_downloadlist=NULL;
	

	for(TInt i=0; i<m_readSearch->Count(); i++ )
	{
		book* list =  m_readSearch->At(i);
		if(list)
			delete list;
	}
	delete m_readSearch;
	m_readSearch=NULL;

	for(TInt i=0; i<m_localMusiclist->Count(); i++ )
	{
		music_list* list =  m_localMusiclist->At(i);
		if(list)
			delete list;
	}
	delete m_localMusiclist;
	m_localMusiclist=NULL;

	for(TInt i=0; i<m_musicMasklist->Count(); i++ )
	{
		music_list* list =  m_musicMasklist->At(i);
		if(list)
			delete list;
	}
	delete m_musicMasklist;
	m_musicMasklist=NULL;

	for(TInt i=0; i<m_musicChannels->Count(); i++ )
	{
		music_bdclass* list =  m_musicChannels->At(i);
		if(list)
			delete list;
	}
	delete m_musicChannels;
	m_musicChannels=NULL;

	for(TInt i=0; i<m_musicClassifies->Count(); i++ )
	{
		music_bdclass* list =  m_musicClassifies->At(i);
		if(list)
			delete list;
	}
	delete m_musicClassifies;
	m_musicClassifies=NULL;

	for(TInt i=0; i<m_musiclist->Count(); i++ )
	{
		song* list =  m_musiclist->At(i);
		if(list)
			delete list;
	}
	delete m_musiclist;
	m_musiclist=NULL;

	for(TInt i=0; i<m_musiclrclist->Count(); i++ )
	{
		song* list =  m_musiclrclist->At(i);
		if(list)
			delete list;
	}
	delete m_musiclrclist;
	m_musiclrclist=NULL;

	for(TInt i=0; i<m_musicSearch->Count(); i++ )
	{
		song* list =  m_musicSearch->At(i);
		if(list)
			delete list;
	}
	delete m_musicSearch;
	m_musicSearch=NULL;
	
	FC_Log("~CenterCore");
	
}

CenterCore* CenterCore::GetInstance()
{
#ifndef _S60_3RD_
	return (CenterCore*)Dll::Tls();
#else
	return centerExtern;
#endif
}



void CenterCore::InitData(const TDesC& _rootpath)
{
	_root_path=_rootpath;
	_rtsp_data_timeout = 3000000;

	_http_iap_id = 0;
	m_curDownId=-1;
#ifdef __WINS__	
	_cmcc_wap_address.Copy( _L("222.186.30.5") );
	_cmcc_wap_port = 4222;
#else
	_cmcc_wap_address.Copy( _L("10.0.0.172") );
	_cmcc_wap_port = 80;
#endif

	m_localReadlist= new (ELeave) CArrayFixFlat<read_list*> (4);
	m_readMasklist= new (ELeave) CArrayFixFlat<read_masklist*> (4);
	m_readChannels= new (ELeave) CArrayFixFlat<read_bdclass*> (4);
	m_readClassifies= new (ELeave) CArrayFixFlat<read_bdclass*> (4);
	m_readList= new (ELeave) CArrayFixFlat<book*> (4);
	m_readSearch= new (ELeave) CArrayFixFlat<book*> (4);
	m_downloadlist=new (ELeave) CArrayFixFlat<download*> (4);

	m_localMusiclist= new (ELeave) CArrayFixFlat<music_list*> (4);
	m_musicMasklist= new (ELeave) CArrayFixFlat<music_list*> (4);
	m_musicChannels= new (ELeave) CArrayFixFlat<music_bdclass*> (4);
	m_musicClassifies= new (ELeave) CArrayFixFlat<music_bdclass*> (4);
	m_musiclist= new (ELeave) CArrayFixFlat<song*> (4);
	m_musiclrclist= new (ELeave) CArrayFixFlat<song*> (4);
	m_musicSearch= new (ELeave) CArrayFixFlat<song*> (4);

	LoadOemConfig();
	LoadConfig(1);
	LoadReadList();
	LoadReadMaskList();
	LoadDownloadList();

	LoadMusicList();
	LoadMusicMaskList();

	
	if(!iSinaMgr)
		iSinaMgr=CSinaInterfaceMgr::NewL(this);

	if(!iDownMgr)
		iDownMgr=CSinaInterfaceMgr::NewL(this);

	if(FC_IsFileExist(KFirstFlag)) //only after setup;
	{
		_http_iap_id =FC_findIapID(0);
		FC_DeleteFile(KFirstFlag);
	}

	curPlayer=new song();
	curPlayer->iaskurl.SetLength(0);
	curPlayer->name.SetLength(0);
	curPlayer->singer.SetLength(0);

	if(!iMediaCore)
		iMediaCore=CMdaAudioShell::NewL(this);


	SaveConfig(1);
	FC_Logsp("CenterCore::InitData()  _http_iap_id=%d ",_http_iap_id);
}




void CenterCore::setIAP(TUint32  aSelectedIap)
{
	_http_iap_id=aSelectedIap;
}

//----------for --self---------------------------

void CenterCore::HideWaitNote () 
{
	if(_show_DlgLD)
	{
		TKeyEvent _key;
		_key.iCode = EKeyEscape;
		_key.iModifiers = 0;
		CCoeEnv::Static()->SimulateKeyEventL (_key, EEventKey);	 
		_show_DlgLD=EFalse;
	}
	FinishWaitNote();
}

void CenterCore::FinishWaitNote()
{
	if ( iWaitNoteDialog != NULL )
	{
		iWaitNoteDialog->SetCallback( NULL );
		iWaitNoteDialog->ProcessFinishedL();    // deletes the dialog
		iWaitNoteDialog = NULL;
	} 

}

void CenterCore::HidePopMenu () 
{
	TKeyEvent _key;
	_key.iCode = EKeyEscape;
	_key.iModifiers = 0;
	CCoeEnv::Static()->SimulateKeyEventL (_key, EEventKey);	 
}

int CenterCore::IsFileExist (const TDesC& _filename) 
{
	RFs _fs;
	RFile _file;
	_fs.Connect ();
	if (KErrNone == _file.Open (_fs, _filename, EFileRead | EFileShareAny)) 
	{
		_file.Close ();
		_fs.Close ();
		return 1;
	} else
	{
		_fs.Close ();
		return 0;
	}
}
void CenterCore::ShowMessage (const TDesC& _message) 
{
	CAknGlobalNote *_global_note = CAknGlobalNote::NewLC ();
	_global_note->ShowNoteL (EAknGlobalInformationNote, _message);
	CleanupStack::PopAndDestroy ();
}

void CenterCore::ShowFormatMessageL (const TDesC& aFormat, const TDesC& aText)
{
	TBufC<128> theFormat(aFormat);
	_internal_buf16.Format(theFormat, &aText);
	ShowMessage(_internal_buf16);
}

void CenterCore::ShowFormatMessageL (const TDesC& aFormat, TInt a)
{
	TBufC<128> theFormat(aFormat);
	_internal_buf16.Format(theFormat, a);
	ShowMessage(_internal_buf16);
}

void CenterCore::ShowConfirmationMessage (const TDesC& _message) 
{
	CAknGlobalNote *_global_note = CAknGlobalNote::NewLC ();
	_global_note->ShowNoteL (EAknGlobalConfirmationNote, _message);
	CleanupStack::PopAndDestroy ();
}

int CenterCore::DisplayMessage (const TDesC& _header, TDesC& _message) 
{
	CAknMessageQueryDialog* _msg_query_dialog = 
		CAknMessageQueryDialog::NewL (_message);
	_msg_query_dialog->SetHeaderTextL (_header);
	return _msg_query_dialog->ExecuteLD (R_AVKON_MESSAGE_QUERY_DIALOG);
}

void CenterCore::ShowWaitNote (const TDesC& _prompt)
{
	iWaitNoteDialog = 
		new (ELeave) CAknWaitDialog (NULL, ETrue);
	iWaitNoteDialog->SetTextL (_prompt);
	iWaitNoteDialog->ExecuteLD (R_WAIT_NOTE_SOFTKEY_EMPTY);
}

void CenterCore::ShowWaitNoteWithCancel (const TDesC& _prompt, MFunWaitDialogObserver* _callback) 
{
	CFunWaitDialog::ShowDlgWithCancelLD( KNullDesC, _prompt, _callback, 
							CenterCore::GetInstance()->m_appUI ->UiStyler(), (TAny*)1 );

}

void CenterCore::RunDlgLD(int _title_resourceID,int _text_resourceID)
{
	_show_DlgLD=ETrue;
	TBuf<64> title;
	TBuf<128> content;
	CEikonEnv::Static()->ReadResource (title, _title_resourceID);
	CEikonEnv::Static()->ReadResource (content, _text_resourceID);
	CFunWaitDialog::RunDlgLD(title,content,m_appUI->UiStyler(), 0 );

}

void CenterCore::ShowDlgLD(int _title_resourceID,int _text_resourceID)
{
	_show_DlgLD=ETrue;
	TBuf<64> title;
	TBuf<128> content;
	if(_title_resourceID)
		CEikonEnv::Static()->ReadResource (title, _title_resourceID); 
	if(_text_resourceID)
		CEikonEnv::Static()->ReadResource (content, _text_resourceID);
	CFunWaitDialog::ShowDlgLD(title,content,m_appUI->UiStyler(), 0 );

}
const TDesC& CenterCore::TextResource (int _resource_id)
{
	CEikonEnv::Static()->ReadResource (_internal_buf16, _resource_id);
	return _internal_buf16;
}


TInt CenterCore::ReSetTaskInfo()
{
	m_taskInfo.SetLength(0);
	FC_Logsp("ReSetTaskInfo,%d,%d,%d",m_curDownId,m_musicStatus,(int)m_readStatus);
	if(m_curDownId!=-1)
	{
#if 0
		_LIT (KString2," %S:%S [%d(kb)/%d(kb)]");
		HBufC* Status=NULL;
		TBuf<100> item;
		TUint size,length;
		TdownloadStatus status;
		Download_ListGetDownloadItem(m_curDownId,item,length,size,status);

		if(status==EdownloadIng)
			Status=StringLoader::LoadLC(R_TEXT_DOWNLOAD_RUN);
		else if(status==EdownloadOk)
			Status=StringLoader::LoadLC(R_TEXT_DOWNLOAD_COMPLETE);
		else if(status==EdownloadErr)
			Status=StringLoader::LoadLC(R_TEXT_DOWNLOAD_ERROR);
		else if(status==EdownloadReady)
			Status=StringLoader::LoadLC(R_TEXT_DOWNLOAD_PAUSE);
		
		m_taskInfo.Format(KString2(),Status,&item,length/1024,size/1024);
		CleanupStack::PopAndDestroy(Status);
#else
		TBuf<100> item;
		TUint size,length;
		TdownloadStatus status;
		Download_ListGetDownloadItem(m_curDownId,item,length,size,status);
		
		m_taskInfo.Append(TextResource(R_TEXT_VIEW_NOW_DOWNLOAD));
		m_taskInfo.Append(_L(": "));
		m_taskInfo.Append(item);
#endif
	}

	if(m_musicStatus)
	{
		m_taskInfo.Append(_L("    "));
		m_taskInfo.Append(TextResource(R_TEXT_VIEW_NOW_LISITEN));
		m_taskInfo.Append(_L(": "));
	//	m_taskInfo.Append(_L("  Current music: "));
		m_taskInfo.Append(curPlayer->name);
	}

	if(m_readStatus)
	{
		m_taskInfo.Append(_L("    "));
		m_taskInfo.Append(TextResource(R_TEXT_VIEW_NOW_READ));
		m_taskInfo.Append(_L(": "));
		m_taskInfo.Append(m_readTitle);
	}

	if(!m_taskInfo.Length())
		return 0;

	if(m_appUI->view1->iContainer)
		m_appUI->view1->iContainer->SetupScrollTask(m_taskInfo);
		
	return 1;
}

const TDesC& CenterCore::GetTaskInfo()
{
	return m_taskInfo;
}

const TDesC& CenterCore::GetPhoneModel()
{
	if( _phone_model.Length()>0 )
		return _phone_model;

#ifdef _NOKIA_90_
	_LIT( KModelFileName, "z:\\resource\\versions\\model.txt" );
	RFs fs;
	RFile file;
	TInt err;
	TBuf8<64> fileText8;
	TBuf<64> fileText;
	_phone_model.Copy( _L("Nokia 3250") );		// defaultly is 3250

	User::LeaveIfError( fs.Connect() );
	err = file.Open( fs, KModelFileName, EFileRead );	
	if( err==KErrNone )
	{
		err = file.Read( fileText8, 64 );
		if( err == KErrNone )
		{
			// copy text to buffer 16
			unsigned short ch;
			int i, j, p, q;
			for( i=0; i<fileText8.Length(); i+=2 )
			{
				ch = fileText8[i];
				ch |= fileText8[i+1]<<8;
				fileText.Append( ch );
			}

			// delete first tag char
			fileText.Delete(0, 1);


			// delete all chars between ( and )
			p=-1; q=-1;
			for( i=0; i<fileText.Length(); i++ )
			{
				if( fileText[i]=='(' )
					p=i;
				else
				if( fileText[i]==')' )
				{
					q=i;
					if( p!=-1 && q>p )
					{
						fileText.Delete( p, q-p+1 );
						i=p;
						p=-1;
						q=-1;
					}
				}
			}

			// delete head and tail white space
			i=0;
			while(fileText[i]==' ' && i<fileText.Length())
				i++;
			if( i>0 )
				fileText.Delete( 0, i );

			i=fileText.Length()-1;
			while(fileText[i]==' ' && i>=0)
				i--;
			if( i<fileText.Length()-1 )
				fileText.Delete( i+1, fileText.Length()-i-1 );

			_phone_model.Copy(fileText);
			//
			//CFondoGlobal::ShowMessage( _phone_model );

		}

		file.Close();
	}
	fs.Close();

	return _phone_model;
#else
	TInt mUid = 0;
	HAL::Get(HALData::EMachineUid, mUid);
	_phone_model.Copy( _L("Nokia 6600") );

	
	const TInt idArray[] = {
		0x10200f97,	0x200005F8, 0x101f466a, 0x101f466a, 0x101fb3f4,
		0x101fb3dd,	0x101f3ee3, 0x101fbb55, 0x101fb3f3, 0x10200F99,
		0x10200F9C, 0x10200F9B, 0x101fb3f3, 0x101f4fc3, 0x101f8c19,
		0x101FB2B1,	0x20001856, 0x20001858, 0x20001857, 0x10200F9A,
		0x200005F9,	0x10200F98, 0x200005FC, 0x200005FA, 0x101FA031,
		0x101F9071, 0x10272F38, 0x10272F39, 0x101FF525
	};
	const char modelArray[][32] = {
		"Nokia 3230", "Nokia 3250", "Nokia 3650", "Nokia 3660", "Nokia 6260",     	
		"Nokia 6600", "Nokia 6620", "Nokia 6630", "Nokia 6670", "Nokia 6680",     	
		"Nokia 6681", "Nokia 6682", "Nokia 7610", "Nokia 7650", "Nokia N-Gage",    
		"Nokia N-Gage QD", "Nokia E60", "Nokia E61", "Nokia E70", "Nokia N70",     	
		"Nokia N80", "Nokia N90", "Nokia N91", "Nokia N92", "Sendo-X",     		
		"Siemens SX1", "Samsung SGH D720", "Samsung SGH D730", "Panasonic X700"  
	};
	
	int i, n=sizeof(idArray)/sizeof(TInt);
	for( i=0; i<n; i++ )
	{
		if( mUid==idArray[i] )
		{
			FC_Utf8ToUnicodeL( _phone_model, TPtrC8((const TUint8*)modelArray[i]) );
			break;
		}
	}

	//CFondoGlobal::ShowMessage( _phone_model );
	return _phone_model;
#endif
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

int CenterCore::LoadConfig (TInt aDefault) {
	RFs _fs;
	RFile _file;
	TInt _ret;

	_fs.Connect ();
	if(aDefault)
		_ret = _file.Open (_fs, DEFAULT_CONFIG_FILENAME, EFileRead);
	else
		_ret = _file.Open (_fs, CONFIG_FILENAME, EFileRead);
	if (KErrNone != _ret) {
		_fs.Close ();
		FC_Logsp("CenterCore::LoadConfig() _file.Open   failed ");
		return 0;
	}

	HBufC8 *_xml_hbuf = HBufC8::NewL (10*1024);
	TPtr8 _xml_tptr8 = _xml_hbuf->Des ();
	if (KErrNone != _file.Read (_xml_tptr8)) {
		delete _xml_hbuf;
		_file.Close ();
		_fs.Close ();
		FC_Logsp("CenterCore::LoadConfig() _file.Read   failed ");
		return 0;
	}

	const char* _xml_string = (const char*)_xml_tptr8.PtrZ ();
	sxml _sxml;

	if(!strlen(_xml_string))
	{
		delete _xml_hbuf;
		_file.Close ();
		_fs.Close ();
		FC_Log("CenterCore::LoadConfig() _xml_string is null ");
		return 0;
	}

	TRAPD ( r, _sxml.load_string (_xml_string) );
	if( r!=KErrNone )
	{
		delete _xml_hbuf;
		_file.Close ();
		_fs.Close ();

		// delete this file
		CenterCore::DeleteFile( CONFIG_FILENAME );
		FC_Logsp("CenterCore::LoadConfig() _sxml.load_string  failed ");
		return 0;
	}
	sxml_node *_node;

	/* load need_light_on */
	_node = _sxml.find_path ("/myleft/background_light");
	if (_node) {
		_background_light = _node->_value.as_string().as_int();
	}

	/* load pause_when_background */
	_node = _sxml.find_path ("/myleft/pause_when_bg");
	if (_node) {
		_pause_when_background = _node->_value.as_string().as_int();
	}
	/* load playmode */
	_node = _sxml.find_path ("/myleft/play_mode");
	if (_node) {
	m_musicSetting.playMode= _node->_value.as_string().as_int();
	}

	/* load playmode */
	_node = _sxml.find_path ("/myleft/play_volume");
	if (_node) {
	m_musicSetting.volume= _node->_value.as_string().as_int();
	}

	/* load down lrc */
	_node = _sxml.find_path ("/myleft/music_downLrc");
	if (_node) {
	m_musicSetting.downLrc= _node->_value.as_string().as_int();
	}
	
	/* load pic */
	_node = _sxml.find_path ("/myleft/music_downPic");
	if (_node) {
	m_musicSetting.downPic= _node->_value.as_string().as_int();
	}
	
	/* load rtsp_data_timeout */
	_node = _sxml.find_path ("/myleft/rtsp_data_timeout");
	if (_node) {
		_rtsp_data_timeout = _node->_value.as_string().as_int();
	}
	/* load sound_volume */
	_node = _sxml.find_path ("/myleft/sound_volume");
	if (_node ) {
		_saved_sound_volume = _node->_value.as_string().as_int();
		_sound_volume = _saved_sound_volume;
	}  
	/* load http_iap_id */
	_node = _sxml.find_path ("/myleft/http_iap_id");
	if (_node) {
		_http_iap_id = _node->_value.as_string().as_int ();
		FC_Logsp("CenterCore::LoadConfig()  _http_iap_id=%d ",_http_iap_id);
		if (!CenterCore::AssureIAP (_http_iap_id))
		{
			FC_Logsp("CenterCore::AssureIAP() not ");
			_http_iap_id = 0;
		}
	}

	
	/* load pic */
	_node = _sxml.find_path ("/myleft/read_downPic");
	if (_node) {
	m_readerSetting.downPic= _node->_value.as_string().as_int();
	}
	/* load color */
	_node = _sxml.find_path ("/myleft/font_r");
	if (_node ) {
		m_readerSetting.textColor.R = _node->_value.as_string().as_int();
	}  
	_node = _sxml.find_path ("/myleft/font_g");
	if (_node ) {
		m_readerSetting.textColor.G = _node->_value.as_string().as_int();
	}  
	_node = _sxml.find_path ("/myleft/font_b");
	if (_node ) {
		m_readerSetting.textColor.B = _node->_value.as_string().as_int();
	}  
	_node = _sxml.find_path ("/myleft/back_r");
	if (_node ) {
		m_readerSetting.backColor.R = _node->_value.as_string().as_int();
	}  
	_node = _sxml.find_path ("/myleft/back_g");
	if (_node ) {
		m_readerSetting.backColor.G = _node->_value.as_string().as_int();
	}  
	_node = _sxml.find_path ("/myleft/back_b");
	if (_node ) {
		m_readerSetting.backColor.B = _node->_value.as_string().as_int();
	}  

	_node = _sxml.find_path ("/myleft/autoRool");
	if (_node ) {
		m_readerSetting.autoRool = _node->_value.as_string().as_int();
	}  
	_node = _sxml.find_path ("/myleft/autoTime");
	if (_node ) {
		m_readerSetting.autoTime = _node->_value.as_string().as_int();
	} 
	_node = _sxml.find_path ("/myleft/textSize");
	if (_node ) {
		m_readerSetting.textSize = _node->_value.as_string().as_int();
	} 
	_node = _sxml.find_path ("/myleft/backPicId");
	if (_node ) {
		m_readerSetting.backPicId= _node->_value.as_string().as_int();
	} 
	_node = _sxml.find_path ("/myleft/bBcakPic");
	if (_node ) {
		m_readerSetting.bBcakPic= _node->_value.as_string().as_int();
	} 
	
	/* load file save path */
	_node = _sxml.find_path ("/myleft/save_path");
	if (_node) {
		ansistring valueName;
		valueName = _node->get_value().as_string();
		TPtrC8 _root_path_ptrc8((const TUint8*)valueName.as_char(), valueName.length());
		FC_Utf8ToUnicodeL( _root_path, _root_path_ptrc8 );
	}

	// download mode
	_node = _sxml.find_path ("/myleft/download_mode");
	if (_node) {
		_sina_download_mode_flag = _node->_value.as_string().as_int();
	}
	// loc id
	_node = _sxml.find_path ("/myleft/loc_id");
	if (_node) {
		_sina_user_loc_id = _node->_value.as_string().as_int();
	}


	delete _xml_hbuf;
	_file.Close ();
	_fs.Close ();
	return 1;
}

int CenterCore::SaveConfig (TInt aDefault) {
	RFs _fs;
	RFile _file;
	TInt _ret;

	_fs.Connect ();
	if(aDefault)
		_ret = _file.Replace (_fs, DEFAULT_CONFIG_FILENAME, EFileWrite);
	else
		_ret = _file.Replace (_fs, CONFIG_FILENAME, EFileWrite);
	if (KErrNone != _ret) {
		_fs.Close ();
		return 0;
	}

	ansistring _xml_string("");
	oansistringstream _xml_oss (_xml_string);
	HBufC8 *_text_buf8_hbuf = HBufC8::NewL(128);
	TPtr8 _text_buf8 = _text_buf8_hbuf->Des();

	sxml _sxml;
	_sxml.add ("/myleft/background_light", _background_light);
	_sxml.add ("/myleft/pause_when_bg", _pause_when_background);
	_sxml.add ("/myleft/play_mode", m_musicSetting.playMode);
	_sxml.add ("/myleft/play_volume", m_musicSetting.volume);
	_sxml.add ("/myleft/music_downPic", (int)m_musicSetting.downPic);
	_sxml.add ("/myleft/music_downLrc", (int)m_musicSetting.downLrc);
	_sxml.add ("/myleft/rtsp_data_timeout", _rtsp_data_timeout);
	_sxml.add ("/myleft/sound_volume", (int)_saved_sound_volume);
	_sxml.add ("/myleft/http_iap_id", (int)_http_iap_id);
	_sxml.add ("/myleft/download_mode", (int)_sina_download_mode_flag);
	_sxml.add ("/myleft/loc_id", (int)_sina_user_loc_id);

	_sxml.add ("/myleft/font_r", (int)m_readerSetting.textColor.R);
	_sxml.add ("/myleft/font_g", (int)m_readerSetting.textColor.G);
	_sxml.add ("/myleft/font_b", (int)m_readerSetting.textColor.B);
	_sxml.add ("/myleft/back_r", (int)m_readerSetting.backColor.R);
	_sxml.add ("/myleft/back_g", (int)m_readerSetting.backColor.G);
	_sxml.add ("/myleft/back_b", (int)m_readerSetting.backColor.B);
	_sxml.add ("/myleft/read_downPic", (int)m_readerSetting.downPic);
	_sxml.add ("/myleft/autoRool", (int)m_readerSetting.autoRool);
	_sxml.add ("/myleft/autoTime", (int)m_readerSetting.autoTime);
	_sxml.add ("/myleft/bBcakPic", (int)m_readerSetting.bBcakPic);
	_sxml.add ("/myleft/backPicId", (int)m_readerSetting.backPicId);
	_sxml.add ("/myleft/textSize", (int)m_readerSetting.textSize);
	
	if(_root_path.Length()){
	_text_buf8.SetLength(0);
	FC_UnicodeToUtf8L( _text_buf8, _root_path );
	_sxml.add ("/myleft/save_path", (char *)_text_buf8.PtrZ());
	}
	
	_sxml.dump_stream (_xml_oss);
	int _length = strlen (_xml_string.as_char()) + 1;

	TPtr8 _xml_tptr8 ((TUint8*)(_xml_string.as_char()), _length, _length);
	if (KErrNone != _file.Write (_xml_tptr8)) {
		_file.Close ();
		_fs.Close ();
		delete _text_buf8_hbuf;
		return 0;
	}
	
	_file.Close ();
	_fs.Close ();
	delete _text_buf8_hbuf;
	return 1;
}

// Oem information
int CenterCore::LoadOemConfig () 
{

	if( _oem_loaded )
		return 1;

	_oem_loaded = ETrue;

	RFs _fs;
	RFile _file;
	TInt _ret;

	_fs.Connect ();
	_ret = _file.Open (_fs, OEM_FILENAME, EFileRead);
	if (KErrNone != _ret) {
		CenterCore::ShowConfirmationMessage (
						_L("Lost Oem file"));
		_fs.Close ();
		return 0;
	}

	HBufC8 *_xml_hbuf = HBufC8::NewL (30*1024);
	TPtr8 _xml_tptr8 = _xml_hbuf->Des ();
	if (KErrNone != _file.Read (_xml_tptr8)) {
		delete _xml_hbuf;
		_file.Close ();
		_fs.Close ();
		return 0;
	}
	const char* _xml_string = (const char*)_xml_tptr8.PtrZ ();
	sxml _sxml;
	TRAPD ( r, _sxml.load_string (_xml_string) );
	if( r!=KErrNone )
	{
		delete _xml_hbuf;
		_file.Close ();
		_fs.Close ();

		// delete this file
		CenterCore::DeleteFile( OEM_FILENAME );
		return 0;
	}
	sxml_node *_node;

	TPtrC8 _str8;
	ansistring _str = ansistring("");

	/* load provider_name */
	_node = _sxml.find_path ("/oem/provider_name");
	if (_node) {
		_str = _node->_value.as_string();
		_str8.Set((unsigned char*)_str.as_char(), _str.length());
		FC_Utf8ToUnicodeL (_provider_name, _str8);
	}

	/* load wap proxy address */
	_node = _sxml.find_path ("/oem/chatroom_proxy_address");
	if (_node) {
		_str = _node->_value.as_string();
		_str8.Set((unsigned char*)_str.as_char(), _str.length());
		FC_Utf8ToUnicodeL (m_chatroom_address, _str8);
	}

	/* load wap proxy port */
	_node = _sxml.find_path ("/oem/chatroom_proxy_port");
	if (_node) {
		m_chatroom_port= _node->_value.as_string().as_int();
	}

	/* load sina music address */
	_node = _sxml.find_path ( "/oem/myleft_address" );
	if (_node) {
		_str = _node->_value.as_string();
		_str8.Set((unsigned char*)_str.as_char(), _str.length());
		FC_Utf8ToUnicodeL (_myleft_address, _str8);
	}

	/* load sina music address */
	_node = _sxml.find_path ( "/oem/down_address" );
	if (_node) {
		_str = _node->_value.as_string();
		_str8.Set((unsigned char*)_str.as_char(), _str.length());
		FC_Utf8ToUnicodeL (_down_address, _str8);
	}

	delete _xml_hbuf;
	_file.Close ();
	_fs.Close ();
	return 1;
}

void CenterCore::GetDownPicPath(TDes& aPath)
{
	aPath.SetLength(0);
	if(m_PicSavePath.Length())
	{
		aPath.Copy(m_PicSavePath);
		return;
	}

	m_PicSavePath.Copy(_root_path);
	m_PicSavePath.Append(_L("Pic\\"));
	
	RFs fsSession;
	User::LeaveIfError(fsSession.Connect()); 
	fsSession.MkDirAll(m_PicSavePath); //KErrNone if successful
	fsSession.Close(); // close file server session

	aPath.Copy(m_PicSavePath);
}

void CenterCore::SetMainDownloadPath(const TDesC& aPath )
{
	RFs fsSession;
	User::LeaveIfError(fsSession.Connect()); 
	fsSession.MkDirAll(aPath); //KErrNone if successful
	fsSession.Close(); // close file server session

	_root_path.Copy(aPath);
}

const TDesC& CenterCore::GetMainDownloadPath()
{
	LoadOemConfig();
	return _root_path;
}

const TDesC& CenterCore::GetMainpageAddress()
{
	LoadOemConfig();
	return _myleft_address;
}

const TDesC& CenterCore::GetDownLoadAddress()
{
	LoadOemConfig();
	return _down_address;
}

const TDesC& CenterCore::GetWapProxyAddress() 
{
	LoadOemConfig();
	return _cmcc_wap_address;
}
TInt CenterCore::GetWapProxyPort() 
{
	LoadOemConfig();
	return _cmcc_wap_port;
}

const TDesC& CenterCore::GetChatRoomProxyAddress() 
{
	LoadOemConfig();
	return m_chatroom_address;
}
TInt CenterCore::GetChatRoomProxyPort() 
{
	LoadOemConfig();
	return m_chatroom_port;
}

const TDesC& CenterCore::GetServiceCenterNumber() 
{
	if( _service_center_number.Length()<=0 )
		GetDefaultServiceCenterNumber( _service_center_number );

	return _service_center_number;
}

void CenterCore::GetDefaultServiceCenterNumber( TDes& aNumber ) 
{

	CMsvSession* msvSession = CMsvSession::OpenSyncL(*this);
	msvSession->SetReceiveEntryEvents(EFalse);
	CClientMtmRegistry* clientMtmReg = CClientMtmRegistry::NewL(*msvSession);
	CSmsClientMtm* smsMtm = static_cast<CSmsClientMtm*>(clientMtmReg->NewMtmL(KUidMsgTypeSMS));


	CSmsSettings &settings=smsMtm->ServiceSettings();

#ifndef EKA2
	TInt scc=settings.NumSCAddresses();
	TInt def=settings.DefaultSC();
#else
	TInt scc=settings.ServiceCenterCount();
	TInt def=settings.DefaultServiceCenter();
#endif

	if(scc>0)
	{

		aNumber.Copy( 
#ifndef EKA2
			settings.SCAddress
#else
			settings.GetServiceCenter
#endif

			(((def >= 0) && (def < scc)) ? def:0).Address()
			);
	}	

	delete smsMtm;
	delete clientMtmReg;
	delete msvSession;
}


int CenterCore::AssureIAP (TUint _id) 
{
	CCommsDatabase *_db;
	_db = CCommsDatabase::NewL (EDatabaseTypeIAP);
	CleanupStack::PushL (_db);

	CCommsDbTableView* commView = _db->OpenTableLC(TPtrC(IAP));
	if (commView->GotoFirstRecord ()==KErrNone) 
	{
		do 
		{
			TUint32 iapId =0;
			commView->ReadUintL(TPtrC(COMMDB_ID), iapId);
			FC_Log("AssureIAP::iapId=%d",iapId);
			if (iapId == _id) 
			{
				CleanupStack::PopAndDestroy (2);
				return 1;
			}
		} while (commView->GotoNextRecord ()==KErrNone);
	}
	CleanupStack::PopAndDestroy (2);
	return 0;

}

TInt CenterCore::CheckHttpWapOrNet_Obsolete()
{

	HBufC *_http_iap_name_hbuf = HBufC::NewL(128);
	TPtr _http_iap_name = _http_iap_name_hbuf->Des();
#ifdef _NOKIA_90_
	TInt ret = 0; // net default
#else
	TInt ret = 1; // wap
#endif

	//
	// get iap name by id
	//
	CCommsDatabase *_db;
	_db = CCommsDatabase::NewL (EDatabaseTypeIAP);
	CleanupStack::PushL (_db);
	CCommsDbTableView* commView = _db->OpenTableLC(TPtrC(IAP));

	TInt err = commView->GotoFirstRecord();
	while (err == KErrNone)
	{
		TUint32 id = 0;
		commView->ReadUintL(TPtrC(COMMDB_ID), id);
		FC_Logsp( "CheckHttpWapOrNet_Obsolete: commView->aIapId=%d",id);	
		
		if(id==_http_iap_id)
		{
			FC_Logsp( "CheckHttpWapOrNet_Obsolete:-------- find iapid-------------");	
			commView->ReadTextL(TPtrC(COMMDB_NAME), _http_iap_name);
			break;
		}
		err = commView->GotoNextRecord();
	}

	CleanupStack::PopAndDestroy (2);

	// 
	if( _http_iap_name.Length()>0 )
	{
		if( _http_iap_name.MatchF( TextResource(R_TEXT_NET_APN1) )!= KErrNotFound )
			ret = 0;
		if( _http_iap_name.MatchF( TextResource(R_TEXT_NET_APN2) )!= KErrNotFound )
			ret = 0;
		if( _http_iap_name.MatchF( TextResource(R_TEXT_NET_APN3) )!= KErrNotFound )
			ret = 0;
		if( _http_iap_name.MatchF( TextResource(R_TEXT_WAP_APN1) )!= KErrNotFound )
			ret = 1;
		if( _http_iap_name.MatchF( TextResource(R_TEXT_WAP_APN2) )!= KErrNotFound )
			ret = 1;
		if( _http_iap_name.MatchF( TextResource(R_TEXT_WAP_APN3) )!= KErrNotFound )
			ret = 1;
	}

	delete _http_iap_name_hbuf;	

	//	iLogger->Log( (TUint)ret );
	//	iLogger->Log( _L("\r\n") );

	return ret; //wap
}

TInt CenterCore::CheckHttpWapOrNet(TInt aIapId, TDes* aServiceType) 
{
#ifdef _CMCC_DIRECT_HTTP_
	return 0;	// net
#endif

//	TInt ret = 1;	// wap
	TInt ret = 0;	// net
#ifndef _NOKIA_90_
	ret = CheckHttpWapOrNet_Obsolete(aIapId);
#else

	
	if( aIapId==0 )
		return 0;
	
	TBuf<128> proxyAddress = GetWapProxyAddress() ;
	TInt proxyPort = GetWapProxyPort();
	TBuf<KCommsDbSvrRealMaxFieldLength> serviceType;

	TBool proxyEnabled;
	TInt iapService=-1;
	TRAPD( r1, iapService = GetIapServiceFromAccessPointIdL( aIapId, serviceType ) );
	if( r1==KErrNone && iapService!=-1 )
	{
		
		TRAPD( r, proxyEnabled = FetchProxyAddressPortL( iapService, serviceType, proxyAddress, proxyPort ) );
		if( r==KErrNone)
		{
			if( proxyEnabled )
			{
				ret = 1;
				_cmcc_wap_address.Copy( proxyAddress );
				_cmcc_wap_port = proxyPort;
			}
			else
				ret = 0;
		}
		else
		{
			FC_Log("FetchProxyAddressPortL fail: %d",(int)r);
			
			ret = CheckHttpWapOrNet_Obsolete(aIapId);
		}
	}
	else
	{
		FC_Log("FetchProxyAddressPortL fail2:%d",(int)iapService);
			
		ret = CheckHttpWapOrNet_Obsolete(aIapId);
	}

	if( aServiceType )
		aServiceType->Copy( serviceType );
#endif
	FC_Logsp("CheckHttpWapOrNet---1.1----, ret: %d",(int)ret);
	return ret; //wap
}

TInt CenterCore::CheckHttpWapOrNet_Obsolete(TInt aIapId)
{
	FC_Log( "CFondoGlobal::CheckHttpWapOrNet_Obsolete: ");	
	
	
	HBufC *_http_iap_name_hbuf = HBufC::NewL(128);
	TPtr _http_iap_name = _http_iap_name_hbuf->Des();
	TInt ret = 0; // net default
//	TInt ret = 1; // wap

	//
	// get iap name by id
	//
	CCommsDatabase *_db;
	_db = CCommsDatabase::NewL (EDatabaseTypeIAP);
	CleanupStack::PushL (_db);
	CCommsDbTableView* commView = _db->OpenTableLC(TPtrC(IAP));
	FC_Logsp( "CheckHttpWapOrNet_Obsolete: aIapId=%d",aIapId);	
 	
    TInt err = commView->GotoFirstRecord();
    while (err == KErrNone)
    {
		TUint32 id = 0;
		commView->ReadUintL(TPtrC(COMMDB_ID), id);
		FC_Logsp( "CheckHttpWapOrNet_Obsolete: commView->aIapId=%d",id);	
		
		if(id==aIapId)
		{
			FC_Logsp( "CheckHttpWapOrNet_Obsolete:-------- find iapid-------------");	
			commView->ReadTextL(TPtrC(COMMDB_NAME), _http_iap_name);
			break;
		}
		err = commView->GotoNextRecord();
    }

	/*
	CApSelect* _select = CApSelect::NewLC (*_db,
		KEApIspTypeAll, EApBearerTypeAll, KEApSortUidAscending);
	if (_select->MoveToFirst ()) {
		do {
			FC_Logsp( "CFondoGlobal::CheckHttpWapOrNet_Obsolete: _select->Uid()=%d",(int)_select->Uid());	
			if (_select->Uid() == aIapId) {
				_http_iap_name.Copy( _select->Name() );
				break;
			}
		} while (_select->MoveNext ());
	}
*/	
	CleanupStack::PopAndDestroy (2);

	// 
	if( _http_iap_name.Length()>0 )
	{
		char httpname[32];
		FC_TBuf16ToCharp(httpname, _http_iap_name);
		FC_LogT( "CheckHttpWapOrNet_Obsolete: httpname=%s",_http_iap_name);	
		if( _http_iap_name.MatchF( TextResource(R_TEXT_NET_APN1) )!= KErrNotFound )
			ret = 0;
		if( _http_iap_name.MatchF( TextResource(R_TEXT_NET_APN2) )!= KErrNotFound )
			ret = 0;
		if( _http_iap_name.MatchF( TextResource(R_TEXT_NET_APN3) )!= KErrNotFound )
			ret = 0;
		if( _http_iap_name.MatchF( TextResource(R_TEXT_WAP_APN1) )!= KErrNotFound )
			ret = 1;
		if( _http_iap_name.MatchF( TextResource(R_TEXT_WAP_APN2) )!= KErrNotFound )
			ret = 1;
		if( _http_iap_name.MatchF( TextResource(R_TEXT_WAP_APN3) )!= KErrNotFound )
			ret = 1;
	}

	delete _http_iap_name_hbuf;	
	FC_Logsp( "CheckHttpWapOrNet_Obsolete: ret=%d",ret);	
	return ret; //wap
}

TInt CenterCore::CheckHttpWapOrNet(TInt aIapId) 
{
#ifdef _CMCC_DIRECT_HTTP_
	return 0;	// net
#endif

#ifndef _NOKIA_90_
	TInt ret = CheckHttpWapOrNet_Obsolete(aIapId);
#else

	TInt ret = 1;	// wap
	if( aIapId==0 )
	{
		return 0;
	}

	TBuf<128> proxyAddress = GetWapProxyAddress() ;
	TInt proxyPort = GetWapProxyPort();

	TBool proxyEnabled;
	TInt iapService=-1;
	TRAPD( r1, iapService = GetIapServiceFromAccessPointIdL( aIapId ) );
	if( r1==KErrNone && iapService!=-1 )
	{
	
		TRAPD( r, proxyEnabled = FetchProxyAddressPortL( iapService, proxyAddress, proxyPort ) );
		if( r==KErrNone)
		{
			if( proxyEnabled )
			{
				ret = 1;
				_cmcc_wap_address.Copy( proxyAddress );
				_cmcc_wap_port = proxyPort;
			}
			else
				ret = 0;
		}
		else
		{
			
			ret = CheckHttpWapOrNet_Obsolete(aIapId);
		}
	}
	else
	{
			
		ret = CheckHttpWapOrNet_Obsolete(aIapId);
	}
#endif
	FC_Logsp("CheckHttpWapOrNet------1-----, ret: %d",(int)ret);
	return ret; //wap
}
TInt CenterCore::CheckHttpWapOrNet() 
{

#ifndef _NOKIA_90_
	TInt ret = CheckHttpWapOrNet_Obsolete();
#else
	TInt ret = 1;	// wap
	if( _http_iap_id==0 )
	{
		return 0;
	}

	TBuf<128> proxyAddress = GetWapProxyAddress() ;
	TInt proxyPort = GetWapProxyPort();

	TBool proxyEnabled;
	TInt iapService=-1;
	TRAPD( r1, iapService = GetIapServiceFromAccessPointIdL( _http_iap_id ) );
	if( r1==KErrNone && iapService!=-1 )
	{
	
		TRAPD( r, proxyEnabled = FetchProxyAddressPortL( iapService, proxyAddress, proxyPort ) );
		if( r==KErrNone)
		{
			if( proxyEnabled )
			{
				ret = 1;
				_cmcc_wap_address.Copy( proxyAddress );
				_cmcc_wap_port = proxyPort;
			}
			else
				ret = 0;
		}
		else
		{
			ret = CheckHttpWapOrNet_Obsolete();
		}
	}
	else
	{
			
		ret = CheckHttpWapOrNet_Obsolete();
	}
#endif
	FC_Logsp("CheckHttpWapOrNet------3-----, ret: %d",(int)ret);
	return ret; //wap
}

#include <aputils.h>
TInt CenterCore::GetIapServiceFromAccessPointIdL(TInt aIapId)
{
	
	CCommsDatabase* commDb = CCommsDatabase::NewL(EDatabaseTypeIAP);
	CleanupStack::PushL(commDb);
	CCommsDbTableView* commView = commDb->OpenTableLC(TPtrC(IAP));
    
	CApUtils* pUtils = CApUtils::NewLC(*commDb);
	TInt retId = -1;
    
    TInt err = commView->GotoFirstRecord();
    while (err == KErrNone)
    {
		TUint32 id = 0;
		commView->ReadUintL(TPtrC(COMMDB_ID), id);
		
		HBufC *name = NULL;
		name = commView->ReadLongTextLC(TPtrC(COMMDB_NAME));
		
		TUint32 iapService = 0;
		commView->ReadUintL(TPtrC(IAP_SERVICE), iapService);
				
		CleanupStack::PopAndDestroy(name);		//name
		
		TBool exists = pUtils->IAPExistsL(id);
		if (exists && id==aIapId)
		{
		 	retId = iapService; 
	 		break;	  
		}	
		
		err = commView->GotoNextRecord();
    }
    
    CleanupStack::PopAndDestroy( 3 );
    
    return retId;
}

TInt CenterCore::GetIapServiceFromAccessPointIdL(TInt aIapId, TDes& aServiceType)
{
	FC_Log("In GetIapServiceFromAccessPointIdL: ");
	
	CCommsDatabase* commDb = CCommsDatabase::NewL(EDatabaseTypeIAP);
	CleanupStack::PushL(commDb);
	CCommsDbTableView* commView = commDb->OpenTableLC(TPtrC(IAP));
    
	CApUtils* pUtils = CApUtils::NewLC(*commDb);
	TInt retId = -1;
    
    TInt err = commView->GotoFirstRecord();
    while (err == KErrNone)
    {
		TUint32 id = 0;
		commView->ReadUintL(TPtrC(COMMDB_ID), id);
		
		HBufC *name = NULL;
		name = commView->ReadLongTextLC(TPtrC(COMMDB_NAME));
		
		TUint32 iapService = 0;
		commView->ReadUintL(TPtrC(IAP_SERVICE), iapService);

		HBufC* serviceType = NULL;
		serviceType = commView->ReadLongTextLC(TPtrC(IAP_SERVICE_TYPE));
		aServiceType.Copy( serviceType->Des() );
				
		CleanupStack::PopAndDestroy(serviceType);	// serviceType
		CleanupStack::PopAndDestroy(name);		//name
		
		
		TBool exists = pUtils->IAPExistsL(id);
		if (exists && id==aIapId)
		{
		 	retId = iapService; 
	 		break;	  
		}	
		
		err = commView->GotoNextRecord();
    }
    
    CleanupStack::PopAndDestroy( 3 );
    
    return retId;
}

TBool CenterCore::FetchProxyAddressPortL(TInt aIapService, const TDesC& aServiceType, 
										   TDes& aProxyAddr, TInt& aProxyPort)
{
	CCommsDatabase* commDb = CCommsDatabase::NewL(EDatabaseTypeIAP);
	CleanupStack::PushL(commDb);
	
	TBool proxyEnabled = EFalse;
	TUint32 port = 0;
	TUint32 ispId;
	HBufC* serviceType_hbuf = HBufC::NewL( KCommsDbSvrRealMaxFieldLength );
	TPtr serviceType = serviceType_hbuf->Des();
	TBool userProxy;
	
	CCommsDbTableView* commView = commDb->OpenTableLC(TPtrC(PROXIES));
	
	if (commView->GotoFirstRecord() == KErrNone)
	{
		do
		{
			commView->ReadUintL( TPtrC(PROXY_ISP), ispId );

			HBufC* serviceTypeTemp = commView->ReadLongTextLC(TPtrC(PROXY_SERVICE_TYPE));
			serviceType.Copy( serviceTypeTemp->Des() );

			commView->ReadBoolL(TPtrC(PROXY_USE_PROXY_SERVER), userProxy);
			
			HBufC* serverName = commView->ReadLongTextLC(TPtrC(PROXY_SERVER_NAME));
			
			aProxyAddr.Copy( serverName->Des() );

			commView->ReadUintL(TPtrC(PROXY_PORT_NUMBER), port);

			aProxyPort = port;

			CleanupStack::PopAndDestroy(serverName);
			CleanupStack::PopAndDestroy(serviceTypeTemp);
			
			if( ispId==aIapService && 0==serviceType.CompareF(aServiceType) )
			{
				proxyEnabled = ETrue;
				break;
			}
		}while (commView->GotoNextRecord() == KErrNone);
	}

	CleanupStack::PopAndDestroy(commView);
	CleanupStack::PopAndDestroy(commDb);
	delete serviceType_hbuf;
	FC_Log("End FetchProxyAddressPortL.\r\n");

	return proxyEnabled;
}

TBool CenterCore::FetchProxyAddressPortL(TInt aIapService, TDes& aProxyAddr, TInt& aProxyPort)
{
	CCommsDatabase* commDb = CCommsDatabase::NewL(EDatabaseTypeIAP);
	CleanupStack::PushL(commDb);
	
	TBool proxyEnabled = EFalse;
	TUint32 port = 0;
	TUint32 ispId;
	
	CCommsDbTableView* commView = commDb->OpenTableLC(TPtrC(PROXIES));
	
	if (commView->GotoFirstRecord() == KErrNone)
	{
		do
		{
			commView->ReadUintL( TPtrC(PROXY_ISP), ispId );
			
			HBufC* serverName = commView->ReadLongTextLC(TPtrC(PROXY_SERVER_NAME));
			
			aProxyAddr.Copy( serverName->Des() );

			commView->ReadUintL(TPtrC(PROXY_PORT_NUMBER), port);

			aProxyPort = port;

			CleanupStack::PopAndDestroy(serverName);
			
			if( ispId==aIapService )
			{
				proxyEnabled = ETrue;
				break;
			}
		}while (commView->GotoNextRecord() == KErrNone);
	}

	CleanupStack::PopAndDestroy(commView);
	CleanupStack::PopAndDestroy(commDb);

	return proxyEnabled;
}

void CenterCore::Music_GetDownMP3Path(TDes& aPath)
{
	aPath.SetLength(0);
	if(m_mp3SavePath.Length())
	{
		aPath.Copy(m_mp3SavePath);
		return;
	}

	m_mp3SavePath.Copy(_root_path);
	m_mp3SavePath.Append(_L("MP3\\"));
	
	RFs fsSession;
	User::LeaveIfError(fsSession.Connect()); 
	fsSession.MkDirAll(m_mp3SavePath); //KErrNone if successful
	fsSession.Close(); // close file server session

	aPath.Copy(m_mp3SavePath);
}


//------------Other-----------------End------------

static char* str_tolower(char* pBuf)
{
	char *p = pBuf;
	while(*p)
	{
		*p = tolower(*p);
		p++;
	}

	return pBuf;
}
 void CenterCore:: GetDirInfo(RFs& iSessionRFs,const TDes & aFileName,char * _ext)
{
	CDir* dirList=NULL;
	
	iSessionRFs.GetDir(aFileName,KEntryAttMaskSupported,EDirsFirst,dirList);

	if(!dirList)
		return;
	for(int i=0;i<dirList->Count();i++)
	{
		TBuf<256>  dirpath;
		dirpath.SetLength(0);
		if((*dirList)[i].IsDir())
		{
			char filepath[256];
			memset(filepath,0,256);
			dirpath.Append(aFileName);
			dirpath.Append((*dirList)[i].iName);
			dirpath.Append(_L("\\"));
			FC_TBuf16ToCharp(filepath, dirpath);
		//	FC_Log("filepath=%s",filepath);
			GetDirInfo(iSessionRFs,dirpath,_ext);
	
		}
		else
		{	
			char filename[125];
			memset(filename,0,125);
			FC_TBuf16ToCharp(filename,(*dirList)[i].iName);
			str_tolower(filename);
			
			if( strstr( filename, _ext))
			{
				FC_Log("filename=%s",filename);
				TBuf<256>  Bfilename;
				Bfilename.Append(aFileName);
				Bfilename.Append((*dirList)[i].iName);

				if(!strcmp( _ext, ".txt"))
					Read_localListAppend(Bfilename,(*dirList)[i].iName);
				else if(!strcmp( _ext, ".mp3"))
					Music_localListAppend(Bfilename,(*dirList)[i].iName);
			}
			
		}
	
	}
	
	if(dirList)
		delete dirList;

}

  void CenterCore:: GetDirInfo(RFs& iSessionRFs,const TDes & aFileName)
{
	CDir* dirList=NULL;
	
	iSessionRFs.GetDir(aFileName,KEntryAttMaskSupported,EDirsFirst,dirList);

	if(!dirList)
		return;
	for(int i=0;i<dirList->Count();i++)
	{
		TBuf<256>  dirpath;
		dirpath.SetLength(0);
		if((*dirList)[i].IsDir())
		{
			char filepath[256];
			memset(filepath,0,256);
			dirpath.Append(aFileName);
			dirpath.Append((*dirList)[i].iName);
			dirpath.Append(_L("\\"));
			FC_TBuf16ToCharp(filepath, dirpath);
		//	FC_Log("filepath=%s",filepath);
			GetDirInfo(iSessionRFs,dirpath);
	
		}
		else
		{	
			char filename[125];
			memset(filename,0,125);
			FC_TBuf16ToCharp(filename,(*dirList)[i].iName);
			str_tolower(filename);
			
			if( strstr( filename, ".txt") || strstr( filename, ".mp3"))
			{
				FC_Log("filename=%s",filename);
				TBuf<256>  Bfilename;
				Bfilename.Append(aFileName);
				Bfilename.Append((*dirList)[i].iName);

				if(strstr( filename, ".txt"))
					Read_localListAppend(Bfilename,(*dirList)[i].iName);
				else if(strstr( filename, ".mp3") )
					Music_localListAppend(Bfilename,(*dirList)[i].iName);
			}
			
		}
	
	}
	
	if(dirList)
		delete dirList;

}
//*************************8STATIC********************BEGIN***********************

//--------------  E:\System\Apps\FunVV\-------------
void CenterCore::GetFullPathName(TDes& aFileName)
{
#if 1
	aFileName = CEikonEnv::Static()->EikAppUi()->Application()->AppFullName();
	TInt pos = aFileName.LocateReverse('\\');
	aFileName.SetLength(pos+1);

	char * apppath=(char*)FC_malloc(128);
	memset(apppath,0,128);
	FC_TBuf16ToCharp(apppath, aFileName);
	FC_Log("CenterCore::GetFullPathName =%s ",apppath);
#else
	// Get default drive and path
	TParse parse;
	TFileName appPath;
	TBuf<5> appDrive;
	parse.Set(CEikonEnv::Static()->EikAppUi()->Application()->AppFullName(), NULL, NULL);

	appPath.Copy(parse.DriveAndPath());
	appDrive.Copy(parse.Drive());

	// Parse the file name
	parse.Set(aFileName, NULL, NULL);
	if (parse.Drive().Length() == 0)
	{
		if (parse.Path().Length() == 0)
		{ // Use default path and drive
			aFileName.Insert(0, appPath);
		}
		else
		{ // Use default drive
			aFileName.Insert(0, appDrive);
		}
	}
#endif
}




//*************************STATIC***********************************END

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// Fundo FData
void CenterCore::UrlParamEncode( const TDesC& strIn, TDes& strEncode, TBool bAll )
{
	char _utf8_str[1024];
	memset( _utf8_str, 0, 1024 );
	TPtr8 _utf8_ptr( (TUint8*)_utf8_str, 1024 );

	char _encode_str[1024];
	memset( _encode_str, 0, 1024);
	int	nPos = 0;

	FC_UnicodeToUtf8L( _utf8_ptr, strIn );

	TUint8*	p = (TUint8*)_utf8_str;
	while( 1 )
	{
		if( *p == 0 )
			break;

		if( !bAll &&
			(((*p >= 'a') && (*p <= 'z'))
			|| ((*p >= 'A') && (*p <= 'Z'))
			|| ((*p >= '0') && (*p <= '9'))))
		{
			_encode_str[nPos++] = *p;
		}
		else
		{
			sprintf( _encode_str+nPos, "%%%02X", *p);
			nPos += 3;
		}

		p++;
	}

	TPtrC8 _encode_ptr( (const TUint8*)_encode_str, strlen(_encode_str) );
	FC_Logsp("UrlParamEncode::_encode_str=%s",_encode_str);
	FC_Utf8ToUnicodeL( strEncode, _encode_ptr );
}

void CenterCore::SaveNodeValueAsDesc(sxml_node* _node, TDes& _string)
{
	TPtrC8 _value8;
	ansistring _node_value = _node->get_value().as_string();
	_string.SetLength(0);

	if( !_node_value.is_null() && _node_value.length()>0 )
	{
		_value8.Set( (const TUint8*)_node_value.as_char(), _node_value.length() );
		FC_Utf8ToUnicodeL( _string, _value8 );
	}
}

void CenterCore::SaveNodeValueAsInt(sxml_node* _node, TInt& _value)
{
	TPtrC8 _value8;
	ansistring _node_value = _node->get_value().as_string();
	_value = 0;

	if( !_node_value.is_null() && _node_value.length()>0 )
	{
		_value = _node_value.as_int();
	}
}

TInt CenterCore::FindNodeAndSaveToDesc(sxml_node* _root_node, const char* _find, TDes& _string)
{
	_string.SetLength(0);
	sxml_node* node = _root_node->find_path( _find );
	if( !node )
		return 0;
	ansistring nodeString = node->get_value().as_string();
	if( !nodeString.is_null() && nodeString.length()>0 )
	{
		if( nodeString.length()<=_string.MaxLength() )
			SaveNodeValueAsDesc( node, _string );
		else
		{
			TPtrC8 _value8;
			_value8.Set( (const TUint8*)nodeString.as_char(), _string.MaxLength() );
			FC_Utf8ToUnicodeL( _string, _value8 );
		}
	}

	return 1;
}

TInt CenterCore::FindNodeAndSaveToInt(sxml_node* _root_node, const char* _find, TInt& _int)
{
	_int = 0;
	sxml_node* node = _root_node->find_path( _find );
	if( !node )
		return 0;
	SaveNodeValueAsInt( node, _int );
	return 1;
}
//*******************STATIC FUNCTION************Begin****************
void CenterCore::DumpException(const char* _str, unsigned char _append)
{
	RFs fs;
	RFile file;

	User::LeaveIfError( fs.Connect() );
	TInt err;
	if( _append )
	{
		err = file.Open( fs, KEXCEPTION_FILENAME, EFileWrite );
		if( err != KErrNone )
		{// can not open file
			err = file.Replace( fs, KEXCEPTION_FILENAME, EFileWrite );
		}
		else
		{// seek to end
			TInt pos = 0;
			file.Seek(ESeekEnd, pos);
		}
	}
	else
		err = file.Replace(fs, KEXCEPTION_FILENAME, EFileWrite);

	if( err == KErrNone )
	{// open file ok
		TPtrC8 strPtrC8((const TUint8*)_str);
		file.Write( strPtrC8 );

		file.Close();
	}

	fs.Close();
}

//*******************STATIC FUNCTION************End****************

//-----------for--music------begin--------------

void CenterCore::Music_SetVolume(int _szflag)
{
	if(10<_szflag || _szflag<0)
		return;
	m_musicSetting.volume=_szflag;
	if(iMediaCore)
		iMediaCore->SetVolume(_szflag);
	return;
}


int CenterCore::LoadMusicList () 
{
	RFs _fs;
	RFile _file;
	TInt _ret;
	HBufC8 *_xml_hbuf = HBufC8::NewL (50*1024);
	TPtr8 _xml_tptr8 = _xml_hbuf->Des ();
	const char* _xml_string = (const char*)_xml_tptr8.PtrZ ();
	sxml _sxml;
	sxml_node *_node;
	int count;
	FC_Logsp("CenterCore::LoadMusicList()  ");
	_fs.Connect ();
	_ret = _file.Open (_fs, MusicLocal_FileName, EFileRead);
	if (KErrNone != _ret) 
	{
		delete _xml_hbuf;
		_fs.Close ();
		FC_Log("CenterCore::LoadMusicList() _file.Open   failed ");
		return 0;
	}

	if (KErrNone != _file.Read (_xml_tptr8)) 
	{
		delete _xml_hbuf;
		_file.Close ();
		_fs.Close ();
		FC_Log("CenterCore::LoadMusicList() _file.Read   failed ");
		return 0;
	}

	if(!strlen(_xml_string))
	{
		delete _xml_hbuf;
		_file.Close ();
		_fs.Close ();
		FC_Log("CenterCore::LoadMusicList() _xml_string is null ");
		return 0;
	}

	FC_Logsp("CenterCore::LoadMusicList()  _xml_string =%s ",_xml_string);
	TRAPD ( r, _sxml.load_string (_xml_string) );
	if( r!=KErrNone )
	{
		delete _xml_hbuf;
		_file.Close ();
		_fs.Close ();

		// delete this file
		CenterCore::DeleteFile( MusicLocal_FileName );
		FC_Logsp("CenterCore::LoadMusicList() _sxml.load_string  failed ");
		return 0;
	}
	
	_node = _sxml.find_path ("/musiclocallist/count");
	if (_node) {
		count = _node->_value.as_string().as_int();
	}

	if(count )
	{
		_node = _sxml.find_path( "musiclocallist/list" );
		Music_localListReset();
		int i, n = _node->child_count();
		sxml_node *node1;
		for( i=0; i<n; i++ )
		{
			node1 = _node->childs(i);
			if( node1->_name==ansistring("listItem") )
			{
				HBufC* url_hbuf = HBufC::NewL( 256);
				HBufC* title_hbuf = HBufC::NewL( 64 );
				TPtr url= url_hbuf->Des();
				TPtr title= title_hbuf->Des();
				
				if( !FindNodeAndSaveToDesc( node1, "listItem/iaskurl", url) )
				{
					delete url_hbuf;
					delete title_hbuf;
					break;
				}
				
				if( !FindNodeAndSaveToDesc( node1, "listItem/title", title) )
				{
					delete url_hbuf;
					delete title_hbuf;
					break;
				}
				Music_localListAppend(url,title);
				delete url_hbuf;
				delete title_hbuf;
			}
		}

					
	}

	delete _xml_hbuf;
	_file.Close ();
	_fs.Close ();
	FC_Logsp("CenterCore::LoadMusicList()  OK ");
	return 1;
}

int CenterCore::SaveMusicList()
{
	RFs _fs;
	RFile _file;
	TInt _ret;
	HBufC8 *_text_buf8_hbuf = HBufC8::NewL(256);
	TPtr8 _text_buf8 = _text_buf8_hbuf->Des();
	sxml _sxml;
	sxml_node *_node;
	int count=0;
	
	_fs.Connect ();
	_ret = _file.Replace (_fs, MusicLocal_FileName, EFileWrite);
	if (KErrNone != _ret) {
		_fs.Close ();
		return 0;
	}
	FC_Logsp("CenterCore::SaveMusicList()   ");
	ansistring _xml_string("");
	oansistringstream _xml_oss (_xml_string);

	count=Music_GetLocalMusicCount();
	_sxml.add ("/musiclocallist/count",count );
	if(count)
	{
		_node=_sxml.create_path ("/musiclocallist/list");
		for(TInt i=0;i<count;i++)
		{
			sxml_node *_node1=_node->add_child("listItem");
			music_list* list =  m_localMusiclist->At(i);
			
			_text_buf8.SetLength(0);
			FC_UnicodeToUtf8L( _text_buf8, list->iaskurl);
			_node1->add_child ("iaskurl", (const char *)_text_buf8.PtrZ());
			
			_text_buf8.SetLength(0);
			FC_UnicodeToUtf8L( _text_buf8, list->title);
			_node1->add_child ("title", (const char *)_text_buf8.PtrZ());
		}
	}
	
	
	_sxml.dump_stream (_xml_oss);
	int _length = strlen (_xml_string.as_char()) + 1;

	TPtr8 _xml_tptr8 ((TUint8*)(_xml_string.as_char()), _length, _length);
//	FC_Logsp("CenterCore::SaveMusicList()  string =%s ",_xml_string.as_char());
	if (KErrNone != _file.Write (_xml_tptr8)) {
		_file.Close ();
		_fs.Close ();
		delete _text_buf8_hbuf;
		return 0;
	}
	
	_file.Close ();
	_fs.Close ();
	delete _text_buf8_hbuf;
	
	return 1;
}

int CenterCore::LoadMusicMaskList () 
{
	RFs _fs;
	RFile _file;
	TInt _ret;
	HBufC8 *_xml_hbuf = HBufC8::NewL (30*1024);
	TPtr8 _xml_tptr8 = _xml_hbuf->Des ();
	const char* _xml_string = (const char*)_xml_tptr8.PtrZ ();
	sxml _sxml;
	sxml_node *_node;
	int count;
	FC_Logsp("CenterCore::LoadMusicMaskList()  ");
	_fs.Connect ();
	_ret = _file.Open (_fs, MusicMask_FileName, EFileRead);
	if (KErrNone != _ret) 
	{
		delete _xml_hbuf;
		_fs.Close ();
		FC_Logsp("CenterCore::LoadMusicMaskList() _file.Open   failed ");
		return 0;
	}

	if (KErrNone != _file.Read (_xml_tptr8)) 
	{
		delete _xml_hbuf;
		_file.Close ();
		_fs.Close ();
		FC_Log("CenterCore::LoadMusicMaskList() _file.Read   failed ");
		return 0;
	}

	FC_Logsp("CenterCore::LoadMusicMaskList() _xml_string=%s ",_xml_string);

	if(!strlen(_xml_string))
	{
		delete _xml_hbuf;
		_file.Close ();
		_fs.Close ();
		return 0;
	}
		

	TRAPD ( r, _sxml.load_string (_xml_string) );
	if( r!=KErrNone )
	{
		delete _xml_hbuf;
		_file.Close ();
		_fs.Close ();

		// delete this file
		CenterCore::DeleteFile( MusicMask_FileName );
		FC_Logsp("CenterCore::LoadReadList() _sxml.load_string  failed ");
		return 0;
	}
	
	_node = _sxml.find_path ("/musicMasklist/count");
	if (_node) {
		count = _node->_value.as_string().as_int();
	}
	FC_Logsp("CenterCore::LoadMusicMaskList() count=%d ",count);
	if(count )
	{
		_node = _sxml.find_path( "musicMasklist/list" );
		Music_MaskListReset();
		int i, n = _node->child_count();
		sxml_node *node1;
		for( i=0; i<n; i++ )
		{
			node1 = _node->childs(i);
			if( node1->_name==ansistring("listItem") )
			{
				HBufC* url_hbuf = HBufC::NewL( 256);
				HBufC* title_hbuf = HBufC::NewL( 64 );
				TPtr url= url_hbuf->Des();
				TPtr title= title_hbuf->Des();
				
				if( !FindNodeAndSaveToDesc( node1, "listItem/iaskurl", url) )
				{
					delete url_hbuf;
					delete title_hbuf;
					break;
				}
				
				if( !FindNodeAndSaveToDesc( node1, "listItem/title", title) )
				{
					delete url_hbuf;
					delete title_hbuf;
					break;
				}
				Music_MaskListAppend(url,title);
				delete url_hbuf;
				delete title_hbuf;
			}
		}

					
	}

	delete _xml_hbuf;
	_file.Close ();
	_fs.Close ();
	FC_Logsp("CenterCore::LoadMusicMaskList() OK");
	return 1;
}


int CenterCore::SaveMusicMaskList () 
{
	RFs _fs;
	RFile _file;
	TInt _ret;
	HBufC8 *_text_buf8_hbuf = HBufC8::NewL(256);
	TPtr8 _text_buf8 = _text_buf8_hbuf->Des();
	sxml _sxml;
	sxml_node *_node;
	int count=0;
	FC_Logsp("CenterCore::SaveMusicMaskList()");
	_fs.Connect ();
	_ret = _file.Replace (_fs, MusicMask_FileName, EFileWrite);
	if (KErrNone != _ret) {
		_fs.Close ();
		FC_Logsp("CenterCore::SaveMusicMaskList(),Replace failed");
		return 0;
	}

	ansistring _xml_string("");
	oansistringstream _xml_oss (_xml_string);

	count=m_musicMasklist->Count();
	_sxml.add ("/musicMasklist/count",count );
	if(count)
	{
		_node=_sxml.create_path ("/musicMasklist/list");
		for(TInt i=0;i<count;i++)
		{
			sxml_node *_node1=_node->add_child("listItem");
			music_list* list =  m_musicMasklist->At(i);
			
			_text_buf8.SetLength(0);
			FC_UnicodeToUtf8L( _text_buf8, list->iaskurl);
			_node1->add_child ("iaskurl", (const char *)_text_buf8.PtrZ());
			
			_text_buf8.SetLength(0);
			FC_UnicodeToUtf8L( _text_buf8, list->title);
			_node1->add_child ("title", (const char *)_text_buf8.PtrZ());
		}
	}
	
	
	_sxml.dump_stream (_xml_oss);
	int _length = strlen (_xml_string.as_char()) + 1;
//	FC_Logsp("CenterCore::SaveMusicMaskList(),_xml_string=%s",_xml_string.as_char());
	TPtr8 _xml_tptr8 ((TUint8*)(_xml_string.as_char()), _length, _length);
	if (KErrNone != _file.Write (_xml_tptr8)) {
		_file.Close ();
		_fs.Close ();
		delete _text_buf8_hbuf;
		return 0;
	}
	
	_file.Close ();
	_fs.Close ();
	delete _text_buf8_hbuf;
	FC_Logsp("CenterCore::SaveMusicMaskList()  OK");
	return 1;
}


TInt CenterCore::Music_GetLocalMusicCount()
{
	return m_localMusiclist->Count();
}


void CenterCore::Music_localListAppend(const TDesC& _url,const TDesC& _title )
{
	int i=0;
	music_list* templist=new music_list();

	if(!templist)
	{
		FC_Log("Music_localListAppend::FC_malloc ()  failed");
		return;
	}
	templist->iaskurl.Copy(_url);
	templist->title.Copy(_title);
	
	m_localMusiclist->AppendL(templist);

}


void CenterCore::Music_localListDelete(const TInt _index)
{
	music_list* list =  m_localMusiclist->At(_index);
	if(list)
			delete list;	

	m_localMusiclist->Delete(_index);
	m_localMusiclist->Compress();

}


void CenterCore::Music_localListReset()
{
	for(TInt i=0; i<m_localMusiclist->Count(); i++ )
	{
		music_list* list =  m_localMusiclist->At(i);
		if(list)
			delete list;
	}

	m_localMusiclist->Reset();

}


int CenterCore::Music_MaskListAppend(const TDesC& _url,const TDesC& _title)
{
	music_list* templist=NULL;
	for(int i=0;i<m_musicMasklist->Count();i++)
	{
		templist=m_musicMasklist->At(i);
		if(templist->iaskurl==_url)
			return i;
	}
	
	templist=new music_list();

	if(!templist)
	{
		FC_Log("Music_localListAppend::FC_malloc ()  failed");
		return -1;
	}
	templist->iaskurl.Copy(_url);
	templist->title.Copy(_title);
	
	m_musicMasklist->AppendL(templist);
	FC_Logsp("Music_localListAppend::OK count =%d",m_musicMasklist->Count());
	return m_musicMasklist->Count()-1;

}


void CenterCore::Music_MaskListReset()
{
	for(TInt i=0; i<m_musicMasklist->Count(); i++ )
	{
		music_list* list =  m_musicMasklist->At(i);
		if(list)
			delete list;
	}

	m_musicMasklist->Reset();

}


void CenterCore::Music_MaskListDelete(const TInt _index)
{
	music_list* list =  m_musicMasklist->At(_index);
	if(list)
			delete list;	

	m_musicMasklist->Delete(_index);
	m_musicMasklist->Compress();
}

void CenterCore::Music_ClassifiesAppend(TInt _typeId,TInt _id,const TDesC& _title )
{
	int i=0;
	music_bdclass* templist=new music_bdclass();

	if(!templist)
	{
		FC_Log("Music_ClassifiesAppend::FC_malloc ()  failed");
		return;
	}
	templist->classes=MYLEFG_API_CLASSIFIES;
	templist->Id=_id;
	templist->typeId=_typeId;
	templist->title.Copy(_title);
	
	m_musicClassifies->AppendL(templist);
}

void CenterCore::Music_ChannelsAppend(TInt _typeId,TInt _id,const TDesC& _title )
{
	int i=0;
	music_bdclass* templist=new music_bdclass();

	if(!templist)
	{
		FC_Log("Music_ChannelsAppend::FC_malloc ()  failed");
		return;
	}
	templist->classes=MYLEFG_API_CHANNELS;
	templist->Id=_id;
	templist->typeId=_typeId;
	templist->title.Copy(_title);
	
	m_musicChannels->AppendL(templist);
}

void CenterCore::Music_bangdListReset()
{

	for(TInt i=0; i<m_musiclist->Count(); i++ )
	{
		song * list =  m_musiclist->At(i);
		if(list)
			delete list;
	}

	m_musiclist->Reset();
}

void CenterCore::Music_bangdListAppend(TInt _Id,const TDesC& _name,const TDesC& _auth,TInt _picID,TInt _lrcID)
{
	int i=0;
	song * templist=new song();

	if(!templist)
	{
		FC_Log("Music_bangdListAppend::FC_malloc ()  failed");
		return;
	}
	templist->id=_Id;
	templist->name.Copy(_name);
	templist->singer.Copy(_auth);
	templist->iaskurl=KNullDesC;
	templist->iPicId=_picID;
	templist->iLrcId=_lrcID;
	
	m_musiclist->AppendL(templist);
}

void CenterCore::Music_LrcListReset()
{
	for(TInt i=0; i<m_musiclrclist->Count(); i++ )
	{
		song* list =  m_musiclrclist->At(i);
		if(list)
			delete list;
	}

	m_musiclrclist->Reset();
}


void CenterCore::Music_LrcListAppend(TInt _Id,const TDesC& _name,const TDesC& _singer )
{
	int i=0;
	song* templist=new song();

	if(!templist)
	{
		FC_Log("Music_LrcListAppend::FC_malloc ()  failed");
		return;
	}
	templist->id=-1;
	templist->iLrcId=_Id;
	templist->iaskurl=KNullDesC;
	templist->name.Copy(_name);
	templist->singer.Copy(_singer);
	
	m_musiclrclist->AppendL(templist);
	FC_Logsp("Music_LrcListAppend::append %d",m_musiclrclist->Count());
}

void CenterCore::Music_SearchListReset()
{
	for(TInt i=0; i<m_musicSearch->Count(); i++ )
	{
		song* list =  m_musicSearch->At(i);
		if(list)
			delete list;
	}

	m_musicSearch->Reset();
}


void CenterCore::Music_SearchListAppend(TInt _Id,const TDesC& _name,const TDesC& _singer,TInt _picID,TInt _lrcID )
{
	int i=0;
	song* templist=new song();

	if(!templist)
	{
		FC_Log("Music_SearchListAppend::FC_malloc ()  failed");
		return;
	}
	templist->id=_Id;
	templist->iaskurl=KNullDesC;
	templist->name.Copy(_name);
	templist->singer.Copy(_singer);
	templist->iPicId=_picID;
	templist->iLrcId=_lrcID;
	
	m_musicSearch->AppendL(templist);
	FC_Logsp("Music_SearchListAppend::append %d",m_musicSearch->Count());
}


unsigned long CenterCore::Music_GetCurTime() 
{
	if(iMediaCore)
	{
		TInt64 pos=iMediaCore->GetPosition()/1000000;
#ifdef _S60_2RD_
		return pos.GetTInt();
#else
		return (unsigned long)pos;
#endif
	}
	return 0;
}

void CenterCore::Music_Seek(DWORD pTime)
{
	if(iMediaCore && m_IsLocalPlay)
	{
		iMediaCore->PauserL();
		iMediaCore->SetPosition((TInt)pTime*1000000);
		iMediaCore->Play();
	}
}

BOOL CenterCore::Music_SeekAndGetLrc()
{
	DWORD curTime=0;
	if(m_IsLocalPlay && iMediaCore)
	{
		TInt64 pos=iMediaCore->GetPosition()/1000;
#ifdef _S60_2RD_
		curTime= pos.GetTInt();
#else
		curTime=pos;
#endif
	}

	if(m_subtitle)
		m_subtitle->Seek(curTime);
	return 1;
}

void CenterCore::Music_SetPlayer(song *_curSong,TInt _duration)
{
	curPlayer->iaskurl.SetLength(0);
//	curPlayer->iwapDownload.SetLength(0);
	curPlayer->name.SetLength(0);
	curPlayer->singer.SetLength(0);
	curPlayer->id=-1;
	
	curPlayer->iaskurl.Copy(_curSong->iaskurl);
//	curPlayer->iwapDownload.Copy(_curSong->iwapDownload);
	curPlayer->name.Copy(_curSong->name);
	curPlayer->singer.Copy(_curSong->singer);
	m_duration=_duration;
//	ReSetTaskInfo();
}

void CenterCore::Music_SetPlayerAndSubtitle(song *_curSong)
{
	int result=0;
	curPlayer->iaskurl.SetLength(0);
//	curPlayer->iwapDownload.SetLength(0);
	curPlayer->name.SetLength(0);
	curPlayer->singer.SetLength(0);
		
	curPlayer->iaskurl.Copy(_curSong->iaskurl);
	curPlayer->id=_curSong->id;
//	curPlayer->iwapDownload.Copy(_curSong->iwapDownload);
	curPlayer->name.Copy(_curSong->name);
	curPlayer->singer.Copy(_curSong->singer);
	
	
	
	TBool isLocal=ETrue;;
	result=Music_SetSubtitle(_curSong->iaskurl);

	if(!result)
	{
		TParse parser;
		TFileName desPaht;
		TBuf<32>  name;
		if(isLocal)
		{
			parser.Set( _curSong->iaskurl, NULL, NULL );
			desPaht.Copy( parser.DriveAndPath() );
			name.Copy(parser.Name());
		}
		iSinaMgr->LaunchSearchLrcUrl(curPlayer->singer,name,KNullDesC,desPaht);
	}
}

int CenterCore::Music_SetSubtitle(TDesC & filename)
{
	TBuf<256> iSubtitleFile;
	TBuf<125> iName;
	TParse parser;
	parser.Set( filename, NULL, NULL );
	iName.Copy( parser.Name() );

	if(m_subtitle)
	{
		delete m_subtitle;

		m_subtitle=NULL;
	}
	if(CLrcSubtitle::Exists(filename, iSubtitleFile)||CLrcSubtitle::Exists(m_mp3SavePath,iName, iSubtitleFile))
	{
		int count ;
		char  name[256];
		memset(name,0,256);
		FC_TBuf16ToCharp(name, iSubtitleFile);
		m_subtitle=new CLrcSubtitle(name);
		m_subtitle->Load();

		m_appUI->viewMusic->SetLrcLibleShowId(ELrcLibShow_HaveLocaLRC);
		return 1;
	}

	return 0;
}

void CenterCore::Music_PlayerListNext()
{
	TInt _index,count;
	song tempsong;
	count=m_musicMasklist->Count();
	
	if(m_musicSetting.playMode==0) //shunxu
	{
		if(m_iCurretPlayIndex==count-1)
			m_iCurretPlayIndex=0;
		else
			m_iCurretPlayIndex++;
	}
	else if(m_musicSetting.playMode==2)  //suiji
	{
		TUint32  random=Math::Random();
		m_iCurretPlayIndex=(TInt)random%count;
		FC_Log("Music_PlayerListNext::random=%lu ,m_iCurretPlayIndex=%d",(unsigned long)random,m_iCurretPlayIndex);
	}//else m_playMode==1 is xunhuan .m_iCurretPlayIndex not changed
	
	if(m_iCurretPlayIndex!=-1)
	{
		m_appUI->viewMusic->ResetAll();
		Music_GetPlayerListSong(&tempsong, m_iCurretPlayIndex);
		Music_playSong(&tempsong);
	}
//	ReSetTaskInfo();
}

void CenterCore::Music_GetPlayerListSong(song* _CurSong,TInt _index)
{
	if(_index<0 ||_index> m_musicMasklist->Count()-1)
		return ;
	music_list* templist=m_musicMasklist->At(_index);;
	_CurSong->iaskurl.Copy(templist->iaskurl);
	_CurSong->name.Copy(templist->title);
}

 void CenterCore::Music_playerSetCurIndex(TInt index)
{
	m_iCurretPlayIndex=index;
}

void CenterCore::Music_playSong(song* _CurSong)
{
	
	if(iMediaCore)
			iMediaCore->StopL();	
	m_IsLocalPlay=ETrue;

	if(_CurSong->iaskurl.FindF(_L("http://"))!=KErrNotFound)
	{
		FC_Logsp("Music_playSong ,Find http://");
		m_IsLocalPlay=EFalse;
	}
	else
	{
		if(!CenterCore::IsFileExist(_CurSong->iaskurl))
		{
			ShowMessage(TextResource(R_MESSAGE_FILE_NOT_EXIST));
			Music_MaskListDelete(m_iCurretPlayIndex);
			m_iCurretPlayIndex--;
		//	playerNext();
			return ;
		}

	}
	Music_SetPlayerAndSubtitle(_CurSong);
	
	if(iMediaCore)
	{
		iMediaCore->OpenMdeiaL(_CurSong->iaskurl);			
	}
//	ReSetTaskInfo();
}

void CenterCore::Music_playerPlay()
{
	if(iMediaCore)
		iMediaCore->Play();	
}

 void CenterCore::Music_playerPause()
{
	if(iMediaCore && m_IsLocalPlay)
	{
		iMediaCore->PauserL();
	}
	ReSetTaskInfo();
}
 
 void CenterCore::Music_playerStop()
{
	m_duration=0;
	if(m_IsLocalPlay)
	{
		if(iMediaCore)
			iMediaCore->StopL();	
	}
	m_musicStatus=CCCB_STATUS_STOP;
	ReSetTaskInfo();
}

//----------for--read--list----------

TInt CenterCore::GetBackPic(TInt index, TDes& _fullName)
{
	TInt result=0;
	RFs iSessionRFs;
	User::LeaveIfError(iSessionRFs.Connect()); 	
	CDir* dirList=NULL;

	TInt isE=BaflUtils::PathExists(iSessionRFs,KBackPictrueFolderE);
	if(isE)
		iSessionRFs.GetDir(KBackPictrueFolderE,KEntryAttMaskSupported,EDirsFirst,dirList);
	else
		iSessionRFs.GetDir(KBackPictrueFolderC,KEntryAttMaskSupported,EDirsFirst,dirList);

	if(!dirList)
	{
		iSessionRFs.Close();
		return 0;
	}
	_LIT(KExt,".jpg");
	_LIT(KHonor,"MyleftPic");
	TBuf<32>  PicName;
	PicName.Copy(KHonor);
	PicName.AppendNum(index);
	PicName.Append(KExt);	
	
	for(int i=0;i<dirList->Count();i++)
	{
		if((*dirList)[i].iName==PicName)
		{
			result=1;
			if(isE)
				_fullName.Copy(KBackPictrueFolderE);
			else
				_fullName.Copy(KBackPictrueFolderC);
			_fullName.Append((*dirList)[i].iName);
		}
	}
	iSessionRFs.Close();

	delete dirList;
	return result;

}

TInt CenterCore::GetBackPicCount()
{
	TInt result=0;
	RFs iSessionRFs;
	User::LeaveIfError(iSessionRFs.Connect()); 	
	CDir* dirList=NULL;
	
	if(BaflUtils::PathExists(iSessionRFs,KBackPictrueFolderE))
		iSessionRFs.GetDir(KBackPictrueFolderE,KEntryAttMaskSupported,EDirsFirst,dirList);
	else
		iSessionRFs.GetDir(KBackPictrueFolderC,KEntryAttMaskSupported,EDirsFirst,dirList);
	
	if(!dirList)
		return 0;

	_LIT(KExt,".jpg");
	_LIT(KHonor,"MyleftPic");

	for(int j=0;j<dirList->Count();j++)
	{

		TBuf<32>  PicName;
		PicName.Copy(KHonor);
		PicName.AppendNum(j);
		PicName.Append(KExt);	

		for(int i=0;i<dirList->Count();i++)
		{

			if((*dirList)[i].iName==PicName)
			{
				result++;
			}
		}
	}
	iSessionRFs.Close();

	return result;

}

void CenterCore::UpdataLocalReadList()
{
	TBuf<125> Cdir,Edir;
	Cdir.Append(_L("c:\\"));
	Edir.Append(_L("e:\\"));
	
	Read_localListReset();

	RFs iSessionRFs;
	User::LeaveIfError(iSessionRFs.Connect()); 
	
	GetDirInfo(iSessionRFs,Cdir,".txt");
	
#ifndef __WINS__
	if(FC_IsDirectoryExist("e:\\"))
		GetDirInfo(iSessionRFs,Edir,".txt");
#endif
	FC_Log("UpDataPlayList()   ok ");
	iSessionRFs.Close();

}

void CenterCore::UpdataLocalMusicList()
{
	TBuf<125> Cdir,Edir;
	Cdir.Append(_L("c:\\"));
	Edir.Append(_L("e:\\"));
	
	Music_localListReset();

	RFs iSessionRFs;
	User::LeaveIfError(iSessionRFs.Connect()); 
	
	GetDirInfo(iSessionRFs,Cdir,".mp3");
	
#ifndef __WINS__
	if(FC_IsDirectoryExist("e:\\"))
		GetDirInfo(iSessionRFs,Edir,".mp3");
#endif
	FC_Log("UpDataPlayList()   ok ");
	iSessionRFs.Close();

}

void CenterCore::UpdataLocalAllList()
{
	TBuf<125> Cdir,Edir;
	Cdir.Append(_L("c:\\"));
	Edir.Append(_L("e:\\"));
	
	Read_localListReset();
	Music_localListReset();
	
	RFs iSessionRFs;
	User::LeaveIfError(iSessionRFs.Connect()); 
	
	GetDirInfo(iSessionRFs,Cdir);
	
#ifndef __WINS__
	if(FC_IsDirectoryExist("e:\\"))
		GetDirInfo(iSessionRFs,Edir);
#endif
	FC_Log("UpDataPlayList()   ok ");
	iSessionRFs.Close();

}


TInt CenterCore::GetLocalReadCount()
{
	return m_localReadlist->Count();
}
void CenterCore::Read_localListReset()
{

	for(TInt i=0; i<m_localReadlist->Count(); i++ )
	{
		read_list* list =  m_localReadlist->At(i);
		if(list)
			delete list;
	}

	m_localReadlist->Reset();
}

void CenterCore::Read_localListAppend(const TDesC& _url,const TDesC& _title )
{
	int i=0;
	read_list* templist=new read_list();

	if(!templist)
	{
		FC_Log("Read_localListAppend::FC_malloc ()  failed");
		return;
	}
	templist->iaskurl.Copy(_url);
	templist->title.Copy(_title);
	
	m_localReadlist->AppendL(templist);
	FC_Log("Read_localListAppend:: OK ,count=",m_localReadlist->Count());
}

void CenterCore::Read_localListDelete(const TInt _index)
{
	read_list* list =  m_localReadlist->At(_index);
	if(list)
			delete list;	

	m_localReadlist->Delete(_index);
	m_localReadlist->Compress();
}

void CenterCore::Read_MaskListReset()
{

	for(TInt i=0; i<m_readMasklist->Count(); i++ )
	{
		read_masklist* list =  m_readMasklist->At(i);
		if(list)
			delete list;
	}

	m_readMasklist->Reset();
}

void CenterCore::Read_MaskListLoad(const TDesC& _url,const TDesC& _title ,const TInt _pos)
{
	TInt ret=-1;
	
	read_masklist* tempitem=new read_masklist();
	
	tempitem->iaskurl.Copy(_url);
	tempitem->title.Copy(_title);
	tempitem->iPos=_pos;
	m_readMasklist->AppendL(tempitem);
}

void CenterCore::Read_MaskListAppend(const TDesC& _url,const TDesC& _title ,const TInt _pos)
{
	TInt ret=-1;
	
	for(TInt i=0; i<m_readMasklist->Count(); i++ )
	{
		read_masklist* list =  m_readMasklist->At(i);
		if(list->iaskurl==_url)   //has old mask 
		{
			if(i!=0)
			{
				read_masklist* Begin =  m_readMasklist->At(0);
				read_masklist  temp;
				
				temp.iaskurl=Begin->iaskurl;
				temp.iPos=Begin->iPos;
				temp.title=Begin->title;

				Begin->iaskurl.Copy(_url);
				Begin->title.Copy(_title);
				Begin->iPos=_pos;

				list->iaskurl.Copy(temp.iaskurl);
				list->title.Copy(temp.title);
				list->iPos=temp.iPos;

			}
			else
			{
				list->iPos=_pos;
			}
			return;

		}
	}

	CArrayFixFlat<read_masklist*> *m_templist= new (ELeave) CArrayFixFlat<read_masklist*> (4);
	read_masklist* tempitem=new read_masklist();
	
	tempitem->iaskurl.Copy(_url);
	tempitem->title.Copy(_title);
	tempitem->iPos=_pos;
	m_templist->AppendL(tempitem);

	for(TInt i=0; i<m_readMasklist->Count(); i++ )
	{
		read_masklist* item =  m_readMasklist->At(i);
		m_templist->AppendL(item);
	}
	
	m_readMasklist->Reset();	
	delete m_readMasklist;
	m_readMasklist=m_templist;
}

void CenterCore::Read_MaskListDelete(const TInt _index)
{
	read_masklist* list =  m_readMasklist->At(_index);
	if(list)
			delete list;	

	m_readMasklist->Delete(_index);
	m_readMasklist->Compress();
}

void CenterCore::Read_ClassifiesAppend(TInt _typeId,TInt _id,const TDesC& _title )
{
	int i=0;
	read_bdclass* templist=new read_bdclass();

	if(!templist)
	{
		FC_Log("Read_ClassifiesAppend::FC_malloc ()  failed");
		return;
	}
	templist->classes=MYLEFG_API_CLASSIFIES;
	templist->Id=_id;
	templist->typeId=_typeId;
	templist->title.Copy(_title);
	
	m_readClassifies->AppendL(templist);
}

void CenterCore::Read_ChannelsAppend(TInt _typeId,TInt _id,const TDesC& _title )
{
	int i=0;
	read_bdclass* templist=new read_bdclass();

	if(!templist)
	{
		FC_Log("Read_ChannelsAppend::FC_malloc ()  failed");
		return;
	}
	templist->classes=MYLEFG_API_CHANNELS;
	templist->Id=_id;
	templist->typeId=_typeId;
	templist->title.Copy(_title);
	
	m_readChannels->AppendL(templist);
}


void CenterCore::Read_bangdListReset()
{

	for(TInt i=0; i<m_readList->Count(); i++ )
	{
		book* list =  m_readList->At(i);
		if(list)
			delete list;
	}

	m_readList->Reset();
}
void CenterCore::Read_bangdListAppend(TInt _Id,const TDesC& _name,const TDesC& _auth,TInt _picID )
{
	int i=0;
	book* templist=new book();

	if(!templist)
	{
		FC_Log("Read_bangdListAppend::FC_malloc ()  failed");
		return;
	}
	templist->Id=_Id;
	templist->name.Copy(_name);
	templist->auth.Copy(_auth);
	templist->iPicID=_picID;
	
	m_readList->AppendL(templist);
}

void CenterCore::Read_SearchListReset()
{

	for(TInt i=0; i<m_readSearch->Count(); i++ )
	{
		book* list =  m_readSearch->At(i);
		if(list)
			delete list;
	}

	m_readSearch->Reset();
}
void CenterCore::Read_SearchListAppend(TInt _Id,const TDesC& _name,const TDesC& _auth,TInt _picId)
{
	int i=0;
	book* templist=new book();

	if(!templist)
	{
		FC_Log("Read_SearchListAppend::FC_malloc ()  failed");
		return;
	}
	templist->Id=_Id;
	templist->name.Copy(_name);
	templist->auth.Copy(_auth);
	templist->iPicID=_picId;
	
	m_readSearch->AppendL(templist);
	FC_Logsp("Read_SearchListAppend::append %d",m_readSearch->Count());
}

int CenterCore::LoadReadList () {
	RFs _fs;
	RFile _file;
	TInt _ret;
	HBufC8 *_xml_hbuf = HBufC8::NewL (30*1024);
	TPtr8 _xml_tptr8 = _xml_hbuf->Des ();
	const char* _xml_string = (const char*)_xml_tptr8.PtrZ ();
	sxml _sxml;
	sxml_node *_node;
	int count;
	
	_fs.Connect ();
	_ret = _file.Open (_fs, READLOCAL_FILENAME, EFileRead);
	if (KErrNone != _ret) 
	{
		delete _xml_hbuf;
		_fs.Close ();
		FC_Logsp("CenterCore::LoadReadList() _file.Open   failed ");
		return 0;
	}

	if (KErrNone != _file.Read (_xml_tptr8)) 
	{
		delete _xml_hbuf;
		_file.Close ();
		_fs.Close ();
		FC_Logsp("CenterCore::LoadReadList() _file.Read   failed ");
		return 0;
	}

	if(!strlen(_xml_string))
	{
		delete _xml_hbuf;
		_file.Close ();
		_fs.Close ();
		FC_Log("CenterCore::LoadMusicList() _xml_string is null ");
		return 0;
	}

	TRAPD ( r, _sxml.load_string (_xml_string) );
	if( r!=KErrNone )
	{
		delete _xml_hbuf;
		_file.Close ();
		_fs.Close ();

		// delete this file
		CenterCore::DeleteFile( READLOCAL_FILENAME );
		FC_Logsp("CenterCore::LoadReadList() _sxml.load_string  failed ");
		return 0;
	}
	
	_node = _sxml.find_path ("/readlocallist/count");
	if (_node) {
		count = _node->_value.as_string().as_int();
	}

	if(count )
	{
		_node = _sxml.find_path( "readlocallist/list" );
		Read_localListReset();
		int i, n = _node->child_count();
		sxml_node *node1;
		for( i=0; i<n; i++ )
		{
			node1 = _node->childs(i);
			if( node1->_name==ansistring("listItem") )
			{
				HBufC* url_hbuf = HBufC::NewL( 256);
				HBufC* title_hbuf = HBufC::NewL( 64 );
				TPtr url= url_hbuf->Des();
				TPtr title= title_hbuf->Des();
				
				if( !FindNodeAndSaveToDesc( node1, "listItem/iaskurl", url) )
				{
					delete url_hbuf;
					delete title_hbuf;
					break;
				}
				
				if( !FindNodeAndSaveToDesc( node1, "listItem/title", title) )
				{
					delete url_hbuf;
					delete title_hbuf;
					break;
				}
			
				Read_localListAppend(url,title);
				delete url_hbuf;
				delete title_hbuf;
			}
		}

					
	}

	delete _xml_hbuf;
	_file.Close ();
	_fs.Close ();
	FC_Logsp("CenterCore::LoadReadList() OK ");
	return 1;
}

int CenterCore::SaveReadList () {
	RFs _fs;
	RFile _file;
	TInt _ret;
	HBufC8 *_text_buf8_hbuf = HBufC8::NewL(256);
	TPtr8 _text_buf8 = _text_buf8_hbuf->Des();
	sxml _sxml;
	sxml_node *_node;
	int count=0;
	
	_fs.Connect ();
	_ret = _file.Replace (_fs, READLOCAL_FILENAME, EFileWrite);
	if (KErrNone != _ret) {
		_fs.Close ();
		return 0;
	}

	ansistring _xml_string("");
	oansistringstream _xml_oss (_xml_string);

	count=GetLocalReadCount();
	_sxml.add ("/readlocallist/count",count );
	FC_Logsp("SaveReadList() count=%d",count);
	if(count)
	{
		_node=_sxml.create_path ("/readlocallist/list");
		for(TInt i=0;i<count;i++)
		{
			sxml_node *_node1=_node->add_child("listItem");
			read_list* list =  m_localReadlist->At(i);
			
			_text_buf8.SetLength(0);
			FC_UnicodeToUtf8L( _text_buf8, list->iaskurl);
			_node1->add_child ("iaskurl", (const char *)_text_buf8.PtrZ());
			
			_text_buf8.SetLength(0);
			FC_UnicodeToUtf8L( _text_buf8, list->title);
			_node1->add_child ("title", (const char *)_text_buf8.PtrZ());
		}
	}
	
	
	_sxml.dump_stream (_xml_oss);
	int _length = strlen (_xml_string.as_char()) + 1;
	FC_Logsp("SaveReadList() =%s",_xml_string.as_char());
	TPtr8 _xml_tptr8 ((TUint8*)(_xml_string.as_char()), _length, _length);
	if (KErrNone != _file.Write (_xml_tptr8)) {
		_file.Close ();
		_fs.Close ();
		delete _text_buf8_hbuf;
		return 0;
	}
	
	_file.Close ();
	_fs.Close ();
	delete _text_buf8_hbuf;
	FC_Logsp("CenterCore::SaveReadList() OK ");
	return 1;
}


int CenterCore::LoadReadMaskList () 
{
	RFs _fs;
	RFile _file;
	TInt _ret;
	HBufC8 *_xml_hbuf = HBufC8::NewL (20*1024);
	TPtr8 _xml_tptr8 = _xml_hbuf->Des ();
	const char* _xml_string = (const char*)_xml_tptr8.PtrZ ();
	sxml _sxml;
	sxml_node *_node;
	int count;
	
	_fs.Connect ();
	_ret = _file.Open (_fs, ReadMask_FileName, EFileRead);
	if (KErrNone != _ret) 
	{
		delete _xml_hbuf;
		_fs.Close ();
		FC_Logsp("CenterCore::LoadReadMaskList() _file.Open   failed ");
		return 0;
	}

	if (KErrNone != _file.Read (_xml_tptr8)) 
	{
		delete _xml_hbuf;
		_file.Close ();
		_fs.Close ();
		FC_Logsp("CenterCore::LoadReadMaskList() _file.Read   failed ");
		return 0;
	}
	
	if(!strlen(_xml_string))
	{
		delete _xml_hbuf;
		_file.Close ();
		_fs.Close ();
		FC_Log("CenterCore::LoadMusicList() _xml_string is null ");
		return 0;
	}
	
	TRAPD ( r, _sxml.load_string (_xml_string) );
	if( r!=KErrNone )
	{
		delete _xml_hbuf;
		_file.Close ();
		_fs.Close ();

		// delete this file
		CenterCore::DeleteFile( ReadMask_FileName );
		FC_Logsp("CenterCore::LoadReadList() _sxml.load_string  failed ");
		return 0;
	}
	
	_node = _sxml.find_path ("/readmasklist/count");
	if (_node) {
		count = _node->_value.as_string().as_int();
	}

	if(count )
	{
		_node = _sxml.find_path( "readmasklist/list" );
		Read_MaskListReset();
		int i, n = _node->child_count();
		sxml_node *node1;
		for( i=0; i<n; i++ )
		{
			node1 = _node->childs(i);
			if( node1->_name==ansistring("listItem") )
			{
				TInt pos=0;
				HBufC* url_hbuf = HBufC::NewL( 256);
				HBufC* title_hbuf = HBufC::NewL( 64 );
				TPtr url= url_hbuf->Des();
				TPtr title= title_hbuf->Des();
				
				if( !FindNodeAndSaveToDesc( node1, "listItem/iaskurl", url) )
				{
					delete url_hbuf;
					delete title_hbuf;
					break;
				}
				
				if( !FindNodeAndSaveToDesc( node1, "listItem/title", title) )
				{
					delete url_hbuf;
					delete title_hbuf;
					break;
				}
				if( !FindNodeAndSaveToInt( node1, "listItem/pos", pos) )
				{
					delete url_hbuf;
					delete title_hbuf;
					break;
				}
	
				Read_MaskListLoad(url,title,pos);
				delete url_hbuf;
				delete title_hbuf;
			}
		}

					
	}
	
	delete _xml_hbuf;
	_file.Close ();
	_fs.Close ();
	FC_Logsp("CenterCore::LoadReadMaskList() OK ");
	return 1;
}

int CenterCore::SaveReadMaskList () {
	RFs _fs;
	RFile _file;
	TInt _ret;
	HBufC8 *_text_buf8_hbuf = HBufC8::NewL(256);
	TPtr8 _text_buf8 = _text_buf8_hbuf->Des();
	sxml _sxml;
	sxml_node *_node;
	int count=0;
	
	_fs.Connect ();
	_ret = _file.Replace (_fs, ReadMask_FileName, EFileWrite);
	if (KErrNone != _ret) {
		_fs.Close ();
		return 0;
	}

	ansistring _xml_string("");
	oansistringstream _xml_oss (_xml_string);

	count=m_readMasklist->Count();
	_sxml.add ("/readmasklist/count",count );
	if(count)
	{
		_node=_sxml.create_path ("/readmasklist/list");
		for(TInt i=0;i<count;i++)
		{
			sxml_node *_node1=_node->add_child("listItem");
			read_masklist* list =  m_readMasklist->At(i);
			
			_text_buf8.SetLength(0);
			FC_UnicodeToUtf8L( _text_buf8, list->iaskurl);
			_node1->add_child ("iaskurl", (const char *)_text_buf8.PtrZ());
			
			_text_buf8.SetLength(0);
			FC_UnicodeToUtf8L( _text_buf8, list->title);
			_node1->add_child ("title", (const char *)_text_buf8.PtrZ());

			_node1->add_child ("pos", (int)list->iPos);
		}
	}
	
	
	_sxml.dump_stream (_xml_oss);
	int _length = strlen (_xml_string.as_char()) + 1;

	TPtr8 _xml_tptr8 ((TUint8*)(_xml_string.as_char()), _length, _length);
	if (KErrNone != _file.Write (_xml_tptr8)) {
		_file.Close ();
		_fs.Close ();
		delete _text_buf8_hbuf;
		return 0;
	}
	FC_Logsp("LoadReadMaskList() =%s",_xml_string.as_char());
	_file.Close ();
	_fs.Close ();
	delete _text_buf8_hbuf;
	FC_Logsp("CenterCore::SaveReadMaskList() OK ");
	return 1;
}

void CenterCore::Read_GetDownTextPath(TDes& aPath)
{
	aPath.SetLength(0);
	if(m_readSavePath.Length())
	{
		aPath.Copy(m_readSavePath);
		return;
	}

	m_readSavePath.Copy(_root_path);
	m_readSavePath.Append(_L("Text\\"));
	
	RFs fsSession;
	User::LeaveIfError(fsSession.Connect()); 
	fsSession.MkDirAll(m_readSavePath); //KErrNone if successful
	fsSession.Close(); // close file server session

	aPath.Copy(m_readSavePath);
}

void CenterCore::Read_SetRead(const TDesC& _url,const TDesC& _title ,TInt _pos)
{
	m_readTitle.Copy(_title);
	m_readPos=_pos;
	m_readUrl.Copy(_url);

	m_readStatus=1;
	ReSetTaskInfo();
}

void CenterCore::Read_SavedRead()
{

}

void CenterCore::Read_SetBackColor(TInt _r,TInt _g,TInt _b)
{
	m_readerSetting.backColor.R=_r;
	m_readerSetting.backColor.G=_g;
	m_readerSetting.backColor.B=_b;	
}
void CenterCore::Read_SetFontCloor(TInt _r,TInt _g,TInt _b)
{
	m_readerSetting.textColor.R=_r;
	m_readerSetting.textColor.G=_g;
	m_readerSetting.textColor.B=_b;
}
//********************download****begin***********************************

int CenterCore::LoadDownloadList () {
	RFs _fs;
	RFile _file;
	TInt _ret;
	HBufC8 *_xml_hbuf = HBufC8::NewL (10*1024);
	TPtr8 _xml_tptr8 = _xml_hbuf->Des ();
	const char* _xml_string = (const char*)_xml_tptr8.PtrZ ();
	sxml _sxml;
	sxml_node *_node;
	int count;
	
	_fs.Connect ();
	_ret = _file.Open (_fs, DownLoad_FileName, EFileRead);
	if (KErrNone != _ret) 
	{
		delete _xml_hbuf;
		_fs.Close ();
		FC_Logsp("CenterCore::LoadReadList() _file.Open   failed ");
		return 0;
	}

	if (KErrNone != _file.Read (_xml_tptr8)) 
	{
		delete _xml_hbuf;
		_file.Close ();
		_fs.Close ();
		FC_Logsp("CenterCore::LoadReadList() _file.Read   failed ");
		return 0;
	}
	
	if(!strlen(_xml_string))
	{
		delete _xml_hbuf;
		_file.Close ();
		_fs.Close ();
		FC_Log("CenterCore::LoadMusicList() _xml_string is null ");
		return 0;
	}
	
	TRAPD ( r, _sxml.load_string (_xml_string) );
	if( r!=KErrNone )
	{
		delete _xml_hbuf;
		_file.Close ();
		_fs.Close ();

		// delete this file
		CenterCore::DeleteFile( DownLoad_FileName );
		FC_Logsp("CenterCore::LoadReadList() _sxml.load_string  failed ");
		return 0;
	}
	
	_node = _sxml.find_path ("/download/count");
	if (_node) {
		count = _node->_value.as_string().as_int();
	}

	if(count )
	{
		_node = _sxml.find_path( "download/list" );
	//	Read_localListReset();
		int i, n = _node->child_count();
		sxml_node *node1;
		for( i=0; i<n; i++ )
		{
			node1 = _node->childs(i);
			if( node1->_name==ansistring("listItem") )
			{
				HBufC* title_hbuf = HBufC::NewL( 256 );
				TPtr title= title_hbuf->Des();
				HBufC* ask_hbuf = HBufC::NewL( 256 );
				TPtr ask= title_hbuf->Des();

				TInt id=0;
				TInt size=0;
				TInt length=0;
				TInt status;
				TInt typeId=0;
				if( !FindNodeAndSaveToDesc( node1, "listItem/filename", title) )
				{
					delete title_hbuf;
					delete ask_hbuf;
					break;
				}
				if( !FindNodeAndSaveToInt( node1, "listItem/size", size) )
				{
					delete title_hbuf;
					delete ask_hbuf;
					break;
				}
				if( !FindNodeAndSaveToInt( node1, "listItem/length", length) )
				{
					delete title_hbuf;
					delete ask_hbuf;
					break;
				}
				if( !FindNodeAndSaveToInt( node1, "listItem/id", id) )
				{
					delete title_hbuf;
					delete ask_hbuf;
					break;
				}
				if( !FindNodeAndSaveToInt( node1, "listItem/status", status) )
				{
					delete title_hbuf;
					delete ask_hbuf;
					break;
				}
				if( !FindNodeAndSaveToInt( node1, "listItem/typeid", typeId) )
				{
					delete title_hbuf;
					delete ask_hbuf;
					break;
				}

				Download_ListLoad(title,id,size,length,(TdownloadStatus )status,typeId);
				delete title_hbuf;
				delete ask_hbuf;
			}
		}

					
	}

	delete _xml_hbuf;
	_file.Close ();
	_fs.Close ();
	FC_Logsp("CenterCore::LoadReadList() OK ");
	return 1;
}

int CenterCore::SaveDownloadList () {
	RFs _fs;
	RFile _file;
	TInt _ret;
	HBufC8 *_text_buf8_hbuf = HBufC8::NewL(256);
	TPtr8 _text_buf8 = _text_buf8_hbuf->Des();
	sxml _sxml;
	sxml_node *_node;
	int count=0;
	
	_fs.Connect ();
	_ret = _file.Replace (_fs, DownLoad_FileName, EFileWrite);
	if (KErrNone != _ret) {
		delete _text_buf8_hbuf;
		_fs.Close ();
		return 0;
	}

	ansistring _xml_string("");
	oansistringstream _xml_oss (_xml_string);

	count=m_downloadlist->Count();
	_sxml.add ("/download/count",count );
	if(count)
	{
		_node=_sxml.create_path ("/download/list");
		for(TInt i=0;i<count;i++)
		{
			sxml_node *_node1=_node->add_child("listItem");
			download* list =  m_downloadlist->At(i);
			
			_text_buf8.SetLength(0);
			FC_UnicodeToUtf8L( _text_buf8, list->filename);
			_node1->add_child ("filename", (const char *)_text_buf8.PtrZ());

			_text_buf8.SetLength(0);
			FC_UnicodeToUtf8L( _text_buf8, list->iaskurl);
			_node1->add_child ("askurl", (const char *)_text_buf8.PtrZ());

			_node1->add_child ("size", (int)list->size);
			_node1->add_child ("length", (int)list->length);
			_node1->add_child ("id", (int)list->id);
			_node1->add_child ("status", (int)list->status);
			_node1->add_child ("typeid", (int)list->typeId);
		}
	}
	
	
	_sxml.dump_stream (_xml_oss);
	int _length = strlen (_xml_string.as_char()) + 1;

	TPtr8 _xml_tptr8 ((TUint8*)(_xml_string.as_char()), _length, _length);
	if (KErrNone != _file.Write (_xml_tptr8)) {
		_file.Close ();
		_fs.Close ();
		delete _text_buf8_hbuf;
		return 0;
	}
	
	_file.Close ();
	_fs.Close ();
	delete _text_buf8_hbuf;
	return 1;
}

void CenterCore::Download_ListLoad(const TDesC& _filename, TInt _id,TInt _size, TInt _len,TdownloadStatus _status,TInt _typeid)
{
	int i=0;
	download* templist=new download();

	if(!templist)
	{
		FC_Log("Download_ListLoad::FC_malloc ()  failed");
		return;
	}
	templist->filename.Copy(_filename);
	templist->size=_size;
	templist->length=_len;
	templist->id=_id;
	templist->status=_status;
	templist->typeId=_typeid;
					
	m_downloadlist->AppendL(templist);
	FC_Log("Download_ListLoad::OK");
}

void CenterCore::Download_ListAppend(const TDesC& _filename, TInt _id,TInt _size, TInt _len,TdownloadStatus _status,TInt _typeid)
{
	int i=0;
	download* templist=new download();

	if(!templist)
	{
		FC_Log("Download_ListAppend::FC_malloc ()  failed");
		return;
	}
	templist->filename.Copy(_filename);
	templist->size=_size;
	templist->length=_len;
	templist->id=_id;
	templist->status=_status;
	templist->typeId=_typeid;
	
	if(_typeid==MYLEFG_API_TYPE_TXT)
		templist->filename.Append(_L(".txt"));
	else if(_typeid==MYLEFG_API_TYPE_MUSIC)
		templist->filename.Append(_L(".mp3"));
	else if(_typeid==MYLEFG_API_TYPE_LRC)
		templist->filename.Append(_L(".lrc"));
	else if(_typeid==MYLEFG_API_TYPE_PIC)
		templist->filename.Append(_L(".jpg"));

	FC_LogT("Download_ListAppend::name=%s",templist->filename);
	m_downloadlist->AppendL(templist);
	FC_Log("Download_ListAppend::OK");
}

void CenterCore::Download_ListDelete(const TInt _index)
{
	download* list =  m_downloadlist->At(_index);
	if(list)
			delete list;	

	m_downloadlist->Delete(_index);
	m_downloadlist->Compress();
}

void CenterCore::Download_ListGetDownloadItem(int i,TDes & itemtext,TUint& aLen, TUint& aSize,TdownloadStatus& _aStatus)
{
	download* list =  m_downloadlist->At(i);
	if(!list)
		return ;

	itemtext.Copy(list->filename);
	aLen=list->length;
	aSize=list->size;
	_aStatus=list->status;
	return;
}

TInt CenterCore::Download_ListGetType(const TInt _index )
{
	download* list =  m_downloadlist->At(_index);
	if(!list)
		return -1;
	
	return list->typeId;
}


void CenterCore::Download_ListSetStatus(const TInt _index, TdownloadStatus _status )
{
	download* list =  m_downloadlist->At(_index);
	if(!list)
		return ;
	
	list->status=_status;
}

int 	CenterCore::Download_ListDownloadCheck()
{
	if(m_curDownId==-1)
		return -1;
	download* list =  m_downloadlist->At(m_curDownId);
	if(!list)
		return -1;

	list->status=EdownloadIng;
	if(iDownMgr->IsDownLoading())
			iDownMgr->GetLengthAndSize((TUint&)list->length,(TUint&)list->size);

	//FC_Logsp("Download_ListDownloadCheck,size=%d,length=%d",list->size,list->length);
	return m_curDownId;

}

int 	CenterCore::Download_ListDownloadNext()
{
	for(TInt i=0;i<m_downloadlist->Count();i++)
	{
		download* list =  m_downloadlist->At(i);
		if(list->status==EdownloadReady)
		{
	
			iDownMgr->LaunchGetReadFile(list->id, list->filename,list->typeId);
			m_curDownId=i;
			break;
		}	
	}

	ReSetTaskInfo();
	FC_Logsp("Download_ListDownloadNext,m_curDownId=%d",m_curDownId);
	return m_curDownId;

}

int 	CenterCore::Download_SartDownload(TInt index)
{
	if(m_curDownId!=-1)
	{
		download* list =  m_downloadlist->At(index);
		if(list->status==EdownloadOk||list->status==EdownloadErr)
		{
	
			iDownMgr->LaunchGetReadFile(list->id, list->filename,list->typeId);
			m_curDownId=index;
		}	

		ReSetTaskInfo();
		FC_Logsp("Download_ListDownloadNext,m_curDownId=%d",m_curDownId);
	}
	return m_curDownId;

}

//*******************download*****end***********************************

//*******************chatroom*****Begin***********************************

void CenterCore::ChatR_LoginChatRoom()
{
	if(!m_CMCommlog)
			m_CMCommlog= new (ELeave) CArrayFixFlat<chatR_log*> (4);
	if(!m_CMPrevlog)
			m_CMPrevlog= new (ELeave) CArrayFixFlat<chatR_log*> (4);
	if(!m_CMBody)
			m_CMBody= new (ELeave) CArrayFixFlat<chatR_body*> (4);

	if(!iChartRData)
	{
		char addr[32];
		memset(addr,0,32);
		FC_TBuf16ToCharp(addr,m_chatroom_address);
		if(strlen(addr)&& m_chatroom_port)
		{
			iChartRData=CChartRGetter::NewL(addr, m_chatroom_port,"demo"," ");
			iChartRData->Connect();
		}
	}
	else
	{
		m_appUI->TitleBar()->ResetTitleTabMenu();
		m_appUI->ActivateLocalViewL(KViewChatRoom);

	}

}


void CenterCore::ChatR_ExitChatRoom()
{
	if(iChartRData)
	{
		delete iChartRData;
		iChartRData=NULL;
	}
	if(m_CMBody)
	{
			ChatR_BodyListReset();
			delete m_CMBody;
			m_CMBody=NULL;
	}

	if(m_CMCommlog)
	{
			ChatR_CommLogListReset();
			delete m_CMCommlog;
			m_CMCommlog=NULL;
	}
	if(m_CMPrevlog)
	{
			ChatR_PrevLogListReset();
			delete m_CMPrevlog;
			m_CMPrevlog=NULL;
	}
}

void CenterCore::ChatR_BodyListAppend(const TDesC& _name)
{
	chatR_body *tempbody= new chatR_body();
	tempbody->name=_name;
	
	m_CMBody->AppendL(tempbody);
	FC_Logsp("ChatR_BodyListAppend::append %d",m_CMBody->Count());

}

void CenterCore::ChatR_BodyListDeleteByName(const TDesC& _name)
{
	for(TInt i=0;i<m_CMBody->Count();i++)
	{
		chatR_body *tempbody=m_CMBody->At(i);
		if(tempbody->name==_name)
		{
			delete tempbody;
			m_CMBody->Delete(i);
			m_CMBody->Compress();
			break;
		}		
	}
}
void CenterCore::ChatR_BodyListDeleteById(const TInt _index)
{
	delete m_CMBody->At(_index);
	m_CMBody->Delete(_index);
	m_CMBody->Compress();
}

void CenterCore::ChatR_BodyListReset()
{
	for(TInt i=0;i<m_CMBody->Count();i++)
	{
		delete m_CMBody->At(i);
		m_CMBody->Delete(i);

	}
	m_CMBody->Reset();
}

void CenterCore::ChatR_CommLogListAppend(const TDesC& _name,const TDesC& _msg,DWORD _time)
{
	chatR_log *tempbody= new chatR_log();


	tempbody->a_chatlog = HBufC::NewL( _msg.Length()+1 );
	TPtr textPtr = tempbody->a_chatlog->Des();
	textPtr.Copy(_msg);
	
	
	tempbody->time=_time;
	tempbody->name.Copy(_name);
	if(m_CMCommlog->Count()>MAX_CHATROOM_LOG_MUN)
	{
		delete m_CMCommlog->At(0)->a_chatlog;
		delete m_CMCommlog->At(0);
		m_CMCommlog->Delete(0);
		m_CMCommlog->Compress();
	}
	
	m_CMCommlog->AppendL(tempbody);
}

void CenterCore::ChatR_CommLogListReset()
{
	for(TInt i=0; i<m_CMCommlog->Count(); i++ )
	{
		delete m_CMCommlog->At(i)->a_chatlog;
		delete m_CMCommlog->At(i);
	}
	m_CMCommlog->Reset();
}

void CenterCore::ChatR_PrevLogListAppend(const TDesC& _name,const TDesC& _msg,DWORD _time)
{
	chatR_log *tempbody= new chatR_log();


	tempbody->a_chatlog = HBufC::NewL( _msg.Length()+1 );
	TPtr textPtr = tempbody->a_chatlog->Des();
	textPtr.Copy(_msg);

	tempbody->time=_time;
	tempbody->name.Copy(_name);
	if(m_CMPrevlog->Count()>MAX_CHATROOM_LOG_MUN)
	{
		delete m_CMPrevlog->At(0)->a_chatlog;
		delete m_CMPrevlog->At(0);
		m_CMPrevlog->Delete(0);
		m_CMPrevlog->Compress();
	}
	
	m_CMPrevlog->AppendL(tempbody);
}

void CenterCore::ChatR_PrevLogListReset()
{
	for(TInt i=0; i<m_CMPrevlog->Count(); i++ )
	{
		delete m_CMPrevlog->At(i)->a_chatlog;
		delete m_CMPrevlog->At(i);
	}
	m_CMPrevlog->Reset();
}

//*******************download*****end***********************************

CFileSearchEngine::CFileSearchEngine( MFileSearchEngineNotifier* aNotifier,TInt atype )
							: CActive (EPriorityStandard)
{
	iFondoGlobal = CenterCore::GetInstance();
	iNotifier = aNotifier;
	iType = (TSearchType)atype;
}
CFileSearchEngine::~CFileSearchEngine()
{
	iFs.Close();
	Cancel();

	if( iThreadRunning )
		iThread.Kill(0);
	iThread.Close();
}
void CFileSearchEngine::ConstructL()
{
	User::LeaveIfError( iFs.Connect() );
	CActiveScheduler::Add(this);
}

// from CActive
/*!
@function DoCancel

@discussion cancel any outstanding operation
*/
void CFileSearchEngine::DoCancel()
{
	iCancelSearch = ETrue;
	if( iThreadRunning )
	{
		iThread.LogonCancel(iStatus);
		iThread.Kill(KErrCancel);
		iThreadRunning = NULL;
	}
}

/*!
@function RunL

@discussion called when operation complete
*/
void CFileSearchEngine::RunL()
{

	if( iThreadRunning )
	{
		iThread.Close();
		iThreadRunning = EFalse;
		iNotifier->SearchComplete(  );

		return;
	}

}

void CFileSearchEngine::StartSearchInThread( )
{
	iThread.Create( _L("FSH"), CFileSearchEngine::FThreadFunction, 0xA000, NULL, (TAny*)this);
	iThreadRunning = ETrue;
	iThread.Logon(iStatus);
	SetActive();
	iThread.Resume();
}

void CFileSearchEngine::CancelSearch()
{
	Cancel();
	if( iThreadRunning )
	{
		iThreadRunning = EFalse;
		iThread.Kill( 0 );
		iThread.Close();
	}
}

TInt CFileSearchEngine::FThreadFunction( TAny* aData ) 
{
	// Create cleanup stack
//	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();


	TInt count = 0;
	CFileSearchEngine* self = STATIC_CAST( CFileSearchEngine*, aData );

	// do search
	TRAPD( r, 

		CActiveScheduler* activeScheduler = new (ELeave) CActiveScheduler;
		CleanupStack::PushL(activeScheduler);
		CActiveScheduler::Install(activeScheduler);
	
		self->DoThreadSearch(); 
		self->WhenExitThreadFunction();

		CleanupStack::PopAndDestroy(activeScheduler);

	);
	
	delete cleanup;
	return 0;
}
void CFileSearchEngine::WhenExitThreadFunction() 
{

}
void CFileSearchEngine::DoThreadSearch() 
{

	TInt count = 0;

	// do search
	iCancelSearch = EFalse;
	if(iFondoGlobal)
	{
		if(iType==ESearchRead)
			iFondoGlobal->UpdataLocalReadList();
		else if(iType==ESearchMp3)
			iFondoGlobal->UpdataLocalMusicList();
		else if(iType==ESearchAll)
			iFondoGlobal->UpdataLocalAllList();
	}
}

