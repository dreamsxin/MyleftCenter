/*
============================================================================
Name        : CFunLyricLabel from FunLyricLabel.h
Author      : Wo Junjun
Version     : 2008-01-21
Copyright   : Funinhand Technology
Description : Declares container control for application.
============================================================================
*/

#include "start.h"
#include "SubTitle.h"
#include "FunLyricLabel.h"

void CFunLyricLabel::ConstructL(MFunLyricLabelObserver* aObserver, const TRect& aRect, CFont *aFont, 
								TRgb aTextColor, TRgb aHighTextColor, TRgb aBackColor)
{
	//iFondoGlobal = CFondoGlobal::Attach();
	iFont = aFont;
	iObserver = aObserver;

	iTextColor = aTextColor;
	iBackColor = aBackColor;
	iHighTextColor = aHighTextColor;

	iTextLines = new (ELeave) CArrayFixFlat<HBufC*> (4);

	iLineNumber = aRect.Height() / KLineHeight;
	iBeforeHighlightNumber = (iLineNumber-1) / 2;
	iAfterHighlightNumber = iLineNumber - iBeforeHighlightNumber - 1;

	iHasLyric = EFalse;

	AllocTextLines( iLineNumber );

	SetRect( aRect );

	ActivateL();
}

/**
* Destructor.
*/
CFunLyricLabel::~CFunLyricLabel()
{
	ClearTextLines();
	delete iTextLines;
}



// ---------------------------------------------------------
// CFunLyricLabel::SizeChanged()
// Called by framework when the view size is changed
// ---------------------------------------------------------
//
void CFunLyricLabel::SizeChanged()
{
}


// ---------------------------------------------------------
// CFunLyricLabel::Draw(const TRect& aRect) const
// ---------------------------------------------------------
//
void CFunLyricLabel::Draw(const TRect& aRect) const
{
	if( !IsVisible() )
		return;

	CWindowGc& gc = SystemGc();
	TRect rc = Rect();

	TInt textHeight = iFont->HeightInPixels();

	// Draw background
	gc.SetPenColor( iBackColor );
	gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
	gc.SetBrushColor( iBackColor );
	gc.DrawRect( rc );

	if( iHasLyric )
	{
		if( iHighlightIndex>=0 )
		{
			// Draw Text
			gc.UseFont( iFont );
			gc.SetPenColor( iTextColor );

			TInt i=0, lineNumber; 
			TRect frameRect( rc.iTl, TSize(rc.Width(), KLineHeight) );
			frameRect.Move( 0, KTopGapHeight );
			lineNumber = iLineNumber;
			if( lineNumber>iTextLines->Count() )
				lineNumber = iTextLines->Count();
			for( i=0; i<lineNumber; i++ )
			{
				if( i==iHighlightIndex )
					gc.SetPenColor( iHighTextColor );
				else
					gc.SetPenColor( iTextColor );
				gc.DrawText( iTextLines->At(i)->Des(), frameRect, textHeight+(frameRect.Height()-textHeight)/2, CGraphicsContext::ECenter );
				frameRect.Move( 0, KLineHeight );
			}

			gc.DiscardFont();
		}
	}
	else
	{
		// Draw Text
		gc.UseFont( iFont );
		gc.SetPenColor( iTextColor );
		
		gc.DrawText( iNoLyricText, rc, textHeight+(rc.Height()-textHeight)/2, CGraphicsContext::ECenter );

		gc.DiscardFont();
	}
}


TKeyResponse CFunLyricLabel::OfferKeyEventL( const TKeyEvent& aKeyEvent,  TEventCode aType ) 
{
	return EKeyWasNotConsumed;
}

void CFunLyricLabel::UpdateLyricDisplay(TInt aPreious, TBool aDrawNow)
{
	SetTextLinesEmpty();
	if( !iObserver || !iObserver->MflloGetSubtitle() )
	{
		iHasLyric = EFalse;
		iCurrentFrame = 0;
		return;
	}

	CSubtitle *subtitle = iObserver->MflloGetSubtitle();

	TInt startFrame, endFrame;
	TInt curFrame = subtitle->GetCurrentFrameIndex()-1;
	if( aPreious )
		curFrame = subtitle->GetCurrentFrameIndex()-2;
	if( curFrame<0 )
		curFrame = 0;
	if( curFrame<iCurrentFrame )
		curFrame = iCurrentFrame;
	
	startFrame = curFrame - iBeforeHighlightNumber;
	endFrame = curFrame + iAfterHighlightNumber;

	
	iHighlightIndex = iBeforeHighlightNumber;
	if( startFrame<0 )
	{
		iHighlightIndex += startFrame;
		startFrame = 0;
		endFrame = iLineNumber-1;
	}

	if( endFrame>=subtitle->GetFrameCount() )
		endFrame = subtitle->GetFrameCount()-1;
	if( endFrame<0 )
		endFrame = 0;

	iCurrentFrame = curFrame;
	iStartFrame = startFrame;
	iEndFrame = endFrame;
	iHasLyric = ETrue;

	//
	TUint i, k=0;
	//TUint8 frameText8[KMaxLineCharNumber];
	TUint frameLen;
	TBuf<KMaxLineCharNumber> frameBuf;
	for( i=startFrame; i<=endFrame; i++ )
	{
		frameBuf.SetLength(0);
		frameLen = 0;
	//	frameText8[0] = '\0';
	//	TPtrC16 	pUnicode(frameBuf);
		 subtitle->GetTextBufByIndex( i, (TUint16*)frameBuf.Ptr(), frameLen, KMaxLineCharNumber );
		 frameBuf.SetLength(frameLen);
		 iTextLines->At(k++)->Des().Copy( frameBuf );
		/*
		if( subtitle->GetTextBufByIndex( i, frameText8, frameLen, KMaxLineCharNumber ) )
		{
			TPtrC8 _text_ptr8 ((const unsigned char*)frameText8, frameLen);
			if( subtitle->GetEncode() == CSubtitle::EEncode_Utf8 )
				FC_Utf8ToUnicodeL( frameBuf, _text_ptr8 );
			else
				FC_GbkToUnicodeL( frameBuf, _text_ptr8 );
			iTextLines->At(k++)->Des().Copy( frameBuf );
		}*/
	}

	//
	if( aDrawNow )
		DrawNow();
}

void CFunLyricLabel::AllocTextLines(TInt aLines)
{
	ClearTextLines();

	TInt i;
	HBufC* hbuf;
	for( i=0; i<aLines; i++ )
	{
		hbuf = HBufC::NewL( KMaxLineCharNumber );
		iTextLines->AppendL( hbuf );
	}
}
void CFunLyricLabel::ClearTextLines()
{
	TInt i, count = iTextLines->Count();
	for( i=0; i<count; i++ )
	{
		delete iTextLines->At(i);
	}
	iTextLines->Delete( 0, count );

	iStartFrame = -1;
	iEndFrame = -1;
	iHighlightIndex = -1;
	iCurrentFrame = 0;
}

void CFunLyricLabel::SetTextLinesEmpty()
{
	TInt i, count = iTextLines->Count();
	for( i=0; i<count; i++ )
	{
		iTextLines->At(i)->Des().SetLength(0);
	}
}

void CFunLyricLabel::ResetLyricFrame()
{
	iCurrentFrame = 0;
}

void CFunLyricLabel::SetObserver( MFunLyricLabelObserver* aObserver )
{
	iObserver = aObserver;
}

void CFunLyricLabel::SetNoLyricText(const TDesC& aText)
{
	if( aText.Length()>128 )
		iNoLyricText.Copy( aText.Left(128) );
	else
		iNoLyricText.Copy( aText );
}

