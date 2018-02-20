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
#include  "MyleftCenterViewSearch.h"
#include  "MyleftCenterContainerSearch.h" 
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
void CMyleftCenterViewSearch::ConstructL()
    {
    BaseConstructL( R_MYLEFTCENTER_VIEW_READ_SEARCH,0 );  //r_myleftcenter_view_read_serach
    }

// ---------------------------------------------------------
// CFunSMusicView2::~CFunSMusicView2()
// destructor
// ---------------------------------------------------------
//
CMyleftCenterViewSearch::~CMyleftCenterViewSearch()
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
TUid CMyleftCenterViewSearch::Id() const
    {
    return KViewSearchId;
    }

// ---------------------------------------------------------
// CFunSMusicView2::HandleCommandL(TInt aCommand)
// takes care of view command handling
// ---------------------------------------------------------
//
void CMyleftCenterViewSearch::HandleCommandL(TInt aCommand)
    {   
    	CenterCore* iplaycore=CenterCore::GetInstance();

    switch ( aCommand )
        {
	case EMyleftCenterCmdAppSearchBefore:
		{
			if(iCurPage>1)
			{
				iCurPage--;
				iContainer->SetPageNum(iCurPage);
				iContainer->UpDateHotlist();
			}
            break;
		}
	  	break;
	case EMyleftCenterCmdAppSearchNext:
		{
			if(iCurPage<iResultPage)
			{
				iCurPage++;
				iContainer->SetPageNum(iCurPage);
				iContainer->UpDateHotlist();
			}
			else
			{
				if(iText.Length())
					iplaycore->iSinaMgr->LaunchSearchReadAppend(iText,iResultPage+1);
				
			}
            break;
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
// CFunSMusicView2::HandleClientRectChange()
// ---------------------------------------------------------
//
void CMyleftCenterViewSearch::HandleClientRectChange()
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
void CMyleftCenterViewSearch::DoActivateL(
   const TVwsViewId& /*aPrevViewId*/,TUid /*aCustomMessageId*/,
   const TDesC8& /*aCustomMessage*/)
    {
    if(CenterCore::GetInstance()->m_appUI->UiStyler()!=CenterCore::GetInstance()->m_appUI->iMyStyler2)
   	 AppUi()->SetUiStyler(CenterCore::GetInstance()->m_appUI->iMyStyler2);
    
    if (!iContainer)
        {
        iContainer = new (ELeave) CMyleftCenterContainerSearch;
        iContainer->SetMopParent(this);
        iContainer->ConstructL( this,ClientRect() );
	// iContainer->ConstructL( AppUi()->ApplicationRect());
	  
        AppUi()->AddToStackL( *this, iContainer );
        }
   }

// ---------------------------------------------------------
// CFunSMusicView2::DoDeactivate()
// 
// ---------------------------------------------------------
//
void CMyleftCenterViewSearch::DoDeactivate()
    {
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        }
    
    delete iContainer;
    iContainer = NULL;
    }



