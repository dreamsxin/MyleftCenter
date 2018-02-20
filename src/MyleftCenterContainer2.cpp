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
#include "MyleftCenterContainer2.h"

#include <eiklabel.h>  // for example label control
#include <utf.h>
#include <MyleftCenter.mbg>
#include  <MyleftCenter.rsg>
#include "FunUIStart.h"
#include "const.h"
#include  "MyleftCenterViewRead.h"
#include  "MyleftCenterView2.h"
#include "centercore.h"


#ifdef _PIC_176X220_
#define KRead_PicW	68
#define KRead_PicH	84
#else
#define KRead_PicW	96
#define KRead_PicH	120
#endif
// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMyleftCenterContainer2::ConstructL(const TRect& aRect)
// EPOC two phased constructor
// ---------------------------------------------------------
//
void CMyleftCenterContainer2::ConstructL(const TRect& aRect)
    {
    CreateWindowL();
	FC_Log("CMyleftCenterContainer2::ConstructL");
	iplaycore=CenterCore::GetInstance();
	
	iMainBackImg=FunUiLib::CreateBitmapL(KMusicMultiBitmapFilename,EMbmMyleftcenterRcover1);
	if(!iMainBackImg)
		return;
	
	TInt iconIds[] = {EMbmMyleftcenterRcover1};
	TInt maskIds[] = {EMbmMyleftcenterRcover1_mask};

	iMaksList= CFunList::NewL( aRect, this, FunUiLib::SmallFont());
//	iMaksList->SetupGraphicIcons( KMusicMultiBitmapFilename, iconIds, maskIds, 1, 2, 96, 120);
	iMaksList->BackDrawer()->SetBackgroundColor( KRgbContainer);
//	iMaksList->BackDrawer()->SetBackgroundImageL( KMusicMultiBitmapFilename, EMbmMyleftcenterMain_back );
//	iMaksList->BackDrawer()->SetRoundRectBorder(KRgbContainerDeep, TSize(2,1),1);
	iMaksList->ActivedBackDrawer()->SetBackgroundColor(  TRgb(248,227,133));
	iMaksList->ActivedBackDrawer()->SetRoundRectBorder(TRgb(254,254,254), TSize(2,1),1);
	iMaksList->SetTextColors( TRgb(0,160,0), TRgb(255,128,0),  TRgb(255,128,0));
//	iMaksList->ChangeItemHeight(KReadinfo_ItemH);
//	iMaksList->ChangeItemHeight(aRect.Height()/2-2);
	iMaksList->SetDisplayNumber(1);
	iMaksList->SetObserver(this);
/*
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
*/
	
	SetupListItemsL();
	if(iMaksList->GetListItemCount())
		SetupCurReaderInfo(0);
	FC_Log("CMyleftCenterContainer2::ConstructL ~~");
    SetRect(aRect);
    ActivateL();
    }

// Destructor
CMyleftCenterContainer2::~CMyleftCenterContainer2()
{
	if(iMaksList)
		delete iMaksList;
    if(iMainBackImg)
		delete iMainBackImg;
}

// ---------------------------------------------------------
// CMyleftCenterContainer2::SizeChanged()
// Called by framework when the view size is changed
// ---------------------------------------------------------
//
void CMyleftCenterContainer2::SizeChanged()
{/*
    	if( iMaksList )
	{
	
		TRect listRect= Rect();		
		iMaksList->SetExtent( listRect.iTl, listRect.Size() );
		
	}*/
	if( iMaksList )
	{
	
		TRect listRect= Rect();
	//	listRect.Move( 1, 0 );
		listRect.Resize( 0, -KRead_PicH);
		iMaksList->SetExtent( listRect.iTl, listRect.Size() );
		
	}

	despRect= Rect();
	TInt moveY=Rect().Height()-KRead_PicH;
	despRect.Move( 1+KRead_PicW, moveY );
	despRect.Resize( -KRead_PicW-1, -moveY);

	FC_Log("CMyleftCenterContainer2::SizeChanged() ~~");
}

// ---------------------------------------------------------
// CMyleftCenterContainer2::CountComponentControls() const
// ---------------------------------------------------------
//
TInt CMyleftCenterContainer2::CountComponentControls() const
    {
    return 1; // return nbr of controls inside this container
    }

// ---------------------------------------------------------
// CMyleftCenterContainer2::ComponentControl(TInt aIndex) const
// ---------------------------------------------------------
//
CCoeControl* CMyleftCenterContainer2::ComponentControl(TInt aIndex) const
    {
    switch ( aIndex )
        {
        case 0:
		return iMaksList;
        default:
            return NULL;
        }
    }

// ---------------------------------------------------------
// CMyleftCenterContainer2::Draw(const TRect& aRect) const
// ---------------------------------------------------------
//
void CMyleftCenterContainer2::Draw(const TRect& aRect) const
{
    	CWindowGc& gc = SystemGc();

	if(iMainBackImg)
		gc.BitBlt(TPoint(1,Rect().Height()-KRead_PicH), iMainBackImg );  


	TRect infoRect= Rect();
	TInt moveY=Rect().Height()-KRead_PicH;
	infoRect.Move( 0, moveY );
	infoRect.Resize( 0, -moveY);

	gc.SetPenStyle( CGraphicsContext::ESolidPen );
	gc.SetPenColor(TRgb(1,1,1));
	gc.SetBrushStyle( CGraphicsContext::ENullBrush );
	gc.SetBrushColor( KRgbContainerDeep);
    	gc.DrawRect( infoRect );


	gc.SetPenStyle( CGraphicsContext::ESolidPen );
	gc.SetPenColor(TRgb(1,1,1));
	gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
    	gc.DrawRect( despRect );
//Drew text
	CFont* despFont=FunUiLib::SmallFont();
	gc.UseFont(despFont);
	gc.SetPenStyle( CGraphicsContext::ESolidPen );
	gc.SetPenColor(TRgb(254,254,254));
	gc.SetBrushStyle( CGraphicsContext::ENullBrush );
	
	TRect despRect2=despRect;
	TInt despTextHeight = despFont->HeightInPixels()+2;
	TInt start=0;
	TInt lineCount=despRect.Height()/despTextHeight;
	despRect2=despRect;
	despRect2.Move( 2,0);
	despRect2.Resize( -4, 0);
	TInt textLen = despFont->TextCount( iInfo, despRect.Size().iWidth-1);
	
	while((start<iInfo.Length()-1) && lineCount>0)
	{
		gc.DrawText( iInfo.Mid(start,textLen), despRect2, despTextHeight );
		start+=textLen;
		textLen = despFont->TextCount( iInfo.Mid(start), despRect.Size().iWidth-1);
		despRect2.Move( 0,despTextHeight);
		lineCount--;
	}
	gc.DiscardFont();
	
	FC_Log("CMyleftCenterContainer2::Draw()~~~ ");
}

TKeyResponse CMyleftCenterContainer2::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
{
	TKeyResponse ret= EKeyWasNotConsumed;
	switch(aKeyEvent.iCode)
	{
		case EKeyBackspace:
		{
			int curId=iMaksList->GetCurrentSelectIndex();

			iplaycore->Read_MaskListDelete(curId);
			UpdateListItemsL();
			ret=EKeyWasConsumed;
            	}
		break;
		default :
			break;
	}

	
	if(ret==EKeyWasNotConsumed)
	{
		if ( iMaksList )
		{
			ret= iMaksList->OfferKeyEventL( aKeyEvent, aType );

			if(aKeyEvent.iCode==EKeyDownArrow||aKeyEvent.iCode==EKeyUpArrow)
			{
				TInt curId=iMaksList->GetCurrentSelectIndex();
				SetupCurReaderInfo(curId);
				DrawNow();
			}
			return ret;
		}
	}

	
	return( EKeyWasNotConsumed );
}

void CMyleftCenterContainer2::MfloListItemClicked(CFunList* aButton, TInt aIndex)
{
	read_masklist * list=iplaycore->m_readMasklist->At(aIndex);
	iplaycore->Read_SetRead(list->iaskurl,list->title,list->iPos);		
	iplaycore->m_readViewid=KView2Id;
	iplaycore->m_appUI->ActivateLocalViewL(KViewReadId);
}
void CMyleftCenterContainer2::SetupListItemsL()
{
	_LIT (KString,"%d%S");
	TBuf<128> aString;
	TInt downCount=0;
	TInt downingCount=0;
	RFs			m_fs;
	RFile			m_file;

	downCount=iMaksList->GetListItemCount();
	iMaksList->DeleteItems(0, downCount);
	FC_Log("CMyleftCenterContainer2::SetupListItemsL() 2222222");
	
	if(iMaksList)
	{
		downCount=iplaycore->m_readMasklist->Count();

		for (TInt i = 0; i< downCount; i++)
		{	
			TBuf<150> 	item;
			TBuf<150>	itemStatus;
			TBuf8<150> 	bitbuf;
			TInt m_codetype;
			TInt m_offset;
			read_masklist * list=iplaycore->m_readMasklist->At(i);
			
			aString.Copy(list->title);
			FC_LogT("CMyleftCenterContainer2::SetupListItemsL() %s",aString);
			//iMaksList->AppendItemL(aString, itemStatus, picid,NULL);
			iMaksList->AppendItemL(aString);
		}	
	}
	
	FC_Log(" ~~CMyleftCenterContainer2::SetupListItemsL() ");
}


void CMyleftCenterContainer2::UpdateListItemsL()
{
	SetupListItemsL();
	DrawNow();
}

void CMyleftCenterContainer2::SetupCurReaderInfo(TInt index)
{
	RFs			m_fs;
	RFile			m_file;
	TInt m_codetype;
	TInt m_offset;
	TBuf8<ReaderInfo_Size> 	bitbuf;
	
	User::LeaveIfError(m_fs.Connect());
	read_masklist * list=iplaycore->m_readMasklist->At(index);

	m_codetype=FC_TextHeadParse(list->iaskurl,m_offset);

	if (KErrNone != m_file.Open (m_fs,list->iaskurl,EFileShareReadersOnly )) 
	{
		iplaycore->Read_MaskListDelete(index);
		iInfo.Copy(_L("Not Exist!"));
		FC_Log("SetupCurReaderInfo Not Exist");
		return ;
	}
	
	m_file.Read(list->iPos,bitbuf,ReaderInfo_Size);
	
	if(m_codetype==ETextCodeANSI)
	{
		_LIT8(KDelete,"\r\n");
		int pos=bitbuf.Find(KDelete);
		while(pos>0)
		{
			bitbuf.Delete(pos,2);
			pos=bitbuf.Find(KDelete);
		}

		 FC_GbkToUnicodeL(iInfo,bitbuf);
	}
	else if(m_codetype==ETextCodeUTF8)
	{
		_LIT8(KDelete,"\r\n");
		int pos=bitbuf.Find(KDelete);
		while(pos>0)
		{
			bitbuf.Delete(pos,2);
			pos=bitbuf.Find(KDelete);
		}

		CnvUtfConverter::ConvertToUnicodeFromUtf8(iInfo,bitbuf);
	}
	else if(m_codetype==ETextCodeUTF16B)
	{
	
		TBuf8<4> KDelete;
		KDelete.Copy(_L8("\r"));
		KDelete.Append(TChar(0x00));
		KDelete.Append(_L8("\n"));
		KDelete.Append(TChar(0x00));
		
		int pos=bitbuf.Find(KDelete);
		while(pos>0)
		{
			bitbuf.Delete(pos,4);
			pos=bitbuf.Find(KDelete);
		}

		char* pBuf8=(char* )bitbuf.Ptr();
		TPtr pBuf16(NULL,0,0);
		pBuf16.Set((TUint16 *)pBuf8,bitbuf.Length()/2,ReaderInfo_Size);
		iInfo.Copy(pBuf16);
	}

	TParse parser;
	TInt picid=0;
	TBuf<256> desPaht;
	parser.Set( list->iaskurl, NULL, NULL );
	iplaycore->GetDownPicPath(desPaht);
	desPaht.Append(parser.Name());
	desPaht.Append(_L(".jpg"));
	FC_LogT("11111111111111111 pic=%s",desPaht);
	if(iMainBackImg)
		delete iMainBackImg;
	iMainBackImg=NULL;
	if(CenterCore::IsFileExist(desPaht))
	{
		iMainBackImg=FunUiLib::LoadImageFromFileL(desPaht);
	#ifdef _PIC_176X220_
		FunUiLib::ScaleBitmapL(iMainBackImg,TSize(KRead_PicW,KRead_PicH));
	#endif
	}
	else
	{
		iMainBackImg=FunUiLib::CreateBitmapL(KMusicMultiBitmapFilename,EMbmMyleftcenterRcover1);
	//	FunUiLib::ScaleBitmapL(iMainBackImg,TSize(KRead_PicW,KRead_PicH));
	}

	m_file.Close();
	m_fs.Close();

}
