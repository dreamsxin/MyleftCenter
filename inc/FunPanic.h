
#ifndef FUNVV_PANIC_H
#define FUNCVV_PANIC_H

typedef enum TFunPanic {
	Malloc_Failed=0,         //  0
	TCP_CONNECT=1,
	TCP_ReadAgain=2,
	TCP_ReadBlockAgain=3,
	ETcpDisconnectAgain=4,
	ETcpBadStatus=5,
	EGprsAllInUsed=6,
	EGprsNoAfterConnCallback=7,
	MSGAO_FULL=8,
	EDownloadShellOpenBadStatus,
	EDownloadShellStartBadStatus,
	EDownloadShellAfterStartBadStatus,
	EDownloadShellRecvBadStatus,
	EHttpAfterConnectBadStatus,
	EHttpDisconnectBadStatus,
	EHttpOnReceiveBadStatus,
	EHttpAfterWriteBadStatus,
	FUN_PANIC_MAX
	};
#endif