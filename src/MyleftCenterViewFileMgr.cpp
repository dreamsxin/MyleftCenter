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

#include  "centercore.h"
#include  "MyleftCenterView.h"
#include  "MyleftCenterViewFileMgr.h"
#include  "MyleftCenterContainerFileMgr.h" 
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
void CMyleftCenterViewFileMgr::ConstructL()
{
    	   BaseConstructL( R_MYLEFTCENTER_VIEW_FILEMGR,0 );  //r_myleftcenter_view_filemgr
 }

// ---------------------------------------------------------
// CFunVVView2::~CFunVVView2()
// destructor
// ---------------------------------------------------------
//
CMyleftCenterViewFileMgr::~CMyleftCenterViewFileMgr()
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
TUid CMyleftCenterViewFileMgr::Id() const
    {
    return KViewFileMgrId;
    }


/*/ ---------------------------------------------------------

// void CFunVVPLListView:: DynInitMenuBarL(TInt aResourceId, CEikMenuBar* aMenuBar)

//Dynamically initialises a menu bars.

//Parameters
	TInt aResourceId Resource ID identifying the menu bar to initialise.
	CEikMenuBar* aMenuBar The in-memory representation of the menu bar.
/ ---------------------------------------------------------*/


 void CMyleftCenterViewFileMgr:: DynInitMenuPaneL(TInt aResourceId, CFunMenuPane* aMenuPane)
{
	CFunMenuPane* menuPane = aMenuPane;
	if( menuPane )
	{
		if(iContainer->doPath.Length()==0 )
		{
			menuPane->SetItemVisible(EFunSMusicCmdViewFilemgrEditPaster,0);
		}
		else
		{
			menuPane->SetItemVisible(EFunSMusicCmdViewFilemgrEditPaster,1);
		}
		
		menuPane->SetItemVisible(EFunSMusicCmdViewFilemgrEditCut,1);
		menuPane->SetItemVisible(EFunSMusicCmdViewFilemgrEditCopy,1);
		menuPane->SetItemVisible(EFunSMusicCmdViewFilemgrEditDelete,1);
		menuPane->SetItemVisible(EFunSMusicCmdViewFilemgrEditRename,1);
		
		if(fmPath.Length()==0)
		{
			menuPane->SetItemVisible(EFunSMusicCmdViewFilemgrEdit,0);
		}
		else
		{
			menuPane->SetItemVisible(EFunSMusicCmdViewFilemgrEdit,1);
			if( iContainer->iFileMgrList->GetCurrentSelectIndex()==0)
			{
				menuPane->SetItemVisible(EFunSMusicCmdViewFilemgrEditCut,0);
				menuPane->SetItemVisible(EFunSMusicCmdViewFilemgrEditCopy,0);
				menuPane->SetItemVisible(EFunSMusicCmdViewFilemgrEditDelete,0);
				menuPane->SetItemVisible(EFunSMusicCmdViewFilemgrEditRename,0);
			}
		}
	}

}

// ---------------------------------------------------------
// CFunVVView2::HandleCommandL(TInt aCommand)
// takes care of view command handling
// ---------------------------------------------------------
//
void CMyleftCenterViewFileMgr::HandleCommandL(TInt aCommand)
{   
	CenterCore*iplaycore=CenterCore::GetInstance();
	TInt index=iContainer->iFileMgrList->GetCurrentSelectIndex();		

    switch ( aCommand )
	{
		case EAknSoftkeyBack:
		{
			AppUi()->ActivateLocalViewL(KViewId);
			break;
		}
		case EFunSMusicCmdViewFilemgrOpen:
		{
			iContainer->DoClicked();
		break;
		}
		case EFunSMusicCmdViewFilemgrEditCut:
		{
			iContainer->SaveCurPath(1);
		}
		break;
		case EFunSMusicCmdViewFilemgrEditCopy:
		{
			iContainer->SaveCurPath(2);
		}
		break;
		case EFunSMusicCmdViewFilemgrEditDelete:
		{
  			TInt ret=CFunMessageDialog::RunDlgLD(KNullDesC,iplaycore->TextResource(R_MESSAGE_DELETE),iplaycore->m_appUI->UiStyler(),NULL,CFunMessageDialog::EMessageDialog_OkCancel,CFunMessageDialog::EMessageIcon_Question);
			if(!ret)
				return;

			iContainer->SaveCurPath(0);
			iContainer->DoDelete();
			iContainer->iFileMgrList->SetCurrentSelectIndex(0);
			iContainer->UpdataL();
		}
		break;
		case EFunSMusicCmdViewFilemgrEditRename:
		{
			iContainer->SaveCurPath(0);

			TBuf<128> newName;
			TBuf<128> prompt;
			TParse p;
			
			p.Set(iContainer->doPath,NULL,NULL);
			prompt=iplaycore->TextResource(R_TEXT_VIEW_SEARCH_KEY);
			newName.Copy(p.NameAndExt());
			TInt ret=CFunInputDialog::RunDlgLD(KNullDesC,prompt,newName,iplaycore->m_appUI->UiStyler(),NULL);
			if(ret)
			{
				iContainer->DoRename(newName);		
				iContainer->UpdataL();
			}
		
		}
		break;
		
		case EFunSMusicCmdViewFilemgrEditCreatFolder:
		{
			TBuf<128> newName;
			TBuf<128> prompt;
			
			prompt=iplaycore->TextResource(R_TEXT_VIEW_SEARCH_KEY);
			TInt ret=CFunInputDialog::RunDlgLD(KNullDesC,prompt,newName,iplaycore->m_appUI->UiStyler(),NULL);
			if(ret)
			{
				iContainer->DoCreatFolder(newName);		
				iContainer->UpdataL();
			}
		}
		break;
		case EFunSMusicCmdViewFilemgrEditPaster:
		{
			iContainer->DoPlaster();
			iContainer->UpdataL();
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
void CMyleftCenterViewFileMgr::HandleClientRectChange()
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
void CMyleftCenterViewFileMgr::DoActivateL(
   const TVwsViewId& /*aPrevViewId*/,TUid /*aCustomMessageId*/,
   const TDesC8& /*aCustomMessage*/)
    {
 //   if(CenterCore::GetInstance()->m_appUI->UiStyler()!=CenterCore::GetInstance()->m_appUI->iMyStyler)
 //  	 AppUi()->SetUiStyler(CenterCore::GetInstance()->m_appUI->iMyStyler);
	AppUi()->SetUiStyler(CenterCore::GetInstance()->m_appUI->iMyStyler2);
	AppUi()->TitleBar()->ResetTitleTabMenu();
	AppUi()->TitleBar()->SetTitle(CenterCore::GetInstance()->TextResource(R_TEXT_POPMENU_FILE_MANAGER));
	AppUi()->TitleBar()->SetTextColor(TRgb(254,254,254));

    if (!iContainer)
        {
        iContainer = CMyleftCenterContainerFileMgr::NewL(this, ClientRect());
        iContainer->SetMopParent(this);
        AppUi()->AddToStackL( *this, iContainer );
        }
   }

// ---------------------------------------------------------
// CFunVVView2::DoDeactivate()
// 
// ---------------------------------------------------------
//
void CMyleftCenterViewFileMgr::DoDeactivate()
{
	FC_Log("CMyleftCenterViewFileMgr::DoDeactivate()");

	if ( iContainer )
    {
		AppUi()->RemoveFromViewStack( *this, iContainer );
		delete iContainer;
		iContainer = NULL;
    }
}
