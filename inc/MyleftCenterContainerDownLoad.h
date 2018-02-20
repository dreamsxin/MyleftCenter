
/*
============================================================================
 Name        : FunVVSearchContainer.h 
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Declares container control for application.
============================================================================
*/

#ifndef FUNVV_CONTAINER_DOWNLOAD_H
#define FUNVV_CONTAINER_DOWNLOAD_H

// INCLUDES
#include <coecntrl.h>
#include <aknlists.h> 
#include <eikfrlb.h> 
#include "FunUistart.h"

//#include "FunVVGrid.h"   
// CLASS DECLARATION
// TInt KNumberOfIcons(2);//Í¼±ê¸öÊý
/**
*  CFunVVContainer  container control class.
*  
*/
class CenterCore;
class CFunList;
class CMyleftCenterContainerDownLoad : public CCoeControl,public MFunListObserver
											,public MFunTimeOutNotify
    {
    public: // Constructors and destructor
        
		static CMyleftCenterContainerDownLoad* NewL(const TRect& aRect);
		static CMyleftCenterContainerDownLoad* NewLC(const TRect& aRect);

        /**
        * EPOC default constructor.
        * @param aRect Frame rectangle for container.
        */
        void ConstructL(const TRect& aRect);
        /**
        * Destructor.
        */
        ~CMyleftCenterContainerDownLoad();

    public: // New functions
	 void UpdataL();
	void StartTime()	;
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
		void MftonTimerExpired();
		void MfloListItemClicked(CFunList* aButton, TInt aIndex);
		TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);    	
		/**
		* From MCoeControlObserver
		* Acts upon changes in the hosted control's state. 
		*
		* @param	aControl	The control changing its state
		* @param	aEventType	The type of control event 
		*/
	void CreateListL();
	void SetupListItemsL();
  public: //data

		
    public: //data
    	CenterCore*iplaycore;

	CFunTimeOutTimer* iTimer;
	CFunList* iDownloadList;
  	CFbsBitmap*  	iMainBackImg;  
	 TBool  is_downloadView;
	 TInt iCurdownId;
	 TUint	size;
	 TUint	length;

    };

#endif


