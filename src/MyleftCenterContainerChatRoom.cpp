/*
============================================================================
 Name        : CMyleftCenterContainer2 from MyleftCenterContainer2.cpp
 Author      : yilizheng
 Version     :
 Copyright   : MyleftCenter
 Description : Container control implementation
============================================================================
*/

// INCLUDE FILES
#include "MyleftCenterContainerChatRoom.h"
#include "MyleftCenterViewChatRoom.h"

#include <eiklabel.h>  // for example label control
#include <AknUtils.h>  //for fount
#include <MyleftCenter.rsg>
#include <utf.h>
#include <gdi.h>
#include <CHARCONV.H>

#include "const.h"
#include "CenterCore.h"
#include "portab_symbian.h"
#if 1
// ================= MEMBER FUNCTIONS =======================


void CMyleftCenterContainerChatRoom::ConstructL(CMyleftCenterViewChatRoom *parent,const TRect& aRect)
    {
    CreateWindowL();
	
	iplaycore=CenterCore::GetInstance();
	iparent=parent;

//	iWidth=aRect.Size().iWidth;


	iList = CFunList::NewL( aRect, this, FunUiLib::TinyFont() );
	iList->SetObserver(this);
	iList->BackDrawer()->SetBackgroundColor( KRgbContainerDeep);
	iList->ActivedBackDrawer()->SetBackgroundColor( KRgbRect);
	iList->ActivedBackDrawer()->SetNoBorder();
	iList->SetTextColors( TRgb(254,254,254), TRgb(254,254,254),  TRgb(254,254,254));
//	iList->SetDisplayNumber(1);

	iPageEdit= CFunEdit::NewL(TRect(100,KTitleHight_Len-5,100+KEdit_Width,KTitleHight_Len-5+KEdit_Hight) , this,  iparent->iText);
	iPageEdit->ChangeFrameColor(TRgb(255,255,255),TRgb(255,255,255));
	iPageEdit->SetFocus(TRUE);

	iPageLable=CFunLabel::NewL( aRect, this, _L("All"), FunUiLib::TinyFont());
	iPageLable->SetTransparent( ETrue );
	iPageLable->SetTextColor(TRgb(1,1,1));
//	iPageLable->SetShadowColor(KRgbContainerDeep);

	m_Commlist= new (ELeave) CArrayFixFlat<SLine*> (4);
	m_Prevlist= new (ELeave) CArrayFixFlat<SLine*> (4);

	m_font = NULL;
	TFontSpec fontSpec=FunUiLib::NormalFont()->FontSpecInTwips();
	fontSpec.iHeight =80;
	CEikonEnv::Static()->ScreenDevice()->GetNearestFontInTwips(m_font,fontSpec);
	//m_font=FunUiLib::SmallFont();
	m_fontH=m_font->HeightInPixels()+3;

	for(int i=0;i<Max_line;i++)
	{
		SLine *tmpbody=new SLine();
		tmpbody->m_Buf.SetLength(0);
		m_Commlist->AppendL(tmpbody);
	}

	for(int i=0;i<Max_line;i++)
	{
		SLine *tmpbody=new SLine();
		tmpbody->m_Buf.SetLength(0);
		m_Prevlist->AppendL(tmpbody);
	}

	SetupListItemsL();
	
/*
	iMainBackImg=FunUiLib::CreateBitmapL(KMusicMultiBitmapFilename,EMbmMyleftcenterMain_back);
	if(!iMainBackImg)
		return;

	int x=0;
//	TSize  iImgSize=iKeyImg->SizeInPixels();
	TSize  iSize=aRect.Size();
	x=(iSize.iWidth -KEdit_Width)/2;
	

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

 */

    SetRect(aRect);
    ActivateL();

    }

// Destructor
CMyleftCenterContainerChatRoom::~CMyleftCenterContainerChatRoom()
{

   	 if(iList)
	  delete iList;

	if(iPageEdit)
		delete iPageEdit;
	if(iPageLable)
		delete iPageLable;

	if(m_Commlist)
	{
		for(int i=0;i<m_Commlist->Count();i++)
		{
			SLine* body=m_Commlist->At(i);
			delete body;
		}
		delete m_Commlist;
	}

	if(m_Prevlist)
	{
		for(int i=0;i<m_Prevlist->Count();i++)
		{
			SLine* body=m_Prevlist->At(i);
			delete body;
		}
		delete m_Prevlist;
	}
	
	if(m_font)
		CEikonEnv::Static()->ScreenDevice()->ReleaseFont(m_font);

}

// ---------------------------------------------------------
// CFunSMusicContainer2::SizeChanged()
// Called by framework when the view size is changed
// ---------------------------------------------------------
//
void CMyleftCenterContainerChatRoom::SizeChanged()
    {

	TInt width=Rect().Width();
	TInt height=Rect().Height();
	iListW=(width-10)/4; //5 rect
	iContentW=width-10-iListW;

	iListH=height-10-KEdit_Hight;
	iComH=(iListH-2)*8/10;
	iPrivH=iListH-2-iComH;
    	if(iList)
    	{
    		TRect listRect= Rect();
		listRect.Move( 4, 4);
		listRect.SetHeight(iListH);
		listRect.SetWidth(iListW);
		iList->SetExtent( listRect.iTl, listRect.Size() );
	}
		
	if(iPageEdit)
	{
    		TRect listRect= Rect();
		listRect.Move( 6+iListW, 6+iListH);
		listRect.SetHeight(KEdit_Hight);
		listRect.SetWidth(iContentW);	
		iPageEdit->SetExtent( listRect.iTl, listRect.Size() );
	}
	
	if( iPageLable)
	{	
		iPagelabR= Rect();		
		iPagelabR.Move(4, 6+iListH );
		iPagelabR.SetHeight(KEdit_Hight);
		iPagelabR.SetWidth(iListW);	
		iPageLable->SetExtent( iPagelabR.iTl, iPagelabR.Size() );		
	}
	
	iContentAllR= Rect();		
	iContentAllR.Move( 6+iListW, 4);
	iContentAllR.SetHeight(iComH);
	iContentAllR.SetWidth(iContentW);	

	iContentPrevR= Rect();		
	iContentPrevR.Move( 6+iListW, 6+iComH);
	iContentPrevR.SetHeight(iPrivH);
	iContentPrevR.SetWidth(iContentW);	

	m_CommLCount=iContentAllR.Height()/m_fontH;	
	m_PrevLCount=iContentPrevR.Height()/m_fontH;	

    }

// ---------------------------------------------------------
// CFunSMusicContainer2::CountComponentControls() const
// ---------------------------------------------------------
//
TInt CMyleftCenterContainerChatRoom::CountComponentControls() const
    {
    return 3; // return nbr of controls inside this container
    }

// ---------------------------------------------------------
// CFunSMusicContainer2::ComponentControl(TInt aIndex) const
// ---------------------------------------------------------
//
CCoeControl* CMyleftCenterContainerChatRoom::ComponentControl(TInt aIndex) const
    {
    switch ( aIndex )
        {
        case 0:
		return iList;
         case 1:
		return iPageEdit;
         case 2:
		return iPageLable;

        default:
            return NULL;
        }
    }


// ---------------------------------------------------------
// CFunSMusicContainer2::Draw(const TRect& aRect) const
// ---------------------------------------------------------
//
void CMyleftCenterContainerChatRoom::Draw(const TRect& aRect) const
{
	CWindowGc& gc = SystemGc();
	gc.Clear(Rect());
	
	gc.SetPenStyle( CGraphicsContext::ESolidPen );
	gc.SetBrushStyle( CGraphicsContext::ENullBrush );
	gc.SetPenColor( KRgbContainerDeep);
	//gc.SetBrushColor( KRgbContainerDeep);
	
	gc.DrawRect( Rect() );

	gc.SetPenColor( KRgbRect);
	TRect rcet=Rect();
	rcet.Move( 1, 1 );
	rcet.Resize( -2, -2);
	gc.DrawRect( rcet );

	gc.SetPenColor( KRgbContainer);
	rcet.Move( 1, 1 );
	rcet.Resize( -2, -2);
	gc.DrawRect( rcet );

	gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
	gc.SetPenColor( KRgbContainer);
	gc.SetBrushColor( KRgbContainerDeep);
	rcet=iPagelabR;
	rcet.Move( -1, -1 );
	rcet.Resize( 2, 2);
	gc.DrawRect( rcet );

	rcet=iContentAllR;
	rcet.Move( -1, -1 );
	rcet.Resize( 2, 2);
	gc.DrawRect( rcet );

	rcet=iContentPrevR;
	rcet.Move( -1, -1 );
	rcet.Resize( 2, 2);
	gc.DrawRect( rcet );

/*
//drew content text
	gc.SetPenColor(TRgb(1,1,1));
//Drew text
	CFont* despFont=FunUiLib::SmallFont();
	gc.UseFont(despFont);
	
	TRect despRect=iContentAllR;
	despRect.Move( 2,2);
	despRect.Resize( -4, -4);
	TInt despTextHeight = despFont->HeightInPixels()+2;
	TInt start=0;
	TInt i=0;
	while( i<m_CommLCount)
	{
		gc.DrawText(m_Commlist->At(i)->m_Buf, despRect, despTextHeight );
		despRect.Move( 0,despTextHeight);
		i++;
	}
	gc.DiscardFont();
*/
}

void CMyleftCenterContainerChatRoom:: ShowComm(TBool _isComm)
{
	
	ActivateGc();
	
	CWindowGc& gc = SystemGc();

	gc.SetPenStyle( CGraphicsContext::ESolidPen );
	gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
	gc.SetPenColor( KRgbRect);
	gc.SetBrushColor( KRgbContainerDeep);

	TRect rcet=iContentAllR;
	TRect despRect=iContentAllR;
	if(!_isComm)
	{
		rcet=iContentPrevR;
		despRect=iContentPrevR;
	}
//	rcet.Move( -1, -1 );
//	rcet.Resize( 2, 2);
	gc.DrawRect( rcet );

	gc.SetPenColor(TRgb(1,1,1));
	gc.UseFont(m_font);

	despRect.Move( 2,2);
	despRect.Resize( -4, -4);
	TInt start=0;
	TInt i=0;
	if(_isComm)
	{
		while( i<m_CommLCount)
		{
			if(!m_Commlist->At(i)->m_Buf.Length())
				break;
			gc.DrawText(m_Commlist->At(i)->m_Buf, despRect, m_fontH);
			despRect.Move( 0,m_fontH);
			despRect.Resize( 0,-m_fontH);
			i++;
				
		}
	}
	else
	{
		while( i<m_PrevLCount)
		{
			if(!m_Prevlist->At(i)->m_Buf.Length())
				break;
			gc.DrawText(m_Prevlist->At(i)->m_Buf, despRect, m_fontH);
			despRect.Move( 0,m_fontH);
			i++;
		}
	}
	gc.DiscardFont();

	
	DeactivateGc();
}


TKeyResponse CMyleftCenterContainerChatRoom::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
{
	TKeyResponse ret =EKeyWasNotConsumed;
	if ( iList)
	{
		ret= iList->OfferKeyEventL( aKeyEvent, aType );
	}

	if( iPageEdit && iPageEdit->IsFocused() )        //若当前控件获得焦点，处理按键
	{
   		// ret= iPageEdit->OfferKeyEventL( aKeyEvent, aType );
		 if( aKeyEvent.iCode==EKeyDevice3 )
		 {
			iPageEdit->GetEditText(iparent->iText);
			if(iparent->iText.Length())
			{
				TInt i=iList->GetCurrentSelectIndex();
				iplaycore->iChartRData->SendMessage(iList->GetItemText(i),iparent->iText);
			}
		
		 }
	}
	
		if(ret==EKeyWasConsumed)
		{
			if( aKeyEvent.iCode==EKeyUpArrow ||aKeyEvent.iCode==EKeyDownArrow)
			{
				TInt i=iList->GetCurrentSelectIndex();
				iPageLable->ChangeTextL(iList->GetItemText(i),ETrue);
			}
		}

		
		return( ret );

}

void CMyleftCenterContainerChatRoom::MfloListItemClicked(CFunList* aButton, TInt aIndex)
{
	TPoint aPos;
	if(iList)
	{
		if(!iList->GetListItemCount())
			return ;
		iList->GetItemPos(aPos);
	}
}

void CMyleftCenterContainerChatRoom::SetupListItemsL()
{
	_LIT (KString,"%d%S");
	TBuf<128> aString;
	TInt iCount=0;
	TInt downingCount=0;
	RFs			m_fs;
	RFile			m_file;

	iCount=iList->GetListItemCount();
	iList->DeleteItems(0, iCount);
	FC_Log("CMyleftCenterContainerChatRoom::SetupListItemsL() 2222222");
	
	if(iList)
	{
		iCount=iplaycore->m_CMBody->Count();
		iList->AppendItemL(_L("All"));
		for (TInt i = 0; i< iCount; i++)
		{	
			TBuf<150> 	item;
			TBuf<150>	itemStatus;
			TBuf8<150> 	bitbuf;
			TInt m_codetype;
			TInt m_offset;
			chatR_body* list=iplaycore->m_CMBody->At(i);
			
			aString.Copy(list->name);
			FC_LogT("CMyleftCenterContainerChatRoom::SetupListItemsL() %s",aString);
			//iMaksList->AppendItemL(aString, itemStatus, picid,NULL);
			iList->AppendItemL(aString);
		}	
	}
	
	FC_Log(" ~CMyleftCenterContainerChatRoom::SetupListItemsL() ");
}


void CMyleftCenterContainerChatRoom::UpdateListItemsL()
{
	SetupListItemsL();
	DrawNow();
}

void CMyleftCenterContainerChatRoom::PushChatCommL(TBool _isComm)
{
	_LIT (KString,"(%02d:%02d:%02d)%S:%S");
	TBuf<256>  aString;
	
	TInt count=iplaycore->m_CMCommlog->Count();
	TInt maxShowCount=m_CommLCount;
	if(!_isComm)
	{
		count=iplaycore->m_CMPrevlog->Count();
		maxShowCount=m_PrevLCount;
		for(TInt i=0;i<m_Prevlist->Count() ;i++)
		{
			m_Prevlist->At(i)->m_Buf.SetLength(0);
		}
	}
	else{
		for(TInt i=0;i<m_Commlist->Count() ;i++)
		{
			m_Commlist->At(i)->m_Buf.SetLength(0);
		}
	}
	TInt logid=count-1;
	
	for(TInt i=0;i<maxShowCount && logid>=0;i++)
	{
		chatR_log *temp=iplaycore->m_CMCommlog->At(logid);
		if(!_isComm)
			temp=iplaycore->m_CMPrevlog->At(logid);
		TInt start=0;
		TInt hour=(temp->time/3600+8)%24;
		TInt mine=(temp->time/60)%60;
		TInt sec=temp->time%60;
		aString.Format(KString(),hour,mine,sec,&(temp->name),temp->a_chatlog);
		TInt textLen =m_font->TextCount( aString, iContentW);
		while(start<aString.Length())
		{
			if(_isComm)
				m_Commlist->At(i)->m_Buf=aString.Mid(start,textLen);
			else
				m_Prevlist->At(i)->m_Buf=aString.Mid(start,textLen);
			i++;
			start+=textLen;
			textLen =m_font->TextCount( aString.Mid(start), iContentW);
		}
		i--;
		logid--;
	}
	
	ShowComm(_isComm);
}

/*
void CMyleftCenterContainerChatRoom::PushChatPrevL()
{
	_LIT (KString,"(%02d:%02d:%02d)%S:%S");
	TBuf<256>  aString;
	TInt count=iplaycore->m_CMPrevlog->Count();
	TInt logid=count-1;

	for(TInt i=0;i<m_Prevlist->Count() ;i++)
	{
		m_Prevlist->At(i)->m_Buf.SetLength(0);
	}

	for(TInt i=0;i<m_PrevLCount && logid>=0;i++)
	{
		chatR_log *temp=iplaycore->m_CMPrevlog->At(logid);
		TInt start=0;
		TInt hour=(temp->time/3600+8)%24;
		TInt mine=(temp->time/60)%60;
		TInt sec=temp->time%60;
		aString.Format(KString(),hour,mine,sec,&(temp->name),temp->a_chatlog);
		TInt textLen =m_font->TextCount( aString, iContentW);
		while(start<aString.Length())
		{
			m_Prevlist->At(i)->m_Buf=aString.Mid(start,textLen);
			i++;
			start+=textLen;
			textLen =m_font->TextCount( aString.Mid(start), iContentW);
		}
		i--;
		logid--;
	}
	
	ShowPrev();
}
*/
#endif
