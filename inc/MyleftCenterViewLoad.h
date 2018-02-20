/*
============================================================================
 Name        : CFunSMusicView from FunSMusicView.h
 Author      : funinhand
 Version     :
 Copyright   : Your copyright notice
 Description : Declares view for application.
============================================================================
*/

#ifndef FUNSMUSICVIEW_LOAD_H
#define FUNSMUSICVIEW_LOAD_H

// INCLUDES
#include <aknview.h>

#include "FunUIStart.h"
// CONSTANTS
// UID of view
const TUid KViewLoadId = {1};

// FORWARD DECLARATIONS
class CMyleftCenterContainerLoad;

// CLASS DECLARATION

/**
*  CFunSMusicView view class.
* 
*/
class CMyleftCenterViewLoad ://public CAknView
							public CFunViewBase
    {
    public: // Constructors and destructor

        /**
        * EPOC default constructor.
        */
        void ConstructL();

        /**
        * Destructor.
        */
        ~CMyleftCenterViewLoad();

    public: // Functions from base classes
        
        /**
        * From CAknView returns Uid of View
        * @return TUid uid of the view
        */
        TUid Id() const;

        /**
        * From MEikMenuObserver delegate commands from the menu
        * @param aCommand a command emitted by the menu 
        * @return void
        */
        void HandleCommandL(TInt aCommand);

        /**
        * From CAknView reaction if size change
        * @return void
        */
        void HandleClientRectChange();

    private:

        /**
        * From CAknView activate the view
        * @param aPrevViewId 
        * @param aCustomMessageId 
        * @param aCustomMessage 
        * @return void
        */
        void DoActivateL(const TVwsViewId& aPrevViewId,TUid aCustomMessageId,
            const TDesC8& aCustomMessage);

        /**
        * From CAknView deactivate the view (free resources)
        * @return void
        */
        void DoDeactivate();

    public: // Data
        CMyleftCenterContainerLoad* iContainer;
    };


class CMyleftCenterContainerLoad : public CCoeControl
{
    public: // Constructors and destructor

        static CMyleftCenterContainerLoad* NewL( CMyleftCenterViewLoad* parent,const TRect& aRect );
        static CMyleftCenterContainerLoad* NewLC( CMyleftCenterViewLoad* parent, const TRect& aRect );

        /**
        * EPOC default constructor.
        * @param aRect Frame rectangle for container.
        */
        void ConstructL( CMyleftCenterViewLoad* parent,const TRect& aRect);

        /**
        * Destructor.
        */
        ~CMyleftCenterContainerLoad();

    public: // New functions
		void StopAndCancle() ;

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

       	
	//void MfboButtonPressed(CFunButton* aButton);
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
/*
      void CreateButtonL(CFunButton* &aButton, TRect aRect,
									TInt aNormalImageId, TInt aClickedImageId, 
									TInt aDisabledImageId, TInt aMaskImageId, 
									const TDesC& aCaption,
									TInt aKeyCode, TBool aVisible); 
									*/
	void SetupListItemsL();
    private: //data
    	
     CMyleftCenterViewLoad* m_parent;
	CFunImageAnimation  *iMagAni;
	CFbsBitmap*  	iMainBackImg;  
    };
#endif


