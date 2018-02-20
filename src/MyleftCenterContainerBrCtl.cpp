/*
============================================================================
 Name        : CFunSMusicContainer2 from FunSMusicContainer2.cpp
 Author      : funinhand
 Version     :
 Copyright   : Your copyright notice
 Description : Container control implementation
============================================================================
*/

// INCLUDE FILES
#include "MyleftCenterViewBrCtl.h"
#include "MyleftCenterContainerBrCtl.h"

#include <eiklabel.h>  // for example label control
#include <eikedwin.h> 
#include <eikrted.h> 
#include <BrCtlInterface.h>
#include <CHARCONV.H>
#include <gdi.h>
#include <bitdev.h>

#include <StringLoader.h> 
#include "CenterCore.h"
#include <MyleftCenter.mbg>
#include <MyleftCenter.rsg>
#include "const.h"
#include "SinaInterfaceMgr.h"
#include  "MyleftCenter.hrh"
#include  "MyleftCenterView.h"
// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CFunSMusicContainer2::ConstructL(const TRect& aRect)
// EPOC two phased constructor
// ---------------------------------------------------------
//
void CMyleftCenterContainerBrCtl::ConstructL(CMyleftCenterViewBrCtl *parent,const TRect& aRect)
    {
	CreateWindowL();
	SetRect(aRect);
	ActivateL();

	iplaycore=CenterCore::GetInstance();
	iparent=parent;

	 iCommandBase = TBrCtlDefs::ECommandIdBase;
	 iBrCtlCapabilities = TBrCtlDefs::ECapabilityDisplayScrollBar | TBrCtlDefs::ECapabilityLoadHttpFw;

	iBrCtlSpecialLoadObserver=CBrCtlSampleAppSpecialLoadObserver::NewL();

	 TRect rect(TPoint(0,0), Size());
    	iBrCtlInterface = CreateBrowserControlL( this, 
        rect, 
        iBrCtlCapabilities,
        iCommandBase, 
        NULL, 
        NULL, 
        iBrCtlSpecialLoadObserver,
        NULL,
        NULL);
		
	iBrCtlInterface->SetBrowserSettingL( TBrCtlDefs::ESettingsApId, iplaycore->_http_iap_id);
 	 iBrCtlInterface->ActivateL();

	 _LIT(KUrl, "http://mobile.myleft.org/");
	//	iContainer->iBrCtlInterface->SetBrowserSettingL( TBrCtlDefs::ESettingsApId, iplaycore->_http_iap_id);
    	iBrCtlInterface->LoadUrlL( KUrl );


    }

// Destructor
CMyleftCenterContainerBrCtl::~CMyleftCenterContainerBrCtl()
{
	if(iBrCtlSpecialLoadObserver)
		delete iBrCtlSpecialLoadObserver;
	if(iBrCtlInterface)
		delete iBrCtlInterface;
}

// ---------------------------------------------------------
// CFunSMusicContainer2::SizeChanged()
// Called by framework when the view size is changed
// ---------------------------------------------------------
//
void CMyleftCenterContainerBrCtl::SizeChanged()
{
   if (iBrCtlInterface)
    {
  	 iBrCtlInterface->SetRect(Rect());
    }

}

// ---------------------------------------------------------
// CFunSMusicContainer2::CountComponentControls() const
// ---------------------------------------------------------
//
TInt CMyleftCenterContainerBrCtl::CountComponentControls() const
{
	if (iBrCtlInterface)
	    return 1;
	return 0;
}

// ---------------------------------------------------------
// CFunSMusicContainer2::ComponentControl(TInt aIndex) const
// ---------------------------------------------------------
//
CCoeControl* CMyleftCenterContainerBrCtl::ComponentControl(TInt aIndex) const
    {
    switch ( aIndex )
        {
        case 0:
			return iBrCtlInterface;
        default:
            return NULL;
        }
    }


// ---------------------------------------------------------
// CFunSMusicContainer2::Draw(const TRect& aRect) const
// ---------------------------------------------------------
//
void CMyleftCenterContainerBrCtl::Draw(const TRect& aRect) const
{
	
}

// ---------------------------------------------------------
// CFunSMusicContainer2::HandleControlEventL(
//     CCoeControl* aControl,TCoeEvent aEventType)
// ---------------------------------------------------------
//
void CMyleftCenterContainerBrCtl::HandleControlEventL(
    CCoeControl* /*aControl*/,TCoeEvent /*aEventType*/)
    {
    // TODO: Add your control event handler code here
    }

TKeyResponse CMyleftCenterContainerBrCtl::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
{
    if (iBrCtlInterface)
        {
        	return iBrCtlInterface->OfferKeyEventL(aKeyEvent, aType);
        }
    return EKeyWasConsumed;
}

