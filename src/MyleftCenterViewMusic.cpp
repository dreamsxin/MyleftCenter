/*
============================================================================
 Name        : CFunSMusicView from FunSMusicView.h
 Author      : funinhand
 Version     :
 Copyright   : Your copyright notice
 Description : CFunSMusicView implementation
============================================================================
*/

// INCLUDE FILES
#include  <aknviewappui.h>
#include  <avkon.hrh>
#include  <MyleftCenter.rsg>
#include <MyleftCenter.mbg>
#include  <UTF.H>
#include  <f32file.h>
#include  "MyleftCenter.hrh"
#include  "const.h"
#include "FunUIStart.h"
#include "CenterCore.h"
#include "SinaInterfaceMgr.h"
#include "LrcSubtitle.h"
#include  "MyleftCenterView.h"
#include  "MyleftCenterView2.h"
#include  "MyleftCenterViewRead.h"
#include  "MyleftCenterViewReadSet.h"
#include  "MyleftCenterViewReadList.h"
#include  "MyleftCenterViewSearch.h"
#include  "MyleftCenterViewMusic.h"
#include  "MyleftCenterViewFileMgr.h"
#include  "MyleftCenterViewDownload.h"
#include  "MyleftCenterViewBrCtl.h"
#include  "MyleftCenterContainerMusic.h" 
#include  "MyleftCenterViewMusicList.h"
#include  "MyleftCenterViewMusicSearch.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CFunSMusicView::ConstructL(const TRect& aRect)
// EPOC two-phased constructor
// ---------------------------------------------------------
//
void CMyleftCenterViewMusic::ConstructL()
    {
    BaseConstructL( R_VIEW_MUSIC,0 );
	iShowMode=0;
	iCurSelIndex=0;
    }

// ---------------------------------------------------------
// CFunSMusicView::~CFunSMusicView()
// destructor
// ---------------------------------------------------------
//
CMyleftCenterViewMusic::~CMyleftCenterViewMusic()
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
TUid CMyleftCenterViewMusic::Id() const
    {
    return KViewMusicId;
    }

 void CMyleftCenterViewMusic:: DynInitMenuPaneL(TInt aResourceId, CFunMenuPane* aMenuPane)
{
	CFunMenuPane* menuPane = aMenuPane;
	CenterCore*iplaycore=CenterCore::GetInstance();
	if(aResourceId==R_MYLEFTCENTER_VIEWTO_SUBPANE)
	{
		menuPane->SetItemVisible(EMyleftCenterCmdAppToViewLisiten,0);		
		menuPane->SetItemVisible(ECmdPopParameSetting,0);		
	}
	if(aResourceId==R_POPMENU_PLAY_MODE) //r_popmenu_play_mode
	{
		menuPane->SetItemChecked(ECmdPopPlayModeSunxu,0);
		menuPane->SetItemChecked(ECmdPopPlayModeXunhuan,0);
		menuPane->SetItemChecked(ECmdPopPlayModeSuiji,0);
		if(iplaycore->m_musicSetting.playMode==0)
		{
			menuPane->SetItemChecked(ECmdPopPlayModeSunxu,1);
		}
		else if(iplaycore->m_musicSetting.playMode==1)
		{
			menuPane->SetItemChecked(ECmdPopPlayModeXunhuan,1);
		}
		else
		{
			menuPane->SetItemChecked(ECmdPopPlayModeSuiji,1);
		}

	}
}

// ---------------------------------------------------------
// CFunSMusicView::HandleCommandL(TInt aCommand)
// takes care of view command handling
// ---------------------------------------------------------
//
#ifdef __WINS__
const char funfiles[]="c:\\data\\play.fun";
const char mp3files[]="c:\\data\\byebye.mp3";
const char fsxfiles[]="c:\\data\\test.fsx";
const char lrcfiles[]="c:\\data\\3260.mp3";
#else
const char funfiles[]="e:\\play.fun";
const char mp3files[]="e:\\byebye.mp3";
const char fsxfiles[]="e:\\test.fsx";
const char lrcfiles[]="e:\\3260.lrc";
#endif

void CMyleftCenterViewMusic::HandleCommandL(TInt aCommand)
{  
    CenterCore*iplaycore=CenterCore::GetInstance();
    switch ( aCommand )
        {
       
        case ECmdPopMusicPlayControlPlay:
            {
		if(iplaycore->m_musicStatus==CCCB_STATUS_PLAY)
			iplaycore->Music_playerPause();
		else if(iplaycore->m_musicStatus==CCCB_STATUS_PAUSE)
			iplaycore->Music_playerPlay();
	//	else if(iplaycore->m_musicStatus==CCCB_STATUS_WAIT)
	//		iplaycore->playerDoPlayMsg();

            }
		break;
        case ECmdPopMusicPlayControlStop:
            {
			iplaycore->Music_playerStop();
            }
		break;
	case ECmdPopMusicPlayControlPrev:
            {
			song tempsong;			
			int itemCount=iplaycore->m_musicMasklist->Count();
			int curId=iplaycore->Music_playerCurrentIndex();
			if(curId==0)
				curId=itemCount-1;
			else
				curId--;
			iplaycore->Music_GetPlayerListSong(&tempsong, curId);
			iplaycore->Music_playerSetCurIndex(curId);
			ResetAll();			
			iplaycore->Music_playSong(&tempsong);
			iContainer->iCurretPlayIndex=curId;


            }
		break;
	case ECmdPopMusicPlayControlNext:
            {
			song tempsong;
			int itemCount=iplaycore->m_musicMasklist->Count();
			int curId=iplaycore->Music_playerCurrentIndex();
			if(curId==itemCount-1)
				curId=0;
			else
				curId++;
			iplaycore->Music_GetPlayerListSong(&tempsong, curId);
			iplaycore->Music_playerSetCurIndex(curId);
			ResetAll();
			iplaycore->Music_playSong(&tempsong);
			iContainer->iCurretPlayIndex=curId;

            }
		break;
	case ECmdPopMusicPlayControlUpVolume:
            {
			int volume=iplaycore->m_musicSetting.volume;
			iplaycore->Music_SetVolume(++volume);
            }
		break;
	case ECmdPopMusicPlayControlDownVolume:
            {
			int volume=iplaycore->m_musicSetting.volume;
			iplaycore->Music_SetVolume(--volume);

            }
		break;
	case ECmdPopMusicPlayControlDelete:
            {
			if(iContainer->iShowMode==ShowModeLRC)
				break;
			int cur=iContainer->GetCurListIndex();
			if(iplaycore->m_musicStatus && cur==iplaycore->m_iCurretPlayIndex)
				return;
			iplaycore->Music_MaskListDelete(cur);
			if(iplaycore->m_musicStatus && (iplaycore->m_iCurretPlayIndex!=-1))
			{
				if(cur<iplaycore->m_iCurretPlayIndex)
				{
					iplaycore->m_iCurretPlayIndex--;
				}

			}
			iContainer->UpdataAll();
            }
		break;
	case ECmdPopMusicPlayControlQuick:
            {
			_LIT(KString,"%d:%02d");
				
			TBuf<16> manLib;	
			unsigned long durtime=iplaycore->m_duration;
			int durmin=durtime/60;
			int dursec=durtime%60;
			TInt value=iplaycore->Music_GetCurTime();
			int step=5;
			manLib.Format(KString(),durmin,dursec);	
			if(durmin>1)
				step=10;

		CFunSliderDialog* dialog = new (ELeave) CFunSliderDialog();
		dialog->ConstructL(iplaycore->TextResource(R_TEXT_POPMENU_QUICK), _L("0:00"), manLib, value, 
						iplaycore->m_appUI->UiStyler(),NULL,(TInt)durtime,0, step);
		dialog->SetSliderObserver( this );
		if( dialog->ExecuteLD() )
		{
			if(iplaycore->m_musicStatus)
			{
			//	iplaycore->playerStop();
				iplaycore->Music_Seek((unsigned long)value);
				
			}

		}
		iContainer->showActiveID=0;
            }
		break;
	case ECmdPopMusicPlayControlLrcBefare:
            {
			if(iplaycore->m_subtitle)
				iplaycore->m_subtitle->SetOffset(-5000);
            }
		break;
	case ECmdPopMusicPlayControlLrcLater:
            {
			if(iplaycore->m_subtitle)
				iplaycore->m_subtitle->SetOffset(5000);
            }
		break;
	case ECmdPopPlayModeSunxu:
            {
			iplaycore->Music_playerSetMode(0);
            }
		break;
	case ECmdPopPlayModeXunhuan:
            {
			iplaycore->Music_playerSetMode(1);
            }
		break;
	case ECmdPopPlayModeSuiji:
            {
			iplaycore->Music_playerSetMode(2);
            }
		break;
	case ECommandViewToLrcOrList:
            {
/*		if(iplaycore->Music_GetSubtitle())
		{
			ChangeShowMode(ShowModeLRC);
		}
		else
		{
			TParse p;
			TBuf<128> singer;
			TBuf<128> songTitle;
			TBuf<128> searchTitle;
			TBuf<128> songname;
			TBuf<128> desPath;
			desPath.SetLength(0);
			singer.Copy(iplaycore->curPlayer->singer);
			songname.Copy(iplaycore->curPlayer->name);
			if(iplaycore->curPlayer->iaskurl.Find(_L("http:://"))==KErrNotFound)
			{
				p.Set(iplaycore->curPlayer->iaskurl,NULL,NULL);
				desPath=p.DriveAndPath();
			}
			songTitle=iplaycore->TextResource(R_TEXT_VIEW_SONG_NAME);
			searchTitle=iplaycore->TextResource(R_TEXT_VIEW_SEARCH_KEY);
			TInt ret=CFunInputDialog::Run2lineDlgLD(searchTitle,songTitle,songname,iplaycore->TextResource(R_TEXT_VIEW_SINGER_NAME),singer,iplaycore->m_appUI->UiStyler());
			if(ret)
			{
				iplaycore->iSinaMgr->LaunchPlayingLrcUrlManual(singer, songname, KNullDesC,desPath);
				idesPath.Copy(desPath);
			}
		}*/
            }
			if((iShowMode==ShowModeLRC))
				ChangeShowMode(ShowModeList);
			else
				ChangeShowMode(ShowModeLRC);
		break;		
/*	case EFunSMusicCmdAppPayFee:
            {
			CFunControlBar*  iControlBar=ControlBar();
			iControlBar->SetCommand(CFunControlBar::ERightCommand, EFunSMusicCommandListTo ,iplaycore->TextResource(R_TEXT_VIEW_SONG));
			iControlBar->SetCommand(CFunControlBar::ELeftCommand,  EAknSoftkeyOptions ,iplaycore->TextResource(R_TEXT_VIEW_SELECT));
			
#ifndef __WINS__
#ifdef _SINA_PAY_NOT_FREE_
			if(iplaycore->PayFeeConstructPayFee(ETrue))
			{
#endif
				int index=iContainer->GetCurListIndex();
			//	FC_Logsp("EFunSMusicCmdAppPayFee index=%d",index);
				song * list=iplaycore->m_lrclist->At(index);
				if(list)
				{
					TBuf<128>  filemane;
					int iCurPlayId=iplaycore->m_iCurretPlayIndex;
					iplaycore->getMyAVItemObjItem(Sina_Playerlist_Name,iCurPlayId,filemane);
			#ifdef _SINA_PAY_NOT_FREE_		
					iplaycore->m_appUI->AutoStartSmsDaemon();
					iplaycore->iPayFeeMgr->GetLatestPayFeeImpl()->Start(list->sinaId);
			#endif
					if(idesPath.Length())
					{
						iplaycore->iSinaMgr->SetDesPath(idesPath);
						idesPath.SetLength(0);
					}
					iplaycore->iSinaMgr->LaunchGetPlayerlrc(list->iaskurl,filemane);		
				}
#ifdef _SINA_PAY_NOT_FREE_
			}
#endif		
#else
			int index=iContainer->GetCurListIndex();
			song * list=iplaycore->m_lrclist->At(index);

			if(idesPath.Length())
			{
				iplaycore->iSinaMgr->SetDesPath(idesPath);
				idesPath.SetLength(0);
			}
			iplaycore->iSinaMgr->LaunchGetPlayerlrc(list->iaskurl,list->name);
#endif
			iShowMode=ShowModeList;
			iContainer->TimerStop();
			iContainer->iShowMode=ShowModeList;
			iContainer->iSelIndex=iplaycore->m_iCurretPlayIndex;
			iControlBar->DrawNow();
			iContainer->UpdataAll();

            }
		break;		
	case EFunSMusicCmdAppNotPayFee:
            {
			SetLrcLibleShowId(ELrcLibShow_NULL);
			CFunControlBar*  iControlBar=ControlBar();
			iControlBar->SetCommand(CFunControlBar::ERightCommand, EFunSMusicCommandListTo ,iplaycore->TextResource(R_TEXT_VIEW_SONG));
			iControlBar->SetCommand(CFunControlBar::ELeftCommand,  EAknSoftkeyOptions ,iplaycore->TextResource(R_TEXT_VIEW_SELECT));
			iShowMode=ShowModeList;
			iContainer->iShowMode=ShowModeList;
			iContainer->iSelIndex=iplaycore->m_iCurretPlayIndex;
			iControlBar->DrawNow();
			iContainer->UpdataAll();
            }
		break;		*/
	case EMyleftCenterCmdAppToMenu:
		{
			CenterCore::GetInstance()->m_appUI->ActivateLocalViewL(KViewId); 
		}
	 break;
	case EMyleftCenterCmdAppToViewWap:
	{
		CenterCore::GetInstance()->m_appUI->ActivateLocalViewL(KViewBrCtlId); 
	}
	 break;
	case EMyleftCenterCmdAppToViewRead:
	{
		AppUi()->TitleBar()->ResetTitleTabMenu();
		AppUi()->TitleBar()->AddTitleTabMenu(KView2Id,	KMusicMultiBitmapFilename,EMbmMyleftcenterTab1_sel,EMbmMyleftcenterTab1);
		AppUi()->TitleBar()->AddTitleTabMenu(KViewReadListId,KMusicMultiBitmapFilename,EMbmMyleftcenterTab2_sel,EMbmMyleftcenterTab2);
		AppUi()->TitleBar()->AddTitleTabMenu(KViewSearchId,	KMusicMultiBitmapFilename,EMbmMyleftcenterTab4_sel,EMbmMyleftcenterTab4);

		if(CenterCore::GetInstance()->m_readStatus)
			CenterCore::GetInstance()->m_appUI->ActivateLocalViewL(KViewReadId); 
		else
			CenterCore::GetInstance()->m_appUI->ActivateLocalViewL(KView2Id); 
	}
	 break;
	case EMyleftCenterCmdAppToViewLisiten:
	{
		CenterCore::GetInstance()->m_appUI->ActivateLocalViewL(KViewMusicId); 
	}
	 break;
	case EMyleftCenterCmdAppToViewDownload:
	{
		CenterCore::GetInstance()->m_appUI->ActivateLocalViewL(KViewDownLoadId); 
	}
	 break;
	case ECmdPopParameSetting:
	{
		CenterCore::GetInstance()->m_appUI->viewReadSet->setType=2;
		CenterCore::GetInstance()->m_appUI->ActivateLocalViewL(KViewReadSetId); 
	}
	 break;
        case EAknSoftkeyBack:
            {
			CenterCore::GetInstance()->m_appUI->ActivateLocalViewL(KViewId);
            break;
            }
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
void CMyleftCenterViewMusic::HandleClientRectChange()
    {
    if ( iContainer )
        {
		iContainer->SetRect( ClientRect() );
        }
    }


// ---------------------------------------------------------
// CFunSMusicView::DoActivateL(...)
// 
// ---------------------------------------------------------
//
void CMyleftCenterViewMusic::DoActivateL(
   const TVwsViewId& /*aPrevViewId*/,TUid /*aCustomMessageId*/,
   const TDesC8& /*aCustomMessage*/)
    {
     if(CenterCore::GetInstance()->m_appUI->UiStyler()!=CenterCore::GetInstance()->m_appUI->iMyStyler2)
 		AppUi()->SetUiStyler(CenterCore::GetInstance()->m_appUI->iMyStyler2);
	
    if (!iContainer)
        {
        iContainer = CMyleftCenterContainerMusic::NewL(ClientRect());
        iContainer->SetMopParent(this);
        AppUi()->AddToStackL( *this, iContainer );
        } 
		CenterCore::GetInstance()->m_mainFouceId=2;
		CenterCore::GetInstance()->m_mp3Viewid=KViewMusicId;
   }

// ---------------------------------------------------------
// CFunSMusicView::DoDeactivate()
// 
// ---------------------------------------------------------
//
void CMyleftCenterViewMusic::DoDeactivate()
    {
    if ( iContainer )
        {
        iShowMode=iContainer->iShowMode;
	iLrcShowId=iContainer->iLrcShowId;
        iCurSelIndex=iContainer->GetCurListIndex();
        AppUi()->RemoveFromViewStack( *this, iContainer );
        }
    
    delete iContainer;
    iContainer = NULL;
    }

void CMyleftCenterViewMusic::ChangeShowMode(TInt _aShowMode)  	//0=List;1=lrc,2= rearch result
{
 	 CenterCore*iplaycore=CenterCore::GetInstance();
	
	iShowMode=_aShowMode;
	if(iContainer)
	{
		iContainer->TimerStop();
		iContainer->iShowMode=_aShowMode;
		iContainer->UpdataAll();
	}


}

void CMyleftCenterViewMusic::SetLrcLibleShowId(TInt _lrcLibShowId)
{
	FC_Log("SetLrcLibleShowId is %d",_lrcLibShowId);
	iLrcShowId=_lrcLibShowId;
	if(iContainer)
		iContainer->SetLrcLibleShowId(_lrcLibShowId);
}

void CMyleftCenterViewMusic::ResetAll()
{
	CenterCore*iplaycore=CenterCore::GetInstance();
	CFunControlBar*  iControlBar=ControlBar();
	
	iLrcShowId=ELrcLibShow_NULL;
	iShowMode=ShowModeList;
	
//	iControlBar->SetCommand(CFunControlBar::ERightCommand, EFunSMusicCommandListTo ,iplaycore->TextResource(R_TEXT_VIEW_SONG));
//	iControlBar->SetCommand(CFunControlBar::ELeftCommand,  EAknSoftkeyOptions ,iplaycore->TextResource(R_TEXT_VIEW_SELECT));

	if(iContainer)
	{
		iContainer->iShowMode=iShowMode;		
		iContainer->iLrcShowId=iLrcShowId;

		iControlBar->DrawNow();
		iContainer->UpdataAll();
	}

	TInt taskID=iplaycore->iSinaMgr->IsFetchingType();
	if(taskID==8)  //get urel list auto
	{
		iplaycore->iSinaMgr->CancelFetchMusicInfo();
	}
}

void CMyleftCenterViewMusic::MsoGetCurrentValue( TInt aValue, TDes& aText ) 
{
	_LIT(KString,"%d:%02d");
	aText.Format(KString(),aValue/60,aValue%60);	
}
