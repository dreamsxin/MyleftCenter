/*
============================================================================
 Name        : CFunSMusicContainer2 from FunSMusicContainer2.h
 Author      : funinhand
 Version     :
 Copyright   : Your copyright notice
 Description : Declares container control for application.
============================================================================
*/

#ifndef MYLEFT_CONTAINER_BRCTL_H
#define MYLEFT_CONTAINER_BRCTL_H

// INCLUDES
#include <coecntrl.h>
#include <eikrted.h> 
#include "FunUIStart.h"

#include <BrCtlInterface.h>
#include "BrCtlObserver.h"
// FORWARD DECLARATIONS
class CFbsBitmap;
class CenterCore;
class CFunDialog;
class CMyleftCenterViewBrCtl ;
// CLASS DECLARATION

/**
*  CFunSMusicContainer2  container control class.
*  
*/
class CMyleftCenterContainerBrCtl : public CCoeControl, MCoeControlObserver
										//,public MFunLinkLabelObserver
    {
    public: // Constructors and destructor
		
        /**
        * EPOC default constructor.
        * @param aRect Frame rectangle for container.
        */
        void ConstructL(CMyleftCenterViewBrCtl *parent,const TRect& aRect);

        /**
        * Destructor.
        */
        ~CMyleftCenterContainerBrCtl();

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
      TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType) ; 
    private: //data
        CMyleftCenterViewBrCtl  *iparent;
	 CenterCore*iplaycore;
     public: 
        CBrCtlInterface* iBrCtlInterface;  	 // Pointer to the browser control interfaces      
        TUint iBrCtlCapabilities;			  // Desired capabilities of the browser control        
        TInt iCommandBase;				// Command Base
	 
	CBrCtlSampleAppSpecialLoadObserver *iBrCtlSpecialLoadObserver;
   };
 
#endif  //FUNSMUSICCONTAINER_LIST_H


