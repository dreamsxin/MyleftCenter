/*
============================================================================
 Name        : CMyleftCenterAppUi from MyleftCenterAppui.cpp
 Author      : yilizheng
 Version     :
 Copyright   : MyleftCenter
 Description : CMyleftCenterAppUi implementation
============================================================================
*/

// INCLUDE FILES

#include <avkon.hrh>
#include <APGCLI.H>

#include <smut.h>

#include "CenterCore.h"
#include <MyleftCenter.mbg>
#include <MyleftCenter.rsg>
#include "MyleftCenter.hrh"
#include "portab_symbian.h"
#include "const.h"
#include "FunUIstart.h"

#include "MyleftCenterAppui.h"
#include "MyleftCenterView.h"
#include "MyleftCenterView2.h"
#include  "MyleftCenterViewLoad.h"
#include  "MyleftCenterViewSearch.h"
#include  "MyleftCenterViewReadList.h"
#include "MyleftCenterViewRead.h"
#include "MyleftCenterViewReadSet.h"
#include  "MyleftCenterViewFileMgr.h"
#include  "MyleftCenterViewDownLoad.h"
#include  "MyleftCenterViewMusicSearch.h"
#include  "MyleftCenterViewMusicList.h"
#include "MyleftCenterViewMusic.h"
#include "MyleftCenterViewChatRoom.h"
#ifdef _S60_3RD_
#include "MyleftCenterViewBrCtl.h"

#include <rsendas.h>
#include <rsendasmessage.h>
#endif

// ================= MEMBER FUNCTIONS =======================
//
// ----------------------------------------------------------
// CMyleftCenterAppUi::ConstructL()
// 
// ----------------------------------------------------------
//
void CMyleftCenterAppUi::ConstructL()
{
	iMyStyler = new (ELeave) CCenterUiStyler( this );
	iMyStyler2=new (ELeave) CCenterUiStyler2( this );
	// Initialise app UI with standard value.
	BaseConstructL(0, iMyStyler);

	iTimer = CFunTimeOutTimer::NewL( 0, *this );
	iTimer->After( 1000000 );

	CFunLabel* timeLabel = CFunLabel::NewL( TRect(TPoint(ClientRect().iBr.iX-60,ClientRect().iTl.iY),TSize(90,22)), 
									TitleBar(), KNullDesC, FunUiLib::SmallFont() );
	timeLabel->SetTextColor( KRgbWhite );
	timeLabel->SetTextAlign( CGraphicsContext::ERight );
	timeLabel->SetTransparent( ETrue );
	timeLabel->SetBackgroundDrawer( TitleBar() );
	TitleBar()->AssignContextControl( timeLabel );
	iTimeLabel = timeLabel;


	TitleBar()->SetTitleStyler(ETrue);
//	TitleBar()->SetIconL(KMusicMultiBitmapFilename,EMbmMyleftcenterTab1_sel);
	TitleBar()->SetTabMenuRect(TRect(TPoint(KTitleIcon_len,0),TSize(KMainWight-KTitleIcon_len-KTitleContent_len,KTitleHight_Len)));


	viewLoad= new (ELeave) CMyleftCenterViewLoad;
	CleanupStack::PushL( viewLoad );
	viewLoad->ConstructL();
	AddViewL( viewLoad );    
	CleanupStack::Pop();    // view uid =1

	view1 = new (ELeave) CMyleftCenterView;
	CleanupStack::PushL( view1 );
	view1->ConstructL();
	AddViewL( view1 );    
	CleanupStack::Pop();    // view uid =2

	view2 = new (ELeave) CMyleftCenterView2;
	CleanupStack::PushL( view2 );
	view2->ConstructL();
	AddViewL( view2 );     
	CleanupStack::Pop();    // view uid =3

	viewReadList= new (ELeave) CMyleftCenterViewReadList;
	CleanupStack::PushL( viewReadList );
	viewReadList->ConstructL();
	AddViewL( viewReadList );    
	CleanupStack::Pop();    // view uid =4

	viewSearch= new (ELeave) CMyleftCenterViewSearch;
	CleanupStack::PushL( viewSearch );
	viewSearch->ConstructL();
	AddViewL( viewSearch );    
	CleanupStack::Pop();    // view uid =5

	viewRead= new (ELeave) CMyleftCenterViewRead;
	CleanupStack::PushL( viewRead );
	viewRead->ConstructL();
	AddViewL( viewRead );    
	CleanupStack::Pop();    // view uid =6

	viewReadSet=new (ELeave) CMyleftCenterViewReadSet;
	CleanupStack::PushL( viewReadSet );
	viewReadSet->ConstructL();
	AddViewL( viewReadSet );    
	CleanupStack::Pop();    // view uid =7

	viewFileMgr=new (ELeave) CMyleftCenterViewFileMgr;
	CleanupStack::PushL( viewFileMgr );
	viewFileMgr->ConstructL();
	AddViewL( viewFileMgr );    
	CleanupStack::Pop();    // view uid =8
	
	viewDownLoad=new (ELeave) CMyleftCenterViewDownLoad;
	CleanupStack::PushL( viewDownLoad );
	viewDownLoad->ConstructL();
	AddViewL( viewDownLoad );    
	CleanupStack::Pop();    // view uid =9

	viewMusic= new (ELeave) CMyleftCenterViewMusic;
	CleanupStack::PushL( viewMusic );
	viewMusic->ConstructL();
	AddViewL( viewMusic );    
	CleanupStack::Pop();    // view uid =10

	viewMusicList= new (ELeave) CMyleftCenterViewMusicList;
	CleanupStack::PushL( viewMusicList );
	viewMusicList->ConstructL();
	AddViewL( viewMusicList );    
	CleanupStack::Pop();    // view uid =11

	viewMusicSearch= new (ELeave) CMyleftCenterViewMusicSearch;
	CleanupStack::PushL( viewMusicSearch );
	viewMusicSearch->ConstructL();
	AddViewL( viewMusicSearch );    
	CleanupStack::Pop();    // view uid =12
#ifdef _S60_3RD_
	viewBrCtl= new (ELeave) CMyleftCenterViewBrCtl;
	CleanupStack::PushL( viewBrCtl );
	viewBrCtl->ConstructL();
	AddViewL( viewBrCtl );    
	CleanupStack::Pop();    // view uid =13
#endif	

	viewChatRoom= new (ELeave) CMyleftCenterViewChatRoom;
	CleanupStack::PushL( viewChatRoom );
	viewChatRoom->ConstructL();
	AddViewL( viewChatRoom );    
	CleanupStack::Pop();    // view uid =12

	AddToStackL(TitleBar());
	SetDefaultViewL(*viewLoad);

}

// ----------------------------------------------------
// CMyleftCenterAppUi::~CMyleftCenterAppUi()
// Destructor
// Frees reserved resources
// ----------------------------------------------------
//
CMyleftCenterAppUi::~CMyleftCenterAppUi()
{
    	if( iTimer && iTimer->IsActive() )
		iTimer->Cancel();
	delete iTimer;

	delete iMyStyler;
	delete iMyStyler2;
	if(TitleBar())
		RemoveFromStack(TitleBar());

}

// ------------------------------------------------------------------------------
// CMyleftCenterAppUi::DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane)
//  This function is called by the EIKON framework just before it displays
//  a menu pane. Its default implementation is empty, and by overriding it,
//  the application can set the state of menu items dynamically according
//  to the state of application data.
// ------------------------------------------------------------------------------
//
void CMyleftCenterAppUi::DynInitMenuPaneL(
    TInt /*aResourceId*/,CEikMenuPane* /*aMenuPane*/)
    {
    }

// ----------------------------------------------------
// CMyleftCenterAppUi::HandleKeyEventL(
//     const TKeyEvent& aKeyEvent,TEventCode /*aType*/)
// takes care of key event handling
// ----------------------------------------------------
//
TKeyResponse CMyleftCenterAppUi::HandleKeyEventL(
    const TKeyEvent& aKeyEvent,TEventCode aType)
    {
		return EKeyWasNotConsumed;
    }

// ----------------------------------------------------
// CMyleftCenterAppUi::HandleCommandL(TInt aCommand)
// takes care of command handling
// ----------------------------------------------------
//
void CMyleftCenterAppUi::HandleCommandL(TInt aCommand)
    {
    switch ( aCommand )
        {
        case EMyleftCenterCmdExit:
            {
            Exit();
            break;
            }
        case EMyleftCenterCmdAppTest:
            {
            break;
            }
	case  EAknCmdExit:
       {
		SendToBackground();
	}
  	break;
        default:
            break;      
        }
    }
#ifdef _S60_3RD_
void CMyleftCenterAppUi::HandleWsEventL(const TWsEvent &aEvent, CCoeControl *aDestination)
   {
   switch (aEvent.Type())
       {
       case KAknUidValueEndKeyCloseEvent  :
          SendToBackground();
           break;
       default:
           CAknAppUi::HandleWsEventL(aEvent, aDestination);
       }
   }
#endif
 // ----------------------------------------------------
 // CMyleftCenterAppUi::TabChangedL(TInt aIndex)
 // This method gets called when CAknTabGroup active 
 // tab has changed.
 // ----------------------------------------------------
 //
 void CMyleftCenterAppUi::TabChangedL(TInt aIndex)
{
}

 void CMyleftCenterAppUi::MftonTimerExpired()
 {
	 TTime now;
 	TBuf<64> timeBuf;
	now.HomeTime();
	now.FormatL(timeBuf,_L("%H:%T")); 
//	CenterCore* playcore=CenterCore::GetInstance();	
//	timeBuf.Append(_L("abcdefghijklmnopqrstuvwxyz"));
	iTimeLabel->ChangeTextL( timeBuf );
	
	if(flag<TIME_OUT_TIME)
	{

		if(flag==(TIME_OUT_TIME-1))
		{
			if(viewLoad->iContainer)
			{
				viewLoad->iContainer->StopAndCancle();					
			}
			ActivateLocalViewL(KViewId);
		}
		flag++;
		
	}	
	iTimer->After( 1000000 );
 }

void CMyleftCenterAppUi::HandleForegroundEventL( TBool aForeground ) 
{

	CFunAppUi::HandleForegroundEventL(aForeground);
	if(!aForeground)
	{
		if(ActiveView()->MenuBar())
		{
			if(ActiveView()->MenuBar()->MenuPane())
			{
				CenterCore::GetInstance()->HidePopMenu();
			}		
		}
	}
	
}
 TInt CMyleftCenterAppUi::DiskInfoL(TInt64& aTotal, TInt64& aFree, const TDriveNumber aVolume)
{

	TVolumeInfo vinfo;
	RFs fs;
	User::LeaveIfError( fs.Connect() );

	TInt err = fs.Volume(vinfo, aVolume);
	aTotal = TInt64(vinfo.iSize);
	aFree = TInt64(vinfo.iFree);

	fs.Close();
	return err;
}

  void CMyleftCenterAppUi::SendToBackground ()
{
	TApaTask task(iEikonEnv->WsSession( )); 
	task.SetWgId( CEikonEnv::Static()->RootWin().Identifier());
	task.SendToBackground(); 
}

void  CMyleftCenterAppUi::OpenOtherFileL(const TDesC& aFileName)
{
	RApaLsSession   appArcSession;
	User::LeaveIfError(appArcSession.Connect());    // connect to AppArc server
	TThreadId id;

	FC_Log("CFunVVAppUi::OpenOtherFileL");
	appArcSession.StartDocument( aFileName,id );
	appArcSession.Close();

}

void CMyleftCenterAppUi::SendSms( const TDesC& aNumber, const TDesC& aText ) 
{
	FC_Log("SendSms :");
	
#ifndef _NOKIA_90_
	RSendAs sendAs;
	User::LeaveIfError(sendAs.Connect());
	CleanupClosePushL(sendAs);

	RSendAsMessage sendAsMessage;
	sendAsMessage.CreateL(sendAs, KUidMsgTypeSMS);
	CleanupClosePushL(sendAsMessage);

	// prepare the message
	sendAsMessage.AddRecipientL(aNumber, RSendAsMessage::ESendAsRecipientTo);
	sendAsMessage.SetBodyTextL(aText);

	// send the message
	sendAsMessage.SendMessageAndCloseL();

	// sendAsMessage (already closed)
	CleanupStack::Pop();

	// sendAs
	CleanupStack::PopAndDestroy();
#else
	if( !iSmsShell )
		iSmsShell = CSmsShell::NewL( NULL );
	iSmsShell->SendL( aNumber, aText );
#endif
}

void CMyleftCenterAppUi::AutoStartSmsDaemon()
{
#ifndef _NOKIA_90_
	_LIT( KSmsDaemonAppName, "FunMusicSmsDaemon" );
	_LIT( KSmsDaemonAppPathName, "C:\\sys\\bin\\FunMusicSmsDaemon.EXE" );
	_LIT( KSmsDaemonAppPathName2, "E:\\sys\\bin\\FunMusicSmsDaemon.EXE" );
#else
	_LIT( KSmsDaemonAppName, "FunMusicSmsDaemon" );
	_LIT( KSmsDaemonAppPathName, "c:\\system\\Programs\\FunMusicSmsDaemon.EXE" );
	_LIT( KSmsDaemonAppPathName2, "e:\\system\\Programs\\FunMusicSmsDaemon.EXE" );
#endif
	TFindProcess findProcess( _L("*") );
	TFullName result;
	//	TBuf8<KMaxFullName> result8;

	while(findProcess.Next(result)==KErrNone)
	{
		TInt idx = result.FindF( KSmsDaemonAppName );
		if(idx!=KErrNotFound)
		{//found process
			return;
		}
	}

	RProcess server2;

	if( KErrNone != server2.Create( KSmsDaemonAppPathName , _L("")))
	{
		if( KErrNone != server2.Create( KSmsDaemonAppPathName2 , _L("")) )
		{
			FC_Panic( 8002 );
			return;
		}
	}
	CleanupClosePushL(server2);

	server2.Resume();

	CleanupStack::PopAndDestroy();
}


 void CCenterUiStyler::InitFunControlUI( CFunBorderedControl* aControl, TInt aUiType, TAny* aData ) 
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
	case EUiTypeList:
		{
		}
		break;
	case EUiTypeDialog:
		{
			CFunDialogBase* dialog = STATIC_CAST(CFunDialogBase*, aControl);
			dialog->TitleBar()->BackDrawer()->SetBackgroundColor( KRgbContainerDeep);
		//	dialog->TitleBar()->BackDrawer()->SetBackgroundImageL( KMusicMultiBitmapFilename, EMbmFunsmusicTitle );
			dialog->TitleBar()->SetHeight( 22 );
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

void CCenterUiStyler::CreateTitleBar( CFunTitleBar* aTitleBar ) 
{
	aTitleBar->SetTitle( KNullDesC );
//	aTitleBar->BackDrawer()->SetBackgroundColor( KRgbTitle );
//	aTitleBar->BackDrawer()->SetRectBorder( KRgbRect, 1 );
	aTitleBar->BackDrawer()->SetBackgroundImageL( KMusicMultiBitmapFilename, EMbmMyleftcenterTop );
//	aTitleBar->SetIconL( KMusicMultiBitmapFilename, EMbmFunsmusicTitle, EMbmFunvvskinFunvv_mask );
//	aTitleBar->SetIconLeftMargin( 2);
	aTitleBar->SetHeight( KTitleHight_Len);
}

void CCenterUiStyler::CreateControlBar( CFunControlBar* aControlBar ) 
{
	TBuf<32> rightBuf;
	CEikonEnv::Static()->ReadResource (rightBuf, R_TEXT_VIEW_HIDE);
	
	aControlBar->BackDrawer()->ClearBackgroundImage();
//	aControlBar->BackDrawer()->SetBackgroundColor( KRgbTitle );
//	aControlBar->BackDrawer()->SetRectBorder( KRgbRect, 1 );
	aControlBar->BackDrawer()->SetBackgroundImageL( KMusicMultiBitmapFilename, EMbmMyleftcenterButton );
	aControlBar->SetHeight( KButtonHight_Len );
	aControlBar->OnParentSizeChanged(iAppUi->ApplicationRect());
//	aControlBar->SetCommandL(CFunControlBar::ELeftCommand, EAknSoftkeyOptions ,KNullDesC,KMusicMultiBitmapFilename,EMbmMyleftcenterLeftcmd,EMbmMyleftcenterLeftcmd_mask);
	aControlBar->SetCommand(CFunControlBar::ERightCommand, EAknSoftkeyBack ,rightBuf);
	aControlBar->SetButtonTextColor(TRgb(254,254,254));
}
void CCenterUiStyler::DynInitMenuPaneL(TInt aResourceId, CFunMenuPane *aMenuPane)
{
	CFunMenuPane* menuPane = aMenuPane;
	if( menuPane )
	{
	//	menuPane->BackDrawer()->SetBackgroundImageL( KMusicMultiBitmapFilename, EMbmMyleftcenterPopmenu );
	//	menuPane->BackDrawer()->SetNoBorder();
		menuPane->BackDrawer()->SetBackgroundColor(  TRgb(11,73,100) );
	//	menuPane->BackDrawer()->SetRoundRectBorder( KRgbGray, TSize(4,4) );
		menuPane->BackDrawer()->SetRoundRectBorder( KRgbRect, TSize(4,4) );

		menuPane->SetTextColors( TRgb(254,254,254), TRgb(254,254,254) );
		menuPane->SetMargin( 5,5, 3,3);
		menuPane->SetItemHeight( KPopSelectIconH);
		menuPane->SetFont( FunUiLib::NormalFont() );

		menuPane->FocusBackDrawer()->SetBackgroundColor(KRgbContainer);
	//	menuPane->FocusBackDrawer()->SetBackgroundImageL( KMusicMultiBitmapFilename, EMbmMyleftcenterPopmenu_select );
		
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

CCenterUiStyler::CCenterUiStyler ( CFunAppUi* aAppUi )
{
	iAppUi = aAppUi;
}

//view styler1  no ControlBar;

 void CCenterUiStyler2::InitFunControlUI( CFunBorderedControl* aControl, TInt aUiType, TAny* aData ) 
{
//   FC_Log("CCenterViewStyler1::InitFunControlUI  ");
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
		//	dialog->TitleBar()->BackDrawer()->SetBackgroundImageL( KMusicMultiBitmapFilename, EMbmFunsmusicTitle );
			dialog->TitleBar()->SetHeight( 22 );
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

void CCenterUiStyler2::CreateTitleBar( CFunTitleBar* aTitleBar ) 
{
	aTitleBar->BackDrawer()->SetBackgroundImageL( KMusicMultiBitmapFilename, EMbmMyleftcenterTop );
//	aTitleBar->SetTitle( KNullDesC );
	aTitleBar->SetHeight(KTitleHight_Len);
}

void CCenterUiStyler2::CreateControlBar( CFunControlBar* aControlBar ) 
{
	aControlBar->SetHeight( 0 );
}
void CCenterUiStyler2::DynInitMenuPaneL(TInt aResourceId, CFunMenuPane *aMenuPane)
{
	CFunMenuPane* menuPane = aMenuPane;
	if( menuPane )
	{
	//	menuPane->BackDrawer()->SetBackgroundImageL( KMusicMultiBitmapFilename, EMbmMyleftcenterPopmenu );
	//	menuPane->BackDrawer()->SetNoBorder();
		menuPane->BackDrawer()->SetBackgroundColor(  TRgb(11,73,100) );
	//	menuPane->BackDrawer()->SetRoundRectBorder( KRgbGray, TSize(4,4) );
		menuPane->BackDrawer()->SetRoundRectBorder( KRgbRect, TSize(4,4) );

		menuPane->SetTextColors( TRgb(254,254,254), TRgb(254,254,254) );
		menuPane->SetMargin( 5,5, 3,3);
		menuPane->SetItemHeight( KPopSelectIconH);
		menuPane->SetFont( FunUiLib::NormalFont() );

		menuPane->FocusBackDrawer()->SetBackgroundColor(KRgbContainer);
	//	menuPane->FocusBackDrawer()->SetBackgroundImageL( KMusicMultiBitmapFilename, EMbmMyleftcenterPopmenu_select );
		
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

CCenterUiStyler2::CCenterUiStyler2 ( CFunAppUi* aAppUi )
{
	iAppUi= aAppUi;
}

