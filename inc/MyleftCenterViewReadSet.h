/*
============================================================================
 Name        : CFunSMusicView from FunSMusicView.h
 Author      : funinhand
 Version     :
 Copyright   : Your copyright notice
 Description : Declares view for application.
============================================================================
*/

#ifndef MYLEFTCENTER_VIEW_READ_SET_H
#define MYLEFTCENTER_VIEW_READ_SET_H

// INCLUDES
#include <aknview.h>
#include <MAknFileSelectionObserver.h> 
#include "FunUIStart.h"
// CONSTANTS
// UID of view
const TUid KViewReadSetId = {7};

// FORWARD DECLARATIONS
class CenterCore;
class CMyleftCenterContainerReadSet;
class CFunSettingList;
// CLASS DECLARATION


/**
*  CFunSMusicView view class.
* 
*/
class CMyleftCenterViewReadSet:public CFunViewBase
    {
    public: // Constructors and destructor

        /**
        * EPOC default constructor.
        */
        void ConstructL();

        /**
        * Destructor.
        */
        ~CMyleftCenterViewReadSet();

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
        CMyleftCenterContainerReadSet* iContainer;
 	TInt  setType;		// 0= set all,	1 =read,2 music

    };


class CMyleftCenterContainerReadSet : public CFunSettingList, public MAknFileSelectionObserver
{
    public: // Constructors and destructor
    
	enum TSetID
	{
		ESetDownloadDir=0,	//default
		ESetHttpApn,
		ESetReadTextSize,
		ESetReadAutoPage,
		ESetReadAutoTime,
		ESetReadUseBackpic,
		ESetReadDownPic,
		ESetMusicDownPic,
		ESetMusicDownLrc
	};

        static CMyleftCenterContainerReadSet* NewL( CMyleftCenterViewReadSet* aParent,const TRect& aRect );
        static CMyleftCenterContainerReadSet* NewLC( CMyleftCenterViewReadSet* aParent,const TRect& aRect );

        /**
        * EPOC default constructor.
        * @param aRect Frame rectangle for container.
        */
        void ConstructL(CMyleftCenterViewReadSet* aParent,const TRect& aRect);

        /**
        * Destructor.
        */
        ~CMyleftCenterContainerReadSet();

    public: // New functions

    public: // Functions from base classes
		void EditItemL( TInt aIndex );
		TBool SavedAndExitL( ) ;
    private: // Functions from base classes

		TBool OkToExitL( const TDesC& aDriveAndPath, const TEntry& aEntry );
       /**
        * From CoeControl,SizeChanged.
        */
   //     void SizeChanged();

       /**
        * From CoeControl,CountComponentControls.
        */
 //       TInt CountComponentControls() const;

       /**
        * From CCoeControl,ComponentControl.
        */
 //       CCoeControl* ComponentControl(TInt aIndex) const;

       /**
        * From CCoeControl,Draw.
        */
//        void Draw(const TRect& aRect) const;

       	
	//void MfboButtonPressed(CFunButton* aButton);
//	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
/*
      void CreateButtonL(CFunButton* &aButton, TRect aRect,
									TInt aNormalImageId, TInt aClickedImageId, 
									TInt aDisabledImageId, TInt aMaskImageId, 
									const TDesC& aCaption,
									TInt aKeyCode, TBool aVisible); 
									*/
//	void SetupListItemsL();
    private: //data
    	CenterCore*	iPlayerCore;
	TBuf<256> 	iSavedPath;	
	TInt  		itime;
	TInt  		iFontSize;
	TBool 		 iAutoRool;	
	TBool 		 iBackPic;
	TBool 		 idownReadPic;
	TBool 		 idownMusicPic;
	TBool 		 idownMusicLrc;
	TUint32 _http_iap_id;
	TBuf<32> iHttpIap;	
 							
    };


#endif //FUNSMUSICVIEW_SETTING_H


