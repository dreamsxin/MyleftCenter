/*
============================================================================
 Name        : CFunSMusicContainer2 from FunSMusicContainer2.h
 Author      : funinhand
 Version     :
 Copyright   : Your copyright notice
 Description : Declares container control for application.
============================================================================
*/

#ifndef FUNSMUSICCONTAINER_LIST_H
#define FUNSMUSICCONTAINER_LIST_H

// INCLUDES
#include <coecntrl.h>
#include <eikrted.h> 
#include "FunUIStart.h"
// FORWARD DECLARATIONS
class CFbsBitmap;
class CenterCore;
class CFunDialog;
class CMyleftCenterViewSearch;
// CLASS DECLARATION

/**
*  CFunSMusicContainer2  container control class.
*  
*/
class CMyleftCenterContainerSearch : public CCoeControl, MCoeControlObserver
										//,public MFunLinkLabelObserver
										,public MFunListObserver
										,public MFunMenuObserver
										,public MFunButtonObserver
    {
    public: // Constructors and destructor

	typedef enum TReadSearchFormID
	{
		TReadSearchPageEdit= 0,
		TReadSearchButton,
		TReadSearchPageLable,
		TReadSearchList
	};
		
        /**
        * EPOC default constructor.
        * @param aRect Frame rectangle for container.
        */
        void ConstructL(CMyleftCenterViewSearch *parent,const TRect& aRect);

        /**
        * Destructor.
        */
        ~CMyleftCenterContainerSearch();

    public: // New functions
	void SetUpHotlist();
	void UpDateHotlist();
	void SetPageNum(TInt iPageId);
    public: // Functions from base classes

//	void MlloLinkClicked(CFunLinkLabel* aLinkLabel, TInt aLinkId);
	void MfloListItemClicked(CFunList* aButton, TInt aIndex);
	void ProcessMenuCommandL(TInt aCommandId);
	void MboButtonPressed(CFunButton* aButton, TInt aCommandId);

	 void DynInitMenuBarL(TInt aResourceId, CFunMenuBar *aMenuBar){}
	 void DynInitMenuPaneL(TInt aResourceId, CFunMenuPane *aMenuPane) {}
	 
	
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
        CMyleftCenterViewSearch  *iparent;
	 CenterCore*iplaycore;
	 
  	CFbsBitmap*  	iMainBackImg;  
 	CFbsBitmap*  	iKeyImg;

	CFunForm* 	iForm;
	CFunLabel*	iPageLable;
	CFunList *	iList;
	CFunButton * Button1;
	CFunEdit * iPageEdit ;
	
	TInt xEnd;
	TInt yEnd;
	TInt iWidth;
public:
	
	CFunMenuPane* iMenuPan;
   };
 
#endif  //FUNSMUSICCONTAINER_LIST_H


