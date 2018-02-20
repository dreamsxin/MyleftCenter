#include "HttpShell.h"
#include "loop_share.h"
#include "symbian_def.h"
#include "CenterCore.h"
#include "funpanic.h"
#include <MyleftCenter.rsg>

//#define HTTP_DATA_RECV_TIMEOUT		240000000
#define HTTP_DATA_RECV_TIMEOUT		30000000

CHttpShell::CHttpShell(MHttpNotifier* aHttpNotifier)
		: CActive(CActive::EPriorityStandard)/*, _data_ptr((TUint8*)_data_empty, 0)*/
{
	iNotifier = aHttpNotifier;
	iState = EHttpShellState_None;
}


CHttpShell::~CHttpShell()
{
	Cancel();
	delete iTcpShell;
	delete iTcpShell2;
	delete iHttpAidShell;

	delete _buffer;
	//delete []_data_buf8;
}

CHttpShell* CHttpShell::NewLC(MHttpNotifier* aHttpNotifier)
{
	CHttpShell* self = new (ELeave)CHttpShell(aHttpNotifier);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CHttpShell* CHttpShell::NewL(MHttpNotifier* aHttpNotifier)
{
	CHttpShell* self=CHttpShell::NewLC(aHttpNotifier);
	CleanupStack::Pop(); // self;
	return self;
}

void CHttpShell::ConstructL()
{
	iCenterCore = CenterCore::GetInstance();
	iTcpShell = CTcpShell::NewL (this);		
	iTcpShell2 = CTcpShell::NewL (this);		

	iHttpAidShell = new (ELeave) CHttpAidShell(this);
#ifdef _NOKIA_90_
	_buffer = new loop_share_buffer( EDownload_Large_Buffer_Size );
#else
	_buffer = new loop_share_buffer( EDownload_Buffer_Size );
#endif
	//_data_buf8 = new TUint8[EDownload_Buffer_Size];

	CActiveScheduler::Add( this );
}

TInt CHttpShell::Prepare(const TDesC& _connect_host, int _connect_port, 
			  const TDesC& _resource_address, const TDesC& _resource_host,
			  TInt _iap_id )
{
	_http_host.Copy (_connect_host);
	_http_port = _connect_port;

	_uri_address.Copy (_resource_address);
	_uri_host.Copy (_resource_host);

	iIapId = _iap_id;
#if defined _FOR_SINA_MOVIE || defined _FOR_SOHU_ || defined _FOR_KONG_TV_
	HBufC *fdata_hbuf = HBufC::NewL(1024);
	TPtr fdata = fdata_hbuf->Des();

	iCenterCore->GetFData( fdata );
	if( KErrNotFound != _uri_address.FindF( _L("?") ) )
		_uri_address.Append( _L("&fdata=") );
	else
		_uri_address.Append( _L("?fdata=") );
	_uri_address.Append( fdata );

	delete fdata_hbuf;
#endif

	iHttpAidShell->Prepare( _connect_host, _connect_port, _uri_address, _uri_host, _iap_id );

	iState = EHttpShellState_None;
	_is_http_tv_play = EFalse;

	if( !_accept_wml )
		_wml_refetch = 0;
	_find_gateway_response = 0;
	_recv_response_ok = EFalse;

	return 1;
}

#define HTTP_CONNECT_TIMEOUT		150000000
TInt CHttpShell::Start( TInt _start_pos, TInt _total_size, TInt _segment_length,
						TBool _post_req/* = EFalse*/, const TDesC& _post_desc/*=KNullDesC*/ )
{
	if( iState == EHttpShellState_None  )
	{
		iState = EHttpShellState_Init;

//	if(!iFondoGlobal->_http_iap_id )
//		 iFondoGlobal->_http_iap_id =FC_findIapID(1);
	

		iTcpShell->SetApnId( iIapId );
#if defined __WINS__
		iTcpShell->SetApnId( 0);	
#endif
		iTcpShell->SetServerName (_http_host);
		iTcpShell->SetServerPort (_http_port);
		iTcpShell->SetTimeOut (iCenterCore->_rtsp_data_timeout);

		
		iTcpShell2->SetApnId( iIapId );
#if defined __WINS__
		iTcpShell->SetApnId( 0);	
#endif
		iTcpShell2->SetServerName (_http_host);
		iTcpShell2->SetServerPort (_http_port);
		iTcpShell2->SetTimeOut (iCenterCore->_rtsp_data_timeout);

		_http_start_bytes = _start_pos;
		_total_bytes_recv = 0;
		_total_file_length = _total_size;
		
		_data_body_ptr = NULL;
		_data_body_valid_length = 0;

		_funinhand_download_session.SetLength(0);
		_post_request = _post_req;
		_post_ptrc.Set( _post_desc );

		iTcpShellDoing = 0;			// TcpShell1 doing
		iTcpShell->ConnectL();		// connect server
		iState = EHttpShellState_Connecting;

		_buffer->Clear();
		iSegmentLength = _segment_length;
		iNextWantLength = iSegmentLength;

		return 1;
	}
	return 0;
}

TInt CHttpShell::Close()
{
	Cancel();
	if(iTcpShell)
		iTcpShell->Cancel();
	if(iTcpShell2)
		iTcpShell2->Cancel();

	_data_body_ptr = NULL;
	_total_bytes_recv = 0;
	_install_notify = 0;
	//iTcpShell->Disconnect();
	//iState = EHttpShellState_Disconnecting;

	_buffer->Clear();

	return 1;
}

TInt CHttpShell::ReportException( TInt aFeedback )
{

	FC_Log("ReportException: %d, %d ",(int )aFeedback,(int)_buffer->can_read_size() );

	if( IsActive() )
	{
		Cancel();
	}

	_http_start_bytes -= (aFeedback+_buffer->can_read_size());
	_total_bytes_recv = iSegmentLength;
	_exception_flag = 1;
	_buffer->Clear();
	return _http_start_bytes;
}

TInt CHttpShell::Read( TDes8& _data_body )
{
	_data_body.SetLength(0);

	if( _data_body.MaxLength()>=EDownload_Buffer_Size )
	{
		if( IsActive() )
			Cancel();
		iTcpShell->Cancel();
		_buffer->Clear();
		_data_body_ptr = NULL;
		iNotifier->MhnHttpLiveResetNotify(KErrGeneral);
		return 0;
	}

	if( iState == EHttpShellState_RecvResponse_Ok )
	{	// start receive data
		if( _buffer->can_write_size() > _data_block.MaxLength() )
		{
			_data_block.SetLength(0);
			_read_len() = 0;
			if( _piece_read )
				iTcpShell->Read( _data_block, _read_len ,HTTP_DATA_RECV_TIMEOUT);
			else
				iTcpShell->ReadBlock( _data_block, HTTP_DATA_RECV_TIMEOUT );
			iState = EHttpShellState_RecvData;
		}
	}

		_data_body.SetMax();
		_data_body_ptr = &_data_body;

		if( !IsActive() )
		{
			TRequestStatus *_reqs = &iStatus;
			User::RequestComplete (_reqs, KErrNone);
			SetActive();
		}
	return 1;
}


//-----------------------------------------------------------------------
// Tcp notifier functions, to receive state from TcpShell
//
void CHttpShell::MtnTcpAfterConnect(TInt aErr)
{
	FC_Log("CHttpShell::MtnTcpAfterConnect1\r\n");
	switch ( iState )
	{
	case EHttpShellState_Connecting:
		if ( aErr != KErrNone )
		{  // connect error
			iState = EHttpShellState_Error;

			iNotifier->MhnHttpAfterStart( R_TEXT_HTTP_CONNECT_FAIL );

		}
		else
		{  // connect ok
			Http_SendRequest( iNextWantLength );
		}
		break;
	default:
		FC_Panic(EHttpAfterConnectBadStatus);
	}
	FC_Log( "CHttpShell::MtnTcpAfterConnect2\r\n");
}

void CHttpShell::MtnTcpAfterDisconnect(TInt aErr)
{
	
	switch ( iState )
	{
	case EHttpShellState_Disconnecting:
		if ( aErr != KErrNone )
		{
			iState = EHttpShellState_Error;
			
			iState = EHttpShellState_Disconnecting;
			MtnTcpAfterDisconnect( KErrNone );
		}
		else
		{	// disconnect ok
			if( _http_start_bytes < _total_file_length )
			{
				_total_bytes_recv = 0;

				if( !_no_content_range 
					&& iHttpAidShell->CanUse() 
					&& iHttpAidShell->GetHttpStartBytes()==_http_start_bytes )
				{

					if( iHttpAidShell->IsPrepareOk() )
					{

						iTcpShell2 = iTcpShell;
						iTcpShell = iHttpAidShell->ReturnTcpShell();
						iTcpShell->SetNotifier( this );
						iHttpAidShell->ReveiveParameters( _http_start_bytes, _total_file_length, _file_bytes, _range_from, _range_to );
						_range_from = _http_start_bytes;
						iHttpAidShell->ResetTcpShell();

						if( _buffer->can_write_size() > _data_block.MaxLength() )
						{
							_data_block.SetLength(0);
							_read_len() = 0;
							if( _piece_read )
								iTcpShell->Read( _data_block, _read_len,HTTP_DATA_RECV_TIMEOUT );
							else
								iTcpShell->ReadBlock( _data_block, HTTP_DATA_RECV_TIMEOUT );
							iState = EHttpShellState_RecvData;
						}
						else
						{
							iState = EHttpShellState_RecvData_WaitBuffer;
						}
					}
					else
					{
						// Wait Tcpshell2 to prepare ok.
						iState = EHttpShellState_WaitingTcpShell2;
						iHttpAidShell->SetNotifyFlag();	// notify me when prepare ok (not ok).
					}
				}
				else
				{
					if( iHttpAidShell->ReturnTcpShell() )
						iHttpAidShell->Close();
						
					_old_http_start_bytes = _http_start_bytes;

					iTcpShell->ConnectL();
					iState = EHttpShellState_Connecting;
				}
			}
			else
			{
				// All data received
				if( _data_body_ptr )
				{	

					iState = EHttpShellState_Disconnected;
					if( !IsActive() )
					{
						TRequestStatus *_reqs = &iStatus;
						User::RequestComplete (_reqs, KErrNone);
						SetActive();
					}
				}
				else
				{
					if( !_recv_response_ok )
					{
						// Not receive response ok. Reconnect
						if( iHttpAidShell->ReturnTcpShell() )
							iHttpAidShell->Close();

						iTcpShell->ConnectL();
						iState = EHttpShellState_Connecting;
					}
				}
			}
			
		}
		
		break;
	default:
		FC_Panic(EHttpDisconnectBadStatus);
	}
}

void CHttpShell::MtnTcpOnReceive(TInt aErr)
{
	_LIT8(_match_pattern, "\r\n\r\n");
	switch ( iState )
	{
	case EHttpShellState_RecvResponse:
		if ( aErr != KErrNone )
		{	//receive http response fail
			FC_Log("receive http response fail\r\n");
			iState = EHttpShellState_Error;

			iNotifier->MhnHttpAfterStart(R_TEXT_HTTP_RECV_RESPONSE_FAIL);

		}
		else
		{
			/* get one byte */
			_http_head8.Append (_one_byte);
			if (_http_head8.Length() >= 4 && 
					_http_head8.Right(4).Compare(_match_pattern) == 0) 
			{
				/* is reach \r\n\r\n */
				if (Http_Analyse_Head ())
				{
					
					// check we get continual data response
					if( _recv_response_ok && _old_http_start_bytes!=_http_start_bytes )
					{
						if( _is_http_tv_play )
						{
							if( IsActive() )
								Cancel();
							_buffer->Clear();
							_data_body_ptr = NULL;
							iNotifier->MhnHttpLiveResetNotify(KErrNone);
						}
					}
					
					iState = EHttpShellState_RecvResponse_Ok;
					_recv_response_ok = ETrue;
					if( _data_body_ptr )
					{	
						if( _buffer->can_write_size() > _data_block.MaxLength() )
						{
							_data_block.SetLength(0);
							_read_len() = 0;
							if( _piece_read )	
								iTcpShell->Read( _data_block, _read_len, HTTP_DATA_RECV_TIMEOUT );
							else
								iTcpShell->ReadBlock( _data_block, HTTP_DATA_RECV_TIMEOUT );
							iState = EHttpShellState_RecvData;
						}
					}
					else
						iNotifier->MhnHttpAfterStart( KErrNone );
				}
				return;
			}
			Http_RecvResponse ();	// continue to receive http response
			return;
		}
		break;
	case EHttpShellState_RecvData:
		if ( aErr != KErrNone )
		{	//receive http data fail
			FC_Log( "receive http data fail:%d,%d,%d,%d,%d ",(int)_data_block.Length(),(int)_data_block.MaxLength (),(int)_total_bytes_recv, (int)_http_start_bytes ,(int)_total_file_length );


			_buffer->Write( (const void*)_data_block.Ptr(), _data_block.Length() );
			_total_bytes_recv += _data_block.Length ();
			_http_start_bytes += _data_block.Length ();


			iNextWantLength = _range_to-_http_start_bytes+1;
			if( iNextWantLength<=0 )
				iNextWantLength = iSegmentLength;

			FC_Log( "next want length=%d",(int)iNextWantLength);

			iTcpShell->Disconnect();
			iState = EHttpShellState_Disconnecting;

			if( _no_content_length )
			{
				FC_Log( "No content length ok" );
				_total_file_length = _http_start_bytes;
			}

				
		}
		else
		{
			//receive data ok
			_buffer->Write( (const void*)_data_block.Ptr(), _data_block.Length() );
			
			_total_bytes_recv += _data_block.Length ();
			_http_start_bytes += _data_block.Length ();

			if( _total_bytes_recv*100/_file_bytes >= 50 
				&& !iHttpAidShell->ReturnTcpShell() 
				&& _total_file_length>50*1024 
				&& _range_from+_file_bytes<_total_file_length 
				&& !_post_request )
			{// start tcpshell2 to prepare next session

				FC_Log("start tcpshell2 to prepare next session.\r\n");
				
				iHttpAidShell->Start( iTcpShell2, _range_from+_file_bytes, _total_file_length, iSegmentLength );
			}

			if (_total_bytes_recv >= _file_bytes)
			{	// data of this session received ok, we disconnect this session, 
				// and issue next session
				iNextWantLength = iSegmentLength;
				iTcpShell->Disconnect();
				iState = EHttpShellState_Disconnecting;
			}
			else
			{
				if( _buffer->can_write_size() > _data_block.MaxLength() )
				{
					// read next block
					_data_block.SetLength(0);
					_read_len() = 0;
					if( _piece_read )
						iTcpShell->Read( _data_block, _read_len,HTTP_DATA_RECV_TIMEOUT );
					else
						iTcpShell->ReadBlock( _data_block, HTTP_DATA_RECV_TIMEOUT );
					iState = EHttpShellState_RecvData;
				}
				else
				{
					// not enough buffer for block
					iState = EHttpShellState_RecvData_WaitBuffer;
				}
			}			

			// check waiting for data
			if( _data_body_ptr )
			{
				if( !IsActive() )
				{
					TRequestStatus *_reqs = &iStatus;
					User::RequestComplete (_reqs, KErrNone);
					SetActive();
				}
			}
			
		}
		break;
	default:
		FC_Panic(EHttpOnReceiveBadStatus);
	}
}

void CHttpShell::MtnTcpAfterWrite(TInt aErr)
{
	switch ( iState )
	{
	case EHttpShellState_Sending:
		if( aErr!= KErrNone )
		{	//send request fail
			FC_Log("Send request fail\r\n");
			iState = EHttpShellState_Error;

			iNotifier->MhnHttpAfterStart( R_TEXT_HTTP_SEND_REQUEST_FAIL );

		}
		else
		{	//send request ok, start to receive response
			FC_Log("Send request ok\r\n");
			_http_head8.SetLength (0);
			Http_RecvResponse();
			FC_Log("Send request ok2\r\n");
		}
		break;
	default:
		FC_Panic(EHttpAfterWriteBadStatus);
	}
}

void CHttpShell::MtnTcpAfterCancel(TInt aErr)
{

}

void CHttpShell::MtnTcpOnTimeout(TInt aErr)
{
	switch ( iState )
	{
	case EHttpShellState_Connecting:
	case EHttpShellState_Sending:
	case EHttpShellState_Disconnecting:
		iState = EHttpShellState_Error;

		iNotifier->MhnHttpAfterStart( R_TEXT_HTTP_CONNECT_TIMEOUT );

		break;
	case EHttpShellState_RecvResponse:
		iState = EHttpShellState_Error;

		iNotifier->MhnHttpAfterStart( R_TEXT_HTTP_RECV_RESPONSE_TIMEOUT );

		break;
	default:
		{
			// disconnect and reconnect
			iNextWantLength = _range_to-_http_start_bytes+1;
			if( iNextWantLength<=0 )
				iNextWantLength = iSegmentLength;
			iTcpShell->Disconnect();
			iState = EHttpShellState_Disconnecting;

		}
		break;
	}
}

void CHttpShell::MtnTcpAfterUserSelectIAP(TInt aErr, TInt aIap)
{
	iIapId = aIap;
	if( iNotifier )
		iNotifier->MhnHttpAfterUserSelectIAP( aErr, aIap );

	if( iCenterCore->CheckHttpWapOrNet(iIapId) )
	{
		_http_host.Copy (iCenterCore->GetWapProxyAddress());
		_http_port = iCenterCore->GetWapProxyPort();

		iTcpShell->SetServerName( iCenterCore->GetWapProxyAddress() );
		iTcpShell->SetServerPort( iCenterCore->GetWapProxyPort() );
		iTcpShell->SetApnId( iIapId );

		iHttpAidShell->Prepare( _http_host, _http_port, _uri_address, _uri_host, iIapId );
	}

}


void CHttpShell::DoCancel()
{

}

void CHttpShell::RunL()
{
	//
	// copy data from buffer 
	//
	if( _data_body_ptr )
	{
		if( _buffer->can_read_size() >= _data_body_ptr->MaxLength() )
		{
			TInt size = _data_body_ptr->MaxLength();
			_buffer->Read( (void*)_data_body_ptr->Ptr(), size );
			_data_body_ptr = NULL;
			if( iState==EHttpShellState_RecvData_WaitBuffer )
			{
				if( _buffer->can_write_size() > _data_block.MaxLength() )
				{
					// continue read block
					_data_block.SetLength(0);
					_read_len() = 0;
					if( _piece_read )
						iTcpShell->Read( _data_block, _read_len,HTTP_DATA_RECV_TIMEOUT );
					else
						iTcpShell->ReadBlock( _data_block, HTTP_DATA_RECV_TIMEOUT );
					iState = EHttpShellState_RecvData;
				}
			}

			iNotifier->MhnHttpAfterRead( KErrNone, size );
		}
		else
		{
			if( _http_start_bytes>=_total_file_length )
			{
				// file end
				TInt size = _buffer->can_read_size();
				if( size>0 )
					_buffer->Read( (void*)_data_body_ptr->Ptr(), size );
				iNotifier->MhnHttpAfterRead( KErrNone, size );
			}
		}
	}
}


void CHttpShell::AidShellPreparedNotify( TInt aErr )
{
	if( iState == EHttpShellState_WaitingTcpShell2 )
	{
		if( !aErr )
		{// Prepared Ok
			FC_Log("Till Prepared Ok.\r\n");

			iTcpShell2 = iTcpShell;
			iTcpShell = iHttpAidShell->ReturnTcpShell();
			iTcpShell->SetNotifier( this );
			iHttpAidShell->ReveiveParameters( _http_start_bytes, _total_file_length, _file_bytes, _range_from, _range_to );
			_range_from = _http_start_bytes;
			iHttpAidShell->ResetTcpShell();

			if( _buffer->can_write_size() > _data_block.MaxLength() )
			{
				_data_block.SetLength(0);
				_read_len() = 0;
				if( _piece_read )
					iTcpShell->Read( _data_block, _read_len,HTTP_DATA_RECV_TIMEOUT );
				else
					iTcpShell->ReadBlock( _data_block, HTTP_DATA_RECV_TIMEOUT );
				iState = EHttpShellState_RecvData;
			}
			else
			{
				// not enough buffer for block
				FC_Log("data buffer is full3.\r\n");
				iState = EHttpShellState_RecvData_WaitBuffer;
			}
		}
		else
		{// not ok
			FC_Log("Till not prepared ok\r\n");
			if( iHttpAidShell->ReturnTcpShell() )
				iHttpAidShell->Close();

			_old_http_start_bytes = _http_start_bytes;
			
			iTcpShell->ConnectL();
			iState = EHttpShellState_Connecting;	
		}
	}
}

void CHttpShell::Http_SendRequest (TInt aWantLength) 
{
	FC_Logsp("CHttpShell::Http_SendRequest\r\n");

	_http_head8.SetLength (0);

	if( _post_request )
		_http_head8.Append (_L("POST http://"));
	else
		_http_head8.Append (_L("GET http://"));
	_http_head8.Append (_uri_host);
	_http_head8.Append (_uri_address);
	_http_head8.Append (_L(" HTTP/1.0\r\n"));

	_http_head8.Append (_L("Host: "));
	_http_head8.Append (_uri_host);
	_http_head8.Append (_L("\r\n"));

//	_http_head8.Append (_L("Accept: application/vnd.wap.wmlc, text/vnd.wap.wml, application/vnd.wap.wbxml, text/html, text/css,//video/fsx, video/fun, text/plain\r\n"));
	_http_head8.Append (_L("Accept: */*\r\n"));
	_http_head8.Append (_L("Accept-Charset: gb2312, iso-8859-1, us-ascii, utf-8\r\n"));
	_http_head8.Append (_L("Accept-Language: zh-ch, zh, en\r\n"));
	_http_head8.Append (_L("Connection: close\r\n"));
	if( !_post_request )
	{
		_http_head8.Append (_L("Range: bytes="));
		_http_head8.AppendNum (_http_start_bytes);
		_http_head8.Append (_L("-"));
		_http_head8.AppendNum (_http_start_bytes + aWantLength - 1);
		_http_head8.Append (_L("\r\n"));
	}
	else
	{
		_http_head8.Append (_L("Content-Length: "));
		_http_head8.AppendNum (_post_ptrc.Length());	
		_http_head8.Append (_L("\r\n"));
	}

	if( _funinhand_download_session.Length()>0 )
	{
		_http_head8.Append(_L("funinhand_download_session: "));
		_http_head8.Append(_funinhand_download_session);
		_http_head8.Append (_L("\r\n"));
	}	

	if( iRefererText.Length()>0 )
	{
		_http_head8.Append (_L("Referer: "));
		_http_head8.Append ( iRefererText );
		_http_head8.Append (_L("\r\n"));
	}

	_http_head8.Append (_L("User-Agent: "));

	_http_head8.Append ( iCenterCore->GetPhoneModel() );

	_http_head8.Append (_L("\r\n"));
	_http_head8.Append (_L("\r\n"));
	if( _post_request )
		_http_head8.Append( _post_ptrc );

	HBufC *_buf_hbuf = HBufC::NewL(4*1024);
	TPtr _buf = _buf_hbuf->Des();
	_buf.Copy (_http_head8);
	
	FC_Logsp((const char *)_http_head8.PtrZ());

	delete _buf_hbuf;
	
	iTcpShell->Write (_http_head8);
	iState = EHttpShellState_Sending;
}
void CHttpShell::Http_RecvResponse() 
{
	_one_byte.SetLength (0);
	iTcpShell->ReadBlock (_one_byte, HTTP_DATA_RECV_TIMEOUT*4);
	iState = EHttpShellState_RecvResponse;
}

int CHttpShell::Http_Analyse_Head ()
{
	const TUint8* _head = _http_head8.PtrZ ();
	int _file_length;
	const char* _ptr = (const char*)_head;
	while (*_ptr != ' ') 
		++_ptr;
	while (*_ptr == ' ')
		++_ptr;

	FC_Logsp("Http_Analyse_Head =%s", _ptr );


	int _status_code = atoi (_ptr);
	_http_response_code = _status_code;
	if (_status_code == 302) {

		char *_new_url = new char[512];

		char *_new_host = new char[128];
		char *_new_uri = new char[512];

		HBufC8 *_host_8_hbuf = HBufC8::NewL(128);
		HBufC8 *_uri_8_hbuf = HBufC8::NewL(512);
		TPtr8 _host_8 = _host_8_hbuf->Des();
		TPtr8 _uri_8 = _uri_8_hbuf->Des();

		HBufC *_host_buf_hbuf = HBufC::NewL(128);
		HBufC *_uri_buf_hbuf = HBufC::NewL(512);
		TPtr _host_buf = _host_buf_hbuf->Des();
		TPtr _uri_buf = _uri_buf_hbuf->Des();
		int i;
		char* _new_ptr = _new_url;

		//CFondoGlobal::ShowMessage( _L("Redirecting...") );
		iNotifier->MhnHttpOnRedirect( KErrNone );

		/* find Location */
		_ptr = strstr ((const char*)_head, "Location:");
		if (_ptr) {
			while (*_ptr != ' ')
				++_ptr;
			while (*_ptr == ' ')
				++_ptr;
			while (*_ptr != 0x0d && *_ptr != 0x0a && *_ptr) {
				*_new_ptr++ = *_ptr++;
			}
			*_new_ptr = 0;
		} else {
			iState = EHttpShellState_Error;

			iNotifier->MhnHttpAfterStart(R_TEXT_HTTP_NO_REDIRECT_LOCATION);

			delete _new_url;
			delete _new_host;
			delete _new_uri;
			delete _host_8_hbuf;
			delete _uri_8_hbuf;
			delete _host_buf_hbuf;
			delete _uri_buf_hbuf;
			return 0;
		}
		if (strncmp (_new_url, "http://", 7) == 0) {
			_new_ptr = &_new_url[7];
		} else 
			_new_ptr = _new_url;

		/* get new_host */
		i = 0;
		while (*_new_ptr && *_new_ptr != '/') {
			_new_host[i++] = *_new_ptr++;
		}
		if (!*_new_ptr) {
			iState = EHttpShellState_Error;

			iNotifier->MhnHttpAfterStart(R_TEXT_HTTP_NO_REDIRECT_LOCATION);

			delete _new_url;
			delete _new_host;
			delete _new_uri;
			delete _host_8_hbuf;
			delete _uri_8_hbuf;
			delete _host_buf_hbuf;
			delete _uri_buf_hbuf;
			return 0;
		}
		_new_host[i] = 0;
		_host_8.Copy ((const TUint8*)_new_host);

		FC_Utf8ToUnicodeL (_host_buf, _host_8);

		/* get new_uri */
		i = 0;
		while (*_new_ptr) {
			_new_uri[i++] = *_new_ptr++;
		}
		_new_uri[i] = 0;
		_uri_8.Copy ((const TUint8*)_new_uri);
	
		FC_Utf8ToUnicodeL (_uri_buf, _uri_8);
		//Prepare(_L("193.168.0.1"), 9228, _uri_buf, _host_buf);
		if( iCenterCore->CheckHttpWapOrNet(iIapId) )
			Prepare(iCenterCore->GetWapProxyAddress(), 
					iCenterCore->GetWapProxyPort(), _uri_buf, _host_buf, iIapId);
		else
		{
			char *_server_host = new char[128];
			HBufC *_server_buf_hbuf = HBufC::NewL(128);
			TPtr _server_buf = _server_buf_hbuf->Des();
			memset( _server_host, 0, 128 );
			char _server_port = 80;
			_ptr = strchr( _new_host, ':' );
			if( _ptr )
			{
				memcpy( _server_host, _new_host, _ptr-_new_host ); 
				_ptr ++;
				_server_port = atoi(_ptr);
			}
			else
			{
				strcpy( _server_host, _new_host );	
			}

			TPtrC8 _server_ptr8( (const TUint8*)_server_host, strlen(_server_host));
			FC_Utf8ToUnicodeL( _server_buf, _server_ptr8 );
			Prepare( _server_buf, _server_port,  _uri_buf, _host_buf, iIapId );

			delete _server_host;
			delete _server_buf_hbuf;
		}
			
		// start new connect
		Start( _http_start_bytes );
		delete _new_url;
		delete _new_host;
		delete _new_uri;
		delete _host_8_hbuf;
		delete _uri_8_hbuf;
		delete _host_buf_hbuf;
		delete _uri_buf_hbuf;
		return 0;
	} else if (_status_code == 416) {
		// 416 Requested Range Not Satisfiable 
		// 服务器不能满足客户在请求中指定的Range头。
		iState = EHttpShellState_Error;

		iNotifier->MhnHttpAfterStart( R_TEXT_HTTP_RESPONSE_416 );

		return 0;
	} else if (_status_code == 404 ) {
		// 404 Not found
		iState = EHttpShellState_Error;

		iNotifier->MhnHttpAfterStart( R_TEXT_HTTP_NOT_FOUND );

		return 0;
	} else if (_status_code != 200 && _status_code != 206) {
		iState = EHttpShellState_Error;

		iNotifier->MhnHttpAfterStart( R_TEXT_HTTP_RESPONSE_NOT_OK );

		return 0;
	}

	/* 
		check we have got a GateWay response, 
			if so we must reconnect server 
	*/
	_ptr = strstr( (const char*)_head, "x-sp-result: " );
	if( _ptr )
	{
		FC_Log( "Find Gateway response\r\n");

		if( _find_gateway_response < 10 )
		{// goto reconnect
			_find_gateway_response ++;
			iTcpShell->Disconnect();
			iState = EHttpShellState_Disconnecting;
		}
		else
		{
			_ptr += 13;
			_status_code = atoi(_ptr);
			if (_status_code == 404 ) {
				// 404 Not found
				iState = EHttpShellState_Error;
		
				iNotifier->MhnHttpAfterStart( R_TEXT_HTTP_NOT_FOUND );

			} else {
				iState = EHttpShellState_Error;

				iNotifier->MhnHttpAfterStart( R_TEXT_HTTP_RESPONSE_NOT_OK );

			}
		}

		return 0;
	}
	else
		_find_gateway_response = 0;


	/* content-length */
	_ptr = strstr((const char*)_head, "Content-length:");
	if (!_ptr) {
		_ptr = strstr((const char*)_head, "Content-Length:");
	}
	if (_ptr) {
		while (*_ptr != ' ')
			++_ptr;
		while (*_ptr == ' ')
			++_ptr;
		_file_length = atoi (_ptr);
		_no_content_length = EFalse;
	} else {

		// there is no Content-length keyword
		if( _total_file_length <= 0 )
			_file_length = 1024 * 1024;
		else
			_file_length = _total_file_length;
		_no_content_length = ETrue;
	}
	_file_bytes = _file_length;
	//_total_file_length = _file_length;
	if( _force_piece_read || _file_length < 16*1024 )
	{
		_piece_read = ETrue;
	}
	else
	{
		_piece_read = EFalse;
	}
	//
	// Check Cmcc tip page
	//
	if( _http_response_code==200 && _file_bytes<ECmcc_Http_TipPage_MaxSize && _wml_refetch==0 
		&& _install_notify==0 && strstr((const char*)_head, "text/vnd.wap.wml") )
	{
		if( !_accept_wml )
		{
			if( !_recv_response_ok )
				_wml_refetch = 1;
		}
		else
		{
			_wml_refetch = 1;
		}
		FC_Log("Find Cmcc tip page content.\r\n" );
		iTcpShell->Disconnect();
		iState = EHttpShellState_Disconnecting;
		return 0;
	}
	else
	{
		if( !_accept_wml )
			_wml_refetch = 0;
	}

	/* Content-range */
	_ptr = strstr ((const char*)_head, "Content-Range:");
	if (!_ptr) {
		_ptr = strstr ((const char*)_head, "Content-range:");
	}
	if (_ptr) {
		/* get start position */
		const char* _pos = strstr( _ptr, "bytes " );
		if( _pos ) {
			_pos += 6;
			_http_start_bytes = (TUint)atoi( _pos );
			_range_from = _http_start_bytes;

			/* get end position */
			_pos = strchr( _pos, '-' );
			if( _pos ) {
				_pos += 1;
				_range_to = (TUint)atoi( _pos );
			}
		}

		/* get totle length */
		while (*_ptr != '/' && *_ptr)
			++_ptr;
		if (*_ptr == '/') {
			++_ptr;
			_total_file_length = atoi (_ptr);
		}
		_no_content_range = EFalse;
	}
	else {
		// no content range
		_no_content_range = ETrue;
		iNotifier->MhnHttpNoContentRange (KErrNone);

		//if( _total_file_length<_file_length )
		if( _file_length>0 )
			_total_file_length = _file_length;
		// reset data buffer cache (2008-09-09)
		_buffer->Clear();
	}

	/* Content-type */
	_ptr = strstr((const char*)_head, "Content-type:");
	if (!_ptr) {
		_ptr = strstr((const char*)_head, "Content-Type:");
	}
	if (_ptr) {
		const char *_ptr2;
		while (*_ptr != ' ')
			++_ptr;
		while (*_ptr == ' ')
			++_ptr;
		_ptr2 = _ptr;
		while (*_ptr2 != '\r')
			++_ptr2;
		_content_type.Copy((const TUint8*)_ptr, _ptr2-_ptr);
	} else {
		_content_type.SetLength(0);
	}

	/* funinhand_download_session  */
	_ptr = strstr ((const char*)_head, "funinhand_download_session:");
	if( _ptr )
	{
		const char *_ptr2;
		while (*_ptr != ' ')
			++_ptr;
		while (*_ptr == ' ')
			++_ptr;
		_ptr2 = _ptr;
		while (*_ptr2 != '\r')
			++_ptr2;
		_funinhand_download_session.Copy((const TUint8*)_ptr, _ptr2-_ptr);
	}

	return 1;
}
TInt CHttpShell::GetTotalLength() 
{
	return _total_file_length;
}

const TDesC8& CHttpShell::GetContentType()
{
	return _content_type;
}

void CHttpShell::SetTVPlayFlag() 
{
	_is_http_tv_play = ETrue;
}

void CHttpShell::SetInstallNotify()
{
	_install_notify = 1;
}

void CHttpShell::SetAcceptWmlFlag(TBool aAccept)
{
	_accept_wml = aAccept;
}

TBool CHttpShell::CanAcceptWml() const
{
	return _accept_wml;
}

void CHttpShell::SetReferer( const TDesC& aText ) 
{
	iRefererText.Copy( aText );
	if( iHttpAidShell )
		iHttpAidShell->SetReferer( aText );
}

void CHttpShell::SetForcePieceRead(TBool aForce)
{
	_force_piece_read = aForce;
}
//================================================================================================
// CHttpAidShell class implementation
//

TInt CHttpAidShell::Prepare( const TDesC& _connect_host, int _connect_port,  
							const TDesC& _resource_address, const TDesC& _resource_host,
							TInt _iap_id ) 
{
	_http_host.Copy (_connect_host);
	_http_port = _connect_port;

	_uri_address.Copy (_resource_address);
	_uri_host.Copy (_resource_host);

	iIapId = _iap_id;

	iState = EHttpAidShellState_None;

	_wml_refetch = 0;

	return 1;
}

TInt CHttpAidShell::Start( CTcpShell* aTcpShell, TInt _start_pos, 
						  TInt _total_size /*= 0 */, TInt _segment_length /*=0*/ ) 
{
	if( iState == EHttpAidShellState_None  )
	{
		FC_Log("CHttpAidShell::Start\r\n");

		iTcpShell = aTcpShell;
		iTcpShell->SetNotifier( this );

		iTcpShell->SetApnId( iIapId );
		iTcpShell->SetServerName (_http_host);
		iTcpShell->SetServerPort (_http_port);
		iTcpShell->SetTimeOut (iCenterCore->_rtsp_data_timeout);

		iState = EHttpAidShellState_Init;
		iNotifyFlag = EFalse;
		iSegmentLength = _segment_length;

		_http_start_bytes = _start_pos;
		_total_file_length = _total_size;
		_range_from = 0;
		_range_to = 0;

		iTcpShell->ConnectL();		// connect server
		iState = EHttpAidShellState_Connecting;

		return 1;
	}

	return 0;
}

TInt CHttpAidShell::Close()
{
	if( iTcpShell )
	{
		iTcpShell->Cancel();
		iTcpShell = NULL;
	}
	iState = EHttpAidShellState_None;
	return 0;
}
void CHttpAidShell::MtnTcpAfterConnect( TInt aErr ) 
{
	switch ( iState )
	{
	case EHttpAidShellState_Connecting:
		if ( aErr != KErrNone )
		{  // connect error
			iState = EHttpAidShellState_Error;
			if( iNotifyFlag )
				iPrimaryShell->AidShellPreparedNotify( 1 );
		}
		else
		{  // connect ok
			Http_SendRequest();
		}
		break;
	default:
		FC_Panic(EHttpAfterConnectBadStatus);
	}
}
void CHttpAidShell::MtnTcpAfterDisconnect( TInt aErr ) 
{
	if( iNotifyFlag )
		iPrimaryShell->AidShellPreparedNotify( 1 );
}
void CHttpAidShell::MtnTcpOnReceive( TInt aErr ) 
{
	_LIT8(_match_pattern, "\r\n\r\n");

	switch ( iState )
	{
	case EHttpAidShellState_RecvResponse:
		if ( aErr != KErrNone )
		{	//receive http response fail
			FC_Log( "receive http response fail\r\n");
			iState = EHttpAidShellState_Error;
			if( iNotifyFlag )
				iPrimaryShell->AidShellPreparedNotify( 1 );
		}
		else
		{
			/* get one byte */
			_http_head8.Append (_one_byte);
			if (_http_head8.Length() >= 4 && 
				_http_head8.Right(4).Compare(_match_pattern) == 0) 
			{
				/* is reach \r\n\r\n */
				if (Http_Analyse_Head ())
				{
					iState = EHttpAidShellState_RecvResponse_Ok;
					if( iNotifyFlag )
						iPrimaryShell->AidShellPreparedNotify( 0 );
				}
				else
				{ // respond error
					iState = EHttpAidShellState_Error;
					if( iNotifyFlag )
						iPrimaryShell->AidShellPreparedNotify( 1 );
				}
				
				return;
			}
			Http_RecvResponse ();	// continue to receive http response
			return;
		}
		break;
	default:
		FC_Panic(EHttpOnReceiveBadStatus);
	}
}

void CHttpAidShell::MtnTcpAfterWrite( TInt aErr ) 
{
	switch ( iState )
	{
	case EHttpAidShellState_Sending:
		if( aErr!= KErrNone )
		{	//send request fail
			FC_Log("Send request fail\r\n");
			iState = EHttpAidShellState_Error;
			if( iNotifyFlag )
				iPrimaryShell->AidShellPreparedNotify( 1 );
		}
		else
		{	//send request ok, start to receive response
			_http_head8.SetLength (0);
			Http_RecvResponse();
		}
		break;
	default:
		FC_Panic(EHttpAfterWriteBadStatus);
	}
}

void CHttpAidShell::MtnTcpAfterCancel( TInt aErr ) 
{

}

void CHttpAidShell::MtnTcpOnTimeout( TInt aErr ) 
{
	iState = EHttpAidShellState_Error;
	if( iNotifyFlag )
		iPrimaryShell->AidShellPreparedNotify( 1 );
}

void CHttpAidShell::MtnTcpAfterUserSelectIAP(TInt aErr, TInt aIap)
{

}

void CHttpAidShell::Http_SendRequest() 
{
	_http_head8.SetLength (0);

	_http_head8.Append (_L("GET http://"));
	_http_head8.Append (_uri_host);
	_http_head8.Append (_uri_address);
	_http_head8.Append (_L(" HTTP/1.0\r\n"));

	_http_head8.Append (_L("Host: "));
	_http_head8.Append (_uri_host);
	_http_head8.Append (_L("\r\n"));

	//	_http_head8.Append (_L("Accept: application/vnd.wap.wmlc, text/vnd.wap.wml, application/vnd.wap.wbxml, text/html, text/css,//video/fsx, video/fun, text/plain\r\n"));
	_http_head8.Append (_L("Accept: */*\r\n"));
	_http_head8.Append (_L("Accept-Charset: gb2312, iso-8859-1, us-ascii, utf-8\r\n"));
	_http_head8.Append (_L("Accept-Language: zh-ch, zh, en\r\n"));
	_http_head8.Append (_L("Connection: close\r\n"));
	_http_head8.Append (_L("Range: bytes="));
	_http_head8.AppendNum (_http_start_bytes);
	_http_head8.Append (_L("-"));
	_http_head8.AppendNum (_http_start_bytes + iSegmentLength - 1);
	_http_head8.Append (_L("\r\n"));
	
	//_http_head8.Append ( _L("funinhand_client: S60_v5.0\r\n"));
	//_http_head8.Append (_L("User-Agent: Nokia6600/2.0\r\n"));
	if( iRefererText.Length()>0 )
	{
		_http_head8.Append (_L("Referer: "));
		_http_head8.Append ( iRefererText );
		_http_head8.Append (_L("\r\n"));
	}

	_http_head8.Append (_L("User-Agent: "));

	_http_head8.Append ( iCenterCore->GetPhoneModel() );

	_http_head8.Append (_L("\r\n"));
	_http_head8.Append (_L("\r\n"));

	HBufC *_buf_hbuf = HBufC::NewL(1024);
	TPtr _buf = _buf_hbuf->Des();
	_buf.Copy (_http_head8);

	FC_Logsp((const char *)_http_head8.PtrZ());
	
	delete _buf_hbuf;
	
	iTcpShell->Write (_http_head8);
	iState = EHttpAidShellState_Sending;
}
void CHttpAidShell::Http_RecvResponse() 
{
	_one_byte.SetLength (0);
	
	iTcpShell->ReadBlock (_one_byte, HTTP_DATA_RECV_TIMEOUT);
	iState = EHttpAidShellState_RecvResponse;
}
int CHttpAidShell::Http_Analyse_Head() 
{
	const TUint8* _head = _http_head8.PtrZ ();
	int _file_length;
	const char* _ptr = (const char*)_head;
	while (*_ptr != ' ') 
		++_ptr;
	while (*_ptr == ' ')
		++_ptr;

	//iFondoGlobal->iLogger->Log( _ptr );

	int _status_code = atoi (_ptr);
	_http_response_code = _status_code;
	if (_status_code == 302) {

		char *_new_url = new char[512];

		char *_new_host = new char[128];
		char *_new_uri = new char[512];

		HBufC8 *_host_8_hbuf = HBufC8::NewL(128);
		HBufC8 *_uri_8_hbuf = HBufC8::NewL(512);
		TPtr8 _host_8 = _host_8_hbuf->Des();
		TPtr8 _uri_8 = _uri_8_hbuf->Des();

		HBufC *_host_buf_hbuf = HBufC::NewL(128);
		HBufC *_uri_buf_hbuf = HBufC::NewL(512);
		TPtr _host_buf = _host_buf_hbuf->Des();
		TPtr _uri_buf = _uri_buf_hbuf->Des();
		int i;
		char* _new_ptr = _new_url;

		//CFondoGlobal::ShowMessage( _L("Redirecting...") );
		//iNotifier->MhnHttpOnRedirect( KErrNone );

		/* find Location */
		_ptr = strstr ((const char*)_head, "Location:");
		if (_ptr) {
			while (*_ptr != ' ')
				++_ptr;
			while (*_ptr == ' ')
				++_ptr;
			while (*_ptr != 0x0d && *_ptr != 0x0a && *_ptr) {
				*_new_ptr++ = *_ptr++;
			}
			*_new_ptr = 0;
		} else {
			iState = EHttpAidShellState_Error;
			//iNotifier->MhnHttpAfterStart(R_TEXT_HTTP_NO_REDIRECT_LOCATION);
			delete _new_url;
			delete _new_host;
			delete _new_uri;
			delete _host_8_hbuf;
			delete _uri_8_hbuf;
			delete _host_buf_hbuf;
			delete _uri_buf_hbuf;
			return 0;
		}
		if (strncmp (_new_url, "http://", 7) == 0) {
			_new_ptr = &_new_url[7];
		} else 
			_new_ptr = _new_url;

		/* get new_host */
		i = 0;
		while (*_new_ptr && *_new_ptr != '/') {
			_new_host[i++] = *_new_ptr++;
		}
		if (!*_new_ptr) {
			iState = EHttpAidShellState_Error;
			//iNotifier->MhnHttpAfterStart(R_TEXT_HTTP_NO_REDIRECT_LOCATION);
			delete _new_url;
			delete _new_host;
			delete _new_uri;
			delete _host_8_hbuf;
			delete _uri_8_hbuf;
			delete _host_buf_hbuf;
			delete _uri_buf_hbuf;
			return 0;
		}
		_new_host[i] = 0;
		_host_8.Copy ((const TUint8*)_new_host);

		FC_Utf8ToUnicodeL (_host_buf, _host_8);

		/* get new_uri */
		i = 0;
		while (*_new_ptr) {
			_new_uri[i++] = *_new_ptr++;
		}
		_new_uri[i] = 0;
		_uri_8.Copy ((const TUint8*)_new_uri);

		FC_Utf8ToUnicodeL (_uri_buf, _uri_8);

		//Prepare(_L("193.168.0.1"), 9228, _uri_buf, _host_buf);
		if( iCenterCore->CheckHttpWapOrNet(iIapId) )
			Prepare( iCenterCore->GetWapProxyAddress(), 
				iCenterCore->GetWapProxyPort(), _uri_buf, _host_buf, iIapId);
		else
		{
			char *_server_host = new char[128];
			HBufC *_server_buf_hbuf = HBufC::NewL(128);
			TPtr _server_buf = _server_buf_hbuf->Des();
			memset( _server_host, 0, 128 );
			char _server_port = 80;
			_ptr = strchr( _new_host, ':' );
			if( _ptr )
			{
				memcpy( _server_host, _new_host, _ptr-_new_host ); 
				_ptr ++;
				_server_port = atoi(_ptr);
			}
			else
			{
				strcpy( _server_host, _new_host );	
			}

			TPtrC8 _server_ptr8( (const TUint8*)_server_host, strlen(_server_host));
			FC_Utf8ToUnicodeL( _server_buf, _server_ptr8 );

			Prepare( _server_buf, _server_port,  _uri_buf, _host_buf, iIapId );

			delete _server_host;
			delete _server_buf_hbuf;
		}

		// start new connect
		Start( iTcpShell, _http_start_bytes );
		delete _new_url;
		delete _new_host;
		delete _new_uri;
		delete _host_8_hbuf;
		delete _uri_8_hbuf;
		delete _host_buf_hbuf;
		delete _uri_buf_hbuf;
		return 0;
	} else if (_status_code == 416) {
		// 416 Requested Range Not Satisfiable 
		// 服务器不能满足客户在请求中指定的Range头。
		iState = EHttpAidShellState_Error;
		//iNotifier->MhnHttpAfterStart( R_TEXT_HTTP_RESPONSE_416 );
		return 0;
	} else if (_status_code == 404 ) {
		// 404 Not found
		iState = EHttpAidShellState_Error;
		//iNotifier->MhnHttpAfterStart( R_TEXT_HTTP_NOT_FOUND );
		return 0;
	} else if (_status_code != 200 && _status_code != 206) {
		iState = EHttpAidShellState_Error;
		//iNotifier->MhnHttpAfterStart( R_TEXT_HTTP_RESPONSE_NOT_OK );
		return 0;
	}

	/* 
	check we have got a GateWay response, 
	if so we must reconnect server 
	*/
	_ptr = strstr( (const char*)_head, "x-sp-result:" );
	if( _ptr )
	{
		FC_Log("Find Gateway response\r\n");
		iTcpShell->Disconnect();
		iState = EHttpAidShellState_Disconnecting;
		return 0;
	}

	/* content-length */
	_ptr = strstr((const char*)_head, "Content-length:");
	if (!_ptr) {
		_ptr = strstr((const char*)_head, "Content-Length:");
	}
	if (_ptr) {
		while (*_ptr != ' ')
			++_ptr;
		while (*_ptr == ' ')
			++_ptr;
		_file_length = atoi (_ptr);
	} else {
		//CFondoGlobal::ShowMessage (_L("Warning: Lost Content-length"));

		// there is no Content-length keyword
		if( _total_file_length <= 0 )
			_file_length = 1024 * 1024;
		else
			_file_length = _total_file_length;
	}
	//SetFileBytes (_file_length);
	_file_bytes = _file_length;
	//_total_file_length = _file_length;

	//
	// Check Cmcc tip page
	//
	if( _http_response_code==200 && _file_bytes<ECmcc_Http_TipPage_MaxSize && _wml_refetch==0
		&& strstr((const char*)_head, "text/vnd.wap.wml") )
	{
		//_wml_refetch = 1;
		FC_Log( "Find Cmcc tip page content.\r\n");
		iTcpShell->Disconnect();
		iState = EHttpAidShellState_Disconnecting;
		return 0;
	}
	else
		_wml_refetch = 0;

	/* Content-range */
	_ptr = strstr ((const char*)_head, "Content-Range:");
	if (!_ptr) {
		_ptr = strstr ((const char*)_head, "Content-range:");
	}
	if (_ptr) {
		/* get start position */
		const char* _pos = strstr( _ptr, "bytes " );
		if( _pos ) {
			_pos += 6;
			_http_start_bytes = (TUint)atoi( _pos );
			_range_from = _http_start_bytes;

			/* get end position */
			_pos = strchr( _pos, '-' );
			if( _pos ) {
				_pos += 1;
				_range_to = (TUint)atoi( _pos );
			}
		}

		/* get totle length */
		while (*_ptr != '/' && *_ptr)
			++_ptr;
		if (*_ptr == '/') {
			++_ptr;
			_total_file_length = atoi (_ptr);
		}
	}
	else {
		// no content range
		//iNotifier->MhnHttpNoContentRange (KErrNone);
		
		if( _file_length>0 )
			_total_file_length = _file_length;
	}

	return 1;
}
TBool CHttpAidShell::IsConnecting()
{
	return iState==EHttpAidShellState_Connecting;
}

TBool CHttpAidShell::IsSendind()
{
	return iState==EHttpAidShellState_Sending;
}

TBool CHttpAidShell::IsRecvResponse()
{
	return iState==EHttpAidShellState_RecvResponse;
}

TBool CHttpAidShell::IsPrepareOk() 
{
	return iState==EHttpAidShellState_RecvResponse_Ok;
}

TBool CHttpAidShell::CanUse()
{
	return iState==EHttpAidShellState_Connecting || iState==EHttpAidShellState_Sending 
			|| iState==EHttpAidShellState_RecvResponse || iState==EHttpAidShellState_RecvResponse_Ok; 
}
CTcpShell* CHttpAidShell::ReturnTcpShell() 
{
	return iTcpShell;
}

void CHttpAidShell::ReveiveParameters( TUint &aHttpStartBytes, TInt &aTotalFileLength, TInt &aFileBytes,
										TUint &aRangeFrom, TUint& aRangeTo ) 
{
	aHttpStartBytes = _http_start_bytes;
	aTotalFileLength = _total_file_length;
	aFileBytes = _file_bytes;
	aRangeFrom = _range_from;
	aRangeTo = _range_to;
}
CHttpAidShell::~CHttpAidShell() 
{

}
CHttpAidShell::CHttpAidShell( CHttpShell* aPrimaryShell ) 
{
	iCenterCore = CenterCore::GetInstance();
	iPrimaryShell = aPrimaryShell;
}
TUint CHttpAidShell::GetHttpStartBytes() 
{
	return _http_start_bytes;
}

void CHttpAidShell::ResetTcpShell() 
{
	iTcpShell = NULL;
	iState = EHttpAidShellState_None;
	iNotifyFlag = EFalse;
}
void CHttpAidShell::SetNotifyFlag() 
{
	iNotifyFlag = ETrue;
}

void CHttpAidShell::SetReferer( const TDesC& aText ) 
{
	iRefererText.Copy( aText );
}

