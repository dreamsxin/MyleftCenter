#ifndef _HttpGetter_H_
#define _HttpGetter_H_

#include "DownloadShell.h"

class MHttpGetterObserver;

class CHttpGetter : public CBase, public MDownloadShellNotifier
{
public: // Constructors and destructor
	/**
	* Destructor.
	*/
	~CHttpGetter();

	/**
	* Two-phased constructor.
	*/
	static CHttpGetter* NewL(MHttpGetterObserver* aObserver);

protected:
	/**
	* Constructor for performing 1st stage construction
	*/
	CHttpGetter(MHttpGetterObserver* aObserver);

	/**
	* EPOC default constructor for performing 2nd stage construction
	*/
	void ConstructL();

public:
	/*
	From MDownloadShellNotifier
	*/
	virtual void MdsDownloaderError( TInt aErr );
	virtual void MdsDownloaderMessage( TInt aResId );
	virtual void MdsDownloaderComplete( TInt aErr );
	virtual void MdsInstallNotifyComplete( TInt aErr );
	virtual TInt MdsGetProtected();
	virtual void MdsAfterUserSelectIAP(TInt aErr, TInt aIap);

protected:
	CenterCore* iCenterCore;
	CDownloadShell* iDownloadShell;

	MHttpGetterObserver* iObserver;
	TFileName iTempFileName;
	TUint	random;
	// 
public:
	TInt Open(const TDesC& aUrl, TInt aIapId);
	void Close();
	void Start(TBool aPostMethod=EFalse, const TDesC& aPostDesc=KNullDesC);
	const TDesC& GetFileName();
	void SetAcceptWml(TBool aAccept);
	void SetRefererText(const TDesC& aText);
	void GetLengthAndSize(TInt &aLength, TInt &aSize);
	void SetForcePieceRead(TBool aForce);
};


class MHttpGetterObserver
{
public:
	virtual void MhgoOnComplete(TInt aErr)=0;
	virtual TInt MhgoGetSaveFile(TDes& aFileName)=0;
	virtual TDes8& MhgoAllocDesc8(TInt aLength)=0;
	virtual void MhgoAfterUserSelectIAP(TInt aErr, TInt aIap)=0;
};

#endif