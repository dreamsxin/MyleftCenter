#ifndef __SMSSHELL_H_
#define __SMSSHELL_H_

#include <e32base.h>
#include <msvapi.h>
#include <mtuireg.h>

const TInt KBfrLength = 20;
const TInt EMaxTelephoneNumberLength = 15;
const TInt EMaxMessageLength = 160;

class CClientMtmRegistry;
class CSmsClientMtm;


class CSmsUiObserver {
public:
	virtual void ServerDown () = 0;
	virtual void MessageReceived () = 0;
	virtual void NoMoreUnread () = 0;
};

class CSmsShell : public CActive, public MMsvSessionObserver {
public:
	static CSmsShell* NewL (CSmsUiObserver* aUiObserver);
	static CSmsShell* NewLC (CSmsUiObserver* aUiObserver);
	virtual ~CSmsShell ();

public:
	TBool SendL (const TDesC& aRecipientNumber, 
				const TDesC& aMessageText);

	void ViewL ();

public:
	void HandleSessionEventL (TMsvSessionEvent aEvent,
			TAny* aArg1, TAny* aArg2, TAny* aArg3);

protected:
	void DoCancel ();
	void RunL ();
	void IssueRequestComplete (TInt _value);

private:
	CSmsShell (CSmsUiObserver* aUiObserver);
	void ConstructL ();

private:
	void AccessMtmL ();
	TBool CreateMsgL ();
	void ScheduleL ();
	void MessageReceivedL (TMsvId aEntryId);
	TBool ValidateL ();

private:
	enum TState {
		EWaitingForMoving = 1,
		EWaitingForScheduling
	};

private:
	TState iState;
	CMsvSession* iSession;
	CClientMtmRegistry* iMtmRegistry;
	CSmsClientMtm* iSmsMtm;
	CMsvOperation* iOperation;
	TBuf<EMaxTelephoneNumberLength> iRecipientNumber;
	TBuf<EMaxMessageLength> iMessageText;
	CSmsUiObserver* iUiObserver;
	CMtmUiRegistry* iMtmUiRegistry;
	CMsvEntrySelection* iSelection;
	TInt iNextUnread;
};
	
#endif

