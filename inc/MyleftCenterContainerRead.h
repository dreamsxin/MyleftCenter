/*
============================================================================
 Name        : CMyleftCenterContainer2 from MyleftCenterContainer2.h
 Author      : yilizheng
 Version     :
 Copyright   : MyleftCenter
 Description : Declares container control for application.
============================================================================
*/

#ifndef MYLEFTCENTER_CONTAINER_READ_H
#define MYLEFTCENTER_CONTAINER_READ_H

// INCLUDES
#include <coecntrl.h>
#include "symbian_def.h"
#ifdef _S60_3RD_
#include <hwrmlight.h>
#endif
#include "FunUIStart.h"
// FORWARD DECLARATIONS
class CEikLabel;        // for example labels
class CenterCore;
// CLASS DECLARATION

#define KColorSetp	8

typedef enum TbookReadCmdId
{
	EbookReadUp= 1,
	EbookReadDown,
	EbookReadRoad
};

typedef	enum 
{
	EDrewTypeText,
	EDrewTypePagebar,
	EDrewTypeColorSet,
	EDrewTypePicSel
}TWapFeeStepType;

typedef struct 
{
	TBuf8<256>	m_Buf;
}SLineBody;


typedef struct 
{

	TInt iWidth;
	TInt iSX;
	TInt iSY;
	TInt iDStep;  //drew Step
	TInt iFileSize;
	TInt iClasses;
	TInt iStep;
	TInt	iNstep;
	TInt iPos;
}SDrugBar;
/**
*  CMyleftCenterContainer2  container control class.
*  
*/
class CCnvCharacterSetConverter;
class CMyleftCenterContainerRead : public CCoeControl, MCoeControlObserver
										,public MFunTimeOutNotify
    {
    public: // Constructors and destructor

	CMyleftCenterContainerRead();
        /**
        * EPOC default constructor.
        * @param aRect Frame rectangle for container.
        */
        void ConstructL(const TRect& aRect);

        /**
        * Destructor.
        */
        ~CMyleftCenterContainerRead();

    public: // New functions
    		void TimerStop();
		void TimerStart();
		void	LightOn();
		 void DrawText(TInt aCommand );
		 void DrawLineBar();
		 void DrawColorSet();
		 void DrawBackPic();
		//   void DrawNow();
		 void GetText(TInt aCommand );
		void GetTextByLine( TInt aCommand );
		void GetTextByLineBodyDown(TInt _codetype);
		void GetTextByLineBodyUp(TInt _codetype);
		
		TInt ConventToUnicodeL(TInt _codetype,TDes& _desBuf,const TDesC8& _szBuf);
		void GetStepPos(TInt _pos);
		void CalculateStepClass(TInt _pos,TInt  _all);
		TInt MoveStepBar(TInt _backcommand);

    public: // Functions from base classes
		void MftonTimerExpired() ;		//MFunTimeOutNotify
    private: // Functions from base classes

       /**
        * From CoeControl,SizeChanged.
        */
        void SizeChanged();

       /**
        * From CoeControl,CountComponentControls.
        */
        TInt CountComponentControls() const;

       /**
        * From CCoeControl,ComponentControl.
        */
        CCoeControl* ComponentControl(TInt aIndex) const;

       /**
        * From CCoeControl,Draw.
        */
        void Draw(const TRect& aRect) const;

		/**
		* From MCoeControlObserver
		* Acts upon changes in the hosted control's state. 
		*
		* @param	aControl	The control changing its state
		* @param	aEventType	The type of control event 
		*/
        void HandleControlEventL(CCoeControl* aControl,TCoeEvent aEventType);
        TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
	void SetUp();
    private: //data
    		CFbsBitmap*  	iBackImg;  
       	 TRgb 		TRgbBack;
		TRgb 		TRgbFont;
		TInt			m_selRgbId;//0-5
		
		CFont *		m_font;
		TInt			m_fontH;
		TInt			m_fontW;
		TInt			m_lineCount;
		TInt			m_lineBuf;
		TInt			m_BufSize;
		
		CenterCore	*iCenterCore;
		CFunTimeOutTimer* iTimer;
		TInt			m_outtime;  //s
		RFs			m_fs;
		RFile			m_file;
		TPtr8		m_pBuf;
		HBufC8		*m_Buf;
		CArrayFixFlat<SLineBody*>	*m_lineBodylist;
		TBool		m_isEnd;
		CCnvCharacterSetConverter *m_converter;
#ifdef _S60_3RD_
		CHWRMLight *iLight;
#endif
	public:
		CFbsBitmap*  	iTempBackImg;  
		TInt			m_iPicId;
		TWapFeeStepType		m_drewType; //   0 
		TInt			m_clolr[6];//font R,G,B ,back R, G, B
		TInt			m_offset;
		TInt 			m_bPos;
		TInt 			m_ePos;
		TInt			m_codetype;
		SDrugBar	m_drugBar;

   };

#endif


