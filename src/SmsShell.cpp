#include <eikenv.h>
#include <coemain.h>
#include <e32std.h>
#include <msvids.h>
#include <msvstd.h>
#include <smsclnt.h>
#include <smut.h>
#include <mtclreg.h>
#include <txtrich.h>
#include <smscmds.h>
#include <mtmuibas.h>
#include <mtmdef.h>
#include <smutset.h>
#include <smuthdr.h>
#include <AknNoteWrappers.h>
#include <MyleftCenter.rsg>
#include <stringloader.h>
#include "SmsShell.h"
#include "CenterCore.h"

CSmsShell::CSmsShell (CSmsUiObserver* aUiObserver) : 
			CActive (CActive::EPriorityStandard) {
	CActiveScheduler::Add (this);
	iUiObserver = aUiObserver;	// pointer to observer
	iNextUnread = 0;		// index of next unread message in iSelection
}

void CSmsShell::ConstructL ()
{
	// Session to message server is opened asynchronously
	iSession = CMsvSession::OpenAsyncL (*this);

	// Entry selection for all received messages
	iSelection = new (ELeave) CMsvEntrySelection ();
}

CSmsShell* CSmsShell::NewL (CSmsUiObserver* aUiObserver)
{
	CSmsShell* self = NewLC (aUiObserver);
	CleanupStack::Pop (self);
	return self;
}

CSmsShell* CSmsShell::NewLC (CSmsUiObserver* aUiObserver)
{
	CSmsShell* self = new (ELeave) CSmsShell (aUiObserver);
	CleanupStack::PushL (self);
	self->ConstructL ();
	return self;
}

CSmsShell::~CSmsShell () 
{
	Cancel ();					//cancel any outstanding request

	delete iOperation;
	delete iMtmUiRegistry;
	delete iSelection;
	delete iSmsMtm;
	delete iMtmRegistry;
	delete iSession;			//session must be deleted last
}

void CSmsShell::DoCancel ()
{
	if (iOperation) {
		iOperation->Cancel ();
	}
}

void CSmsShell::RunL () {
	User::LeaveIfError (iStatus != KErrNone);

	// Determine the current operations progress.
	// ProgressL returns an 8 bit descriptor.
	TBufC8<KMsvProgressBufferLength> progress (iOperation->ProgressL ());

	_LIT8 (KCompare, "KErrNone");
	User::LeaveIfError (!progress.Compare (KCompare));


	// The pointer to the current CMsvOperation object is no longer needed.
	delete iOperation;
	iOperation = NULL;

	// Determine which request has finished
	switch (iState) {
	case EWaitingForMoving:
		// Once a message is moved to Outbox it is scheduled for sending
		ScheduleL ();
		break;

	case EWaitingForScheduling:
	{
		TMsvEntry entry (iSmsMtm->Entry().Entry ());
		TInt state (entry.SendingState ());
		#if 0
		if (state == KMsvSendStateWaiting || state == KMsvSendStateScheduled) {
			HBufC* text = StringLoader::LoadLC (R_SMS_MOVED_TO_OUTBOX);
			CAknInformationNote* informationNote =
				new (ELeave) CAknInformationNote;
			informationNote->ExecuteLD (*text);
			CleanupStack::PopAndDestroy (text);
		}
		#endif
		break;
	}

	default:
		break;
	}
}


void CSmsShell::HandleSessionEventL (TMsvSessionEvent aEvent,
		TAny* aArg1, TAny* aArg2, TAny*) {
	switch (aEvent) {
	// Session to server established
	case EMsvServerReady:
	{
		TMsvId serviceId (KUidMsgTypeSMS.iUid);	//SMS service id

		// Determine if the event was successful
		// ServiceProgress inserts TBuf8 value in progress
		TBuf8<KBfrLength> progress;
		iSession->ServiceProgress (serviceId, progress);
		_LIT8 (KCompare, "KErrNone");

		if (progress.Compare (KCompare)) {
			#if 0
			HBufC* text = StringLoader::LoadLC (R_SMS_SERVER_SESSION);
			CAknInformationNote* informationNote = new (ELeave) CAknInformationNote;
			informationNote->ExecuteLD (*text);
			CleanupStack::PopAndDestroy (text);
			#endif
			// Check that MtmRegistry has not already been accessed
			if (!iMtmRegistry) {
				AccessMtmL ();
			}
		} else {
			#if 0
			HBufC* text = StringLoader::LoadLC (R_SMS_SESSION_ERROR);
			CAknErrorNote* errorNote = new (ELeave) CAknErrorNote;
			errorNote->ExecuteLD (*text);
			CleanupStack::PopAndDestroy (text);
			#endif
		}
		break;
	}
	
	// A new entry has been created on the message server
	case EMsvEntriesCreated:
	{
		// Entry id is obtained from the session event arguments
		TMsvId* entryId = STATIC_CAST (TMsvId*, aArg2);

		// We are interested in messages that are created in Inbox
		if (*entryId != KMsvGlobalInBoxIndexEntryId) {
			break;
		}

		// We take the created entried into a selection
		CMsvEntrySelection* newEntries =
			STATIC_CAST (CMsvEntrySelection*, aArg1);

		// Process each created entry
		for (TInt i(0); i < newEntries->Count(); i++) {
			// We are interested in SMS message
			if ((iSession->GetEntryL (newEntries->At (i)))->
					Entry().iMtm == KUidMsgTypeSMS) {
				// Add the entry to the selection of all received messages.
				iSelection->AppendL (newEntries->At(i), 1);

				// Set received message visible
				MessageReceivedL (newEntries->At (i));
			}
		}
		break;
	}

	case EMsvCloseSession:
	case EMsvServerTerminated:
	case EMsvGeneralError:
	case EMsvServerFailedToStart:
		if (iUiObserver)
			iUiObserver->ServerDown ();		// close application	
		{
			#if 0
			HBufC* text = StringLoader::LoadLC (R_SMS_SESSION_ERROR);
			CAknErrorNote* errorNote = new (ELeave) CAknErrorNote;
			errorNote->ExecuteLD (*text);
			CleanupStack::PopAndDestroy (text);
			#endif
		}
		break;
	
	default:
		break;
	}
}


// Access the MTM Registry and Create a SMS specific Client MTM instance
void CSmsShell::AccessMtmL () {
	// Create an MTM Registry object
	iMtmRegistry = CClientMtmRegistry::NewL (*iSession);

	// Create an SMS client MTM object
	iSmsMtm = STATIC_CAST (CSmsClientMtm*,
			iMtmRegistry->NewMtmL (KUidMsgTypeSMS));
}

// Starts the process of creating and sending an SMS message
TBool CSmsShell::SendL (const TDesC& aRecipientNumber,
				const TDesC& aMessageText) {
	iRecipientNumber.Copy (aRecipientNumber);
	iMessageText.Copy (aMessageText);

	if (CreateMsgL ()) {
		return ETrue;
	}
	return EFalse;
}

#ifndef _NOKIA_90_
TBool CSmsShell::CreateMsgL () {
	// Current entry is the Draft folder
	iSmsMtm->SwitchCurrentEntryL (KMsvDraftEntryId);

	// Create a new SMS message entry as a child of the current context
	iSmsMtm->CreateMessageL (KUidMsgTypeSMS.iUid);

	CMsvEntry& serverEntry = iSmsMtm->Entry ();
	TMsvEntry entry (serverEntry.Entry ());

	CRichText& body = iSmsMtm->Body (); 	// the body of the message
	body.Reset ();

	// Insert the message text gotten as input from user
	body.InsertL (0, iMessageText);

	// Message will be sent immediately
	entry.SetSendingState (KMsvSendStateWaiting);
	// insert current time
	entry.iDate.HomeTime ();

	// Set the SMS message settings for the message
	CSmsHeader& header = iSmsMtm->SmsHeader ();
	CSmsSettings* settings = CSmsSettings::NewL ();
	CleanupStack::PushL (settings);

	settings->CopyL (iSmsMtm->ServiceSettings ());	// restore settings
	settings->SetDelivery (ESmsDeliveryImmediately); // to be delivered immediately
	settings->SetCharacterSet(TSmsDataCodingScheme::ESmsAlphabetUCS2) ; 
	header.SetSmsSettingsL (*settings);			// new settings

	// Let's check if there is a service center address
	if (header.Message().ServiceCenterAddress().Length() == 0) {
		// No, there isn't. We assume there is at least one service
		// center number set and use the default service center number
		CSmsSettings* serviceSettings = &(iSmsMtm->ServiceSettings());

		// Check if number of service center addresses in the list is null
		if (!serviceSettings->NumSCAddresses()) {
			#if 0
			HBufC* text = StringLoader::LoadLC (R_SMS_SC_NBR_MISSING);
			CAknWarningNote* warningNote = new (ELeave) CAknWarningNote;
			warningNote->ExecuteLD (*text);
			CleanupStack::PopAndDestroy (text);
			#endif

			// Remember to pop settings even if this branch is taken
			CleanupStack::PopAndDestroy (settings);
			return EFalse;
		} else {
			// Set service center address to default
			// The caller does not take ownership of the returned object
			CSmsNumber* smsCenter =
				&(serviceSettings->SCAddress (serviceSettings->DefaultSC()));
			header.Message().SetServiceCenterAddressL (smsCenter->Address());
		}
	}
	CleanupStack::PopAndDestroy (settings);

	// Recipient number is displayed also as the recipient alias
	entry.iDetails.Set (iRecipientNumber);

	// Add addressee
	iSmsMtm->AddAddresseeL (iRecipientNumber, entry.iDetails);

	// Validate message
	if (!ValidateL ()) {
		return EFalse;
	}

	entry.SetVisible (ETrue);		// set message as visible
	entry.SetInPreparation (EFalse);	// set together with the visibility flag
	serverEntry.ChangeL (entry);	// commit changes
	iSmsMtm->SaveMessageL ();		// save message

	TMsvSelectionOrdering selection;
	CMsvEntry* parentEntry = 
		CMsvEntry::NewL (iSmsMtm->Session(), KMsvDraftEntryId, selection);
	CleanupStack::PushL (parentEntry);

	// Move message to OutBox
	iOperation = parentEntry->MoveL (
			entry.Id(), KMsvGlobalOutBoxIndexEntryId, iStatus);

	CleanupStack::PopAndDestroy (parentEntry);

	iState = EWaitingForMoving;
	SetActive ();


	return ETrue;
}
#else
TBool CSmsShell::CreateMsgL () {
	// Current entry is the Draft folder
	iSmsMtm->SwitchCurrentEntryL (KMsvDraftEntryId);

	// Create a new SMS message entry as a child of the current context
	iSmsMtm->CreateMessageL (KUidMsgTypeSMS.iUid);

	CMsvEntry& serverEntry = iSmsMtm->Entry ();
	TMsvEntry entry (serverEntry.Entry ());

	CRichText& body = iSmsMtm->Body (); 	// the body of the message
	body.Reset ();

	// Insert the message text gotten as input from user
	body.InsertL (0, iMessageText);

	// Message will be sent immediately
	entry.SetSendingState (KMsvSendStateWaiting);
	// insert current time
	entry.iDate.HomeTime ();

	// Set the SMS message settings for the message
	CSmsHeader& header = iSmsMtm->SmsHeader ();
	CSmsSettings* settings = CSmsSettings::NewL ();
	CleanupStack::PushL (settings);

	settings->CopyL (iSmsMtm->ServiceSettings ());	// restore settings
	settings->SetDelivery (ESmsDeliveryImmediately); // to be delivered immediately
	settings->SetCharacterSet(TSmsDataCodingScheme::ESmsAlphabetUCS2) ; 
	header.SetSmsSettingsL (*settings);			// new settings

	// Let's check if there is a service center address
	if (header.Message().ServiceCenterAddress().Length() == 0) {
		// No, there isn't. We assume there is at least one service
		// center number set and use the default service center number
		CSmsSettings* serviceSettings = &(iSmsMtm->ServiceSettings());

		// Check if number of service center addresses in the list is null
		if (!serviceSettings->ServiceCenterCount()) {
#if 0
			HBufC* text = StringLoader::LoadLC (R_SMS_SC_NBR_MISSING);
			CAknWarningNote* warningNote = new (ELeave) CAknWarningNote;
			warningNote->ExecuteLD (*text);
			CleanupStack::PopAndDestroy (text);
#endif

			// Remember to pop settings even if this branch is taken
			CleanupStack::PopAndDestroy (settings);
			return EFalse;
		} else {
			// Set service center address to default
			// The caller does not take ownership of the returned object
			CSmsServiceCenter* smsCenter =
				&(serviceSettings->GetServiceCenter (serviceSettings->DefaultServiceCenter()));
			header.Message().SetServiceCenterAddressL (smsCenter->Address());
		}
	}
	CleanupStack::PopAndDestroy (settings);

	// Recipient number is displayed also as the recipient alias
	entry.iDetails.Set (iRecipientNumber);

	// Add addressee
	iSmsMtm->AddAddresseeL (iRecipientNumber, entry.iDetails);

	// Validate message
	if (!ValidateL ()) {
		return EFalse;
	}

	entry.SetVisible (ETrue);		// set message as visible
	entry.SetInPreparation (EFalse);	// set together with the visibility flag
	serverEntry.ChangeL (entry);	// commit changes
	iSmsMtm->SaveMessageL ();		// save message

	TMsvSelectionOrdering selection;
	CMsvEntry* parentEntry = 
		CMsvEntry::NewL (iSmsMtm->Session(), KMsvDraftEntryId, selection);
	CleanupStack::PushL (parentEntry);

	// Move message to OutBox
	iOperation = parentEntry->MoveL (
		entry.Id(), KMsvGlobalOutBoxIndexEntryId, iStatus);

	CleanupStack::PopAndDestroy (parentEntry);

	iState = EWaitingForMoving;
	SetActive ();


	return ETrue;
}
#endif

TBool CSmsShell::ValidateL () {
	// Empty part list to hold the result
	TMsvPartList result (KMsvMessagePartNone);

	// Validate message body
	result = iSmsMtm->ValidateMessage (KMsvMessagePartBody);

	if (result != KMsvMessagePartNone) {
		#if 0
		HBufC* text = StringLoader::LoadLC (R_SMS_MSG_BODY);
		CAknWarningNote* warningNote = new (ELeave) CAknWarningNote;
		warningNote->ExecuteLD (*text);
		CleanupStack::PopAndDestroy (text);
		#endif
		return EFalse;
	}

	// Validate recipient
	result = iSmsMtm->ValidateMessage (KMsvMessagePartRecipient);

	if (result != KMsvMessagePartNone) {
		#if 0
		HBufC* text = StringLoader::LoadLC (R_SMS_RECIPIENT_NBR);
		CAknWarningNote* warningNote = new (ELeave) CAknWarningNote;
		warningNote->ExecuteLD (*text);
		CleanupStack::PopAndDestroy (text);
		#endif
		return EFalse;
	}
	return ETrue;
}

void CSmsShell::ScheduleL () {
	CMsvEntrySelection* selection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL (selection);
	// add message to selection
	selection->AppendL (iSmsMtm->Entry().EntryId());

	// Add entry to task scheduler
	// dummy parameters needed for InvokeAsyncFunctionL
	TBuf8<1> dummyParams;
	iOperation = iSmsMtm->InvokeAsyncFunctionL (
			ESmsMtmCommandScheduleCopy,
			*selection, dummyParams, iStatus);

	CleanupStack::PopAndDestroy (selection);

	iState = EWaitingForScheduling;
	SetActive ();
}

void CSmsShell::MessageReceivedL (TMsvId aEntryId) {
	CMsvEntry* serverEntry = iSession->GetEntryL (aEntryId);
	CleanupStack::PushL (serverEntry);

	// currently handled message entry
	TMsvEntry entry = serverEntry->Entry ();	

	entry.SetNew (ETrue);
	entry.SetUnread (ETrue);
	entry.SetVisible (ETrue);

	serverEntry->ChangeL (entry);	//commit changes
	if (iUiObserver)
		iUiObserver->MessageReceived ();	// let UI know we have received a message
	
	CleanupStack::PopAndDestroy (serverEntry);

	#if 0
	HBufC* text = StringLoader::LoadLC (R_SMS_MSG_RECEIVED);
	CAknInformationNote* informationNote =
			new (ELeave) CAknInformationNote;
	informationNote->ExecuteLD (*text);
	CleanupStack::PopAndDestroy (text);
	#endif
}


void CSmsShell::ViewL () {
	// There is an own registry for UI MTM's
	iMtmUiRegistry = CMtmUiRegistry::NewL (*iSession);

	// We are interested in the next unread message
	TMsvId entryId (iSelection->At (iNextUnread));
	CMsvEntry* serverEntry = iSession->GetEntryL (entryId);
	CleanupStack::PushL (serverEntry);

	// Create new MTM
	CBaseMtm* clientMtm = iMtmRegistry->NewMtmL (
					serverEntry->Entry().iMtm);
	CleanupStack::PushL (clientMtm);
	clientMtm->SwitchCurrentEntryL (serverEntry->EntryId());

	// Check if there are more unreads messages
	iNextUnread ++;
	if (iNextUnread < iSelection->Count()) {
		if (iUiObserver)
			iUiObserver->MessageReceived ();	// still message to read
	} else {
		if (iUiObserver)
			iUiObserver->NoMoreUnread ();		// no more messages to read
	}

	TMsvEntry entry (serverEntry->Entry ());
	entry.SetNew (EFalse);		// message is no longer new
	entry.SetUnread (EFalse);	// message is no longer unread
	serverEntry->ChangeL (entry);	// commit changes

	// UI MTM for SMS
	CBaseMtmUi* ui = iMtmUiRegistry->NewMtmUiL (*clientMtm);
	CleanupStack::PushL (ui);

	// Display the SMS using the UI MTM
	iOperation = ui->ViewL (iStatus);

	CleanupStack::PopAndDestroy (3); //ui, clientMtm, serverEntry
	SetActive ();
}


