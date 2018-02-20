/*
============================================================================
 Name        : CFunSMusicView2 from FunSMusicView2.h
 Author      : funinhand
 Version     :
 Copyright   : Your copyright notice
 Description : Declares view for application.
============================================================================
*/

#ifndef FUNSMUSICVIEW2_H
#define FUNSMUSICVIEW2_H

// INCLUDES
#include <aknview.h>
#include "FunViewBase.h"

// CONSTANTS
// UID of view
const TUid KViewReadListId = {4};

// FORWARD DECLARATIONS
class CMyleftCenterContainerReadList;
class CFunViewBase;
// CLASS DECLARATION

/**
*  CFunSMusicView2 view class.
* 
*/
class CMyleftCenterViewReadList ://	public CAknView
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
        ~CMyleftCenterViewReadList();

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
	void ChangeCbaRight( const TDesC& _textstr, TInt _cmd) ;
	void  DynInitMenuPaneL(TInt aResourceId, CFunMenuPane* aMenuPane);

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
     CMyleftCenterContainerReadList* iContainer;
	TInt 		icurSellectId;
	TInt 		icurPage;
	TInt 		iPrevIndex;
    };

#endif


