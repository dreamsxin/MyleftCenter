/*
============================================================================
 Name        : CFunSMusicView2 from FunSMusicView2.h
 Author      : funinhand
 Version     :
 Copyright   : Your copyright notice
 Description : Declares view for application.
============================================================================
*/

#ifndef MYLEFT_VIEW_MUSIC_SEARCH_H
#define MYLEFT_VIEW_MUSIC_SEARCH_H

// INCLUDES
#include <aknview.h>
#include "FunViewBase.h"

// CONSTANTS
// UID of view
const TUid KViewMusicSearchId = {12};

// FORWARD DECLARATIONS
class CMyleftCenterContainerMusicSearch;
class CFunViewBase;
// CLASS DECLARATION

/**
*  CFunSMusicView2 view class.
* 
*/
class CMyleftCenterViewMusicSearch ://	public CAknView
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
        ~CMyleftCenterViewMusicSearch();

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
        CMyleftCenterContainerMusicSearch* iContainer;
	TBuf<256> iText;
	TInt   iCurPage;
	TInt  	iResultPage;
    };

#endif //FUNSMUSICVIEW_SEARCH_H


