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
#include "MyleftCenterContainerRead.h"

#include <eiklabel.h>  // for example label control
#include <AknUtils.h>  //for fount
#include <MyleftCenter.rsg>
#include <utf.h>
#include <gdi.h>
#include <CHARCONV.H>

#include "const.h"
#include "CenterCore.h"
#include "portab_symbian.h"
// ================= MEMBER FUNCTIONS =======================


CMyleftCenterContainerRead::CMyleftCenterContainerRead():m_isEnd(EFalse),m_pBuf(NULL,0,0)
{
}
// ---------------------------------------------------------
// CMyleftCenterContainer2::ConstructL(const TRect& aRect)
// EPOC two phased constructor
// ---------------------------------------------------------
//
void CMyleftCenterContainerRead::ConstructL(const TRect& aRect)
    {
    CreateWindowL();
	
	iCenterCore=CenterCore::GetInstance();
	
	m_outtime=iCenterCore->m_readerSetting.autoTime;

	TRgbBack=TRgb(iCenterCore->m_readerSetting.backColor.R, iCenterCore->m_readerSetting.backColor.G, iCenterCore->m_readerSetting.backColor.B) ;
	TRgbFont=TRgb(iCenterCore->m_readerSetting.textColor.R, iCenterCore->m_readerSetting.textColor.G, iCenterCore->m_readerSetting.textColor.B) ;

	TRgbBack=TRgb(iCenterCore->m_readerSetting.backColor.R,iCenterCore->m_readerSetting.backColor.G,iCenterCore->m_readerSetting.backColor.G) ;
	TRgbFont=TRgb(iCenterCore->m_readerSetting.textColor.R,iCenterCore->m_readerSetting.textColor.G,iCenterCore->m_readerSetting.textColor.B) ;	
	m_clolr[0]	=TRgbFont.Red()/KColorSetp;
	m_clolr[1]	=TRgbFont.Green()/KColorSetp;
	m_clolr[2]	=TRgbFont.Blue()/KColorSetp;
	m_clolr[3]	=TRgbBack.Red()/KColorSetp;
	m_clolr[4]	=TRgbBack.Green()/KColorSetp;
	m_clolr[5]	=TRgbBack.Blue()/KColorSetp;

	if(iCenterCore->m_readerSetting.bBcakPic)
	{
		TBuf<256> filename;
		TInt ret=CenterCore::GetBackPic(iCenterCore->m_readerSetting.backPicId, filename);
		if(ret)
		{
			iBackImg=FunUiLib::LoadImageFromFileL(filename);
		}
	}
	
	m_codetype=FC_TextHeadParse(iCenterCore->m_readUrl,m_offset);

	if(m_codetype==ETextCodeANSI)
	{
		if(!m_converter)
		{
			m_converter=CCnvCharacterSetConverter::NewL ();
			m_converter->PrepareToConvertToOrFromL (KCharacterSetIdentifierGbk, CEikonEnv::Static()->FsSession());
		}
	}

	if(iCenterCore->m_readStatus)
	{
		User::LeaveIfError(m_fs.Connect());
		User::LeaveIfError(m_file.Open(m_fs,iCenterCore->m_readUrl,EFileShareReadersOnly ));
		m_bPos=iCenterCore->m_readPos;
	}
	iCenterCore->Read_MaskListAppend(iCenterCore->m_readUrl,iCenterCore->m_readTitle,iCenterCore->m_readPos);
	
	m_lineBodylist= new (ELeave) CArrayFixFlat<SLineBody*> (4);
	
	m_font = NULL;
	TFontSpec fontSpec=FunUiLib::NormalFont()->FontSpecInTwips();
	if(!iCenterCore->m_readerSetting.textSize)
		iCenterCore->m_readerSetting.textSize=160;
	fontSpec.iHeight =iCenterCore->m_readerSetting.textSize;
	CEikonEnv::Static()->ScreenDevice()->GetNearestFontInTwips(m_font,fontSpec);
	
	m_fontH=m_font->HeightInPixels()+3;
	m_fontW=m_font->TextWidthInPixels(iCenterCore->TextResource(R_TEXT_POPMENU_PARAME_SETTING))/4;
	m_lineBuf=aRect.Width()/m_fontW;
	m_lineCount=aRect.Height()/m_fontH;
	m_BufSize=m_lineBuf*2;
	
	for(int i=0;i<m_lineCount;i++)
	{
		SLineBody *tmpbody=new SLineBody();
		tmpbody->m_Buf.SetLength(0);
		m_lineBodylist->AppendL(tmpbody);
	}

	
	if(iCenterCore->m_readStatus)
		GetTextByLine(EbookReadRoad);

#ifdef _S60_3RD_ 
	iLight = CHWRMLight::NewL();
#endif

	if(iCenterCore->m_readerSetting.autoRool)
	{
		iTimer = CFunTimeOutTimer::NewL( 0, *this );
		iTimer->After( 1000000*m_outtime  );
	}
    SetRect(aRect);
    ActivateL();
    }

// Destructor
CMyleftCenterContainerRead::~CMyleftCenterContainerRead()
{
    	if( iTimer && iTimer->IsActive() )
		iTimer->Cancel();
	delete iTimer;
	
	if(iBackImg)
		delete iBackImg;
	
	if(iTempBackImg)
		delete iTempBackImg;
	
	if(m_Buf)
		delete m_Buf;
	
	if(iCenterCore->m_readStatus)
	{
		m_file.Close();
		m_fs.Close();
	}

	if(m_lineBodylist)
	{
		for(int i=0;i<m_lineBodylist->Count();i++)
		{
			SLineBody* body=m_lineBodylist->At(i);
			delete body;
		}
		delete m_lineBodylist;
	}

	if(m_font)
		CEikonEnv::Static()->ScreenDevice()->ReleaseFont(m_font);

	if(m_converter)
		delete m_converter;

#ifdef _S60_3RD_
		if(iLight)
			delete iLight;
#endif

}

// ---------------------------------------------------------
// CMyleftCenterContainer2::SizeChanged()
// Called by framework when the view size is changed
// ---------------------------------------------------------
//
void CMyleftCenterContainerRead::SizeChanged()
{
	TInt filsize;
	m_file.Size(filsize);
	CalculateStepClass(m_bPos,	filsize);
	SetUp();
	GetTextByLine(EbookReadRoad);
}

// ---------------------------------------------------------
// CMyleftCenterContainer2::CountComponentControls() const
// ---------------------------------------------------------
//
TInt CMyleftCenterContainerRead::CountComponentControls() const
    {
    return 0; // return nbr of controls inside this container
    }

// ---------------------------------------------------------
// CMyleftCenterContainer2::ComponentControl(TInt aIndex) const
// ---------------------------------------------------------
//
CCoeControl* CMyleftCenterContainerRead::ComponentControl(TInt aIndex) const
    {
    switch ( aIndex )
        {
        case 0:
        case 1:
        default:
            return NULL;
        }
    }

// ---------------------------------------------------------
// CMyleftCenterContainerRead::MftonTimerExpired(const TRect& aRect) const
// ---------------------------------------------------------
//
 void CMyleftCenterContainerRead::MftonTimerExpired()
 {
 	LightOn();
	DrawText(EbookReadDown);
	iTimer->After( 1000000*m_outtime );
 }

void CMyleftCenterContainerRead::TimerStop()
{
	if( iTimer && iTimer->IsActive() )
		iTimer->Cancel();
}

void CMyleftCenterContainerRead::TimerStart()
{
	if(iTimer &&  ! iTimer->IsActive() && iCenterCore->m_readerSetting.autoRool)
		iTimer->After( 1000000*m_outtime  );
}

void CMyleftCenterContainerRead::LightOn () 
{
	User::ResetInactivityTime ();
#ifdef _S60_3RD_
	if(iLight->LightStatus(CHWRMLight::EPrimaryDisplay)!= CHWRMLight::ELightOn)
	{
		iLight->LightOnL(CHWRMLight::EPrimaryDisplay);
	}  
#endif
}

// ---------------------------------------------------------
// CMyleftCenterContainer2::Draw(const TRect& aRect) const
// ---------------------------------------------------------
//
void CMyleftCenterContainerRead::Draw(const TRect& aRect) const
{

    CWindowGc& gc = SystemGc();
	
	gc.Clear ( Rect() );

	//const  CFont *font = LatinPlain12();

	gc.SetBrushColor( TRgbBack );
	gc.SetBrushStyle( CGraphicsContext::ESolidBrush );

	if((m_drewType==EDrewTypeText)|| (m_drewType==EDrewTypePagebar))
	{
		if(iBackImg)
			gc.BitBlt( Rect().iTl, iBackImg );
		else
			gc.DrawRect( Rect() );
	}
	else if(m_drewType==EDrewTypePicSel)
	{
		if(iTempBackImg)
			gc.BitBlt( Rect().iTl, iTempBackImg );
	}
	
	gc.SetPenColor(TRgbFont);
	gc.SetPenStyle( CGraphicsContext::ESolidPen );
 //	gc.UseFont(LatinPlain12());
	gc.UseFont(m_font);

	if((m_drewType==EDrewTypeText) || (m_drewType==EDrewTypePagebar))
	{
		for(TInt i=0;i<m_lineCount;i++)
		{	
			SLineBody *tmpbody=m_lineBodylist->At(i);
			TBuf<100> lineBuf;
			if(m_codetype==ETextCodeANSI)
			{
				if(m_converter)
				{
					TInt _state = CCnvCharacterSetConverter::KStateDefault;
					m_converter->ConvertToUnicode (lineBuf, tmpbody->m_Buf, _state);
				}
			}
			else if(m_codetype==ETextCodeUTF8)
			{
			// 	FC_Utf8ToUnicodeL(lineBuf,tmpbody->m_Buf);
				CnvUtfConverter::ConvertToUnicodeFromUtf8(lineBuf,tmpbody->m_Buf);
			}
			else if(m_codetype==ETextCodeUTF16B)
			{
				char* pBuf8=(char* )tmpbody->m_Buf.Ptr();
				TPtr pBuf16(NULL,0,0);
				pBuf16.Set((TUint16 *)pBuf8,tmpbody->m_Buf.Length()/2,100);
				lineBuf.Copy(pBuf16);
				
			}
			gc.DrawText(lineBuf,TPoint(0,m_fontH+m_fontH*i));
			
		}
	}
	else if(m_drewType==EDrewTypePicSel)
	{
		HBufC8* hbuf8=HBufC8::NewLC(3000);
		HBufC* hbuf16=HBufC::NewLC(1500);
		TPtr8 pbuf8=hbuf8->Des();
		TPtr pbuf16=hbuf16->Des();

		RFs		fileSer; 
		RFile		file;
		fileSer.Connect();
		User::LeaveIfError(file.Open(fileSer,KReadAboutFilename,EFileShareReadersOnly ));
		file.Read(pbuf8);				
		file.Close();
		fileSer.Close();		

		CnvUtfConverter::ConvertToUnicodeFromUtf8(pbuf16,pbuf8);

		TInt textLen = m_font->TextCount( pbuf16, Rect().Width());
		TInt start=0;
		TInt i=0;
		while((start<pbuf16.Length()-1) && i<m_lineCount)
		{
			
			gc.DrawText( pbuf16.Mid(start,textLen),TPoint(0,m_fontH+m_fontH*i));
			start+=textLen;
			textLen = m_font->TextCount( pbuf16.Mid(start), Rect().Width());
			i++;
		}
		
		CleanupStack::PopAndDestroy (2);
	}

	if(m_drewType==EDrewTypePagebar)
	{
		TRect  linBackBar(TPoint(0,m_drugBar.iSY-m_fontH), TSize(Rect().Width(),m_fontH*3));
		gc.SetBrushColor( TRgbBack );
		gc.DrawRect( linBackBar);
		
		gc.SetBrushColor( TRgb(160,160,160));
		gc.SetPenColor(TRgb(160,160,160)); 
		
		TRect  linBar(TPoint(m_drugBar.iSX,m_drugBar.iSY), TSize(m_drugBar.iWidth,m_fontH));
		gc.DrawRect( linBar);

		TInt linepos=m_drugBar.iSX+m_drugBar.iDStep*m_drugBar.iNstep;
		if(m_drugBar.iNstep==m_drugBar.iClasses)
			linepos=m_drugBar.iSX+m_drugBar.iWidth-4;
		TRect  Bar(TPoint(linepos,m_drugBar.iSY), TSize(3,m_fontH));
		
		gc.SetBrushColor( TRgb(255,128,0));
		gc.SetPenColor(TRgb(255,128,0)); 
		gc.DrawRect( Bar);

		TBuf<10>  iposText;
		iposText.AppendNum(TInt64(m_drugBar.iPos));
		gc.DrawText(iposText,TPoint(linepos,m_fontH*2+m_drugBar.iSY));

	}

	if(m_drewType==EDrewTypeColorSet)
	{
		gc.DiscardFont();
		gc.UseFont(FunUiLib::NormalFont());
		TInt lineW=128,step=4;
		TInt rectSpec=Rect().Height()/10;
		TInt lineX=(Rect().Width()-128)/2;
	
		gc.SetBrushColor(  TRgb(1,1,1) );
		gc.SetPenColor(TRgb((254,254,254))); 
		gc.DrawRect( Rect());

		gc.DrawText(_L("FontColor"),TPoint(lineX-35,rectSpec*2+13));
		gc.DrawText(_L("R"),TPoint(lineX-13,rectSpec*3+13));
		gc.DrawText(_L("G"),TPoint(lineX-13,rectSpec*4+13));
		gc.DrawText(_L("B"),TPoint(lineX-13,rectSpec*5+13));

		gc.DrawText(_L("BackColor"),TPoint(lineX-35,rectSpec*6+13));
		gc.DrawText(_L("R"),TPoint(lineX-13,rectSpec*7+13));
		gc.DrawText(_L("G"),TPoint(lineX-13,rectSpec*8+13));
		gc.DrawText(_L("B"),TPoint(lineX-13,rectSpec*9+13));

		//-------line------------
		gc.SetBrushColor( TRgb(160,160,160));
		gc.SetPenColor(TRgb(160,160,160)); 

		gc.DrawRect( TRect(TPoint(lineX,rectSpec*3),TSize(lineW,5)));
		gc.DrawRect( TRect(TPoint(lineX,rectSpec*4),TSize(lineW,5)));
		gc.DrawRect( TRect(TPoint(lineX,rectSpec*5),TSize(lineW,5)));
		gc.DrawRect( TRect(TPoint(lineX,rectSpec*7),TSize(lineW,5)));
		gc.DrawRect( TRect(TPoint(lineX,rectSpec*8),TSize(lineW,5)));
		gc.DrawRect( TRect(TPoint(lineX,rectSpec*9),TSize(lineW,5)));

		//-------bar------------	
		gc.SetBrushColor( TRgb(255,128,0));
		gc.SetPenColor(TRgb(255,128,0)); 
		gc.DrawRect( TRect(TPoint(lineX+m_clolr[0]*step,rectSpec*3-1),TSize(4,7)));
		gc.DrawRect( TRect(TPoint(lineX+m_clolr[1]*step,rectSpec*4-1),TSize(4,7)));
		gc.DrawRect( TRect(TPoint(lineX+m_clolr[2]*step,rectSpec*5-1),TSize(4,7)));
		gc.DrawRect( TRect(TPoint(lineX+m_clolr[3]*step,rectSpec*7-1),TSize(4,7)));
		gc.DrawRect( TRect(TPoint(lineX+m_clolr[4]*step,rectSpec*8-1),TSize(4,7)));
		gc.DrawRect( TRect(TPoint(lineX+m_clolr[5]*step,rectSpec*9-1),TSize(4,7)));

		gc.SetBrushStyle( CGraphicsContext::ENullBrush );
		gc.SetPenColor(TRgb(254,254,254)); 
		if(m_selRgbId<3) // 0,1,2
		{
			gc.DrawRect( TRect(TPoint(lineX-18,rectSpec*(m_selRgbId+3)-10),TSize(lineW+36,26)));
		}
		else//3      
		{
			gc.DrawRect( TRect(TPoint(lineX-18,rectSpec*(m_selRgbId+4)-10),TSize(lineW+36,26)));
		}
		
		//-------refer------------
		TInt referX=Rect().Width()-90,referW=80;
		TInt referY=Rect().iTl.iY+10,referH=40;
		gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
		gc.SetPenColor(  TRgb(m_clolr[0]*KColorSetp+1,m_clolr[1]*KColorSetp+1,m_clolr[2]*KColorSetp+1));
		gc.SetBrushColor(TRgb(m_clolr[3]*KColorSetp+1,m_clolr[4]*KColorSetp+1,m_clolr[5]*KColorSetp+1)); 
		gc.DrawRect( TRect(TPoint(referX,referY),TSize(referW,referH)));
		gc.DrawText(iCenterCore->TextResource(R_TEXT_APP_NAME),TPoint(referX+5,referY+referH));

		gc.UseFont(m_font);
	}
	gc.DiscardFont();
    }

// ---------------------------------------------------------
// CMyleftCenterContainer2::HandleControlEventL(
//     CCoeControl* aControl,TCoeEvent aEventType)
// ---------------------------------------------------------
//
void CMyleftCenterContainerRead::HandleControlEventL(
    CCoeControl* /*aControl*/,TCoeEvent /*aEventType*/)
{
}

// ---------------------------------------------------------
// FunVVContainerList::OfferKeyEventL(
//     CCoeControl* aControl,TCoeEvent aEventType)
// ---------------------------------------------------------
//
TKeyResponse CMyleftCenterContainerRead::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
    {
    		TKeyResponse ret=EKeyWasNotConsumed;
		TInt code = aKeyEvent.iCode;
		switch(code)
		{
			case EKeyDevice3:
			{
				if(m_drewType==EDrewTypePagebar)	
				{
					m_drewType=EDrewTypeText;
					TimerStart();
					m_bPos=m_drugBar.iPos;
					GetTextByLine(EbookReadRoad);
					DrawNow();
				}
				else if(m_drewType==EDrewTypeColorSet)
				{
					m_drewType=EDrewTypeText;
					TimerStart();
					TRgbFont.SetRed(m_clolr[0]*KColorSetp+1);
					TRgbFont.SetGreen(m_clolr[1]*KColorSetp+1);
					TRgbFont.SetBlue(m_clolr[2]*KColorSetp+1);
					
					TRgbBack.SetRed(m_clolr[3]*KColorSetp+1);
					TRgbBack.SetGreen(m_clolr[4]*KColorSetp+1);
					TRgbBack.SetBlue(m_clolr[5]*KColorSetp+1);
					DrawNow();
				}
				else if(m_drewType==EDrewTypePicSel)
				{
					m_drewType=EDrewTypeText;
					TimerStart();
					iCenterCore->m_readerSetting.bBcakPic=1;
					iCenterCore->m_readerSetting.backPicId=m_iPicId;
					if(iBackImg)
					{
						delete iBackImg;
						iBackImg=NULL;
					}
					if(iTempBackImg)
					{
						iBackImg=iTempBackImg;
						iTempBackImg=NULL;
					}
					DrawNow();
				}
				ret= EKeyWasConsumed;
			}
			break;
			case EKeyUpArrow:
			{
				if(m_drewType==EDrewTypeText)	
				{
					DrawText(EbookReadUp);
				//	iCenterCore->m_readPos=m_bPos;
					ret= EKeyWasConsumed;
				}
				else if(m_drewType==EDrewTypeColorSet)	
				{
					m_selRgbId--;
					if(m_selRgbId<0)
						m_selRgbId=0;
					DrawColorSet();
					ret= EKeyWasConsumed;
				}
				else if(m_drewType==EDrewTypePicSel)	
				{
					TBuf<256> filename;
					if(m_iPicId>0)
					{
						m_iPicId--;
						TInt ret=CenterCore::GetBackPic(m_iPicId, filename);
						if(ret)
						{
							if(iTempBackImg)
								delete iTempBackImg;
							iTempBackImg=FunUiLib::LoadImageFromFileL(filename);
						}
						DrawNow();
					}
					ret= EKeyWasConsumed;
				}		
			}
			break;
			case EKeyDownArrow:
			{
				if(m_drewType==EDrewTypeText)	
				{
					DrawText(EbookReadDown);
				//	iCenterCore->m_readPos=m_bPos;
					ret=EKeyWasConsumed;
				}
				else if(m_drewType==EDrewTypeColorSet)	
				{
					m_selRgbId++;
					if(m_selRgbId>5)
						m_selRgbId=5;
					DrawColorSet();
					ret= EKeyWasConsumed;
				}
				else if(m_drewType==EDrewTypePicSel)	
				{
					TBuf<256> filename;
					TInt count=CenterCore::GetBackPicCount();

					if(m_iPicId<count-1)
					{
						m_iPicId++;						
						TInt ret=CenterCore::GetBackPic(m_iPicId, filename);
						if(ret)
						{
							if(iTempBackImg)
								delete iTempBackImg;
							iTempBackImg=FunUiLib::LoadImageFromFileL(filename);
						}
						DrawNow();
					}
					ret= EKeyWasConsumed;
				}				
			}
			break;
			case EKeyLeftArrow:
			{
				if(m_drewType==EDrewTypePagebar)
				{
					MoveStepBar(1);
					DrawLineBar();
					ret=EKeyWasConsumed;
				}
				else if(m_drewType==EDrewTypeColorSet)
				{
					m_clolr[m_selRgbId]--;
					if(m_clolr[m_selRgbId]<0)
						m_clolr[m_selRgbId]=0;
					DrawColorSet();
					ret= EKeyWasConsumed;
				}
			}
			break;
			case EKeyRightArrow:
			{
				if(m_drewType==EDrewTypePagebar)
				{
					MoveStepBar(0);
					DrawLineBar();
					ret=EKeyWasConsumed;
				}
				else if(m_drewType==EDrewTypeColorSet)
				{
					m_clolr[m_selRgbId]++;
					if(m_clolr[m_selRgbId]>31)
						m_clolr[m_selRgbId]=31;
					DrawColorSet();
					ret= EKeyWasConsumed;
				}
			}
			break;
			default:
				break;
		}
		return ret;

    }  

  void CMyleftCenterContainerRead::DrawText(TInt aCommand )
   {

	ActivateGc();
	CWindowGc& gc = SystemGc();
	// example code...
	gc.SetBrushColor( TRgbBack);
	gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
	
	if(iBackImg)
		gc.BitBlt( Rect().iTl, iBackImg );
	else
		gc.DrawRect( Rect() );


	gc.SetPenStyle( CGraphicsContext::ESolidPen );
	gc.SetPenColor(TRgbFont); 
	gc.UseFont(m_font);

	GetTextByLine(aCommand);
	
	if(iCenterCore->m_readStatus)
	{
		for(TInt i=0;i<m_lineCount;i++)
		{	
			SLineBody *tmpbody=m_lineBodylist->At(i);
			TBuf<100> lineBuf;
			ConventToUnicodeL(m_codetype,lineBuf,tmpbody->m_Buf);
			gc.DrawText(lineBuf,TPoint(0,m_fontH+m_fontH*i));
		}
	}
	gc.DiscardFont();
	DeactivateGc();
}

  void CMyleftCenterContainerRead::DrawLineBar()
   {

	ActivateGc();
	CWindowGc& gc = SystemGc();
	// example code...
	gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
	gc.SetPenStyle( CGraphicsContext::ESolidPen );
	
	TRect  linBackBar(TPoint(0,m_drugBar.iSY-m_fontH), TSize(Rect().Width(),m_fontH*3));
	gc.SetBrushColor( TRgbBack );
	gc.SetPenColor(TRgbFont);
	gc.DrawRect( linBackBar);

	gc.SetBrushColor( TRgb(160,160,160));
	gc.SetPenColor(TRgb(160,160,160)); 
	
	TRect  linBar(TPoint(m_drugBar.iSX,m_drugBar.iSY), TSize(m_drugBar.iWidth,m_fontH));
	gc.DrawRect( linBar);

	TInt linepos=m_drugBar.iSX+m_drugBar.iDStep*m_drugBar.iNstep;
	if(m_drugBar.iNstep==m_drugBar.iClasses)
		linepos=m_drugBar.iSX+m_drugBar.iWidth-4;
	TRect  Bar(TPoint(linepos,m_drugBar.iSY), TSize(3,m_fontH));
	
	gc.SetBrushColor( TRgb(255,128,0));
	gc.SetPenColor(TRgb(255,128,0)); 
	gc.DrawRect( Bar);

	gc.UseFont(FunUiLib::NormalFont());
	TBuf<10>  iposText;
	iposText.AppendNum(TInt64(m_drugBar.iPos));
	gc.DrawText(iposText,TPoint(linepos,m_fontH*2+m_drugBar.iSY));
	
	gc.DiscardFont();
	DeactivateGc();
}

  void CMyleftCenterContainerRead::DrawColorSet()
{
	ActivateGc();
	CWindowGc& gc = SystemGc();
	gc.UseFont(FunUiLib::NormalFont());

	TInt lineW=128,step=4;
	TInt rectSpec=Rect().Height()/10;
	TInt lineX=(Rect().Width()-128)/2;

	gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
	gc.SetPenStyle( CGraphicsContext::ESolidPen );
	gc.SetBrushColor(  TRgb(1,1,1) );
	gc.SetPenColor(TRgb((254,254,254))); 
	gc.DrawRect( Rect());

	gc.DrawText(_L("FontColor"),TPoint(lineX-35,rectSpec*2+13));
	gc.DrawText(_L("R"),TPoint(lineX-13,rectSpec*3+13));
	gc.DrawText(_L("G"),TPoint(lineX-13,rectSpec*4+13));
	gc.DrawText(_L("B"),TPoint(lineX-13,rectSpec*5+13));

	gc.DrawText(_L("BackColor"),TPoint(lineX-35,rectSpec*6+13));
	gc.DrawText(_L("R"),TPoint(lineX-13,rectSpec*7+13));
	gc.DrawText(_L("G"),TPoint(lineX-13,rectSpec*8+13));
	gc.DrawText(_L("B"),TPoint(lineX-13,rectSpec*9+13));

	//-------line------------
	gc.SetBrushColor( TRgb(160,160,160));
	gc.SetPenColor(TRgb(160,160,160)); 

	gc.DrawRect( TRect(TPoint(lineX,rectSpec*3),TSize(lineW,5)));
	gc.DrawRect( TRect(TPoint(lineX,rectSpec*4),TSize(lineW,5)));
	gc.DrawRect( TRect(TPoint(lineX,rectSpec*5),TSize(lineW,5)));
	gc.DrawRect( TRect(TPoint(lineX,rectSpec*7),TSize(lineW,5)));
	gc.DrawRect( TRect(TPoint(lineX,rectSpec*8),TSize(lineW,5)));
	gc.DrawRect( TRect(TPoint(lineX,rectSpec*9),TSize(lineW,5)));

	//-------bar------------	
	gc.SetBrushColor( TRgb(255,128,0));
	gc.SetPenColor(TRgb(255,128,0)); 
	gc.DrawRect( TRect(TPoint(lineX+m_clolr[0]*step,rectSpec*3-1),TSize(4,7)));
	gc.DrawRect( TRect(TPoint(lineX+m_clolr[1]*step,rectSpec*4-1),TSize(4,7)));
	gc.DrawRect( TRect(TPoint(lineX+m_clolr[2]*step,rectSpec*5-1),TSize(4,7)));
	gc.DrawRect( TRect(TPoint(lineX+m_clolr[3]*step,rectSpec*7-1),TSize(4,7)));
	gc.DrawRect( TRect(TPoint(lineX+m_clolr[4]*step,rectSpec*8-1),TSize(4,7)));
	gc.DrawRect( TRect(TPoint(lineX+m_clolr[5]*step,rectSpec*9-1),TSize(4,7)));
	//--------- select rect----------------
	gc.SetBrushStyle( CGraphicsContext::ENullBrush );
	gc.SetPenColor(TRgb(254,254,254)); 
	if(m_selRgbId<3) // 0,1,2
	{
		gc.DrawRect( TRect(TPoint(lineX-18,rectSpec*(m_selRgbId+3)-10),TSize(lineW+36,26)));
	}
	else//3      
	{
		gc.DrawRect( TRect(TPoint(lineX-18,rectSpec*(m_selRgbId+4)-10),TSize(lineW+36,26)));
	}
	
	//-------refer------------
	TInt referX=Rect().Width()-90,referW=80;
	TInt referY=Rect().iTl.iY+10,referH=40;
	gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
	gc.SetPenColor(  TRgb(m_clolr[0]*KColorSetp+1,m_clolr[1]*KColorSetp+1,m_clolr[2]*KColorSetp+1));
	gc.SetBrushColor(TRgb(m_clolr[3]*KColorSetp+1,m_clolr[4]*KColorSetp+1,m_clolr[5]*KColorSetp+1)); 
	gc.DrawRect( TRect(TPoint(referX,referY),TSize(referW,referH)));
	gc.DrawText(iCenterCore->TextResource(R_TEXT_APP_NAME),TPoint(referX+5,referY+referH));
	
	gc.DiscardFont();
	DeactivateGc();

}

void CMyleftCenterContainerRead::DrawBackPic()
   {

	ActivateGc();
	CWindowGc& gc = SystemGc();

	gc.Clear ( Rect() );
	// example code...
	gc.SetBrushColor( TRgbBack);
	gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
	
	if(iTempBackImg)
		gc.BitBlt( Rect().iTl, iTempBackImg );

	gc.SetPenStyle( CGraphicsContext::ESolidPen );
	gc.SetPenColor(TRgbFont); 
	gc.UseFont(m_font);
	
	if(iCenterCore->m_readStatus)
	{
		for(TInt i=0;i<m_lineCount;i++)
		{	
			SLineBody *tmpbody=m_lineBodylist->At(i);
			TBuf<100> lineBuf;
			ConventToUnicodeL(m_codetype,lineBuf,tmpbody->m_Buf);
			gc.DrawText(lineBuf,TPoint(0,m_fontH+m_fontH*i));
		}
	}
	gc.DiscardFont();
	DeactivateGc();
}

void CMyleftCenterContainerRead::GetText( TInt aCommand )
   {
/*
	TInt filsize;
	m_file.Size(filsize);
	m_pBuf.FillZ();

	switch( aCommand )
	{
	case EbookReadDown:
		{	
			if((m_ePos+m_BufSize)>filsize)
			{
				m_file.Read(m_Pos,m_pBuf);
				m_isEnd=ETrue;
			}
			else
			{
				m_file.Read(m_Pos,m_pBuf,m_BufSize);
				FC_TextTrim(m_pBuf);
				m_Pos=m_Pos+m_BufSize;
				m_isEnd=EFalse;
			}
			break;
		}
	case EbookReadUp:
		{
			if(m_isEnd)   //end
			{			
				if(m_Pos==0)
				{
					m_file.Read(m_Pos,m_pBuf,m_BufSize);  //On first page
					FC_TextTrim(m_pBuf);
				}
				else
				{
					m_file.Read(m_Pos-m_BufSize,m_pBuf,m_BufSize);
					FC_TextTrim(m_pBuf);
					m_isEnd=EFalse;
				}
					
			}
			else
			{
				if(m_Pos<2*m_BufSize)  //is first page
				{
					m_file.Read(0,m_pBuf,m_BufSize);
					FC_TextTrim(m_pBuf);
					m_Pos=m_BufSize;
				}
				else
				{
					m_file.Read(m_Pos-2*m_BufSize,m_pBuf,m_BufSize);
					FC_TextTrim(m_pBuf);
					m_Pos=m_Pos-m_BufSize;		
				
				}
			
			}
			break;
		}
	case EbookReadRoad:
		{
			if(m_Pos<m_BufSize)
			{
				m_file.Read(m_Pos,m_pBuf,m_BufSize);
				FC_TextTrim(m_pBuf);
				if(m_Pos+m_BufSize<filsize)
					m_Pos=m_Pos+m_BufSize;
				else
					m_isEnd=ETrue;
			}
			else
			{
				if(m_Pos+m_BufSize>filsize)
				{
					m_file.Read(m_Pos,m_pBuf,m_BufSize);
					FC_TextTrim(m_pBuf);
					m_isEnd=ETrue;
				}
				else
				{
					m_file.Read(m_Pos-m_BufSize,m_pBuf,m_BufSize);
					FC_TextTrim(m_pBuf);
				}

			}
				
			//m_read->iPos=m_read->iPos+30;
			break;
		}
	default:
		break;
   }
   */
}



void CMyleftCenterContainerRead::SetUp()
{
	if(m_lineBodylist)
	{
		for(int i=0;i<m_lineBodylist->Count();i++)
		{
			SLineBody* body=m_lineBodylist->At(i);
			delete body;
		}
		m_lineBodylist->Reset();
	}
		
	m_font = NULL;
	TFontSpec fontSpec=FunUiLib::NormalFont()->FontSpecInTwips();
	fontSpec.iHeight =iCenterCore->m_readerSetting.textSize;
	CEikonEnv::Static()->ScreenDevice()->GetNearestFontInTwips(m_font,fontSpec);
	
	m_fontH=m_font->HeightInPixels()+3;
	m_fontW=m_font->TextWidthInPixels(iCenterCore->TextResource(R_TEXT_POPMENU_PARAME_SETTING))/4;
	m_lineBuf=Rect().Width()/m_fontW;
	m_lineCount=Rect().Height()/m_fontH;
	m_BufSize=m_lineBuf*2;
	
	for(int i=0;i<m_lineCount;i++)
	{
		SLineBody *tmpbody=new SLineBody();
		tmpbody->m_Buf.SetLength(0);
		m_lineBodylist->AppendL(tmpbody);
	}
}

void CMyleftCenterContainerRead::GetTextByLine( TInt aCommand )
   {
	TInt foundAt;
	TInt filsize;
	m_file.Size(filsize);

	
	switch( aCommand )
	{
	case EbookReadDown:
	{	
		if(m_ePos<filsize)
		{
			for(TInt i=0;i<m_lineCount;i++)
			{
				SLineBody *tmpbody=m_lineBodylist->At(i);
				tmpbody->m_Buf.SetLength(0);
			}	

			m_bPos=m_ePos;
			GetTextByLineBodyDown(m_codetype);
		}
	}
	break;
	case EbookReadUp:
		{
			if(m_bPos>m_offset)
			{
				for(TInt i=0;i<m_lineCount;i++)
				{
					SLineBody *tmpbody=m_lineBodylist->At(i);
					tmpbody->m_Buf.SetLength(0);
				}	
				GetTextByLineBodyUp(m_codetype);		
			}
		}
		break;
	case EbookReadRoad:
	{
		for(TInt i=0;i<m_lineCount;i++)
		{
			SLineBody *tmpbody=m_lineBodylist->At(i);
			tmpbody->m_Buf.SetLength(0);
		}	

		if(m_bPos==0)
		{
			m_bPos+=m_offset;
		}
		
		GetTextByLineBodyDown(m_codetype);
		if(m_ePos>=filsize)
			m_isEnd=ETrue;
			
	}
	break;
	default:
		break;
   }
}

void CMyleftCenterContainerRead::GetTextByLineBodyDown(TInt _codetype)
{
	int itempos=m_bPos;
	for(TInt j=0;j<m_lineCount;j++)
	{
		SLineBody *tmpbody=m_lineBodylist->At(j);
		for(TInt i=0;i<m_BufSize;i++)
		{
			TBuf8<8> bitbuf;
			m_file.Read(itempos,bitbuf,1);
			char *p=(char *)bitbuf.Ptr();
			int a=(int)*p;
			a=a&0x000000FF;

			if(_codetype==ETextCodeUTF8)
			{
				if(a<=(int)0x7F)
				{
					tmpbody->m_Buf.Append(bitbuf);
					itempos++;
					if((tmpbody->m_Buf.Find(_L8("\r\n")))!= KErrNotFound)
					{
						tmpbody->m_Buf.SetLength(tmpbody->m_Buf.Length()-2);
						if(i==1)
							j--;
						break;
					}

					if(i==m_BufSize)
					{
						m_file.Read(itempos,bitbuf,1);
						if(((tmpbody->m_Buf.Find(_L8("\r")))!= KErrNotFound)&&((bitbuf.Find(_L8("\n")))!= KErrNotFound))
						{
							itempos++;
							tmpbody->m_Buf.SetLength(tmpbody->m_Buf.Length()-1);
						}
						
					}

				}
				else if((a<=(int)0xDF)&& (a>=(int)0xC0) )
				{
					m_file.Read(itempos,bitbuf,2);
					tmpbody->m_Buf.Append(bitbuf);
					itempos+=2;
					i++;
				}
				else if((a<=(int)0xEF)&& (a>=(int)0xE0) )
				{
					m_file.Read(itempos,bitbuf,3);
					tmpbody->m_Buf.Append(bitbuf);
					itempos+=3;
					i++;
				}
				else if((a<=(int)0xF7)&& (a>=(int)0xF0) )
				{
					m_file.Read(itempos,bitbuf,4);
					tmpbody->m_Buf.Append(bitbuf);
					itempos+=4;
					i++;
				}
				else if((a<=(int)0xFB)&& (a>=(int)0xF8) )
				{
					m_file.Read(itempos,bitbuf,5);
					tmpbody->m_Buf.Append(bitbuf);
					itempos+=5;
					i++;
				}
				else if((a==(int)0xFD)|| (a==(int)0xFC) )
				{
					m_file.Read(itempos,bitbuf,6);
					tmpbody->m_Buf.Append(bitbuf);
					itempos+=6;
					i++;
				}
				else 
				{
					m_file.Read(itempos,bitbuf,2);
					tmpbody->m_Buf.Append(bitbuf);
					itempos+=2;
					i++;
				}
/*
				if(i==m_BufSize)
				{
					m_file.Read(itempos,bitbuf,2);
					if(((bitbuf.Find(_L8("\r")))!= KErrNotFound)||((bitbuf.Find(_L8("\n")))!= KErrNotFound))
					{
						itempos+=2;
					}
					
				}*/
			}
			else if(_codetype==ETextCodeUTF16B)
			{
					m_file.Read(itempos,bitbuf,2);
					{
						char *p=(char *)bitbuf.Ptr();
						int intbuf2=(int)*p++;
						int intbuf1=(int)*p++;
						intbuf1=intbuf1&0x000000FF;
						intbuf2=intbuf2&0x000000FF;
						intbuf1=intbuf1<<8;
						intbuf2=intbuf2|intbuf1;
						if((intbuf2>=(int)0xE000)&& (intbuf2<=(int)0xF8FF))
						{
							itempos+=2;
							continue;
						}
						if(intbuf2<=(int)0x0080)
						{
							i--;
						}
					}
					tmpbody->m_Buf.Append(bitbuf);
					TBuf8<4> aa;
					aa.Copy(_L8("\r"));
					aa.Append(TChar(0x00));

					if((tmpbody->m_Buf.Find(aa))!= KErrNotFound)
					{
						tmpbody->m_Buf.SetLength(tmpbody->m_Buf.Length()-2);
						itempos+=4;
						if(i<=1)
							j--;
						break;
					}

					itempos+=2;
					i++;
			}
			else if(_codetype==ETextCodeANSI)
			{
				if(a<=(int)0x7F)
				{
					tmpbody->m_Buf.Append(bitbuf);
					itempos+=1;
					
					if((tmpbody->m_Buf.Find(_L8("\r\n")))!= KErrNotFound)
					{
						tmpbody->m_Buf.SetLength(tmpbody->m_Buf.Length()-2);
						if(i==1)
							j--;
						break;
					}

					if(i==m_BufSize)
					{
						m_file.Read(itempos,bitbuf,1);
						if(((tmpbody->m_Buf.Find(_L8("\r")))!= KErrNotFound)||((bitbuf.Find(_L8("\n")))!= KErrNotFound))
						{
							itempos++;
							tmpbody->m_Buf.SetLength(tmpbody->m_Buf.Length()-1);
						}
						
					}

				}
				else if((a<=(int)0xFE)&& (a>=(int)0x81) )
				{
					m_file.Read(itempos,bitbuf,2);
					tmpbody->m_Buf.Append(bitbuf);
					itempos+=2;
					i++;
				}
			}

		}
		
	}

	m_ePos=itempos;
}

void CMyleftCenterContainerRead::GetTextByLineBodyUp(TInt _codetype)
{
	int itempos=m_bPos;
	m_ePos=m_bPos;
	if(_codetype==ETextCodeUTF16B)
		itempos-=2;
	else
		itempos--;
	
	for(TInt j=m_lineCount-1;j>=0;j--)
	{
		SLineBody *tmpbody=m_lineBodylist->At(j);
		for(TInt i=0;(i<m_BufSize)&&(itempos>=m_offset);i++)
		{
			TBuf8<8> bitbuf;
			m_file.Read(itempos,bitbuf,1);
			char *p=(char *)bitbuf.Ptr();
			int a=(int)*p;
			a=a&0x000000FF;

			if(_codetype==ETextCodeUTF8)
			{
				if(a<=(int)0x7F)
				{
					tmpbody->m_Buf.Insert(0,bitbuf);
					itempos-=1;
					if((tmpbody->m_Buf.Find(_L8("\r\n")))!= KErrNotFound)
					{
						tmpbody->m_Buf.Delete(0,2);
						if(i==1)
							j++;
						break;
					}
				}
				else  // (a<=(int)0xBF)&& (a>=(int)0x80)
				{
					int leng=1;
					int b;
					do
					{
						itempos--;
						m_file.Read(itempos,bitbuf,1);
						leng++;

						char *q=(char *)bitbuf.Ptr();
						b=(int)*q;
						b=b&0x000000FF;
					}while((b<=(int)0xBF)&& (b>=(int)0x80));
					
					m_file.Read(itempos,bitbuf,leng);
					tmpbody->m_Buf.Insert(0,bitbuf);
					itempos--;
					i++;
				}
			}
			else if(_codetype==ETextCodeUTF16B)
			{
					m_file.Read(itempos,bitbuf,2);
					{
						char *p=(char *)bitbuf.Ptr();
						int intbuf2=(int)*p++;
						int intbuf1=(int)*p++;
						intbuf1=intbuf1&0x000000FF;
						intbuf2=intbuf2&0x000000FF;
						intbuf1=intbuf1<<8;
						intbuf2=intbuf2|intbuf1;
						if((intbuf2>=(int)0xE000)&& (intbuf2<=(int)0xF8FF))
						{
							itempos-=2;
							continue;
						}
						if(intbuf2==(int)0x000a)
						{
							itempos-=4;
							if(i<=2)
								j++;
							break;
						}
						if(intbuf2<=(int)0x0080)
						{
							i--;
						}
					}
					/*
					TBuf8<4> aa;
					aa.Copy(_L8("\n"));
					aa.Append(TChar(0x00));

					if((tmpbody->m_Buf.Find(aa))!= KErrNotFound)
					{
						tmpbody->m_Buf.Delete(0,2);
						itempos-=4;
						if(i<=2)
							j--;
						break;
					}
*/
					tmpbody->m_Buf.Insert(0,bitbuf);
					itempos-=2;
					i++;
			}
			else if(_codetype==ETextCodeANSI)
			{
				if(a<=(int)0x7F)
				{
					{
						int bpos=itempos;
						bpos--;
						TBuf8<2> bbuf;
						m_file.Read(bpos,bbuf,2);
						char *q=(char *)bbuf.Ptr();
						int b=(int)*q;
						b=b&0x000000FF;

						if((b<=(int)0xFE)&& (b>=(int)0x81) )  // is 2 bit char
						{
							i++;
							itempos--;
							tmpbody->m_Buf.Insert(0,bitbuf);
							continue;
						}
					}
					tmpbody->m_Buf.Insert(0,bitbuf);
					itempos--;
					if((tmpbody->m_Buf.Find(_L8("\n")))!= KErrNotFound)
					{
						itempos--;
						tmpbody->m_Buf.Delete(0,1);
						if(i<=1)
							j++;
						break;
					}
				}
				else if((a<=(int)0xFE)&& (a>=(int)0x81) )
				{
					itempos--;
					m_file.Read(itempos,bitbuf,2);
					tmpbody->m_Buf.Insert(0,bitbuf);
					itempos--;
					i++;
				}
			}
		}
		
	}

	if(_codetype==ETextCodeUTF16B)
		itempos+=2;
	else
		itempos++;
	m_bPos=itempos;
}

TInt CMyleftCenterContainerRead::ConventToUnicodeL(TInt _codetype,TDes& _desBuf,const TDesC8& _szBuf)
{
	if(_codetype==ETextCodeANSI)
	{
		if(!m_converter)
		{
			m_converter=CCnvCharacterSetConverter::NewL ();
			m_converter->PrepareToConvertToOrFromL (KCharacterSetIdentifierGbk, CEikonEnv::Static()->FsSession());
		}
		TInt _state = CCnvCharacterSetConverter::KStateDefault;
		 m_converter->ConvertToUnicode (_desBuf, _szBuf, _state);

		// FC_GbkToUnicodeL(_desBuf,_szBuf);
		 return _desBuf.Length();
	}
	else if(_codetype==ETextCodeUTF8)
	{
		return CnvUtfConverter::ConvertToUnicodeFromUtf8(_desBuf,_szBuf);
	//	FC_Utf8ToUnicodeL(_desBuf,_szBuf);
	//	return _desBuf.Length();
	}
	else if(_codetype==ETextCodeUTF16B)
	{
		char* pBuf8=(char* )_szBuf.Ptr();
		TPtr pBuf16(NULL,0,0);
		pBuf16.Set((TUint16 *)pBuf8,_szBuf.Length()/2,100);
		_desBuf.Copy(pBuf16);
		return _desBuf.Length();
	}		
}

void CMyleftCenterContainerRead::CalculateStepClass(TInt _pos,TInt  _all)
{
	if(_all<=1024)  //  1 kb
		m_drugBar.iClasses=10;
	else if((1024<_all) &&( _all<=1024*1024/2))  //  1kb~ 0.5mb
		m_drugBar.iClasses=25;
	else if((1024*1024/2<_all) &&( _all<=1024*1024)) // 0.5mb~ 1mb
		m_drugBar.iClasses=60;
	else  // >1mb
		m_drugBar.iClasses=100;

	m_drugBar.iWidth=Rect().Width()*4/5;
	m_drugBar.iDStep=m_drugBar.iWidth/m_drugBar.iClasses;
	m_drugBar.iWidth=m_drugBar.iDStep*m_drugBar.iClasses;
		
	m_drugBar.iSX=(Rect().Width()-m_drugBar.iWidth)/2;
	m_drugBar.iSY=m_fontH*m_lineCount*2/3;
	m_drugBar.iFileSize=_all;
	m_drugBar.iStep=m_drugBar.iFileSize/m_drugBar.iClasses;

	for(TInt i=0;i<m_drugBar.iClasses+1;i++)
	{
		if((_pos>=i*m_drugBar.iStep)&&(_pos<(i+1)*m_drugBar.iStep))
		{
			m_drugBar.iNstep=i;
			m_drugBar.iPos=_pos;
			break;
		}
	}
	
}

void CMyleftCenterContainerRead::GetStepPos(TInt _pos)
{

	for(TInt i=0;i<m_drugBar.iClasses+1;i++)
	{
		if((_pos>=i*m_drugBar.iStep)&&(_pos<(i+1)*m_drugBar.iStep))
		{
			m_drugBar.iNstep=i;
			m_drugBar.iPos=_pos;
			break;
		}
	}
}

TInt CMyleftCenterContainerRead::MoveStepBar(TInt _backcommand)
{
	if(_backcommand)  //back .
	{
		m_drugBar.iNstep--;
		if(m_drugBar.iNstep<0)
			m_drugBar.iNstep=0;
		m_drugBar.iPos=m_drugBar.iNstep*m_drugBar.iStep;
		
	}
	else  //go ahead 
	{
		m_drugBar.iNstep++;
		if(m_drugBar.iNstep>=m_drugBar.iClasses)
		{
			m_drugBar.iNstep=m_drugBar.iClasses;
			m_drugBar.iPos=m_drugBar.iFileSize;
		}
		else
		{
			m_drugBar.iPos=m_drugBar.iNstep*m_drugBar.iStep;
		}
	}
	return m_drugBar.iPos;

}
