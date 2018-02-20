/*
============================================================================
 Name        : CFunVVSearchView from CFunVVSearchView.h
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CFunVVView2 implementation
============================================================================
*/

// INCLUDE FILES
#include  <aknviewappui.h>
#include  <avkon.hrh>
#include  <MyleftCenter.rsg>
#include  "MyleftCenter.hrh"
#include  <eikenv.h>
#include <AknQueryDialog.h> 
#include <StringLoader.h> 
#include <akntitle.h> 

#include  "CenterCore.h"
#include "SinaInterfaceMgr.h"
#include  "MyleftCenterView.h"
#include  "MyleftCenterViewDownLoad.h"
#include  "MyleftCenterContainerDownLoad.h" 
/*
#include  "FunVVPLListView.h"
#include  "FunVVDyListView.h"
#include  "FunVVDownLoadView.h"
#include  "FunVVDownLoadContainer.h" 
#include  "FunVVDownLoadSettingView.h"
*/
// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CFunVVView2::ConstructL(const TRect& aRect)
// EPOC two-phased constructor
// ---------------------------------------------------------
//
void CMyleftCenterViewDownLoad::ConstructL()
{
    	   BaseConstructL( R_VIEW_DOWNLOAD,0 );
/*	FPlayerCore *iplaycore=FPlayerCore::GetInstance();
	iTimer = new (ELeave) WCTimer( this );
	if ( iTimer )
	{
		iTimer->ConstructL();

	}
*/
}

// ---------------------------------------------------------
// CFunVVView2::~CFunVVView2()
// destructor
// ---------------------------------------------------------
//
CMyleftCenterViewDownLoad::~CMyleftCenterViewDownLoad()
    {
    if ( iContainer )
        {
		AppUi()->RemoveFromViewStack( *this, iContainer );
		delete iContainer;
        }
  
    }

// ---------------------------------------------------------
// TUid CFunVVPLListView::Id()
// 
// ---------------------------------------------------------
//
TUid CMyleftCenterViewDownLoad::Id() const
    {
    return KViewDownLoadId;
    }


/*/ ---------------------------------------------------------

// void CFunVVPLListView:: DynInitMenuBarL(TInt aResourceId, CEikMenuBar* aMenuBar)

//Dynamically initialises a menu bars.

//Parameters
	TInt aResourceId Resource ID identifying the menu bar to initialise.
	CEikMenuBar* aMenuBar The in-memory representation of the menu bar.
/ ---------------------------------------------------------*/


 void CMyleftCenterViewDownLoad:: DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane)
{
/*
	TInt index=iContainer->iPlayListbox->CurrentItemIndex();
	 CAknView::DynInitMenuPaneL(aResourceId,aMenuPane);
	if(aResourceId==R_FUNVV_VIEW_DOWNLOAD_MENU)
	{
		if(iContainer->is_downloadView)
		{

			aMenuPane->SetItemDimmed(EFunSMusicCmdDownLoad, ETrue);
			aMenuPane->SetItemDimmed(EFunSMusicCmdDownLoadStop, ETrue);
			aMenuPane->SetItemDimmed(EFunSMusicCmdDownLoadAllDown, ETrue);
			aMenuPane->SetItemDimmed(EFunSMusicCmdDownLoadAllStop, ETrue);
			aMenuPane->SetItemDimmed(EFunSMusicCmdDownLoadNew, ETrue);
			aMenuPane->SetItemDimmed(EFunSMusicCmdDownLoadLoaded, ETrue);
			aMenuPane->SetItemDimmed(EFunSMusicCmdDownLoadDelete, ETrue);
			aMenuPane->SetItemDimmed(EFunSMusicCmdDownLoadMoveTo, ETrue);
			aMenuPane->SetItemDimmed(EFunSMusicCmdDownLoadSetting, ETrue);
			return;
		}
		
		if(index==0 )
		{
			aMenuPane->SetItemDimmed(EFunSMusicCmdDownLoad, ETrue);
			aMenuPane->SetItemDimmed(EFunSMusicCmdDownLoadStop, ETrue);
			aMenuPane->SetItemDimmed(EFunSMusicCmdDownLoadDelete, ETrue);
			aMenuPane->SetItemDimmed(EFunSMusicCmdDownLoadMoveTo, ETrue);
			aMenuPane->SetItemDimmed(EFunSMusicCmdDownLoadeddelete, ETrue);
			aMenuPane->SetItemDimmed(EFunSMusicCmdDownLoadclear, ETrue);
			aMenuPane->SetItemDimmed(EFunSMusicCmdDownLoadRedown, ETrue);
			aMenuPane->SetItemDimmed(EFunSMusicCmdDownLoadAddToplist, ETrue);

		}
		else
		{
			FPlayerCore *iplaycore=FPlayerCore::GetInstance();
			int curRun=iplaycore->dl_getDownCurRun();
			
			if(index-1==curRun)  //is in Run
			{
				aMenuPane->SetItemDimmed(EFunSMusicCmdDownLoadOpen, ETrue);
				aMenuPane->SetItemDimmed(EFunSMusicCmdDownLoad, ETrue);
				aMenuPane->SetItemDimmed(EFunSMusicCmdDownLoadeddelete, ETrue);
				aMenuPane->SetItemDimmed(EFunSMusicCmdDownLoadclear, ETrue);
				aMenuPane->SetItemDimmed(EFunSMusicCmdDownLoadRedown, ETrue);
				aMenuPane->SetItemDimmed(EFunSMusicCmdDownLoadAddToplist, ETrue);

			}
			else		// is stop
			{
				aMenuPane->SetItemDimmed(EFunSMusicCmdDownLoadOpen, ETrue);
				aMenuPane->SetItemDimmed(EFunSMusicCmdDownLoadStop, ETrue);
				aMenuPane->SetItemDimmed(EFunSMusicCmdDownLoadeddelete, ETrue);
				aMenuPane->SetItemDimmed(EFunSMusicCmdDownLoadclear, ETrue);
				aMenuPane->SetItemDimmed(EFunSMusicCmdDownLoadRedown, ETrue);
				aMenuPane->SetItemDimmed(EFunSMusicCmdDownLoadAddToplist, ETrue);
			}
		}
	}
*/
 }

// ---------------------------------------------------------
// CFunVVView2::HandleCommandL(TInt aCommand)
// takes care of view command handling
// ---------------------------------------------------------
//
void CMyleftCenterViewDownLoad::HandleCommandL(TInt aCommand)
{   
	CenterCore*iplaycore=CenterCore::GetInstance();
	TInt index=iContainer->iDownloadList->GetCurrentSelectIndex();		

    switch ( aCommand )
	{
		case EAknSoftkeyBack:
		{
			AppUi()->ActivateLocalViewL(KViewId);
		}	
		break;
		case ECmdDownLoadOpen:
		{/*
			if(!iContainer->is_downloadView )
			{
				if(index==0)
				{
					iContainer->is_downloadView=ETrue;
					iContainer->iDownloadList->SetCurrentSelectIndex(0);
					iContainer->UpdataL();	
				}
			}
			else
			{
			 	song tempSong;
				int appendId=0;
				TBuf<128> itemStatus;
				iplaycore->dl_getDownListItem2Path(index, tempSong.iaskurl);
				iplaycore->dl_getDownListItem(index,tempSong.name,itemStatus);
				
				appendId=iplaycore->myAVAddItemToPlayer(tempSong.name, tempSong.iaskurl);

				iplaycore->playerSetCurIndex(appendId);
				iplaycore->Music_playSong(&tempSong);*/
			}
		break;
		case ECmdDownLoad:
		{
			iplaycore->Download_SartDownload(index);

		}
		break;
		case ECmdDownLoadStop:
		{
			if(iplaycore->m_curDownId==index)
			{
				iplaycore->iDownMgr->CancelFetchDownload();
				iplaycore->Download_ListSetStatus(iplaycore->m_curDownId, EdownloadReady);
				iplaycore->m_curDownId=-1;
				iContainer->UpdataL();		
			}
		}
		break;
		case ECmdDownLoadDeleteItem:
		{
			iplaycore->Download_ListDelete(index);
			iContainer->UpdataL();		
		}
		break;
		case ECmdDownLoadDeleteItemAndFile:
		{/*
			iplaycore->dl_stopDownList();
			*/
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
// CFunVVView2::HandleClientRectChange()
// ---------------------------------------------------------
//
void CMyleftCenterViewDownLoad::HandleClientRectChange()
    {
    if ( iContainer )
        {
        iContainer->SetRect( ClientRect() );
        }
    }

// ---------------------------------------------------------
// CFunVVView2::DoActivateL(...)
// 
// ---------------------------------------------------------
//
void CMyleftCenterViewDownLoad::DoActivateL(
   const TVwsViewId& /*aPrevViewId*/,TUid /*aCustomMessageId*/,
   const TDesC8& /*aCustomMessage*/)
    {
	AppUi()->SetUiStyler(CenterCore::GetInstance()->m_appUI->iMyStyler2);
	AppUi()->TitleBar()->ResetTitleTabMenu();
	AppUi()->TitleBar()->SetTitle(CenterCore::GetInstance()->TextResource(R_TEXT_POPMENU_DOWNLOAD_MANAGER));
	AppUi()->TitleBar()->SetTextColor(TRgb(254,254,254));

    if (!iContainer)
        {
        iContainer = CMyleftCenterContainerDownLoad::NewL( ClientRect());
        iContainer->SetMopParent(this);
        AppUi()->AddToStackL( *this, iContainer );
        }
		CenterCore::GetInstance()->m_mainFouceId=5;
   }

// ---------------------------------------------------------
// CFunVVView2::DoDeactivate()
// 
// ---------------------------------------------------------
//
void CMyleftCenterViewDownLoad::DoDeactivate()
{
	FC_Log("CMyleftCenterViewDownLoad::DoDeactivate()");
//    if(iTimer)
//		iTimer->Stop();

	if ( iContainer )
    {
		AppUi()->RemoveFromViewStack( *this, iContainer );
		delete iContainer;
		iContainer = NULL;
    }
}

