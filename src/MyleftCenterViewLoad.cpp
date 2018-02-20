/*
============================================================================
 Name        : CFunSMusicView from FunSMusicView.h
 Author      : funinhand
 Version     :
 Copyright   : Your copyright notice
 Description : CFunSMusicView implementation
============================================================================
*/

// INCLUDE FILES
#include  <aknviewappui.h>
#include  <avkon.hrh>
#include  <MyleftCenter.rsg>
#include  "MyleftCenter.hrh"
#include  "MyleftCenterViewLoad.h"
#include "CenterCore.h"
#include "SinaInterfaceMgr.h"
#include "const.h"
#include <MyleftCenter.mbg>
#include "FunUIStart.h"
#include "MyleftCenterView.h"
//#include "FunSMusicViewlist.h"
// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CFunSMusicView::ConstructL(const TRect& aRect)
// EPOC two-phased constructor
// ---------------------------------------------------------
//
void CMyleftCenterViewLoad::ConstructL()
    {
    BaseConstructL( R_MYLEFTCENTER_VIEWLOAD,0 );
    }

// ---------------------------------------------------------
// CFunSMusicView::~CFunSMusicView()
// destructor
// ---------------------------------------------------------
//
CMyleftCenterViewLoad::~CMyleftCenterViewLoad()
    {
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        }

    delete iContainer;
    }

// ---------------------------------------------------------
// TUid CFunSMusicView::Id()
//
// ---------------------------------------------------------
//
TUid CMyleftCenterViewLoad::Id() const
    {
    return KViewLoadId;
    }

// ---------------------------------------------------------
// CFunSMusicView::HandleCommandL(TInt aCommand)
// takes care of view command handling
// ---------------------------------------------------------
//

void CMyleftCenterViewLoad::HandleCommandL(TInt aCommand)
    {   
    CenterCore*iCenterCore=CenterCore::GetInstance();
    switch ( aCommand )
        {
        default:
            {
            AppUi()->HandleCommandL( aCommand );
            break;
            }
        }
    }

// ---------------------------------------------------------
// CFunSMusicView::HandleClientRectChange()
// ---------------------------------------------------------
//
void CMyleftCenterViewLoad::HandleClientRectChange()
    {
    if ( iContainer )
        {
			iContainer->SetRect( AppUi()->ApplicationRect());
        }
    }

// ---------------------------------------------------------
// CFunSMusicView::DoActivateL(...)
// 
// ---------------------------------------------------------
//
void CMyleftCenterViewLoad::DoActivateL(
   const TVwsViewId& /*aPrevViewId*/,TUid /*aCustomMessageId*/,
   const TDesC8& /*aCustomMessage*/)
    {
    if (!iContainer)
        {
        iContainer = CMyleftCenterContainerLoad::NewL(this, AppUi()->ApplicationRect());
        iContainer->SetMopParent(this);
        AppUi()->AddToStackL( *this, iContainer );
        } 
   }

// ---------------------------------------------------------
// CFunSMusicView::DoDeactivate()
// 
// ---------------------------------------------------------
//
void CMyleftCenterViewLoad::DoDeactivate()
    {
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        }
    
    delete iContainer;
    iContainer = NULL;
    }


//=========== MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CTest4AppView3::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMyleftCenterContainerLoad* CMyleftCenterContainerLoad::NewL( CMyleftCenterViewLoad* parent, const TRect& aRect )
    {
    CMyleftCenterContainerLoad* self = CMyleftCenterContainerLoad::NewLC(parent, aRect );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CTest4AppView3::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMyleftCenterContainerLoad* CMyleftCenterContainerLoad::NewLC( CMyleftCenterViewLoad* parent, const TRect& aRect )
    {
    CMyleftCenterContainerLoad* self = new ( ELeave ) CMyleftCenterContainerLoad;
    CleanupStack::PushL( self );
    self->ConstructL( parent,aRect );
    return self;
    }


// ---------------------------------------------------------
// CFunSMusicContainer::ConstructL(const TRect& aRect)
// EPOC two phased constructor
// ---------------------------------------------------------
//
void CMyleftCenterContainerLoad::ConstructL( CMyleftCenterViewLoad* parent,const TRect& aRect)
    {
    	CreateWindowL();
	m_parent=parent	;
	iMainBackImg=FunUiLib::CreateBitmapL(KMusicMultiBitmapFilename,EMbmMyleftcenterLoading);
	if(!iMainBackImg)
		return;

/*
	TRect listRect= aRect;
	listRect.Move( 98, 55 );
	listRect.SetSize( TSize(44,43));
	TInt ids[] = {EMbmFunsmusicIcon_wait0, EMbmFunsmusicIcon_wait1,
				EMbmFunsmusicIcon_wait2, EMbmFunsmusicIcon_wait3,
				EMbmFunsmusicIcon_wait4, EMbmFunsmusicIcon_wait5};
	TInt masks[] = {EMbmFunsmusicIcon_wait0_mask, EMbmFunsmusicIcon_wait1_mask,
				EMbmFunsmusicIcon_wait2_mask, EMbmFunsmusicIcon_wait3_mask,
				EMbmFunsmusicIcon_wait4_mask, EMbmFunsmusicIcon_wait5_mask};

  	iMagAni=CFunImageAnimation::NewL(listRect,this,KMusicMultiBitmapFilename,ids,masks,6);
	iMagAni->MakeVisible( ETrue );
	*/
    SetRect(aRect);;
    ActivateL();

	if(iMagAni)
	  	iMagAni->StartAnimationL(500);

    }

// Destructor
CMyleftCenterContainerLoad::~CMyleftCenterContainerLoad()
{
	if(iMagAni)
	{
		if(iMagAni->IsRunning())
			iMagAni->StopAnimation();
		delete iMagAni;
	}
	if(iMainBackImg)
		delete iMainBackImg;
	FC_Log("CFunSMusicContainerLoad::~CFunSMusicContainerLoad() ");
}

// ---------------------------------------------------------
// CFunSMusicContainer::SizeChanged()
// Called by framework when the view size is changed
// ---------------------------------------------------------
//
void CMyleftCenterContainerLoad::SizeChanged()
    {
    }

// ---------------------------------------------------------
// CFunSMusicContainer::CountComponentControls() const
// ---------------------------------------------------------
//
TInt CMyleftCenterContainerLoad::CountComponentControls() const
    {
    if(iMagAni)
		return 1;
	return 0;
    }

// ---------------------------------------------------------
// CFunSMusicContainer::ComponentControl(TInt aIndex) const
// ---------------------------------------------------------
//
CCoeControl* CMyleftCenterContainerLoad::ComponentControl(TInt aIndex) const
    {
    switch ( aIndex )
        {
        case 0:
		if(iMagAni)
			return iMagAni;
		 return NULL;
        default:
            return NULL;
        }
    }

// ---------------------------------------------------------
// CFunSMusicContainer::Draw(const TRect& aRect) const
// ---------------------------------------------------------
//
void CMyleftCenterContainerLoad::Draw(const TRect& aRect) const
    {
    	CWindowGc& gc = SystemGc();
	gc.SetPenStyle( CGraphicsContext::ENullPen );
	gc.SetBrushColor( TRgb(1,1,1) );
	gc.SetBrushStyle( CGraphicsContext::ESolidBrush );

	if(m_parent->AppUi()->ActiveView()==m_parent)
	{
		gc.DrawRect( Rect() );
		TSize  iImgSize=iMainBackImg->SizeInPixels();
		TSize  iSize=Rect().Size();
		int x=(iSize.iWidth -iImgSize.iWidth)/2;
		gc.BitBlt( TPoint(x,iSize.iHeight/2-80), iMainBackImg );
	}

    }

TKeyResponse CMyleftCenterContainerLoad::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
{
	return EKeyWasNotConsumed;
}

void CMyleftCenterContainerLoad::StopAndCancle() 
{
//	CenterCore *iCenterCore=CenterCore::GetInstance();
	if(iMagAni)
	{
		if(iMagAni->IsRunning())
		{
			FC_Log("CFunSMusicContainerLoad::StopAndCancle() ");
			iMagAni->StopAnimation();
		}
	}
 }


