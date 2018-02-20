#ifndef _TCP_SHELL_H_
#define _TCP_SHELL_H_

#include "TimeOutTimer.h"
#include "TimeOutNotify.h"
//#include "FondoPlayer.pan"
#include <commdbconnpref.h>
#include <Es_sock.h>
#include <in_sock.h>

class MTcpNotifier;
class CTcpShell : public CActive, public MTimeOutNotify {
public:
	enum TTcpShellState {
		init_state,		//0
		cancel_state,		// 1

		start_pref_state,		// 2
		predisconnecting_state,
		
		connecting_state,		// 4
		connected_state,
		connect_failed_state,

		timeout_state,			// 7
		lookingup_state,			// 8
		lookup_success_state,
		lookup_failed_state,

		disconnecting_state,		// 11
		disconnected_state,
		disconnect_failed_state,

		writing_state,		// 14
		write_success_state,
		write_fail_state,
		write_reset_state,

		reading_state,		// 18
		read_success_state,
		read_fail_state,
		read_reset_state  //21
	};

public:
	TBuf<128> _server_name;
	TInt _server_port;
	TInt _time_out_value;
	TCommDbConnPref _prefs;

public:
	TTcpShellState _shell_state;
	TInt _is_connected;

	RSocketServ _socket_service;
	RConnection _conn;
	RSocket _socket;
	int _conn_valid;

	CTimeOutTimer *_timer;

	RHostResolver _host_resolver;
	TNameEntry _name_entry;
	TNameRecord _name_record;

	int _apn_id;

protected:
	TInetAddr _server_address;
	TBuf<32> _server_ip_string;
	TBool  isOpen;

	MTcpNotifier *iNotifier;
public:
	static CTcpShell* NewL (MTcpNotifier *aNotifier);
	~CTcpShell ();

	CTcpShell() : CActive(20), _apn_id(0){}

	void SetServerName (const TDesC& _aName) {
		_server_name.Copy (_aName);
	}

	void SetServerPort (TInt _aPort) {
		_server_port = _aPort;
	}

	void SetTimeOut (TInt _aTO) {
		_time_out_value = _aTO;
	}

	void SetApnId (int _id) {
		_apn_id = _id;
	}

protected:
	/* From CTimeoutTimer */
	void TimerExpired ();
protected:
	void ConstructL (MTcpNotifier *aNotifier);
	void ChangeStateTo (TTcpShellState _newstate);
	void ConnectIpIntL (TUint32 _addr);
	

	void ListNetworksAndSignalL();
public:
	void StartPrefConnection ();
	void ConnectL ();
	void ConnectServerL ();
	void Disconnect ();
	void DoCancel ();
	void DoTimeout ();
 	 TInt RunError( TInt aError );
	TInt Write (const TDesC8& _desc);
	TInt Read (TDes8& _desc, TSockXfrLength& _read_len, TInt _new_timeout_value) ;
	TInt ReadBlock (TDes8& _desc, TInt _new_timeout_value);
	TInt ReadBlock (TDes8& _desc, TInt _new_timeout_value, TSockXfrLength& _length);
	void RunL ();
	TInt IsBusy () const;
	void SetNotifier(MTcpNotifier *aNotifier);
};

class MTcpNotifier
{
public:
	virtual void MtnTcpAfterConnect(TInt aErr) = 0;
	virtual void MtnTcpAfterDisconnect(TInt aErr) = 0;
	virtual void MtnTcpOnReceive(TInt aErr) = 0;
	virtual void MtnTcpAfterWrite(TInt aErr) = 0;
	virtual void MtnTcpAfterCancel(TInt aErr) = 0;
	virtual void MtnTcpOnTimeout(TInt aErr) = 0;
};

#endif

