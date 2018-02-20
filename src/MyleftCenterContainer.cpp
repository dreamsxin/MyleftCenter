/*
============================================================================
 Name        : CMyleftCenterContainer from MyleftCenterContainer.h
 Author      : yilizheng
 Version     :
 Copyright   : MyleftCenter
 Description : Container control implementation
============================================================================
*/

// INCLUDE FILES
#include "MyleftCenterContainer.h"

#include <eiklabel.h>  // for example label control
#include <MyleftCenter.mbg>
#include  <MyleftCenter.rsg>
#include "const.h"
#include "FunUIStart.h"
#include "CenterCore.h"


#include "MyleftCenterView2.h"
#include  "MyleftCenterViewLoad.h"
#include  "MyleftCenterViewSearch.h"
#include  "MyleftCenterViewReadList.h"
#include  "MyleftCenterViewDownLoad.h"
#include  "MyleftCenterViewReadSet.h"
#include  "MyleftCenterViewMusic.h"
#include  "MyleftCenterViewMusicList.h"
#include  "MyleftCenterViewMusicSearch.h"
#include  "MyleftCenterViewChatRoom.h"
#include  "MyleftCenterContainerChatRoom.h"
#ifdef _S60_3RD_
#include  "MyleftCenterViewBrCtl.h"
#include  "MyleftCenterContainerBrCtl.h"
#endif

#include "chatroomtask.h"
// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMyleftCenterContainer::ConstructL(const TRect& aRect)
// EPOC two phased constructor
// ---------------------------------------------------------
//
void CMyleftCenterContainer::ConstructL(const TRect& aRect)
    {
    CreateWindowL();

	iFocusID=CenterCore::GetInstance()->m_mainFouceId;
	iMainBackImg=FunUiLib::CreateBitmapL(KMusicMultiBitmapFilename,EMbmMyleftcenterMain_back);
	if(!iMainBackImg)
		return;

	iMainImg=FunUiLib::CreateBitmapL(KMusicMultiBitmapFilename,EMbmMyleftcenterMain);
	if(!iMainBackImg)
	{
		delete iMainBackImg;
		return;
	}

 	CFbsBitmap*  iTempnImg;
	m_iconlist= new (ELeave) CArrayFixFlat<CFbsBitmap*> (4);

	iTempnImg=FunUiLib::CreateBitmapL(KMusicMultiBitmapFilename,EMbmMyleftcenterMusic);
	m_iconlist->AppendL(iTempnImg);
	iTempnImg=FunUiLib::CreateBitmapL(KMusicMultiBitmapFilename,EMbmMyleftcenterRead);
	m_iconlist->AppendL(iTempnImg);
	iTempnImg=FunUiLib::CreateBitmapL(KMusicMultiBitmapFilename,EMbmMyleftcenterChat);
	m_iconlist->AppendL(iTempnImg);
	iTempnImg=FunUiLib::CreateBitmapL(KMusicMultiBitmapFilename,EMbmMyleftcenterDownload);
	m_iconlist->AppendL(iTempnImg);
	iTempnImg=FunUiLib::CreateBitmapL(KMusicMultiBitmapFilename,EMbmMyleftcenterBlog);
	m_iconlist->AppendL(iTempnImg);
	iTempnImg=FunUiLib::CreateBitmapL(KMusicMultiBitmapFilename,EMbmMyleftcenterUsercenter);
	m_iconlist->AppendL(iTempnImg);
	iTempnImg=FunUiLib::CreateBitmapL(KMusicMultiBitmapFilename,EMbmMyleftcenterSetting);
	m_iconlist->AppendL(iTempnImg);
	/*
	iTempnImg=FunUiLib::CreateBitmapL(KMusicMultiBitmapFilename,EMbmMyleftcenterNewspaper);
	m_iconlist->AppendL(iTempnImg);	
	iTempnImg=FunUiLib::CreateBitmapL(KMusicMultiBitmapFilename,EMbmMyleftcenterMovie);
	m_iconlist->AppendL(iTempnImg);
	iTempnImg=FunUiLib::CreateBitmapL(KMusicMultiBitmapFilename,EMbmMyleftcenterBookmark);
	m_iconlist->AppendL(iTempnImg);
	iTempnImg=FunUiLib::CreateBitmapL(KMusicMultiBitmapFilename,EMbmMyleftcenterDownload);
	m_iconlist->AppendL(iTempnImg);
	iTempnImg=FunUiLib::CreateBitmapL(KMusicMultiBitmapFilename,EMbmMyleftcenterUsercenter);
	m_iconlist->AppendL(iTempnImg);
*/

//	iBottomImg=FunUiLib::LoadImageFromFileL(_L("C:\\scrbottom.png"));

	iTimer = CFunTimeOutTimer::NewL( 0, *this );
	iTimer->After(1000000);

	if(CenterCore::GetInstance()->GetTaskInfo().Length())
	{
		const CFont* normalFont = LatinPlain12();
		iCurStartX=aRect.Width();
		iSrcollWidth=normalFont->TextWidthInPixels(CenterCore::GetInstance()->GetTaskInfo());
	}
//	SetupScrollTask(aInfomation);

    SetRect(aRect);
    ActivateL();
    }

// Destructor
CMyleftCenterContainer::~CMyleftCenterContainer()
{
    	if( iTimer && iTimer->IsActive() )
		iTimer->Cancel();
	delete iTimer;

	if(iBottomImg)
		delete iBottomImg;
	
   	if(iMainBackImg)
		delete iMainBackImg;

	if(iMainImg)
		delete iMainImg;

	if(m_iconlist)
	{
		for(TInt i=0; i<m_iconlist->Count(); i++ )
		{
			CFbsBitmap* iTempnImg =  m_iconlist->At(i);
			if(iTempnImg)
				delete iTempnImg;
		}

	//	m_iconlist->Reset();
		delete m_iconlist;
	}
}

// ---------------------------------------------------------
// CMyleftCenterContainer::SizeChanged()
// Called by framework when the view size is changed
// ---------------------------------------------------------
//
void CMyleftCenterContainer::SizeChanged()
{
	TRect iRect= Rect();
	TInt width=iRect.Width()-8;
	iLineCount= width/KMainIcon_WH;
	iIconGap=(width-KMainIcon_WH*iLineCount)/(2*iLineCount);
}

// ---------------------------------------------------------
// CMyleftCenterContainer::CountComponentControls() const
// ---------------------------------------------------------
//
TInt CMyleftCenterContainer::CountComponentControls() const
    {
    return 0; // return nbr of controls inside this container
    }

// ---------------------------------------------------------
// CMyleftCenterContainer::ComponentControl(TInt aIndex) const
// ---------------------------------------------------------
//
CCoeControl* CMyleftCenterContainer::ComponentControl(TInt aIndex) const
    {
    switch ( aIndex )
        {
        case 0:
        case 1:
        default:
            return NULL;
        }
    }

 void CMyleftCenterContainer::MftonTimerExpired()
 {
 	if(!iBottomImg)
	{
		CWsScreenDevice* screenDev = CCoeEnv::Static()->ScreenDevice();
		iBottomImg = new (ELeave) CFbsBitmap;
		if(iBottomImg->Create( TSize(Rect().Width(),20),screenDev->DisplayMode() )!=KErrNone )
		{
			FC_Log("iBottomImg->Create  Failed");
			delete iBottomImg;
			iBottomImg=NULL;
		}
		if( screenDev->CopyScreenToBitmap( iBottomImg , TRect(TPoint(0,Rect().Width()-19),TSize(Rect().Width(),20)))!=KErrNone )// use bitmap as appropriate, for example save it to file
		{
			FC_Log("screenDev->CopyScreenToBitmap  Failed");
			delete iBottomImg;
			iBottomImg=NULL;
		}
	}
/*
	if(CenterCore::GetInstance()->Download_ListDownloadCheck()!=-1)
	{
		CenterCore::GetInstance()->ReSetTaskInfo();
	}
*/
	TInt timeIntev=500000;
	 iCurStartX-=15;
	if(iCurStartX<(4-iSrcollWidth))
	{
		iCurStartX=Rect().Width();
		timeIntev=700000*10;
	}

	DrawNow();
	//ShowScrollTask();
	if(CenterCore::GetInstance()->GetTaskInfo().Length())
	{
		iTimer->After( timeIntev );
	}
 }
// ---------------------------------------------------------
// CMyleftCenterContainer::Draw(const TRect& aRect) const
// ---------------------------------------------------------
//
void CMyleftCenterContainer::Draw(const TRect& aRect) const
{
	CWindowGc& gc = SystemGc();

	if(CenterCore::GetInstance()->GetTaskInfo().Length())
	{
		const CFont* normalFont = LatinPlain12();
		gc.UseFont( normalFont );
		gc.SetPenStyle( CGraphicsContext::ESolidPen );
		gc.SetPenColor(TRgb(1,254,1));//yellow
		gc.DrawText( CenterCore::GetInstance()->GetTaskInfo(), TPoint(iCurStartX,Rect().Height()-4));

		gc.DiscardFont();
	}

	gc.SetPenStyle( CGraphicsContext::ENullPen );
	gc.SetPenColor( KRgbRect);
	gc.SetBrushColor( KRgbContainerDeep);
	gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
	gc.DrawRect( Rect() );
	
	gc.SetPenStyle( CGraphicsContext::ESolidPen );
	gc.SetBrushColor( KRgbContainer);
	//gc.SetBrushStyle( CGraphicsContext::ENullBrush );
	TRect rcet=Rect();
	rcet.Move( 2, 2 );
	rcet.Resize( -4, -4);
	gc.DrawRect( rcet );
	rcet.Move( 1, 1 );
	rcet.Resize( -2, -2);
	gc.DrawRect( rcet );

	gc.SetPenColor( TRgb(1,1,1));
	TPoint leftbutpos(rcet.iTl.iX,rcet.iTl.iY+rcet.Height()+1);
	TPoint Toppos(rcet.iTl.iX+rcet.Width()+1,rcet.iTl.iY);
	TPoint butpos(rcet.iBr);
	butpos.iX++;butpos.iY++;
	gc.DrawLine( leftbutpos,butpos );
	gc.DrawLine( Toppos,butpos );
	
	gc.SetBrushStyle( CGraphicsContext::ENullBrush );
	rcet.Move( 1, 1 );
	rcet.Resize( -2, -2);
	
//	gc.BitBlt( Rect().iTl, iMainBackImg );
//	gc.BitBlt( Rect().iTl, iMainImg );
	TPoint startpos(rcet.iTl);
	
	for(TInt i=0;i<KMain_Icon_Num;i++)
	{
		TPoint startpos(rcet.iTl);
		startpos.iY+=5;
		
		if(i>=iLineCount && i<iLineCount*2)
		{
			startpos.iX+=(2*iIconGap+KMainIcon_WH)*(i-iLineCount)+iIconGap;
			startpos.iY+=KMainIcon_WH+10;
		}
		else if(i<iLineCount)
		{
			startpos.iX+=(2*iIconGap+KMainIcon_WH)*i+iIconGap;
		}
		else 
		{
			startpos.iX+=(2*iIconGap+KMainIcon_WH)*(i-iLineCount*2)+iIconGap;
			startpos.iY+=KMainIcon_WH*2+15;
		}
		
		gc.BitBlt( startpos, m_iconlist->At(i));
		if(i==iFocusID)
		{
			TRect fourceRcet(startpos,TSize(KMainIcon_WH,KMainIcon_WH));
			gc.DrawRect( fourceRcet );
		}
	}
	
}

void CMyleftCenterContainer:: ShowScrollTask()
{
	//FC_Log("CMyleftCenterContainer::ShowScrollTask() ");	
	ActivateGc();
	
	CWindowGc& gc = SystemGc();

	if(iBottomImg)
	{
	//	TRect bmpBackPieceRect(TPoint(0,0),iBottomImg->SizeInPixels());
	//	gc.BitBltMasked(TPoint(0,Rect().Height()-20),iBottomImg,bmpBackPieceRect,iBottomMaskImg,EFalse);
		//FC_Log("CMyleftCenterContainer::ShowScrollTask()  iBottomImg");	
		gc.BitBlt( TPoint(0,Rect().Height()-20), iBottomImg);

	}
	else
	{
	//	FC_Log("CMyleftCenterContainer::ShowScrollTask()  DrawRect");	
		gc.SetBrushColor( KRgbContainer);
		gc.SetPenStyle( CGraphicsContext::ENullPen );
		gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
		gc.DrawRect( TRect(TPoint(0,Rect().Height()-20),TPoint(Rect().Width(),Rect().Height())) );
	}

	const CFont* normalFont = LatinPlain12();
	gc.UseFont( normalFont );
	gc.SetPenStyle( CGraphicsContext::ESolidPen );
	gc.SetPenColor(TRgb(1,1,1));//yellow
	gc.DrawText( CenterCore::GetInstance()->GetTaskInfo(), TPoint(iCurStartX,Rect().Height()));

	gc.DiscardFont();

	DeactivateGc();
}

void CMyleftCenterContainer:: SetupScrollTask(const TDesC& _title)
{
	if( iTimer && iTimer->IsActive() )
		iTimer->Cancel();
	iCurStartX=Rect().Width();

	const CFont* normalFont = LatinPlain12();
	iSrcollWidth=normalFont->TextWidthInPixels(_title);

	if(_title.Length())
		iTimer->After(1000000);
}


TKeyResponse CMyleftCenterContainer::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
{
	TKeyResponse ret=EKeyWasNotConsumed;
		
	switch(aKeyEvent.iCode)
	{
	case  EKeyDevice3:
		{
			CenterCore * icenterCore=CenterCore::GetInstance();
			if(iFocusID==1)//READ
			{
				icenterCore->m_appUI->TitleBar()->ResetTitleTabMenu();
			//	icenterCore->m_appUI->TitleBar()->SetTabMenuSelBgColor(KRgbContainer);
			//	icenterCore->m_appUI->TitleBar()->AddTitleTabMenu(KView2Id,	icenterCore->TextResource(R_TEXT_READ));
			//	icenterCore->m_appUI->TitleBar()->AddTitleTabMenu(KViewReadListId,icenterCore->TextResource(R_TEXT_LIST));
			//	icenterCore->m_appUI->TitleBar()->AddTitleTabMenu(KViewSearchId,icenterCore->TextResource(R_TEXT_VIEW_SEARCH_SONG));//r_text_view_search_song

				icenterCore->m_appUI->ActivateLocalViewL(KView2Id);
			}
			else if(iFocusID==0) //MUSIC
			{
				icenterCore->m_appUI->TitleBar()->ResetTitleTabMenu();
		//		icenterCore->m_appUI->TitleBar()->AddTitleTabMenu(KViewMusicId,	icenterCore->TextResource(R_TEXT_READ));
		//		icenterCore->m_appUI->TitleBar()->AddTitleTabMenu(KViewMusicListId,icenterCore->TextResource(R_TEXT_LIST));
		//		icenterCore->m_appUI->TitleBar()->AddTitleTabMenu(KViewMusicSearchId,icenterCore->TextResource(R_TEXT_VIEW_SEARCH_SONG));

				icenterCore->m_appUI->ActivateLocalViewL(KViewMusicId);
			}
			else if(iFocusID==3)   //download
			{
				icenterCore->m_appUI->TitleBar()->ResetTitleTabMenu();
				icenterCore->m_appUI->TitleBar()->SetTitle(CenterCore::GetInstance()->TextResource(R_TEXT_POPMENU_DOWNLOAD_MANAGER));
				icenterCore->m_appUI->TitleBar()->SetTextColor(TRgb(254,254,254));
			//	icenterCore->m_appUI->ActivateLocalViewL(KViewReadListId);
				icenterCore->m_appUI->ActivateLocalViewL(KViewDownLoadId);
			}
			else if(iFocusID==7)   //wap not used
			{
#ifdef _S60_3RD_
				icenterCore->m_appUI->ActivateLocalViewL(KViewBrCtlId);
#endif
			}
			else if(iFocusID==6)   //setting
			{
				icenterCore->m_appUI->viewReadSet->setType=0;
				icenterCore->m_appUI->ActivateLocalViewL(KViewReadSetId);
			}
			else if(iFocusID==2)   //chartroom test
			{
				icenterCore->m_appUI->TitleBar()->ResetTitleTabMenu();
				icenterCore->ChatR_LoginChatRoom();
			}
			else if(iFocusID==3)   //chartroom test
			{
				if(icenterCore->iChartRData)
				{
					  icenterCore->iChartRData->SendMessage("0", "helo-I am E63-");
				}
			}

			ret=EKeyWasConsumed;
		}
		break;
	case EKeyUpArrow:
		{
			TInt id=iFocusID-iLineCount;
			if(0<=id &&id <KMain_Icon_Num)
				iFocusID=id;
			
			DrawNow();
			ret=EKeyWasConsumed;
		}
		break;
	case EKeyLeftArrow:	
		{
			iFocusID--;
			if(iFocusID<0)
				iFocusID=KMain_Icon_Num-1;
			DrawNow();
			ret=EKeyWasConsumed;
		}
		break;
	case EKeyDownArrow:
		{
			TInt id=iFocusID+iLineCount;
			if(0<=id &&id <KMain_Icon_Num)
				iFocusID=id;

			DrawNow();
			ret=EKeyWasConsumed;
		}
		break;
	case EKeyRightArrow:
		{
			iFocusID++;
			if(iFocusID>KMain_Icon_Num-1)
				iFocusID=0;
			DrawNow();
			ret=EKeyWasConsumed;
		}
		break;
	default:
		break;

	}
		
	return ret;
}
