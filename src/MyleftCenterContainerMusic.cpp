/*
============================================================================
 Name        : CFunSMusicContainer from FunSMusicContainer.h
 Author      : funinhand
 Version     :
 Copyright   : Your copyright notice
 Description : Container control implementation
============================================================================
*/

// INCLUDE FILES
#include "MyleftCenterContainerMusic.h"

#include <eiklabel.h>  // for example label control
#include "const.h"
#include "funImage.h"
#include "funButton.h"
#include "CenterCore.h"
#include <MyleftCenter.mbg>
#include <MyleftCenter.rsg>
#include "FunUIStart.h"
#include "LrcSubtitle.h"
#include "FunLyricLabel.h"
#include  "MyleftCenterViewMusic.h"
#include "SinaInterfaceMgr.h"
//=========== MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CTest4AppView3::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMyleftCenterContainerMusic* CMyleftCenterContainerMusic::NewL( const TRect& aRect )
    {
    CMyleftCenterContainerMusic* self = CMyleftCenterContainerMusic::NewLC( aRect );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CTest4AppView3::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMyleftCenterContainerMusic* CMyleftCenterContainerMusic::NewLC( const TRect& aRect )
    {
    CMyleftCenterContainerMusic* self = new ( ELeave ) CMyleftCenterContainerMusic;
    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    return self;
    }


// ---------------------------------------------------------
// CFunSMusicContainer::ConstructL(const TRect& aRect)
// EPOC two phased constructor
// ---------------------------------------------------------
//
void CMyleftCenterContainerMusic::ConstructL(const TRect& aRect)
    {
    	CreateWindowL();
		
	iplaycore=CenterCore::GetInstance();
	iShowMode=iplaycore->m_appUI->viewMusic->iShowMode;
	iSelIndex=iplaycore->m_appUI->viewMusic->GetCurSelindex();
	iCurretPlayIndex=iplaycore->Music_playerCurrentIndex();
	iLrcShowId=iplaycore->m_appUI->viewMusic->iLrcShowId;
#ifdef _USE_PLAY_BACK_IMG	
	iMainBackImg=FunUiLib::CreateBitmapL(KMusicMultiBitmapFilename,EMbmMyleftcenterPlay_back);
	if(!iMainBackImg)
		return;
#endif
	iPrevImg=FunUiLib::CreateBitmapL(KMusicMultiBitmapFilename,EMbmMyleftcenterPrev);
	if(!iPrevImg)
	{
		Destroy();
		return;
	}

	iStopImg=FunUiLib::CreateBitmapL(KMusicMultiBitmapFilename,EMbmMyleftcenterStop);
	if(!iStopImg)
	{
		Destroy();
		return;
	}
	
	iPlayImg=FunUiLib::CreateBitmapL(KMusicMultiBitmapFilename,EMbmMyleftcenterDoplay);
	if(!iPlayImg)
	{
		Destroy();
		return;
	}
	
	iNextImg=FunUiLib::CreateBitmapL(KMusicMultiBitmapFilename,EMbmMyleftcenterNext);
	if(!iNextImg)
	{
		Destroy();
		return;
	}

#ifndef _USE_PLAY_BACK_IMG	
	iSingerImg=FunUiLib::CreateBitmapL(KMusicMultiBitmapFilename,EMbmMyleftcenterSigner);
	if(!iSingerImg)
	{
		Destroy();
		return;
	}

	iVolumeImg=FunUiLib::CreateBitmapL(KMusicMultiBitmapFilename,EMbmMyleftcenterVolume);
	if(!iVolumeImg)
	{
		Destroy();
		return;
	}

	iPrevFImg=FunUiLib::CreateBitmapL(KMusicMultiBitmapFilename,EMbmMyleftcenterPrev_sel);
	if(!iPrevFImg)
	{
		Destroy();
		return;
	}

	iStopFImg=FunUiLib::CreateBitmapL(KMusicMultiBitmapFilename,EMbmMyleftcenterStop_sel);
	if(!iStopFImg)
	{
		Destroy();
		return;
	}
	
	iPlayFImg=FunUiLib::CreateBitmapL(KMusicMultiBitmapFilename,EMbmMyleftcenterPlay_sel);
	if(!iPlayFImg)
	{
		Destroy();
		return;
	}
	
	iNextFImg=FunUiLib::CreateBitmapL(KMusicMultiBitmapFilename,EMbmMyleftcenterNext_sel);
	if(!iNextFImg)
	{
		Destroy();
		return;
	}
#endif
	songLrcLib=CFunLabel::NewL( aRect, this, KNullDesC, FunUiLib::NormalFont());	
	songLrcLib->SetTransparent( ETrue );
	songLrcLib->SetTextAlign( CGraphicsContext::ECenter );
	songLrcLib->SetTextColor(KRgbRect);

	songNameLib=CFunLabel::NewL( aRect, this, KNullDesC, FunUiLib::NormalFont());	
	songNameLib->SetTransparent( ETrue );
	songNameLib->SetTextAlign( CGraphicsContext::ECenter );
	
	songTimeLib=CFunLabel::NewL(  aRect, this, KNullDesC, FunUiLib::SmallFont());
	songTimeLib->SetTextColor(TRgb(245,245,245));
	songTimeLib->SetTextAlign( CGraphicsContext::ECenter );
	songTimeLib->SetTransparent( ETrue );

	streamPercentLib=CFunLabel::NewL( aRect, this, KNullDesC, FunUiLib::SmallFont());
	streamPercentLib->SetTextColor(TRgb(245,245,245));
	streamPercentLib->SetTextAlign( CGraphicsContext::ECenter );
	streamPercentLib->SetTransparent( ETrue );

	TInt ids[] = {EMbmMyleftcenterSelect};
	TInt masks[] = {EMbmMyleftcenterSelect_mask};	
	iList = CFunList::NewL( aRect, this, FunUiLib::SmallFont() );
	iList->SetObserver(this);
	iList->BackDrawer()->SetBackgroundColor(  TRgb(245,245,245) );
//	iList->BackDrawer()->SetBackgroundColor( TRgb(247,237,217) );
	iList->ActivedBackDrawer()->SetBackgroundColor( TRgb(248,227,133) );
	iList->ActivedBackDrawer()->SetNoBorder();
	iList->SetTextColors( TRgb(0,160,0), TRgb(0,160,0),  TRgb(0,160,0));
//	iList->SetTextColors( KRgbRect, KRgbRect, KRgbRect);
	iList->SetDisplayNumber(1);
	iList->SetupGraphicIcons(KMusicMultiBitmapFilename,ids,masks,1,0);
	SetupListItemsL();

	TRect LyricRect= aRect;
	LyricRect.Move( 1, KTitleHight_Len );
	LyricRect.Resize( -2, -KTitleHight_Len-KPlayContrl_PicW );
	iLyricLabel =new CFunLyricLabel();
	iLyricLabel->ConstructL(this,  LyricRect, FunUiLib::NormalFont(),KRgbText, KRgbRect, TRgb(245,245,245));
//	iLyricLabel->ConstructL(this,  LyricRect, FunUiLib::SmallFont(),TRgb(1,1,1), KRgbRect, KRgbContainer);
	iLyricLabel->SetContainerWindowL( *this );
	iLyricLabel->SetNoLyricText(iplaycore->TextResource(R_TEXT_VIEW_NO_MANU_LRC));

	MflloGetSubtitle();
	GetSingerIcon();
	if(iplaycore->m_musicStatus)
	{
		TUint aCurtime=0;
		TBuf<256> name;	
		_LIT(KString,"%S %S");

		if(iSubtitle )
		{
			aCurtime=iSubtitle->GetCurrentFrameIndex();
			if(iLyricLabel && (iShowMode==ShowModeLRC))
			{
				iCurtime=aCurtime;
				iLyricLabel->UpdateLyricDisplay(EFalse, EFalse);
			}
			else
			{
				TUint frameLen=0;
				TBuf<KMaxLineCharNumber> frameBuf;
				
				 iSubtitle->GetTextBufByIndex(aCurtime, (TUint16*)frameBuf.Ptr(), frameLen, KMaxLineCharNumber );
				 frameBuf.SetLength(frameLen);			  
				songLrcLib->ChangeTextL(frameBuf,EFalse);
			}			
		}
		else //no subtitle
		{
		
		/*	if(iShowMode==ShowModeList)
				songLrcLib->ChangeTextL(iplaycore->TextResource(R_TEXT_VIEW_NO_LRC),EFalse);
			else if(iShowMode==ShowModeSreach)
				songLrcLib->ChangeTextL(iplaycore->TextResource(R_TEXT_VIEW_DOWNLOAD_LRC),EFalse);
			else
				songLrcLib->ChangeTextL(KNullDesC,EFalse);*/
			if(iLrcShowId==ELrcLibShow_NULL)
				songLrcLib->ChangeTextL(KNullDesC,EFalse);
			else if(iLrcShowId==ELrcLibShow_NoAutoLRC)
				songLrcLib->ChangeTextL(iplaycore->TextResource(R_TEXT_VIEW_NO_AUTO_LRC),EFalse);
			else if(iLrcShowId==ELrcLibShow_NoManuLRC)
				songLrcLib->ChangeTextL(iplaycore->TextResource(R_TEXT_VIEW_NO_MANU_LRC),EFalse);
			else if(iLrcShowId==ELrcLibShow_HaveLRC)
				songLrcLib->ChangeTextL(iplaycore->TextResource(R_TEXT_VIEW_DOWNLOAD_LRC),EFalse);
			else if(iLrcShowId==ELrcLibShow_PayNotify)
				songLrcLib->ChangeTextL(iplaycore->TextResource(R_TEXT_VIEW_PAY_NOTIFY1),EFalse);
		}

		if(iplaycore->curPlayer)
		{
			if(iplaycore->curPlayer->name.Length() && iplaycore->curPlayer->singer.Length())
				name.Format(KString(),&iplaycore->curPlayer->name,&iplaycore->curPlayer->singer);
			songNameLib->ChangeTextL(name,EFalse);
		}

	}	
	
	iTimer = CFunTimeOutTimer::NewL( 0, *this );
	iTimer->After( 300000 );

    SetRect(aRect);
    ActivateL();

}

// Destructor
CMyleftCenterContainerMusic::~CMyleftCenterContainerMusic()
{
	Destroy();
}


// ---------------------------------------------------------
// CFunSMusicContainer::SizeChanged()
// Called by framework when the view size is changed
// ---------------------------------------------------------
//
void CMyleftCenterContainerMusic::SizeChanged()
    {
	if( iList )
	{
	
		TRect listRect= Rect();
		listRect.Move( 1, KTitleHight_Len );
		listRect.Resize( -2, -KTitleHight_Len-KPlayContrl_PicW );
		iList->SetExtent( listRect.iTl, listRect.Size() );
		
	}
	if( iLyricLabel )
	{
	
		TRect listRect= Rect();
		listRect.Move( 1, 0 );
		listRect.Resize( -2, -KPlayContrl_PicW );
		iLyricLabel->SetExtent( listRect.iTl, listRect.Size() );
		
	}

	if( songLrcLib )
	{	
		TRect lrcRect= Rect();
		lrcRect.Move( 1, 0 );
		lrcRect.Resize( -2, -lrcRect.Height()+KPlayContrl_lrcH);
		songLrcLib->SetExtent( lrcRect.iTl, lrcRect.Size() );		
	}

	contrlRect= Rect();
	TInt moveY=Rect().Height()-KPlayContrl_PicW;
	contrlRect.Move( 1, moveY );
	contrlRect.Resize( -2-KPlayContrl_PicW, -moveY);

	if(songNameLib)
	{
		songNameLib->SetRect(TRect(TPoint (contrlRect.iTl.iX,contrlRect.iTl.iY),TPoint (contrlRect.iBr.iX,(contrlRect.iBr.iY-55))));
	}
	if(songTimeLib)
	{
		songTimeLib->SetRect(TRect(TPoint (contrlRect.iTl.iX,(contrlRect.iTl.iY+25)),TPoint ((contrlRect.iBr.iX-60),(contrlRect.iBr.iY-30))));
	}
	if(streamPercentLib)
	{
		streamPercentLib->SetRect(TRect(TPoint ((contrlRect.iTl.iX+100),(contrlRect.iTl.iY+25)),TPoint (contrlRect.iBr.iX,(contrlRect.iBr.iY-30))));
	}
	 DrawNow();
    }

// ---------------------------------------------------------
// CFunSMusicContainer::CountComponentControls() const
// ---------------------------------------------------------
//
TInt CMyleftCenterContainerMusic::CountComponentControls() const
{
	TInt count=4;
	if(iShowMode!=ShowModeLRC)
		count++;  //add songLrcLib
    return count; // return nbr of controls inside this container
}

// ---------------------------------------------------------
// CFunSMusicContainer::ComponentControl(TInt aIndex) const
// ---------------------------------------------------------
//
CCoeControl* CMyleftCenterContainerMusic::ComponentControl(TInt aIndex) const
    {
    switch ( aIndex )
        {
        
        case 0:
		if(iShowMode!=ShowModeLRC)
			return iList;
		else
			return iLyricLabel;
        case 1:
           return songNameLib;
        case 2:
           return songTimeLib;
        case 3:
           return streamPercentLib;
	case 4:
		return songLrcLib;
        default:
            return NULL;
        }
    }

// ---------------------------------------------------------
// CFunSMusicContainer::Draw(const TRect& aRect) const
// ---------------------------------------------------------
//
void CMyleftCenterContainerMusic::Draw(const TRect& aRect) const
    {

    CWindowGc& gc = SystemGc();
	TRect ContainerRect= Rect();
//	FC_Logsp("CFunSMusicContainer::Draw::iTl.iY=%d,%d,",ContainerRect.iTl.iY,ContainerRect.iBr.iY);
#ifndef _USE_PLAY_BACK_IMG	
	gc.SetPenStyle( CGraphicsContext::ENullPen );
	gc.SetBrushColor( TRgb(245,245,245) );
	gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
    	gc.DrawRect( ContainerRect );

	gc.SetPenStyle( CGraphicsContext::ESolidPen );
	gc.SetPenColor(KRgbRect);
	gc.DrawLine(TPoint(0,0),TPoint(0,ContainerRect.Height()));
	gc.DrawLine(TPoint(ContainerRect.Width()-1,0),TPoint(ContainerRect.Width()-1,ContainerRect.Height()));
	gc.DrawLine(TPoint(0,KPlayContrl_lrcH-1),TPoint(ContainerRect.Width(),KPlayContrl_lrcH-1));
	
	gc.SetPenColor(KRgbContainerDeep);
	gc.SetBrushColor( KRgbContainerDeep);
    	gc.DrawRect( contrlRect );

	if(iSingerIcon)
		gc.BitBlt( TPoint(ContainerRect.Width()-KPlayContrl_PicW-1,ContainerRect.Height()-KPlayContrl_PicW), iSingerIcon );
   	else if(iSingerImg)
		gc.BitBlt( TPoint(ContainerRect.Width()-KPlayContrl_PicW-1,ContainerRect.Height()-KPlayContrl_PicW), iSingerImg );

   	if(iVolumeImg)
		gc.BitBlt( TPoint(12,ContainerRect.Height()-32), iVolumeImg );

	gc.SetPenColor( TRgb(245,245,245) );
	gc.DrawLine(TPoint(26,ContainerRect.Height()-29),TPoint(26+10*11,ContainerRect.Height()-29));  //volume line
#else
	gc.BitBlt( TPoint(0,0), iMainBackImg );
#endif
	int volume=iplaycore->m_musicSetting.volume;
	gc.SetPenStyle( CGraphicsContext::ENullPen );
	gc.SetBrushColor( TRgb(245,245,245) );
	gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
	TRect volumeRect= TRect(TPoint(26,ContainerRect.Height()-30),TSize(0,3));
	volumeRect.Resize( +(volume*11), 0 );
    	gc.DrawRect( volumeRect );

	TSize  iImgSize=iPlayImg->SizeInPixels();
	TInt buttenY=ContainerRect.Height()-1-iImgSize.iHeight;
	
#ifndef _USE_PLAY_BACK_IMG		
	int startX=4;
	gc.BitBlt( TPoint(startX,buttenY), iPrevFImg);
	
	startX=4+iImgSize.iWidth+1;
	gc.BitBlt( TPoint(startX,buttenY), iPlayFImg);
	
	startX=4+iImgSize.iWidth*2+2;
	gc.BitBlt( TPoint(startX,buttenY), iStopFImg);
	
	startX=4+iImgSize.iWidth*3+3;
	gc.BitBlt( TPoint(startX,buttenY), iNextFImg);
#endif	

	if(showActiveID==EButton_Prev)
	{
		int x=4;
		gc.BitBlt( TPoint(x,buttenY), iPrevImg);

	}
	else if(showActiveID==EButton_Play)
	{
		int x=4;
		x=x+iImgSize.iWidth+1;
		gc.BitBlt( TPoint(x,buttenY), iPlayImg);

	}
	else if(showActiveID==EButton_Stop)
	{
		int x=4;
		x=x+iImgSize.iWidth*2+2;
		gc.BitBlt( TPoint(x,buttenY), iStopImg);

	}
	else if(showActiveID==EButton_Next)
	{
		int x=4;
		x=x+iImgSize.iWidth*3+3;
		gc.BitBlt( TPoint(x,buttenY), iNextImg);

	}
    }

/*
void CFunSMusicContainer::MfboButtonPressed(CFunButton* aButton)
{
	//iFondoGlobal->iHomeView->HandleCommandL (EOpenItemCmd);
}
*/
TKeyResponse CMyleftCenterContainerMusic::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
{
	TKeyResponse ret=EKeyWasNotConsumed;
	if(aType==EEventKeyUp)
	{
		switch(aKeyEvent.iScanCode)
		{
			case  50:  // '2' code  , show play prew button
			case  56:  // '8' code
			case  53:  // '5' code  show stop button
			case  167:  //OK code
			{
				showActiveID=0;
				DrawNow();
			}
				break;
			default:
				break;
		}
		return ret;
	}
	
	if(aType==EEventKeyDown)
	{
		switch(aKeyEvent.iScanCode)
		{
			case  50:  // '2' code  , show play prew button
			{
				showActiveID=EButton_Prev;
				ShowButton(EButton_Prev);
			}
			break;
			case  56:  // '8' code
			{
				showActiveID=EButton_Next;
				ShowButton(EButton_Next);
			}
			break;
			case  53:  // '5' code  show stop button
			{
				showActiveID=EButton_Stop;
				ShowButton(EButton_Stop);
			}
			break;
			case  167:  //OK code
			{
				showActiveID=EButton_Play;
				ShowButton(EButton_Play);
			}
			break;
			default:
			break;
		}
	
		return ret;
	}
	
	switch(aKeyEvent.iCode)
	{
	case  EKeyDevice3:
		{
			if(iShowMode==ShowModeLRC)
			{
				if(iplaycore->m_musicStatus==CCCB_STATUS_PLAY)
					iplaycore->Music_playerPause();
				else if(iplaycore->m_musicStatus==CCCB_STATUS_PAUSE)
					iplaycore->Music_playerPlay();
			/*	else if(iplaycore->m_musicStatus==CCCB_STATUS_WAIT)
				{
					iplaycore->playerDoPlayMsg();
				}*/
				ret=EKeyWasConsumed;
			}
			else if(iShowMode==ShowModeSreach)
			{
				int curId=GetCurListIndex();
				song *temp=iplaycore->m_musiclrclist->At(curId);
				if(temp)
				{
					TBuf<32> nameExt;
					music_list* templist=iplaycore->m_musicMasklist->At(iplaycore->m_iCurretPlayIndex);
						
					nameExt.Copy(templist->title);
					nameExt.SetLength(nameExt.Length()-4);

					iplaycore->Download_ListAppend(nameExt,temp->iLrcId,0, 0,EdownloadReady,MYLEFG_API_TYPE_LRC);
			
					CenterCore::ShowMessage(iplaycore->TextResource(R_DOWN_LIST));
					if(iplaycore->m_curDownId==-1)
					{	
						iplaycore->m_curDownId=iplaycore->m_downloadlist->Count()-1;
						download *tempdown=iplaycore->m_downloadlist->At(iplaycore->m_curDownId);
						iplaycore->iDownMgr->LaunchGetReadFile(tempdown->id, tempdown->filename,tempdown->typeId);
					}
					iplaycore->m_appUI->viewMusic->ChangeShowMode(0);
				}
				ret=EKeyWasConsumed;
			}
		}
		break;
	case EKeyBackspace:
		{
			ret=EKeyWasConsumed;
			if(iShowMode==ShowModeLRC)
				break;
			int cur=GetCurListIndex();
			if(iplaycore->m_musicStatus && cur==iplaycore->m_iCurretPlayIndex)
				break;
			iplaycore->Music_MaskListDelete(cur);
			if(iplaycore->m_musicStatus && (iplaycore->m_iCurretPlayIndex!=-1))
			{
				if(cur<iplaycore->m_iCurretPlayIndex)
				{
					iplaycore->m_iCurretPlayIndex--;
				}
			}
			UpdataAll();
            }
		break;
	case 'g':
	case '5':
		{
			if(iplaycore->m_musicStatus)
			{
			//	iplaycore->Music_ResetPlayerAndSubtitle();
				iplaycore->Music_playerStop();
				iSelIndex=iList->GetCurrentSelectIndex();
				UpdataAll();
				DrawNow();
				ret=EKeyWasConsumed;
			}		
			
		}
		break;
	case 'f':
	case '4':
		{
			int volume=iplaycore->m_musicSetting.volume;
			--volume;
			iplaycore->Music_SetVolume(volume);
			ret=EKeyWasConsumed;
			DrawNow();
		}
		break;
	case 'h':	
	case '6':	
		{
			int volume=iplaycore->m_musicSetting.volume;
			++volume;
			iplaycore->Music_SetVolume(volume);
			ret=EKeyWasConsumed;
			DrawNow();
		}
		break;
	case 't':
	case '2':
		{
			if(iplaycore->m_musicStatus)
			{
				song tempsong;
				int itemCount=iplaycore->m_musicMasklist->Count();
				int curId=iplaycore->Music_playerCurrentIndex();
				if(curId==0)
					curId=itemCount-1;
				else
					curId--;
				iplaycore->Music_GetPlayerListSong(&tempsong, curId);
				iplaycore->Music_playerSetCurIndex(curId);
				iplaycore->Music_playSong(&tempsong);
				iCurretPlayIndex=curId;
				iSelIndex=iList->GetCurrentSelectIndex();
				iplaycore->m_appUI->viewMusic->ResetAll();
				//UpdataAll();
			}
		}
		break;
	case 'b':
	case '8':
		{
			if(iplaycore->m_musicStatus)
			{
				song tempsong;
				int itemCount=iplaycore->m_musicMasklist->Count();
				int curId=iplaycore->Music_playerCurrentIndex();
				if(curId==itemCount-1)
					curId=0;
				else
					curId++;
				iplaycore->Music_GetPlayerListSong(&tempsong, curId);
				iplaycore->Music_playerSetCurIndex(curId);
				iplaycore->Music_playSong(&tempsong);
				iCurretPlayIndex=curId;
				iSelIndex=iList->GetCurrentSelectIndex();
				iplaycore->m_appUI->viewMusic->ResetAll();
			}
		}
		break;
	case '*':
		{
			if(iplaycore->m_subtitle)
				iplaycore->m_subtitle->SetOffset(-5000);
		}
		break;
	case '#':
		{
			if(iplaycore->m_subtitle)
				iplaycore->m_subtitle->SetOffset(5000);
		}
		break;
	default:
		break;

	}
	
	if(ret==EKeyWasNotConsumed)
	{
		if(iList && (iShowMode!=ShowModeLRC) && iList->GetListItemCount())
			ret= iList->OfferKeyEventL( aKeyEvent, aType );
	}
	
	return ret;
}



void CMyleftCenterContainerMusic::MfloListItemClicked(CFunList* aButton, TInt aIndex)
{
 	song tempSong;
	if(!aButton->GetListItemCount())
		return ;
	
	iCurretPlayIndex=iplaycore->Music_playerCurrentIndex();
	FC_Log("CFunSMusicContainer::MfloListItemClicked ,iCurretPlayIndex=%d,aIndex=%d,status=%d ",iCurretPlayIndex,aIndex,iplaycore->m_musicStatus);
	if(iShowMode==ShowModeSreach)
	{
		return;
	}
	
	if(aIndex==iCurretPlayIndex)
	{
		if(iplaycore->m_musicStatus==CCCB_STATUS_PLAY)
			iplaycore->Music_playerPause();
		else if(iplaycore->m_musicStatus==CCCB_STATUS_PAUSE)
			iplaycore->Music_playerPlay();
	//	else if(iplaycore->m_musicStatus==CCCB_STATUS_WAIT)
	//		iplaycore->Music_playerDoPlayMsg();
		else if(iplaycore->m_musicStatus==CCCB_STATUS_STOP)
		{
			iplaycore->Music_GetPlayerListSong(&tempSong,aIndex);
			iplaycore->Music_playSong(&tempSong);
		}
	}
	else
	{
		showActiveID=0;
		iSelIndex=aIndex;
		iplaycore->Music_GetPlayerListSong(&tempSong,aIndex);
		iplaycore->Music_playSong(&tempSong);
		iplaycore->Music_playerSetCurIndex(aIndex);
		iCurretPlayIndex=aIndex;
		UpdataAll();
		//iList->SetCurrentSelectIndex(aIndex);
	}
}

void CMyleftCenterContainerMusic::MftonTimerExpired() 
{
//	TTime now;
	TBuf<64> timeBuf;
	TBuf<32> bufPercent;	
	TBuf<256> name;	
	_LIT(KString,"%2d:%02d/%2d:%02d");
	_LIT(KString1,"%02d %%");
	_LIT(KString2,"%S %S");
	
	if(iplaycore->m_musicStatus)
	{
		int min=0;
		int sec=0;
		int durmin=0;
		int dursec=0;
		unsigned long dwtime= iplaycore->Music_GetCurTime();
		unsigned long durtime=iplaycore->m_duration;
	//	FC_Logsp("MftonTimerExpired dwtime=%lu",dwtime);
		if(dwtime)
		{
			min=dwtime/60;
			sec=dwtime%60;
		}
		if(durtime)
		{
			durmin=durtime/60;
			dursec=durtime%60;
		}
		timeBuf.Format(KString(),min,sec,durmin,dursec);		
		songTimeLib->ChangeTextL(timeBuf,EFalse);

	/*	if(!iplaycore->playerIsLocalPlay())
		{
			unsigned long  streamper= iplaycore->playerGetStreamPercent();
			bufPercent.Format(KString1(),streamper);	
			streamPercentLib->ChangeTextL(bufPercent,EFalse);
		}*/
		
		if(iplaycore->curPlayer)
		{
			if(iplaycore->curPlayer->name.Length())
				name.Format(KString2(),&iplaycore->curPlayer->name,&iplaycore->curPlayer->singer);
			songNameLib->ChangeTextL(name,EFalse);
		}


	}
	

	if(iplaycore->m_musicStatus==CCCB_STATUS_PLAY)
	{
		iplaycore->Music_SeekAndGetLrc();
	}
	
	TUint aCurtime=0;
	if(iplaycore->m_musicStatus)
	{
		if(iSubtitle)
		{
			aCurtime=iSubtitle->GetCurrentFrameIndex();
			if(aCurtime!=iCurtime)
			{			
				if(iLyricLabel && (iShowMode==ShowModeLRC))
				{
					iCurtime=aCurtime;
					iLyricLabel->UpdateLyricDisplay(EFalse, EFalse);
				}
				else
				{
					TUint frameLen=0;
					TBuf<KMaxLineCharNumber> frameBuf;
					
					 iSubtitle->GetTextBufByIndex(aCurtime, (TUint16*)frameBuf.Ptr(), frameLen, KMaxLineCharNumber );
					 frameBuf.SetLength(frameLen);			  
					songLrcLib->ChangeTextL(frameBuf,EFalse);
				}			
			}
		}
		else
		{		
			if(iLrcShowId==ELrcLibShow_NULL)
				songLrcLib->ChangeTextL(KNullDesC,EFalse);
			else if(iLrcShowId==ELrcLibShow_NoAutoLRC)
				songLrcLib->ChangeTextL(iplaycore->TextResource(R_TEXT_VIEW_NO_AUTO_LRC),EFalse);
			else if(iLrcShowId==ELrcLibShow_NoManuLRC)
				songLrcLib->ChangeTextL(iplaycore->TextResource(R_TEXT_VIEW_NO_MANU_LRC),EFalse);
			else if(iLrcShowId==ELrcLibShow_HaveLRC)
				songLrcLib->ChangeTextL(iplaycore->TextResource(R_TEXT_VIEW_DOWNLOAD_LRC),EFalse);	
			else if(iLrcShowId==ELrcLibShow_PayNotify)
				songLrcLib->ChangeTextL(iplaycore->TextResource(R_TEXT_VIEW_PAY_NOTIFY1),EFalse);


		}
	}
	
	iTimer->After( 1000000 );
	iflag=1;
	DrawNow();
}

CSubtitle* CMyleftCenterContainerMusic::MflloGetSubtitle()
{
	TBuf<125> iSubtitleFile;
	if(iSubtitle && (iSubtitle==iplaycore->Music_GetSubtitle()))
		return iSubtitle;
	
	iSubtitle=iplaycore->Music_GetSubtitle();
	return  iSubtitle;
}

void CMyleftCenterContainerMusic::GetSingerIcon()
{
	if(iSingerIcon)
		delete iSingerIcon;
	iSingerIcon=NULL;

	FC_Log("GetSingerIcon");
	if(iplaycore->curPlayer)
	{
		FC_Log("GetSingerIcon-----2-------");
		if(!iplaycore->curPlayer->iaskurl.Length())
				return ;
		TParse parser;
		TInt picid=0;
		TFileName desPaht;

		parser.Set( iplaycore->curPlayer->iaskurl, NULL, NULL );
		iplaycore->GetDownPicPath(desPaht);
		desPaht.Append(parser.Name());
		desPaht.Append(_L(".jpg"));
		if(CenterCore::IsFileExist(desPaht))
		{
			TSize reault;
			iSingerIcon=FunUiLib::LoadImageFromFileL(desPaht);
			reault=FunUiLib::ScaleBitmapL(iSingerIcon,TSize(KPlayContrl_PicW,KPlayContrl_PicW));
			if(reault!=TSize(KPlayContrl_PicW,KPlayContrl_PicW))
			{
				FC_Logsp("GetSingerIcon--ScaleBitmapL --failed-------");
				delete iSingerIcon;
				iSingerIcon=NULL;
			}
		}
		else
		{
			FC_Logsp("GetSingerIcon-----File  is not Exist-------");
		}
	}
}

void CMyleftCenterContainerMusic::SetupListItemsL()
{
	_LIT (KString,"%S");
	_LIT(kString1,"%S.............%S");

	TBuf<100> aString;
	TInt itemCount=0;
	TBuf<100> item;

	itemCount=iList->GetListItemCount();
	iList->DeleteItems(0, itemCount);
	iList->ResetAllItemIconIndex();
	
	if(iShowMode==ShowModeList)
	{
		if(iplaycore)
			itemCount=iplaycore->m_musicMasklist->Count();

		for (TInt i = 0; i< itemCount; i++)
		{	
			music_list* temp=iplaycore->m_musicMasklist->At(i);
			
			aString.Format(KString(),&temp->title);		
			iList->AppendItemL( aString );
		}

		iList->ChangeItemIconIndex(iCurretPlayIndex, 0);
	}
	else if(iShowMode==ShowModeSreach)
	{		
		for(TInt i=0;i<iplaycore->m_musiclrclist->Count();i++)
		{
			song * list=iplaycore->m_musiclrclist->At( i );
			aString.Format(kString1(),&(list->name),&(list->singer));
			iList->AppendItemL(aString);
		}
	}

	iList->SetCurrentSelectIndex(iSelIndex);

}

void CMyleftCenterContainerMusic::TimerStop()
{
	if( iTimer && iTimer->IsActive() )
		iTimer->Cancel();
}

void CMyleftCenterContainerMusic::ResetPlayIndexAndSubtitle(int _curIndex)
{
	if(_curIndex==-1)
		return;
	song tempSong;
	
	iCurretPlayIndex=_curIndex;
	
	if( iList && iShowMode==ShowModeList)
	{
		iList->ResetAllItemIconIndex();
		iList->ChangeItemIconIndex(_curIndex, 0);
		iList->DrawNow();
	}

	iplaycore->Music_GetPlayerListSong(&tempSong,_curIndex);
	iplaycore->Music_SetPlayerAndSubtitle(&tempSong);
}

void CMyleftCenterContainerMusic::UpdataAll()
{
	if( iTimer && iTimer->IsActive() )
		iTimer->Cancel();

	TUint aCurtime=0;
	TBuf<256> name;	
	TBuf<24> bufPercent;	
	_LIT(KString,"%S %S");
//	_LIT(KString1,"%d %%");

	iCurretPlayIndex=iplaycore->Music_playerCurrentIndex();

	MflloGetSubtitle();
	SetupListItemsL();
	GetSingerIcon();
	
	streamPercentLib->ChangeTextL(KNullDesC,EFalse);
	songNameLib->ChangeTextL(KNullDesC,EFalse);
	songLrcLib->ChangeTextL(KNullDesC,EFalse);
	songTimeLib->ChangeTextL(KNullDesC,EFalse);

	if(iplaycore->m_musicStatus)
	{
		if(iSubtitle )
		{
			aCurtime=iSubtitle->GetCurrentFrameIndex();
			if(iLyricLabel && (iShowMode==ShowModeLRC))
			{
				iCurtime=aCurtime;
				iLyricLabel->UpdateLyricDisplay(EFalse, EFalse);
			}
			else
			{
				TUint frameLen=0;
				TBuf<KMaxLineCharNumber> frameBuf;
				
				 iSubtitle->GetTextBufByIndex(aCurtime, (TUint16*)frameBuf.Ptr(), frameLen, KMaxLineCharNumber );
				 frameBuf.SetLength(frameLen);			  
				songLrcLib->ChangeTextL(frameBuf,EFalse);
			}			
		}
		else //no subtitle
		{
		/*	if(iShowMode==ShowModeList)
				songLrcLib->ChangeTextL(iplaycore->TextResource(R_TEXT_VIEW_NO_LRC),EFalse);
			else if(iShowMode==ShowModeSreach)
				songLrcLib->ChangeTextL(iplaycore->TextResource(R_TEXT_VIEW_DOWNLOAD_LRC),EFalse);*/
			if(iLrcShowId==ELrcLibShow_NULL)
				songLrcLib->ChangeTextL(KNullDesC,EFalse);
			else if(iLrcShowId==ELrcLibShow_NoAutoLRC)
				songLrcLib->ChangeTextL(iplaycore->TextResource(R_TEXT_VIEW_NO_AUTO_LRC),EFalse);
			else if(iLrcShowId==ELrcLibShow_NoManuLRC)
				songLrcLib->ChangeTextL(iplaycore->TextResource(R_TEXT_VIEW_NO_MANU_LRC),EFalse);
			else if(iLrcShowId==ELrcLibShow_HaveLRC)
				songLrcLib->ChangeTextL(iplaycore->TextResource(R_TEXT_VIEW_DOWNLOAD_LRC),EFalse);
			else if(iLrcShowId==ELrcLibShow_PayNotify)
				songLrcLib->ChangeTextL(iplaycore->TextResource(R_TEXT_VIEW_PAY_NOTIFY1),EFalse);

		}

		if(iplaycore->curPlayer)
		{
			if(iplaycore->curPlayer->name.Length() && iplaycore->curPlayer->singer.Length())
				name.Format(KString(),&iplaycore->curPlayer->name,&iplaycore->curPlayer->singer);
			songNameLib->ChangeTextL(name,EFalse);
		}

	}
	
	DrawNow();
	iTimer->After( 200000 );
	
}

void CMyleftCenterContainerMusic::ShowButton(int ButtonID)
{
	ActivateGc();	
	CWindowGc& gc = SystemGc();
	TSize  iImgSize=iPlayImg->SizeInPixels();
	TInt buttenY=Rect().Height()-1-iImgSize.iHeight;

	if(ButtonID==EButton_Prev)
	{
		int x=4;
		gc.BitBlt( TPoint(x,buttenY), iPrevImg);

	}
	else if(ButtonID==EButton_Play)
	{
		int x=0;
		
		x=4+iImgSize.iWidth+1;
		gc.BitBlt( TPoint(x,buttenY), iPlayImg);

	}
	else if(ButtonID==EButton_Stop)
	{
		int x=0;
		x=4+iImgSize.iWidth*2+2;
		gc.BitBlt( TPoint(x,buttenY), iStopImg);

	}
	else if(ButtonID==EButton_Next)
	{
		int x=0;
		x=4+iImgSize.iWidth*3+3;
		gc.BitBlt( TPoint(x,buttenY), iNextImg);

	}

	DeactivateGc();
}

void CMyleftCenterContainerMusic::Destroy()
{
	if(iMainBackImg)
		delete iMainBackImg;
	
	if( iList)
	{
		delete iList;
	}
	if( iTimer && iTimer->IsActive() )
		iTimer->Cancel();
	delete iTimer;
	
	if(songLrcLib)
		delete songLrcLib;
	if(songNameLib)
		delete songNameLib;
	if(songTimeLib)
		delete songTimeLib;
	if(streamPercentLib)
		delete streamPercentLib;
	if(iSingerImg)
		delete iSingerImg;
	if(iLyricLabel)
		delete iLyricLabel;

	if(iPrevImg)
		delete iPrevImg;
	if(iPlayImg)
		delete iPlayImg;
	if(iStopImg)
		delete iStopImg;
	if(iNextImg)
		delete iNextImg;

	if(iPrevFImg)
		delete iPrevFImg;
	if(iPlayFImg)
		delete iPlayFImg;
	if(iStopFImg)
		delete iStopFImg;
	if(iNextFImg)
		delete iNextFImg;

	if(iVolumeImg)
		delete iVolumeImg;

}
