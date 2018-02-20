/*
============================================================================
 Name        : DownloadShell.cpp
 Author      : Wo Junjun
 Version     :
 Copyright   : Funinhand Technology
 Description : CDownloadShell implementation
============================================================================
*/

#include "DownloadShell.h"
#include "symbian_def.h"
#include "CenterCore.h"
#include "funpanic.h"
#include "eikfutil.h"
#include <MyleftCenter.rsg>
#include "const.h"
#include "portab_symbian.h"

#define KDefaultServerHttpPort	80
//_LIT( KCmccWapAddress, "193.168.0.1" );
//#define KDefaultHttpPort 9228
//_LIT( KCmccWapAddress, "10.0.0.172" );
//#define KDefaultHttpPort 80

CDownloadShell::CDownloadShell(MDownloadShellNotifier *aNotifier)
{
	iState = EDownloadShellState_Init;
	iNotifier = aNotifier;
	iCompleted = EFalse;
}


CDownloadShell::~CDownloadShell()
{
	delete iHttpShell;
	iFs.Close();
}

CDownloadShell* CDownloadShell::NewLC(MDownloadShellNotifier *aNotifier)
{
	CDownloadShell* self = new (ELeave)CDownloadShell(aNotifier);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CDownloadShell* CDownloadShell::NewL(MDownloadShellNotifier *aNotifier)
{
	CDownloadShell* self=CDownloadShell::NewLC(aNotifier);
	CleanupStack::Pop(); // self;
	return self;
}

void CDownloadShell::ConstructL()
{
	iCenterCore = CenterCore::GetInstance();

	User::LeaveIfError( iFs.Connect() );
	iHttpShell = CHttpShell::NewL(this);

	_connect_count = 0;
	iNeedCheckDisk = ETrue;
}

//================================================================================
// Download Shell functions
//
//================================================================================
TInt CDownloadShell::Open( const TDesC& aUrl, const TFileName& aFileName, TUint aLen, TUint aSize, TInt aIapId ) 
{
	if( iState == EDownloadShellState_Init || iState == EDownloadShellState_Error)
	{
		iLength = aLen;
		iSize = aSize;
		iFlushLength = 0;
		iCompleted = EFalse;
		_connect_count = 0;
		iIapId = aIapId;

		if( !iHttpShell )
		{
			iHttpShell = CHttpShell::NewL(this);
		}

		if( !ParseHttpString( aUrl ) )
		{
			iState = EDownloadShellState_Error;
			return R_TEXT_PARSE_URL_ERROR;
		}
		
		// Prepare http shell
		HBufC *_resource_host_hbuf = HBufC::NewL(128);
		TPtr _resource_host = _resource_host_hbuf->Des();
		_resource_host.Append( _server_host );
		_resource_host.Append( _L(":") );
		_resource_host.AppendNum( _server_port );
//#ifndef _DEL_TEMPLETE_
		if( iCenterCore->CheckHttpWapOrNet(iIapId, &iApnType) )
			iHttpShell->Prepare( iCenterCore->GetWapProxyAddress(), iCenterCore->GetWapProxyPort(), _uri_string, _resource_host, iIapId );
		else
			iHttpShell->Prepare( _server_host, _server_port,  _uri_string, _resource_host, iIapId );
//#endif
		delete _resource_host_hbuf;

		// Open file
		TInt err, pos;
		iFinalTargetFile.Copy( aFileName );
		iTargetFile.Copy( aFileName );
//#ifndef _DEL_TEMPLETE_
		iTargetFile.Append( KDownloadFileExt );	// append ".FP!" to target file name

		if( CenterCore::IsFileExist( iTargetFile ) )
		{
			err = iFile.Open( iFs, iTargetFile, EFileWrite);
			if(err!= KErrNone)
			{
				iState = EDownloadShellState_Error;
				return R_TEXT_DOWNLOAD_OPENFILE_FAIL;
			}
			pos = 0;
			iFile.Seek ( ESeekEnd, pos );
		}
		else
		{
			err = iFile.Replace( iFs, iTargetFile, EFileWrite );
			if(err!= KErrNone)
			{
				iState = EDownloadShellState_Error;
				return R_TEXT_DOWNLOAD_CREATEFILE_FAIL;
			}
		}
//#endif
		// get length
		User::LeaveIfError( iFile.Size(iLength) );

		iState = EDownloadShellState_Open_Ok;
	}
	else
	{
		FC_Panic( EDownloadShellOpenBadStatus );
	}

	return KErrNone;
}

void CDownloadShell::Start(TBool aPostMethod, const TDesC& aPostDesc) 
{
	if( iState == EDownloadShellState_Open_Ok )
	{

		//iHttpShell->Start( iLength, iSize );
		HttpShellStart( iLength, iSize, aPostMethod, aPostDesc );
		_connect_count ++;
		iState = EDownloadShellState_Setup;
//#ifndef _DEL_TEMPLETE_
		iNotifier->MdsDownloaderMessage( R_TEXT_DOWNLOAD_CONNECTING );
//#endif
	}
	else
		FC_Panic( EDownloadShellStartBadStatus );
}

void CDownloadShell::Close(TBool aDeleteObject) 
{
	if(iHttpShell)
	{
		iHttpShell->Close();
		if( aDeleteObject )
		{
			delete iHttpShell;
			iHttpShell = NULL;
		}
	}

	TBool opened;
	if( KErrNone == iFs.IsFileOpen( iTargetFile, opened ) )
		if( opened )
		{
			if( _file_buffer.Length()>0 )
			{
				iFile.Write( _file_buffer, _file_buffer.Length() );
				_file_buffer.SetLength( 0 );	//
			}
			if( iFlushLength )
				iFile.Flush();
			iFile.Close();
		}

	//
	if( iState == EDownloadShellState_Receive_Ok )
	{ // rename target file to final file name
		int idx = 1;
		TBuf<16> fileExt;
		TFileName driveAndPathAndName; 

		TParse parser;
		parser.Set( iFinalTargetFile, NULL, NULL );
		driveAndPathAndName.Copy( parser.DriveAndPath() );
		driveAndPathAndName.Append( parser.Name() );
		fileExt.Copy( parser.Ext() );


		if( iNotifier->MdsGetProtected() )
		{
			iFinalTargetFile.Append( KFptExt );
			iFileNameChanged = ETrue;
		}

		while (1)
		{
			if( KErrNone != EikFileUtils::RenameFile( iTargetFile, iFinalTargetFile, 0 ))
			{
				iFinalTargetFile.Copy( driveAndPathAndName );
				iFinalTargetFile.Append( _L("(") );
				iFinalTargetFile.AppendNum( idx++ );
				iFinalTargetFile.Append( _L(")") );
				iFinalTargetFile.Append( fileExt );
				if( iNotifier->MdsGetProtected() )
					iFinalTargetFile.Append( KFptExt );
				iFileNameChanged = ETrue;
				
				FC_Log("Change file name: ");
				
				if( idx>16 )
					break;
			}
			else
			{//rename ok
				break;	
			}
		}
			
	}
	else
	{
		CenterCore::DeleteFile(iTargetFile);
	}

	iState = EDownloadShellState_Init;
}

void CDownloadShell::GetLengthAndSize( TUint& aLen, TUint& aSize ) 
{
	aLen = iLength;
	aSize = iSize;
}

void CDownloadShell::ChangeLength( TUint aLen )
{// call this function before calling Start function to change the start position of download
	iLength = aLen;
}

//================================================================================
// Http notifier functions
//
//================================================================================
void CDownloadShell::MhnHttpAfterStart( TInt aErr ) 
{
	if( iState == EDownloadShellState_Setup )
	{
		if( aErr != KErrNone )
		{	// setup fail

			if( aErr == R_TEXT_HTTP_RECV_RESPONSE_FAIL 
				|| aErr == R_TEXT_HTTP_RESPONSE_NOT_OK )
			{	// no response from server, may be socket closed
				// we delete old socket, and create a new socket
				TInt _http_start = iHttpShell->ReportException( 0 );
				TBool acceptWml = iHttpShell->CanAcceptWml();

				if(iHttpShell)
					iHttpShell->Close();
				delete iHttpShell;
				iHttpShell = NULL;

				iHttpShell = CHttpShell::NewL( this );
				iHttpShell->SetAcceptWmlFlag( acceptWml );

				HBufC *_resource_host_hbuf = HBufC::NewL(128);
				TPtr _resource_host = _resource_host_hbuf->Des();
				_resource_host.Append( _server_host );
				_resource_host.Append( _L(":") );
				_resource_host.AppendNum( _server_port );
				if( iCenterCore->CheckHttpWapOrNet(iIapId, &iApnType) )
					iHttpShell->Prepare( iCenterCore->GetWapProxyAddress(), iCenterCore->GetWapProxyPort(), _uri_string, _resource_host, iIapId );
				else
					iHttpShell->Prepare( _server_host, _server_port, _uri_string, _resource_host, iIapId );
				//iHttpShell->Start( iLength, iSize );
				HttpShellStart( iLength, iSize );
				iNotifier->MdsDownloaderMessage(R_TEXT_HTTP_RECONNECT_SERVER);
				_connect_count ++;
				iState = EDownloadShellState_Setup;
				delete _resource_host_hbuf;
				return;
			}
			else
			if( aErr == R_TEXT_HTTP_CONNECT_TIMEOUT )
			{
				if( _connect_count==1 )
				{
					TInt _http_start = iHttpShell->ReportException( 0 );
					TBool acceptWml = iHttpShell->CanAcceptWml();

					if(iHttpShell)
						iHttpShell->Close();
					delete iHttpShell;
					iHttpShell = NULL;

					iHttpShell = CHttpShell::NewL( this );
					iHttpShell->SetAcceptWmlFlag( acceptWml );

					HBufC *_resource_host_hbuf = HBufC::NewL(128);
					TPtr _resource_host = _resource_host_hbuf->Des();
					_resource_host.Append( _server_host );
					_resource_host.Append( _L(":") );
					_resource_host.AppendNum( _server_port );
					iHttpShell->Prepare( _server_host, _server_port, _uri_string, _resource_host, iIapId );
					//iHttpShell->Start( iLength, iSize );
					HttpShellStart( iLength, iSize );
					_connect_count ++;
					iState = EDownloadShellState_Setup;
					iNotifier->MdsDownloaderMessage( R_TEXT_DOWNLOAD_CONNECTING );
					delete _resource_host_hbuf;
				}
				else
				{
					iState = EDownloadShellState_Error;
					iNotifier->MdsDownloaderError( aErr );
				}
			}
			else
			{
				iState = EDownloadShellState_Error;
				iNotifier->MdsDownloaderError( aErr );
			}
			
		}
		else
		{	// setup http server ok
			iState = EDownloadShellState_Setup_Ok;

			// get total file size
			iSize = iHttpShell->GetTotalLength();
			if( iLength >= iSize )
			{  // receive completed
				iCompleted = ETrue;
				iState = EDownloadShellState_Receive_Ok;
				iNotifier->MdsDownloaderComplete( 0 );
				return;
			}

			// check disk full
			if( iNeedCheckDisk && CheckDiskFull(iTargetFile, iSize) )
			{
				iState = EDownloadShellState_Error;
				iNotifier->MdsDownloaderError( R_TEXT_DOWNLOAD_DISK_FULL );
				return;
			}

			iFlushLength = iLength + CHttpShell::EDOWNLOAD_SEGMENT_LENGTH;

			// get content type
			iContentType.SetLength(0);
			HBufC8 *_content_type_hbuf = HBufC8::NewL(128);
			TPtr8 _content_type = _content_type_hbuf->Des();
			_content_type.Copy( iHttpShell->GetContentType() );
			if( _content_type.Length()>0 )
			{
				int i, p = _content_type.Length()-1;
				for(i=p; i>=0; i--)
				{
					if( _content_type[i]=='/' || _content_type[i]=='.' )
					{
						if( p-i>0 )
						{
							FC_AsciiToUnicodeL( iContentType, _content_type.Right(_content_type.Length()-i-1) );

							FC_Log("Content Type: ");
							//iFondoGlobal->iLogger->Log( iContentType );
						}
					}
				}
			}
			delete _content_type_hbuf;

			// prepare protection data
			PrepareProtData( iLength );

			// start receive
			_data_block.SetLength(0);
			iHttpShell->Read( _data_block );
			iState = EDownloadShellState_Receiving;
			iNotifier->MdsDownloaderMessage( R_TEXT_DOWNLOAD_RECEIVING );
		}
	}
	else if( iState == EDownloadShellState_InstallNotify )
	{
		if( aErr != KErrNone )
		{  // fail
			iState = EDownloadShellState_Error;
			iNotifier->MdsInstallNotifyComplete( aErr );
		}
		else
		{  // install notify ok
			iState = EDownloadShellState_Receive_Ok;
			iNotifier->MdsInstallNotifyComplete( 0 );
		}
	}
	else
	{
		if( aErr != KErrNone )
		{
			iState = EDownloadShellState_Error;
			iNotifier->MdsDownloaderError( aErr );
		}
		else
		{
			FC_Log( "EDownloadShellAfterStartBadStatus: %d", (int)iState);
			FC_Panic( EDownloadShellAfterStartBadStatus );
		}
	}
}

void CDownloadShell::MhnHttpAfterClose( TInt aErr ) 
{

}

void CDownloadShell::MhnHttpAfterRead( TInt aErr, TInt aLength ) 
{
	if( iState == EDownloadShellState_Receiving )
	{
		if( aErr != KErrNone )
		{	// setup fail
			iState = EDownloadShellState_Error;
			iNotifier->MdsDownloaderError( aErr );
		}
		else
		{
			TInt thisLen = aLength;
			iLength += thisLen;
		//	FC_Log("AfterRead: %d,%d,%d",(int)thisLen,(int)iLength,(int)iSize);
			if( thisLen == 0 )
			{
				if( iLength >= iSize )
				{  // receive ok
					iCompleted = ETrue;
					iState = EDownloadShellState_Receive_Ok;
					iNotifier->MdsDownloaderComplete(0);
				}
				else
				{	// error
					iState = EDownloadShellState_Error;
					iNotifier->MdsDownloaderError( R_TEXT_DOWNLOAD_DATA_ERROR );
				}
			}
			else
			{
				// write data to file
				TInt err = FileWrite( _data_block, thisLen );
				if( err != KErrNone )
				{	// write fail
					iState = EDownloadShellState_Error;
					iNotifier->MdsDownloaderError( R_TEXT_DOWNLOAD_WRITE_FAIL );
					return;
				}
				if( iLength >= iFlushLength )
				{
					if( KErrNone != iFile.Flush() )
					{
						iState = EDownloadShellState_Error;
						iNotifier->MdsDownloaderError( R_TEXT_DOWNLOAD_WRITE_FAIL );
						return;
					}
					iFlushLength += CHttpShell::EDOWNLOAD_SEGMENT_LENGTH;
				}
				
				if( iLength >= iSize )
				{ //download ok
						
					iCompleted = ETrue;
					iState = EDownloadShellState_Receive_Ok;
					iNotifier->MdsDownloaderComplete(0);
					return;
				}

				//receive next block
				_data_block.SetLength(0);
				iHttpShell->Read( _data_block );

				if (!iCompleted)
					iNotifier->MdsDownloaderMessage( 0 );
			}
		}	
	}
	else
		FC_Panic( EDownloadShellRecvBadStatus );
}

void CDownloadShell::MhnHttpOnRedirect( TInt aErr ) 
{
	iNotifier->MdsDownloaderMessage (R_TEXT_HTTP_REDIRECTING);
}

void CDownloadShell::MhnHttpNoContentRange(TInt aErr)
{
	if( iState == EDownloadShellState_InstallNotify )
		return;
	iNotifier->MdsDownloaderMessage (R_TEXT_DOWNLOAD_NO_CONTENT_LENGTH);
	//
	// we reopen this file, and set write ptr to start of file

	TBool opened;
	if( KErrNone == iFs.IsFileOpen( iTargetFile, opened ) )
		if( opened )
		{
			if( iFlushLength )
				iFile.Flush();
			iFile.Close();
		}
	TInt err = iFile.Replace( iFs, iTargetFile, EFileWrite );
	if(err!= KErrNone)
	{
		iState = EDownloadShellState_Error;
		iNotifier->MdsDownloaderError( R_TEXT_DOWNLOAD_CREATEFILE_FAIL );
	}
	iLength = 0;	// save data from start

	// prepare protection data
	PrepareProtData(0);
}

void CDownloadShell::MhnHttpLiveResetNotify(TInt aErr)
{

}

void CDownloadShell::MhnHttpAfterUserSelectIAP(TInt aErr, TInt aIap)
{
	if( aErr == KErrNone )
		iIapId = aIap;
	if( iNotifier )
		iNotifier->MdsAfterUserSelectIAP( aErr, aIap );
}

//==============================================================================
// Helper functions
//==============================================================================
TInt CDownloadShell::ParseHttpString (const TDesC& aHttpUrl)
{
	_network_url_string.Copy(aHttpUrl);

	if (_network_url_string.Length() <= 7) 
	{
		/* url too short */
		return 0;
	}

	HBufC8 *_url_asc_string_hbuf = HBufC8::NewL(512);
	TPtr8 _url_asc_string = _url_asc_string_hbuf->Des();
	FC_UnicodeToAsciiL (_url_asc_string, _network_url_string);
	const char * _url_ptr = (const char*) _url_asc_string.Ptr();
	TInt _url_length = _url_asc_string.Length ();
	if (strncmp (_url_ptr, "http://", 7) != 0) 
	{
		/* start not with rtsp:// */
		delete _url_asc_string_hbuf;
		return 0;
	}

	char *_server_name_char_str = new char[256];
	_server_name_char_str[0] = 0;

	int i;
	TInt _server_rtsp_port = KDefaultServerHttpPort;
	for (i = 7; i < _url_length; i++) 
	{
		if (_url_ptr[i] == '/' || _url_ptr[i] == ':') 
		{
			strncpy (_server_name_char_str, 
				&_url_ptr[7], i - 7);
			_server_name_char_str[i - 7] = 0;
			if (_url_ptr[i] == ':') {
				_server_rtsp_port = atoi (&_url_ptr[i+1]);
			}
			break;
		}
	}
	if( i == _url_length )
	{
		strncpy (_server_name_char_str, &_url_ptr[7], i - 7);
		_server_name_char_str[i - 7] = 0;
	}
	if (!_server_name_char_str[0]) 
	{
		/* parse rtsp url string error */
		delete _url_asc_string_hbuf;
		delete []_server_name_char_str;
		return 0;
	}

	TPtrC8 _server_name_bufc8( (const TUint8*)_server_name_char_str, strlen(_server_name_char_str) );
	HBufC *_server_name_bufc16_hbuf = HBufC::NewL(_server_name_bufc8.Length());
	TPtr _server_name_bufc16 = _server_name_bufc16_hbuf->Des();
	FC_AsciiToUnicodeL (_server_name_bufc16, _server_name_bufc8);
	_server_host.Copy (_server_name_bufc16);
	_server_port = _server_rtsp_port;

	const char* _ptr = _url_ptr + 7;
	while (*_ptr != '/' && (_ptr-_url_ptr)<_url_length)
		_ptr++;

	if( _url_length - (_ptr-_url_ptr) > 0 )
	{
	TPtrC8 _uri_string_bufc8((const TUint8*)_ptr, _url_length - (_ptr-_url_ptr));
	FC_AsciiToUnicodeL (_uri_string, _uri_string_bufc8);
	}
	else
		_uri_string.SetLength( 0 );
	
	/* success */
	delete _url_asc_string_hbuf;
	delete []_server_name_char_str;
	delete _server_name_bufc16_hbuf;
	return 1;
}

void CDownloadShell::InstallNotify( const TDesC& aUrl )
{
	if( iState == EDownloadShellState_Receive_Ok )
	{
		iHttpShell->Close();

		iFlushLength = 0;

		if( !ParseHttpString( aUrl ) )
		{
			iNotifier->MdsInstallNotifyComplete( R_TEXT_PARSE_URL_ERROR );
			return;
		}

		// Prepare http shell
		HBufC *_resource_host_hbuf = HBufC::NewL(128);
		TPtr _resource_host = _resource_host_hbuf->Des();
		_resource_host.Append( _server_host );
		_resource_host.Append( _L(":") );
		_resource_host.AppendNum( _server_port );
		
		if( iCenterCore->CheckHttpWapOrNet(iIapId, &iApnType) )
			iHttpShell->Prepare( iCenterCore->GetWapProxyAddress(), iCenterCore->GetWapProxyPort(), _uri_string, _resource_host, iIapId );
		else
			iHttpShell->Prepare( _server_host, _server_port, _uri_string, _resource_host, iIapId );
		
		iState = EDownloadShellState_InstallNotify;
		delete _resource_host_hbuf;

		iHttpShell->SetInstallNotify();
		iHttpShell->Start( 0 );
	}
}

TBool CDownloadShell::CheckDiskFull( const TDesC& aFileName, TInt aTotalSize )
{
	if( aFileName.Length()<=0 )
		return EFalse;

	RFs fs;
	User::LeaveIfError( fs.Connect() );
	
	TBool ret;
	TEntry entry;
	TInt64 diskFreeSize, diskTotalSize;
	TInt driveNumber, fileSize = 0;
	fs.CharToDrive( aFileName[0], driveNumber );
	
	// fetch disk size
	iCenterCore->m_appUI->DiskInfoL( diskTotalSize, diskFreeSize, (TDriveNumber)driveNumber );
	
	// fetch current file size
	if( KErrNone == fs.Entry( aFileName, entry ) )
		fileSize = entry.iSize;
	fs.Close();

	return diskFreeSize <= TInt64(aTotalSize-entry.iSize);
}

const TDesC& CDownloadShell::GetFinalTargetFile()
{
	return iFinalTargetFile;
}

TInt CDownloadShell::FileWrite(TDes8& aDes,TInt aLength)
{
	if( iNotifier->MdsGetProtected() )
	{
		// get file position
		_prot_ptr = 0;
		iFile.Seek( ESeekCurrent, _prot_ptr );

		// protect data before write to file
		TInt i;
		for( i=0; i<aLength; i++ )
		{
			aDes[i] ^= _prot_array[_prot_ptr%8];
			_prot_ptr++;
		}
	}

	if( _file_buffer.Length()+aLength>KFileBufferSize )
	{
		TInt err = iFile.Write( _file_buffer, _file_buffer.Length() );
		_file_buffer.SetLength(0);
		_file_buffer.Append( aDes.Ptr(), aLength );
		return err;
	}
	else
		_file_buffer.Append( aDes.Ptr(), aLength );
	return KErrNone;
	
		
	//return iFile.Write( aDes, aLength );
}
void CDownloadShell::PrepareProtData(TInt aStartPtr) 
{
	if( !iNotifier->MdsGetProtected() )
		return; // data not protected
	TInt64 fid;
	TUint _low = I64LOW(fid);
	TUint _high = I64HIGH(fid);
	
	_prot_array.SetMax();
	_prot_array[0] = (_high >> 24) & 0xFF;
	_prot_array[1] = (_high >> 16) & 0xFF;
	_prot_array[2] = (_high >> 8) & 0xFF;
	_prot_array[3] = (_high) & 0xFF;
	_prot_array[4] = (_low >> 24) & 0xFF;
	_prot_array[5] = (_low >> 16) & 0xFF;
	_prot_array[6] = (_low >> 8) & 0xFF;
	_prot_array[7] = (_low) & 0xFF;

	_prot_ptr = aStartPtr;

//	iFondoGlobal->iLogger->Log( _L("PrepareProtData Ok\r\n") );
}

void CDownloadShell::NotNeedCheckDisk() 
{
	iNeedCheckDisk = EFalse;
}

#define KLAN_DOWNLAOD_SEGMENT	1024*1024
TInt CDownloadShell::HttpShellStart( TInt _start_pos, TInt _size /*= 0*/,
								TBool _post_method/*=EFalse*/, const TDesC& _post_desc/*=KNullDesC*/ ) 
{
	TInt r;
	if( KErrNotFound!=iApnType.FindF( _L("LAN") ) )
		r = iHttpShell->Start( _start_pos, _size, KLAN_DOWNLAOD_SEGMENT, _post_method, _post_desc );
	else
		r = iHttpShell->Start( _start_pos, _size, CHttpShell::EDOWNLOAD_SEGMENT_LENGTH, _post_method, _post_desc );
	return r;
}
void CDownloadShell::SetAcceptWml( TBool aAccept ) 
{
	if( iHttpShell )
		iHttpShell->SetAcceptWmlFlag( aAccept );
}
void CDownloadShell::SetRefererText( const TDesC& aText ) 
{
	if( iHttpShell )
		iHttpShell->SetReferer( aText );
}

void CDownloadShell::SetForcePieceRead( TBool aForce )
{
	if( iHttpShell )
		iHttpShell->SetForcePieceRead( aForce );
}
