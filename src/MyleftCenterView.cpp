/*
============================================================================
 Name        : CMyleftCenterView from MyleftCenterView.h
 Author      : yilizheng
 Version     :
 Copyright   : MyleftCenter
 Description : CMyleftCenterView implementation
============================================================================
*/
#include  "MyleftCenterContainer.h" 
// INCLUDE FILES
#include  <aknviewappui.h>
#include  <avkon.hrh>
#include  <MyleftCenter.rsg>
#include  "MyleftCenterView.h"
#include  "MyleftCenterView2.h"
#include  "MyleftCenterViewRead.h"
#include  "MyleftCenterViewReadList.h"
#include  "MyleftCenterViewSearch.h"
#include  "MyleftCenterViewReadSet.h"
#include  "MyleftCenterViewMusic.h"
#include  "MyleftCenterViewMusicList.h"
#include  "MyleftCenterViewMusicSearch.h"
#include  "MyleftCenterViewFileMgr.h"
#include  "MyleftCenterViewDownload.h"
#include  "MyleftCenterViewBrCtl.h"

#include <MyleftCenter.mbg>
#include "MyleftCenter.hrh"
#include <charconv.h>
#include <utf.h>
#include "const.h"
#include "FunUIstart.h"
#include "CenterCore.h"
#include "SinaInterfaceMgr.h"
// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMyleftCenterView::ConstructL(const TRect& aRect)
// EPOC two-phased constructor
// ---------------------------------------------------------
//
void CMyleftCenterView::ConstructL()
{

	 iMyStyler = new (ELeave) CCenterView1Styler( this );
	

   	 BaseConstructL( R_MYLEFTCENTER_VIEW1,0 );

}

// ---------------------------------------------------------
// CMyleftCenterView::~CMyleftCenterView()
// destructor
// ---------------------------------------------------------
//
CMyleftCenterView::~CMyleftCenterView()
{
	if ( iContainer )
	{
		AppUi()->RemoveFromViewStack( *this, iContainer );
	}
	delete iContainer;

	delete iMyStyler;
}

// ---------------------------------------------------------
// TUid CMyleftCenterView::Id()
//
// ---------------------------------------------------------
//
TUid CMyleftCenterView::Id() const
    {
    return KViewId;
    }

 void CMyleftCenterView:: DynInitMenuPaneL(TInt aResourceId, CFunMenuPane* aMenuPane)
{
	CFunMenuPane* menuPane = aMenuPane;
	if( menuPane && (aResourceId==R_MYLEFTCENTER_VIEWTO_SUBPANE))//R_MYLEFTCENTER_VIEW1_MENU
	{
			menuPane->SetItemVisible(EMyleftCenterCmdAppToMenu,0);			
	}
}
// ---------------------------------------------------------
// CMyleftCenterView::HandleCommandL(TInt aCommand)
// takes care of view command handling
// ---------------------------------------------------------
//
void CMyleftCenterView::HandleCommandL(TInt aCommand)
    {   
    switch ( aCommand )
        {
        case EMyleftCenterCmdAppTest:
            {

			HBufC8* hbuf8=HBufC8::NewL(3000);
			HBufC* hbuf16=HBufC::NewL(1500);
			TPtr8 pbuf8=hbuf8->Des();
			TPtr pbuf16=hbuf16->Des();

			RFs		fileSer; 
			RFile		file;
			fileSer.Connect();
			User::LeaveIfError(file.Open(fileSer,KReadAboutFilename,EFileRead ));
			file.Read(pbuf8);				
			file.Close();
			fileSer.Close();		

			CnvUtfConverter::ConvertToUnicodeFromUtf8(pbuf16,pbuf8);

  			TInt ret=CFunMessageDialog::RunDlgLD(CenterCore::GetInstance()->TextResource(R_TEXT_POPMENU_HELP),pbuf16,CenterCore::GetInstance()->m_appUI->UiStyler(),NULL,CFunMessageDialog::EMessageDialog_OK,CFunMessageDialog::EMessageIcon_None);
		//	FPlayerCore::DisplayMessage(_L("FondoMusic"),pbuf16);

			delete hbuf16;
			delete hbuf8;
		}
		break;
	case EMyleftCenterCmdPopFileManager:
	{
		CenterCore::GetInstance()->m_appUI->ActivateLocalViewL(KViewFileMgrId); 
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
		CenterCore::GetInstance()->m_appUI->viewReadSet->setType=0;
		CenterCore::GetInstance()->m_appUI->ActivateLocalViewL(KViewReadSetId); 
	}
	 break;
        case EAknSoftkeyBack:
            {
		CenterCore::GetInstance()->m_appUI->SendToBackground();
            break;
            }
	case  EAknCmdExit:
		{
			 AppUi()->HandleCommandL(EEikCmdExit);
		}
	  break;
	case EMyleftCenterCmdExit:
		{
			AppUi()->HandleCommandL(aCommand);
		}		
        default:
            {
            AppUi()->HandleCommandL( aCommand );
            break;
            }
        }
    }

// ---------------------------------------------------------
// CMyleftCenterView::HandleClientRectChange()
// ---------------------------------------------------------
//
void CMyleftCenterView::HandleClientRectChange()
    {
    if ( iContainer )
        {
        iContainer->SetRect( ClientRect() );
        }
    }

// ---------------------------------------------------------
// CMyleftCenterView::DoActivateL(...)
// 
// ---------------------------------------------------------
//
void CMyleftCenterView::DoActivateL(
   const TVwsViewId& /*aPrevViewId*/,TUid /*aCustomMessageId*/,
   const TDesC8& /*aCustomMessage*/)
    {

	AppUi()->SetUiStyler(iMyStyler);
	
    if (!iContainer)
        {
        iContainer = new (ELeave) CMyleftCenterContainer;
        iContainer->SetMopParent(this);
        iContainer->ConstructL( ClientRect());
        AppUi()->AddToStackL( *this, iContainer );
        } 
   }

// ---------------------------------------------------------
// CMyleftCenterView::DoDeactivate()
// 
// ---------------------------------------------------------
//
void CMyleftCenterView::DoDeactivate()
{

	
	if ( iContainer )
	{
		AppUi()->RemoveFromViewStack( *this, iContainer );
	}

	delete iContainer;
	iContainer = NULL;

}


 void CCenterView1Styler::InitFunControlUI( CFunBorderedControl* aControl, TInt aUiType, TAny* aData ) 
{
	switch( aUiType )
	{
	case EUiTypeTitleTabBar:
	case EUiTypeTitleBar:
		CreateTitleBar( STATIC_CAST(CFunTitleBar*, aControl) );
		break;
	case EUiTypeControlBar:
		CreateControlBar( STATIC_CAST( CFunControlBar*, aControl ) );
		break;
	case EUiTypeMenuPane:
		DynInitMenuPaneL( 0, STATIC_CAST(CFunMenuPane*, aControl) );
		break;
	case EUiTypeDialog:
		{
			CFunDialogBase* dialog = STATIC_CAST(CFunDialogBase*, aControl);
			dialog->TitleBar()->BackDrawer()->SetBackgroundColor( KRgbContainerDeep);
			dialog->TitleBar()->SetTextColor(TRgb(254,254,254));
			dialog->TitleBar()->SetHeight( KButtonHight_Len );
			CreateControlBar( dialog->ControlBar() );

		//	dialog->BackDrawer()->SetRectBorder( KRgbGray, 1 );
		//	dialog->BackDrawer()->SetRoundRectBorder( KRgbRect, TSize(3,3),4 );
			dialog->BackDrawer()->SetBackgroundColor( KRgbContainer );
			dialog->BackDrawer()->SetGradientBackground( CFunBackgroundDrawer::EVerticalOrientation,
											KRgbWhite, TRgb(255,255,192) );
		}
		break;
	default :
			break;
	}
}

void CCenterView1Styler::CreateTitleBar( CFunTitleBar* aTitleBar ) 
{
	aTitleBar->SetTitle( KNullDesC );
	aTitleBar->SetHeight( 0);
}

void CCenterView1Styler::CreateControlBar( CFunControlBar* aControlBar ) 
{
	TBuf<32> rightBuf;
	CEikonEnv::Static()->ReadResource (rightBuf, R_TEXT_VIEW_HIDE);

	aControlBar->BackDrawer()->SetNoBorder( );
//	aControlBar->BackDrawer()->ClearBackgroundImage();
	aControlBar->BackDrawer()->SetBackgroundImageL( KMusicMultiBitmapFilename, EMbmMyleftcenterButton );

	//aControlBar->BackDrawer()->SetBackgroundColor( KRgbContainerDeep);
	//aControlBar->BackDrawer()->SetRectBorder( KRgbRect, 2);
	aControlBar->SetHeight( KButtonHight_Len );
	aControlBar->OnParentSizeChanged(iView->AppUi()->ApplicationRect());
	aControlBar->SetButtonTextColor(TRgb(254,254,254));
//	aControlBar->SetCommandText(CFunControlBar::ECenterCommand, _L("ABCDEFGHIJKLMNOPQRSTUVWXYZ"));
}
void CCenterView1Styler::DynInitMenuPaneL(TInt aResourceId, CFunMenuPane *aMenuPane)
{
	CFunMenuPane* menuPane = aMenuPane;
	if( menuPane )
	{
	//	menuPane->BackDrawer()->SetBackgroundImageL( KMusicMultiBitmapFilename, EMbmMyleftcenterPopmenu );
	//	menuPane->BackDrawer()->SetNoBorder();
		menuPane->BackDrawer()->SetBackgroundColor( KRgbRect);
	//	menuPane->BackDrawer()->SetRoundRectBorder( KRgbGray, TSize(4,4) );
		menuPane->BackDrawer()->SetRectBorder( TRgb(1,1,1), 1);

		menuPane->SetTextColors( TRgb(254,254,254), TRgb(254,254,254) );
		menuPane->SetMargin( 1,1, 0,0);
		menuPane->SetItemHeight( KPopSelectIconH);
		menuPane->SetFont( FunUiLib::NormalFont() );

	//	menuPane->FocusBackDrawer()->SetBackgroundColor(KRgbContainer);
		menuPane->FocusBackDrawer()->SetBackgroundImageL( KMusicMultiBitmapFilename, EMbmMyleftcenterPopmenu_select );
		
		menuPane->SetControlHeight( KButtonHight_Len );
	//	menuPane->ControlBar()->BackDrawer()->SetBackgroundColor( KRgbTitle );
	//	menuPane->ControlBar()->BackDrawer()->SetRectBorder( KRgbRect );
		menuPane->ControlBar()->BackDrawer()->SetBackgroundImageL( KMusicMultiBitmapFilename, EMbmMyleftcenterButton );
		menuPane->ControlBar()->SetButtonTextColor(TRgb(254,254,254));
			
		//menuPane->ScrollBar()->BackDrawer()->SetRectBorder( KRgbTitle, 0 );
		menuPane->ScrollBar()->BackDrawer()->SetBackgroundColor( KRgbRect);
		menuPane->ScrollBar()->Set3DThumb( KRgbContainerDeep,KRgbContainerDeep,KRgbTitle);

		menuPane->SetFadeBehind(EFalse);
	}

}

CCenterView1Styler::CCenterView1Styler ( CFunViewBase* aView)
{
	iView= aView;
}

