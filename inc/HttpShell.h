#ifndef _HTTP_SHELL_H_
#define _HTTP_SHELL_H_

//#include "start.h"
#include "TcpShell.h"

class MHttpNotifier;
class CHttpAidShell;
class loop_share_buffer;
class CenterCore;
//#define _READ_ONEORMORE_


class CHttpShell : public CActive, public MTcpNotifier
{
public: // Constructors and destructor

	~CHttpShell();
	static CHttpShell* NewL(MHttpNotifier* aHttpNotifier);
	static CHttpShell* NewLC(MHttpNotifier* aHttpNotifier);

private:

	CHttpShell(MHttpNotifier* aSrcObsvr);
	void ConstructL();

protected:
	/*
	From MTcpNotifier
	*/
	virtual void MtnTcpAfterConnect(TInt aErr);
	virtual void MtnTcpAfterDisconnect(TInt aErr);
	virtual void MtnTcpOnReceive(TInt aErr);
	virtual void MtnTcpAfterWrite(TInt aErr);
	virtual void MtnTcpAfterCancel(TInt aErr);
	virtual void MtnTcpOnTimeout(TInt aErr);
	virtual void MtnTcpAfterUserSelectIAP(TInt aErr, TInt aIap);

	//
	// From CActive
	virtual void DoCancel();
	virtual void RunL();

public:
	enum THttpShellState
	{	
		EHttpShellState_None,
		EHttpShellState_Init,
		EHttpShellState_Connecting,
		EHttpShellState_Sending,
		EHttpShellState_RecvResponse,
		EHttpShellState_RecvData,
		EHttpShellState_RecvData_WaitBuffer,
		EHttpShellState_RecvResponse_Ok,
		EHttpShellState_Disconnecting,
		EHttpShellState_Disconnected,
		EHttpShellState_WaitingTcpShell2,
		EHttpShellState_Error
	};
	enum { EDOWNLOAD_SEGMENT_LENGTH = 100*1024 };
	enum { ECmcc_Http_TipPage_MaxSize = 4*1024 };
	enum { EDownload_Buffer_Size = 32*1024 };
	enum { EDownload_Large_Buffer_Size = 64*1024 };
	
	TInt Prepare(const TDesC& _connect_host, int _connect_port, 
					const TDesC& _resource_address, const TDesC& _resource_host,
					TInt _iap_id);

	TInt Start( TInt _start_pos, TInt _total_size = 0, TInt _segment_length = EDOWNLOAD_SEGMENT_LENGTH, 
				TBool _post_req = EFalse, const TDesC& _post_desc=KNullDesC );
	TInt Close();
	TInt Read( TDes8& _data_body );
	//TInt Recv( TDes8& _data_body, TSockXfrLength& _length );
	TInt GetTotalLength();
	const TDesC8& GetContentType(); 

	TInt _exception_flag;
	TInt ReportException( TInt aFeedback );
	void SetTVPlayFlag();

	void AidShellPreparedNotify( TInt aErr );

	TInt _install_notify;
	void SetInstallNotify();

	void SetAcceptWmlFlag(TBool aAccept);
	TBool CanAcceptWml() const;
	void SetReferer(const TDesC& aText);
	void SetForcePieceRead(TBool aForce);
protected:
	void Http_SendRequest (TInt aWantLength=EDOWNLOAD_SEGMENT_LENGTH);
	void Http_RecvResponse ();
	int Http_Analyse_Head ();
protected:
	CTcpShell *iTcpShell, *iTcpShell2;
	CHttpAidShell* iHttpAidShell;
	CenterCore* iCenterCore;
	THttpShellState iState;
	MHttpNotifier* iNotifier;
	TInt iTcpShellDoing;

	TInt _http_port;
	TBuf<256> _http_host;
	TBuf<512> _uri_address;
	TBuf<256> _uri_host;
	TDes8* _data_body_ptr;
	TInt _data_body_valid_length;
	//TPtr8 _data_ptr;
	//char _data_empty[4];
	//TUint8* _data_buf8;

	TBuf8<4*1024> _http_head8;
	TBuf8<1> _one_byte;
	TUint _http_start_bytes;
	TInt _http_response_code;
	TInt _total_file_length;
	TInt _file_bytes;
	TInt _total_bytes_recv;
	TUint _range_from, _range_to;
	TBool _no_content_range;
	TBool _no_content_length;
	TUint _old_http_start_bytes;

	loop_share_buffer* _buffer;
	TBuf8<1024> _data_block;
	TInt iNextWantLength;
	TInt iSegmentLength;
	TSockXfrLength 	_read_len;

	TBuf8<128> _content_type;
	TBuf8<128> _funinhand_download_session;
	TBool _is_http_tv_play;

	TInt _wml_refetch;
	TInt _find_gateway_response;
	TBool _recv_response_ok;
	TInt _post_request;
	TPtrC _post_ptrc;

	TBool _force_piece_read;
	TBool _piece_read;
	TBool _accept_wml;
	TBuf<512> iRefererText;
	TInt iIapId;
};

class CHttpAidShell : public CBase, public MTcpNotifier
{
public:
	~CHttpAidShell();
	CHttpAidShell( CHttpShell* aPrimaryShell );
public:
	enum THttpShellState
	{	
		EHttpAidShellState_None,
		EHttpAidShellState_Init,
		EHttpAidShellState_Connecting,
		EHttpAidShellState_Sending,
		EHttpAidShellState_RecvResponse,
		EHttpAidShellState_RecvData,
		EHttpAidShellState_RecvResponse_Ok,
		EHttpAidShellState_Disconnecting,
		EHttpAidShellState_Error
	};
	enum { EDOWNLOAD_SEGMENT_LENGTH = 100*1024 };
	enum { ECmcc_Http_TipPage_MaxSize = 4*1024 };

	TInt Prepare(const TDesC& _connect_host, int _connect_port, 
			const TDesC& _resource_address, const TDesC& _resource_host,
			TInt _iap_id);

	TInt Start( CTcpShell* aTcpShell, TInt _start_pos, TInt _total_size = 0, TInt _segment_length = EDOWNLOAD_SEGMENT_LENGTH );
	TInt Close();

	TBool IsConnecting();
	TBool IsSendind();
	TBool IsRecvResponse();
	TBool IsPrepareOk();
	TBool CanUse();
	void SetNotifyFlag();
	CTcpShell* ReturnTcpShell();
	void ResetTcpShell();
	void ReveiveParameters(TUint &aHttpStartBytes, TInt &aTotalFileLength, TInt &aFileBytes,
								TUint &aRangeFrom, TUint& aRangeTo);
	TUint GetHttpStartBytes();
	void SetReferer(const TDesC& aText);
protected:
	/*
	From MTcpNotifier
	*/
	virtual void MtnTcpAfterConnect(TInt aErr);
	virtual void MtnTcpAfterDisconnect(TInt aErr);
	virtual void MtnTcpOnReceive(TInt aErr);
	virtual void MtnTcpAfterWrite(TInt aErr);
	virtual void MtnTcpAfterCancel(TInt aErr);
	virtual void MtnTcpOnTimeout(TInt aErr);
	virtual void MtnTcpAfterUserSelectIAP(TInt aErr, TInt aIap);
protected:
	void Http_SendRequest ();
	void Http_RecvResponse ();
	int Http_Analyse_Head ();
protected:
	CTcpShell *iTcpShell;
	CenterCore* iCenterCore;
	THttpShellState iState;
	CHttpShell* iPrimaryShell;
	TBool iNotifyFlag;

	TInt _http_port;
	TBuf<256> _http_host;
	TBuf<512> _uri_address;
	TBuf<256> _uri_host;

	TBuf8<4*1024> _http_head8;
	TBuf8<1> _one_byte;
	TUint _http_start_bytes;
	TInt _http_response_code;
	TInt _total_file_length;
	TInt _file_bytes;
	TUint _range_from, _range_to;

	TInt _wml_refetch;
	TInt iSegmentLength;
	TBuf<512> iRefererText;
	TInt iIapId;
};

class MHttpNotifier
{
public:
	virtual void MhnHttpAfterStart(TInt aErr) = 0;
	virtual void MhnHttpAfterClose(TInt aErr) = 0;
	virtual void MhnHttpAfterRead(TInt aErr, TInt aLength) = 0;
	virtual void MhnHttpOnRedirect(TInt aErr) = 0;
	virtual void MhnHttpNoContentRange(TInt aErr) = 0;
	virtual void MhnHttpLiveResetNotify(TInt aErr) = 0;
	virtual void MhnHttpAfterUserSelectIAP(TInt aErr, TInt aIap) = 0;
};


#endif	//_HTTP_SHELL_H_