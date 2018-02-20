/*
============================================================================
 Name        : DownloadShell.h
 Author      : Wo Junjun
 Version     :
 Copyright   : Funinhand Technology
 Description : CDownloadShell declaration
============================================================================
*/

#ifndef DOWNLOADSHELL_H
#define DOWNLOADSHELL_H

// INCLUDES
//#include "start.h"
#include "HttpShell.h"

#include <f32file.h>
#include <cdblen.h>
// CLASS DECLARATION
class MDownloadShellNotifier;
class CenterCore;
/**
*  CDownloadShell
* 
*/
class CDownloadShell : public CBase, public MHttpNotifier
{
public: // Constructors and destructor

	/**
        * Destructor.
        */
	~CDownloadShell();

        /**
        * Two-phased constructor.
        */
	static CDownloadShell* NewL(MDownloadShellNotifier *aNotifier);

        /**
        * Two-phased constructor.
        */
	static CDownloadShell* NewLC(MDownloadShellNotifier *aNotifier);

private:

	/**
        * Constructor for performing 1st stage construction
        */
	CDownloadShell(MDownloadShellNotifier *aNotifier);

	/**
        * EPOC default constructor for performing 2nd stage construction
        */
	void ConstructL();
public:
	enum TDownloadShellState
	{
		EDownloadShellState_Init,
		EDownloadShellState_Open_Ok,
		EDownloadShellState_Setup,
		EDownloadShellState_Setup_Ok,
		EDownloadShellState_Receiving,
		EDownloadShellState_Receive_Ok,
		EDownloadShellState_InstallNotify,
		EDownloadShellState_Error
	};
	enum { KMaxRecvBlockSize = 8*512 };
	enum { KFileBufferSize = 100*1024 };

	/*
	From MHttpNotifier
	*/
	virtual void MhnHttpAfterStart(TInt aErr);
	virtual void MhnHttpAfterClose(TInt aErr);
	virtual void MhnHttpAfterRead(TInt aErr, TInt aLength);
	virtual void MhnHttpOnRedirect(TInt aErr);
	virtual void MhnHttpNoContentRange(TInt aErr);
	virtual void MhnHttpLiveResetNotify(TInt aErr);
	virtual void MhnHttpAfterUserSelectIAP(TInt aErr, TInt aIap);

	TInt Open (const TDesC& aUrl, const TFileName& aFileName, TUint aLen, TUint aSize, TInt aIapId);
	void Start (TBool aPostMethod=EFalse, const TDesC& aPostDesc=KNullDesC);
	void Close (TBool aDeleteObject=ETrue);
	void GetLengthAndSize( TUint& aLen, TUint& aSize );
	void ChangeLength( TUint aLen );
	void NotNeedCheckDisk();

	void InstallNotify( const TDesC& aUrl );
	TBool iFileNameChanged;
	const TDesC& GetFinalTargetFile();
	void SetAcceptWml(TBool aAccept);
	void SetRefererText(const TDesC& aText);
	void SetForcePieceRead(TBool aForce);
protected:
	TInt ParseHttpString (const TDesC& aHttpUrl);
	TBool CheckDiskFull( const TDesC& aFileName, TInt aTotalSize );
	TInt FileWrite(TDes8& aDes,TInt aLength);

	TInt HttpShellStart(TInt _start_pos, TInt _size = 0, 
				TBool _post_method=EFalse, const TDesC& _post_desc=KNullDesC);
protected:
	CenterCore *iCenterCore;
	CHttpShell* iHttpShell;
	TDownloadShellState iState;
	MDownloadShellNotifier* iNotifier;
	TBool iCompleted;
	RFs iFs;
	RFile iFile;
	TBool iNeedCheckDisk;

	TFileName iFinalTargetFile;
	TFileName iTargetFile;
	TInt iLength, iSize;
	TInt iFlushLength;
	TBuf<128> iContentType; 

	TBuf<512> _network_url_string;
	TBuf<128> _server_host;
	TInt _server_port;
	TBuf<512> _uri_string;
	TBuf8<KFileBufferSize> _file_buffer;

	TBuf8<KMaxRecvBlockSize> _data_block;
	TSockXfrLength _length;

	TInt _connect_count;
	TBool _post_method;

	/* protection data */
	TBuf8<8> _prot_array;
	TInt _prot_ptr;
	void PrepareProtData(TInt aStartPtr);

	TBuf<KCommsDbSvrRealMaxFieldLength> iApnType;
	TInt iIapId;
};

class MDownloadShellNotifier
{
public:
	virtual void MdsDownloaderError( TInt aErr ) = 0;
	virtual void MdsDownloaderMessage( TInt aResId ) = 0;
	virtual void MdsDownloaderComplete( TInt aErr ) = 0;
	virtual void MdsInstallNotifyComplete( TInt aErr ) = 0;
	virtual TInt MdsGetProtected() = 0;
	virtual void MdsAfterUserSelectIAP(TInt aErr, TInt aIap) = 0;
};

#endif // DOWNLOADSHELL_H
