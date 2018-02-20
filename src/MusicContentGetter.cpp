#include "MusicContentGetter.h"
#include "symbian_def.h"
#include "CenterCore.h"
#include "funpanic.h"

CMusicContentGetter* CMusicContentGetter::NewL(TInt aIapId)
{
	CMusicContentGetter *self = CMusicContentGetter::NewLC(aIapId);
	CleanupStack::Pop();
	return self;	
}

CMusicContentGetter* CMusicContentGetter::NewLC(TInt aIapId)
{
	CMusicContentGetter *self = new (ELeave) CMusicContentGetter();
	CleanupStack::PushL( self );
	self->ConstructL(aIapId);
	return self;		
}

CMusicContentGetter::~CMusicContentGetter()
{
	delete iHttpGetter;
	if( iMusicContentText_hbuf )
		delete iMusicContentText_hbuf;
	if( iPostBuffer_hbuf )
		delete iPostBuffer_hbuf;
}
	
void CMusicContentGetter::ConstructL(TInt aIapId)
{
	iCenterCore = CenterCore::GetInstance();
	iHttpGetter = CHttpGetter::NewL( this );
	iSavedFileName.SetLength(0);	
	iIapId = aIapId;
}
CMusicContentGetter::CMusicContentGetter()
	: iMusicContentText(_fake_musiccontent, 2),
	  iPostBuffer(_fake_postbuffer, 2)
{	
}

void CMusicContentGetter::AllocMusicContentTextL(TInt aLength)
{
	if( iMusicContentText.MaxLength()<aLength )
	{
		if( iMusicContentText_hbuf )
		{
			delete iMusicContentText_hbuf;
			iMusicContentText_hbuf = NULL;
		}
		iMusicContentText_hbuf = HBufC8::NewL( aLength );
		iMusicContentText.Set( iMusicContentText_hbuf->Des() );
	}
}

void CMusicContentGetter::SetPostBuffer(const TDesC& aText)
{
	if( iPostBuffer.MaxLength()<aText.Length() )
	{
		if( iPostBuffer_hbuf )
		{
			delete iPostBuffer_hbuf;
			iPostBuffer_hbuf = NULL;
		}
		iPostBuffer_hbuf = HBufC::NewL( aText.Length()+1 );
		iPostBuffer.Set( iPostBuffer_hbuf->Des() );
	}

	iPostBuffer.Copy( aText );
}

void CMusicContentGetter::MhgoOnComplete(TInt aErr)
{
	iWaitComplete = EFalse;

	if( iShowProcess )
		iCenterCore->HideWaitNote();	

	if( iNotifier )	
	{
		int r =	iNotifier->McgnOnComplete(aErr);
		if( r )
			return;
	}
	
}
TInt CMusicContentGetter::MhgoGetSaveFile(TDes& aFileName)
{
	if( iSavedFileName.Length()>0 )
	{
		aFileName.Copy( iSavedFileName );
		return KErrNone;
	}
	
	return -1;
}
TDes8& CMusicContentGetter::MhgoAllocDesc8(TInt aLength)
{
	AllocMusicContentTextL(aLength);
	return iMusicContentText;		
}

void CMusicContentGetter::OnDismissDialogL( TInt aButtonId, TBool& aCanExit )
{
	CancelFetch();	
}
	
TInt CMusicContentGetter::FetchAndSaveToFile(const TDesC& aUrl, const TDesC& aFileName, 
											MMusicContentGetterNotifier* aNotifier, 
											TBool aShowProcess, TInt aProcessInfoId, TBool aHideCancel,
											TBool aPostMethod/*=EFalse*/, const TDesC& aPostDesc/*=KNullDesC*/)
{
	iSavedFileName.Copy( aFileName );
	iNotifier = aNotifier;
	
	
	TInt err = iHttpGetter->Open( aUrl, iIapId );
	FC_Log( "CMusicContentGetter::FetchAndSaveToFile:  err:%d",(int)err);
	if( KErrNone == err )
	{
		// Show wait dialog
		if( aShowProcess )
		{
			iShowProcess = aShowProcess;
			if( aHideCancel )
				iCenterCore->ShowWaitNote( iCenterCore->TextResource( aProcessInfoId ) );
			else
				iCenterCore->ShowWaitNoteWithCancel( iCenterCore->TextResource( aProcessInfoId ), this );

		}

		SetPostBuffer( aPostDesc );
		iHttpGetter->Start(aPostMethod, iPostBuffer);
		iWaitComplete = ETrue;
	}
	return err;
}

TInt CMusicContentGetter::FetchAndSaveToDesc(const TDesC& aUrl, 
											MMusicContentGetterNotifier* aNotifier, 
											TBool aShowProcess, TInt aProcessInfoId, TBool aHideCancel,
											TBool aPostMethod/*=EFalse*/, const TDesC& aPostDesc/*=KNullDesC*/)
{
	iSavedFileName.SetLength(0);
	iNotifier = aNotifier;
	
	TInt err = iHttpGetter->Open( aUrl, iIapId );
	if( KErrNone == err )
	{
		// Show wait dialog
		if( aShowProcess )
		{
			if( aHideCancel )
				iCenterCore->ShowWaitNote( iCenterCore->TextResource( aProcessInfoId ) );
			else
				iCenterCore->ShowWaitNoteWithCancel( iCenterCore->TextResource( aProcessInfoId ), this );
			iShowProcess = aShowProcess;

		}

		SetPostBuffer( aPostDesc );
		iHttpGetter->Start(aPostMethod, iPostBuffer);
		iWaitComplete = ETrue;
	}
	return err;
}
	
const TDes8& CMusicContentGetter::GetMusicContent() const 
{
	return iMusicContentText;	
}

const TDesC& CMusicContentGetter::GetSavedFileName() const
{
	return iSavedFileName;
}

void CMusicContentGetter::CancelFetch()
{
//	iFondoGlobal->iLogger->Log( _L("CMusicContentGetter::CancelFetch.\r\n") );
	if( iHttpGetter )
		iHttpGetter->Close(); 
}

void CMusicContentGetter::SetAcceptWml( TBool aAccept ) 
{
	if( iHttpGetter )
		iHttpGetter->SetAcceptWml( aAccept );
}
void CMusicContentGetter::SetRefererText( const TDesC& aText ) 
{
	if( iHttpGetter )
		iHttpGetter->SetRefererText( aText );
}
void CMusicContentGetter::MhgoAfterUserSelectIAP( TInt aErr, TInt aIap ) 
{
	if( iNotifier )
		iNotifier->McgnAfterUserSelectIAP( aErr, aIap );

	if( aErr==KErrNone )
		iIapId = aIap;
}

void CMusicContentGetter::GetLengthAndSize(TInt &aLength, TInt &aSize)
{
	if( iHttpGetter )
		iHttpGetter->GetLengthAndSize( (TInt &)aLength, (TInt &)aSize );
}

void CMusicContentGetter::SetForcePieceRead( TBool aForce )
{
	if( iHttpGetter )
		iHttpGetter->SetForcePieceRead( aForce );
}