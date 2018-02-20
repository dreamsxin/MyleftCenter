/*
============================================================================
 Name        : CMyleftCenterView2 from MyleftCenterView2.h
 Author      : yilizheng
 Version     :
 Copyright   : MyleftCenter
 Description : CMyleftCenterView2 implementation
============================================================================
*/

// INCLUDE FILES
#include  <aknviewappui.h>
#include  <avkon.hrh>

#include  <MyleftCenter.rsg>
#include  <MyleftCenter.mbg>
#include  "MyleftCenter.hrh"
#include  "MyleftCenterView.h"
#include  "MyleftCenterViewChatRoom.h"
#include  "MyleftCenterViewReadList.h"
#include  "MyleftCenterViewload.h"
#include  "MyleftCenterView.h"
#include  "MyleftCenterViewReadSet.h"
#include  "MyleftCenterViewMusic.h"
#include  "MyleftCenterViewMusicList.h"
#include  "MyleftCenterViewMusicSearch.h"
#include  "MyleftCenterViewFileMgr.h"
#include  "MyleftCenterViewDownload.h"
#include  "MyleftCenterViewBrCtl.h"
#include  "MyleftCenterContainerChatRoom.h" 
#include  "MyleftCenterView2.h"
#include  "MyleftCenterContainer2.h" 
#include  "CenterCore.h"
#include  "const.h"
// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMyleftCenterView2::ConstructL(const TRect& aRect)
// EPOC two-phased constructor
// ---------------------------------------------------------
//
void CMyleftCenterViewChatRoom::ConstructL()
{
	BaseConstructL( R_MYLEFTCENTER_VIEW_CHATROOM,0 );
	iText.SetLength(0);
}

// ---------------------------------------------------------
// CMyleftCenterView2::~CMyleftCenterView2()
// destructor
// ---------------------------------------------------------
//
CMyleftCenterViewChatRoom::~CMyleftCenterViewChatRoom()
    {
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        }

    delete iContainer;
    }

// ---------------------------------------------------------
// TUid CMyleftCenterView2::Id()
// 
// ---------------------------------------------------------
//
TUid CMyleftCenterViewChatRoom::Id() const
    {
    return KViewChatRoom;
    }

 void CMyleftCenterViewChatRoom:: DynInitMenuPaneL(TInt aResourceId, CFunMenuPane* aMenuPane)
{
	CFunMenuPane* menuPane = aMenuPane;
	if( menuPane && (aResourceId==R_MYLEFTCENTER_VIEWTO_SUBPANE))
	{
		//	menuPane->SetItemVisible(EMyleftCenterCmdAppToViewRead,0);	
		//	menuPane->SetItemVisible(ECmdPopParameSetting,0);		
	}
}

// ---------------------------------------------------------
// CMyleftCenterView2::HandleCommandL(TInt aCommand)
// takes care of view command handling
// ---------------------------------------------------------
//
void CMyleftCenterViewChatRoom::HandleCommandL(TInt aCommand)
    {  
    switch ( aCommand )
        {
        case EAknSoftkeyBack:
            {
			CenterCore* iPlayerCore =   CenterCore::GetInstance();
			
			iPlayerCore->m_appUI->ActivateLocalViewL(KViewId); 
            }
	 break;
/*	case EMyleftCenterCmdViewReadPageto:
		{
			iContainer->m_drewType=EDrewTypePagebar;
			iContainer->TimerStop();
			iContainer->GetStepPos(iContainer->m_bPos);
			iContainer->DrawNow();
		}
	 	break;
	case EMyleftCenterCmdViewReadSetColor:
		{
			iContainer->m_drewType=EDrewTypeColorSet;
			iContainer->TimerStop();
			iContainer->DrawNow();
		}
		break;
	case EMyleftCenterCmdViewReadSetPic:
		{
			iContainer->m_drewType=EDrewTypePicSel;
			TBuf<256> filename;
			
			TInt ret=CenterCore::GetBackPic(0, filename);
			if(ret)
			{
				iContainer->iTempBackImg=FunUiLib::LoadImageFromFileL(filename);
				iContainer->TimerStop();
				iContainer->DrawNow();
			}
			else
				iContainer->m_drewType=EDrewTypeText;
		}
		break;
		
	case EMyleftCenterCmdViewReadEdit:
		{
		}
	 	break;
	case EMyleftCenterCmdViewReadSetting:
		{
			CenterCore::GetInstance()->m_appUI->viewReadSet->setType=1;
			CenterCore::GetInstance()->m_appUI->ActivateLocalViewL(KViewReadSetId); 
		}
	 	break;
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
		CenterCore::GetInstance()->m_appUI->ActivateLocalViewL(KViewReadId); 
	}
	 break;
	case EMyleftCenterCmdAppToViewLisiten:
	{
		AppUi()->TitleBar()->ResetTitleTabMenu();
		AppUi()->TitleBar()->AddTitleTabMenu(KViewMusicId,	KMusicMultiBitmapFilename,EMbmMyleftcenterTab1_sel,EMbmMyleftcenterTab1);
		AppUi()->TitleBar()->AddTitleTabMenu(KViewMusicListId,KMusicMultiBitmapFilename,EMbmMyleftcenterTab2_sel,EMbmMyleftcenterTab2);
		AppUi()->TitleBar()->AddTitleTabMenu(KViewMusicSearchId,	KMusicMultiBitmapFilename,EMbmMyleftcenterTab4_sel,EMbmMyleftcenterTab4);

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
		CenterCore::GetInstance()->m_appUI->viewReadSet->setType=1;
		CenterCore::GetInstance()->m_appUI->ActivateLocalViewL(KViewReadSetId); 
	}
	 break;
*/
	default:
            {
            AppUi()->HandleCommandL( aCommand );
            break;
            }
        }
    }

// ---------------------------------------------------------
// CMyleftCenterView2::HandleClientRectChange()
// ---------------------------------------------------------
//
void CMyleftCenterViewChatRoom::HandleClientRectChange()
    {
    if ( iContainer )
        {
        iContainer->SetRect( AppUi()->ApplicationRect());
        }
    }

// ---------------------------------------------------------
// CMyleftCenterView2::DoActivateL(...)
// 
// ---------------------------------------------------------
//
void CMyleftCenterViewChatRoom::DoActivateL(
   const TVwsViewId& /*aPrevViewId*/,TUid /*aCustomMessageId*/,
   const TDesC8& /*aCustomMessage*/)
    {
    if (!iContainer)
        {
        iContainer = new (ELeave) CMyleftCenterContainerChatRoom;
        iContainer->ConstructL( this,AppUi()->ApplicationRect());
        AppUi()->AddToStackL( *this, iContainer );
        }
		
   }

// ---------------------------------------------------------
// CMyleftCenterView2::DoDeactivate()
// 
// ---------------------------------------------------------
//
void CMyleftCenterViewChatRoom::DoDeactivate()
{
	CenterCore* iPlayerCore =   CenterCore::GetInstance();
	if ( iContainer )
	{
		AppUi()->RemoveFromViewStack( *this, iContainer );
		delete iContainer;

	}
	iContainer = NULL;
}



