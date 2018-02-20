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
#include "MyleftCenterContainerReadList.h"

#include <eikenv.h>
#include <coemain.h>
#include <eiklabel.h>  // for example label control
#include <StringLoader.h> 
#include "centercore.h"
#include <MyleftCenter.mbg>
#include  <MyleftCenter.rsg>
#include  "MyleftCenter.hrh"
#include "const.h"
#include "FunUIStart.h"
#include  "MyleftCenterView.h"
#include  "MyleftCenterViewReadList.h"
#include  "MyleftCenterViewRead.h"
#include "SinaInterfaceMgr.h"
// ================= MEMBER FUNCTIONS =======================


// -----------------------------------------------------------------------------
// CTest4AppView3::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMyleftCenterContainerReadList* CMyleftCenterContainerReadList::NewL(CMyleftCenterViewReadList* aParent,  const TRect& aRect )
    {
    CMyleftCenterContainerReadList* self = CMyleftCenterContainerReadList::NewLC( aParent,aRect );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CTest4AppView3::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMyleftCenterContainerReadList* CMyleftCenterContainerReadList::NewLC(CMyleftCenterViewReadList* aParent,  const TRect& aRect )
    {
    CMyleftCenterContainerReadList* self = new ( ELeave ) CMyleftCenterContainerReadList;
    CleanupStack::PushL( self );
    self->ConstructL( aParent,aRect );
    return self;
    }


// ---------------------------------------------------------
// CFunSMusicContainer2::ConstructL(const TRect& aRect)
// EPOC two phased constructor
// ---------------------------------------------------------
//
void CMyleftCenterContainerReadList::ConstructL(CMyleftCenterViewReadList* aParent, const TRect& aRect)
    {
    CreateWindowL();

	iParent=aParent;
	iplaycore=CenterCore::GetInstance();
	icurPage=aParent->icurPage;
	iPrevIndex=aParent->iPrevIndex;

	iMainBackImg=FunUiLib::CreateBitmapL(KMusicMultiBitmapFilename,EMbmMyleftcenterMain_back);
	if(!iMainBackImg)
		return;

	songLrcLib=CFunLabel::NewL( aRect, this, KNullDesC, FunUiLib::NormalFont());	
	songLrcLib->SetTransparent( EFalse );
	songLrcLib->BackDrawer()->SetRectBorder(KRgbWhite, 1);
	songLrcLib->BackDrawer()->SetBackgroundColor(KRgbContainer);
	songLrcLib->SetTextAlign( CGraphicsContext::ECenter);
	songLrcLib->SetTextColor(TRgb(254,254,254));
	UpdateSongLrcLibL();

	TRect listRect= aRect;
	iList = CFunList::NewL( listRect, this, FunUiLib::TinyFont() );
	iList->SetObserver(this);	
	iList->BackDrawer()->SetBackgroundImageL(KMusicMultiBitmapFilename,EMbmMyleftcenterMain_back);
//	iList->BackDrawer()->SetGradientBackground( CFunBackgroundDrawer::EVerticalOrientation,KRgbWhite, KRgbContainer);
	iList->ActivedBackDrawer()->SetBackgroundColor( KRgbContainerDeep);
	iList->ActivedBackDrawer()->SetRectBorder(KRgbRect, 1);
	iList->SetTextColors( TRgb(254,254,254), TRgb(254,254,254),  TRgb(254,254,254));
	iList->SetDisplayNumber(1);
	SetupListItemsL();
	iList->SetCurrentSelectIndex(aParent->icurSellectId);

	iMenuPan=new (ELeave) CFunMenuPane();
	if( iMenuPan )
	{
		iMenuPan->ConstructFromResourceL(this,R_FUNSMUSIC_READ_SEARCHLIST_MENU);

	//	iMenuPan->BackDrawer()->SetRoundRectBorder( KRgbGray, TSize(4,4) );
		iMenuPan->BackDrawer()->SetBackgroundColor(  TRgb(192,192,192) );
		iMenuPan->SetTextColors( TRgb(0,0,0), TRgb(255,255,255) );
		iMenuPan->SetMargin( 10,10, 0,0 );
		iMenuPan->SetItemHeight( 15 );
		iMenuPan->SetFont( FunUiLib::TinyFont() );
		iMenuPan->SetFadeBehind(EFalse);
		iMenuPan->FocusBackDrawer()->SetBackgroundColor(KRgbTitle);
		iMenuPan->SetObserver(this);
		iMenuPan->SetControlHeight( KTitleHight_Len );
		iMenuPan->ControlBar()->BackDrawer()->SetBackgroundImageL( KMusicMultiBitmapFilename, EMbmMyleftcenterButton );
	//	iMenuPan->ControlBar()->BackDrawer()->SetRectBorder( KRgbRect );
	//	iMenuPan->ControlBar()->SetButtonTextColor(TRgb(254,254,254));

//		iMenuPan->ScrollBar()->BackDrawer()->SetRectBorder( KRgbGray, 0 );
//		iMenuPan->ScrollBar()->BackDrawer()->SetBackgroundColor( KRgbWhite );
//		iMenuPan->ScrollBar()->Set3DThumb( KRgbWhite, KRgbYellow, KRgbDarkYellow );
	}
	

    SetRect(aRect);
    ActivateL();
    }

// Destructor
CMyleftCenterContainerReadList::~CMyleftCenterContainerReadList()
    {
	if(iSearchEngine)
		delete iSearchEngine;
	iSearchEngine=NULL;
	if(iMainBackImg)
		delete iMainBackImg;
	if( iList)
		delete iList;
	if(songLrcLib)
		delete songLrcLib;

	if(iMenuPan)
		delete iMenuPan;
    }

// ---------------------------------------------------------
// CFunSMusicContainer2::SizeChanged()
// Called by framework when the view size is changed
// ---------------------------------------------------------
//
void CMyleftCenterContainerReadList::SizeChanged()
    {
    	if( songLrcLib )
	{	
		TRect lrcRect= Rect();
		lrcRect.Move( 1, 0 );
		lrcRect.Resize( -2, -lrcRect.Height()+KTitleHight_Len);
		songLrcLib->SetExtent( lrcRect.iTl, lrcRect.Size() );		
	}

	if( iList )
	{
	
		TRect listRect= Rect();
		listRect.Move( 1, KTitleHight_Len );
		listRect.Resize( -2, -KTitleHight_Len);
		iList->SetExtent( listRect.iTl, listRect.Size() );
		
	}
	 DrawNow();
    }

// ---------------------------------------------------------
// CFunSMusicContainer2::CountComponentControls() const
// ---------------------------------------------------------
//
TInt CMyleftCenterContainerReadList::CountComponentControls() const
    {
    return 2; // return nbr of controls inside this container
    }

// ---------------------------------------------------------
// CFunSMusicContainer2::ComponentControl(TInt aIndex) const
// ---------------------------------------------------------
//
CCoeControl* CMyleftCenterContainerReadList::ComponentControl(TInt aIndex) const
    {
    switch ( aIndex )
        {
        case 0:
		 return songLrcLib;
        case 1:
		 return iList;
        default:
            return NULL;
        }
    }


// ---------------------------------------------------------
// CFunSMusicContainer2::Draw(const TRect& aRect) const
// ---------------------------------------------------------
//
void CMyleftCenterContainerReadList::Draw(const TRect& aRect) const
{
	CWindowGc& gc = SystemGc();
	gc.BitBlt(Rect().iTl, iMainBackImg );
}


TKeyResponse CMyleftCenterContainerReadList::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
{
	TKeyResponse ret= EKeyWasNotConsumed;
	switch(aKeyEvent.iCode)
	{
		case EKeyBackspace:
		{
			if(icurPage==1)
			{
				int curId=iList->GetCurrentSelectIndex();

				iplaycore->Read_localListDelete(curId);
				UpdateListItemsL();
				ret=EKeyWasConsumed;
			}			
            	}
		break;
		default :
			break;
	}

	
	if(ret==EKeyWasNotConsumed)
	{
		if ( iList  && iList->GetListItemCount())
		{
			return iList->OfferKeyEventL( aKeyEvent, aType );
		}
	}

	return EKeyWasNotConsumed;
}

void CMyleftCenterContainerReadList::MfloListItemClicked(CFunList* aButton, TInt aIndex)
{

      if(icurPage==0)
      	{
      		iList->SetCurrentSelectIndex(0);
		iParent->ChangeCbaRight(iplaycore->TextResource(R_TEXT_VIEW_BACK), EAknSoftkeyBack);

		if(aIndex==0)
		{
			icurPage=1;
			if(FC_IsFileExist(KUpdataListFlag)) //only after setup;
			{
				UpdatePhoneListItemsL();
				FC_DeleteFile(KUpdataListFlag);		
			}
			else
			{
				UpdateListItemsL();
			}

		}
		else if(aIndex==1)
		{
			icurPage=2;
			if(!iplaycore->m_readChannels->Count())
				iplaycore->iSinaMgr->LaunchGetBangdanClass((TInt)MYLEFG_API_CHANNELS,(TInt)MYLEFG_API_TYPE_TXT);
			else
				UpdateListItemsL();
		}
		else
		{
			icurPage=3;
			if(!iplaycore->m_readClassifies->Count())
				iplaycore->iSinaMgr->LaunchGetBangdanClass((TInt)MYLEFG_API_CLASSIFIES,(TInt)MYLEFG_API_TYPE_TXT);
			else
				UpdateListItemsL();
		}
		

	 }
	  else if(icurPage==1)//local play
	 {
	 	read_list * list=iplaycore->m_localReadlist->At(aIndex);
		if(!iplaycore->IsFileExist(list->iaskurl))
		{
			iplaycore->Read_localListDelete(aIndex);
			UpdateListItemsL();
			return;
		}
	 	iplaycore->Read_SetRead(list->iaskurl,list->title,0);		
		iplaycore->m_readViewid=KViewReadListId;
		iplaycore->m_appUI->ActivateLocalViewL(KViewReadId);

	}
	  else if(icurPage==2)  //only playlist
	 {
		icurPage=4;
		iPrevIndex=aIndex;
		read_bdclass *list=iplaycore->m_readChannels->At(aIndex);
		iplaycore->iSinaMgr->LaunchGetList(list->Id,list->typeId,list->classes);
	}
	  else if(icurPage==3)  //only playlist
	 {
		icurPage=5;
		iPrevIndex=aIndex;
		read_bdclass *list=iplaycore->m_readClassifies->At(aIndex);
		iplaycore->iSinaMgr->LaunchGetList(list->Id,list->typeId,list->classes);
	}
	  else if(icurPage==4 ||icurPage==5)  // show menu
	 {
		TPoint aPos;
		if(iList)
			iList->GetItemPos(aPos);
		if(iMenuPan)
			iMenuPan->TryDisplayMenuPaneL(aPos, EFalse);	
	}
	  
}

void CMyleftCenterContainerReadList::ProcessMenuCommandL(TInt aCommandId)
{
	TInt index=iList->GetCurrentSelectIndex();
	book*templist=iplaycore->m_readList->At(index);
    switch ( aCommandId )
        {
	case EMyleftCenterCmdViewReadListInfo:
		{
		}
		  break;
	case EMyleftCenterCmdAppToCollection:
		{

		}
		break;
	case EMyleftCenterCmdAppDownLoad:
		{
			TBuf<256>  idownloadDes;
			
			iplaycore->Read_GetDownTextPath(idownloadDes);		
			idownloadDes.Append(templist->name);
			idownloadDes.Append(_L(".txt"));
			if(CenterCore::IsFileExist(idownloadDes))
			{
		  		TInt ret=CFunMessageDialog::RunDlgLD(KNullDesC,iplaycore->TextResource(R_MESSAGE_DELETE),iplaycore->m_appUI->UiStyler(),NULL,CFunMessageDialog::EMessageDialog_OkCancel,CFunMessageDialog::EMessageIcon_Question);
				if(!ret)
					return;
			}
			iplaycore->Download_ListAppend(templist->name,templist->Id,0, 0,EdownloadReady,MYLEFG_API_TYPE_TXT);

			if(templist->iPicID>=0 && iplaycore->m_readerSetting.downPic)
				iplaycore->Download_ListAppend(templist->name,templist->iPicID,0, 0,EdownloadReady,MYLEFG_API_TYPE_PIC);

			CenterCore::ShowMessage(iplaycore->TextResource(R_DOWN_LIST));
			if(iplaycore->m_curDownId==-1)
			{	
				iplaycore->m_curDownId=iplaycore->m_downloadlist->Count()-1;
				download *tempdown=iplaycore->m_downloadlist->At(iplaycore->m_curDownId);
				iplaycore->iDownMgr->LaunchGetReadFile(tempdown->id, tempdown->filename,tempdown->typeId);
			}
		}
		 break;
        default:
            break;
         }
	
}

 void CMyleftCenterContainerReadList::SetupListItemsL()
{
	_LIT (KString,"%S");
	_LIT (KString1,"%S (%d)");
	_LIT (KString2,"%S (%S)");
	
	TBuf<100> aString;
	TInt itemCount=0;
	TBuf<100> item;

	itemCount=iList->GetListItemCount();
	iList->DeleteItems(0, itemCount);
	
	if(icurPage==0)
	{
		itemCount=2;
		HBufC* itemName;
		int count=iplaycore->GetLocalReadCount();
		FC_Logsp("CMyleftCenterContainerReadList::SetupListItemsL  LocalReadCount=%d",count);
		itemName=StringLoader::LoadLC(R_TEXT_READ_PHONE);
		aString.Format(KString1(),itemName,count);
		iList->AppendItemL( aString );
		CleanupStack::PopAndDestroy(itemName);

	//	count=iplaycore->getMyAVItemObjCount(Sina_Onlinelist_Name);
		itemName=StringLoader::LoadLC(R_TEXT_WAP_CHANNELS);
		aString.Format(KString(),itemName);
		iList->AppendItemL( aString );
		CleanupStack::PopAndDestroy(itemName);

		itemName=StringLoader::LoadLC(R_TEXT_WAP_CLASSIFIES);
		aString.Format(KString(),itemName);
		iList->AppendItemL( aString );
		CleanupStack::PopAndDestroy(itemName);

	}
	else if(icurPage==1)
	{

		itemCount=iplaycore->GetLocalReadCount();

		for (TInt i = 0; i< itemCount; i++)
		{	
			read_list* list =  iplaycore->m_localReadlist->At(i);
			item.Copy(list->title);
			aString.Format(KString(),&item);		
			iList->AppendItemL( aString );
		}
	}
	else if(icurPage==2)
	{
	
		itemCount=iplaycore->m_readChannels->Count();

		for (TInt i = 0; i< itemCount; i++)
		{	
			read_bdclass *templist=iplaycore->m_readChannels->At(i);
			aString.Format(KString(),&templist->title);				
			iList->AppendItemL( aString );
		}
	}
	else if(icurPage==3)
	{
	
		itemCount=iplaycore->m_readClassifies->Count();

		for (TInt i = 0; i< itemCount; i++)
		{	
			read_bdclass*templist=iplaycore->m_readClassifies->At(i);
			aString.Format(KString(),&templist->title);				
			iList->AppendItemL( aString );
		}
	}
	else if(icurPage==4||icurPage==5)
	{
	
		itemCount=iplaycore->m_readList->Count();

		for (TInt i = 0; i< itemCount; i++)
		{	
			book*templist=iplaycore->m_readList->At(i);
			if(templist->auth.Length())
				aString.Format(KString2(),&templist->name,&templist->auth);		
			else
				aString.Format(KString(),&templist->name);	
			iList->AppendItemL( aString );
		}
	}

}

 void CMyleftCenterContainerReadList::UpdateListItemsL()
 {
  	UpdateSongLrcLibL();
	SetupListItemsL();
	DrawNow();
 }

 void CMyleftCenterContainerReadList::UpdateSongLrcLibL()
 {
 	if(!songLrcLib)
		return ;
	TBuf<100> aTitle;
	if(icurPage==0)
	{
		aTitle=KNullDesC;
	}
	else if(icurPage==1)
	{
		aTitle.Copy(_L(" "));
		aTitle.Append(iplaycore->TextResource(R_TEXT_READ_PHONE));
	}
	else if(icurPage==2)
	{
		aTitle.Copy(_L(" "));
		aTitle.Append(iplaycore->TextResource(R_TEXT_WAP_CHANNELS));
	}
	else if(icurPage==3)
	{
		aTitle.Copy(_L(" "));
		aTitle.Append(iplaycore->TextResource(R_TEXT_WAP_CLASSIFIES));
	}
	else if(icurPage==4)
	{
		read_bdclass *list=iplaycore->m_readChannels->At(iPrevIndex);
		aTitle.Copy(_L(" "));
		aTitle.Append(iplaycore->TextResource(R_TEXT_WAP_CHANNELS));
		aTitle.Append(_L("->"));
		aTitle.Append(list->title);		
	}
	else if(icurPage==5)
	{
		read_bdclass *list=iplaycore->m_readClassifies->At(iPrevIndex);
		
		aTitle.Copy(_L(" "));
		aTitle.Append(iplaycore->TextResource(R_TEXT_WAP_CLASSIFIES));
		aTitle.Append(_L("->"));
		aTitle.Append(list->title);
	}
	
	songLrcLib->ChangeTextL(aTitle,EFalse);
 }
 void CMyleftCenterContainerReadList::UpdatePhoneListItemsL()
 {
 
	iplaycore->ShowWaitNote(iplaycore->TextResource(R_TEXT_PLAYLIST_UPDATA));	
	if(iSearchEngine)
	{
		delete iSearchEngine;
		iSearchEngine = NULL;
	}
	iSearchEngine = new (ELeave) CFileSearchEngine(this,2);
	iSearchEngine->ConstructL();
	iSearchEngine->StartSearchInThread();
 }

void CMyleftCenterContainerReadList::SearchComplete()
{
	iplaycore->FinishWaitNote();
	UpdateListItemsL();
}
