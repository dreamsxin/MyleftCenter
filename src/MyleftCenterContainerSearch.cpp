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
#include "MyleftCenterViewSearch.h"
#include "MyleftCenterContainerSearch.h"

#include <eiklabel.h>  // for example label control
#include <eikedwin.h> 
#include <eikrted.h> 
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
void CMyleftCenterContainerSearch::ConstructL(CMyleftCenterViewSearch *parent,const TRect& aRect)
    {
    CreateWindowL();
	
	iplaycore=CenterCore::GetInstance();
	iparent=parent;

	iWidth=aRect.Size().iWidth;

	iMainBackImg=FunUiLib::CreateBitmapL(KMusicMultiBitmapFilename,EMbmMyleftcenterMain_back);
	if(!iMainBackImg)
		return;
/*

	iKeyImg=FunUiLib::CreateBitmapL(KMusicMultiBitmapFilename,EMbmFunsmusicKey);
	if(!iKeyImg)
	{
		delete iMainBackImg;
		return;
	}
*/
	int x=0;
//	TSize  iImgSize=iKeyImg->SizeInPixels();
	TSize  iSize=aRect.Size();
	x=(iSize.iWidth -KEdit_Width)/2;
	
	iPageEdit= CFunEdit::NewL(TRect(x,KTitleHight_Len-5,x+KEdit_Width,KTitleHight_Len-5+KEdit_Hight) , this,  iparent->iText);
	iPageEdit->ChangeFrameColor(TRgb(255,255,255),TRgb(255,255,255));

	x=(iSize.iWidth -35)/2;
	Button1=CFunButton::NewL(TRect(TPoint (x,KTitleHight_Len+KEdit_Hight),TSize(35,27)), this,this, 
								EMyleftCenterCmdAppSearchBook, KNullDesC,-1,
								KMusicMultiBitmapFilename,EMbmMyleftcenterSearch, EMbmMyleftcenterSearch_sel, 
								EMbmMyleftcenterSearch_sel,EMbmMyleftcenterSearch,EMbmMyleftcenterSearch_mask,-1,-1,ETrue);

//	iPageLable=CFunLabel::NewL(TPoint (iSize.iWidth-50,65), 40, 0, this,KNullDesC, FunUiLib::NormalFont(), CFunLabel::ESingleLine);
	iPageLable=CFunLabel::NewL( aRect, this, KNullDesC, FunUiLib::NormalFont());
	iPageLable->SetTransparent( ETrue );
	iPageLable->SetTextColor(TRgb(1,1,1));
	TBuf<12> aString;
	TBuf<12> pageth;
	TInt icur=iplaycore->m_appUI->viewSearch->iCurPage;
	if(icur)
	{
		CEikonEnv::Static()->ReadResource (aString, R_TEXT_VIEW_SEARCH_PAGE_NUM);
		pageth.Format(aString,icur);
		iPageLable->ChangeTextL(pageth,EFalse);
	}
	
	TRect listRect= aRect;
	iList = CFunList::NewL( listRect, this, FunUiLib::TinyFont() );
	iList->SetObserver(this);
	iList->BackDrawer()->SetBackgroundColor( KRgbContainer);
	iList->ActivedBackDrawer()->SetBackgroundColor( KRgbContainerDeep);
	iList->ActivedBackDrawer()->SetNoBorder();
	iList->SetTextColors( TRgb(254,254,254), TRgb(254,254,254),  TRgb(254,254,254));
	iList->SetDisplayNumber(1);

	SetUpHotlist();
	
	iForm =CFunForm::NewL( aRect, this );
	iForm->BackDrawer()->SetNoBorder();
	iForm->BackDrawer()->SetNullBackground();
	iForm->AddControlL(TReadSearchPageEdit,iPageEdit,ETrue, EFalse,0);
	iForm->AddControlL(TReadSearchButton,Button1,ETrue, ETrue,0);
	iForm->AddControlL(TReadSearchPageLable,iPageLable,EFalse, ETrue,0);
	iForm->AddControlL(TReadSearchList,iList,ETrue, ETrue,0);
	iForm->SetFocusIndex(TReadSearchButton);
//creat popMenu
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
		iMenuPan->FocusBackDrawer()->SetBackgroundColor(TRgb(128,192,192));
		iMenuPan->SetObserver(this);
		iMenuPan->SetControlHeight( KTitleHight_Len );
		iMenuPan->ControlBar()->BackDrawer()->SetBackgroundImageL( KMusicMultiBitmapFilename, EMbmMyleftcenterButton );
	}

 

    SetRect(aRect);
    ActivateL();

    }

// Destructor
CMyleftCenterContainerSearch::~CMyleftCenterContainerSearch()
{

    if(iMainBackImg)
   	 delete iMainBackImg;
    if(iKeyImg)
   	 delete iKeyImg;
    if(iMenuPan)
	  delete iMenuPan;

	delete iForm;
}

// ---------------------------------------------------------
// CFunSMusicContainer2::SizeChanged()
// Called by framework when the view size is changed
// ---------------------------------------------------------
//
void CMyleftCenterContainerSearch::SizeChanged()
    {
    	if(iList)
    	{
    		TRect listRect= Rect();
		listRect.Move( 1, 80);
		listRect.Resize( -2, -80);
		iList->SetExtent( listRect.iTl, listRect.Size() );
	}
		
	if( iPageLable)
	{	
		TRect lrcRect= Rect();
		TSize  iSize=lrcRect.Size();
		TInt moveX=(iSize.iWidth+KEdit_Width)/2+1;
		
		lrcRect.Move(moveX, 65 );
		lrcRect.Resize(0-moveX, 15-lrcRect.Height());
		iPageLable->SetExtent( lrcRect.iTl, lrcRect.Size() );		
	}

		
	if( iForm )
		iForm->SetExtent( Rect().iTl, Rect().Size() );	
    }

// ---------------------------------------------------------
// CFunSMusicContainer2::CountComponentControls() const
// ---------------------------------------------------------
//
TInt CMyleftCenterContainerSearch::CountComponentControls() const
    {
    return 1; // return nbr of controls inside this container
    }

// ---------------------------------------------------------
// CFunSMusicContainer2::ComponentControl(TInt aIndex) const
// ---------------------------------------------------------
//
CCoeControl* CMyleftCenterContainerSearch::ComponentControl(TInt aIndex) const
    {
    switch ( aIndex )
        {
        case 0:
		return iForm;
 
        default:
            return NULL;
        }
    }


// ---------------------------------------------------------
// CFunSMusicContainer2::Draw(const TRect& aRect) const
// ---------------------------------------------------------
//
void CMyleftCenterContainerSearch::Draw(const TRect& aRect) const
{
	CWindowGc& gc = SystemGc();
#if 1	
	gc.BitBlt(Rect().iTl, iMainBackImg );
#else
	gc.SetPenStyle( CGraphicsContext::ESolidPen );
	gc.SetPenColor(KRgbRect);
	//gc.SetBrushColor( TRgb(245,245,245) );
	gc.SetBrushColor( KRgbContainer);
	gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
    	gc.DrawRect( aRect );
#endif
	if(iKeyImg)
	{
		int x=0;
		TSize  iImgSize=iKeyImg->SizeInPixels();
		TSize  iSize=Rect().Size();
		x=(iSize.iWidth -iImgSize.iWidth)/2;
		gc.BitBlt( TPoint(x,KTitleHight_Len), iKeyImg);
	}
	
}

// ---------------------------------------------------------
// CFunSMusicContainer2::HandleControlEventL(
//     CCoeControl* aControl,TCoeEvent aEventType)
// ---------------------------------------------------------
//
void CMyleftCenterContainerSearch::HandleControlEventL(
    CCoeControl* /*aControl*/,TCoeEvent /*aEventType*/)
    {
    // TODO: Add your control event handler code here
    }

TKeyResponse CMyleftCenterContainerSearch::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
{

	TInt index=iForm->GetFocusIndex();
	if(index)
	{
		if((aKeyEvent.iCode==EKeyLeftArrow) ||(aKeyEvent.iCode==EKeyRightArrow))
			return EKeyWasNotConsumed;
	}

	if(iList->GetListItemCount()==0 && index==3 && aKeyEvent.iCode==EKeyDownArrow)
		return EKeyWasConsumed;
	
	if(iForm->Control(index)==iList)
	{
		int listCur=iList->GetCurrentSelectIndex();
		int listCount=iList->GetListItemCount();
		if((aKeyEvent.iCode==EKeyUpArrow) && (listCur==0))
		{
		//	iForm->SelectControl(-1);
			iList->SetCurrentSelectIndex(-1);
			iForm->SetFocusIndex(TReadSearchButton);
			
			DrawNow();
			return EKeyWasConsumed;
		}
			
	}
	
	return iForm->OfferKeyEventL( aKeyEvent, aType );
}

void CMyleftCenterContainerSearch::MboButtonPressed(CFunButton* aButton, TInt aCommandId)
{
	iparent->iText.SetLength(0);

	if(aCommandId==EMyleftCenterCmdAppSearchBook)
	{
		iPageEdit->GetEditText(iparent->iText);
		if(iparent->iText.Length())
			iplaycore->iSinaMgr->LaunchSearchRead(iparent->iText);
	}
	
}

void CMyleftCenterContainerSearch::MfloListItemClicked(CFunList* aButton, TInt aIndex)
{
	TPoint aPos;
	if(iList)
	{
		if(!iList->GetListItemCount())
			return ;
		iList->GetItemPos(aPos);
	}
	if(iMenuPan)
		iMenuPan->TryDisplayMenuPaneL(aPos, EFalse);	
}

void CMyleftCenterContainerSearch::ProcessMenuCommandL(TInt aCommandId)
{
	TInt index=(iplaycore->m_appUI->viewSearch->iCurPage-1)*One_Page_Lenth+ iList->GetCurrentSelectIndex();
	if(iplaycore->m_readSearch->Count()<index)
		return;
	book*templist=iplaycore->m_readSearch->At(index);
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

void CMyleftCenterContainerSearch::SetUpHotlist()
{
	

	TInt itemCount=0;
	TInt aStartId=0;
	TInt iPageId=iplaycore->m_appUI->viewSearch->iCurPage;
	TBuf<256> aString;
	_LIT(kString,"%S(%S)");

	itemCount=iList->GetListItemCount();
	iList->DeleteItems(0, itemCount);
	
	if(iPageId<1)
		return;
	
	aStartId=(iPageId-1)*One_Page_Lenth;
	itemCount=iplaycore->m_readSearch->Count();
	FC_Logsp("CFunSMusicContainerSearch::SetUpHotlist(),aStartId=%d,itemCount=%d",aStartId,itemCount);
	for(TInt i=aStartId;i<itemCount && i<aStartId+One_Page_Lenth;i++)
	{
		book * list=iplaycore->m_readSearch->At( i );
		aString.Format(kString(),&(list->name),&(list->auth));
		iList->AppendItemL(aString);
		FC_Logsp("CFunSMusicContainerSearch::SetUpHotlist(),AppendItemL=%d",i);
	}

	iList->SetCurrentSelectIndex(0);

}


void CMyleftCenterContainerSearch::UpDateHotlist()
{
	SetUpHotlist();
	DrawNow();
}

void CMyleftCenterContainerSearch::SetPageNum(TInt iPageId)
{
	TBuf<12> aString;
	TBuf<12> pageth;
	CEikonEnv::Static()->ReadResource (aString, R_TEXT_VIEW_SEARCH_PAGE_NUM);
	pageth.Format(aString,iPageId);
	iPageLable->ChangeTextL(pageth,ETrue);
}


