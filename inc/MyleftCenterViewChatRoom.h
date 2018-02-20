/*
============================================================================
 Name        : CMyleftCenterView2 from MyleftCenterView2.h
 Author      : yilizheng
 Version     :
 Copyright   : MyleftCenter
 Description : Declares view for application.
============================================================================
*/

#ifndef MYLEFTCENTER_VIEW_CHATROOM_H
#define MYLEFTCENTER_VIEW_CHATROOM_H

// INCLUDES
#include <aknview.h>
#include "funuistart.h"

// CONSTANTS
// UID of view
const TUid KViewChatRoom = {14};

// FORWARD DECLARATIONS
class CMyleftCenterContainerChatRoom;

// CLASS DECLARATION

/**
*  CMyleftCenterView2 view class.
* 
*/
class CMyleftCenterViewChatRoom : public CFunViewBase
    {
    public: // Constructors and destructor

        /**
        * EPOC default constructor.
        */
        void ConstructL();

        /**
        * Destructor.
        */
        ~CMyleftCenterViewChatRoom();

    public: // Functions from base classes
                        
		/**
        * From CAknView returns Uid of View
        * @return TUid uid of the view
        */
        TUid Id() const;
 	void  DynInitMenuPaneL(TInt aResourceId, CFunMenuPane* aMenuPane);

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
        CMyleftCenterContainerChatRoom* iContainer;
	TBuf<256> iText;
    };

#endif


