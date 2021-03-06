/*
============================================================================
 Name        : CFunSMusicContainer2 from FunSMusicContainer2.h
 Author      : funinhand
 Version     :
 Copyright   : Your copyright notice
 Description : Declares container control for application.
============================================================================
*/

#ifndef MYLEFT_CONTAINER_READLIST_H
#define MYLEFT_CONTAINER_READLIST_H

// INCLUDES
#include <coecntrl.h>
  #include "FunUistart.h"
#include "CenterCore.h"
// FORWARD DECLARATIONS
class CFbsBitmap;
class CMyleftCenterViewReadList;
class CFileSearchEngine;
// CLASS DECLARATION

/**
*  CFunSMusicContainer2  container control class.
*  
*/
class CMyleftCenterContainerReadList : public CCoeControl,public MFunListObserver
									,public MFileSearchEngineNotifier
									,public MFunMenuObserver
    {
    public: // Constructors and destructor

	 static CMyleftCenterContainerReadList* NewL(CMyleftCenterViewReadList* aParent, const TRect& aRect );
        static CMyleftCenterContainerReadList* NewLC(CMyleftCenterViewReadList* aParent,  const TRect& aRect );
        /**
        * EPOC default constructor.
        * @param aRect Frame rectangle for container.
        */
        void ConstructL(CMyleftCenterViewReadList* aParent, const TRect& aRect);

        /**
        * Destructor.
        */
        ~CMyleftCenterContainerReadList();

    public: // New functions
	void	UpdateListItemsL();
	void UpdateSongLrcLibL();
	void UpdatePhoneListItemsL();
    public: // Functions from base classes
	 void SearchComplete();
	TBool SearchCondition(const TDesC& aFileName) {return EFalse;}

    private: // Functions from base classes

       /**
        * From CoeControl,SizeChanged.
        */
        void SizeChanged();
	   //From MFunMenuObserver
	void ProcessMenuCommandL(TInt aCommandId);

	 void DynInitMenuBarL(TInt aResourceId, CFunMenuBar *aMenuBar){}
	 void DynInitMenuPaneL(TInt aResourceId, CFunMenuPane *aMenuPane) {}
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
      TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType) ; 
	void MfloListItemClicked(CFunList* aButton, TInt aIndex);
 	void SetupListItemsL();

private: //data

	CMyleftCenterViewReadList* iParent;
	 CenterCore *iplaycore;
  	CFbsBitmap*  	iMainBackImg;  
	CFileSearchEngine *iSearchEngine;
	CFunMenuPane* iMenuPan;
public:
	CFunLabel*	songLrcLib;
	CFunList* 	iList;
	TInt	 		icurPage;
	TInt 			iPrevIndex;
   };

#endif


