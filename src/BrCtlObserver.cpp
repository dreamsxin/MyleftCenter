/*
* ============================================================================
*  Name        : BrCtlSampleAppSpecialLoadObserver.cpp
*  Part of     : BrCtlSampleApp
*  Interface   : Browser Control API
*  Description : Handle special load requests like Non-HTTP(S) load requests,
*                and Non-HTML content. Control over network connections
*  Version     : 
*
*  Copyright (c) 2005-2006 Nokia Corporation.
*  This material, including documentation and any related 
*  computer programs, is protected by copyright controlled by 
*  Nokia Corporation.
* ============================================================================
*/

// INCLUDE FILES
#include <apmrec.h>
#include <apgcli.h>
#include <eikappui.h>
#include <eikenv.h>
#include "BrCtlObserver.h"
#include <avkon.hrh>
#include <aknmessagequerydialog.h>
#include <CommDbConnPref.h>

#include "centercore.h"
// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CBrCtlSampleAppSpecialLoadObserver::NetworkConnectionNeededL
// ---------------------------------------------------------
//
void CBrCtlSampleAppSpecialLoadObserver::NetworkConnectionNeededL(TInt* aConnectionPtr,
                                      TInt* aSockSvrHandle,
                                      TBool* aNewConn,
                                      TApBearerType* aBearerType) 
{
	*aBearerType = EApBearerTypeAllBearers;


	if(iFirstTime)
	{
		//New connection is established only once
		User::LeaveIfError(iSocketServer.Connect(KESockDefaultMessageSlots));
		User::LeaveIfError(iConnection.Open(iSocketServer, KConnectionTypeDefault));
		TCommDbConnPref prefs;
		prefs.SetDialogPreference(ECommDbDialogPrefDoNotPrompt);
		prefs.SetDirection(ECommDbConnectionDirectionOutgoing);
		prefs.SetIapId( CenterCore::GetInstance()->_http_iap_id);        //preferred IAP
		User::LeaveIfError(iConnection.Start(prefs));
		*aNewConn = ETrue;
		iFirstTime = EFalse;
	}
	else
	{
		*aNewConn = EFalse;
	}


	*aConnectionPtr = reinterpret_cast<TInt>(&iConnection);
	*aSockSvrHandle = iSocketServer.Handle();
	return;


}

// ---------------------------------------------------------
// CBrCtlSampleAppSpecialLoadObserver::HandleRequestL
// ---------------------------------------------------------
//
TBool CBrCtlSampleAppSpecialLoadObserver::HandleRequestL(RArray<TUint>* aTypeArray, CDesCArrayFlat* aDesArray) 
    {	
    TInt i;
    // The 2 arrays must be in sync. Each element in iTypeArray 
    // identifies the type of the corresponding element in iDesArray.
    if( aTypeArray->Count() != aDesArray->Count() )
        {
        User::Leave( KErrArgument );
        }

	// Get url
    HBufC* url = NULL;
    // Search url in the param list
    for( i = 0; i < aTypeArray->Count(); i++ )
        {
        if( (*aTypeArray)[i] == EParamRequestUrl )
            {
            // the url is found
            url = HBufC::NewLC( (*aDesArray)[i].Length() );
            url->Des().Copy( (*aDesArray)[i] );
            break;
            }
        }
    if( !url )
        {
        // The url wasn't found. 
        User::Leave( KErrArgument );
        }

	// Get referrer header
    HBufC* refererHeader = NULL;
    // Search url in the param list
    for( i = 0; i < aTypeArray->Count(); i++ )
        {
        if( (*aTypeArray)[i] == EParamRefererHeader )
            {
            // the referer Header is found
            refererHeader = HBufC::NewLC( (*aDesArray)[i].Length() );
            refererHeader->Des().Copy( (*aDesArray)[i] );
            break;
            }
        }

	// This is where you would place your scheme handling code
	// For example a scheme could be "rtsp://" or "sms:"
	// The scheme handler is then responsible for dispatching the request to 
	// the correct component.

    if( refererHeader )
		{
			CleanupStack::PopAndDestroy( 1 ); // refererHeader
		}

    CleanupStack::PopAndDestroy( 1 ); // url
    return ETrue;
    }

// ---------------------------------------------------------
// CBrCtlSampleAppSpecialLoadObserver::HandleDownloadL
// ---------------------------------------------------------
//
TBool CBrCtlSampleAppSpecialLoadObserver::HandleDownloadL(RArray<TUint>* aTypeArray, CDesCArrayFlat* aDesArray)
    {
    TInt i = 0;
    TInt count = aTypeArray->Count();
    for (i = 0; i < count; i++)
        {
        if ((*aTypeArray)[i] == EParamLocalFileName && aDesArray[i].Length() > 0)
            {
            TInt j = 0;
            for (j = 0; j < count; j++)
                {
                if ((*aTypeArray)[j] == EParamReceivedContentType)
                    {
                    HBufC8* dataType8 = HBufC8::NewLC((*aDesArray)[j].Length());
                    dataType8->Des().Copy((*aDesArray)[j]);
                    TDataType dataType(*dataType8);
                    iHandler->OpenFileEmbeddedL((*aDesArray)[i], dataType);
                    CleanupStack::PopAndDestroy();
                    break;
                    }
                }
            break;
            }
        }
    return EFalse;
    }

// ---------------------------------------------------------
// CBrCtlSampleAppSpecialLoadObserver::ConstructL
// ---------------------------------------------------------
//
void CBrCtlSampleAppSpecialLoadObserver::ConstructL()
    {
   iHandler = CDocumentHandler::NewL() ;
   iFirstTime = ETrue;
    }

// ---------------------------------------------------------
// CBrCtlSampleAppSpecialLoadObserver::~CBrCtlSampleAppSpecialLoadObserver
// ---------------------------------------------------------
//
CBrCtlSampleAppSpecialLoadObserver::~CBrCtlSampleAppSpecialLoadObserver()
    {
    delete iHandler;
    }


// ---------------------------------------------------------
// CBrCtlSampleAppSpecialLoadObserver::NewL
// ---------------------------------------------------------
//
CBrCtlSampleAppSpecialLoadObserver* CBrCtlSampleAppSpecialLoadObserver::NewL()
    {
    CBrCtlSampleAppSpecialLoadObserver* self = new(ELeave)CBrCtlSampleAppSpecialLoadObserver;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// End of File  

