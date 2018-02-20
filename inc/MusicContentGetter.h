#include "HttpGetter.h"
#include <aknprogressdialog.h> 
#include "FunUIStart.h"
#ifndef _MUSICCONTENT_GETTER_H_
#define _MUSICCONTENT_GETTER_H_

class MMusicContentGetterNotifier;

class CMusicContentGetter: public CBase
	,public MHttpGetterObserver, public MFunWaitDialogObserver 
{
	
public:
	static CMusicContentGetter* NewLC(TInt aIapId);
	static CMusicContentGetter* NewL(TInt aIapId);
	~CMusicContentGetter();
	
protected:
	void ConstructL(TInt aIapId);
	CMusicContentGetter();
	
protected:
	CenterCore*iCenterCore;

	// Http Getter
	CHttpGetter *iHttpGetter;
	
	// The file name when need to save content
	TFileName iSavedFileName;
	
	// The buffer to contain content
	HBufC8* iMusicContentText_hbuf;
	TPtr8 iMusicContentText;
	TUint8 _fake_musiccontent[4];

	// The buffer to contain post char
	HBufC* iPostBuffer_hbuf;
	TPtr iPostBuffer;
	TUint16 _fake_postbuffer[4];
	
	// The notifier
	MMusicContentGetterNotifier *iNotifier;
	
	// Wait Dialog showing
	TBool iShowProcess;
	
	TBool iWaitComplete;
	TInt iIapId;
protected:
	void AllocMusicContentTextL(TInt aLength);
	void SetPostBuffer(const TDesC& aText);
public:
	/*
		From MHttpGetterObserver
	*/
	virtual void MhgoOnComplete(TInt aErr);
	virtual TInt MhgoGetSaveFile(TDes& aFileName);
	virtual TDes8& MhgoAllocDesc8(TInt aLength);
	virtual void MhgoAfterUserSelectIAP(TInt aErr, TInt aIap);

	/*
		From MProgressDialogCallback
	*/
	void OnDismissDialogL( TInt aButtonId, TBool& aCanExit );
	
public:
	TInt FetchAndSaveToFile(const TDesC& aUrl, const TDesC& aFileName, 
							MMusicContentGetterNotifier* aNotifier, 
							TBool aShowProcess=EFalse, TInt aProcessInfoId=0, TBool aHideCancel=EFalse,
							TBool aPostMethod=EFalse, const TDesC& aPostDesc=KNullDesC);
	TInt FetchAndSaveToDesc(const TDesC& aUrl, MMusicContentGetterNotifier* aNotifier, 
							TBool aShowProcess=EFalse, TInt aProcessInfoId=0, TBool aHideCancel=EFalse,
							TBool aPostMethod=EFalse, const TDesC& aPostDesc=KNullDesC);
	
	void CancelFetch();
	const TDes8& GetMusicContent() const;
	const TDesC& GetSavedFileName() const;

	void SetAcceptWml(TBool aAccept);
	void SetRefererText(const TDesC& aText);

	void GetLengthAndSize(TInt &aLength, TInt &aSize);
	void SetForcePieceRead(TBool aForce);
};


class MMusicContentGetterNotifier
{
public:
	virtual TInt McgnOnComplete(TInt aErr)=0;
	virtual void McgnAfterUserSelectIAP(TInt aErr, TInt aIap) = 0;
};


#endif		// _MUSICCONTENT_GETTER_H_