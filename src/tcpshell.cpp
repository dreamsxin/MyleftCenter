
/*
============================================================================
 Name        : tcpshell from tcpshell.h
 Author      : zyl copy from wojunjun
 Version     :
 Copyright   : Your copyright notice
 Description : Container control PlayList
============================================================================
*/

// INCLUDE FILES
#include "tcpshell.h"
#include <commdbconnpref.h>
#include <aputils.h>
#include <Es_sock.h>
#include <in_sock.h>
#include "funPanic.h"
#include "portab_symbian.h"

CTcpShell* CTcpShell::NewL (MTcpNotifier *aNotifier) 
{
	CTcpShell* self = new (ELeave) CTcpShell;
	CleanupStack::PushL (self);
	self->ConstructL (aNotifier);
	CleanupStack::Pop ();
	return self;
}


CTcpShell::~CTcpShell () 
{
	FC_Log("CTcpShell::~CTcpShell () ");
	Cancel ();
	delete _timer;FC_Log("~CTcpShell ()::	delete _timer; ");
	
	_socket.Close ();FC_Log("~CTcpShell ()::	_socket.Close (); ");
	isOpen=FALSE;
	if (_conn_valid)
		_conn.Close ();FC_Log("~CTcpShell ()::	_conn.Close (); ");
	
	_socket_service.Close ();FC_Log("~CTcpShell ()::	_socket_service.Close (); ");
}

void CTcpShell::TimerExpired () 
{
	FC_Log("CTcpShell::TimerExpired () _shell_state=%d", _shell_state);
//	FC_Logsp("CTcpShell::TimerExpired () _shell_state=%d", _shell_state);
	int _cur_shell_state = _shell_state;
	Cancel ();
	switch (_cur_shell_state) 
	{
		case connecting_state:
		case lookingup_state:
		{
		if(iNotifier)
			iNotifier->MtnTcpAfterConnect(KErrTimedOut );


		break;
		}
		case writing_state:
		{
		if(iNotifier)
			iNotifier->MtnTcpAfterWrite(KErrTimedOut );


		break;
		}

		case reading_state:
		{
		if(iNotifier)
			iNotifier->MtnTcpOnReceive(KErrTimedOut );


		break;
		}
		default :
		{
		if(iNotifier)
			iNotifier->MtnTcpOnTimeout(0);

		}
		break;

	}
	
}

void CTcpShell::ConstructL (MTcpNotifier *aNotifier) 
{
	FC_Log("CTcpShell::ConstructL () ,this=%d",this);
	_conn_valid = 0;
	_is_connected = 0;
	_shell_state = init_state;
	iNotifier = aNotifier;
	
	_timer = CTimeOutTimer::NewL (EPriorityHigh, *this);
	CActiveScheduler::Add (this);
	User::LeaveIfError (_socket_service.Connect());
	//ConnectSmoothly();
}

void CTcpShell::ChangeStateTo (TTcpShellState _newstate) 
{
	_shell_state = _newstate;
}

#ifndef _NOKIA_61_	 // defined(_NOKIA_70_) || defined(_NOKIA_80_)	|| defined(_NOKIA_90_) 
#include <rconnmon.h> 
void CTcpShell::ConnectL () 
{

	User::After (200000);
	//ListNetworksAndSignalL();
	FC_Log("CTcpShell::ConnectL () ");
	if (_apn_id && !_conn_valid) {
		CCommsDatabase *_db = CCommsDatabase::NewL ();
		CleanupStack::PushL (_db);
//		CApUtils* _aputils = CApUtils::NewLC (*_db);  FC_Log("CTcpShell::ConnectL ()--- _apn_id=%d",_apn_id);
 		FC_Log("CTcpShell::ConnectL ()--- _apn_id=%d",_apn_id);
		int _real_iap_id =_apn_id;// _aputils->IapIdFromWapIdL (_apn_id);FC_Log("CTcpShell::ConnectL ()---_aputils->IapIdFromWapIdL()");
		CleanupStack::PopAndDestroy (1);
		FC_Log("_real_iap_id=%d ",_real_iap_id);
		_conn.Open (_socket_service);
		_prefs.SetDialogPreference (ECommDbDialogPrefDoNotPrompt);
		_prefs.SetBearerSet (ECommDbBearerUnknown);
		_prefs.SetDirection (ECommDbConnectionDirectionOutgoing);
		_prefs.SetIapId (_real_iap_id);
		ChangeStateTo (start_pref_state);
		_conn.Start (_prefs, iStatus);
		SetActive ();
		
	} 
	else 
	{
		
		ConnectServerL ();
	}

//	ConnectServerL ();
	FC_Log("ConnectL (){_conn.iStatus=%d} ",iStatus.Int());
	FC_Log("~ConnectL ()");
}
#else
void CTcpShell::ConnectL () 
{
	ConnectServerL ();
}
#endif

void CTcpShell::ConnectIpIntL (TUint32 _addr) 
{
	FC_Log("ConnectIpIntL()");
	_socket.Close ();
	FC_Log("ConnectIpIntL()-----_socket.Close ();");
	if (_conn_valid) 
	{
		User::LeaveIfError (_socket.Open (
			_socket_service, KAfInet, KSockStream,
			KProtocolInetTcp, _conn));
		isOpen=TRUE;
	} 
	else 
	{
		
		User::LeaveIfError (_socket.Open (
			_socket_service, KAfInet, KSockStream, 
			KProtocolInetTcp));
		isOpen=TRUE;
	}
	FC_Log("ConnectIpIntL()-----_socket.Open()");
	
	_server_address.SetPort (_server_port);
	_server_address.SetAddress (_addr);
	FC_Log("ConnectIpIntL()-----_server_address.SetPort=%d,_server_address.SetAddress=%d",_server_port,_addr);
	
	_socket.Connect (_server_address, iStatus);FC_Log("ConnectIpIntL()-----_server_address.Connect ()");
	ChangeStateTo (connecting_state);
	SetActive ();

	_timer->After (_time_out_value);

	FC_Log("~ConnectIpIntL()::iStatus=%d",iStatus.Int());
}

void CTcpShell::ConnectServerL () 
{
		FC_Log("ConnectServerL()");
	if (IsActive()) 
	{
		FC_Panic(TCP_CONNECT);
	}

	if (_is_connected) 
	{
		_socket.Shutdown (RSocket::ENormal, iStatus);
		SetActive ();
		ChangeStateTo (predisconnecting_state);
		_timer->After (2000000);
		return;
	}

	TInetAddr _temp_addr;
	if (_temp_addr.Input (_server_name) == KErrNone) 
	{
		FC_Log("ConnectServerL() _temp_addr.Input (_server_name) == KErrNone");
		ConnectIpIntL (_temp_addr.Address());
	} 
	else 
	{
		if (_conn_valid) 
		{
			User::LeaveIfError (_host_resolver.Open (
				_socket_service, KAfInet, 
				KProtocolInetUdp, _conn));
		} 
		else 
		{
			User::LeaveIfError (_host_resolver.Open (
				_socket_service, KAfInet,
				KProtocolInetUdp));
		}
		FC_Log("ConnectServerL() step 3333333333333333");
		_host_resolver.GetByName (_server_name, _name_entry, iStatus);
		SetActive ();
		ChangeStateTo (lookingup_state);
		_timer->After (_time_out_value);
	}

	FC_Log("~ConnectServerL()");
}

void CTcpShell::Disconnect () 
{
	if (IsActive()) 
	{
		FC_Panic(ETcpDisconnectAgain); 	//cancel by zyl
	}
	_timer->Cancel ();
	if(isOpen)
	{
	_socket.Shutdown (RSocket::ENormal, iStatus);
	SetActive ();
	_timer->After (_time_out_value);
	}
	ChangeStateTo (disconnecting_state);
}

void CTcpShell::DoCancel () 
{
	FC_Log("CTcpShell::DoCancel () ~~~ ~~_shell_state::=%d ",_shell_state);
	_timer->Cancel ();
	switch (_shell_state) 
	{
	case start_pref_state:
		_is_connected = 0;
		break;
	case predisconnecting_state:
		_socket.Close ();
		isOpen=FALSE;
		_is_connected = 0;
		break;
	case connecting_state:
		_socket.CancelConnect ();
		_is_connected = 0;
		break;
	case lookingup_state:
		_host_resolver.Cancel ();
		_host_resolver.Close ();
		_is_connected = 0;
		break;
	case disconnecting_state:
		_socket.Close ();
		isOpen=FALSE;
		_is_connected = 0;
		break;
	case writing_state:
		_socket.CancelWrite ();
		break;
	case reading_state:
		_socket.CancelRead ();
		break;
	default:
		FC_Panic(ETcpBadStatus);  
		break;
	}

	ChangeStateTo (cancel_state);
//	FC_Log("CTcpShell::DoCancel () ~~~ ~~_shell_state::cancel_state ");
	//IssueRequestComplete (KErrCancel);
	//if(iNotifier)
	//	iNotifier->MtnTcpAfterCancel(KErrNone);

	return;
}

  TInt CTcpShell::RunError( TInt aError )
  {
	FC_Log("CTcpShell::RunError () ,this=%d, aError=%d",this, aError);
	//iObserver.NotifyLoadCompleted(aError, *this);
	return KErrNone;
  }
  void CTcpShell::RunL () 
{
	//FC_Log("CTcpShell::RunL () ,this=%d, status=%d",this, iStatus.Int());
	//FC_Logsp("CTcpShell::RunL () ,this=%d, status=%d",this, iStatus.Int());
	_timer->Cancel ();
 	switch (_shell_state) 
	{
	case start_pref_state:
		if (iStatus == KErrNone) 
		{
			_conn_valid = 1;
			ConnectServerL ();
			FC_Log("CTcpShell::RunL () ~~~ ~~start_pref_state::KErrNone ");
		} 
		else 
		{
			ChangeStateTo (connect_failed_state);
			_is_connected = 0;
			//IssueRequestComplete (KErrGeneral);
			if(iNotifier)
				iNotifier->MtnTcpAfterConnect(KErrGeneral);
			FC_Log("CTcpShell::RunL () ~~~start_pref_state:: KErrGeneral ");
		}
		
		break;

	case predisconnecting_state:
		_is_connected = 0;
		ChangeStateTo (init_state);
		ConnectL ();
		break;

	case connecting_state:
		
		if (iStatus == KErrNone) 
		{
		
			ChangeStateTo (connected_state);
			_is_connected = 1;
			//IssueRequestComplete (KErrNone);
			if(iNotifier)
				iNotifier->MtnTcpAfterConnect(KErrNone);
			FC_Log("CTcpShell::RunL () ~~~ connecting_state ::KErrNone");
			
		} 
		else 
		{
			ChangeStateTo (connect_failed_state);
			_is_connected = 0;
			FC_Log("CTcpShell::RunL () ~~~ connecting_state ::KErrGeneral");
			//IssueRequestComplete (KErrGeneral);
			if(iNotifier)
				iNotifier->MtnTcpAfterConnect(KErrGeneral);
		}
		break;

	case lookingup_state:
		_host_resolver.Close ();
		if (iStatus == KErrNone) 
		{
			_name_record = _name_entry ();
			TInetAddr::Cast (_name_record.iAddr).Output (_server_ip_string);
			ChangeStateTo (lookup_success_state);
			_is_connected = 0;
			ConnectIpIntL (TInetAddr::Cast (_name_record.iAddr).Address ());
			return;
		} 
		else 
		{
			ChangeStateTo (lookup_failed_state);
			_is_connected = 0;
			//IssueRequestComplete (KErrGeneral);
			if(iNotifier)
				iNotifier->MtnTcpAfterConnect(KErrGeneral);
		}
		
		break;

	case disconnecting_state:
		if (iStatus == KErrNone) 
		{
			_socket.Close ();
			isOpen=FALSE;
			_is_connected = 0;
			ChangeStateTo (disconnected_state);
			//IssueRequestComplete (KErrNone);
			if(iNotifier)
				iNotifier->MtnTcpAfterDisconnect(KErrNone);
		} 
		else 
		{
			ChangeStateTo (disconnect_failed_state);
			_is_connected = 0;
			//IssueRequestComplete (KErrGeneral);
			if(iNotifier)
				iNotifier->MtnTcpAfterDisconnect(KErrGeneral);
		}
		break;

	case writing_state:
		if (iStatus == KErrNone) 
		{
			ChangeStateTo (write_success_state);
//			FC_Log("RunL:	iStatus=%d,_shell_state=write_success_state",iStatus.Int());
			if(iNotifier)
				iNotifier->MtnTcpAfterWrite(KErrNone);
		}
		else if (iStatus == KErrEof) 
		{
			ChangeStateTo (write_reset_state);
			//IssueRequestComplete (KErrGeneral);
			FC_Log("RunL:	iStatus=%d,_shell_state=write_reset_state",iStatus.Int());
			if(iNotifier)
				iNotifier->MtnTcpAfterWrite(KErrEof);
		} 
		else 
		{
			ChangeStateTo (write_fail_state);
			//IssueRequestComplete (KErrGeneral);
			FC_Log("RunL:	iStatus=%d,_shell_state=write_fail_state",iStatus.Int());
			if(iNotifier)
				iNotifier->MtnTcpAfterWrite(KErrGeneral);
		}
		break;

	case reading_state:
		if (iStatus == KErrNone) 
		{
			ChangeStateTo (read_success_state);
			//IssueRequestComplete (KErrNone);
//			FC_Log("RunL:	iStatus=0x%d,_shell_state=read_success_state",iStatus.Int());
			if(iNotifier)
				iNotifier->MtnTcpOnReceive(KErrNone);
		} 
		else if (iStatus == KErrEof) 
		{
			ChangeStateTo (read_reset_state);
			//IssueRequestComplete (KErrGeneral);
			FC_Log("RunL:	iStatus=%d,_shell_state=read_reset_state",iStatus.Int());
			if(iNotifier)
				iNotifier->MtnTcpOnReceive(KErrEof);
		} 
		else // if (iStatus != KRequestPending) 
		{
			ChangeStateTo (read_fail_state);
			//IssueRequestComplete (KErrGeneral);
			FC_Log("RunL:	iStatus=%d,_shell_state=read_fail_state",iStatus.Int());
			if(iNotifier)
				iNotifier->MtnTcpOnReceive(KErrGeneral);
		}
		break;
	case cancel_state:
		if(iNotifier)
			iNotifier->MtnTcpAfterCancel(KErrNone);
		break;
	default:
		FC_Panic(ETcpBadStatus);		//cancel by zyl
		break;
	}
//	FC_Log("~RunL()::iStatus=%d,_shell_state=%d,this=%d",iStatus.Int(),_shell_state,this);

	return;
}

TInt CTcpShell::Write (const TDesC8& _desc) 
{
	FC_Log("CTcpShell::Write(),this=%d ",this);

	if (IsActive()) 
	{
		DoCancel();
		FC_Log("CTcpShell::Write() ---	IsActive()");
		return 1;
	}

	if (!_is_connected)
		return 0;

	_timer->Cancel ();
	if (_desc.Length() > 0) 
	{
		_socket.Write (_desc, iStatus);
		_timer->After (_time_out_value);
		SetActive ();
		ChangeStateTo (writing_state);
	}
	FC_Log("CTcpShell::~Write() ");
	return 1;
}


TInt CTcpShell::Read (TDes8& _desc, TSockXfrLength& _read_len, TInt _new_timeout_value) 
{
	FC_Log("CTcpShell::Read(),this=%d ",this);
	if (IsActive()) 
	{
		return 1;
		//FC_Panic(TCP_ReadAgain);	
	}
	if (!_is_connected)
		return 0;
		
	_timer->Cancel ();
	_socket.RecvOneOrMore (_desc, 0, iStatus, _read_len);
	_timer->After (_new_timeout_value);
	SetActive ();
	ChangeStateTo (reading_state);
	
	FC_Log("CTcpShell::~Read() ");
	return 1;
}

TInt CTcpShell::ReadBlock (TDes8& _desc, TInt _new_timeout_value) 
{
//	FC_Log("CTcpShell::ReadBlock() ,this=%d",this);
	if (IsActive()) 
	{
		return 1;
		//FC_Panic(TCP_ReadBlockAgain);	
	}
	if (!_is_connected)
		return 0;

	_timer->Cancel ();
	_socket.Recv (_desc, 0, iStatus);
	_timer->After (_new_timeout_value);
	SetActive ();
	ChangeStateTo (reading_state);
//	FC_Log("CTcpShell::~ReadBlock() ");
	return 1;
}

TInt CTcpShell::ReadBlock (TDes8& _desc, TInt _new_timeout_value, TSockXfrLength& _length)
{
//	FC_Log("CTcpShell::ReadBlock() ,this=%d",this);
	if (IsActive()) 
	{
		return 1;
		//FC_Panic(TCP_ReadBlockAgain);	
	}
	if (!_is_connected)
		return 0;
	//User::After(100000);
	_timer->Cancel ();
	_socket.Recv (_desc, 0, iStatus, _length);
	_timer->After (_new_timeout_value);
	SetActive ();
	ChangeStateTo (reading_state);
//	FC_Log("CTcpShell::~ReadBlock() ");
	return 1;
}

TInt CTcpShell::IsBusy () const 
{
	return IsActive ();
}

void CTcpShell::ListNetworksAndSignalL() 
{
//	_LIT(KNetInfo, "Network: %S\tSignal: %d");

	TBuf<100> netName;

//	TBuf<100> line;
	HBufC *msg_hbuf = HBufC::NewL(1024);
	TPtr msg = msg_hbuf->Des();
	TUint count;

	RConnectionMonitor monitor;

//	TPckgBuf<TConnMonNetworkNames> pkgNetworks;

	// establish connection with the monitor server

	monitor.ConnectL();

	// prepare leave clean-up

	CleanupClosePushL(monitor);

	TRequestStatus status;

	// get the list of available networks

	//monitor.GetPckgAttribute(EBearerIdWLAN, 0, KNetworkNames, pkgNetworks, status); 
	monitor.GetConnectionCount( count, status );

	// suspend thread until the info is retrieved

	// production code should use active objects

	User::WaitForRequest( status ) ;

	// leave if the asynchronous method returned an error

	User::LeaveIfError(status.Int());

	//  reset networks list

	//aNetworks.Reset();

	// add retrieved networks to the list

	for(TUint i=0; i<count; i++)

	{
		TUint conn_id, sub_count; //sub_conn_id;
		monitor.GetConnectionInfo( i+1, conn_id, sub_count );
		msg.AppendNum( conn_id );
		msg.Append( _L(";") );
		msg.AppendNum( sub_count );
		msg.Append( _L(";") );

		monitor.GetStringAttribute( conn_id, 0, KIAPName, netName, status );
		User::WaitForRequest( status ) ;
		User::LeaveIfError(status.Int());

		msg.Append( netName );
		msg.Append( _L(";") );

		monitor.GetStringAttribute( conn_id, 0, KAccessPointName, netName, status );
		User::WaitForRequest( status ) ;
		User::LeaveIfError(status.Int());

		msg.Append( netName );
		msg.Append( _L(";") );

		/*for( TUint j=0; j<sub_count; j++ )
		{
			monitor.GetSubConnectionInfo( conn_id, j, sub_conn_id );
			msg.AppendNum( sub_conn_id );
			msg.Append( _L(";") );
		}*/
		

		msg.Append(_L("\r\n"));
		

	}

	msg.AppendNum( count );

	// close server session

	CleanupStack::PopAndDestroy(&monitor);

	//CFondoGlobal::ShowBox( _L(""), msg);
	delete msg_hbuf;

}
void CTcpShell::SetNotifier( MTcpNotifier *aNotifier ) 
{
	iNotifier = aNotifier;
}

