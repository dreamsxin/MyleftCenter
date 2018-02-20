/*
============================================================================
 Name        : CMyleftCenterContainer2 from MyleftCenterContainer2.h
 Author      : yilizheng
 Version     :
 Copyright   : MyleftCenter
 Description : Declares container control for application.
============================================================================
*/

#ifndef MYLEFTCENTER_CONTAINER_CHATROOM_H
#define MYLEFTCENTER_CONTAINER_CHATROOM_H

// INCLUDES
#include <coecntrl.h>
#include <eikedwin.h>
#include "symbian_def.h"
#ifdef _S60_3RD_
#include <hwrmlight.h>
#endif
#include "FunUIStart.h"
#include "MyleftCenterContainerRead.h"

#define content_max  526
#define Max_line	25
// FORWARD DECLARATIONS
class CEikLabel;        // for example labels
class CenterCore;
// CLASS DECLARATION


typedef struct 
{
	TBuf<256>	m_Buf;
}SLine;
/**
*  CMyleftCenterContainer2  container control class.
*  
*/
class CCnvCharacterSetConverter;
class CMyleftCenterViewChatRoom;
class CMyleftCenterContainerChatRoom : public CCoeControl, MCoeControlObserver
												,MFunListObserver
    {
    public: // Constructors and destructor

		
        /**
        * EPOC default constructor.
        * @param aRect Frame rectangle for container.
        */
        void ConstructL(CMyleftCenterViewChatRoom *parent,const TRect& aRect);

        /**
        * Destructor.
        */
        ~CMyleftCenterContainerChatRoom();

    public: // New functions
		void UpdateListItemsL();
		void SetupListItemsL();
		void PushChatCommL(TBool _isComm);
		//void PushChatPrevL();
    public: // Functions from base classes

//	void MlloLinkClicked(CFunLinkLabel* aLinkLabel, TInt aLinkId);
	void MfloListItemClicked(CFunList* aButton, TInt aIndex);
	 
	
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
	void  ShowComm(TBool _isComm);
	//void  ShowPrev();
		/**
		* From MCoeControlObserver
		* Acts upon changes in the hosted control's state. 
		*
		* @param	aControl	The control changing its state
		* @param	aEventType	The type of control event 
		*/
	 void HandleControlEventL(CCoeControl* aControl,TCoeEvent aEventType){};
      TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType) ; 
    private: //data
        CMyleftCenterViewChatRoom  *iparent;
	 CenterCore*iplaycore;
	 
	CFunList *	iList;
	CFunEdit* iPageEdit ;
	CFunLabel*	iPageLable;
	TBuf<content_max>		icontent1;
	TBuf<content_max>		icontent2;
	CArrayFixFlat<SLine*>	*m_Commlist;
	CArrayFixFlat<SLine*>	*m_Prevlist;

	CFont *		m_font;
	TInt			m_fontH;
	TInt			m_CommLCount;
	TInt			m_PrevLCount;

	TRect iPagelabR;
	TRect iContentAllR;
	TRect iContentPrevR;
	
	TInt iListW;
	TInt iListH;
	TInt iContentW;
	TInt iComH;
	TInt iPrivH;
	//TInt iWidth;
   };

#endif


