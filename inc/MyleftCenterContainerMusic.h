/*
============================================================================
 Name        : CFunSMusicContainer from FunSMusicContainer.h
 Author      : funinhand
 Version     :
 Copyright   : Your copyright notice
 Description : Declares container control for application.
============================================================================
*/

#ifndef MYLEFT_CONTAINER_MUSIC_H
#define MYLEFT_CONTAINER_MUSIC_H

// INCLUDES
#include <coecntrl.h>
#include <eiklbo.h>  
#include "FunUistart.h"
#include "FunLyricLabel.h"

/*
#include "FunButton.h"
#include "FunLabel.h"
#include "FunImage.h"
*/
// FORWARD DECLARATIONS



// CLASS DECLARATION
class CenterCore;
class CFbsBitmap;
class CLrcSubtitle;
/**
*  CFunSMusicContainer  container control class.
*  
*/

typedef enum _TLrcLibShowId
{
	ELrcLibShow_NULL=0,
	ELrcLibShow_NoAutoLRC,// 1
	ELrcLibShow_NoManuLRC, // 2
	ELrcLibShow_HaveLRC,// 3
	ELrcLibShow_HaveLocaLRC,// 4
	ELrcLibShow_PayNotify// 5
}TLrcLibShowId;


class CMyleftCenterContainerMusic : public CCoeControl, public MFunListDrawer
								,public MFunTimeOutNotify
								,public MFunLyricLabelObserver
								,public MFunListObserver
    {

public:
enum _TButtonID
{
	EButton_Prev=1,
	EButton_Play,
	EButton_Stop,
	EButton_Next
};


    public: // Constructors and destructor

        static CMyleftCenterContainerMusic* NewL( const TRect& aRect );
        static CMyleftCenterContainerMusic* NewLC( const TRect& aRect );

        /**
        * EPOC default constructor.
        * @param aRect Frame rectangle for container.
        */
        void ConstructL(const TRect& aRect);

        /**
        * Destructor.
        */
        ~CMyleftCenterContainerMusic();
	void ClearTextLines();
    public: // New functions
   	int GetCurListIndex()
		{
		if(iList)
			return iList->GetCurrentSelectIndex();
		return 0;
		}
   	int GetCurListCount()
		{
		if(iList)
			return iList->GetListItemCount();
		return 0;
		}

		void TimerStop();
		void ResetPlayIndexAndSubtitle(int _curIndex);
		void UpdataAll();
		void SetLrcLibleShowId(TInt _lrcLibShowId){iLrcShowId=_lrcLibShowId;}
    public: // Functions from base classes
	void MldMeasureItem(CFunList* aControl, TInt aItemIndex, TInt& aHeight){}
	void MldDrawBackground(CFunList* aControl, const TRect& aRect, TBool &aDefaultDraw){}
	void MldDrawItem(CFunList* aControl, TInt aItemIndex, const TRect& aRect, TBool &aDefaultDraw){}

	
	void MfloListItemClicked(CFunList* aButton, TInt aIndex);
	void MftonTimerExpired();
	CSubtitle* MflloGetSubtitle();
	void GetSingerIcon();
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

	 void ShowButton(int ButtonID);      	
	//void MfboButtonPressed(CFunButton* aButton);
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
	void Destroy();

/*
      void CreateButtonL(CFunButton* &aButton, TRect aRect,
									TInt aNormalImageId, TInt aClickedImageId, 
									TInt aDisabledImageId, TInt aMaskImageId, 
									const TDesC& aCaption,
									TInt aKeyCode, TBool aVisible); 
									*/
	void SetupListItemsL();
    private: //data

    	CenterCore*iplaycore;
	CFunTimeOutTimer* iTimer;
	
  	TRect contrlRect;  
	
	
	CLrcSubtitle * iSubtitle;
	CFunLyricLabel *iLyricLabel;
  	CFbsBitmap*  	iMainBackImg;  
	CFbsBitmap*  	iSingerImg;
	CFbsBitmap*	iSingerIcon;
	CFbsBitmap*  	iVolumeImg;  
	
	int 			iAlltime;
	TUint 			iCurtime;
	CFunLabel*	songLrcLib;
	CFunLabel*	songNameLib;
	CFunLabel*	songTimeLib;
	CFunLabel*	streamPercentLib;

  	CFbsBitmap*  	iPrevFImg;  
 	CFbsBitmap*  	iPlayFImg;
	CFbsBitmap*  	iStopFImg;
	CFbsBitmap*  	iNextFImg;
	
  	CFbsBitmap*  	iPrevImg;  
 	CFbsBitmap*  	iPlayImg;
	CFbsBitmap*  	iStopImg;
	CFbsBitmap*  	iNextImg;
	
  	
	
public:
	CFunList* iList;
	int showActiveID;	
	int   iCurretPlayIndex;
	 int 	iSelIndex;
	int	iShowMode ;	//0=List;1=lrc,2= rearch result
	TInt iLrcShowId;  //_TLrcLibShowId
	TBool	iflag ;	//0=first;  
};

#endif


