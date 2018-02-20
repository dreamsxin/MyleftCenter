#ifndef _TIMEOUT_TIMER_H_
#define _TIMEOUT_TIMER_H_

#include <e32base.h>
#include <e32std.h>
#include "TimeOutNotify.h"

class CTimeOutTimer : public CTimer {
public:
	static CTimeOutTimer* NewL (const TInt aPriority,
	MTimeOutNotify& aTimeOutNotify) {
		CTimeOutTimer* self = new(ELeave) CTimeOutTimer (
			aPriority,
			aTimeOutNotify);
		CleanupStack::PushL (self);
		self->ConstructL ();
		CleanupStack::Pop ();
		return self;
	}
	~CTimeOutTimer () {
		Cancel ();
	}
protected:
	virtual void RunL () {
		_notify.TimerExpired ();
	}
private:
	CTimeOutTimer (const TInt aPriority,
	MTimeOutNotify& aTimeOutNotify) :
		CTimer(aPriority), _notify(aTimeOutNotify) {
	}
	void ConstructL () {
		CTimer::ConstructL ();
		CActiveScheduler::Add (this);
	}
private:
	MTimeOutNotify& _notify;
};

#endif

