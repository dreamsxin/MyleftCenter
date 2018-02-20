/*
============================================================================
 Name        : CMyleftCenterView from MyleftCenterView.h
 Author      : yilizheng
 Version     :
 Copyright   : MyleftCenter
 Description : Declares view for application.
============================================================================
*/

#ifndef MYLEFTCENTERVIEW_H
#define MYLEFTCENTERVIEW_H

// INCLUDES
#include <aknview.h>
#include "funuistart.h"

// CONSTANTS
// UID of view
const TUid KViewId = {2};

// FORWARD DECLARATIONS
class CMyleftCenterContainer;
class CCenterView1Styler;
// CLASS DECLARATION

/**
*  CMyleftCenterView view class.
* 
*/
class CMyleftCenterView : public CFunViewBase
    {
    public: // Constructors and destructor

        /**
        * EPOC default constructor.
        */
        void ConstructL();

        /**
        * Destructor.
        */
        ~CMyleftCenterView();

    public: // Functions from base classes
        
        /**
        * From CAknView returns Uid of View
        * @return TUid uid of the view
        */
        TUid Id() const;
 	void DynInitMenuPaneL(TInt aResourceId, CFunMenuPane* aMenuPane);

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
   	 CCenterView1Styler *	iMyStyler;
    public:
        CMyleftCenterContainer* iContainer;
    };



class CCenterView1Styler : public CBase, public MFunUiStyler
{
protected:
	CFunViewBase* iView;

public:
	CCenterView1Styler ( CFunViewBase* aView);

public:
	void InitFunControlUI(CFunBorderedControl* aControl, TInt aUiType, TAny* aData);

protected:
	void CreateTitleBar(CFunTitleBar* aTitleBar);
	void CreateControlBar(CFunControlBar* aControlBar);
	void DynInitMenuPaneL(TInt aResourceId, CFunMenuPane *aMenuPane);
};


#endif


