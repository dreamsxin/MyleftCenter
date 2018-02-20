/*
============================================================================
 Name        : CFunSMusicView2 from FunSMusicView2.h
 Author      : funinhand
 Version     :
 Copyright   : Your copyright notice
 Description : CFunSMusicView2 implementation
============================================================================
*/

// INCLUDE FILES
#include  <aknviewappui.h>
#include <eikbtgpc.h> 
#include  <avkon.hrh>
#include <eikappui.h>
#include <aknview.h> 
#include  <MyleftCenter.rsg>
#include  "MyleftCenterViewMusicList.h"
#include  "MyleftCenterView.h"
#include  "MyleftCenterContainerMusicList.h" 
#include "FunUIStart.h"
#include "CenterCore.h"
#include  "MyleftCenter.hrh"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CFunSMusicView2::ConstructL(const TRect& aRect)
// EPOC two-phased constructor
// ---------------------------------------------------------
//
void CMyleftCenterViewMusicList::ConstructL()
{
    BaseConstructL( R_MYLEFTCENTER_VIEW_READLIST,0 );
}

// ---------------------------------------------------------
// CFunSMusicView2::~CFunSMusicView2()
// destructor
// ---------------------------------------------------------
//
CMyleftCenterViewMusicList::~CMyleftCenterViewMusicList()
    {
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        }

    delete iContainer;
    }

// ---------------------------------------------------------
// TUid CFunSMusicView2::Id()
// 
// ---------------------------------------------------------
//
TUid CMyleftCenterViewMusicList::Id() const
    {
    return KViewMusicListId;
    }

// ---------------------------------------------------------
// CMainView::ChangeCbaRight(TInt aCommand)
// takes care of view command handling
// ---------------------------------------------------------
//
void CMyleftCenterViewMusicList::ChangeCbaRight( const TDesC& _textstr, TInt _cmd) 
{
	CFunControlBar*  iControlBar=ControlBar();	
	iControlBar->SetCommand(CFunControlBar::ERightCommand, _cmd ,_textstr);
	iControlBar->DrawNow();
}
// ---------------------------------------------------------
// CMainView::ChangeCbaRight(TInt aCommand)
// takes care of view command handling
// ---------------------------------------------------------
//
 void CMyleftCenterViewMusicList:: DynInitMenuPaneL(TInt aResourceId, CFunMenuPane* aMenuPane)
{
	CFunMenuPane* menuPane = aMenuPane;
	if( menuPane )
	{
		if(iContainer->icurPage==0)
		{
			menuPane->SetItemVisible(EMyleftCenterCmdViewReadListUpdataList,1);
			menuPane->SetItemVisible(EMyleftCenterCmdViewReadListUpdataBD,1);
			menuPane->SetItemVisible(EMyleftCenterCmdViewReadListRename,0);
			menuPane->SetItemVisible(EMyleftCenterCmdViewReadListDelete,0);
			menuPane->SetItemVisible(EMyleftCenterCmdViewReadListDeleteAll,0);
			menuPane->SetItemVisible(EMyleftCenterCmdViewReadListInfo,0);
			menuPane->SetItemVisible(EMyleftCenterCmdViewPlayAll,0);
		}
		else if(iContainer->icurPage==1)
		{
			menuPane->SetItemVisible(EMyleftCenterCmdViewReadListUpdataList,0);
			menuPane->SetItemVisible(EMyleftCenterCmdViewReadListUpdataBD,0);
			
			menuPane->SetItemVisible(EMyleftCenterCmdViewReadListRename,1);
			menuPane->SetItemVisible(EMyleftCenterCmdViewReadListDelete,1);
			menuPane->SetItemVisible(EMyleftCenterCmdViewReadListDeleteAll,1);
			menuPane->SetItemVisible(EMyleftCenterCmdViewReadListInfo,1);
			menuPane->SetItemVisible(EMyleftCenterCmdViewPlayAll,1);
		}
		else
		{
			menuPane->SetItemVisible(EMyleftCenterCmdViewReadListUpdataList,0);
			menuPane->SetItemVisible(EMyleftCenterCmdViewReadListUpdataBD,0);
			menuPane->SetItemVisible(EMyleftCenterCmdViewReadListRename,0);
			menuPane->SetItemVisible(EMyleftCenterCmdViewReadListDelete,0);
			menuPane->SetItemVisible(EMyleftCenterCmdViewReadListDeleteAll,0);
			menuPane->SetItemVisible(EMyleftCenterCmdViewReadListInfo,0);
			menuPane->SetItemVisible(EMyleftCenterCmdViewPlayAll,0);
		}
			
	}

}
// ---------------------------------------------------------
// CFunSMusicView2::HandleCommandL(TInt aCommand)
// takes care of view command handling
// ---------------------------------------------------------
//
void CMyleftCenterViewMusicList::HandleCommandL(TInt aCommand)
{   
	CenterCore*   iplaycore=CenterCore::GetInstance();
    switch ( aCommand )
        {
        case EMyleftCenterCmdViewReadListUpdataList:
		{
			iContainer->UpdatePhoneListItemsL();
		}
		break;
        case EMyleftCenterCmdViewReadListUpdataBD:
		{
		//	iContainer->UpdatePhoneListItemsL();
		}
		break;
	case EMyleftCenterCmdViewReadListRename:
		{
		
		}
		break;		
	case EMyleftCenterCmdViewReadListDelete:
		{
			int curId=iContainer->iList->GetCurrentSelectIndex();

			iplaycore->Music_localListDelete(curId);
			iContainer->UpdateListItemsL();
	
		}
		break;		
	case EMyleftCenterCmdViewReadListDeleteAll:
		{
			int curId=iContainer->iList->GetCurrentSelectIndex();
			music_list* list = iplaycore->m_localMusiclist->At(curId);
			CenterCore::DeleteFile(list->iaskurl);
			iplaycore->Music_localListDelete(curId);
			
			iContainer->UpdateListItemsL();
		}
		break;		
	case EMyleftCenterCmdViewReadListInfo:
		{
			HBufC* hbuf16=HBufC::NewLC(250);
			TPtr pbuf16=hbuf16->Des();
			int curId=iContainer->iList->GetCurrentSelectIndex();
			music_list* list = iplaycore->m_localMusiclist->At(curId);

			RFs			m_fs;
			RFile			m_file;
			User::LeaveIfError(m_fs.Connect());
			User::LeaveIfError(m_file.Open(m_fs,list->iaskurl,EFileRead ));
			TInt filsize;
			m_file.Size(filsize);
			m_file.Close();
			m_fs.Close();
			
			filsize=filsize/1024;
				
			pbuf16.Copy(list->title);
			pbuf16.Append(_L("\r\n"));
			pbuf16.Append(list->iaskurl);
			pbuf16.Append(_L("\r\n"));
			pbuf16.Append(_L("Size:"));
			pbuf16.AppendNum(filsize);
			pbuf16.Append(_L("kb"));

			TInt ret=CFunMessageDialog::RunDlgLD(CenterCore::GetInstance()->TextResource(R_TEXT_POPMENU_READ_INFO),pbuf16,CenterCore::GetInstance()->m_appUI->UiStyler(),NULL,CFunMessageDialog::EMessageDialog_OK,CFunMessageDialog::EMessageIcon_None);

			CleanupStack::PopAndDestroy ();
		}
		break;		
	case EMyleftCenterCmdViewPlayAll:
		{
			song tempSong;	
			int count=iplaycore->Music_GetLocalMusicCount();
			int appendId=-1;
			for(int i=0;i<count;i++)
			{
				music_list * list=iplaycore->m_localMusiclist->At(i);
				appendId= iplaycore->Music_MaskListAppend(list->iaskurl, list->title);
				if(i==0)
				{
					tempSong.iaskurl.Copy(list->iaskurl);
					tempSong.name.Copy(list->title);
					iplaycore->m_iCurretPlayIndex=appendId;
					iplaycore->Music_playSong(&tempSong);	
				}
			}					
			iContainer->UpdateListItemsL();
		}
		break;	
        case EAknSoftkeyBack:
	{		
		if(iContainer->icurPage==0)
		{
			iplaycore->m_appUI->ActivateLocalViewL(KViewId);
		}
		else if(iContainer->icurPage==3|| iContainer->icurPage==2|| iContainer->icurPage==1)
		{
			iContainer->icurPage=0;
			iContainer->iList->SetCurrentSelectIndex(0);
			iContainer->UpdateListItemsL();
		}     
		else if(iContainer->icurPage==4)
		{
			iContainer->icurPage=2;
			iContainer->iList->SetCurrentSelectIndex(0);
			iContainer->UpdateListItemsL();
		}     	
		else if(iContainer->icurPage==5)
		{
			iContainer->icurPage=3;
			iContainer->iList->SetCurrentSelectIndex(0);
			iContainer->UpdateListItemsL();
		}     	

	}
	break;
	case EMyleftCenterCmdAppToMenu:
            {
			CenterCore::GetInstance()->m_appUI->ActivateLocalViewL(KViewId);
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
// CFunSMusicView2::HandleClientRectChange()
// ---------------------------------------------------------
//
void CMyleftCenterViewMusicList::HandleClientRectChange()
    {
    if ( iContainer )
        {
        iContainer->SetRect( ClientRect() );
        }
    }

// ---------------------------------------------------------
// CFunSMusicView2::DoActivateL(...)
// 
// ---------------------------------------------------------
//
void CMyleftCenterViewMusicList::DoActivateL(
   const TVwsViewId& /*aPrevViewId*/,TUid /*aCustomMessageId*/,
   const TDesC8& /*aCustomMessage*/)
{
//	if(CenterCore::GetInstance()->m_appUI->UiStyler()!=CenterCore::GetInstance()->m_appUI->iMyStyler)
//		AppUi()->SetUiStyler(CenterCore::GetInstance()->m_appUI->iMyStyler);
	 if(CenterCore::GetInstance()->m_appUI->UiStyler()!=CenterCore::GetInstance()->m_appUI->iMyStyler2)
	 	AppUi()->SetUiStyler(CenterCore::GetInstance()->m_appUI->iMyStyler2);
	if (!iContainer)
	{
		iContainer = CMyleftCenterContainerMusicList::NewL(this,ClientRect());
		iContainer->SetMopParent(this);

		AppUi()->AddToStackL( *this, iContainer );
	}
}

// ---------------------------------------------------------
// CFunSMusicView2::DoDeactivate()
// 
// ---------------------------------------------------------
//
void CMyleftCenterViewMusicList::DoDeactivate()
    {
    if ( iContainer )
        {
		icurSellectId=iContainer->iList->GetCurrentSelectIndex();
		icurPage=iContainer->icurPage;
		iPrevIndex=iContainer->iPrevIndex;
		AppUi()->RemoveFromViewStack( *this, iContainer );
        }
    
    delete iContainer;
    iContainer = NULL;
    }



