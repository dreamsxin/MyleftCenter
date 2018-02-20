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
#include  <UTF.H>
#include  <MyleftCenter.rsg>
#include <MyleftCenter.mbg>
#include  "MyleftCenter.hrh"
#include "MyleftCenterAppui.h"
#include  "MyleftCenterView.h"
#include  "MyleftCenterView2.h"
#include  "MyleftCenterContainer2.h" 
#include  "MyleftCenterViewReadSet.h"
#include  "MyleftCenterViewReadList.h"
#include  "MyleftCenterViewSearch.h" 
#include  "CenterCore.h"
#include "SinaInterfaceMgr.h"
#include  "const.h"
// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMyleftCenterView2::ConstructL(const TRect& aRect)
// EPOC two-phased constructor
// ---------------------------------------------------------
//
void CMyleftCenterView2::ConstructL()
{
	BaseConstructL( R_MYLEFTCENTER_VIEW2,0 );
}

// ---------------------------------------------------------
// CMyleftCenterView2::~CMyleftCenterView2()
// destructor
// ---------------------------------------------------------
//
CMyleftCenterView2::~CMyleftCenterView2()
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
TUid CMyleftCenterView2::Id() const
    {
    return KView2Id;
    }

// ---------------------------------------------------------
// CMyleftCenterView2::HandleCommandL(TInt aCommand)
// takes care of view command handling
// ---------------------------------------------------------
//
void CMyleftCenterView2::HandleCommandL(TInt aCommand)
    {   
    switch ( aCommand )
        {
        case EMyleftCenterCmdViewReadHelp:
	{
#if 0
		HBufC8* hbuf8=HBufC8::NewLC(3000);
		HBufC* hbuf16=HBufC::NewLC(1500);
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

		CleanupStack::PopAndDestroy (2);
#else

		HBufC8* hbuf8=HBufC8::NewLC(512);
		HBufC* hbuf16=HBufC::NewLC(1024);
		TPtr8 pbuf8=hbuf8->Des();
		TPtr pbuf16=hbuf16->Des();

		RFs		fileSer; 
		RFile		file;
		fileSer.Connect();
		User::LeaveIfError(file.Open(fileSer,_L("e:\\testUrl.txt"),EFileRead ));
		file.Read(pbuf8);				
		file.Close();
		fileSer.Close();		

		FC_AsciiToUnicodeL(pbuf16, pbuf8);
	//	CnvUtfConverter::ConvertToUnicodeFromUtf8(pbuf16,pbuf8);
		
		CenterCore::GetInstance()->iSinaMgr->LaunchGetTestUrl(pbuf16);

//("http://60.191.0.157:8080/yue/sina2.php?method=getYueInit&username=1000&v=0")
		CleanupStack::PopAndDestroy (2);

#endif
	}          
	  	break;
        case EMyleftCenterCmdViewReadSetting:
            {
				CenterCore::GetInstance()->m_appUI->viewReadSet->setType=1;
				CenterCore::GetInstance()->m_appUI->ActivateLocalViewL(KViewReadSetId);
            break;
            }			
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
// CMyleftCenterView2::HandleClientRectChange()
// ---------------------------------------------------------
//
void CMyleftCenterView2::HandleClientRectChange()
    {
    if ( iContainer )
        {
        iContainer->SetRect( ClientRect() );
        }
    }

// ---------------------------------------------------------
// CMyleftCenterView2::DoActivateL(...)
// 
// ---------------------------------------------------------
//
void CMyleftCenterView2::DoActivateL(
   const TVwsViewId& /*aPrevViewId*/,TUid /*aCustomMessageId*/,
   const TDesC8& /*aCustomMessage*/)
    {
    FC_Logsp("CMyleftCenterView2::DoActivateL ");
	/*
    if(CenterCore::GetInstance()->m_appUI->UiStyler()!=CenterCore::GetInstance()->m_appUI->iMyStyler)
   	 AppUi()->SetUiStyler(CenterCore::GetInstance()->m_appUI->iMyStyler);*/
   	 AppUi()->SetUiStyler(CenterCore::GetInstance()->m_appUI->iMyStyler2);


    if (!iContainer)
        {
        iContainer = new (ELeave) CMyleftCenterContainer2;
        iContainer->SetMopParent(this);
        iContainer->ConstructL( ClientRect() );
        AppUi()->AddToStackL( *this, iContainer );
	
        }
	CenterCore::GetInstance()->m_mainFouceId=1;
	CenterCore::GetInstance()->m_readViewid=KView2Id;
   }

// ---------------------------------------------------------
// CMyleftCenterView2::DoDeactivate()
// 
// ---------------------------------------------------------
//
void CMyleftCenterView2::DoDeactivate()
    {
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        }
    
    delete iContainer;
    iContainer = NULL;
    }



