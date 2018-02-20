/*
============================================================================
 Name        : CMyleftCenterAppUi from MyleftCenterAppui.h
 Author      : yilizheng
 Version     :
 Copyright   : MyleftCenter
 Description : Declares UI class for application.
============================================================================
*/

#ifndef MYLEFTCENTERAPPUI_H
#define MYLEFTCENTERAPPUI_H

// INCLUDES
#include <aknviewappui.h>
#include "FunUIStart.h"
#include "portab_symbian.h"

#define TIME_OUT_TIME	1

// FORWARD DECLARATIONS
class CMyleftCenterContainer;
class CCenterUiStyler;
class CCenterUiStyler2;

class CMyleftCenterView;
class CMyleftCenterView2;
class CMyleftCenterViewLoad;
class CMyleftCenterViewReadList;
class CMyleftCenterViewSearch;
class  CMyleftCenterViewRead;
class CMyleftCenterViewReadSet;
class CMyleftCenterViewFileMgr;
class CMyleftCenterViewDownLoad;
class CMyleftCenterViewMusic;
class CMyleftCenterViewMusicList;
class CMyleftCenterViewMusicSearch;
#ifdef _S60_3RD_
class  CMyleftCenterViewBrCtl;
#endif
class CMyleftCenterViewChatRoom;
// CLASS DECLARATION

/**
* Application UI class.
* Provides support for the following features:
* - EIKON control architecture
* - view architecture
* - status pane
* 
*/
class CMyleftCenterAppUi : public CFunAppUi,public MFunTimeOutNotify
    {
    public: // // Constructors and destructor

        /**
        * EPOC default constructor.
        */      
        void ConstructL();

        /**
        * Destructor.
        */      
        ~CMyleftCenterAppUi();
        
    public: // New functions

    public: // Functions from base classes
	void TabChangedL(TInt aIndex);
	void MftonTimerExpired() ;		//MFunTimeOutNotify
	void HandleForegroundEventL( TBool aForeground ) ;
    private:
        // From MEikMenuObserver
        void DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane);

    public:
        /**
        * From CEikAppUi, takes care of command handling.
        * @param aCommand command to be handled
        */
        void HandleCommandL(TInt aCommand);
#ifdef _S60_3RD_
	void HandleWsEventL(const TWsEvent &aEvent, CCoeControl *aDestination);
#endif
	 TInt DiskInfoL(TInt64& aTotal, TInt64& aFree, const TDriveNumber aVolume);
	   void SendToBackground ();
	   void  OpenOtherFileL(const TDesC& aFileName);
	   void SendSms( const TDesC& aNumber, const TDesC& aText ) ;
	   void AutoStartSmsDaemon();
        /**
        * From CEikAppUi, handles key events.
        * @param aKeyEvent Event to handled.
        * @param aType Type of the key event. 
        * @return Response code (EKeyWasConsumed, EKeyWasNotConsumed). 
        */
        virtual TKeyResponse HandleKeyEventL(
            const TKeyEvent& aKeyEvent,TEventCode aType);

    private: //Data
   	 int 		flag;
	CFunTimeOutTimer* iTimer;
	CFunLabel* iTimeLabel;
	

public:
	CCenterUiStyler *	iMyStyler;
	CCenterUiStyler2 *	iMyStyler2;
	
	CMyleftCenterView* 		view1 ;
	CMyleftCenterView2* 		view2; 
	CMyleftCenterViewLoad*	viewLoad;
	CMyleftCenterViewReadList*	viewReadList;
	CMyleftCenterViewSearch*	viewSearch;
	CMyleftCenterViewRead *	viewRead;
	CMyleftCenterViewReadSet* viewReadSet;
	CMyleftCenterViewFileMgr *	viewFileMgr;
	CMyleftCenterViewDownLoad* viewDownLoad;

	CMyleftCenterViewMusic* 		viewMusic;
	CMyleftCenterViewMusicList*		viewMusicList;
	CMyleftCenterViewMusicSearch*  viewMusicSearch;
	CMyleftCenterViewChatRoom*  viewChatRoom;
#ifdef _S60_3RD_
	CMyleftCenterViewBrCtl*  viewBrCtl;
#endif
    };


class CCenterUiStyler : public CBase, public MFunUiStyler
{
protected:
	CFunAppUi* iAppUi;

public:
	CCenterUiStyler ( CFunAppUi* aAppUi );

public:
	void InitFunControlUI(CFunBorderedControl* aControl, TInt aUiType, TAny* aData);

protected:
	void CreateTitleBar(CFunTitleBar* aTitleBar);
	void CreateControlBar(CFunControlBar* aControlBar);
	void DynInitMenuPaneL(TInt aResourceId, CFunMenuPane *aMenuPane);
};

class CCenterUiStyler2 : public CBase, public MFunUiStyler
{
protected:
	CFunAppUi* iAppUi;

public:
	CCenterUiStyler2 ( CFunAppUi* aAppUi);

public:
	void InitFunControlUI(CFunBorderedControl* aControl, TInt aUiType, TAny* aData);

protected:
	void CreateTitleBar(CFunTitleBar* aTitleBar);
	void CreateControlBar(CFunControlBar* aControlBar);
	void DynInitMenuPaneL(TInt aResourceId, CFunMenuPane *aMenuPane);
};

#endif


