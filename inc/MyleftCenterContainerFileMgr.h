
/*
============================================================================
 Name        : FunVVSearchContainer.h 
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Declares container control for application.
============================================================================
*/

#ifndef MYLEFT_CONTAINER_FILEMGR_H
#define MYLEFT_CONTAINER_FILEMGR_H

// INCLUDES
#include <coecntrl.h>
#include <aknlists.h> 
#include <eikfrlb.h> 
#include "FunUistart.h"

//#include "FunVVGrid.h"   
// CLASS DECLARATION
// TInt KNumberOfIcons(2);//Í¼±ê¸öÊý
/**
*  CFunVVContainer  container control class.
*  
*/
class CenterCore;
class CFunList;
class CMyleftCenterViewFileMgr;
class CMyleftCenterContainerFileMgr : public CCoeControl,public MFunListObserver
    {
    public: // Constructors and destructor
        
		static CMyleftCenterContainerFileMgr* NewL(CMyleftCenterViewFileMgr *parent,const TRect& aRect);
		static CMyleftCenterContainerFileMgr* NewLC(CMyleftCenterViewFileMgr *parent,const TRect& aRect);

        /**
        * EPOC default constructor.
        * @param aRect Frame rectangle for container.
        */
        void ConstructL(CMyleftCenterViewFileMgr *parent,const TRect& aRect);
        /**
        * Destructor.
        */
        ~CMyleftCenterContainerFileMgr();

    public: // New functions
	void UpdataL();
	void DoClicked();
	void SaveCurPath(TInt _aim);
	TInt DoPlaster();
	TInt DoDelete();
	TInt DoRename(TDesC & _newName);
	TInt DoCreatFolder(TDesC & _newName);
    public: // Functions from base classes
	
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
		void MfloListItemClicked(CFunList* aButton, TInt aIndex);
		TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);    	
		/**
		* From MCoeControlObserver
		* Acts upon changes in the hosted control's state. 
		*
		* @param	aControl	The control changing its state
		* @param	aEventType	The type of control event 
		*/
	void SetupListItemsL();
		
    public: //data
	CenterCore*iplaycore;
	CMyleftCenterViewFileMgr *iparent;
	CFunList* 	iFileMgrList;
	CFbsBitmap*  	iMainBackImg;  
	TBuf<215> 	doPath;
	TBool 		is_doPathDir;
	TInt		m_arm;  // 1 -cut ,2-copy
    };

#endif


