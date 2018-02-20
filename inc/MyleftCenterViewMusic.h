/*
============================================================================
 Name        : CFunSMusicView from FunSMusicView.h
 Author      : funinhand
 Version     :
 Copyright   : Your copyright notice
 Description : Declares view for application.
============================================================================
*/

#ifndef MYLEFT_VIEW_MUSIC_H
#define MYLEFT_VIEW_MUSIC_H

// INCLUDES
#include <aknview.h>

#include "FunUIStart.h"
// CONSTANTS
// UID of view
const TUid KViewMusicId = {10};


// FORWARD DECLARATIONS
class CMyleftCenterContainerMusic;


enum viewShowMode
{
	ShowModeList=0,
	ShowModeLRC,
	ShowModeSreach
};

// CLASS DECLARATION

/**
*  CFunSMusicView view class.
* 
*/
class CMyleftCenterViewMusic ://public CAknView
							public CFunViewBase
							,public MFunSliderObserver
    {
    public: // Constructors and destructor

        /**
        * EPOC default constructor.
        */
        void ConstructL();

        /**
        * Destructor.
        */
        ~CMyleftCenterViewMusic();

    public: // Functions from base classes

	void MsoGetCurrentValue( TInt aValue, TDes& aText ) ;
        /**
        * From CAknView returns Uid of View
        * @return TUid uid of the view
        */
        TUid Id() const;

	// From MFunMenuObserver
	void DynInitMenuPaneL(TInt aResourceId, CFunMenuPane *aMenuPane);


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
	void ChangeShowMode(TInt _aShowMode) ;
	void SetShowMode(TInt _aShowMode){iShowMode=_aShowMode;}  	//0=List;1=lrc,2= rearch result
	void SetLrcLibleShowId(TInt _lrcLibShowId);
	int GetCurSelindex(){return iCurSelIndex;}
	void ResetAll();
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
	TBuf<128> idesPath;
    public: // Data
        CMyleftCenterContainerMusic* iContainer;
	int  		iShowMode;
	 int 	iCurSelIndex;
	 TInt iLrcShowId;  //_TLrcLibShowId
    };

#endif


