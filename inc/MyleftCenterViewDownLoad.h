/*
============================================================================
 Name        : MyleftCenterViewDownLoad.h
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Declares view for application.
============================================================================
*/

#ifndef MYLEFT_DOWNLOAD_VIEW_H
#define MYLEFT_DOWNLOAD_VIEW_H

// INCLUDES
#include <aknview.h>
#include <eikmenup.h>
#include "funuistart.h"

const TUid KViewDownLoadId = {9};

// FORWARD DECLARATIONS
class CMyleftCenterContainerDownLoad;

// CLASS DECLARATION

/**
*  CFunVVView2 view class.
* 
*/
class CMyleftCenterViewDownLoad : public CFunViewBase
    {
    public: // Constructors and destructor

        /**
        * EPOC default constructor.
        */
        void ConstructL();

        /**
        * Destructor.
        */
        ~CMyleftCenterViewDownLoad();

    public: // Functions from base classes
                        
		/**
        * From CAknView returns Uid of View
        * @return TUid uid of the view
        */
        TUid Id() const;
	 void  DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);
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
		/*
	int OnTime();
	void StartTime();
	void StopTime();*/
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
//	void SetDownLoadtabText(const TDesC& aText );
    public: // Data
   //	 WCTimer *iTimer;
        CMyleftCenterContainerDownLoad* iContainer;
    };

#endif


