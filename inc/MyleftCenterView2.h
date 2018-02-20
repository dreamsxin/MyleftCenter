/*
============================================================================
 Name        : CMyleftCenterView2 from MyleftCenterView2.h
 Author      : yilizheng
 Version     :
 Copyright   : MyleftCenter
 Description : Declares view for application.
============================================================================
*/

#ifndef MYLEFTCENTERVIEW2_H
#define MYLEFTCENTERVIEW2_H

// INCLUDES
#include <aknview.h>
#include "funuistart.h"

// CONSTANTS
// UID of view
const TUid KView2Id = {3};

// FORWARD DECLARATIONS
class CMyleftCenterContainer2;
// CLASS DECLARATION

/**
*  CMyleftCenterView2 view class.
* 
*/
class CMyleftCenterView2 : public CFunViewBase
    {
    public: // Constructors and destructor

        /**
        * EPOC default constructor.
        */
        void ConstructL();

        /**
        * Destructor.
        */
        ~CMyleftCenterView2();

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

    private: // Data
        CMyleftCenterContainer2* iContainer;
//	 CCenterViewStyler1 *	iMyStyler;
    };

#endif


