/*
============================================================================
 Name        : CFunSMusicViewSetting from CFunSMusicViewSetting.h
 Author      : funinhand
 Version     :
 Copyright   : Your copyright notice
 Description : CFunSMusicView implementation
============================================================================
*/

// INCLUDE FILES
#include  <aknviewappui.h>
#include  <avkon.hrh>
#include <AknCommonDialogs.h> 
#include  <MyleftCenter.rsg>
#include  "MyleftCenter.hrh"
#include  "MyleftCenterView.h"
#include  "MyleftCenterView2.h"
#include  "MyleftCenterViewReadSet.h"
#include "CenterCore.h"
#include "SinaInterfaceMgr.h"
#include "const.h"
#include <MyleftCenter.mbg>
#include "FunUIStart.h"
#include "MyleftCenterViewRead.h"
#include "portab_symbian.h"
// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CFunSMusicView::ConstructL(const TRect& aRect)
// EPOC two-phased constructor
// ---------------------------------------------------------
//
void CMyleftCenterViewReadSet::ConstructL()
    {
    BaseConstructL( R_MYLEFTCENTER_VIEW_READSET,0 );
    }

// ---------------------------------------------------------
// CFunSMusicView::~CFunSMusicView()
// destructor
// ---------------------------------------------------------
//
CMyleftCenterViewReadSet::~CMyleftCenterViewReadSet()
    {
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        }

    delete iContainer;
    }

// ---------------------------------------------------------
// TUid CFunSMusicView::Id()
//
// ---------------------------------------------------------
//
TUid CMyleftCenterViewReadSet::Id() const
    {
    return KViewReadSetId;
    }

// ---------------------------------------------------------
// CFunSMusicView::HandleCommandL(TInt aCommand)
// takes care of view command handling
// ---------------------------------------------------------
//

void CMyleftCenterViewReadSet::HandleCommandL(TInt aCommand)
    {   
    CenterCore*iplaycore=CenterCore::GetInstance();
    switch ( aCommand )
        {
  		case EAknSoftkeyBack:
		{
			iContainer->SavedAndExitL();
			if(setType==0)
				AppUi()->ActivateLocalViewL(KViewId);
			else if(setType==1)
				AppUi()->ActivateLocalViewL(iplaycore->m_readViewid);
			else if(setType==2)
				AppUi()->ActivateLocalViewL(iplaycore->m_mp3Viewid);
		}
  		break;    
        default:
            {
            AppUi()->HandleCommandL( aCommand );
            break;
            }
        }
    }

// ---------------------------------------------------------
// CFunSMusicView::HandleClientRectChange()
// ---------------------------------------------------------
//
void CMyleftCenterViewReadSet::HandleClientRectChange()
    {
    if ( iContainer )
        {
			iContainer->SetRect( ClientRect());
        }
    }

// ---------------------------------------------------------
// CFunSMusicView::DoActivateL(...)
// 
// ---------------------------------------------------------
//
void CMyleftCenterViewReadSet::DoActivateL(
   const TVwsViewId& /*aPrevViewId*/,TUid /*aCustomMessageId*/,
   const TDesC8& /*aCustomMessage*/)
    {
    if(CenterCore::GetInstance()->m_appUI->UiStyler()!=CenterCore::GetInstance()->m_appUI->iMyStyler2)
    	{
   	 	AppUi()->SetUiStyler(CenterCore::GetInstance()->m_appUI->iMyStyler2);
		AppUi()->TitleBar()->ResetTitleTabMenu();
		AppUi()->TitleBar()->SetTitle(CenterCore::GetInstance()->TextResource(R_TEXT_POPMENU_PARAME_SETTING));//r_text_popmenu_parame_setting
		AppUi()->TitleBar()->SetTextColor(TRgb(254,254,254));
	}
    if (!iContainer)
        {
        iContainer = CMyleftCenterContainerReadSet::NewL(this, ClientRect());
        iContainer->SetMopParent(this);
        AppUi()->AddToStackL( *this, iContainer );
        } 
	CenterCore::GetInstance()->m_mainFouceId=7;
   }

// ---------------------------------------------------------
// CFunSMusicView::DoDeactivate()
// 
// ---------------------------------------------------------
//
void CMyleftCenterViewReadSet::DoDeactivate()
    {
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        }
    
    delete iContainer;
    iContainer = NULL;
    }


//=========== MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CTest4AppView3::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMyleftCenterContainerReadSet* CMyleftCenterContainerReadSet::NewL( CMyleftCenterViewReadSet* aParent,const TRect& aRect )
    {
    CMyleftCenterContainerReadSet* self = CMyleftCenterContainerReadSet::NewLC(aParent, aRect );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CTest4AppView3::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMyleftCenterContainerReadSet* CMyleftCenterContainerReadSet::NewLC(CMyleftCenterViewReadSet* aParent, const TRect& aRect )
    {
    CMyleftCenterContainerReadSet* self = new ( ELeave ) CMyleftCenterContainerReadSet;
    CleanupStack::PushL( self );
    self->ConstructL( aParent,aRect );
    return self;
    }


// ---------------------------------------------------------
// CFunSMusicContainer::ConstructL(const TRect& aRect)
// EPOC two phased constructor
// ---------------------------------------------------------
//
void CMyleftCenterContainerReadSet::ConstructL(CMyleftCenterViewReadSet* aParent,const TRect& aRect)
    {
  //  	CreateWindowL();
	CFunSettingList::ConstructL( aRect, NULL );
	
	iPlayerCore=CenterCore::GetInstance();
	if(!iPlayerCore)
		return ;

	BackDrawer()->SetGradientBackground( CFunBackgroundDrawer::EVerticalOrientation,KRgbWhite, KRgbContainer);
//	BackDrawer()->SetBackgroundColor( TRgb(247,237,217) );
	ActivedBackDrawer()->SetBackgroundColor( KRgbContainerDeep);
	ActivedBackDrawer()->SetNoBorder();
	SetTextColors( TRgb(1,1,1), TRgb(1,1,1),  TRgb(1,1,1));

	HBufC *text1_hbuf = HBufC::NewLC( 128 );
	TPtr text1 = text1_hbuf->Des();
	HBufC *text2_hbuf = HBufC::NewLC( 128 );
	TPtr text2 = text2_hbuf->Des();

	// 1 Download Dir
	iSavedPath.Append(iPlayerCore->GetMainDownloadPath());
	AppendTextSettingItemL(ESetDownloadDir, iPlayerCore->TextResource( R_TEXT_DOWNLOAD_SETTING_MEMORY ),
				iSavedPath);

		// 6 _http_APN
	_http_iap_id=iPlayerCore->_http_iap_id;
	FC_findIapName(_http_iap_id,iHttpIap);
	AppendTextSettingItemL( ESetHttpApn, iPlayerCore->TextResource( R_TEXT_SETTING_HTTP_IAP ),
				iHttpIap);

 	if(aParent->setType==1)
	 {
		// 2 text size
		iFontSize=iPlayerCore->m_readerSetting.textSize;
		CEikonEnv::Static()->ReadResource (text1, R_TEXT_SETTING_FONTSIZE);//r_text_setting_fontsize
		AppendSliderSettingItemL( ESetReadTextSize, iPlayerCore->TextResource( R_TEXT_SETTING_FONTSIZE ),
					text1, _L("100"), _L("400"), iFontSize, 400, 100, 5 );

		// 3 Auto Page
		iAutoRool=iPlayerCore->m_readerSetting.autoRool;
		CEikonEnv::Static()->ReadResource (text1, R_TEXT_SETTING_ON);
		CEikonEnv::Static()->ReadResource (text2, R_TEXT_SETTING_OFF);//r_text_setting_atuorool
		AppendBinarySettingItemL( ESetReadAutoPage, iPlayerCore->TextResource( R_TEXT_SETTING_AUTOROOL ),
					iAutoRool, (TDesC&)text1, (TDesC&)text2 );

		// 4 Auto time
		itime=iPlayerCore->m_readerSetting.autoTime;
		CEikonEnv::Static()->ReadResource (text1, R_TEXT_SETTING_AUTOROOL_TIME);//r_text_setting_atuorool_time
		AppendSliderSettingItemL( ESetReadAutoTime, iPlayerCore->TextResource( R_TEXT_SETTING_AUTOROOL_TIME ),
					text1, _L("2"), _L("60"), itime, 60, 2, 1 );

		// 5 back pic
		iBackPic=iPlayerCore->m_readerSetting.bBcakPic;
		CEikonEnv::Static()->ReadResource (text1, R_TEXT_SETTING_ON);
		CEikonEnv::Static()->ReadResource (text2, R_TEXT_SETTING_OFF);//r_text_setting_backpic
		AppendBinarySettingItemL( ESetReadUseBackpic, iPlayerCore->TextResource( R_TEXT_SETTING_BACKPIC ),
					iBackPic, (TDesC&)text1, (TDesC&)text2 );
		// 6 down pic
		idownReadPic=iPlayerCore->m_readerSetting.downPic;
		CEikonEnv::Static()->ReadResource (text1, R_TEXT_SETTING_ON);
		CEikonEnv::Static()->ReadResource (text2, R_TEXT_SETTING_OFF);//r_text_setting_backpic
		AppendBinarySettingItemL( ESetReadDownPic, iPlayerCore->TextResource( R_TEXT_SETTING_DOWN_READPIC ),//r_text_setting_down_readpic
					idownReadPic, (TDesC&)text1, (TDesC&)text2 );

	 }

	if(aParent->setType==2)
	{
		idownMusicPic=iPlayerCore->m_musicSetting.downPic;
		CEikonEnv::Static()->ReadResource (text1, R_TEXT_SETTING_ON);
		CEikonEnv::Static()->ReadResource (text2, R_TEXT_SETTING_OFF);//r_text_setting_backpic
		AppendBinarySettingItemL( ESetMusicDownPic, iPlayerCore->TextResource( R_TEXT_SETTING_DOWN_MUSICPIC ),//r_text_setting_down_musicpic
					idownMusicPic, (TDesC&)text1, (TDesC&)text2 );
		
		idownMusicLrc=iPlayerCore->m_musicSetting.downLrc;
		CEikonEnv::Static()->ReadResource (text1, R_TEXT_SETTING_ON);
		CEikonEnv::Static()->ReadResource (text2, R_TEXT_SETTING_OFF);//r_text_setting_backpic
		AppendBinarySettingItemL( ESetMusicDownLrc, iPlayerCore->TextResource( R_TEXT_SETTING_DOWN_MUSICLRC ),//r_text_setting_down_musiclrc
					idownMusicLrc, (TDesC&)text1, (TDesC&)text2 );

	}
	CleanupStack::PopAndDestroy( 2 );
	LoadSettings();
	
    SetRect(aRect);;
    ActivateL();


    }

// Destructor
CMyleftCenterContainerReadSet::~CMyleftCenterContainerReadSet()
{
}
void CMyleftCenterContainerReadSet::EditItemL (TInt _index) 
{
	int _ret;
	 if (_index == ESetDownloadDir)
	{

#if defined(_BUILD_7650_)
		// 7650 not support RunSaveDlgNoDirectorySelectionLD, will die
		return;
#else
		// Allow user to select download directory
		iSavedPath.Append(iPlayerCore->GetMainDownloadPath());
		if (AknCommonDialogs::RunMoveDlgLD  (
			iSavedPath, R_MEMORY_SELECTION_DIALOG, R_FILE_SELECTION_DIALOG, this))
		{
			iPlayerCore->SetMainDownloadPath(iSavedPath);
			CFunSettingList::LoadSettings();
		}
#endif
	} 
	if (_index == ESetHttpApn) 
	{
		_http_iap_id=FC_findNextIapName(_http_iap_id,iHttpIap);
		int ret=iPlayerCore->AssureIAP (_http_iap_id);
		FC_Logsp("CMyleftCenterContainerReadSet::EditItemL( )  _http_iap_id=%d,ret=%d",_http_iap_id,ret);	
		iPlayerCore->setIAP(_http_iap_id);
		CFunSettingList::LoadSettings ();
		DrawNow ();
	} 
	else 
	{
		CFunSettingList::EditItemL (_index );
	}
}

TBool CMyleftCenterContainerReadSet::OkToExitL( const TDesC& aDriveAndPath, const TEntry& aEntry ) 
{
	return ETrue;
}

TBool CMyleftCenterContainerReadSet::SavedAndExitL( ) 
{
	if(iPlayerCore->m_appUI->viewReadSet->setType==1)
	{
		iPlayerCore->m_readerSetting.autoTime=itime;
		iPlayerCore->m_readerSetting.autoRool=iAutoRool;
		iPlayerCore->m_readerSetting.textSize=iFontSize;
		iPlayerCore->m_readerSetting.bBcakPic=iBackPic;
		iPlayerCore->m_readerSetting.downPic=idownReadPic;
		
	}
	if(iPlayerCore->m_appUI->viewReadSet->setType==2)
	{
		iPlayerCore->m_musicSetting.downPic=idownMusicPic;
		iPlayerCore->m_musicSetting.downLrc=idownMusicLrc;
	}
	FC_Log("CMyleftCenterContainerReadSet::SavedAndExitL( )  itime=%d,iAutoRool=%d,iFontSize=%d",itime,iAutoRool,iFontSize);
	return ETrue;
}

