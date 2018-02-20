/*
============================================================================
 Name        : CMyleftCenterContainer2 from MyleftCenterContainer2.h
 Author      : yilizheng
 Version     :
 Copyright   : MyleftCenter
 Description : Declares container control for application.
============================================================================
*/

#ifndef MYLEFTCENTERCONTAINER2_H
#define MYLEFTCENTERCONTAINER2_H

// INCLUDES
#include <coecntrl.h>
 #include "FunUistart.h"
// FORWARD DECLARATIONS
class CEikLabel;        // for example labels
class CenterCore;
// CLASS DECLARATION


#define ReaderInfo_Size	256
/**
*  CMyleftCenterContainer2  container control class.
*  
*/
class CMyleftCenterContainer2 : public CCoeControl
									,public MFunListObserver
    {
    public: // Constructors and destructor
        
        /**
        * EPOC default constructor.
        * @param aRect Frame rectangle for container.
        */
        void ConstructL(const TRect& aRect);

        /**
        * Destructor.
        */
        ~CMyleftCenterContainer2();

    public: // New functions

    public: // Functions from base classes

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
	void MfloListItemClicked(CFunList* aButton, TInt aIndex);
       /**
        * From CCoeControl,Draw.
        */
        void Draw(const TRect& aRect) const;
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
	void SetupListItemsL();
	void SetupCurReaderInfo(TInt index);
	void UpdateListItemsL();
		/**
		* From MCoeControlObserver
		* Acts upon changes in the hosted control's state. 
		*
		* @param	aControl	The control changing its state
		* @param	aEventType	The type of control event 
		*/
        
    private: //data
        CenterCore *iplaycore;
	CFbsBitmap*  	iMainBackImg;   
	CFunList* iMaksList;
	TBuf<ReaderInfo_Size>		iInfo;

	TRect despRect;
   };

#endif


