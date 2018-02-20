/*
============================================================================
 Name        : CMyleftCenterContainer from MyleftCenterContainer.h
 Author      : yilizheng
 Version     :
 Copyright   : MyleftCenter
 Description : Declares container control for application.
============================================================================
*/

#ifndef MYLEFTCENTERCONTAINER_H
#define MYLEFTCENTERCONTAINER_H

// INCLUDES
#include <coecntrl.h>
#include "FunUIStart.h"   
// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  CMyleftCenterContainer  container control class.
*  
*/
class CMyleftCenterContainer : public CCoeControl,public MFunTimeOutNotify
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
        ~CMyleftCenterContainer();

    public: // New functions
		void SetupScrollTask(const TDesC& _title);
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
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);

	void ShowScrollTask();
	

    private: //data
    	TInt 				iFocusID;
        CFbsBitmap*  	iMainBackImg;  
	 CFbsBitmap*  	iMainImg;  
	  CFbsBitmap*  	iBottomImg; 
	 CArrayFixFlat<CFbsBitmap*> *m_iconlist;
	 CFunTimeOutTimer* iTimer;
	 TInt iSrcollWidth	;
	 TInt iCurStartX;
	 TInt iLineCount;
	 TInt iIconGap;
    };

#endif


