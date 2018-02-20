/*
============================================================================
 Name        : FunSMusicDownLoadContainer from FunSMusicDownLoadContainer.h
 Author      : yilizheng
 Version     :
 Copyright   : Your copyright notice
 Description : Container control PlayList
============================================================================
*/

// INCLUDE FILES
#include "MyleftCenterContainerDownLoad.h"

//#include <eiklabel.h>  // for example label control

#include "portab_symbian.h"

#include <MyleftCenter.rsg>
#include "MyleftCenter.hrh"
#include <aknmessagequerydialog.h> 
#include <AknIconArray.h> 
#include <barsread.h>
#include <StringLoader.h> 
#include <eikfrlb.h>   //CFormattedCellListBoxItemDrawer()
#include <eikclbd.h>	//CColumnListBoxData::SetIconArray()
#include <MyleftCenter.mbg>
#include "CenterCore.h"
#include "MyleftCenterViewDownLoad.h"
#include "const.h"
// ================= MEMBER FUNCTIONS =======================



	CMyleftCenterContainerDownLoad* CMyleftCenterContainerDownLoad::NewL(const TRect& aRect)
	{
			CMyleftCenterContainerDownLoad* self=CMyleftCenterContainerDownLoad::NewLC(aRect);
			CleanupStack::Pop(self);
			return self;

	}

	CMyleftCenterContainerDownLoad* CMyleftCenterContainerDownLoad::NewLC(const TRect& aRect)
	{
			CMyleftCenterContainerDownLoad* self=new(ELeave) CMyleftCenterContainerDownLoad;
			CleanupStack::PushL(self);
			self->ConstructL(aRect);
			return self;
	}

// ---------------------------------------------------------
// FunVVContainerList::ConstructL(const TRect& aRect)
// EPOC two phased constructor
// ---------------------------------------------------------
//
void CMyleftCenterContainerDownLoad::ConstructL(const TRect& aRect)
    {
    CreateWindowL();

	iplaycore=CenterCore::GetInstance();
	if(!iplaycore)
		return ;
/*	
	iMainBackImg=FunUiLib::CreateBitmapL(KMusicMultiBitmapFilename,EMbmFunsmusicMain_back);
	if(!iMainBackImg)
		return;
*/	
	iTimer = CFunTimeOutTimer::NewL( 0, *this );
	iTimer->After( 2000000 );

//set iDownloadList      EMbmMyleftcenterMain_back
	TInt iconIds[] = { EMbmMyleftcenterDownloading, EMbmMyleftcenterDownload_ok,
					EMbmMyleftcenterDownload_error,EMbmMyleftcenterDownload_stop};
	TInt maskIds[] = { EMbmMyleftcenterDownloading_mask, EMbmMyleftcenterDownload_ok_mask,
					EMbmMyleftcenterDownload_error_mask,EMbmMyleftcenterDownload_stop_mask};

	iDownloadList= CFunList::NewL( aRect, this, FunUiLib::TinyFont() );
	iDownloadList->SetupGraphicIcons( KMusicMultiBitmapFilename, iconIds, maskIds, 4, 0, 13, 13);
	iDownloadList->BackDrawer()->SetGradientBackground( CFunBackgroundDrawer::EVerticalOrientation,KRgbWhite, KRgbContainer);
	//iDownloadList->BackDrawer()->SetRectBorder( TRgb(160,160,160) ,1);
	iDownloadList->ActivedBackDrawer()->SetBackgroundColor( KRgbContainerDeep);
	iDownloadList->ActivedBackDrawer()->SetRoundRectBorder(TRgb(254,254,254), TSize(2,1),1);
	iDownloadList->SetTextColors( TRgb(1,1,1), TRgb(254,254,254),  TRgb(254,254,254));
	iDownloadList->ChangeItemHeight(KDownload_ItemH);
	iDownloadList->SetDisplayNumber(0);
	iDownloadList->SetObserver(this);
	SetupListItemsL();
	
    SetRect(aRect);
    ActivateL();
    }

// Destructor
CMyleftCenterContainerDownLoad::~CMyleftCenterContainerDownLoad()
{
	if( iTimer && iTimer->IsActive() )
		iTimer->Cancel();
	delete iTimer;
		
	if(iDownloadList)
	{
		delete iDownloadList;
	}
	
	if(iMainBackImg)
	{
		delete iMainBackImg;
	}
}

// ---------------------------------------------------------
// FunVVContainerList::SizeChanged()
// Called by framework when the view size is changed
// ---------------------------------------------------------
//
void CMyleftCenterContainerDownLoad::SizeChanged()
    {
	if( iDownloadList )
	{
	
		TRect listRect= Rect();
		listRect.Move( 1, 0 );
		listRect.Resize( -2, 0);
		
		iDownloadList->SetExtent( listRect.iTl, listRect.Size() );
		
	}
	 DrawNow();
    }

// ---------------------------------------------------------
// CFunVVContainer::CountComponentControls() const
// ---------------------------------------------------------
//
TInt CMyleftCenterContainerDownLoad::CountComponentControls() const
    {
    return 1; // return nbr of controls inside this container
    }

// ---------------------------------------------------------
// CFunVVContainer::ComponentControl(TInt aIndex) const
// ---------------------------------------------------------
//
CCoeControl* CMyleftCenterContainerDownLoad::ComponentControl(TInt aIndex) const
    {
    switch ( aIndex )
        {
        case 0:
            return iDownloadList;
 
        default:
            return NULL;
        }
    }

// ---------------------------------------------------------
// CFunVVContainer::Draw(const TRect& aRect) const
// ---------------------------------------------------------
//
void CMyleftCenterContainerDownLoad::Draw(const TRect& aRect) const
    {
    CWindowGc& gc = SystemGc();

	gc.SetPenStyle( CGraphicsContext::ESolidPen );
	gc.SetPenColor(KRgbRect);
	gc.DrawLine(TPoint(0,0),TPoint(0,aRect.Height()));
	gc.DrawLine(TPoint(aRect.Width()-1,0),TPoint(aRect.Width()-1,aRect.Height()));

    }

// ---------------------------------------------------------
// FunVVContainerList::HandleControlEventL(
//     CCoeControl* aControl,TCoeEvent aEventType)
// ---------------------------------------------------------
//

// ---------------------------------------------------------
// FunVVContainerList::OfferKeyEventL(
//     CCoeControl* aControl,TCoeEvent aEventType)
// ---------------------------------------------------------
//
TKeyResponse CMyleftCenterContainerDownLoad::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
    {
		//TInt code = aKeyEvent.iCode;
		if ( iDownloadList )
		{
			return iDownloadList->OfferKeyEventL( aKeyEvent, aType );
		}
		return( EKeyWasNotConsumed );

    }

void CMyleftCenterContainerDownLoad::MftonTimerExpired() 
{
//	FC_Logsp("MftonTimerExpired,iplaycore->m_curDownId=%d",iplaycore->m_curDownId);
	if(iplaycore->m_curDownId!=-1)
	{	
		iCurdownId=iplaycore->Download_ListDownloadCheck();
		UpdataL();
		
	}
	
	iTimer->After( 1000000 );
}

void CMyleftCenterContainerDownLoad::MfloListItemClicked(CFunList* aButton, TInt aIndex)
{
	TInt index=aIndex;
/*
	if(is_downloadView)///in page 2
	{
	 	song tempSong;
		int appendId=0;
		TBuf<128> itemStatus;
		iplaycore->dl_getDownListItem2Path(index, tempSong.iaskurl);
		iplaycore->dl_getDownListItem(index,tempSong.name,itemStatus);
		
		appendId=iplaycore->myAVAddItemToPlayer(tempSong.name, tempSong.iaskurl);

		iplaycore->playerSetCurIndex(appendId);
		iplaycore->Music_playSong(&tempSong);

		return;
	}

	if(index==0)// goto page 2
	{
		is_downloadView=ETrue;
		iDownloadList->SetCurrentSelectIndex(0);
		UpdataL();
	}
	else//start download
	{
		int curRun=iplaycore->dl_getDownCurRun();
		if(index-1==curRun)
			iplaycore->dl_stopDownload(index-1);
		else
			iplaycore->dl_startDownload(index-1);
	}
*/
}
// ---------------------------------------------------------
// CZStdTest1Container::HandleListBoxEventL(
//     SCEikListBox* /*aListBox*/,TListBoxEvent aListBoxEvent)
// ---------------------------------------------------------
//
void CMyleftCenterContainerDownLoad::SetupListItemsL()
{
	_LIT (KString,"%d%S");
	_LIT (KString2,"%S    %d(kb)/%d(kb)");
	TBuf<128> aString;
	TInt downCount=0;
	TInt downingCount=0;

	downCount=iDownloadList->GetListItemCount();
	iDownloadList->DeleteItems(0, downCount);

	if(1)
	{
		downCount=iplaycore->m_downloadlist->Count();

		for (TInt i = 0; i< downCount; i++)
		{	
			int iIconId=1;  //download ok
			HBufC* Status=NULL;
			TBuf<100> item;
			TBuf<100>	itemStatus;
			TUint size,length;
			TdownloadStatus status;
			iplaycore->Download_ListGetDownloadItem(i,item,length,size,status);

			if(status==EdownloadIng)
				Status=StringLoader::LoadLC(R_TEXT_DOWNLOAD_RUN);
			else if(status==EdownloadOk)
				Status=StringLoader::LoadLC(R_TEXT_DOWNLOAD_COMPLETE);
			else if(status==EdownloadErr)
				Status=StringLoader::LoadLC(R_TEXT_DOWNLOAD_ERROR);
			else if(status==EdownloadReady)
				Status=StringLoader::LoadLC(R_TEXT_DOWNLOAD_PAUSE);

			itemStatus.Format(KString2(),Status,length/1024,size/1024);
			iDownloadList->AppendItemL( item, itemStatus, (int)status,NULL);
			CleanupStack::PopAndDestroy(Status);
		}	
	}
/*	else
	{
		HBufC* itemString1,*itemString2;
		itemString1=StringLoader::LoadLC(R_DOWN_LIST);
		itemString2=StringLoader::LoadLC(R_DOWN_COUNT);
		
		downCount=iplaycore->dl_getDownListCount();
		aString.Format(KString(),downCount,itemString2);

		iDownloadList->AppendItemL( itemString1->Des(), aString, 0,NULL);
		
		CleanupStack::PopAndDestroy(itemString2);
		CleanupStack::PopAndDestroy(itemString1);
		
		downingCount=iplaycore->dl_getDowningListCount();

		for (TInt i = 0; i< downingCount; i++)
		{	
			TBuf<100> item;
			TBuf<100>	itemStatus;
			int iconNum=5;		//DOWN_STATUS_DOWNLOADING==1;
			int status=iplaycore->dl_getDowningListItem(i,item,itemStatus);
			
			if(status==4)			//ERROR
			  	iconNum=3;
			else if(status==5)		//RUN
				iconNum=1;
			else if(status==3)   	//DOWN_STATUS_STOP  
				iconNum=4;

			iDownloadList->AppendItemL( item, itemStatus, iconNum,NULL);
		}	
	}*/
}


 void CMyleftCenterContainerDownLoad::UpdataL()
 {
 	SetupListItemsL();
	DrawNow();
 }


void CMyleftCenterContainerDownLoad::StartTime()
{
	//FPlayerCore *iplaycore=FPlayerCore::GetInstance();

	if ( iTimer )
	{
		if ( !iTimer->IsActive() )
		{
			iTimer->After( 1000 * 1000 ); //1 
		}
	}
}
