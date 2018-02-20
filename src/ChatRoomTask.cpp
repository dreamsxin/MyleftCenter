
#include "portab_symbian.h"
#include "CenterCore.h"
#include "const.h"

#include "ChatRoomTask.h"

#include  "MyleftCenterViewChatRoom.h"
#include  "MyleftCenterContainerChatRoom.h"
#if 0
CBOOL HttpGetParserUrl(SHttpGet* pHttpGet)
{
	char	szBody[MAXPATH];
	char*	p;

	if( pHttpGet == NULL )
		return FALSE;

	if( strlen( pHttpGet->m_szUrl) < 7 )
		return FALSE;

	if( strncmp( pHttpGet->m_szUrl, "http://", 7))
		return FALSE;

	strcpy( szBody, pHttpGet->m_szUrl + 7);

	p = strchr( szBody, '/');
	if( p != NULL )
	{
		strcpy( pHttpGet->m_szObject, p);
		*p = '\0';
	}
	else
	{
		pHttpGet->m_szObject[0] = '\0';
	}

	strcpy( pHttpGet->m_szServerAddr, szBody);

	if( strlen( pHttpGet->m_szServerAddr) == 0 )
		return FALSE;

	p = strchr( pHttpGet->m_szServerAddr, ':');
	if( p == NULL )
	{
		pHttpGet->m_dwServerHttpPort = 80;
	}
	else
	{
		*p = '\0';
		pHttpGet->m_dwServerHttpPort = atoi( p + 1);
	}

	return TRUE;
}

void HttpGetClose(SHttpGet* pHttpGet)
{
	if( pHttpGet == NULL )
		return;

	if( pHttpGet->m_nSocketID >= 0 )
	{
		FC_Log("HttpGetClose:: m_nSocketID=%d", pHttpGet->m_nSocketID);
		FC_socketClose(pHttpGet->m_nSocketID);
	}

	pHttpGet->m_nSocketID = -1;
}

void HttpGetFormatRequest(SHttpGet* pHttpGet, DWORD dwFrom, DWORD dwTo)
{
	char	szPort[10];
	int	nPostLen = strlen(pHttpGet->m_szPost);
	
	if( pHttpGet == NULL )
		return;

	sprintf(szPort,":%lu", pHttpGet->m_dwServerHttpPort);
	memset( pHttpGet->m_szRequestHeader,0,MAX_HTTP_HEAD_LEN);

	if( nPostLen > 0 )
		strcpy(pHttpGet->m_szRequestHeader,"POST ");
	else
		strcpy(pHttpGet->m_szRequestHeader,"GET ");
		
	if( pHttpGet->m_nProxyPort )
	{
		strcat(pHttpGet->m_szRequestHeader,"http://");
		strcat(pHttpGet->m_szRequestHeader,pHttpGet->m_szServerAddr);
		strcat(pHttpGet->m_szRequestHeader,szPort);
	}
	strcat(pHttpGet->m_szRequestHeader,pHttpGet->m_szObject);
	strcat(pHttpGet->m_szRequestHeader," HTTP/1.0");
	strcat(pHttpGet->m_szRequestHeader,"\r\n");

	strcat(pHttpGet->m_szRequestHeader,"Host: ");
	strcat(pHttpGet->m_szRequestHeader,pHttpGet->m_szServerAddr);
	strcat(pHttpGet->m_szRequestHeader,szPort);
	strcat(pHttpGet->m_szRequestHeader,"\r\n");

#ifdef TARGET_SYMBIAN
	strcat(pHttpGet->m_szRequestHeader,"Accept: */*");
	strcat(pHttpGet->m_szRequestHeader,"\r\n");

	strcat(pHttpGet->m_szRequestHeader,"Accept-Charset: gb2312, iso-8859-1, us-ascii, utf-8");
	strcat(pHttpGet->m_szRequestHeader,"\r\n");

	strcat(pHttpGet->m_szRequestHeader,"Accept-Language: zh-ch, zh, en");
	strcat(pHttpGet->m_szRequestHeader,"\r\n");

	strcat(pHttpGet->m_szRequestHeader,"Connection: close");
	strcat(pHttpGet->m_szRequestHeader,"\r\n");

	strcat(pHttpGet->m_szRequestHeader,"User-Agent: Nokia N73");
	strcat(pHttpGet->m_szRequestHeader,"\r\n");
#else
	strcat(pHttpGet->m_szRequestHeader,"Accept: */*");
	strcat(pHttpGet->m_szRequestHeader,"\r\n");

	strcat(pHttpGet->m_szRequestHeader,"Accept-Charset: iso-8859-1, us-ascii, utf-8");
	strcat(pHttpGet->m_szRequestHeader,"\r\n");

	strcat(pHttpGet->m_szRequestHeader,"Accept-Language: zh-ch, zh");
	strcat(pHttpGet->m_szRequestHeader,"\r\n");

	strcat(pHttpGet->m_szRequestHeader,"Connection: Close");
	strcat(pHttpGet->m_szRequestHeader,"\r\n");

	strcat(pHttpGet->m_szRequestHeader,"User-Agent: Dopod838");
	strcat(pHttpGet->m_szRequestHeader,"\r\n");
#endif

	if( strlen( pHttpGet->m_szReferer) > 0 )
	{
		strcat(pHttpGet->m_szRequestHeader,"Referer: ");
		strcat(pHttpGet->m_szRequestHeader,pHttpGet->m_szReferer);
		strcat(pHttpGet->m_szRequestHeader,"\r\n");
	}

	if( nPostLen > 0 )
	{
		char		szLength[64];
		sprintf( szLength, "Content-Length: %d", nPostLen);
		strcat( pHttpGet->m_szRequestHeader,szLength);
		strcat( pHttpGet->m_szRequestHeader,"\r\n");
	}

	if( dwTo > dwFrom )
	{
		char szTemp[64];
		sprintf( szTemp, "Range: bytes=%lu-%lu\r\n", dwFrom, dwTo);
		strcat( pHttpGet->m_szRequestHeader, szTemp);
		FC_Log("HttpGetFormatRequest, %s", szTemp);
	}

	strcat(pHttpGet->m_szRequestHeader,"\r\n");

	if( nPostLen > 0 )
	{
		strcat(pHttpGet->m_szRequestHeader, pHttpGet->m_szPost);
	}
	FC_Log("HttpGetFormatRequest,%s", pHttpGet->m_szRequestHeader);
}

void	HttpGetInit(SHttpGet* pHttpGet, char* szUrl, char* szProxy, int nPort, char* szReferer, char* szPost)
{
	if( pHttpGet == NULL )
		return;

	strcpy( pHttpGet->m_szUrl, szUrl);
	strcpy( pHttpGet->m_szReferer, szReferer);
	strcpy( pHttpGet->m_szPost, szPost);
	
	pHttpGet->m_nProxyPort = nPort;
	if( nPort )
	{
		strcpy( pHttpGet->m_szProxy, szProxy);
	}

	pHttpGet->m_nSocketID = -1;
	pHttpGet->m_hThread = NULL;
	pHttpGet->m_bStop = TRUE;
	pHttpGet->m_dwCurRead = 0;
	pHttpGet->m_nStatus = GET_STATUS_STOP;
	pHttpGet->m_bThreadRunning = FALSE;

	pHttpGet->m_szRequestHeader = NULL;
	pHttpGet->m_szResponseHeader = NULL;
}

void	HttpGetUnInit(SHttpGet* pHttpGet)
{
	if( pHttpGet == NULL )
		return;

	HttpGetStop(pHttpGet);
	HttpGetClose(pHttpGet);
	
	SAFE_FREE(pHttpGet->m_szRequestHeader);
	SAFE_FREE(pHttpGet->m_szResponseHeader);
}

CBOOL HttpGetPrepareThread(SHttpGet* pHttpGet)
{
	if( pHttpGet == NULL )
		return FALSE;

	pHttpGet->m_bThreadRunning = TRUE;

	if( pHttpGet->m_pDataBuf )
		pHttpGet->m_pDataBuf->m_dwDataSize = 0;

	pHttpGet->m_nTryNum = 0;

	if( pHttpGet->m_szRequestHeader == NULL )
	{
		pHttpGet->m_szRequestHeader = (char*)FC_malloc(MAX_HTTP_HEAD_LEN);
	}
	if( pHttpGet->m_szResponseHeader == NULL )
	{
		pHttpGet->m_szResponseHeader = (char*)FC_malloc(MAX_HTTP_HEAD_LEN);
	}

	pHttpGet->m_nLoopTry = 0;

	return ((pHttpGet->m_szRequestHeader != NULL) && (pHttpGet->m_szResponseHeader != NULL));
}

void HttpGetUnPrepareThread(SHttpGet* pHttpGet)
{
	if( pHttpGet == NULL )
		return;

	SAFE_FREE(pHttpGet->m_szRequestHeader);
	SAFE_FREE(pHttpGet->m_szResponseHeader);

	pHttpGet->m_pDataBuf = NULL;
	pHttpGet->m_bStop = TRUE;
	pHttpGet->m_bThreadRunning = FALSE;

#ifndef	MULTITHREAD
	pHttpGet->m_funcHttpSourceCallback( pHttpGet->m_hOwner);
#endif
}

#ifdef	MULTITHREAD
CBOOL HttpGetSocketWrite( SHttpGet* pHttpGet, char* szBuf, DWORD dwLen)
{
	DWORD	dwWrite = 0;
	int		dwWT;
	int		nStatus;
	int		nTryNum = 0;

	if( pHttpGet == NULL )
		return FALSE;

	while(!pHttpGet->m_bStop)
	{
		if( nTryNum > MAX_SOCKET_TRY_NUM )
			break;

		dwWT = FC_socketSend( pHttpGet->m_nSocketID, szBuf + dwWrite, dwLen - dwWrite, &nStatus);

		if( dwWT + dwWrite == dwLen )
		{
			return TRUE;
		}

		if( nStatus == FC_SOCKET_TIMEOUT )
		{
			nTryNum++;
			if( dwWT > 0 )
				dwWrite += dwWT;

			FC_Sleep(20);
			continue;
		}

		if( nStatus == FC_SOCKET_SUCCESS )
		{
			if( dwWT > 0 )
			{
				nTryNum = 0;
				dwWrite += dwWT;
			}

			if( dwWT == 0 )
			{
				nTryNum++;
				FC_Sleep(20);
			}

			continue;
		}

		break;
	}

	return FALSE;
}

CBOOL HttpGetSocketRead( SHttpGet* pHttpGet, char* szBuf, DWORD dwLen)
{
	int		dwRD;
	int		nStatus;
	int		nTryNum = 0;
	DWORD	dwOnce;

	while(!pHttpGet->m_bStop)
	{
		if( nTryNum > MAX_SOCKET_TRY_NUM )
			break;

		if( dwLen == pHttpGet->m_dwCurRead )
		{
			return TRUE;
		}

		dwOnce = dwLen - pHttpGet->m_dwCurRead;

		dwRD = FC_socketRecv( pHttpGet->m_nSocketID, szBuf + pHttpGet->m_dwCurRead, dwOnce, &nStatus);

		if( dwRD + pHttpGet->m_dwCurRead == dwLen )
		{
			pHttpGet->m_dwCurRead += dwRD;
			return TRUE;
		}

		if( nStatus == FC_SOCKET_TIMEOUT )
		{
			nTryNum++;
			if( dwRD > 0 )
				pHttpGet->m_dwCurRead += dwRD;

			FC_Sleep(20);
			continue;
		}

		if( nStatus == FC_SOCKET_SUCCESS )
		{
			if( dwRD > 0 )
			{
				nTryNum = 0;
				pHttpGet->m_dwCurRead += dwRD;
			}

			if( dwRD == 0 )
			{
				nTryNum++;
				FC_Sleep(20);
			}

			continue;
		}

		break;
	}

	return FALSE;
}

CBOOL HttpGetConnectServer(SHttpGet* pHttpGet)
{
	int	nRet;

	HttpGetClose(pHttpGet);

	pHttpGet->m_nSocketID = FC_socketCreate(FC_SOCKET_TCP);
	if( pHttpGet->m_nSocketID  < 0 )
	{
		FC_Log("HttpGetConnectServer:: create fail");
		return FALSE;
	}
	FC_Log("HttpGetConnectServer:: create m_nSocketID=%d", pHttpGet->m_nSocketID);

	if( pHttpGet->m_nProxyPort )
	{
		nRet = FC_socketConnect( pHttpGet->m_nSocketID, pHttpGet->m_szProxy, pHttpGet->m_nProxyPort);
	}
	else
	{
		nRet = FC_socketConnect( pHttpGet->m_nSocketID, pHttpGet->m_szServerAddr, (WORD)pHttpGet->m_dwServerHttpPort);
	}

	return nRet > 0;
}

CBOOL HttpGetSocketReadResponse( SHttpGet* pHttpGet, char* szBuf, DWORD dwLen)
{
	DWORD	i;

	memset( szBuf, 0, dwLen);

	pHttpGet->m_dwResponseSaveLen = 0;
	pHttpGet->m_dwCurRead = 0;

	HttpGetSocketRead( pHttpGet, szBuf, dwLen);

	if( pHttpGet->m_dwCurRead < 4 )
		return FALSE;

	for( i = 0 ; i <= pHttpGet->m_dwCurRead - 4 ; i++ )
	{
		if(szBuf[i] == '\r' && szBuf[i+1] == '\n' && szBuf[i+2] == '\r' && szBuf[i+3] == '\n')
		{
			szBuf[i] = 0;

			if( i + 4 < pHttpGet->m_dwCurRead )
			{
				pHttpGet->m_dwResponseSaveLen = pHttpGet->m_dwCurRead - i - 4;
				memcpy( pHttpGet->m_pResponseSave, szBuf+i+4, pHttpGet->m_dwResponseSaveLen);
			}

			return TRUE;
		}
	}

	return FALSE;
}

CBOOL HttpGetSendRequest( SHttpGet* pHttpGet, DWORD* pFrom, DWORD* pTo, DWORD* pLength)
{
	CBOOL	bRet = FALSE;
	int		nTryNum = 0;

	if( !HttpGetParserUrl(pHttpGet))
		return FALSE;

	HttpGetFormatRequest( pHttpGet, *pFrom, *pTo);

	while( !pHttpGet->m_bStop )
	{
		int		nLength;
		float	fVer;
		int		nRet;
		char*	p;

		if( !HttpGetConnectServer(pHttpGet))
			break;

		nTryNum++;
		if( nTryNum > MAX_SOCKET_TRY_NUM )
			break;

		nLength = strlen(pHttpGet->m_szRequestHeader);

		if( !HttpGetSocketWrite( pHttpGet, pHttpGet->m_szRequestHeader, nLength))
		{
			continue;
		}

		if( !HttpGetSocketReadResponse( pHttpGet, pHttpGet->m_szResponseHeader, MAX_HTTP_HEAD_LEN))
		{
			FC_Sleep(20);
			continue;
		}

		FC_Log(pHttpGet->m_szResponseHeader);
		str_tolower(pHttpGet->m_szResponseHeader);

		if( sscanf( pHttpGet->m_szResponseHeader, ("http/%f %d"), &fVer, &nRet) != 2 )
		{
			break;
		}

		if( nRet == 302 )
		{
			char	szNewAddress[MAXPATH];
			p = strstr( pHttpGet->m_szResponseHeader, "location");
			if( p == NULL )
			{
				break;
			}

			if( sscanf( p, ("location: %s"), szNewAddress) != 1 )
			{
				break;
			}

			strcpy( pHttpGet->m_szUrl, szNewAddress);
			pHttpGet->m_bIsMoved = TRUE;
			if( !HttpGetParserUrl(pHttpGet))
				break;

			HttpGetFormatRequest( pHttpGet, *pFrom, *pTo);

			continue;
		}
		else if( nRet >= 300 )
		{
			break;
		}

		if( pHttpGet->m_pDataBuf == NULL )
		{
			bRet = TRUE;
			break;
		}

		p = strstr( pHttpGet->m_szResponseHeader, ("content-range:"));
		if( p != NULL )
		{
			if( sscanf( p, ("content-range: bytes %d-%d/%d"), pFrom, pTo, pLength) != 3 )
			{
				break;
			}
		}
		else
		{
			p = strstr( pHttpGet->m_szResponseHeader, ("content-length:"));
			if( p == NULL )
			{
				break;
			}
			if( sscanf( p, ("content-length: %d"), pLength) != 1 )
			{
				break;
			}
			*pFrom = 0;
			*pTo = *pLength - 1;
		}

		bRet = TRUE;
		break;
	}

	return bRet;
}

void HttpGetGetThread(SHttpGet* pHttpGet)
{
	if( !HttpGetPrepareThread(pHttpGet))
	{
		HttpGetUnPrepareThread(pHttpGet);
		return;
	}

	while( !pHttpGet->m_bStop && pHttpGet->m_nLoopTry < 3 )
	{
		CBOOL	bReadOK;
		DWORD	dwFrom;
		DWORD	dwTo;

		if( pHttpGet->m_pDataBuf == NULL )
		{
			dwFrom = 0;
			dwTo = 0;
		}
		else
		{
			dwFrom = pHttpGet->m_pDataBuf->m_dwFrom + pHttpGet->m_pDataBuf->m_dwDataSize;
			dwTo = pHttpGet->m_pDataBuf->m_dwFrom + pHttpGet->m_pDataBuf->m_dwLength - 1;
		}

		pHttpGet->m_nStatus = GET_STATUS_REQUESTING;

		if( !HttpGetSendRequest( pHttpGet, &dwFrom, &dwTo, &pHttpGet->m_dwFileLength))
		{
			pHttpGet->m_nLoopTry++;
			continue;
		}

		pHttpGet->m_nLoopTry = 0;

		FC_Log("HttpGetGetThread:: HttpGetSendRequest OK, from = %lu, to = %lu, length = %lu, ResponseSaveLen = %lu", dwFrom, dwTo, pHttpGet->m_dwFileLength, pHttpGet->m_dwResponseSaveLen);

		if( dwFrom == 0 && (dwTo == pHttpGet->m_dwFileLength - 1) && pHttpGet->m_dwFileLength < MAX_HTTP_HEAD_LEN && (strlen(pHttpGet->m_szObject) > 0) )
		{
			char*	szTemp;
			CBOOL	bWapForm = FALSE;
				
			pHttpGet->m_pResponseSave[pHttpGet->m_dwResponseSaveLen] = 0;
			szTemp = str_dup(pHttpGet->m_pResponseSave);
			str_replace( szTemp, "&amp;", "&");
			
			if( strstr( pHttpGet->m_pResponseSave, pHttpGet->m_szObject)) // WAP FORM
				bWapForm = TRUE;

			SAFE_FREE(szTemp);

			if( bWapForm )
			{
				pHttpGet->m_nLoopTry++;
				continue;
			}
		}

		pHttpGet->m_nLoopTry = 0;

		pHttpGet->m_nStatus = GET_STATUS_REQUESTED;

		if( pHttpGet->m_pDataBuf == NULL )
		{
			break;
		}

		if( pHttpGet->m_pDataBuf->m_dwDataSize > 0 )
		{
			if( (dwFrom != (pHttpGet->m_pDataBuf->m_dwFrom + pHttpGet->m_pDataBuf->m_dwDataSize) )
				|| (dwTo != (pHttpGet->m_pDataBuf->m_dwFrom + pHttpGet->m_pDataBuf->m_dwLength - 1)) )
			{
				pHttpGet->m_pDataBuf->m_dwDataSize = 0;
				break;
			}
		}
		else if( (dwTo - dwFrom + 1) == 0 )
		{
			if( pHttpGet->m_dwFileLength == 0x7FFFFFFF )
			{
				FC_Sleep(500);
				continue;
			}

			break;
		}
		else
		{
			pHttpGet->m_pDataBuf->m_dwFrom = dwFrom;
			pHttpGet->m_pDataBuf->m_dwLength = dwTo - dwFrom + 1;
			if( pHttpGet->m_pDataBuf->m_dwLength > pHttpGet->m_pDataBuf->m_dwBufSize )
				pHttpGet->m_pDataBuf->m_dwLength = pHttpGet->m_pDataBuf->m_dwBufSize;
		}

		pHttpGet->m_dwCurRead = pHttpGet->m_pDataBuf->m_dwDataSize;
		if( pHttpGet->m_dwResponseSaveLen > 0 )
		{
			memcpy( (char*)pHttpGet->m_pDataBuf->m_pData + pHttpGet->m_dwCurRead, pHttpGet->m_pResponseSave, pHttpGet->m_dwResponseSaveLen);
			pHttpGet->m_dwCurRead += pHttpGet->m_dwResponseSaveLen;
		}

		bReadOK = HttpGetSocketRead( pHttpGet, (char*)pHttpGet->m_pDataBuf->m_pData, pHttpGet->m_pDataBuf->m_dwLength);
		pHttpGet->m_pDataBuf->m_dwDataSize = pHttpGet->m_dwCurRead;
		if( bReadOK )
		{
			pHttpGet->m_nStatus = GET_STATUS_READED;
			break;
		}
		else
		{
			pHttpGet->m_nLoopTry++;
		}
	}

	HttpGetUnPrepareThread(pHttpGet);
}

DWORD WINAPI HttpGetThreadProc(LPVOID pParam)
{
	SHttpGet*	pHttpGet = (SHttpGet*)pParam;

	pHttpGet->m_bThreadRunning = TRUE;
	HttpGetGetThread(pHttpGet);
	pHttpGet->m_bThreadRunning = FALSE;

	return 0;
}

#else	//!MULTITHREAD
void HttpGetSetCallback(SHttpGet* pHttpGet, HANDLE hOwner, HttpSourceCallbackFunc funcHttpSourceCallback)
{
	pHttpGet->m_hOwner = hOwner;
	pHttpGet->m_funcHttpSourceCallback = funcHttpSourceCallback;
}

void HttpGetGetThreadLoop(SHttpGet* pHttpGet)
{
	FC_Log("HttpGetGetThreadLoop");
	if( pHttpGet->m_bStop || pHttpGet->m_nLoopTry > 3 )
	{
		HttpGetUnPrepareThread(pHttpGet);
		return;
	}

	if( pHttpGet->m_pDataBuf == NULL )
	{
		pHttpGet->m_dwFrom = 0;
		pHttpGet->m_dwTo = 0;
	}
	else
	{
		pHttpGet->m_dwFrom = pHttpGet->m_pDataBuf->m_dwFrom + pHttpGet->m_pDataBuf->m_dwDataSize;
		pHttpGet->m_dwTo = pHttpGet->m_pDataBuf->m_dwFrom + pHttpGet->m_pDataBuf->m_dwLength - 1;
	}

	pHttpGet->m_nStatus = GET_STATUS_REQUESTING;
	pHttpGet->m_bSendAgain = TRUE;

	if( !HttpGetParserUrl(pHttpGet))
	{
		HttpGetUnPrepareThread(pHttpGet);
		return;
	}

	HttpGetFormatRequest( pHttpGet, pHttpGet->m_dwFrom, pHttpGet->m_dwTo);

	pHttpGet->m_nTryNum = 0;

	HttpGetSendRequestLoop( pHttpGet);
	FC_Log("HttpGetGetThreadLoop return");
}

void HttpGetGetThreadBranch(SHttpGet* pHttpGet)
{
	FC_Log("HttpGetGetThreadBranch");

	if( !HttpGetPrepareThread(pHttpGet))
	{
		HttpGetUnPrepareThread(pHttpGet);
		return;
	}

	HttpGetGetThreadLoop(pHttpGet);
}

void HttpGetSendRequestLoop(SHttpGet* pHttpGet)
{
	FC_Log("HttpGetSendRequestLoop");
	if( pHttpGet->m_bStop || pHttpGet->m_nTryNum > MAX_SOCKET_TRY_NUM )
	{
		HttpGetAfterSendRequest( pHttpGet, FALSE);
		return;
	}

	HttpGetConnectServerBranch(pHttpGet);
	FC_Log("HttpGetSendRequestLoop return");
}

void HttpGetAfterSendRequest(SHttpGet* pHttpGet, CBOOL bSuccess)
{
	FC_Log("HttpGetAfterSendRequest");
	if( !bSuccess )
	{
		pHttpGet->m_nLoopTry++;
		HttpGetGetThreadLoop(pHttpGet);
		return;
	}

	pHttpGet->m_nLoopTry = 0;

	pHttpGet->m_nStatus = GET_STATUS_REQUESTED;
	
	if( pHttpGet->m_pDataBuf == NULL )
	{
		HttpGetUnPrepareThread(pHttpGet);
		return;
	}

	FC_Log("gfrom=%lu, gto=%lu, pfrom=%lu, pds=%lu plen=%lu", pHttpGet->m_dwFrom, pHttpGet->m_dwTo, pHttpGet->m_pDataBuf->m_dwFrom, pHttpGet->m_pDataBuf->m_dwDataSize, pHttpGet->m_pDataBuf->m_dwLength);
	if( pHttpGet->m_pDataBuf->m_dwDataSize > 0 )
	{
		if( (pHttpGet->m_dwFrom != (pHttpGet->m_pDataBuf->m_dwFrom + pHttpGet->m_pDataBuf->m_dwDataSize) )
			|| (pHttpGet->m_dwTo != (pHttpGet->m_pDataBuf->m_dwFrom + pHttpGet->m_pDataBuf->m_dwLength - 1)) )
		{
			pHttpGet->m_pDataBuf->m_dwDataSize = 0;
			HttpGetUnPrepareThread(pHttpGet);
			return;
		}
	}
	else if( (pHttpGet->m_dwFrom != pHttpGet->m_pDataBuf->m_dwFrom) && (pHttpGet->m_dwFrom == 0) )
	{
		pHttpGet->m_nLoopTry++;
		HttpGetGetThreadLoop(pHttpGet);
		return;
	}
	else if( (pHttpGet->m_dwTo - pHttpGet->m_dwFrom + 1) == 0 )
	{
		if( pHttpGet->m_dwFileLength == 0x7FFFFFFF )
		{
			FC_Sleep(500);
			HttpGetGetThreadLoop(pHttpGet);
			return;
		}
		HttpGetUnPrepareThread(pHttpGet);
		return;
	}
	else
	{
		pHttpGet->m_pDataBuf->m_dwFrom = pHttpGet->m_dwFrom;
		pHttpGet->m_pDataBuf->m_dwLength = pHttpGet->m_dwTo - pHttpGet->m_dwFrom + 1;
		if( pHttpGet->m_pDataBuf->m_dwLength > pHttpGet->m_pDataBuf->m_dwBufSize )
			pHttpGet->m_pDataBuf->m_dwLength = pHttpGet->m_pDataBuf->m_dwBufSize;
	}

	pHttpGet->m_dwCurRead = pHttpGet->m_pDataBuf->m_dwDataSize;
	if( pHttpGet->m_dwResponseSaveLen > 0 )
	{
		memcpy( (char*)pHttpGet->m_pDataBuf->m_pData + pHttpGet->m_dwCurRead, pHttpGet->m_pResponseSave, pHttpGet->m_dwResponseSaveLen);
		pHttpGet->m_dwCurRead += pHttpGet->m_dwResponseSaveLen;
	}

	pHttpGet->m_bReadRespone = FALSE;
	pHttpGet->m_pReadBuf = (char*)pHttpGet->m_pDataBuf->m_pData;
	pHttpGet->m_dwNeedRead = pHttpGet->m_pDataBuf->m_dwLength;
	pHttpGet->m_bReadError = FALSE;
	pHttpGet->m_nTryNum = 0;

	if( pHttpGet->m_dwNeedRead == pHttpGet->m_dwCurRead )
	{
		HttpGetSocketReadBranch(pHttpGet);
	}
	else
	{
		FC_socketRecv( pHttpGet->m_nSocketID, pHttpGet->m_pReadBuf + pHttpGet->m_dwCurRead, 
				pHttpGet->m_dwNeedRead - pHttpGet->m_dwCurRead,
				(HANDLE)pHttpGet, HttpGetAfterSocketRead);
	}

	return;
}

void HttpGetConnectServerBranch(SHttpGet* pHttpGet)
{
	FC_Log("HttpGetConnectServerBranch");
	HttpGetClose(pHttpGet);

	pHttpGet->m_nSocketID = FC_socketCreate(FC_SOCKET_TCP);
	if( pHttpGet->m_nSocketID  < 0 )
	{
		HttpGetAfterConnect((HANDLE)pHttpGet, FALSE);
		return;
	}

	FC_Log("HttpGetConnectServer:: create m_nSocketID=%d", pHttpGet->m_nSocketID);

	if( pHttpGet->m_nProxyPort )
	{
		FC_socketConnect( pHttpGet->m_nSocketID, pHttpGet->m_szProxy, pHttpGet->m_nProxyPort, (HANDLE)pHttpGet, HttpGetAfterConnect);
	}
	else
	{
		FC_socketConnect( pHttpGet->m_nSocketID, pHttpGet->m_szServerAddr, pHttpGet->m_dwServerHttpPort, (HANDLE)pHttpGet, HttpGetAfterConnect);
	}
	FC_Log("HttpGetConnectServerBranch return");
}

void HttpGetAfterConnect(HANDLE hOwner, CBOOL bSuccess)
{
	SHttpGet* pHttpGet = (SHttpGet*)hOwner;

	FC_Log("HttpGetAfterConnect");
	if( !bSuccess )
	{
		FC_Log("HttpGetAfterConnect:: connect fail");
		HttpGetAfterSendRequest(pHttpGet, FALSE);
		return;
	}

	pHttpGet->m_pWriteBuf = pHttpGet->m_szRequestHeader;
	pHttpGet->m_dwNeedWrite = strlen(pHttpGet->m_szRequestHeader);
	pHttpGet->m_dwCurWrite = 0;
	pHttpGet->m_nTryNum = 0;

	HttpGetSocketWriteBranch( pHttpGet);
}

void HttpGetSocketReadBranch( SHttpGet* pHttpGet)
{
	FC_Log("HttpGetSocketReadBranch");
	if( pHttpGet->m_bStop )
	{
		HttpGetUnPrepareThread(pHttpGet);
		return;
	}

	if( pHttpGet->m_bReadRespone )
	{
		FC_Log("HttpGetSocketReadBranch:: m_bReadRespone, pHttpGet->m_dwCurRead = %lu", pHttpGet->m_dwCurRead);
		{
			DWORD	i;
			if( pHttpGet->m_dwCurRead <= 4 )
			{
				if( pHttpGet->m_bReadError )
				{
					HttpGetAfterSocketReadResponse(pHttpGet, FALSE);
					return;
				}
			}
			else
			{
				for( i = 0 ; i <= pHttpGet->m_dwCurRead - 4 ; i++ )
				{
					if( (pHttpGet->m_pReadBuf[i] == '\r') && (pHttpGet->m_pReadBuf[i+1] == '\n') && (pHttpGet->m_pReadBuf[i+2] == '\r') && (pHttpGet->m_pReadBuf[i+3] == '\n') )
					{
						pHttpGet->m_pReadBuf[i] = 0;

						FC_Log("responelen = %d, m_pReadBuf=%lu, %s", i, (DWORD)pHttpGet->m_pReadBuf, pHttpGet->m_pReadBuf);

						if( i + 4 < pHttpGet->m_dwCurRead )
						{
							pHttpGet->m_dwResponseSaveLen = pHttpGet->m_dwCurRead - i - 4;
							memcpy( pHttpGet->m_pResponseSave, pHttpGet->m_pReadBuf+i+4, pHttpGet->m_dwResponseSaveLen);
						}

						HttpGetAfterSocketReadResponse(pHttpGet, TRUE);
						return;
					}
				}

				if( pHttpGet->m_bReadError || (pHttpGet->m_dwNeedRead >= pHttpGet->m_dwCurRead) )
				{
					HttpGetAfterSocketReadResponse(pHttpGet, FALSE);
					return;
				}
			}
		}
	}
	else
	{
		pHttpGet->m_pDataBuf->m_dwDataSize = pHttpGet->m_dwCurRead;
		FC_Log("HttpGetSocketReadBranch:: m_dwNeedRead=%d, m_dwCurRead=%d", pHttpGet->m_dwNeedRead, pHttpGet->m_dwCurRead);
		if( pHttpGet->m_dwNeedRead == pHttpGet->m_dwCurRead )
		{
			FC_Log("HttpGetSocketReadBranch:: m_dwFrom=%d, m_dwTo=%d, m_dwFileLength=%d", pHttpGet->m_dwFrom, pHttpGet->m_dwTo, pHttpGet->m_dwFileLength);
			if( pHttpGet->m_dwFrom == 0 && (pHttpGet->m_dwTo == pHttpGet->m_dwFileLength - 1) && pHttpGet->m_dwFileLength < MAX_HTTP_HEAD_LEN  && (strlen(pHttpGet->m_szObject) > 0) )
			{
				char*	szTemp;
				CBOOL	bWapForm = FALSE;
				
				pHttpGet->m_pReadBuf[pHttpGet->m_dwCurRead] = '\0';
				szTemp = str_dup(pHttpGet->m_pReadBuf);
				str_replace( szTemp, "&amp;", "&");
				
				FC_Log("HttpGetSocketReadBranch:: szTemp=%s", szTemp);
				FC_Log("HttpGetSocketReadBranch:: m_szObject=%s", pHttpGet->m_szObject);
				if( strstr( szTemp, pHttpGet->m_szObject)) // WAP FORM
					bWapForm = TRUE;

				SAFE_FREE(szTemp);
				
				if( bWapForm )
				{
					FC_Log("HttpGetSocketReadBranch:: WAP FORM %s", pHttpGet->m_pReadBuf);
					pHttpGet->m_nLoopTry++;
					pHttpGet->m_pDataBuf->m_dwFrom = 0;
					pHttpGet->m_pDataBuf->m_dwLength = pHttpGet->m_dwLenSave;
					pHttpGet->m_pDataBuf->m_dwDataSize = 0;
					HttpGetGetThreadLoop(pHttpGet);
					return;
				}
			}

			FC_Log("HttpGetSocketReadBranch:: Read OK");
			HttpGetUnPrepareThread(pHttpGet);
			return;
		}

		if( pHttpGet->m_bReadError )
		{
			FC_Log("HttpGetSocketReadBranch:: Read fail, pHttpGet->m_pDataBuf->m_dwDataSize=%lu", pHttpGet->m_pDataBuf->m_dwDataSize);
			pHttpGet->m_nLoopTry++;
			if( pHttpGet->m_pDataBuf->m_dwDataSize > 0 )
				pHttpGet->m_pDataBuf->m_dwDataSize--;

			HttpGetGetThreadLoop(pHttpGet);
			return;
		}

		pHttpGet->m_funcHttpSourceCallback( pHttpGet->m_hOwner);
	}

	FC_Log("HttpGetSocketReadBranch:: NeedRead = %d, CurRead = %d", pHttpGet->m_dwNeedRead, pHttpGet->m_dwCurRead);
	FC_socketRecv( pHttpGet->m_nSocketID, pHttpGet->m_pReadBuf + pHttpGet->m_dwCurRead, 
				pHttpGet->m_dwNeedRead - pHttpGet->m_dwCurRead,
				(HANDLE)pHttpGet, HttpGetAfterSocketRead);
}

void HttpGetAfterSocketRead(HANDLE hOwner, int nStatus, int nLen)
{
	SHttpGet* pHttpGet = (SHttpGet*)hOwner;
	FC_Log("HttpGetAfterSocketRead");

	if( pHttpGet->m_nTryNum > MAX_SOCKET_TRY_NUM )
	{
		FC_Log("HttpGetAfterSocketRead:: MAX_SOCKET_TRY_NUM");
		pHttpGet->m_bReadError = TRUE;
		HttpGetSocketReadBranch(pHttpGet);
		return;
	}

	if( nStatus == FC_SOCKET_TIMEOUT )
	{
		FC_Log("HttpGetAfterSocketRead:: FC_SOCKET_TIMEOUT");

		pHttpGet->m_nTryNum++;
		if( nLen > 0 )
			pHttpGet->m_dwCurRead += (DWORD)nLen;

		HttpGetSocketReadBranch(pHttpGet);
	}
	else if( nStatus == FC_SOCKET_SUCCESS )
	{
		FC_Log("HttpGetAfterSocketRead:: FC_SOCKET_SUCCESS");

		if( nLen > 0 )
		{
			pHttpGet->m_nTryNum = 0;
			pHttpGet->m_dwCurRead += (DWORD)nLen;
		}
		else
		{
			pHttpGet->m_nTryNum++;
		}

		HttpGetSocketReadBranch(pHttpGet);
	}
	else if( nStatus == FC_SOCKET_ERROR )
	{
		FC_Log("HttpGetAfterSocketRead:: FC_SOCKET_ERROR");

		pHttpGet->m_bReadError = TRUE;
		HttpGetSocketReadBranch(pHttpGet);
	}
}

void HttpGetSocketWriteBranch( SHttpGet* pHttpGet)
{
	FC_Log("HttpGetSocketWriteBranch");
	if( pHttpGet->m_bStop || pHttpGet->m_nTryNum > MAX_SOCKET_TRY_NUM )
	{
		HttpGetUnPrepareThread(pHttpGet);
		return;
	}

	if( pHttpGet->m_dwNeedWrite == pHttpGet->m_dwCurWrite )
	{
		HttpGetSocketReadResponseBranch(pHttpGet, pHttpGet->m_szResponseHeader, MAX_HTTP_HEAD_LEN);
		return;
	}

	FC_socketSend( pHttpGet->m_nSocketID, pHttpGet->m_pWriteBuf + pHttpGet->m_dwCurWrite, 
				pHttpGet->m_dwNeedWrite - pHttpGet->m_dwCurWrite,
				(HANDLE)pHttpGet, HttpGetAfterSocketWrite);
}

void HttpGetAfterSocketWrite(HANDLE hOwner, int nStatus, int nLen)
{
	SHttpGet* pHttpGet = (SHttpGet*)hOwner;

	FC_Log("HttpGetAfterSocketWrite");
	if( nStatus == FC_SOCKET_TIMEOUT )
	{
		pHttpGet->m_nTryNum++;
		if( nLen > 0 )
			pHttpGet->m_dwCurWrite += (DWORD)nLen;

		HttpGetSocketWriteBranch(pHttpGet);
	}
	else if( nStatus == FC_SOCKET_SUCCESS )
	{
		if( nLen > 0 )
		{
			pHttpGet->m_nTryNum = 0;
			pHttpGet->m_dwCurWrite += (DWORD)nLen;
		}
		else
		{
			pHttpGet->m_nTryNum++;
		}

		HttpGetSocketWriteBranch(pHttpGet);
	}
	else if( nStatus == FC_SOCKET_ERROR )
	{
		HttpGetUnPrepareThread(pHttpGet);
	}
}

void HttpGetSocketReadResponseBranch( SHttpGet* pHttpGet, char* pBuf, DWORD dwLen)
{
	FC_Log("HttpGetSocketReadResponseBranch");
	memset( pBuf, 0, dwLen);
	pHttpGet->m_dwResponseSaveLen = 0;
	pHttpGet->m_dwCurRead = 0;

	pHttpGet->m_bReadError = FALSE;
	pHttpGet->m_bReadRespone = TRUE;
	pHttpGet->m_pReadBuf = pBuf;
	pHttpGet->m_dwFrom = dwLen;
	pHttpGet->m_nTryNum = 0;
	pHttpGet->m_dwNeedRead = dwLen;

	FC_socketRecv( pHttpGet->m_nSocketID, pHttpGet->m_pReadBuf, 
				pHttpGet->m_dwNeedRead,
				(HANDLE)pHttpGet, HttpGetAfterSocketRead);
	
}

void HttpGetAfterSocketReadResponse(SHttpGet* pHttpGet, CBOOL bSuccess)
{
	float	fVer;
	int		nRet;
	char*	p;

	FC_Log("HttpGetAfterSocketReadResponse:: bSuccess=%d", bSuccess);
	if( !bSuccess )
	{
		pHttpGet->m_nTryNum++;
		HttpGetSendRequestLoop(pHttpGet);
		return;
	}

	FC_Log(pHttpGet->m_szResponseHeader);
	str_tolower(pHttpGet->m_szResponseHeader);

	if( sscanf( pHttpGet->m_szResponseHeader, ("http/%f %d"), &fVer, &nRet) != 2 )
	{
		HttpGetAfterSendRequest( pHttpGet, FALSE);
		return;
	}

	if( nRet == 302 )
	{
		char	szNewAddress[MAXPATH];
		p = strstr( pHttpGet->m_szResponseHeader, "location");
		if( p == NULL )
		{
			HttpGetAfterSendRequest( pHttpGet, FALSE);
			return;
		}

		if( sscanf( p, ("location: %s"), szNewAddress) != 1 )
		{
			HttpGetAfterSendRequest( pHttpGet, FALSE);
			return;
		}

		strcpy( pHttpGet->m_szUrl, szNewAddress);
		pHttpGet->m_bIsMoved = TRUE;

		if( !HttpGetParserUrl(pHttpGet))
		{
			HttpGetAfterSendRequest( pHttpGet, FALSE);
			return;
		}

		HttpGetFormatRequest( pHttpGet, pHttpGet->m_dwFrom, pHttpGet->m_dwTo);

		HttpGetSendRequestLoop(pHttpGet);
		return;
	}
	else if( nRet >= 300 )
	{
		HttpGetAfterSendRequest( pHttpGet, FALSE);
		return;
	}

	if( pHttpGet->m_pDataBuf == NULL )
	{
		HttpGetAfterSendRequest( pHttpGet, TRUE);
		return;
	}

	p = strstr( pHttpGet->m_szResponseHeader, ("content-range:"));
	if( p != NULL )
	{
		if( sscanf( p, ("content-range: bytes %d-%d/%d"), &pHttpGet->m_dwFrom, &pHttpGet->m_dwTo, &pHttpGet->m_dwFileLength) != 3 )
		{
			HttpGetAfterSendRequest( pHttpGet, FALSE);
			return;
		}
		FC_Log("HttpGetAfterSocketReadResponse:: m_dwFrom=%d, m_dwTo=%d, m_dwFileLength=%d", pHttpGet->m_dwFrom, pHttpGet->m_dwTo, pHttpGet->m_dwFileLength);
	}
	else
	{
		p = strstr( pHttpGet->m_szResponseHeader, ("content-length:"));
		if( p == NULL )
		{
			HttpGetAfterSendRequest( pHttpGet, FALSE);
			return;
		}
		if( sscanf( p, ("content-length: %d"), &pHttpGet->m_dwFileLength) != 1 )
		{
			HttpGetAfterSendRequest( pHttpGet, FALSE);
			return;
		}
		//if( (nRet == 200) && (pHttpGet->m_dwResponseSaveLen < pHttpGet->m_dwFileLength) && (pHttpGet->m_dwFileLength < MAX_HTTP_HEAD_LEN - strlen(pHttpGet->m_szResponseHeader) - 4) )
		//{
		//	HttpGetAfterSendRequest( pHttpGet, FALSE);
		//	return;
		//}

		pHttpGet->m_dwFrom = 0;
		pHttpGet->m_dwTo = pHttpGet->m_dwFileLength - 1;

		FC_Log("HttpGetAfterSocketReadResponse::m_dwFileLength=%lu, m_dwResponseSaveLen=%lu", pHttpGet->m_dwFileLength, pHttpGet->m_dwResponseSaveLen);
		{
			/*
			HANDLE	hFile = FC_open( "e:\\wap_log.txt", "a");
			char*	pStart;
			char*	pEnd;
			char*	pWapAdd;
			if( hFile )
			{
				FC_write( hFile, pHttpGet->m_pResponseSave, pHttpGet->m_dwResponseSaveLen);
				FC_close(hFile);
			}
			
			pStart = strstr( pHttpGet->m_pResponseSave, pHttpGet->m_szUrl);
			if( pStart )
			{
				pEnd = strstr( pStart, "\"");
				pWapAdd = strstr( pStart, "t=");
				if( pEnd && (pWapAdd < pEnd) )
				{
					*pEnd = '\0';

					FC_Log("HttpGetAfterSocketReadResponse::pStart=%s", pStart);
					strcpy( pHttpGet->m_szUrl, pStart);

					if( !HttpGetParserUrl(pHttpGet))
					{
						HttpGetAfterSendRequest( pHttpGet, FALSE);
						return;
					}

					HttpGetFormatRequest( pHttpGet, pHttpGet->m_dwFrom, pHttpGet->m_dwTo);

					HttpGetSendRequestLoop(pHttpGet);
					return;
				}
			}
			*/
		}
	}

	HttpGetAfterSendRequest( pHttpGet, TRUE);
	return;
}

#endif // MULTITHREAD

CBOOL	HttpGetStart(SHttpGet* pHttpGet, SDataBuf* pDataBuf)
{
	FC_Log("HttpGetStart");
	HttpGetStop(pHttpGet);

	pHttpGet->m_bStop = FALSE;
	pHttpGet->m_pDataBuf = pDataBuf;

	pHttpGet->m_dwFileLength = 0;
	pHttpGet->m_dwCurRead = 0;

	pHttpGet->m_bThreadRunning = TRUE;

	if( pDataBuf )
		pHttpGet->m_dwLenSave = pDataBuf->m_dwLength;

#ifdef MULTITHREAD
	pHttpGet->m_hThread = FC_CreateThread( HttpGetThreadProc, pHttpGet, &pHttpGet->m_dwThreadID);

	if( pHttpGet->m_hThread == NULL )
	{
		pHttpGet->m_bThreadRunning = FALSE;
		return FALSE;
	}
#else
	HttpGetGetThreadBranch(pHttpGet);
#endif

	FC_Log("HttpGetStart return TRUE");
	return TRUE;
}

void	HttpGetStop(SHttpGet* pHttpGet)
{
	if( pHttpGet == NULL )
		return;

	pHttpGet->m_dwCurRead = 0;
	pHttpGet->m_nStatus = GET_STATUS_STOP;
	pHttpGet->m_bStop = TRUE;

	if( pHttpGet->m_hThread )
	{
		FC_WaitThreadEnd(pHttpGet->m_hThread);
		FC_CloseThread(pHttpGet->m_hThread);
		pHttpGet->m_hThread = NULL;
	}
	pHttpGet->m_bThreadRunning = FALSE;
}

CBOOL	HttpGetIsAlive(SHttpGet* pHttpGet)
{
	if( pHttpGet == NULL )
		return FALSE;

	return pHttpGet->m_bThreadRunning;
}

CBOOL	HttpGetIsMoved(SHttpGet* pHttpGet)
{
	if( pHttpGet == NULL )
		return FALSE;

	return pHttpGet->m_bIsMoved;
}

DWORD	HttpGetGetFileLength(SHttpGet* pHttpGet)
{
	if( pHttpGet == NULL )
		return 0;

	return pHttpGet->m_dwFileLength;
}

char*	HttpGetGetMovedUrl(SHttpGet* pHttpGet)
{
	if( pHttpGet == NULL )
		return NULL;

	return pHttpGet->m_szUrl;
}

DWORD	HttpGetGetCurRead(SHttpGet* pHttpGet)
{
	if( pHttpGet == NULL )
		return 0;

	return pHttpGet->m_dwCurRead;
}

DWORD	HttpGetGetStatus(SHttpGet* pHttpGet)
{
	if( pHttpGet == NULL )
		return GET_STATUS_STOP;

	return pHttpGet->m_nStatus;
}

#endif

void ChartRGetAfterConnect(HANDLE hOwner, CBOOL bSuccess);
void ChartRGetAfterSocketWrite(HANDLE hOwner, int nStatus, int nLen);
void ChartRGetAfterSocketRead(HANDLE hOwner, int nStatus, int nLen);
void ChartRGetAfterCancel(HANDLE hOwner, CBOOL bSuccess);

CChartRGetter* CChartRGetter::NewL(char *_szProxy,int _szProxyPort,char * _szUsername,char* _szPassword)
{
	CChartRGetter* self = new (ELeave)CChartRGetter();
	CleanupStack::PushL(self);
	self->ConstructL(_szProxy,_szProxyPort,_szUsername,_szPassword);
	CleanupStack::Pop();

	return self;
}
CChartRGetter::~CChartRGetter() 
{
    	if( iTimer && iTimer->IsActive() )
		iTimer->Cancel();
	delete iTimer;

	Close();
	FC_gprsReleaseConnect((HANDLE)iGprsData);

	if(_username)
		FC_free(_username);

	if(password)
		FC_free(password);

	//delete iGprsData;
}
CChartRGetter::CChartRGetter() 
{
}

void CChartRGetter::ConstructL(char *_szProxy,int _szProxyPort,char * _szUsername,char* _szPassword) 
{
  	strcpy(m_szProxy,_szProxy);
	m_nProxyPort=_szProxyPort;
	iCenterCore= CenterCore::GetInstance();
	iTimer = CFunTimeOutTimer::NewL( 0, *this );
	
	m_RecvBuf=(char*)FC_malloc(MAX_BUFFER_LEN*MAX_SAVE_BLOCK);
	memset(m_RecvBuf,0,MAX_BUFFER_LEN*MAX_SAVE_BLOCK);
	
	m_dwCurRead=(char*)FC_malloc(MAX_BUFFER_LEN);
	memset(m_dwCurRead,0,MAX_BUFFER_LEN);

	m_sendbuf=(char*)FC_malloc(MAX_BUFFER_LEN);
	memset(m_sendbuf,0,MAX_BUFFER_LEN);

	_username=(char*)FC_malloc(32);
	memset(_username,0,32);
	strcpy(_username,_szUsername);
	password=(char*)FC_malloc(32);
	memset(password,0,32);
	strcpy(password,_szPassword);

}

TInt CChartRGetter::Connect() 
{
	if(!FC_gprsIsConnect(iGprsData, 0))
	{
		BOOL bWap=iCenterCore->CheckHttpWapOrNet(iCenterCore->_http_iap_id) ;

		iGprsData=(GprsConnData*)FC_gprsConnect(bWap);

		FC_socketCreate((int)iGprsData);
		FC_socketConnect((int)iGprsData, m_szProxy, m_nProxyPort,this,  ChartRGetAfterConnect);
		FC_socketSetCancelCallBack((int)iGprsData,ChartRGetAfterCancel);
		return 1;
	}
	else	
		return 0;
}


void CChartRGetter::MftonTimerExpired() 
{
	if(iGprsData)
	{
		AppendToRecvbuf();
		ReceivedThreadMgr();
		Receive();
		iTimer->After( 5000000 );
	}
	
}

void CChartRGetter::StartTimer() 
{	
    	if( iTimer && iTimer->IsActive() )
		iTimer->Cancel();
	iTimer->After( 5000000 );
}

void CChartRGetter::Close() 
{
	if(iGprsData)
	{
		FC_gprsReleaseConnect(iGprsData);	// do not disconnect gprs
		m_nTryNum =0;
	}
	iGprsData=NULL;
	
	if(m_sendbuf)
		FC_free(m_sendbuf);
	m_sendbuf=NULL;

	if(m_dwCurRead)
		FC_free(m_dwCurRead);
	m_dwCurRead=NULL;

	if(m_RecvBuf)
		FC_free(m_RecvBuf);
	m_RecvBuf=NULL;
	
	
}

void CChartRGetter::Receive()
{
	if(iGprsData)
	{
		if(!strlen(m_dwCurRead))
		{
			FC_socketRecv((int)iGprsData, m_dwCurRead, 0, this, ChartRGetAfterSocketRead);
			m_latest=do_Receive;
			FC_Log("CChartRGetter::Receive,m_nTryNum=%d",m_nTryNum);	
		}
		else
		{
			FC_Log("CChartRGetter::Receive,m_dwCurRead have data");	
		}
	}	
}

void CChartRGetter::Send( char* pBuf, int nBytes)
{
	if(iGprsData)
	{
		FC_socketSend((int)iGprsData,pBuf,nBytes,this,ChartRGetAfterSocketWrite);
		FC_Log("CChartRGetter::Send ()m_nTryNum=,%d",m_nTryNum);	
	}	
}


void CChartRGetter::SendLogin()
{
	if(iGprsData)
	{
		TInt len=0;
		if(m_sendbuf)
			memset(m_sendbuf,0,MAX_BUFFER_LEN);
		
		strcpy(m_sendbuf,"<event type='10' anonymous='1' username='demo'/> ");
		len=strlen(m_sendbuf);
		
		FC_Log("CChartRGetter::SendLogin,%d,sendbuf=%s",len,m_sendbuf);	
		Send(m_sendbuf,len);
		m_latest=do_Longin;
	}	
}

void CChartRGetter::SendMessage( char * number,char* ptext)
{
	if(iGprsData)
	{
		TInt len=0;
		if(m_sendbuf)
			memset(m_sendbuf,0,MAX_BUFFER_LEN);
		//strcpy(m_sendbuf,"<event type='10' anonymous='1'>");
		sprintf(m_sendbuf,"<event type='30' tofd='%s'><body>%s</body></event>",number,ptext);
		len=strlen(m_sendbuf);

		FC_Log("CChartRGetter::SendMessage ()m_sendbuf=%s",m_sendbuf);	
		Send(m_sendbuf,len);
		m_latest=do_Send;
	}	
}

void CChartRGetter::SendMessage(const TDesC& _name,const TDesC& _msg)
{
	if(iGprsData)
	{
		TInt len=0;
		if(m_sendbuf)
			memset(m_sendbuf,0,MAX_BUFFER_LEN);

		char *name=(char *)FC_malloc(_name.Length()*3);
		char *msg=(char *)FC_malloc(_msg.Length()*3);
		memset(name,0,_name.Length()*3);
		memset(msg,0,_msg.Length()*3);
		FC_TBuf16ToCharp(name, _name);
		FC_TBuf16ToCharp(msg, _msg);
		
		sprintf(m_sendbuf,"<event type='30' tofd='%s'><body>%s</body></event>",name,msg);
		len=strlen(m_sendbuf);
		FC_free(name);
		FC_free(msg);
		
		FC_Log("CChartRGetter::SendMessage ()m_sendbuf=%s",m_sendbuf);	
		Send(m_sendbuf,len);
		m_latest=do_Send;
	}	
}

void CChartRGetter::SendMessageAgain()
{
	if(m_sendbuf)
	{
		TInt len=0;
		len=strlen(m_sendbuf);

		FC_Log("CChartRGetter::SendPrevMessage ()m_sendbuf=%s",m_sendbuf);	
		Send(m_sendbuf,len);
		m_latest=do_Send;
	}	
}
/*
void CChartRGetter::SendPing()
{
	if(iGprsData)
	{
		TInt len=0;
		if(m_sendbuf)
			memset(m_sendbuf,0,MAX_BUFFER_LEN);
		
		strcpy(m_sendbuf,"<event type='0'/>");
		len=strlen(m_sendbuf);
		
		FC_Log("CChartRGetter::SendLogin,%d,sendbuf=%s",len,m_sendbuf);	
		Send(m_sendbuf,len);
	}	
}
*/
void CChartRGetter::ReceivedThreadMgr()
{
	TInt		evnType=-1;
	if(!iGprsData)
		return ;
	char * msg=(char*)FC_malloc(MAX_BUFFER_LEN);
	memset(msg,0,MAX_BUFFER_LEN);

	if(!GetMsgFormRecvbuf(msg))
	{
		FC_free(msg);
		return;
	}
	FC_Log("CChartRGetter::ReceivedThreadMgr message=%s",msg);	
	
	evnType=GetEventTpye(msg);
	switch(evnType)
	{
		case EV_TYPE_FORE_GETWAY_ERR:
			{
			}
			break;
		case EV_TYPE_FORE_GETWAY_OK:
			{
				SendLogin();	

			}
			break;
		case EV_TYPE_ERR :
			{
			}
			break;
		case EV_TYPE_PING :
			{
			}
			break;
		case EV_TYPE_AUTH ://用户登入返回信息
			{
				TBuf<256> ret;
				_LIT (success, "1");
				CenterCore::get_attribute(msg,"result", ret);
				if(ret==success)
				{
					iCenterCore->m_appUI->TitleBar()->ResetTitleTabMenu();
					iCenterCore->m_appUI->ActivateLocalViewL(KViewChatRoom);
				}
				
			}
			break;
		case EV_TYPE_AUTH_OTHER_LOGIN : //是同一个用户在其他地方登陆
			{
			}
			break;
		case EV_TYPE_USER_LOGIN :
			{
			}
			break;
		case EV_TYPE_USER_LOGOUT :
			{
			}
			break;
		case EV_TYPE_USER_ADD :  //已经在线的人 名单
			{
				TBuf<256> name;
				CenterCore::get_attribute(msg,"username", name);
				
				iCenterCore->ChatR_BodyListAppend(name);
				if(iCenterCore->m_appUI->viewChatRoom->iContainer)
					iCenterCore->m_appUI->viewChatRoom->iContainer->UpdateListItemsL();
				
			}
			break; 
		case EV_TYPE_MESSAGE :
			{
				TBuf<32> name;
				TBuf<256> mess;
				TBuf<32> toN;
				TInt itime;
				CenterCore::get_attribute(msg,"from", name);
				CenterCore::get_attribute(msg,"to", toN);
				CenterCore::get_attribute(msg,"timestamp", &itime);
				CenterCore::get_body(msg,"body", mess);

				if(toN.Length())  //prev 
				{
					iCenterCore->ChatR_PrevLogListAppend(name, mess,itime);
					if(iCenterCore->m_appUI->viewChatRoom->iContainer)
						iCenterCore->m_appUI->viewChatRoom->iContainer->PushChatCommL(0);

				}
				else //comm chat
				{
					iCenterCore->ChatR_CommLogListAppend(name, mess,itime);
					if(iCenterCore->m_appUI->viewChatRoom->iContainer)
						iCenterCore->m_appUI->viewChatRoom->iContainer->PushChatCommL(1);
				}

			}
			break;
		default:
			break;
	}
	FC_free(msg);
}

TInt CChartRGetter::GetEventTpye(char * _szMessage)
{
	if(!_szMessage)
		return EV_TYPE_ERR;

	float	fVer;
	int		nRet;
	TInt		evnType=-1;
	char *p =NULL;	
	
	if( sscanf( _szMessage, ("http/%f %d"), &fVer, &nRet)== 2 )
	{
		FC_Log("GetEventTpye:nRet=%d",nRet);
		if(nRet<300 && 199<nRet) //  200~299 success
		{
			
			return EV_TYPE_FORE_GETWAY_OK;
		}
		else
		{
			return EV_TYPE_FORE_GETWAY_ERR;
		}
	}

	if( sscanf( _szMessage, ("<event type='%d'"), &evnType)== 1 )
		return evnType;
	else
		return EV_TYPE_ERR;

}


void CChartRGetter::AppendToRecvbuf()
{
	TInt allLen=0,curlen=0;
	if(m_isLocked ||!strlen(m_dwCurRead) )
		return ;

	m_isLocked=1;
	allLen=strlen(m_RecvBuf);
	curlen=strlen(m_dwCurRead);

	if((allLen+curlen)<MAX_BUFFER_LEN*MAX_SAVE_BLOCK)
		strcpy(m_RecvBuf,m_dwCurRead);

	memset(m_dwCurRead,0,MAX_BUFFER_LEN);
	m_isLocked=0;
}

TInt CChartRGetter::GetMsgFormRecvbuf(char * content)
{
	TInt curlen=0, allLen=0;
	char *start=NULL,*end=NULL;
	TInt movebit=2;
	if(m_isLocked ||!strlen(m_RecvBuf))
		return 0;

	if(!content)
		return 0;
	
	m_isLocked=1;

	allLen=strlen(m_RecvBuf);
	start = strstr(m_RecvBuf, "<event type=");
	end = strstr(m_RecvBuf, "/>");
	if(!end)
	{
		end = strstr(m_RecvBuf, "</event>");
		movebit=8;
	}
	if(!start||!end)//not complete message 
	{
		start = strstr(m_RecvBuf, "http/"); // ||from getway
		if(start)
		{
			strcpy(content,start);
			memset(m_RecvBuf,0,MAX_BUFFER_LEN*MAX_SAVE_BLOCK);
			m_isLocked=0;
			return 1;
		}
		m_isLocked=0;
		return 0;			
	}
	
	end+=movebit;
	curlen=end-start;
	strncpy(content,start,curlen);
	//end++;
	
	char *tempbuf=(char*)FC_malloc(MAX_BUFFER_LEN*MAX_SAVE_BLOCK);
	memset(tempbuf,0,MAX_BUFFER_LEN*MAX_SAVE_BLOCK);
	strcpy(tempbuf,end);
	FC_free(m_RecvBuf);
	m_RecvBuf=tempbuf;
	
	m_isLocked=0;
	
	return curlen;
}


void ChartRGetAfterConnect(HANDLE hOwner, CBOOL bSuccess)
{
	CChartRGetter* pHttpGet = (CChartRGetter*)hOwner;

	FC_Log("ChartRGetAfterConnect :%d--iGprsData=%d,m_nTryNum=%d,",bSuccess,pHttpGet->iGprsData,pHttpGet->m_nTryNum);
	if( !bSuccess )
	{
		FC_Log("ChartRGetAfterConnect:: connect fail");
		
		if(pHttpGet->m_nTryNum<MAX_SOCKET_TRY_NUM)
		{
			pHttpGet->Connect();
			pHttpGet->m_nTryNum ++;
		}
		else
		{
			pHttpGet->Close();

		}
		//HttpGetAfterSendRequest(pHttpGet, FALSE);
		return;
	}

//	pHttpGet->m_pWriteBuf = pHttpGet->m_szRequestHeader;
//	pHttpGet->m_dwNeedWrite = strlen(pHttpGet->m_szRequestHeader);
//	pHttpGet->m_dwCurWrite = 0;
	pHttpGet->m_nTryNum = 0;
	pHttpGet->SendLogin();
	pHttpGet->StartTimer();

}

void ChartRGetAfterSocketWrite(HANDLE hOwner, int nStatus, int nLen)
{
	CChartRGetter* pHttpGet = (CChartRGetter*)hOwner;
	TInt len;
	FC_Log("ChartRGetAfterSocketWrite,nStatus=%d",nStatus);

	if( pHttpGet->m_nTryNum > MAX_SOCKET_TRY_NUM )
	{
		FC_Log("ChartRGetAfterSocketWrite:: MAX_SOCKET_TRY_NUM");
		//pHttpGet->m_bReadError = TRUE;
		pHttpGet->Close();
		return;
	}	

	if( nStatus == FC_SOCKET_TIMEOUT )
	{
		pHttpGet->m_nTryNum++;
		len=strlen(pHttpGet->m_sendbuf);
		
		FC_Log("CChartRGetter::ChartRGetAfterSocketWrite,FC_SOCKET_TIMEOUT");	
		if(len)
			pHttpGet->Send(pHttpGet->m_sendbuf,len);
	}
	else if( nStatus == FC_SOCKET_SUCCESS )
	{
		if( nLen > 0 )
		{
			pHttpGet->m_nTryNum = 0;
		//	pHttpGet->m_dwCurWrite += (DWORD)nLen;
		//	FC_free(pHttpGet->m_sendbuf);
			//pHttpGet->m_sendbuf=NULL;
			pHttpGet->Receive();
		}
		else
		{
			pHttpGet->m_nTryNum++;
			len=strlen(pHttpGet->m_sendbuf);	
			if(len)
				pHttpGet->Send(pHttpGet->m_sendbuf,len);

		}
	}
	else if( nStatus == FC_SOCKET_ERROR )
	{
		pHttpGet->Close();
	}
}

void ChartRGetAfterSocketRead(HANDLE hOwner, int nStatus, int nLen)
{
	CChartRGetter* pHttpGet = (CChartRGetter*)hOwner;

	FC_Log("ChartRGetAfterSocketRead",nStatus);

	if( pHttpGet->m_nTryNum > MAX_SOCKET_TRY_NUM+100 )
	{
		FC_Log("ChartRGetAfterSocketRead:: MAX_SOCKET_TRY_NUM");
		//pHttpGet->m_bReadError = TRUE;
		pHttpGet->Close();
		return;
	}

	if( nStatus == FC_SOCKET_TIMEOUT )
	{
		FC_Log("ChartRGetAfterSocketRead:: FC_SOCKET_TIMEOUT");

		pHttpGet->m_nTryNum++;
		//if( nLen > 0 )
		//	pHttpGet->m_dwCurRead += (DWORD)nLen;

	//	HttpGetSocketReadBranch(pHttpGet);
	}
	else if( nStatus == FC_SOCKET_SUCCESS )
	{
		FC_Log("ChartRGetAfterSocketRead:: FC_SOCKET_SUCCESS");

		if( nLen > 0 )
		{
			pHttpGet->m_nTryNum = 0;
			pHttpGet->AppendToRecvbuf();
		}
		else
		{
			pHttpGet->m_nTryNum++;
			pHttpGet->Receive();
		}

		//HttpGetSocketReadBranch(pHttpGet);
	}
	else if( nStatus == FC_SOCKET_ERROR )
	{
		FC_Log("ChartRGetAfterSocketRead:: FC_SOCKET_ERROR");

		pHttpGet->Close();
	//	HttpGetSocketReadBranch(pHttpGet);
	}
}

void ChartRGetAfterCancel(HANDLE hOwner, CBOOL bSuccess)
{
	CChartRGetter* pHttpGet = (CChartRGetter*)hOwner;

	FC_Log("ChartRGetAfterCancel :%d-",bSuccess);
	if( !bSuccess )
	{
		FC_Log("ChartRGetAfterCancel:: Cancel fail");
		
		return;
	}

	switch(pHttpGet->m_latest)
	{
		case CChartRGetter::do_Nothing:
		{

		}
		break;
		case CChartRGetter::do_Longin:
		{
			pHttpGet->SendLogin();
		}
		break;
		case CChartRGetter::do_Send:
		{
			pHttpGet->SendMessageAgain();
		}
		break;
		case CChartRGetter::do_Receive:
		{
			pHttpGet->Receive();
		}
		break;
		default:
			break;

	}

}
