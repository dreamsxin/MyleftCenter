/*
============================================================================
Name        : CFunLyricLabel from FunLyricLabel.h
Author      : Wo Junjun
Version     : 2008-01-21
Copyright   : Funinhand Technology
Description : Declares container control for application.
============================================================================
*/

#ifndef _FUN_LYRIC_LABEL_H_
#define _FUN_LYRIC_LABEL_H_

// INCLUDES
#include <coecntrl.h>

// FORWARD DECLARATIONS
class FPlayerCore;
class CSubtitle;
class MFunLyricLabelObserver;

#define KMaxLineCharNumber	64
#define KLineHeight			20
#define KTopGapHeight		10

class CFunLyricLabel : public CCoeControl
{
public: // Constructors and destructor

	/**
	* EPOC default constructor.
	* @param aRect Frame rectangle for container.
	*/
	void ConstructL(MFunLyricLabelObserver* aObserver, const TRect& aRect, CFont *aFont, 
					TRgb aTextColor, TRgb aHighTextColor, TRgb aBackColor);

	/**
	* Destructor.
	*/
	~CFunLyricLabel();
	
protected: // Functions from base classes

	/**
	* From CoeControl,SizeChanged.
	*/
	void SizeChanged();

	/**
	* From CCoeControl,Draw.
	*/
	void Draw(const TRect& aRect) const;

	/**
	* From CoeControl,OfferKeyEventL.
	* Handles the key-events.
	* @return if key-event is consumed, EKeyWasConsumed. Else EKeyWasNotConsumed.
	* @param aKeyEvent Key event
	* @param aType Type of key event( EEventKey, EEventKeyUp or EEventKeyDown )
	*/
	TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

protected:	// Data
	FPlayerCore* iFondoGlobal;
	MFunLyricLabelObserver* iObserver;

	CFont* iFont;
	TRgb iBackColor, iTextColor, iHighTextColor;

	TInt iLineNumber;
	TInt iStartFrame, iEndFrame;
	TInt iCurrentFrame;

	TInt iHighlightIndex;
	TInt iBeforeHighlightNumber, iAfterHighlightNumber;

	CArrayFixFlat<HBufC*> *iTextLines;
	void AllocTextLines(TInt aLines);
	void ClearTextLines();
	void SetTextLinesEmpty();
	void Utf8ToUnicodeL (TDes16& _unicodeBuffer,const TDesC8& _utf8Buffer) ;
	void GbkToUnicodeL (TDes16& _unicodeBuffer,const TDesC8& _asciiBuffer);
	
	TBool iHasLyric;
	TBuf<128> iNoLyricText;
public:
	void UpdateLyricDisplay(TInt aPreious=EFalse, TBool aDrawNow=ETrue);
	void ResetLyricFrame();
	void SetNoLyricText(const TDesC& aText);

	void SetObserver( MFunLyricLabelObserver* aObserver );
};

class MFunLyricLabelObserver
{
public:
	virtual CSubtitle* MflloGetSubtitle() = 0;
};


#endif	// _FUN_LYRIC_LABEL_H_