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
#include  <avkon.hrh>
#include <eikappui.h>
#include <aknview.h> 
#include  <MyleftCenter.rsg>
#include  "MyleftCenterView.h"
#include  "MyleftCenterViewBrCtl.h"
#include  "MyleftCenterContainerBrCtl.h" 
 #include "FunUIStart.h"
#include "CenterCore.h"
#include  "MyleftCenter.hrh"
#include "SinaInterfaceMgr.h"
// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CFunSMusicView2::ConstructL(const TRect& aRect)
// EPOC two-phased constructor
// ---------------------------------------------------------
//
void CMyleftCenterViewBrCtl::ConstructL()
    {
    BaseConstructL( R_MYLEFTCENTER_VIEW_BRCTL,0 );  //r_myleftcenter_view_read_serach
    iCloseContainer=EFalse;
    }

// ---------------------------------------------------------
// CFunSMusicView2::~CFunSMusicView2()
// destructor
// ---------------------------------------------------------
//
CMyleftCenterViewBrCtl::~CMyleftCenterViewBrCtl()
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
TUid CMyleftCenterViewBrCtl::Id() const
    {
    return KViewBrCtlId;
    }

// ---------------------------------------------------------
// CFunSMusicView2::HandleCommandL(TInt aCommand)
// takes care of view command handling
// ---------------------------------------------------------
//
void CMyleftCenterViewBrCtl::HandleCommandL(TInt aCommand)
    {   
    	CenterCore* iplaycore=CenterCore::GetInstance();

    switch ( aCommand )
        {
	case ECommandViewWapRefresh:
		{
	            if (iContainer->iBrCtlInterface)
	                {
	              	 iContainer->iBrCtlInterface->HandleCommandL(iContainer->iCommandBase + TBrCtlDefs::ECommandReload);
	                }

		}
	  	break;
	case ECommandViewWapForward:
		{
	            if (iContainer->iBrCtlInterface)
	                {
	                iContainer->iBrCtlInterface->HandleCommandL(iContainer->iCommandBase + TBrCtlDefs::ECommandForward);
	                }
		}
	  	break;
	case ECommandViewWapBack:
		{
	            if (iContainer->iBrCtlInterface)
	                {
	                iContainer->iBrCtlInterface->HandleCommandL(iContainer->iCommandBase + TBrCtlDefs::ECommandBack);
	                }
		}
	  	break;
	case ECommandViewWapClose:
		{
	            if (iContainer->iBrCtlInterface)
	                {
	                iContainer->iBrCtlInterface->HandleCommandL(iContainer->iCommandBase + TBrCtlDefs::ECommandDisconnect);
	                }
			 iCloseContainer=ETrue;
			CenterCore::GetInstance()->m_appUI->ActivateLocalViewL(KViewId); 
		}
	  	break;
        case EAknSoftkeyBack:
            {
			 iCloseContainer=EFalse;
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
void CMyleftCenterViewBrCtl::HandleClientRectChange()
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
void CMyleftCenterViewBrCtl::DoActivateL(
   const TVwsViewId& /*aPrevViewId*/,TUid /*aCustomMessageId*/,
   const TDesC8& /*aCustomMessage*/)
    {
    if(CenterCore::GetInstance()->m_appUI->UiStyler()!=CenterCore::GetInstance()->m_appUI->iMyStyler)
	{
	 	AppUi()->SetUiStyler(CenterCore::GetInstance()->m_appUI->iMyStyler);
		AppUi()->TitleBar()->ResetTitleTabMenu();
		//AppUi()->TitleBar()->SetTitle(CenterCore::GetInstance()->TextResource(R_TEXT_POPMENU_PARAME_SETTING));//r_text_popmenu_parame_setting
		AppUi()->TitleBar()->SetTextColor(TRgb(254,254,254));
	}
    if (!iContainer)
        {
        iContainer = new (ELeave) CMyleftCenterContainerBrCtl;
        iContainer->SetMopParent(this);
         iContainer->ConstructL( this,ClientRect() );
	// iContainer->ConstructL( AppUi()->ApplicationRect());
        }
	 AppUi()->AddToStackL( *this, iContainer );
	 CenterCore::GetInstance()->m_mainFouceId=0;
   }

// ---------------------------------------------------------
// CFunSMusicView2::DoDeactivate()
// 
// ---------------------------------------------------------
//
void CMyleftCenterViewBrCtl::DoDeactivate()
    {
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        }

	if(iCloseContainer)
	{
		delete iContainer;
		iContainer = NULL;
	}
    }



