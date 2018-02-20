#include "HttpGetter.h"
#include "symbian_def.h"
#include "CenterCore.h"
#include "const.h"
#include "e32math.h"

CHttpGetter* CHttpGetter::NewL(MHttpGetterObserver* aObserver)
{
	CHttpGetter* self = new (ELeave)CHttpGetter(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();

	return self;
}
CHttpGetter::~CHttpGetter() 
{
	delete iDownloadShell;
}
CHttpGetter::CHttpGetter( MHttpGetterObserver* aObserver ) 
		: iObserver(aObserver)
{
	
}

void CHttpGetter::ConstructL() 
{

	iCenterCore= CenterCore::GetInstance();

	iDownloadShell = CDownloadShell::NewL (this);
	random=User::TickCount();
}

TInt CHttpGetter::Open( const TDesC& aUrl, TInt aIapId ) 
{
	TFileName tempFileName; 
	if(FC_IsDirectoryExist("E:\\"))
		iTempFileName.Copy( _L("E:\\") );
	else
		iTempFileName.Copy( _L("C:\\") );
	iTempFileName.Append ( KTempHttpGetFileName );
	iTempFileName.AppendNum ( random);
	tempFileName.Copy( iTempFileName );
	tempFileName.Append( KDownloadFileExt );

	//delete temp file
	 TInt _err = CenterCore::DeleteFile( iTempFileName );
	_err = CenterCore::DeleteFile( tempFileName );

	// open download
	_err = iDownloadShell->Open( aUrl, iTempFileName, 0, 1024*1024, aIapId );
	FC_Log( "CHttpGetter::Open:  _err:%d,random=%d",(int)_err,(int)random);
	return _err;
}

void CHttpGetter::Close() 
{
	if(iDownloadShell)
		iDownloadShell->Close(EFalse);	// do not disconnect gprs
}

void CHttpGetter::Start(TBool aPostMethod, const TDesC& aPostDesc) 
{
	// start download
	iDownloadShell->Start(aPostMethod, aPostDesc);
}
void CHttpGetter::MdsDownloaderError( TInt aErr ) 
{
	Close();

	//delete temp file
	CenterCore::DeleteFile( iTempFileName );

	iObserver->MhgoOnComplete( aErr );
}
void CHttpGetter::MdsDownloaderMessage( TInt aResId ) 
{

}
void CHttpGetter::MdsDownloaderComplete( TInt aErr ) 
{
	MdsInstallNotifyComplete( aErr );
}
void CHttpGetter::MdsInstallNotifyComplete( TInt aErr ) 
{

	// close download shell
	Close();
	if( iDownloadShell->iFileNameChanged )
		iTempFileName.Copy( iDownloadShell->GetFinalTargetFile() );


	if( aErr == KErrNone )
	{
		// 
		TFileName saveFile;
		TInt err = iObserver->MhgoGetSaveFile( saveFile );
		if( err == KErrNone )
		{// save to file
		
			CenterCore::CopyFile( iTempFileName, saveFile );
		
		}
		else
		{// copy to buffer
			RFs fs;
			RFile file;

			User::LeaveIfError( fs.Connect() );
			err = file.Open( fs, iTempFileName, EFileRead );
			if( err == KErrNone )
			{
				TInt fileSize;
				err = file.Size( fileSize );
				if( err == KErrNone )
				{
					TDes8 &fileText = iObserver->MhgoAllocDesc8( fileSize+1 );
					if( fileText.MaxLength()>=fileSize )
					{
						file.Read( fileText, fileSize );
					}
					else
					{
						file.Read( fileText, fileText.MaxLength() );
					}

					FC_Logsp("CHttpGetter:-------  Result----------  is\n%s",(const char*)fileText.PtrZ() );

				}

				file.Close();
			}

			fs.Close();
		}
		
	}

	CenterCore::DeleteFile( iTempFileName );

	iObserver->MhgoOnComplete( aErr );
}
TInt CHttpGetter::MdsGetProtected() 
{
	return 0;
}
void CHttpGetter::MdsAfterUserSelectIAP( TInt aErr, TInt aIap ) 
{
	if( iObserver )
		iObserver->MhgoAfterUserSelectIAP( aErr, aIap );
}

const TDesC& CHttpGetter::GetFileName() 
{
	return iTempFileName;
}
void CHttpGetter::SetAcceptWml(TBool aAccept)
{
		iDownloadShell->SetAcceptWml( aAccept );
}

void CHttpGetter::SetRefererText( const TDesC& aText ) 
{
	iDownloadShell->SetRefererText( aText );
}

void CHttpGetter::GetLengthAndSize(TInt &aLength, TInt &aSize)
{
	iDownloadShell->GetLengthAndSize( (TUint&)aLength, (TUint&)aSize );
}

void CHttpGetter::SetForcePieceRead( TBool aForce )
{
	iDownloadShell->SetForcePieceRead( aForce );
}
